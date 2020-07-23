/*! \file l3_egress.c
 *
 * XGS L3 egress objects management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/l3.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_egress_int.h>
#include <bcm_int/ltsw/xgs/l3_egress.h>
#include <bcm_int/ltsw/failover.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/ecn.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/virtual.h>

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

/* Scale of MPLS label. */
#define L3_MPLS_LBL_MAX      (0xfffff)
#define L3_MPLS_LBL_VALID(_label_)                      \
    (_label_ <= L3_MPLS_LBL_MAX)

/* Data structure to save the info of L3_UC_NHOPt entry */
typedef struct nh_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT];
} nh_cfg_t;

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

typedef struct ecmp_member_info_s {
    bool ul_ecmp;
    uint64_t nhop_id;
    uint64_t ul_nhop_ecmp_id;
    uint64_t weight;
    uint64_t port_id;
} ecmp_member_info_t;

/* Data structure to save the info of ECMP */
typedef struct ecmp_cfg_s {
    /* Weighted or not. */
    bool     weighted;

    /* resilient HASH or not. */
    bool     res_hash;

    /* Index. */
    uint64_t ecmp_id;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
    uint64_t lb_mode;
    uint64_t sorted;
    uint64_t max_paths;
    uint64_t num_paths;
    uint64_t weighted_mode;
    uint64_t weight;
    uint64_t weighted_size;
    uint64_t nhop_id;
    uint64_t rh_nhop_id;
    uint64_t rh_size;
    uint64_t rh_num_paths;
    uint64_t rh_random_seed;
    uint64_t ctr_egr_eflex_object;
    uint64_t ctr_egr_eflex_action_id;

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
    ecmp_member_info_t *member_info;
} ecmp_cfg_t;

typedef struct nh_entry_info_s {
    /* NHOP type. */
    int nhop_type;
#define L3_INVALID_NHOP_TYPE 0
#define L3_UC_NHOP_TYPE 1
#define L3_MC_NHOP_TYPE 2
#define L3_MPLS_TRANSIT_NHOP_TYPE 3
#define L3_MPLS_ENCAP_NHOP_TYPE 4
#define L3_IFP_ACTION_NHOP_TYPE 5

} nh_entry_info_t;

typedef struct l3_egr_info_s {
    /* L3 egress manager initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Minimum nexthop index. */
    int nh_min;

    /* Maximum nexthop index. */
    int nh_max;

    /* Bitmap of free nh index . */
    SHR_BITDCL *nh_fbmp;

    /* Minimum Underlay ECMP index. */
    int ulecmp_grp_min;

    /* Maximum Underlay ECMP index. */
    int ulecmp_grp_max;

    /* Bitmap of free Underlay ECMP group index . */
    SHR_BITDCL *ulecmp_grp_fbmp;

    /* Bitmap of free Underlay Weighted ECMP group index . */
    SHR_BITDCL *ulecmp_grp_weighted_fbmp;

    /* Underlay ECMP flags. */
    uint32_t *ulecmp_grp_flags;

    /* Underlay ECMP group lb mode. */
    uint32_t *ulecmp_grp_lb_mode;

    /* Underlay ECMP group max paths. */
    uint32_t *ulecmp_grp_max_paths;

    /* Minimum Overlay ECMP index. */
    int olecmp_grp_min;

    /* Maximum Overlay ECMP index. */
    int olecmp_grp_max;

    /* Overlay ECMP group count. */
    int olecmp_grp_cnt;

    /* Bitmap of free Overlay ECMP group index . */
    SHR_BITDCL *olecmp_grp_fbmp;

    /* Bitmap of free Overlay Weighted ECMP group index . */
    SHR_BITDCL *olecmp_grp_weighted_fbmp;

    /* Overlay ECMP flags. */
    uint32_t *olecmp_grp_flags;

    /* Overlay ECMP group lb mode. */
    uint32_t *olecmp_grp_lb_mode;
#define ECMP_LB_MODE_REGULAR 1
#define ECMP_LB_MODE_RANDOM 2
#define ECMP_LB_MODE_RESILIENT 3
#define ECMP_LB_MODE_REPLICATION 4
#define ECMP_LB_MODE_WEIGHTED 5
#define ECMP_LB_MODE_DLB 6
#define ECMP_LB_MODE_DGM 7

    /* Overlay ECMP group max paths. */
    uint32_t *olecmp_grp_max_paths;

    /* Maximum ECMP paths. */
    int ecmp_path_max;

    /* Maximum Overlay ECMP paths. */
    int ol_ecmp_path_max;

    /* Maximum Resilient Hash ECMP paths. */
    int ecmp_rh_path_max;

    /* Maximum weighted ECMP paths. */
    int wgt_ecmp_path_max;

    /* Maximum overlay weighted ECMP paths. */
    int wgt_ol_ecmp_path_max;

    /* Maximum number of entries for each hash output selection instance. */
    int ecmp_hash_flow_sel_ent_num;

    /* Maximum number of entries for each hash output selection instance. */
    int ecmp_hash_port_sel_ent_num;

    /* Offset in ECMP group for DLB. */
    int ecmp_dlb_grp_offset;

    /* ECMP DLB group start index. */
    int ecmp_dlb_grp_min;

    /* Array of nexthop entry info. */
    nh_entry_info_t *nh_ei;

    /* Array of ECMP group entry info. */
    int nhidx_width;

    /* Overlay nexthop entries. */
    uint32_t olnh_ent;

    /* Overlay ECMP member entries. */
    uint32_t olecmp_member_ent;
} l3_egr_info_t;

static l3_egr_info_t l3_egr_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_EGR_INITED(_u) (l3_egr_info[_u].inited)

#define L3_EGR_LOCK(_u)     \
    sal_mutex_take(l3_egr_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_EGR_UNLOCK(_u)   \
    sal_mutex_give(l3_egr_info[_u].mutex)

#define L3_OL_NHOP_ENT(_u)   (l3_egr_info[_u].olnh_ent)

#define L3_NHOP_MIN(_u)   (l3_egr_info[_u].nh_min)

#define L3_NHOP_MAX(_u)   (l3_egr_info[_u].nh_max)

#define L3_UL_NHOP_ENT(_u)   (L3_NHOP_MAX(_u) - L3_OL_NHOP_ENT(_u) + 1)
#define L3_UL_NHOP_CHECK(_u, _idx) (_idx < L3_UL_NHOP_ENT(_u) ? true : false)

#define L3_NHOP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].nh_fbmp, (_id))

#define L3_NHOP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].nh_fbmp, (_id))

#define L3_NHOP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].nh_fbmp, (_id))

#define L3_NHOP_INFO(_u) (l3_egr_info[_u].nh_ei)

#define L3_OL_ECMP_MEMBER_ENT(_u)   (l3_egr_info[_u].olecmp_member_ent)

#define L3_UL_ECMP_GRP_MIN(_u)   (l3_egr_info[_u].ulecmp_grp_min)

#define L3_UL_ECMP_GRP_MAX(_u)   (l3_egr_info[_u].ulecmp_grp_max)

#define L3_UL_ECMP_GRP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].ulecmp_grp_fbmp, (_id))

#define L3_UL_ECMP_GRP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].ulecmp_grp_fbmp, (_id))

#define L3_UL_ECMP_GRP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].ulecmp_grp_fbmp, (_id))

#define L3_UL_ECMP_GRP_WGT_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].ulecmp_grp_weighted_fbmp, (_id))

#define L3_UL_ECMP_GRP_WGT_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].ulecmp_grp_weighted_fbmp, (_id))

#define L3_UL_ECMP_GRP_WGT_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].ulecmp_grp_weighted_fbmp, (_id))

#define L3_OL_ECMP_GRP_MIN(_u)   (l3_egr_info[_u].olecmp_grp_min)

#define L3_OL_ECMP_GRP_MAX(_u)   (l3_egr_info[_u].olecmp_grp_max)

#define L3_OL_ECMP_GRP_CNT(_u)   (l3_egr_info[_u].olecmp_grp_cnt)

#define L3_OL_ECMP_GRP_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].olecmp_grp_fbmp, (_id))

#define L3_OL_ECMP_GRP_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].olecmp_grp_fbmp, (_id))

#define L3_OL_ECMP_GRP_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].olecmp_grp_fbmp, (_id))

#define L3_OL_ECMP_GRP_WGT_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_egr_info[_u].olecmp_grp_weighted_fbmp, (_id))

#define L3_OL_ECMP_GRP_WGT_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_egr_info[_u].olecmp_grp_weighted_fbmp, (_id))

#define L3_OL_ECMP_GRP_WGT_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_egr_info[_u].olecmp_grp_weighted_fbmp, (_id))


#define L3_UL_ECMP_GRP_FLAGS(_u, _id) (l3_egr_info[_u].ulecmp_grp_flags[_id])
#define L3_UL_ECMP_GRP_LB_MODE(_u, _id) (l3_egr_info[_u].ulecmp_grp_lb_mode[_id])
#define L3_OL_ECMP_GRP_FLAGS(_u, _id) (l3_egr_info[_u].olecmp_grp_flags[_id])
#define L3_OL_ECMP_GRP_LB_MODE(_u, _id) (l3_egr_info[_u].olecmp_grp_lb_mode[_id])

#define L3_UL_ECMP_GRP_MAX_PATHS(_u, _id) (l3_egr_info[_u].ulecmp_grp_max_paths[_id])
#define L3_OL_ECMP_GRP_MAX_PATHS(_u, _id) (l3_egr_info[_u].olecmp_grp_max_paths[_id])

#define L3_ECMP_MAX_PATHS(_u) (l3_egr_info[_u].ecmp_path_max)
#define L3_OL_ECMP_MAX_PATHS(_u) (l3_egr_info[_u].ol_ecmp_path_max)
#define L3_ECMP_RH_MAX_PATHS(_u) (l3_egr_info[_u].ecmp_rh_path_max)

#define L3_ECMP_WEIGHTED_MAX_PATHS(_u) (l3_egr_info[_u].wgt_ecmp_path_max)
#define L3_OL_ECMP_WEIGHTED_MAX_PATHS(_u) (l3_egr_info[_u].wgt_ol_ecmp_path_max)

#define L3_ECMP_FLOW_HASH_SEL_ENT_NUM(_u) (l3_egr_info[_u].ecmp_hash_flow_sel_ent_num)
#define L3_ECMP_PORT_HASH_SEL_ENT_NUM(_u) (l3_egr_info[_u].ecmp_hash_port_sel_ent_num)

#define L3_ECMP_DLB_GRP_MIN(_u) (l3_egr_info[_u].ecmp_dlb_grp_min)
#define L3_ECMP_DLB_GRP_OFFSET(_u) (l3_egr_info[_u].ecmp_dlb_grp_offset)

#define L3_ECMP_DLB_GRP_CONFIGURED(_u, _id)                             \
    (((_id) >= L3_ECMP_DLB_GRP_OFFSET(_u)) &&                            \
     bcmi_ltsw_dlb_id_configured(_u, bcmi_dlb_type_ecmp,                \
                                 ((_id) - L3_ECMP_DLB_GRP_OFFSET(_u))))
#define L3_NHOP_IDX_MASK(_u) ((1 << l3_egr_info[_u].nhidx_width) - 1)

#define L3_NHOP_TYPE(_u, _idx) (l3_egr_info[_u].nh_ei[_idx].nhop_type)

/* ECMP Hash subset select. */
typedef enum ecmp_hash_sub_sel_e {
    /*
     * If CONCAT = 1 select hash bins b1, b0, a1, a0.
     * If CONCAT = 0 select hash bin a0.
     */
    BCMINT_LTSW_ECMP_SUB_SEL_USE_0_0 = 0,

    /*
     * If CONCAT = 1 select hash bins c1, c0, a1, a0.
     * If CONCAT = 0 select hash bin a1.
     */
    BCMINT_LTSW_ECMP_SUB_SEL_USE_1_0 = 1,

    /*
     * If CONCAT = 1 select hash bins 32b'0, c1, c0.
     * If CONCAT = 0 select hash bin b0.
     */
    BCMINT_LTSW_ECMP_SUB_SEL_USE_PORT_LBN = 2,

    /*
     * If CONCAT = 1 select MH.DEST_PORT/b1,b0,a1,a0.
     * If CONCAT = 0 select hash bin MH.DEST_PORT/a0.
     */
    BCMINT_LTSW_ECMP_SUB_SEL_USE_DEST_PORT = 3,

    /*
     * If CONCAT = 1 Reserved.
     * If CONCAT = 0 select hash bin a1.
     */
    BCMINT_LTSW_ECMP_SUB_SEL_USE_0_1 = 4,

    /*
     * If CONCAT = 1 Reserved.
     * If CONCAT = 0 select hash bin b1.
     */
    BCMINT_LTSW_ECMP_SUB_SEL_USE_1_1 = 5,

    BCMINT_LTSW_ECMP_SUB_SEL_CNT
} ecmp_hash_sub_sel_t;

/* L3 egress flex counter action type. */
typedef enum l3_egress_flexctr_action_e {
    /* L3MC . */
    BCMINT_LTSW_L3MC_NHOP_FLEXCTR_ACTION = 0,

    /* L3. */
    BCMINT_LTSW_L3UC_NHOP_FLEXCTR_ACTION = 1,

    /* MPLS. */
    BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION = 2,

    /* Cnt. */
    BCMINT_LTSW_L3_EGRESS_FLEXCTR_ACTION_CNT = 3,

} l3_egress_flexctr_action_t;

typedef struct l3_egress_flex_action_profile_s {
    /*! Action. */
    uint32_t action;
} l3_egress_flex_action_profile_t;

/* _min should be alway 32 bit align . */
#define SHR_BIT_ITER_RANGE(_a, _min, _max, _b)               \
    for ((_b) = _min; (_b) < (_max); (_b)++)        \
        if ((_a)[(_b) / SHR_BITWID] == 0)        \
            (_b) += (SHR_BITWID - 1);            \
        else if (SHR_BITGET((_a), (_b)))

static const uint64_t wgt_sz_val[] = {
    128,
    256,
    512,
    1024,
    2048,
    4096
};

static const uint64_t res_sz_val[] = {
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096,
    8192,
    16384,
    32768
};

static int
nh_get(int unit, int nhidx, bcm_l3_egress_t *egr);

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get egress object ID from LT index.
 *
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

    nh_ecmp_idx = ltidx;

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_l3_egress_obj_id_construct(unit, nh_ecmp_idx, type, intf));

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

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_l3_egress_obj_id_resolve(unit, intf, &nh_ecmp_idx, type));

    *ltidx = nh_ecmp_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill entry of NHOP.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti Logical table info.
 * \param [in] cfg Data of L3 NHOP.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_nh_fill(
    int unit,
    bcmlt_entry_handle_t eh,
    const bcmint_ltsw_l3_egress_tbl_t *ti,
    nh_cfg_t *cfg)
{
    const bcmint_ltsw_l3_egress_fld_t *fi = ti->flds;
    uint64_t *fv;
    const char *sym_val;
    uint32_t i, cnt;
    uint64_t fld_bmp;

    SHR_FUNC_ENTER(unit);

    cnt = BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT;
    fv = cfg->flds;
    fld_bmp = cfg->fld_bmp;

    for (i = 0; i < cnt; i++) {
        /* Skip fields that are invalid for the given LT. */
        if (!(ti->fld_bmp & (1LL << i))) {
            continue;
        }

        /*
         * Skip fields that are not intended to be operated.
         * Key fields are always required.
         */
        if (!(fld_bmp & (1LL << i)) && !fi[i].key) {
            continue;
        }

        if (fi[i].symbol) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fi[i].scalar_to_symbol(unit, fv[i], &sym_val));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh,
                                              fi[i].fld_name,
                                              sym_val));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, fi[i].fld_name, fv[i]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of L3 NHOP.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti Logical table info.
 * \param [out] cfg Data of L3 NHOP.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

static int
lt_nh_parse(
    int unit,
    bcmlt_entry_handle_t eh,
    const bcmint_ltsw_l3_egress_tbl_t *ti,
    nh_cfg_t *cfg)
{
    const bcmint_ltsw_l3_egress_fld_t *fi = ti->flds;
    uint64_t *fv;
    uint32_t i, cnt;
    const char *sym_val;
    uint64_t fld_bmp;
    int rv;

    SHR_FUNC_ENTER(unit);

    cnt = BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT;
    fv = cfg->flds;
    fld_bmp = cfg->fld_bmp;

    for (i = 0; i < cnt; i++) {
        /* Skip fields that are invalid for the given LT. */
        if (!(ti->fld_bmp & (1LL << i))) {
            continue;
        }

        /*
         * Skip fields that are not intended to be operated.
         * Key fields are always required.
         */
        if (!(fld_bmp & (1LL << i)) && !fi[i].key) {
            continue;
        }

        if (fi[i].symbol) {
            rv = bcmlt_entry_field_symbol_get(eh, fi[i].fld_name,
                                              &sym_val);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (fi[i].symbol_to_scalar(unit, sym_val, &fv[i]));
        } else {
            rv = bcmlt_entry_field_get(eh, fi[i].fld_name, &fv[i]);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Fill entry handle of ECMPt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti Logical table info.
 * \param [in] cfg Data of ECMPt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ecmp_level_fill(
    int unit,
    bcmlt_entry_handle_t eh,
    const bcmint_ltsw_l3_egress_tbl_t *ti,
    ecmp_cfg_t *cfg)
{
    uint64_t *data = NULL;
    uint32_t i, sz, max_paths, start_idx, num;
    const bcmint_ltsw_l3_egress_fld_t *fi = ti->flds;
    const char *str = NULL;
    const char *sym_val;
    int fld_id;

    SHR_FUNC_ENTER(unit);

    max_paths = cfg->weighted ? L3_ECMP_WEIGHTED_MAX_PATHS(unit) :
                                (cfg->res_hash ? L3_ECMP_RH_MAX_PATHS(unit) :
                                                 L3_ECMP_MAX_PATHS(unit));

    if ((cfg->start_idx + cfg->num_of_unset + cfg->num_of_set) > max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_ID;
    str = fi[fld_id].fld_name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, str, cfg->ecmp_id));

    fld_id = BCMINT_XGS_L3_ECMP_TBL_LB_MODE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].scalar_to_symbol(unit, cfg->lb_mode, &sym_val));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, str, sym_val));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, str, cfg->max_paths));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].scalar_to_symbol(unit, cfg->weighted_size, &sym_val));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, str, sym_val));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, str, cfg->num_paths));
    }

    if (cfg->num_of_set) {
        sz = sizeof(uint64_t) * cfg->num_of_set;
        SHR_ALLOC(data, sz, "bcmLtswXgsEcmpMemberData");
        SHR_NULL_CHECK(data, SHR_E_MEMORY);

        start_idx = cfg->start_idx;
        num = cfg->num_of_set;

        fld_id = BCMINT_XGS_L3_ECMP_TBL_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            for (i = 0; i < cfg->num_of_set; i++) {
                data[i] = cfg->member_info[i].ul_nhop_ecmp_id;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(eh, str,
                                             start_idx, data, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_WEIGHT;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            for (i = 0; i < cfg->num_of_set; i++) {
                data[i] = cfg->member_info[i].weight ? cfg->member_info[i].weight : 1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(eh, str,
                                             start_idx, data, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            for (i = 0; i < cfg->num_of_set; i++) {
                data[i] = cfg->member_info[i].ul_nhop_ecmp_id;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(eh, str,
                                             start_idx, data, num));
        }

        if (cfg->member_info) {
            fld_id = BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].nhop_id;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].ul_ecmp ?
                              cfg->member_info[i].ul_nhop_ecmp_id : 0;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].ul_ecmp ?
                              0 : cfg->member_info[i].ul_nhop_ecmp_id;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].nhop_id;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].ul_ecmp ?
                              cfg->member_info[i].ul_nhop_ecmp_id : 0;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].ul_ecmp ?
                              0 : cfg->member_info[i].ul_nhop_ecmp_id;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].ul_ecmp;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }

            fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY;
            if (cfg->fld_bmp & (1LL << fld_id)) {
                str = fi[fld_id].fld_name;
                for (i = 0; i < cfg->num_of_set; i++) {
                    data[i] = cfg->member_info[i].ul_ecmp;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(eh, str,
                                                 start_idx, data, num));
            }
        }
    }

    if (cfg->num_of_unset) {
        start_idx = cfg->start_idx + cfg->num_of_set;
        num = cfg->num_of_unset;

        fld_id = BCMINT_XGS_L3_ECMP_TBL_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }

        fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY;
        if (cfg->fld_bmp & (1LL << fld_id)) {
            str = fi[fld_id].fld_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_unset(eh, str,
                                               start_idx, num));
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, str, cfg->sorted));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].scalar_to_symbol(unit, cfg->weighted_mode, &sym_val));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, str, sym_val));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_SIZE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].scalar_to_symbol(unit, cfg->rh_size, &sym_val));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, str, sym_val));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, str, cfg->rh_num_paths));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, str, cfg->rh_random_seed));
    }

exit:
    SHR_FREE(data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry handle of ECMPt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti Logical table info.
 * \param [out] cfg Data of ECMPt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ecmp_level_parse(
    int unit,
    bcmlt_entry_handle_t eh,
    const bcmint_ltsw_l3_egress_tbl_t *ti,
    ecmp_cfg_t *cfg)
{
    uint64_t *data = NULL;
    int i, sz;
    uint32_t act_size = 0;
    const char *lb_mode, *wgt_sz, *wgt_mode, *res_sz;
    const bcmint_ltsw_l3_egress_fld_t *fi = ti->flds;
    const char *str = NULL;
    int fld_id;

    SHR_FUNC_ENTER(unit);

    fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_ID;
    str = fi[fld_id].fld_name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, str, &cfg->ecmp_id));

    fld_id = BCMINT_XGS_L3_ECMP_TBL_LB_MODE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, str, &lb_mode));
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].symbol_to_scalar(unit, lb_mode, &cfg->lb_mode));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, str, &cfg->sorted));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, str, &cfg->max_paths));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, str, &cfg->num_paths));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, str, &wgt_mode));
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].symbol_to_scalar(unit, wgt_mode, &cfg->weighted_mode));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, str, &wgt_sz));
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].symbol_to_scalar(unit, wgt_sz, &cfg->weighted_size));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_SIZE;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, str, &res_sz));
        SHR_IF_ERR_VERBOSE_EXIT
            (fi[fld_id].symbol_to_scalar(unit, res_sz, &cfg->rh_size));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, str, &cfg->rh_num_paths));
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, str, &cfg->rh_random_seed));
    }

    /* Exit if no member info to be fetched.*/
    if (!cfg->num_of_set) {
        SHR_EXIT();
    }

    sz = sizeof(uint64_t) * cfg->num_of_set;
    SHR_ALLOC(data, sz, "bcmLtswXgsEcmpMemberData");
    SHR_NULL_CHECK(data, SHR_E_MEMORY);

    fld_id = BCMINT_XGS_L3_ECMP_TBL_NHOP_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx,
                                         data, cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].ul_nhop_ecmp_id = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_WEIGHT;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx,
                                         data, cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].weight = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].ul_nhop_ecmp_id = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].nhop_id = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].ul_ecmp = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            if (cfg->member_info[i].ul_ecmp) {
                cfg->member_info[i].ul_nhop_ecmp_id = data[i];
            }
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            if (!cfg->member_info[i].ul_ecmp) {
                cfg->member_info[i].ul_nhop_ecmp_id = data[i];
            }
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].nhop_id = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            cfg->member_info[i].ul_ecmp = data[i];
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            if (cfg->member_info[i].ul_ecmp) {
                cfg->member_info[i].ul_nhop_ecmp_id = data[i];
            }
        }
    }

    fld_id = BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID;
    if (cfg->fld_bmp & (1LL << fld_id)) {
        str = fi[fld_id].fld_name;
        sal_memset(data, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, str, cfg->start_idx, data,
                                         cfg->num_of_set, &act_size));
        for (i = 0; i < act_size; i++) {
            if (!cfg->member_info[i].ul_ecmp) {
                cfg->member_info[i].ul_nhop_ecmp_id = data[i];
            }
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
 * \brief Compare two dlb member and returns the status.
 *
 * \param [in] a First dlb member to compare.
 * \param [in] b Second dlb member to compare.
 *
 * \retval -1 a is less than b.
 * \retval 0 a equals b.
 * \retval 1 a is greater than b.
 */
static int
ecmp_cmp_dlb_member(const void *a, const void *b)
{
    bcmi_ltsw_dlb_member_t *first, *second;

    first = (bcmi_ltsw_dlb_member_t *)a;
    second = (bcmi_ltsw_dlb_member_t *)b;

    return ecmp_cmp_int((void *)&first->nhop_id, (void *)&second->nhop_id);
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
    ecmp_member_info_t *emi_1, *emi_2;
    uint64_t first, second;
    uint64_t nhop;

    emi_1 = (ecmp_member_info_t *)a;
    emi_2 = (ecmp_member_info_t *)b;

    nhop = emi_1->ul_nhop_ecmp_id;

    first = nhop;

    nhop = emi_2->ul_nhop_ecmp_id;

    second = nhop;

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
    } else if (size == sizeof(bcmi_ltsw_dlb_member_t)) {
        sal_qsort(arr, arr_cnt, size, ecmp_cmp_dlb_member);
    } else if (size == sizeof(ecmp_member_info_t)) {
        sal_qsort(arr, arr_cnt, size, ecmp_cmp_member_info);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a l3 egress flex action profile handle.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [out] profile_hdl Profile handle.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_egress_flex_action_profile_handle_get(
    int unit,
    l3_egress_flexctr_action_t type,
    bcmi_ltsw_profile_hdl_t *profile_hdl)
{
    SHR_FUNC_ENTER(unit);
    switch (type) {
        case BCMINT_LTSW_L3MC_NHOP_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_L3MC_NHOP_FLEX_ACTION;
            break;
        case BCMINT_LTSW_L3UC_NHOP_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_L3UC_NHOP_FLEX_ACTION;
            break;
        case BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_MPLS_NHOP_FLEX_ACTION;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a l3 egress flex action profile table name.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [out] ltname Logical table name.
 * \param [out] keyname Logical table key.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_egress_flex_action_profile_table_name_get(
    int unit,
    l3_egress_flexctr_action_t type,
    const char **ltname,
    const char **keyname)
{
    SHR_FUNC_ENTER(unit);
    switch (type) {
        case BCMINT_LTSW_L3MC_NHOP_FLEXCTR_ACTION:
            *ltname = L3_MC_NHOP_CTR_EGR_EFLEX_ACTIONs;
            *keyname = L3_MC_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            break;
        case BCMINT_LTSW_L3UC_NHOP_FLEXCTR_ACTION:
            *ltname = L3_UC_NHOP_CTR_EGR_EFLEX_ACTIONs;
            *keyname = L3_UC_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            break;
        case BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION:
            *ltname = TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTIONs;
            *keyname = TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a l3 egress flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] profile_idx Profile Index.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_egress_flex_action_profile_entry_get(
    int unit,
    l3_egress_flexctr_action_t type,
    int profile_idx,
    l3_egress_flex_action_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname = NULL;
    const char *keyname = NULL;
    int dunit;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_table_name_get(unit,
                                                      type,
                                                      &ltname,
                                                      &keyname));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, keyname, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ACTIONs, &data));
    prof_entry->action = (uint32_t)data;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a l3 flex egress action profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] profile_idx Profile Index.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_egress_flex_action_profile_entry_set(
    int unit,
    l3_egress_flexctr_action_t type,
    int profile_idx,
    l3_egress_flex_action_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *ltname = NULL;
    const char *keyname = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_table_name_get(unit,
                                                      type,
                                                      &ltname,
                                                      &keyname));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, keyname, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, ACTIONs, (uint64_t)prof_entry->action));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a l3 egress flex action profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] profile_idx Profile Index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_egress_flex_action_profile_entry_del(
    int unit,
    l3_egress_flexctr_action_t type,
    int profile_idx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *ltname = NULL;
    const char *keyname = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_table_name_get(unit,
                                                      type,
                                                      &ltname,
                                                      &keyname));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, keyname, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add l3 egress flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] entry l3 flex action entry.
 * \param [out] index Base index of l3 flex action.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_flex_action_profile_add(
    int unit,
    l3_egress_flexctr_action_t type,
    l3_egress_flex_action_profile_t *entry,
    int *index)
{
    int rv;
    int profile_idx;
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_handle_get(unit, type, &profile_hdl));

    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          profile_hdl,
                                          entry, 0, 1, index);
    if (rv == SHR_E_EXISTS) {
        /* the same profile already exists */
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
       SHR_ERR_EXIT(rv);
    }

    profile_idx = *index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_entry_set(unit,
                                                 type,
                                                 profile_idx,
                                                 entry));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get entries from LT l3 egress flex action.
 *
 * This function is used to get entries from LT l3 flex action.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] type                L3 flex action type.
 * \param [in] index               Profile base index.
 * \param [out] prof_entry      Profile entries.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_egress_flex_action_profile_get(
    int unit,
    l3_egress_flexctr_action_t type,
    int index,
    l3_egress_flex_action_profile_t *prof_entry)
{
    int profile_idx;

    SHR_FUNC_ENTER(unit);
    profile_idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_entry_get(unit, type,
                                                 profile_idx,
                                                 prof_entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete entries from LT l3 egress flex action.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] index Base Index of LT l3 flex action.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_flex_action_profile_delete(
    int unit,
    l3_egress_flexctr_action_t type,
    int index)
{
    int rv;
    int profile_idx;
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    profile_idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_handle_get(unit, type, &profile_hdl));

    rv = bcmi_ltsw_profile_index_free(unit, profile_hdl,
                                      index);

    if (rv == SHR_E_BUSY) {
        /* profile entry is still inused */
       SHR_EXIT();
    }

    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    /* empty the profile entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_entry_del(unit,
                                                 type,
                                                 profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash callback function for l3 egress flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries L3  egress flex action profile entries.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [out] hash Hash value returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_egress_flex_action_profile_entry_hash_cb(
    int unit,
    void *entries,
    int entries_per_set,
    uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(hash, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      sizeof(l3_egress_flex_action_profile_t));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Comparison callback function for l3 mc nhop flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries l3 flex action entry.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [in] profile_idx Profile Index of LT entry.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_mc_nh_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_egress_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_get(unit,
                                           BCMINT_LTSW_L3MC_NHOP_FLEXCTR_ACTION,
                                           profile_idx,
                                           &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 uc nhop flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries l3 flex action entry.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [in] profile_idx Profile Index of LT entry.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_uc_nh_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_egress_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_get(unit,
                                           BCMINT_LTSW_L3UC_NHOP_FLEXCTR_ACTION,
                                           profile_idx,
                                           &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 mpls nhop flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries l3 flex action entry.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [in] profile_idx Profile Index of LT entry.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_mpls_nh_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_egress_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_get(unit,
                                           BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION,
                                           profile_idx,
                                           &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover l3 egress flex action.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_flex_action_profile_recover(int unit, l3_egress_flexctr_action_t type)
{
    uint32_t ref_cnt;
    uint32_t index;
    uint64_t idx_min, idx_max;
    int dunit;
    l3_egress_flex_action_profile_t profile_entry = {0};
    bcmi_ltsw_profile_hdl_t profile_hdl;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int rv;
    uint64_t id;
    const char *ltname = NULL;
    const char *keyname = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_handle_get(unit,
                                                  type,
                                                  &profile_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_table_name_get(unit,
                                                      type,
                                                      &ltname,
                                                      &keyname));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       ltname,
                                       keyname,
                                       &idx_min,
                                       &idx_max));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, keyname, &id));
        index = (uint32_t)id;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl,
                                             index, &ref_cnt));
        if (ref_cnt == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_egress_flex_action_profile_get(unit,
                                                   type,
                                                   index,
                                                   &profile_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_hash_update(unit,
                                               profile_hdl,
                                               &profile_entry,
                                               1, index));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                                  1, index, 1));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the information of L3 egress flex action profile.
 *
 * This function is used to clear the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_flex_action_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_MC_NHOP_CTR_EGR_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_UC_NHOP_CTR_EGR_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTIONs));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Deinit LT l3 egress flex action information.
 *
 * This function is used to deinit LT l3 flex action information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_egress_flex_action_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flex_action_profile_clear(unit));
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
    const char *ltname = NULL;
    const char *fldname = NULL;
    const char *keyname = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t opcode;
    uint64_t val;
    int nhop_type;
    l3_egress_flex_action_profile_t action_profile;
    int index;
    l3_egress_flexctr_action_t flex_ctr_type;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf_id, &ltidx, &type));

    if (L3_NHOP_FBMP_GET(unit, ltidx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    nhop_type = L3_NHOP_TYPE(unit, ltidx);

    switch (nhop_type) {
        case L3_UC_NHOP_TYPE:
            ltname = L3_UC_NHOPs;
            fldname = L3_UC_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            flex_ctr_type = BCMINT_LTSW_L3UC_NHOP_FLEXCTR_ACTION;
            break;
        case L3_MC_NHOP_TYPE:
            ltname = L3_MC_NHOPs;
            fldname = L3_MC_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            flex_ctr_type = BCMINT_LTSW_L3MC_NHOP_FLEXCTR_ACTION;
            break;
        case L3_MPLS_ENCAP_NHOP_TYPE:
            ltname = TNL_MPLS_ENCAP_NHOPs;
            fldname = TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            flex_ctr_type = BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION;
            break;
        case L3_MPLS_TRANSIT_NHOP_TYPE:
            ltname = TNL_MPLS_TRANSIT_NHOPs;
            fldname = TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION_IDs;
            flex_ctr_type = BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            break;
    }

    keyname = NHOP_IDs;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, keyname, ltidx));

    if (flexctr->op == L3_EGR_FLXCTR_OP_SET) {
        if (flexctr->flags & L3_EGR_FLXCTR_F_ACTION) {
            action_profile.action = flexctr->action;
            if (flexctr->action) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_egress_flex_action_profile_add(unit,
                                                       flex_ctr_type,
                                                       &action_profile,
                                                       &index));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, fldname, index));
            } else {
                /* Get the old action and delete it. */
                opcode = BCMLT_OPCODE_LOOKUP;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, fldname, &val));

                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_egress_flex_action_profile_delete(unit, flex_ctr_type, (int)val));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, fldname, 0));
            }
        }

        if (flexctr->flags & L3_EGR_FLXCTR_F_INDEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_EGR_EFLEX_OBJECTs, flexctr->index));
        }
        opcode = BCMLT_OPCODE_UPDATE;
    } else if (flexctr->op == L3_EGR_FLXCTR_OP_GET) {
        opcode = BCMLT_OPCODE_LOOKUP;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

    if (flexctr->op == L3_EGR_FLXCTR_OP_GET) {
        if (flexctr->flags & L3_EGR_FLXCTR_F_ACTION) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, fldname, &val));
            index = (int)val;
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_egress_flex_action_profile_get(unit, flex_ctr_type, index, &action_profile));
            flexctr->action = action_profile.action;
        }

        if (flexctr->flags & L3_EGR_FLXCTR_F_INDEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_EGR_EFLEX_OBJECTs, &val));
            flexctr->index = val;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index.
 * \param [in] ul  Underlay.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ecmp_flex_action_profile_entry_get(
    int unit,
    int profile_idx,
    bool ul,
    l3_egress_flex_action_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t data;
    const char *lt_name;
    const char *lt_key_name;
    const char *lt_act_name;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    if (ul) {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL;
    }
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID].fld_name;
    lt_act_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION].fld_name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, lt_act_name, &data));
    prof_entry->action = (uint32_t)data;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index.
 * \param [in] ul  Underlay.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ecmp_flex_action_profile_entry_set(
    int unit,
    int profile_idx,
    bool ul,
    l3_egress_flex_action_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *lt_name;
    const char *lt_key_name;
    const char *lt_act_name;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    if (ul) {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL;
    }
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID].fld_name;
    lt_act_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION].fld_name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_act_name, (uint64_t)prof_entry->action));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index.
 * \param [in] ul  Underlay.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ecmp_flex_action_profile_entry_del(
    int unit,
    int profile_idx,
    bool ul)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *lt_name;
    const char *lt_key_name;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (ul) {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL;
    }
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID].fld_name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entry ECMP_OVERLAY_CTR_ING_EFLEX_ACTION entry.
 * \param [in] ul  Underlay.
 * \param [out] index Base index of ECMP_OVERLAY_CTR_ING_EFLEX_ACTION.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_flex_action_profile_add(
    int unit,
    l3_egress_flex_action_profile_t *entry,
    bool ul,
    int *index)
{
    int rv;
    int profile_idx;
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);

    profile_hdl = ul ? BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION :
                       BCMI_LTSW_PROFILE_L3_OL_ECMP_FLEX_ACTION;

    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          profile_hdl,
                                          entry, 0, 1, index);
    if (rv == SHR_E_EXISTS) {
        /* the same profile already exists */
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
       SHR_ERR_EXIT(rv);
    }

    profile_idx = *index;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flex_action_profile_entry_set(unit, profile_idx, ul, entry));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get entries from LT ECMP_OVERLAY_CTR_ING_EFLEX_ACTION.
 *
 * This function is used to get entries from LT ECMP_OVERLAY_CTR_ING_EFLEX_ACTION.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index               Profile base index.
 * \param [in] ul                    Underlay.
 * \param [out] prof_entry      Profile entries.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
ecmp_flex_action_profile_get(
    int unit,
    int index,
    bool ul,
    l3_egress_flex_action_profile_t *prof_entry)
{
    int profile_idx;

    SHR_FUNC_ENTER(unit);
    profile_idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flex_action_profile_entry_get(unit, profile_idx,
                                            ul, prof_entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete entries from LT ECMP_OVERLAY_CTR_ING_EFLEX_ACTION.
 *
 * \param [in] unit Unit number.
 * \param [in] index Base Index of LT ECMP_OVERLAY_CTR_ING_EFLEX_ACTION.
 * \param [in] ul                    Underlay.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_flex_action_profile_delete(
    int unit,
    int index,
    bool ul)
{
    int rv;
    int profile_idx;
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    profile_idx = index;

    profile_hdl = ul ? BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION :
                       BCMI_LTSW_PROFILE_L3_OL_ECMP_FLEX_ACTION;

    rv = bcmi_ltsw_profile_index_free(unit,
                                      profile_hdl,
                                      index);

    if (rv == SHR_E_BUSY) {
        /* profile entry is still inused */
       SHR_EXIT();
    }

    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    /* empty the profile entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flex_action_profile_entry_del(unit, ul, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash callback function for ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile entries.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [out] hash Hash value returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ecmp_flex_action_profile_entry_hash_cb(
    int unit,
    void *entries,
    int entries_per_set,
    uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(hash, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      sizeof(l3_egress_flex_action_profile_t));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Comparison callback function for ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries ECMP_OVERLAY_CTR_ING_EFLEX_ACTION entry.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [in] profile_idx Profile Index of LT entry.
 * \param [in] ul Underlay.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
olecmp_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_egress_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flex_action_profile_get(unit, profile_idx, false, &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for ECMP_CTR_ING_EFLEX_ACTION profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries ECMP_OVERLAY_CTR_ING_EFLEX_ACTION entry.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [in] profile_idx Profile Index of LT entry.
 * \param [in] ul Underlay.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ulecmp_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_egress_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flex_action_profile_get(unit, profile_idx, true, &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Recover LT ECMP_OVERLAY_CTR_ING_EFLEX_ACTION.
 *
 * \param [in] unit Unit number.
 * \param [in] ul Underlay.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_flex_action_profile_recover(int unit, bool ul)
{
    uint32_t ref_cnt;
    uint32_t index;
    uint64_t idx_min, idx_max;
    int dunit;
    l3_egress_flex_action_profile_t profile_entry = {0};
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int rv;
    uint64_t id;
    const char *lt_name;
    const char *lt_key_name;
    bcmint_ltsw_l3_ecmp_flexctr_act_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (ul) {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL;
    }
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));
    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID].fld_name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name,
                                       lt_key_name,
                                       &idx_min, &idx_max));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, lt_key_name, &id));
        index = (uint32_t)id;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl,
                                             index, &ref_cnt));
        if (ref_cnt == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_flex_action_profile_get(unit, index, ul, &profile_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &profile_entry,
                                               1, index));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                                  1, index, 1));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the information of L3 flex action profile.
 *
 * This function is used to clear the information of L3 flex action profile.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_flex_action_profile_clear(int unit)
{
    const char *lt_name;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));
        lt_name = tbl_info->name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, lt_name));
        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
            tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL;
            SHR_IF_ERR_EXIT
                (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));
            lt_name = tbl_info->name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, lt_name));
        }

    }
exit:
    SHR_FUNC_EXIT();

    return 0;
}

/*
 * \brief Deinit LT ECMP_CTR_ING_EFLEX_ACTION information.
 *
 * This function is used to deinit LT ECMP_CTR_ING_EFLEX_ACTION information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
ecmp_flex_action_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flex_action_profile_clear(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate flex counter related fields of ECMP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Ecmp interface.
 * \param [in/out] flexctr Pointer to flex counter config info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Failure.
 */
static int
ecmp_flexctr_op(
    int unit,
    bcm_if_t ecmp_group_id,
    l3_egr_flexctr_cfg_t *flexctr)
{
    bcmlt_opcode_t opcode;
    l3_egress_flex_action_profile_t action_profile;
    int index;
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t eh_2 = BCMLT_INVALID_HDL;
    uint32 ecmp_group_flags = 0;
    int dunit;
    uint64_t val;
    bool ul = false;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    const char *action_name = NULL;
    const char *object_name = NULL;
    const char *key_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        ecmp_group_flags = L3_UL_ECMP_GRP_FLAGS(unit, ltidx);
        ul = true;
    }

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        ecmp_group_flags = L3_OL_ECMP_GRP_FLAGS(unit, ltidx);
    }

    if ((ecmp_group_flags &
        BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_group_flags &
        BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        tbl_id = ul ? BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL :
                      BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
        ltname = tbl_info->name;
    } else {
        tbl_id = ul ? BCMINT_LTSW_L3_ECMP_TBL_UL :
                      BCMINT_LTSW_L3_ECMP_TBL_OL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
        ltname = tbl_info->name;
    }

    action_name =
        tbl_info->flds[BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID].fld_name;
    object_name =
        tbl_info->flds[BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT].fld_name;
    key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_TBL_ECMP_ID].fld_name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key_name, ltidx));

    if (flexctr->op == L3_EGR_FLXCTR_OP_SET) {
        if (flexctr->flags & L3_EGR_FLXCTR_F_ACTION) {
            action_profile.action = flexctr->action;
            if (flexctr->action) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ecmp_flex_action_profile_add(unit,
                                                  &action_profile,
                                                  ul,
                                                  &index));


                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, action_name, index));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit, ltname, &eh_2));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh_2, key_name, ltidx));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, eh_2, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh_2, action_name, &val));
                index = (int)val;

                SHR_IF_ERR_VERBOSE_EXIT
                    (ecmp_flex_action_profile_delete(unit, index, ul));

                index = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, action_name, index));
            }
        }

        if (flexctr->flags & L3_EGR_FLXCTR_F_INDEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, object_name, flexctr->index));
        }

        opcode = BCMLT_OPCODE_UPDATE;
    } else if (flexctr->op == L3_EGR_FLXCTR_OP_GET) {
        opcode = BCMLT_OPCODE_LOOKUP;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

    if (flexctr->op == L3_EGR_FLXCTR_OP_GET) {
        if (flexctr->flags & L3_EGR_FLXCTR_F_ACTION) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, action_name, &val));
            index = (int)val;

            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_flex_action_profile_get(unit, index, ul, &action_profile));

            flexctr->action = action_profile.action;
        }

        if (flexctr->flags & L3_EGR_FLXCTR_F_INDEX) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, object_name, &val));
            flexctr->index = val;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (eh_2 != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh_2);
        eh_2 = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ingress next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] mpls_view If it is MPLS view.
 * \param [in] ipmc_view If it is IPMC view.
 * \param [in] transit_view If it is MPLS transit view.
 * \param [in] field_view If it is field view.
 * \param [in] ul If it is underlay.
 * \param [in] egr Next hop entry data.
 * \param [in] egr Next hop entry logical table data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nh_cfg_set(
    int unit, bool mpls_view, bool ipmc_view, bool transit_view,
    bool field_view, bool ul, nh_cfg_t *nh,
    bcm_l3_egress_t *egr)
{
    uint64_t value;
    uint32_t mpls_flags = egr->mpls_flags;
    int l3_eif;
    bcmi_ltsw_l3_egr_intf_type_t l3_intf_type;
    bcmi_ltsw_qos_map_type_t qos_type;
    int ecn_map_type;
    int prof_id;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    fv = nh->flds;

    if (!ipmc_view && !field_view) {
        fv[BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID] = egr->vlan;
        nh->fld_bmp |= mpls_view ? 0 : (1LL << BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID);

        value = egr->flags & BCM_L3_COPY_TO_CPU;
        fv[BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU] = value ? 1 : 0;
        nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU);

        value = egr->flags & BCM_L3_DST_DISCARD;
        fv[BCMINT_XGS_L3_EGRESS_TBL_DROP] = value ? 1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP;
        if (egr->flags & BCM_L3_TGID) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID] = egr->trunk;
            fv[BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK] = 1;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK);
        } else {
            if (ul) {
                fv[BCMINT_XGS_L3_EGRESS_TBL_MODPORT] = (uint32)egr->port;
                fv[BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK] = 0;
                nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_MODPORT) |
                               (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK);
            } else {
                /* overlay nhop only used in Vxlan case currently. */
                uint32 vp = 0;
                bcm_if_t ul_egr_obj;
                int ul_nhop_idx;
                bcmi_ltsw_l3_egr_obj_type_t type;
                vp = BCM_GPORT_FLOW_PORT_ID_GET(egr->port);
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_virtual_port_egress_obj_get(unit, egr->port,
                                                           &ul_egr_obj));
                SHR_IF_ERR_VERBOSE_EXIT
                    (xgs_ltsw_l3_egress_obj_id_resolve(unit, ul_egr_obj,
                                                       &ul_nhop_idx, &type));
                if (type != BCMI_LTSW_L3_EGR_OBJ_T_UL) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                fv[BCMINT_XGS_L3_EGRESS_TBL_DVP] = vp;
                nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP;
                fv[BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID] = ul_nhop_idx;
                fv[BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID] = 1;
                nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID) |
                               (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID);
            }
        }
    } else if (ipmc_view) {
        fv[BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC] =
            (egr->flags & BCM_L3_KEEP_DSTMAC) ? 0 : 1;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC;

        fv[BCMINT_XGS_L3_EGRESS_TBL_DROP_L3] =
            (egr->multicast_flags & BCM_L3_MULTICAST_L3_DROP) ? 1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP_L3;
    }

    if (egr->intf == BCM_IF_INVALID) {
        if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        } else {
            fv[BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = 0;
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egr_intf_to_index(unit, egr->intf, &l3_eif, &l3_intf_type));

        fv[BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID] = l3_eif;
        fv[BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = 1;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID;
        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
             nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID;
        }
    }

    if (!mpls_view) {
        bcmi_ltsw_util_mac_to_uint64(&(fv[BCMINT_XGS_L3_EGRESS_TBL_DST_MAC]),
                                     egr->mac_addr);
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC;

        value = (egr->flags & BCM_L3_KEEP_SRCMAC) ? 1 : 0;
        fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC] = value ? 1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC;

        value = (egr->flags & BCM_L3_KEEP_DSTMAC) ? 1 : 0;
        fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC] = value ? 1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC;

        value = (egr->flags & BCM_L3_KEEP_TTL) ? 1 : 0;
        fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL] = value ? 1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL;

        value = (egr->flags & BCM_L3_KEEP_VLAN) ? 1 : 0;
        fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID] = value ? 1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID;

        fv[BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID] = egr->intf_class;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID;
    } else {
        if (((mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
            (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) &&
            ((mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
            (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
            (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK &&
            mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (transit_view) {
            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRESERVE) {
                fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] =
                    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_PRESERVE;
            } else if (L3_MPLS_LBL_VALID(egr->mpls_label)) {
                fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] =
                    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_SWAP;
            } else {
                fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] =
                    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_NONE;
            }
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION;
        } else {
            fv[BCMINT_XGS_L3_EGRESS_TBL_VPN_LABEL] = 1;
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_VPN_LABEL;
        }

        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET ||
            mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE] =
                BCMINT_LTSW_TNL_MPLS_EXP_MODE_FIXED;
            fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP] = egr->mpls_exp;
            fv[BCMINT_XGS_L3_EGRESS_TBL_PRI] = egr->mpls_pkt_pri;
            fv[BCMINT_XGS_L3_EGRESS_TBL_CFI] = egr->mpls_pkt_cfi;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_PRI) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_CFI);
        } else if (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE] =
                BCMINT_LTSW_TNL_MPLS_EXP_MODE_MAP;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_map_id_resolve(unit, egr->mpls_qos_map_id,
                                              &qos_type, &prof_id));
            if (bcmiQosMapTypeMplsEgress != qos_type) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fv[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID] = prof_id;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID);
        } else if ((fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] !=
                    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_SWAP) &&
                   (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)){
            fv[BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE] =
                BCMINT_LTSW_TNL_MPLS_EXP_MODE_USE_INNER_LABEL;
            if (egr->mpls_qos_map_id != 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_qos_map_id_resolve(unit, egr->mpls_qos_map_id,
                                                  &qos_type, &prof_id));
                if (bcmiQosMapTypeMplsEgress != qos_type) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            } else {
                prof_id = 0;
            }
            fv[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID] = prof_id;
            fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP] = egr->mpls_exp;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP);
        } else if ((fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] ==
                    BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_SWAP) &&
                   (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE] =
                BCMINT_LTSW_TNL_MPLS_EXP_MODE_USE_SWAP_LABEL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_map_id_resolve(unit, egr->mpls_qos_map_id,
                                              &qos_type, &prof_id));
            if ((bcmiQosMapTypeMplsEgress != qos_type) &&
                (egr->mpls_qos_map_id != 0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fv[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID] = prof_id;
            fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP] = egr->mpls_exp;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP);
        }

        bcmi_ltsw_util_mac_to_uint64(&value, egr->mac_addr);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_tnl_dst_mac_profile_add(unit,
                                                    value,
                                                    (int *)&fv[BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID]));
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID;

        if (egr->mpls_label != BCM_MPLS_LABEL_INVALID) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL] = egr->mpls_label;
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL;
        }

        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL] = egr->mpls_ttl;
        } else {
            fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL] = 0;
        }
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL;

        fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP] = egr->mpls_exp;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP;

        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_ecn_map_id_resolve(unit, egr->mpls_ecn_map_id, &ecn_map_type, (uint32_t *)&prof_id));
            if (ecn_map_type != BCMI_LTSW_ECN_MPLS_ECN_MAP_TYPE_INTCN2EXP) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID] = prof_id;
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID;

            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY] = 1;
            } else {
                fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY] = 0;
            }
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY;
        }

        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_ecn_map_id_resolve(unit, egr->mpls_ecn_map_id, &ecn_map_type, (uint32_t *)&prof_id));
            if (ecn_map_type != BCMI_LTSW_ECN_MPLS_ECN_MAP_TYPE_ECN2EXP) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID] = prof_id;
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID;

            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY] = 1;
            } else {
                fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY] = 0;
            }
            nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY;
        }
    }

    if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        SHR_EXIT();
    }

    if (!ipmc_view) {
        fv[BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] = (egr->flags3 & BCM_L3_FLAGS3_DO_NOT_CUT_THROUGH) ?
                                                          1 : 0;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH;
    }

    /* uderlay multicast nhop. */
    if (ipmc_view && ul) {
        uint32 vp = 0;
        uint32 min = 0;
        uint32 max = 0;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vp_range_get(unit, &min, &max));
        if ((egr->encap_id >= min) && (egr->encap_id <= max)) {
           vp = egr->encap_id;
        }
        fv[BCMINT_XGS_L3_EGRESS_TBL_DVP] = vp;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP;
        if (egr->intf != BCM_IF_INVALID) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID] = egr->intf;
            fv[BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID] = 1;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID);
        }
    }

    /* Overla multicast nhop. */
    if (ipmc_view && !ul) {
        /* overlay nhop only used in Vxlan case currently. */
        uint32 vp = 0;
        bcm_if_t ul_egr_obj;
        int ul_nhop_idx;
        bcmi_ltsw_l3_egr_obj_type_t type;
        bcm_l3_egress_t egr_local;

        vp = BCM_GPORT_FLOW_PORT_ID_GET(egr->port);
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_port_egress_obj_get(unit, egr->port,
                                                   &ul_egr_obj));
        SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_l3_egress_obj_id_resolve(unit, ul_egr_obj,
                                           &ul_nhop_idx, &type));
        if (type != BCMI_LTSW_L3_EGR_OBJ_T_UL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        fv[BCMINT_XGS_L3_EGRESS_TBL_DVP] = vp;
        nh->fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP;
        fv[BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID] = ul_nhop_idx;
        fv[BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID] = 1;
        nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID);

        if (egr->intf != BCM_IF_INVALID) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID] = egr->intf;
            fv[BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID] = 1;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID);
        }

        sal_memset(&egr_local, 0, sizeof(egr_local));
        SHR_IF_ERR_VERBOSE_EXIT
            (nh_get(unit, ul_nhop_idx, &egr_local));

        if (egr_local.intf != BCM_IF_INVALID) {
            fv[BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID] = egr_local.intf;
            fv[BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID] = 1;
            nh->fld_bmp |= (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID) |
                           (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress next hop entry.
 *
 * \param [in] unit Unit number.
 * \param [in] mpls_view If it is MPLS view.
 * \param [in] ipmc_view If it is IPMC view.
 * \param [in] transit_view If it is MPLS transit view.
 * \param [in] field_view If it is field view.
 * \param [in] ul If it is underlay.
 * \param [in] egr Next hop entry data.
 * \param [in] egr Next hop entry logical table data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nh_cfg_get(int unit, bool mpls_view, bool ipmc_view, bool transit_view,
           bool field_view, bool ul, nh_cfg_t *nh, bcm_l3_egress_t *egr)
{
    uint64_t value;
    int rv;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    fv = nh->flds;

    if (!ipmc_view && !field_view) {
        egr->vlan = (bcm_vlan_t)fv[BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID];

        value = fv[BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU];
        egr->flags |= value ? BCM_L3_COPY_TO_CPU : 0;

        value = fv[BCMINT_XGS_L3_EGRESS_TBL_DROP];
        egr->flags |= value ? BCM_L3_DST_DISCARD : 0;

        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY) &&
            fv[BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID]) {
            value = fv[BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID];
            if (value) {
                value = fv[BCMINT_XGS_L3_EGRESS_TBL_DVP];
                BCM_GPORT_FLOW_PORT_ID_SET(egr->port, (uint32)value);
            }
        } else {
            value = fv[BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK];
            if (value) {
                egr->flags |= BCM_L3_TGID;
                egr->trunk = (bcm_trunk_t)fv[BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID];
            } else {
                egr->port = (bcm_port_t)fv[BCMINT_XGS_L3_EGRESS_TBL_MODPORT];
            }
        }
    } else if (ipmc_view) {
        egr->flags |= BCM_L3_IPMC;
        egr->flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC] ?
                      0 : BCM_L3_KEEP_DSTMAC;
        egr->multicast_flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_DROP_L3] ?
                                BCM_L3_MULTICAST_L3_DROP : 0;
    }

    if (field_view) {
        egr->flags2 |= BCM_L3_FLAGS2_FIELD_ONLY;
    }

    if (!ipmc_view) {
        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY) &&
            !fv[BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID]) {
            egr->intf = BCM_IF_INVALID;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egr_index_to_intf(unit,
                                                (int)fv[BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID],
                                                BCMI_LTSW_L3_EGR_INTF_T_UL,
                                                &egr->intf));
        }
    }

    if (egr->intf == bcmi_ltsw_l3_intf_l2tocpu_idx_get(unit)) {
        egr->flags |= BCM_L3_L2TOCPU;
    }


    if (transit_view) {
        if (fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] ==
            BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_PRESERVE) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRESERVE;
        }
    }

    if (!mpls_view) {
        bcmi_ltsw_util_uint64_to_mac(egr->mac_addr,
                                     &fv[BCMINT_XGS_L3_EGRESS_TBL_DST_MAC]);
        egr->flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC] ?
                      BCM_L3_KEEP_SRCMAC : 0;

        egr->flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC] ?
                      BCM_L3_KEEP_DSTMAC : 0;
        egr->flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL] ?
                      BCM_L3_KEEP_TTL : 0;
        egr->flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID] ?
                      BCM_L3_KEEP_VLAN : 0;
        egr->intf_class = (int)fv[BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID];
    } else {
        if (!transit_view) {
            egr->flags |= BCM_L3_ROUTE_LABEL;
        }

        rv = bcmi_ltsw_mpls_tnl_dst_mac_profile_get(unit,
                                                    (int)fv[BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID],
                                                    &value);
        if (SHR_SUCCESS(rv)) {
            bcmi_ltsw_util_uint64_to_mac(egr->mac_addr, &value);
        }

        egr->mpls_label = (bcm_mpls_label_t)fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL];
        egr->mpls_ttl = (int)fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL];
        egr->mpls_exp = (uint8)fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP];

        if (egr->mpls_ttl) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }

        value = fv[BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE];
        if (value == BCMINT_LTSW_TNL_MPLS_EXP_MODE_FIXED) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
            egr->mpls_exp = (uint8)fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP];
            egr->mpls_pkt_pri = (uint8)fv[BCMINT_XGS_L3_EGRESS_TBL_PRI];
            egr->mpls_pkt_cfi = (uint8)fv[BCMINT_XGS_L3_EGRESS_TBL_CFI];
        } else if (value == BCMINT_LTSW_TNL_MPLS_EXP_MODE_MAP) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_map_id_construct(unit,
                                                (int)fv[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID],
                                                bcmiQosMapTypeMplsEgress,
                                                &egr->mpls_qos_map_id));
        } else if (value == BCMINT_LTSW_TNL_MPLS_EXP_MODE_USE_INNER_LABEL){
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_map_id_construct(unit,
                                                (int)fv[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID],
                                                bcmiQosMapTypeMplsEgress,
                                                &egr->mpls_qos_map_id));
            egr->mpls_exp = (uint8)fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP];
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        } else if (value == BCMINT_LTSW_TNL_MPLS_EXP_MODE_USE_SWAP_LABEL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_map_id_construct(unit,
                                                (int)fv[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID],
                                                bcmiQosMapTypeMplsEgress,
                                                &egr->mpls_qos_map_id));
            egr->mpls_exp = (uint8)fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP];
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        }

        value = fv[BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION];
        if ((value == BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_PRESERVE) ||
            (value == BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_SWAP)) {
            rv = bcmi_ltsw_ecn_map_id_construct(unit,
                                                BCMI_LTSW_ECN_MPLS_ECN_MAP_TYPE_INTCN2EXP,
                                                (int)fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID],
                                                &egr->mpls_ecn_map_id);
            if (SHR_SUCCESS(rv)) {
                egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
                egr->mpls_flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY] ?
                                   BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST : 0;
            }
        }

        if ((value != BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_PRESERVE) &&
            (value != BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_SWAP) &&
            (value != BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_NONE)) {
            rv = bcmi_ltsw_ecn_map_id_construct(unit,
                                                BCMI_LTSW_ECN_MPLS_ECN_MAP_TYPE_ECN2EXP,
                                                (int)fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID],
                                                &egr->mpls_ecn_map_id);
            if (SHR_SUCCESS(rv)) {
                egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP;
                egr->mpls_flags |= fv[BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY] ?
                                   BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST : 0;
            }
        }
        if (transit_view &&
            ((value == BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_NONE) ||
             (value == BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_PRESERVE))) {
            egr->mpls_label = BCM_MPLS_LABEL_INVALID;
        }
    }

    if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        SHR_EXIT();
    }

    if (!ipmc_view) {
        egr->flags3 |= fv[BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] ?
                       BCM_L3_FLAGS3_DO_NOT_CUT_THROUGH : 0;
    }

    if (ipmc_view && ul) {
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_DVP];
        if (value)
        {
            egr->encap_id = (bcm_if_t)value;
        }
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID];
        if (value) {
            egr->intf = (bcm_if_t)fv[BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID];
        } else {
            egr->intf = BCM_IF_INVALID;
        }
    }

    if (ipmc_view && !ul) {
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_DVP];
        if (value)
        {
            BCM_GPORT_FLOW_PORT_ID_SET(egr->port, (uint32)value);
        }
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID];
        if (value) {
            egr->intf = (bcm_if_t)fv[BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID];
        } else {
            egr->intf = BCM_IF_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete  next hop.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nh_delete(int unit, int nhidx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    int nhop_type;
    nh_cfg_t nh;
    bool da_profile_existed = false;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    fv = nh.flds;

    nhop_type = L3_NHOP_TYPE(unit, nhidx);

    dunit = bcmi_ltsw_dev_dunit(unit);

    switch (nhop_type) {
        case L3_UC_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;
            break;
        case L3_MC_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;
            break;
        case L3_IFP_ACTION_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION;
            break;
        case L3_MPLS_ENCAP_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP;
            da_profile_existed = true;
            break;
        case L3_MPLS_TRANSIT_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
    nh.fld_bmp = 1LL << BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_parse(unit, eh, tbl_info, &nh));

    if (da_profile_existed) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_tnl_dst_mac_profile_del(unit,
                                                    (int)fv[BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID]));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    L3_NHOP_TYPE(unit, nhidx) = L3_INVALID_NHOP_TYPE;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set ingress  next hop.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [in] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nh_set(int unit, bool ul, int nhidx, bcm_l3_egress_t *egr)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    const char *lt_name = NULL;
    int nhop_type;
    bool mpls_view = false;
    bool ipmc_view = false;
    bool transit_view = false;
    bool field_view = false;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if (egr->flags & BCM_L3_IPMC) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;
        nhop_type = L3_MC_NHOP_TYPE;
        ipmc_view = true;
    } else if (egr->flags2 & BCM_L3_FLAGS2_FIELD_ONLY) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION;
        nhop_type = L3_IFP_ACTION_NHOP_TYPE;
        field_view= true;
    } else if ((egr->flags & BCM_L3_ROUTE_LABEL) &&
               (egr->mpls_label != BCM_MPLS_LABEL_INVALID)) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP;
        nhop_type = L3_MPLS_ENCAP_NHOP_TYPE;
        mpls_view = true;
    } else if ((egr->mpls_label != BCM_MPLS_LABEL_INVALID) ||
                (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRESERVE)) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;
        nhop_type = L3_MPLS_TRANSIT_NHOP_TYPE;
        mpls_view = true;
        transit_view = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;
        nhop_type = L3_UC_NHOP_TYPE;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    fv = nh.flds;

    sal_memset(&nh, 0, sizeof(nh));
    fv[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = nhidx;
    nh.fld_bmp |= 1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID;
    if ((egr->flags & BCM_L3_REPLACE) && (nhop_type != L3_NHOP_TYPE(unit, nhidx))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (nh_delete(unit, nhidx));
        egr->flags &= ~BCM_L3_REPLACE;
        L3_NHOP_FBMP_SET(unit, nhidx);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (nh_cfg_set(unit, mpls_view, ipmc_view,
                    transit_view, field_view, ul, &nh,
                    egr));

    op = L3_NHOP_FBMP_GET(unit, nhidx) ? BCMLT_OPCODE_INSERT :
                                         BCMLT_OPCODE_UPDATE;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_fill(unit, eh, tbl_info, &nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    L3_NHOP_TYPE(unit, nhidx)= nhop_type;
exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress next hop.
 *
 * \param [in] unit Unit number.
 * \param [in] nhidx Entry index.
 * \param [out] egr Next hop entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
nh_get(int unit, int nhidx, bcm_l3_egress_t *egr)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    int nhop_type;
    bool mpls_view = false;
    bool ipmc_view = false;
    bool transit_view = false;
    bool field_view = false;
    bool ul = false;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&nh, 0, sizeof(nh));
    ul = L3_UL_NHOP_CHECK(unit, nhidx);
    nhop_type = L3_NHOP_TYPE(unit, nhidx);

    switch (nhop_type) {
        case L3_UC_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;
            break;
        case L3_MC_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;
            ipmc_view = true;
            break;
        case L3_IFP_ACTION_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION;
            field_view = true;
            break;
        case L3_MPLS_ENCAP_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP;
            mpls_view = true;
            break;
        case L3_MPLS_TRANSIT_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;
            mpls_view = true;
            transit_view = true;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
    nh.fld_bmp = tbl_info->fld_bmp;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

        lt_name = tbl_info->name;
        lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
        nh.fld_bmp = tbl_info->fld_bmp;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_name, &eh));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_parse(unit, eh, tbl_info, &nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (nh_cfg_get(unit, mpls_view, ipmc_view, transit_view, field_view, ul, &nh, egr));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the DLB member port.
 *
 * \param [in] unit Unit number.
 * \param [in] count Number of elements in nhop_array.
 * \param [in/out] member ECMP member info array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dlb_member_port_get(int unit, int count, ecmp_member_info_t *member)
{
    int dunit, i, ltidx;
    uint64_t value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trh = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ei;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trh));

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_NHOPs, &eh));

        ltidx = member[i].ul_nhop_ecmp_id & L3_NHOP_IDX_MASK(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, NHOP_IDs, ltidx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trh, BCMLT_OPCODE_LOOKUP, eh));

        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_transaction_commit(unit, trh, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_num_get(trh, i, &ei));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl, MODPORTs, &value));
        member[i].port_id = (uint16_t)value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl, NHOP_IDs, &value));
        member[i].ul_nhop_ecmp_id = (uint16_t)value;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
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
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
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
 * \param [in] ecmp_member_count Number of elements in emcp_member_array.
 * \param [in] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_set(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
                  int count, ecmp_member_info_t *member)
{
    ecmp_cfg_t ecmp;
    int dunit, i;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));
    ecmp.ecmp_id = idx;

    if ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        /* Weight size <=512 only valid for dynamic weighted mode. */
        if ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) &&
            (ecmp_info->max_paths <= BCMINT_LTSW_L3_ECMP_WGT_SZ_512)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        /* If count of member <= 512, max_paths must be 512. */
        if ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) &&
            (ecmp_info->max_paths != wgt_sz_val[2]) &&
            (count <= wgt_sz_val[2])) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;

        ecmp.fld_bmp |= (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID);

        ecmp.num_paths = count;
        ecmp.member_info = member;
        ecmp.weighted = true;
        /* Assign weight mode. */
        ecmp.weighted_mode = (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) ?
                             BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED : BCMINT_LTSW_L3_ECMP_MEMBER_REPLICATION;
        ecmp.fld_bmp |= (ecmp.weighted_mode == BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED) ?
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;
        switch (ecmp_info->max_paths) {
        case 128:
            ecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_128;
            break;
        case 256:
            ecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_256;
            break;
        case 512:
            ecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_512;
            break;
        case 1024:
            ecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_1K;
            break;
        case 2048:
            ecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_2K;
            break;
        case 4096:
            ecmp.weighted_size = BCMINT_LTSW_L3_ECMP_WGT_SZ_4K;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Assign weight value. */
        for (i = 0; i < count; i++) {
            ecmp.member_info[i].weight = member[i].weight;
        }
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;

        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
            ecmp.fld_bmp |= (1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED);
            ecmp.rh_num_paths = count;
            ecmp.res_hash = true;
            
            ecmp.rh_random_seed = 0;
            switch (ecmp_info->dynamic_size) {
            case 64:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_64;
                break;
            case 128:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_128;
                break;
            case 256:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_256;
                break;
            case 512:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_512;
                break;
            case 1024:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_1K;
                break;
            case 2048:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_2K;
                break;
            case 4096:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_4K;
                break;
            case 8192:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_8K;
                break;
            case 16384:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_16K;
                break;
            case 32768:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_32K;
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            ecmp.member_info = member;
        } else {
            ecmp.fld_bmp |= (1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID);
            ecmp.num_paths = count;
            ecmp.member_info = member;
            ecmp.max_paths = ecmp_info->max_paths;
            /* skip sort member in DLB mode. */
            if ((ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_NORMAL) &&
                (ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) &&
                (ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL) &&
                (ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
                ecmp.sorted = (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) ?
                              0 : 1;
            } else {
                ecmp_info->ecmp_group_flags |= BCM_L3_ECMP_PATH_NO_SORTING;
                ecmp.sorted = 0;
            }
        }

        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM;
        } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT;
        } else if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    /* ECMP member info. */
    ecmp.start_idx = 0;
    ecmp.num_of_set = count;

    if (ecmp_info->flags & BCM_L3_REPLACE) {
        op = BCMLT_OPCODE_UPDATE;
        /* Set the unused member elements to default value. */
        ecmp.num_of_unset = ecmp_info->max_paths - count;
    } else {
        op = BCMLT_OPCODE_INSERT;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index of ECMP group.
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
    bool weighted_ecmp;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    weighted_ecmp = ((L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
                    (L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_MEMBER_WEIGHTED)) ? true :false;


    tbl_id = weighted_ecmp ? BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL :
                             BCMINT_LTSW_L3_ECMP_TBL_UL;
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, ECMP_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    L3_UL_ECMP_GRP_FBMP_SET(unit, idx);
    L3_UL_ECMP_GRP_WGT_FBMP_SET(unit, idx);
    L3_UL_ECMP_GRP_FLAGS(unit, idx) = 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
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
ulecmp_config_find(int unit, int ecmp_member_count,
                   ecmp_member_info_t *ecmp_member,
                   int pri_cnt,
                   bcm_l3_egress_ecmp_t *ecmp_info)
{
    int dunit, ltidx, max_paths, sz, alt_cnt;
    int ecmp_grp_min, ecmp_grp_max;
    ecmp_member_info_t *emi = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ecmp_cfg_t ecmp;
    uint64_t num_paths;
    const char *ltname;
    bool wgt_i, dlb_en;
    bcmi_ltsw_dlb_config_t dlb;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    alt_cnt = ecmp_member_count - pri_cnt;

    max_paths = (L3_ECMP_RH_MAX_PATHS(unit) > L3_ECMP_WEIGHTED_MAX_PATHS(unit)) ?
                L3_ECMP_RH_MAX_PATHS(unit) : L3_ECMP_WEIGHTED_MAX_PATHS(unit);
    max_paths = (max_paths > L3_ECMP_MAX_PATHS(unit)) ? max_paths : L3_ECMP_MAX_PATHS(unit);

    sz = max_paths * sizeof(ecmp_member_info_t);
    SHR_ALLOC(emi, sz, "bcmLtswXgsEcmpMemberInfo");
    SHR_NULL_CHECK(emi, SHR_E_MEMORY);

    dunit = bcmi_ltsw_dev_dunit(unit);

    ecmp_grp_min = L3_UL_ECMP_GRP_MIN(unit);
    ecmp_grp_max = L3_UL_ECMP_GRP_MAX(unit);

    for (ltidx = ecmp_grp_min; ltidx <= ecmp_grp_max;
         ltidx ++) {

        /* Skip unused entries. */
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            continue;
        }

        if (L3_ECMP_DLB_GRP_CONFIGURED(unit, ltidx)) {
            wgt_i = false;
            dlb_en = true;

            /* Fetch group info from DLB_ECMP. */
            sal_memset(&dlb, 0, sizeof(bcmi_ltsw_dlb_config_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp,
                                          ltidx - L3_ECMP_DLB_GRP_OFFSET(unit),
                                          &dlb));
            if ((dlb.alternate_num_paths != alt_cnt) ||
                (dlb.num_paths != pri_cnt)) {
                continue;
            }
        } else {
            wgt_i = ((L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
                    (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_MEMBER_WEIGHTED)) ?
                    true : false;
            dlb_en = false;
        }

        tbl_id = wgt_i ? BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL :
                         BCMINT_LTSW_L3_ECMP_TBL_UL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

        ltname = tbl_info->name;

        sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));
        ecmp.weighted = wgt_i ? true : false;

        if (eh != BCMLT_INVALID_HDL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_free(eh));
            eh = BCMLT_INVALID_HDL;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ECMP_IDs, ltidx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        if (L3_UL_ECMP_GRP_LB_MODE(unit, ltidx) == ECMP_LB_MODE_RESILIENT) {
            ecmp.res_hash = true;
        }

        ecmp.fld_bmp = ecmp.res_hash ? (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) :
                                       (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        num_paths = ecmp.res_hash ? ecmp.rh_num_paths :
                                    ecmp.num_paths;

        if (num_paths != ecmp_member_count) {
            continue;
        }

        ecmp.fld_bmp = ecmp.res_hash ?
                      ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE)) :
                      (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID);

        ecmp.member_info = emi;

        sal_memset(emi, 0, sz);
        ecmp.num_of_set = num_paths;

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        if ((L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_PATH_NO_SORTING) ||
            ecmp.res_hash) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_member_sort(unit, emi, ecmp_member_count,
                                  sizeof(ecmp_member_info_t)));
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & (BCM_L3_ECMP_PATH_NO_SORTING |
                                                 BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED |
                                                 BCM_L3_ECMP_MEMBER_WEIGHTED)) {
            if (pri_cnt) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ecmp_member_sort(unit, emi, pri_cnt,
                                      sizeof(ecmp_member_info_t)));
            }
            if (alt_cnt) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ecmp_member_sort(unit, emi + pri_cnt, alt_cnt,
                                      sizeof(ecmp_member_info_t)));
            }
        }

        if (sal_memcmp(ecmp_member, emi,
                       (sizeof(ecmp_member_info_t) * num_paths))) {
            continue;
        }

        ecmp.fld_bmp = wgt_i ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE) :
                               ((1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                                (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS));

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        if (dlb_en) {
            ecmp_info->max_paths = ecmp.max_paths;
            ecmp_info->dynamic_age = dlb.inactivity_time;
            ecmp_info->dynamic_size = dlb.flowset_size;

            if (dlb.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
                ecmp_info->dgm.cost = dlb.alternate_config.alternate_path_cost;
                ecmp_info->dgm.bias = dlb.alternate_config.alternate_path_bias;
                ecmp_info->dgm.threshold =
                    dlb.alternate_config.primary_path_threshold;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (dlb_amode_to_ecmp_dmode(unit, dlb.assignment_mode,
                                             &ecmp_info->dynamic_mode));
            }
        }  else if (wgt_i) {
            ecmp_info->max_paths = wgt_sz_val[ecmp.weighted_size];
        } else if (ecmp.res_hash) {
            ecmp_info->dynamic_size = res_sz_val[ecmp.rh_size];
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        } else {
            ecmp_info->max_paths = ecmp.max_paths;
            if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
            } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }
        }
        ecmp_info->ecmp_group_flags = L3_UL_ECMP_GRP_FLAGS(unit, ltidx);
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                       &ecmp_info->ecmp_intf));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FREE(emi);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
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
 * \param [in] ul Indicates an underlay ECMP group.
 * \param [out] dlb_member Member info for DLB group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_config_member_add(int unit, int idx, ecmp_member_info_t *member,
                         bool alt, int pri_cnt, ecmp_member_info_t *dlb_member)
{
    ecmp_cfg_t ecmp;
    int dunit, alt_cnt, i;
    uint32_t sz, num_paths = 0;
    ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t ecmp_grp_flags;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));

    ecmp_grp_flags = L3_UL_ECMP_GRP_FLAGS(unit, idx);

    /* ECMP group with  BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED doesn`t support member operations. */
    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_LIMIT);
    }

    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    ecmp.ecmp_id = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (ecmp.weighted) {
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE);
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));
        if (ecmp.num_paths == wgt_sz_val[ecmp.weighted_size]) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
        ecmp_grp_flags |= BCM_L3_ECMP_PATH_NO_SORTING;
    } else {
        ecmp.fld_bmp = 1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE;
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));
        if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp.res_hash = true;
            /* Resilient HASH dosn`t support sorting. */
            ecmp_grp_flags |= BCM_L3_ECMP_PATH_NO_SORTING;
        }
        if (ecmp.res_hash) {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE);
        } else {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS);
        }
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        /* Check if there is room in ECMP group. */
        if (!ecmp.res_hash && (ecmp.num_paths == ecmp.max_paths)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }

        if (ecmp.res_hash && (ecmp.rh_num_paths == res_sz_val[ecmp.rh_size])) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
    }

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        if (ecmp.num_paths < pri_cnt) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    } else {
        pri_cnt = ecmp.res_hash ? ecmp.rh_num_paths : ecmp.num_paths;
    }

    alt_cnt = ecmp.res_hash ? 0 : (ecmp.num_paths - pri_cnt);

    if (ecmp.res_hash) {
        num_paths = ecmp.rh_num_paths;
    } else {
        num_paths = ecmp.num_paths;
    }

    sz = sizeof(ecmp_member_info_t) * (num_paths + 1);
    SHR_ALLOC(arr, sz, "bcmLtswXgsUlecmpMemberInfo");
    SHR_NULL_CHECK(arr, SHR_E_MEMORY);
    sal_memset(arr, 0, sz);

    ecmp.num_of_set = num_paths;
    ecmp.member_info = arr;
    ecmp.fld_bmp = ecmp.res_hash ? 1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID :
                                   1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID;

    SHR_IF_ERR_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (alt || !alt_cnt) {
        /* Add the new member to the tail*/
        sal_memcpy(&arr[num_paths], member,
                   sizeof(ecmp_member_info_t));
    } else {
        /* Shift the alternate paths. */
        for (i = num_paths; i > pri_cnt; i--) {
            sal_memcpy(&arr[i], &arr[i - 1], sizeof(ecmp_member_info_t));
        }
        /* Add the new member before alternate paths. */
        sal_memcpy(&arr[pri_cnt], member, sizeof(ecmp_member_info_t));
    }
    /* Sort the members. */
    if (!(L3_UL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_PATH_NO_SORTING) &&
        !ecmp.res_hash) {
        if (alt) {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, &arr[pri_cnt], alt_cnt + 1,
                                  sizeof(ecmp_member_info_t)));
        } else {
            SHR_IF_ERR_EXIT
                (ecmp_member_sort(unit, arr, pri_cnt + 1,
                                  sizeof(ecmp_member_info_t)));
        }
    }

    if (ecmp.res_hash) {
        ecmp.rh_num_paths = num_paths + 1;
    } else {
        ecmp.num_paths = num_paths + 1;
    }
    ecmp.num_paths = num_paths + 1;
    ecmp.start_idx = 0;
    ecmp.num_of_set = num_paths + 1;

    ecmp.fld_bmp = ecmp.res_hash ?
                  ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID)) :
                  ((1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID));

    ecmp.fld_bmp |= ecmp.weighted ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (dlb_member) {
        sal_memcpy(dlb_member, arr, (num_paths + 1) * sizeof(ecmp_member_info_t));
    }

exit:
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
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
ulecmp_config_member_del(int unit, int idx, ecmp_member_info_t *member,
                         bool alt, int pri_cnt, int *member_idx)
{
    ecmp_cfg_t ecmp;
    int dunit, i, sz, min, max;
    ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t ecmp_grp_flags;
    uint32_t lb_mode;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));

    lb_mode = L3_UL_ECMP_GRP_LB_MODE(unit, idx);

    ecmp_grp_flags = L3_UL_ECMP_GRP_FLAGS(unit, idx);

    /* ECMP group with  BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED doesn`t support member operations. */
    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_LIMIT);
    }

    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    ecmp.ecmp_id = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (!ecmp.weighted && (lb_mode == ECMP_LB_MODE_RESILIENT)) {
        ecmp.res_hash = true;
    }

    sz = ecmp.res_hash ? sizeof(ecmp_member_info_t) * L3_ECMP_RH_MAX_PATHS(unit) :
                         sizeof(ecmp_member_info_t) * L3_ECMP_MAX_PATHS(unit);
    sz = ecmp.weighted ? sizeof(ecmp_member_info_t) * L3_ECMP_WEIGHTED_MAX_PATHS(unit) : sz;

    SHR_ALLOC(arr, sz, "bcmLtswXgsEcmpMemberInfo");
    SHR_NULL_CHECK(arr, SHR_E_MEMORY);
    sal_memset(arr, 0, sz);
    ecmp.member_info = arr;

    ecmp.start_idx = 0;
    ecmp.num_of_set = ecmp.res_hash ? L3_ECMP_RH_MAX_PATHS(unit) : L3_ECMP_MAX_PATHS(unit);
    ecmp.num_of_set = ecmp.weighted ? L3_ECMP_WEIGHTED_MAX_PATHS(unit) : ecmp.num_of_set;

    ecmp.fld_bmp = ecmp.res_hash ? (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) :
                                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
    ecmp.fld_bmp |= ecmp.res_hash ? (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID) :
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID);
    ecmp.fld_bmp |= ecmp.weighted ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (ecmp.num_paths < pri_cnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        min = alt ? pri_cnt : 0;
        max = alt ? ecmp.num_paths : pri_cnt;
    } else {
        min = 0;
        max = ecmp.res_hash ? ecmp.rh_num_paths : ecmp.num_paths;
    }
    for (i = min; i < max; i++) {
        if ((member->nhop_id == arr[i].nhop_id) &&
            (member->ul_nhop_ecmp_id == arr[i].ul_nhop_ecmp_id)) {
            break;
        }
    }

    /* Check if interface that to be deleted was found. */
    if (i >= max) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Shift remaining elements of member array. */
    if (!ecmp.res_hash) {
        ecmp.start_idx = i;
        ecmp.member_info = &arr[i + 1];
        ecmp.num_of_set = ecmp.num_paths - i - 1;
        ecmp.num_paths--;
        ecmp.num_of_unset = 1;
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID);
        ecmp.fld_bmp |= ecmp.weighted ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;
    } else {
        ecmp.start_idx = i;
        ecmp.member_info = &arr[i + 1];
        ecmp.num_of_set = ecmp.rh_num_paths - i - 1;
        ecmp.rh_num_paths--;
        ecmp.num_of_unset = 1;
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (member_idx) {
        *member_idx = i;
    }

exit:
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Delete all member from an underlay ECMP table.
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
    ecmp_cfg_t ecmp;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t ecmp_grp_flags;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_grp_flags = L3_UL_ECMP_GRP_FLAGS(unit, idx);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));

    if ((ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    ecmp.ecmp_id = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    ecmp.fld_bmp = 1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS;

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (!ecmp.num_paths) {
        SHR_EXIT();
    }

    ecmp.start_idx = 0;
    ecmp.num_of_set = 0;
    ecmp.num_of_unset = ecmp.num_paths;
    ecmp.num_paths = 0;
    ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] ecmp_member_count Number of elements in emcp_member_array.
 * \param [in] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_config_set(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
                  int count, ecmp_member_info_t *member)
{
    ecmp_cfg_t ecmp;
    int dunit, i, max_paths;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t op;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));
    ecmp.ecmp_id = idx;

    if ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        if (ecmp_info->max_paths > wgt_sz_val[0]) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        max_paths = L3_OL_ECMP_WEIGHTED_MAX_PATHS(unit);

        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;

        ecmp.fld_bmp |= (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID);
        ecmp.num_paths = count;
        ecmp.member_info = member;
        ecmp.weighted = true;
        /* Assign weight mode. */
        ecmp.weighted_mode = (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) ?
                             BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED : BCMINT_LTSW_L3_ECMP_MEMBER_REPLICATION;
        ecmp.fld_bmp |= (ecmp.weighted_mode == BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED) ?
                        (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;
        /* Assign weight value. */
        for (i = 0; i < count; i++) {
            ecmp.member_info[i].weight = member[i].weight;
        }
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;

        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
            max_paths = L3_ECMP_RH_MAX_PATHS(unit);
            ecmp.fld_bmp |= (1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED);
            ecmp.rh_num_paths = count;
            ecmp.res_hash = true;
            
            ecmp.rh_random_seed = 0;
            switch (ecmp_info->dynamic_size) {
            case 64:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_64;
                break;
            case 128:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_128;
                break;
            case 256:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_256;
                break;
            case 512:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_512;
                break;
            case 1024:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_1K;
                break;
            case 2048:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_2K;
                break;
            case 4096:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_4K;
                break;
            case 8192:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_8K;
                break;
            case 16384:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_16K;
                break;
            case 32768:
                ecmp.rh_size = BCMINT_LTSW_L3_ECMP_RES_SZ_32K;
                break;
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            ecmp.member_info = member;
        } else {
            ecmp.fld_bmp |= (1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                            (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID);
            max_paths = L3_OL_ECMP_MAX_PATHS(unit);
            ecmp.num_paths = count;
            ecmp.member_info = member;
            ecmp.max_paths = ecmp_info->max_paths;
        }

        if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DISABLED) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM;
        } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT;
        } else if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL)
            || (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    /* ECMP member info. */
    ecmp.start_idx = 0;
    ecmp.num_of_set = count;

    if (ecmp_info->flags & BCM_L3_REPLACE) {
        op = BCMLT_OPCODE_UPDATE;
        /* Set the unused member elements to default value. */
        ecmp.num_of_unset = max_paths - count;
    } else {
        op = BCMLT_OPCODE_INSERT;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index of ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_config_del(int unit, int idx)
{
    int dunit;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bool weighted_ecmp;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    weighted_ecmp = ((L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
                     (L3_OL_ECMP_GRP_FLAGS(unit, idx) & BCM_L3_ECMP_MEMBER_WEIGHTED)) ?
                     true :false;


    tbl_id = weighted_ecmp ? BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL :
                             BCMINT_LTSW_L3_ECMP_TBL_OL;
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, ECMP_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    L3_OL_ECMP_GRP_FBMP_SET(unit, idx);
    L3_OL_ECMP_GRP_WGT_FBMP_SET(unit, idx);
    L3_OL_ECMP_GRP_FLAGS(unit, idx) = 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an overlay ECMP group.
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
olecmp_config_get(int unit, int idx, bcm_l3_egress_ecmp_t *ecmp_info,
                  int ecmp_member_size, bcm_l3_ecmp_member_t *ecmp_member_array,
                  int *ecmp_member_count)
{
    ecmp_cfg_t ecmp;
    int dunit, sz, i, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int num_paths;
    uint32_t lb_mode;
    uint32_t ecmp_grp_flags;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_info->ecmp_group_flags = L3_OL_ECMP_GRP_FLAGS(unit, idx);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));
    ecmp.ecmp_id = idx;
    lb_mode = L3_OL_ECMP_GRP_LB_MODE(unit, idx);
    ecmp_grp_flags = L3_OL_ECMP_GRP_FLAGS(unit, idx);
    if ((ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    if (!ecmp.weighted) {
        if (lb_mode == ECMP_LB_MODE_REGULAR) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else if (lb_mode == ECMP_LB_MODE_RANDOM) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM;
        } else if (lb_mode == ECMP_LB_MODE_RESILIENT) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT;
        }
        if ((ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) ||
            (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM)) {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS);
        } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED);
            ecmp.res_hash = true;
        }
    } else {
        if (lb_mode == ECMP_LB_MODE_WEIGHTED) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED;
        }
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (ecmp.weighted) {
        ecmp_info->max_paths = L3_OL_ECMP_GRP_MAX_PATHS(unit, idx);
    } else {
        ecmp_info->max_paths = ecmp.max_paths;
        if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
        } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
        } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
            ecmp_info->dynamic_size = res_sz_val[ecmp.rh_size];
            ecmp_info->max_paths = L3_OL_ECMP_GRP_MAX_PATHS(unit, idx);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

    if (ecmp_member_size > 0) {
        num_paths = (ecmp.fld_bmp & (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS)) ?
                    ecmp.rh_num_paths : ecmp.num_paths;
        *ecmp_member_count = ecmp_member_size < num_paths ?
                             ecmp_member_size : num_paths;

        ecmp.start_idx = 0;
        ecmp.num_of_set = *ecmp_member_count;
        sz = sizeof(ecmp_member_info_t) * ecmp.num_of_set;
        SHR_ALLOC(ecmp.member_info, sz, "bcmLtswXgsEcmpMemberInfo");
        SHR_NULL_CHECK(ecmp.member_info, SHR_E_MEMORY);
        sal_memset(ecmp.member_info, 0, sz);

        ecmp.fld_bmp = (ecmp.fld_bmp & (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS)) ?
                       ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID)) :
                       ((1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID));

        ecmp.fld_bmp |= ecmp.weighted ? 1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT : 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        for (i = 0; i < *ecmp_member_count; i++) {
            if (ecmp.member_info[i].nhop_id) {
                ltidx = ecmp.member_info[i].nhop_id;
                ecmp_member_array[i].weight = ecmp.member_info[i].weight;
                type = BCMI_LTSW_L3_EGR_OBJ_T_OL;
                SHR_IF_ERR_VERBOSE_EXIT
                    (xgs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &ecmp_member_array[i].egress_if));
            }
            if (ecmp.member_info[i].ul_nhop_ecmp_id) {
                ltidx = ecmp.member_info[i].ul_nhop_ecmp_id;
                ecmp_member_array[i].weight = ecmp.member_info[i].weight;
                type = ecmp.member_info[i].ul_ecmp ? BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL :
                                                     BCMI_LTSW_L3_EGR_OBJ_T_UL;
                SHR_IF_ERR_VERBOSE_EXIT
                    (xgs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &ecmp_member_array[i].egress_if_2));

            }
        }
    } else {
        *ecmp_member_count = ecmp.res_hash ? ecmp.rh_num_paths : ecmp.num_paths;
    }

exit:
    SHR_FREE(ecmp.member_info);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
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
olecmp_config_find(int unit, int ecmp_member_count,
                   ecmp_member_info_t *ecmp_member,
                   bcm_l3_egress_ecmp_t *ecmp_info)
{
    int dunit, ltidx, max_paths, sz;
    int ecmp_grp_min, ecmp_grp_max;
    ecmp_member_info_t *emi = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ecmp_cfg_t ecmp;
    uint64_t num_paths;
    const char *ltname;
    bool wgt_i;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    max_paths = (L3_ECMP_RH_MAX_PATHS(unit) > L3_OL_ECMP_WEIGHTED_MAX_PATHS(unit)) ?
                L3_ECMP_RH_MAX_PATHS(unit) : L3_OL_ECMP_WEIGHTED_MAX_PATHS(unit);
    max_paths = (max_paths > L3_ECMP_MAX_PATHS(unit)) ? max_paths : L3_ECMP_MAX_PATHS(unit);

    sz = max_paths * sizeof(ecmp_member_info_t);
    SHR_ALLOC(emi, sz, "bcmLtswXgsEcmpMemberInfo");
    SHR_NULL_CHECK(emi, SHR_E_MEMORY);

    dunit = bcmi_ltsw_dev_dunit(unit);

    ecmp_grp_min = L3_OL_ECMP_GRP_MIN(unit);
    ecmp_grp_max = L3_OL_ECMP_GRP_MAX(unit);

    for (ltidx = ecmp_grp_min; ltidx <= ecmp_grp_max;
         ltidx ++) {

        /* Skip unused entries. */
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            continue;
        }

        wgt_i = ((L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
                 (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_MEMBER_WEIGHTED))  ?
                 true : false;

        tbl_id = wgt_i ? BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL :
                         BCMINT_LTSW_L3_ECMP_TBL_OL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

        ltname = tbl_info->name;

        sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));
        ecmp.weighted = wgt_i ? true : false;

        if (eh != BCMLT_INVALID_HDL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_free(eh));
            eh = BCMLT_INVALID_HDL;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));


        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, ECMP_IDs, ltidx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        if (L3_OL_ECMP_GRP_LB_MODE(unit, ltidx) == ECMP_LB_MODE_RESILIENT) {
            ecmp.res_hash = true;
        }

        ecmp.fld_bmp = ecmp.res_hash ? (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) :
                                       (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        num_paths = ecmp.res_hash ? ecmp.rh_num_paths : ecmp.num_paths;

        if (num_paths != ecmp_member_count) {
            continue;
        }

        ecmp.fld_bmp = ecmp.res_hash ?
                      ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID)) :
                      ((1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID));

        ecmp.member_info = emi;
        sal_memset(emi, 0, sz);
        ecmp.num_of_set = num_paths;

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        if ((L3_OL_ECMP_GRP_FLAGS(unit, ltidx) &
            (BCM_L3_ECMP_PATH_NO_SORTING |
             BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED |
             BCM_L3_ECMP_MEMBER_WEIGHTED)) ||
            ecmp.res_hash) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_member_sort(unit, emi, ecmp_member_count,
                                  sizeof(ecmp_member_info_t)));
        }

        if (sal_memcmp(ecmp_member, emi,
                       (sizeof(ecmp_member_info_t) * num_paths))) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        if (wgt_i) {
            ecmp_info->max_paths = wgt_sz_val[ecmp.weighted_size];
        } else if (ecmp.res_hash) {
            ecmp_info->dynamic_size = res_sz_val[ecmp.rh_size];
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        } else {
            ecmp_info->max_paths = ecmp.max_paths;
            if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
            } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
                ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }
        }
        ecmp_info->ecmp_group_flags = L3_OL_ECMP_GRP_FLAGS(unit, ltidx);
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                       &ecmp_info->ecmp_intf));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FREE(emi);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer toECMP member info.
 * \param [in] alt Indicate an alternate path to be added.
 * \param [in] pri_cnt Number of primary paths.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_config_member_add(int unit, int idx, ecmp_member_info_t *member,
                         bool alt, int pri_cnt)
{
    ecmp_cfg_t ecmp;
    int dunit, alt_cnt, i;
    uint32_t sz, num;
    ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t ecmp_grp_flags;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));

    ecmp_grp_flags = L3_OL_ECMP_GRP_FLAGS(unit, idx);

    /* ECMP group with  BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED doesn`t support member operations. */
    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_LIMIT);
    }

    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    ecmp.ecmp_id = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (ecmp.weighted) {
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));
        if (ecmp.num_paths == wgt_sz_val[ecmp.weighted_size]) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
        ecmp_grp_flags |= BCM_L3_ECMP_PATH_NO_SORTING;
    } else {
        ecmp.fld_bmp = 1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE;
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));
        if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp.res_hash = true;
            /* Resilient HASH dosn`t support sorting. */
            ecmp_grp_flags |= BCM_L3_ECMP_PATH_NO_SORTING;
        }
        if (ecmp.res_hash) {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE);
        } else {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS);
        }
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));



        /* Check if there is room in ECMP group. */
        if (!ecmp.res_hash && (ecmp.num_paths == ecmp.max_paths)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }

        if (ecmp.res_hash && (ecmp.rh_num_paths == res_sz_val[ecmp.rh_size])) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
    }

    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        if (ecmp.num_paths < pri_cnt) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    } else {
        pri_cnt = ecmp.res_hash ? ecmp.rh_num_paths : ecmp.num_paths;
    }

    alt_cnt = ecmp.res_hash ? 0 : (ecmp.num_paths - pri_cnt);
    if ((ecmp_grp_flags & BCM_L3_ECMP_PATH_NO_SORTING) &&
        (alt || !alt_cnt)) {
        /* Resilient HASH dosn`t support sorting. */
        if (ecmp.res_hash) {
            /* Add the new member to the tail. */
            ecmp.start_idx = ecmp.rh_num_paths;
            ecmp.rh_num_paths++;
            ecmp.num_of_set = 1;
            ecmp.member_info = member;

        } else {
            /* Add the new member to the tail. */
            ecmp.start_idx = ecmp.num_paths;
            ecmp.num_paths++;
            ecmp.num_of_set = 1;
            ecmp.member_info = member;
        }
    } else {
        num = ecmp.num_paths + 1;
        sz = sizeof(ecmp_member_info_t) * num;
        SHR_ALLOC(arr, sz, "bcmLtswXgsEcmpMemberInfo");
        SHR_NULL_CHECK(arr, SHR_E_MEMORY);
        sal_memset(arr, 0, sz);

        ecmp.num_of_set = ecmp.num_paths;
        ecmp.member_info = arr;
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID);
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        if (alt || !alt_cnt) {
            /* Add the new member to the tail*/
            sal_memcpy(&arr[ecmp.num_paths], member,
                       sizeof(ecmp_member_info_t));
        } else {
            /* Shift the alternate paths. */
            for (i = ecmp.num_paths; i > pri_cnt; i--) {
                sal_memcpy(&arr[i], &arr[i - 1], sizeof(ecmp_member_info_t));
            }
            /* Add the new member before alternate paths. */
            sal_memcpy(&arr[pri_cnt], member, sizeof(ecmp_member_info_t));
        }
        /* Sort the members. */
        if (!(ecmp_grp_flags & BCM_L3_ECMP_PATH_NO_SORTING)) {
            if (alt) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ecmp_member_sort(unit, &arr[pri_cnt], alt_cnt + 1,
                                      sizeof(ecmp_member_info_t)));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ecmp_member_sort(unit, arr, pri_cnt + 1,
                                      sizeof(ecmp_member_info_t)));
            }
        }
        ecmp.num_paths = num;
        ecmp.start_idx = 0;
        ecmp.num_of_set = num;
    }

    ecmp.fld_bmp = ecmp.res_hash ?
                  ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID)) :
                  ((1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID));
    ecmp.fld_bmp |= ecmp.weighted ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an ECMP table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer to ECMP member structure.
 * \param [in] alt Indicate a alternate path to be removed.
 * \param [in] pri_cnt Number of primary paths.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_config_member_del(int unit, int idx, ecmp_member_info_t *member,
                         bool alt, int pri_cnt)
{
    ecmp_cfg_t ecmp;
    int dunit, i, sz, min, max;
    ecmp_member_info_t *arr = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t ecmp_grp_flags;
    uint32_t lb_mode;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));

    lb_mode = L3_OL_ECMP_GRP_LB_MODE(unit, idx);

    ecmp_grp_flags = L3_OL_ECMP_GRP_FLAGS(unit, idx);

    /* ECMP group with  BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED doesn`t support member operations. */
    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_LIMIT);
    }

    if (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    ecmp.ecmp_id = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (!ecmp.weighted && (lb_mode == ECMP_LB_MODE_RESILIENT)) {
        ecmp.res_hash = true;
    }

    sz = ecmp.res_hash ? sizeof(ecmp_member_info_t) * L3_ECMP_RH_MAX_PATHS(unit) :
                         sizeof(ecmp_member_info_t) * L3_ECMP_MAX_PATHS(unit);
    sz = ecmp.weighted ? sizeof(ecmp_member_info_t) * L3_ECMP_WEIGHTED_MAX_PATHS(unit) : sz;

    SHR_ALLOC(arr, sz, "bcmLtswXgsEcmpMemberInfo");
    SHR_NULL_CHECK(arr, SHR_E_MEMORY);
    sal_memset(arr, 0, sz);
    ecmp.member_info = arr;

    ecmp.start_idx = 0;
    ecmp.num_of_set = ecmp.res_hash ? L3_ECMP_RH_MAX_PATHS(unit) : L3_ECMP_MAX_PATHS(unit);
    ecmp.num_of_set = ecmp.weighted ? L3_ECMP_WEIGHTED_MAX_PATHS(unit) : ecmp.num_of_set;

    ecmp.fld_bmp = ecmp.res_hash ? (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) :
                                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
    ecmp.fld_bmp |= ecmp.res_hash ? ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID)) :
                                    ((1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID));
    ecmp.fld_bmp |= ecmp.weighted ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (ecmp.num_paths < pri_cnt) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    min = 0;
    max = ecmp.res_hash ? ecmp.rh_num_paths : ecmp.num_paths;

    for (i = min; i < max; i++) {
        if ((member->nhop_id == arr[i].nhop_id) &&
            (member->ul_nhop_ecmp_id == arr[i].ul_nhop_ecmp_id)) {
            break;
        }
    }

    /* Check if interface that to be deleted was found. */
    if (i >= max) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Shift remaining elements of member array. */
    if (!ecmp.res_hash) {
        ecmp.start_idx = i;
        ecmp.member_info = &arr[i + 1];
        ecmp.num_of_set = ecmp.num_paths - i - 1;
        ecmp.num_paths--;
        ecmp.num_of_unset = 1;
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID);
        ecmp.fld_bmp |= ecmp.weighted ? (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) : 0;
    } else {
        ecmp.start_idx = i;
        ecmp.member_info = &arr[i + 1];
        ecmp.num_of_set = ecmp.rh_num_paths - i - 1;
        ecmp.rh_num_paths--;
        ecmp.num_of_unset = 1;
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    SHR_FREE(arr);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Delete all member from an ECMP table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_config_member_del_all(int unit, int idx)
{
    ecmp_cfg_t ecmp;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    uint32_t ecmp_grp_flags;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_grp_flags = L3_OL_ECMP_GRP_FLAGS(unit, idx);

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));

    if ((ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    ecmp.ecmp_id = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    if (!ecmp.weighted) {
        ecmp.fld_bmp = 1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE;
        SHR_IF_ERR_VERBOSE_EXIT
           (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));
        if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp.res_hash = true;
        }
    }

    ecmp.fld_bmp = ecmp.res_hash ? 1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS :
                                   1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS;

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (!ecmp.num_paths && !ecmp.rh_num_paths) {
        SHR_EXIT();
    }

    ecmp.start_idx = 0;
    ecmp.num_of_set = 0;
    ecmp.num_of_unset = ecmp.num_paths;
    ecmp.num_paths = 0;
    ecmp.fld_bmp = ecmp.res_hash ? 1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS :
                                   1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS;
    ecmp.fld_bmp |= ecmp.res_hash ? ((1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                                     (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                                     (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                                     (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID)) :
                                    ((1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                                    (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
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
             int pri_cnt, int alt_cnt, ecmp_member_info_t *member)
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
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_dmode_to_dlb_amode(unit, ecmp_info->dynamic_mode,
                                     &cfg.assignment_mode));
    }

    if (pri_cnt) {

        sz = sizeof(bcmi_ltsw_dlb_member_t) * pri_cnt;
        SHR_ALLOC(dlb_member, sz, "bcmXgsL3EcmpDlbMember");
        SHR_NULL_CHECK(dlb_member, SHR_E_MEMORY);
        sal_memset(dlb_member, 0, sz);

        cfg.num_paths = pri_cnt;
        cfg.member = dlb_member;
        for (i = 0; i < pri_cnt; i++) {
            dlb_member[i].nhop_id = member[i].ul_nhop_ecmp_id;
            dlb_member[i].port_id = member[i].port_id;
        }
    }

    if (alt_cnt) {
        sz = sizeof(bcmi_ltsw_dlb_member_t) * alt_cnt;
        SHR_ALLOC(alt_dlb_member, sz, "bcmXgsL3EcmpDlbMember");
        SHR_NULL_CHECK(alt_dlb_member, SHR_E_MEMORY);
        sal_memset(alt_dlb_member, 0, sz);

        cfg.alternate_num_paths = alt_cnt;
        cfg.alternate_member = alt_dlb_member;
        for (i = 0; i < alt_cnt; i++) {
            alt_dlb_member[i].nhop_id = member[i + pri_cnt].ul_nhop_ecmp_id;
            alt_dlb_member[i].port_id = member[i + pri_cnt].port_id;
        }
    }

    dlb_id = idx - L3_ECMP_DLB_GRP_MIN(unit);

    if (!((ecmp_info->flags & BCM_L3_REPLACE) &&
          L3_ECMP_DLB_GRP_CONFIGURED(unit, idx))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_id_create(unit, BCMI_LTSW_DLB_O_WITH_ID,
                                     bcmi_dlb_type_ecmp, &dlb_id));
            dlb_id_created = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
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
    int dlb_id = idx - L3_ECMP_DLB_GRP_MIN(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
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

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));

    sz = (cap.max_members_per_group + cap.max_alt_members_per_group) *
         sizeof(bcmi_ltsw_dlb_member_t);
    SHR_ALLOC(dlb_member, sz, "bcmLtswXgsEcmpDlbMember");
    SHR_NULL_CHECK(dlb_member, SHR_E_MEMORY);
    sal_memset(dlb_member, 0, sz);

    cfg.member = dlb_member;
    cfg.max_num_paths = cap.max_members_per_group;
    cfg.alternate_member = dlb_member + cap.max_members_per_group;
    cfg.max_alternate_num_paths = cap.max_alt_members_per_group;

    dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));

    if (member_idx >= cfg.num_paths ) {
        if (!cfg.alternate_num_paths) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        i = member_idx - cfg.num_paths;
        num = cfg.alternate_num_paths;
        ptr = cfg.alternate_member;
        cfg.alternate_num_paths--;
    } else {
        if (!cfg.num_paths) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        i = member_idx;
        num = cfg.num_paths;
        ptr = cfg.member;
        cfg.num_paths--;
    }

    for (; i < (num - 1); i++) {
        ptr[i].port_id = ptr[i + 1].port_id;
        ptr[i].nhop_id = ptr[i + 1].nhop_id;
    }
    ptr[num - 1].nhop_id = 0;
    ptr[num - 1].port_id = 0;

    SHR_IF_ERR_VERBOSE_EXIT
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

    dlb_id = idx - L3_ECMP_DLB_GRP_MIN(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));

    if (!cfg.alternate_num_paths && !cfg.num_paths) {
        SHR_EXIT();
    }

    cfg.alternate_num_paths = 0;
    cfg.num_paths = 0;
    SHR_IF_ERR_VERBOSE_EXIT
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
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, egr->port, port));
    } else {
        /* ECMP DLB members should be local */
        SHR_IF_ERR_VERBOSE_EXIT
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
 * \brief Allocate and fill ECMP member info array.
 *
 * \param [in] unit Unit number.
 * \param [in] count Number of elements to be allocated.
 * \param [in] ecmp_member ECMP members' information.
 * \param [out] member Allocated member info pointer.
 * \param [out] pri_cnt Number of primary paths.
 * \param [out] types Egress object types.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ecmp_member_info_array_alloc(int unit, int count,
                             bcm_l3_ecmp_member_t *ecmp_member,
                             bool skip_weight,
                             ecmp_member_info_t **member,
                             int *pri_cnt, uint32_t *types)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int sz, ltidx, i, j, m, egr_types, n;
    bcm_if_t intfs[2];
    bool ol, ul;
    bcmi_ltsw_l3_egr_obj_type_t type;
    ecmp_member_info_t *emi = NULL;
    SHR_BITDCL *fbmp;

    SHR_FUNC_ENTER(unit);

    if (!count) {
        *member = NULL;
        SHR_EXIT();
    }

    sz = sizeof(ecmp_member_info_t) * count;
    SHR_ALLOC(emi, sz, "bcmLtswXgsEcmpMemberInfo");
    SHR_NULL_CHECK(emi, SHR_E_MEMORY);
    sal_memset(emi, 0, sz);

    i = 0;
    j = count - 1;
    egr_types = 0;
    for (m = 0; m < count; m++) {
        intfs[0] = ecmp_member[m].egress_if;
        intfs[1] = ecmp_member[m].egress_if_2;
        ol = false;
        ul = false;
        for (n = 0; n < 2; n++) {
            if (intfs[n] <= 0) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_l3_egress_obj_id_resolve(unit, intfs[n],
                                                   &ltidx, &type));
            if ((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) && !ol) {
                fbmp = ei->nh_fbmp;
                if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                    emi[j].nhop_id = ltidx;
                }else {
                    emi[i].nhop_id = ltidx;
                }
                ol = true;
            } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) && !ul) {
                fbmp = ei->nh_fbmp;
                if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                    emi[j].ul_nhop_ecmp_id = ltidx;
                } else {
                    emi[i].ul_nhop_ecmp_id = ltidx;
                }
                ul = true;
            } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) && !ul) {
                fbmp = ei->ulecmp_grp_fbmp;
                if (ecmp_member[m].flags & BCM_L3_ECMP_MEMBER_DGM_ALTERNATE) {
                    emi[j].ul_nhop_ecmp_id = ltidx;
                    emi[j].ul_ecmp = true;
                } else {
                    emi[i].ul_nhop_ecmp_id = ltidx;
                    emi[i].ul_ecmp = true;
                }
                ul = true;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (SHR_BITGET(fbmp, (ltidx & L3_NHOP_IDX_MASK(unit)))) {
                SHR_IF_ERR_MSG_EXIT
                (SHR_E_PARAM,
                 (BSL_META_U(unit, "No egress interface.(%d, %d)\n"),
                  intfs[0], intfs[1]));
            }

            egr_types |= (1 << type);
        }
        if (!ol && !ul) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_PARAM,
                 (BSL_META_U(unit, "No egress interface.(%d, %d)\n"),
                  intfs[0], intfs[1]));
        }
        emi[i].weight = skip_weight ? 0 : ecmp_member[m].weight;
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

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(emi);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] ecmp_member_count Number of elements in emcp_member_array.
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
    ecmp_member_info_t *member = NULL;
    uint32_t egr_types = 0, dlb_ft;
    bool wgt, dgm, dlb_pre, ecmp_added = false;
    bcmi_ltsw_dlb_capability_t cap;

    SHR_FUNC_ENTER(unit);

    wgt = ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
           (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) ? true : false;
    dgm = (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) ?
          true : false;
    dlb_pre = ((ecmp_info->flags & BCM_L3_REPLACE) &&
               L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) ? true : false;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, ecmp_member_count,
                                      ecmp_member_array, false, &member,
                                      &pri_cnt, &egr_types));

    /*
     * Validate the egress objects:
     *  1. No underlay ECMP object as a member.
     *  2. No alternate path if without DGM enabled.
     */
    if ((egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) ||
        ((pri_cnt != ecmp_member_count) && !dgm)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    alt_cnt = ecmp_member_count - pri_cnt;

    if (dlb_en) {
        /* Validate the number of members. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_feature_get(unit, &dlb_ft));

        if (dgm && !(dlb_ft & BCMI_LTSW_DLB_F_ECMP_DGM)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }

        if ((pri_cnt > cap.max_members_per_group) ||
            (alt_cnt > cap.max_alt_members_per_group)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Get ports for each DLB ECMP member. */
        if (pri_cnt) {
            SHR_IF_ERR_VERBOSE_EXIT
                (dlb_member_port_get(unit, pri_cnt, member));
        }
        if (alt_cnt) {
            SHR_IF_ERR_VERBOSE_EXIT
                (dlb_member_port_get(unit, alt_cnt, &member[pri_cnt]));
        }
    }

    /* Only regular mode and random mode support member sort. */
    if (!(ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) && !wgt &&
        ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* Sort primary paths. */
        if (pri_cnt) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_member_sort(unit, member, pri_cnt,
                                  sizeof(ecmp_member_info_t)));
        }
        /* Sort alternate paths. */
        if (alt_cnt) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_member_sort(unit, &member[pri_cnt], alt_cnt,
                                  sizeof(ecmp_member_info_t)));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ulecmp_config_set(unit, idx, ecmp_info, ecmp_member_count, member));

    if (!(ecmp_info->flags & BCM_L3_REPLACE)) {
        ecmp_added = true;
    }

    if (dlb_en) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_dlb_set(unit, idx, ecmp_info, pri_cnt, alt_cnt, member));
    } else if (dlb_pre) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_dlb_del(unit, idx));
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_PATH_NO_SORTING;
    } else {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_PATH_NO_SORTING;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED;
    } else {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_MEMBER_WEIGHTED;
    } else {
        L3_UL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_MEMBER_WEIGHTED;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        L3_UL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_REPLICATION;
    } else if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        L3_UL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_WEIGHTED;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        L3_UL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_RESILIENT;
        L3_UL_ECMP_GRP_MAX_PATHS(unit, idx) = ecmp_info->max_paths;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
        L3_UL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_RANDOM;
    } else if (dlb_en) {
        L3_UL_ECMP_GRP_LB_MODE(unit, idx) = dgm ? ECMP_LB_MODE_DGM : ECMP_LB_MODE_DLB;
    } else {
        L3_UL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_REGULAR;
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
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_dlb_del(unit, idx));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ulecmp_config_del(unit, idx));

    L3_UL_ECMP_GRP_FBMP_SET(unit, idx);
    L3_UL_ECMP_GRP_WGT_FBMP_SET(unit, idx);
    L3_UL_ECMP_GRP_FLAGS(unit, idx) = 0;
    L3_UL_ECMP_GRP_LB_MODE(unit, idx) = 0;
    L3_UL_ECMP_GRP_MAX_PATHS(unit, idx) = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an underlay ECMP group.
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
    bcmi_ltsw_dlb_config_t dlb;
    bool dlb_en = false;
    ecmp_cfg_t ecmp;
    int dunit, sz, i, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int num_paths;
    uint32_t lb_mode;
    uint32_t ecmp_grp_flags;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_info->ecmp_group_flags = L3_UL_ECMP_GRP_FLAGS(unit, idx);

    /* Fetch group info from DLB_ECMP. */
    if (ltsw_feature(unit, LTSW_FT_DLB) &&
        L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) {
        dlb_en = true;

        sal_memset(&dlb, 0, sizeof(bcmi_ltsw_dlb_config_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp,
                                      idx - L3_ECMP_DLB_GRP_OFFSET(unit),
                                      &dlb));
    }

    sal_memset(&ecmp, 0, sizeof(ecmp_cfg_t));
    ecmp.ecmp_id = idx;
    lb_mode = L3_UL_ECMP_GRP_LB_MODE(unit, idx);
    ecmp_grp_flags = L3_UL_ECMP_GRP_FLAGS(unit, idx);
    if ((ecmp_grp_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_grp_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;
        ecmp.weighted = true;
    } else {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_fill(unit, eh, tbl_info, &ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    if (!ecmp.weighted) {
        if (lb_mode == ECMP_LB_MODE_REGULAR) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR;
        } else if (lb_mode == ECMP_LB_MODE_RANDOM) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM;
        } else if (lb_mode == ECMP_LB_MODE_RESILIENT) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT;
        }
        if ((ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) ||
            (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM)) {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED);
        } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                           (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED);
        }
    } else {
        if (lb_mode == ECMP_LB_MODE_WEIGHTED) {
            ecmp.lb_mode = BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED;
        }
        ecmp.fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE) |
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

    if (dlb_en) {
        if (ecmp.num_paths != (dlb.num_paths + dlb.alternate_num_paths)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        ecmp_info->max_paths = ecmp.max_paths;
        ecmp_info->dynamic_age = dlb.inactivity_time;
        ecmp_info->dynamic_size = dlb.flowset_size;

        if (dlb.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;
            ecmp_info->dgm.cost = dlb.alternate_config.alternate_path_cost;
            ecmp_info->dgm.bias = dlb.alternate_config.alternate_path_bias;
            ecmp_info->dgm.threshold =
                               dlb.alternate_config.primary_path_threshold;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (dlb_amode_to_ecmp_dmode(unit, dlb.assignment_mode,
                                         &ecmp_info->dynamic_mode));
        }
    } else if (ecmp.weighted) {
        ecmp_info->max_paths = wgt_sz_val[ecmp.weighted_size];
    } else {
        ecmp_info->max_paths = ecmp.max_paths;
        if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_REGULAR) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
        } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RANDOM) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
        } else if (ecmp.lb_mode == BCMINT_LTSW_L3_ECMP_LB_MODE_RESILIENT) {
            ecmp_info->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
            ecmp_info->dynamic_size = res_sz_val[ecmp.rh_size];
            ecmp_info->max_paths = L3_UL_ECMP_GRP_MAX_PATHS(unit, idx);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

    if (ecmp_member_size > 0) {
        num_paths = (ecmp.fld_bmp & (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS)) ?
                    ecmp.rh_num_paths : ecmp.num_paths;
        *ecmp_member_count = ecmp_member_size < num_paths ?
                             ecmp_member_size : num_paths;

        ecmp.start_idx = 0;
        ecmp.num_of_set = *ecmp_member_count;
        sz = sizeof(ecmp_member_info_t) * ecmp.num_of_set;
        SHR_ALLOC(ecmp.member_info, sz, "bcmLtswXgsEcmpMemberInfo");
        SHR_NULL_CHECK(ecmp.member_info, SHR_E_MEMORY);
        sal_memset(ecmp.member_info, 0, sz);

        ecmp.fld_bmp = (ecmp.fld_bmp & (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS)) ?
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID) :
                       (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID);

        if (ecmp.weighted_mode == BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHTED) {
            ecmp.fld_bmp |= 1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp));

        for (i = 0; i < *ecmp_member_count; i++) {
            if (ecmp.member_info[i].ul_nhop_ecmp_id) {
                ltidx = ecmp.member_info[i].ul_nhop_ecmp_id;
                ecmp_member_array[i].weight = ecmp.member_info[i].weight;
                type = BCMI_LTSW_L3_EGR_OBJ_T_UL;
                SHR_IF_ERR_VERBOSE_EXIT
                    (xgs_ltsw_l3_egress_obj_id_construct(unit, ltidx, type,
                                                         &ecmp_member_array[i].egress_if));
            }
            if (dlb_en && (i >= dlb.num_paths)) {
                ecmp_member_array[i].flags = BCM_L3_ECMP_MEMBER_DGM_ALTERNATE;
            }
        }
    } else {
        *ecmp_member_count = (ecmp.fld_bmp & (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS)) ?
                              ecmp.rh_num_paths : ecmp.num_paths;
    }

exit:
    SHR_FREE(ecmp.member_info);
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an underlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] emcp_member Pointer to ECMP member structure.
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
    ecmp_member_info_t *member = NULL;
    ecmp_member_info_t *dlb_emi = NULL;
    bcmi_ltsw_dlb_member_t *dlb_member = NULL;
    bcmi_ltsw_dlb_config_t cfg;

    SHR_FUNC_ENTER(unit);

    dlb = (ltsw_feature(unit, LTSW_FT_DLB) &&
           L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) ? true : false;
    dlb_id = idx - L3_ECMP_DLB_GRP_OFFSET(unit);

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));
    if (dlb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));

        sz = (cfg.num_paths + cfg.alternate_num_paths + 1) *
              sizeof(ecmp_member_info_t);
        SHR_ALLOC(dlb_emi, sz, "bcmLtswXgsDlbEcmpMemberInfo");
        SHR_NULL_CHECK(dlb_emi, SHR_E_MEMORY);
        sal_memset(dlb_emi, 0, sz);

        sz = (cfg.num_paths + cfg.alternate_num_paths + 1) *
              sizeof(bcmi_ltsw_dlb_member_t);
        SHR_ALLOC(dlb_member, sz, "bcmLtswXgsDlbMemberInfo");
        SHR_NULL_CHECK(dlb_member, SHR_E_MEMORY);
        sal_memset(dlb_member, 0, sz);
    }
    dgm = (cfg.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) ? true : false;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, false, &member,
                                      &pri_cnt, &egr_types));

    /*
     * Validate the egress objects:
     *  1. No underlay ECMP object as a member.
     *  2. No alternate path if without DGM enabled.
     */
    if ((egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) ||
        (!pri_cnt && !dgm)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    alt = pri_cnt ? false : true;

    SHR_IF_ERR_VERBOSE_EXIT
        (ulecmp_config_member_add(unit, idx, member, alt, cfg.num_paths,
                                  dlb_emi));
    if (dlb) {
        if (alt) {
            cfg.alternate_num_paths++;
        } else {
            cfg.num_paths++;
        }

        rv = dlb_member_port_get(unit,
                                 (cfg.alternate_num_paths + cfg.num_paths),
                                 dlb_emi);
        if (SHR_FAILURE(rv)) {
            /* Roll back the newly added member in underlay ECMP group. */
            (void)ulecmp_config_member_del(unit, idx, member, alt, cfg.num_paths,
                                           NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        for (i = 0; i < cfg.num_paths + cfg.alternate_num_paths; i++) {
            dlb_member[i].port_id = dlb_emi[i].port_id;
            dlb_member[i].nhop_id = dlb_emi[i].ul_nhop_ecmp_id;
        }

        cfg.member = dlb_member;
        cfg.alternate_member = dlb_member + cfg.num_paths;

        rv = bcmi_ltsw_dlb_config_set(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg);
        if (SHR_FAILURE(rv)) {
            /* Roll back the newly added member in underlay ECMP group. */
            (void)ulecmp_config_member_del(unit, idx, member, alt, cfg.num_paths,
                                           NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
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
 * \param [in] emcp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ulecmp_member_del(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    bool dlb, dgm, alt;
    int dlb_id, pri_cnt, member_idx;
    uint32_t egr_types;
    ecmp_member_info_t *member = NULL;
    bcmi_ltsw_dlb_config_t cfg;

    SHR_FUNC_ENTER(unit);

    dlb = (ltsw_feature(unit, LTSW_FT_DLB) &&
           L3_ECMP_DLB_GRP_CONFIGURED(unit, idx)) ? true : false;

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_dlb_config_t));
    if (dlb) {
        dlb_id = idx - L3_ECMP_DLB_GRP_MIN(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_config_get(unit, bcmi_dlb_type_ecmp, dlb_id, &cfg));
    }
    dgm = (cfg.flags & BCMI_LTSW_DLB_CF_DGM_ENABLED) ? true : false;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, false, &member,
                                      &pri_cnt, &egr_types));

    /*
     * Validate the egress objects:
     *  1. No underlay ECMP object as a member.
     *  2. No alternate path if without DGM enabled.
     */
    if ((egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) ||
        (!pri_cnt && !dgm)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*
     * Validate the egress objects:
     *  1. No alternate path if without DGM enabled.
     */
    if (!pri_cnt && !dgm) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    alt = pri_cnt ? false : true;

    SHR_IF_ERR_VERBOSE_EXIT
        (ulecmp_config_member_del(unit, idx, member, alt, cfg.num_paths,
                                  &member_idx));

    if (dlb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_dlb_member_del(unit, idx, member_idx));
    }

exit:
    if (member) {
        SHR_FREE(member);
    }
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
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_dlb_member_del_all(unit, idx));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ulecmp_config_member_del_all(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] ecmp_member_count Number of elements in emcp_member_array.
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
    ecmp_member_info_t *member = NULL;
    bool wgt, ecmp_added = false;


    SHR_FUNC_ENTER(unit);

    wgt = ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
           (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) ? true : false;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, ecmp_member_count,
                                      ecmp_member_array, false, &member,
                                      NULL, NULL));

    /* Only regular mode and random mode support member sort. */
    if (!(ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) && !wgt &&
        ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_member_sort(unit, member, ecmp_member_count,
                                  sizeof(ecmp_member_info_t)));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (olecmp_config_set(unit, idx, ecmp_info, ecmp_member_count, member));

    if (!(ecmp_info->flags & BCM_L3_REPLACE)) {
        ecmp_added = true;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_PATH_NO_SORTING) {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_PATH_NO_SORTING;
    } else {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_PATH_NO_SORTING;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED;
    } else {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_MEMBER_WEIGHTED;
    } else {
        L3_OL_ECMP_GRP_FLAGS(unit, idx) &= ~BCM_L3_ECMP_MEMBER_WEIGHTED;
    }

    L3_OL_ECMP_GRP_FLAGS(unit, idx) |= BCM_L3_ECMP_OVERLAY;

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        L3_OL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_REPLICATION;
        L3_OL_ECMP_GRP_MAX_PATHS(unit, idx) = ecmp_info->max_paths;
    } else if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        L3_OL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_WEIGHTED;
        L3_OL_ECMP_GRP_MAX_PATHS(unit, idx) = ecmp_info->max_paths;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        L3_OL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_RESILIENT;
        L3_OL_ECMP_GRP_MAX_PATHS(unit, idx) = ecmp_info->max_paths;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RANDOM) {
        L3_OL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_RANDOM;
    } else {
        L3_OL_ECMP_GRP_LB_MODE(unit, idx) = ECMP_LB_MODE_REGULAR;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (ecmp_added) {
            (void)olecmp_config_del(unit, idx);
        }
    }
    SHR_FREE(member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx ECMP index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_del(int unit, int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (olecmp_config_del(unit, idx));

    L3_OL_ECMP_GRP_FBMP_SET(unit, idx);
    L3_OL_ECMP_GRP_WGT_FBMP_SET(unit, idx);
    L3_OL_ECMP_GRP_FLAGS(unit, idx) = 0;
    L3_OL_ECMP_GRP_LB_MODE(unit, idx) = 0;
    L3_OL_ECMP_GRP_MAX_PATHS(unit, idx) = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an overlay ECMP group.
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
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (olecmp_config_get(unit, idx, ecmp_info, ecmp_member_size,
                           ecmp_member_array, ecmp_member_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] emcp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_member_add(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    ecmp_member_info_t *member = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, false, &member,
                                      NULL, NULL));
    SHR_IF_ERR_VERBOSE_EXIT
        (olecmp_config_member_add(unit, idx, member, 0, 0));

exit:
    if (member) {
        SHR_FREE(member);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] emcp_member Pointer to ECMP member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_member_del(int unit, int idx, bcm_l3_ecmp_member_t *ecmp_member)
{
    ecmp_member_info_t *member = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, 1, ecmp_member, false, &member,
                                      NULL, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (olecmp_config_member_del(unit, idx, member, 0, 0));

exit:
    if (member) {
        SHR_FREE(member);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all member from a overlay ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
olecmp_member_del_all(int unit, int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (olecmp_config_member_del_all(unit, idx));

exit:
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

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_type_init(unit, bcmi_dlb_type_ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));

    ei->ecmp_dlb_grp_offset = ei->ulecmp_grp_max - cap.id_max;
    ei->ecmp_dlb_grp_min = ei->ecmp_dlb_grp_offset + cap.id_min;

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
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
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_type_deinit(unit, bcmi_dlb_type_ecmp));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Clear the tables in L3 egress module.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_egress_tables_clear(int unit)
{
    int i, num;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    const char *ltname[] = {
        L3_UC_NHOPs,
        L3_MC_NHOPs,
        ECMP_CONTROLs,
        L3_UC_MTUs,
        L3_MC_MTUs,
        L3_UC_TNL_MTUs,
        TNL_MPLS_ENCAP_NHOPs,
        TNL_MPLS_TRANSIT_NHOPs
    };

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    num = COUNTOF(ltname);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }

    for (i = 0; i < BCMINT_LTSW_L3_ECMP_TBL_CNT; i++) {
        tbl_id = i;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
        if (tbl_info->name == NULL) {
            continue;
        }
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, tbl_info->name));
    }

exit:
    SHR_FUNC_EXIT();

    return 0;
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
    nh_cfg_t nh_cfg;
    ecmp_cfg_t ecmp_cfg;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    int rv = SHR_E_NONE;
    uint64_t *fv;
    uint64_t value;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    const char *ltname;

    SHR_FUNC_ENTER(unit);

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_nh_parse(unit, eh, tbl_info, &nh_cfg));
        fv = nh_cfg.flds;
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID];
        /* Don't recover from entry that not eligible for LT operation. */
        if ((value < L3_NHOP_MIN(unit)) ||
            (value > L3_NHOP_MAX(unit))) {
            continue;
        }

        L3_NHOP_FBMP_CLR(unit, value);
        L3_NHOP_TYPE(unit, value) = L3_UC_NHOP_TYPE;
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_nh_parse(unit, eh, tbl_info, &nh_cfg));

        fv = nh_cfg.flds;
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID];

        /* Don't recover from entry that not eligible for LT operation. */
        if ((value < L3_NHOP_MIN(unit)) ||
            (value > L3_NHOP_MAX(unit))) {
            continue;
        }
        L3_NHOP_FBMP_CLR(unit, value);
        L3_NHOP_TYPE(unit, value) = L3_MC_NHOP_TYPE;
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_nh_parse(unit, eh, tbl_info, &nh_cfg));

        fv = nh_cfg.flds;
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID];

        /* Don't recover from entry that not eligible for LT operation. */
        if ((value < L3_NHOP_MIN(unit)) ||
            (value > L3_NHOP_MAX(unit))) {
            continue;
        }
        L3_NHOP_FBMP_CLR(unit, value);
        L3_NHOP_TYPE(unit, value) = L3_MPLS_TRANSIT_NHOP_TYPE;
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_nh_parse(unit, eh, tbl_info, &nh_cfg));

        fv = nh_cfg.flds;
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID];

        /* Don't recover from entry that not eligible for LT operation. */
        if ((value < L3_NHOP_MIN(unit)) ||
            (value > L3_NHOP_MAX(unit))) {
            continue;
        }
        L3_NHOP_FBMP_CLR(unit, value);
        L3_NHOP_TYPE(unit, value) = L3_MPLS_ENCAP_NHOP_TYPE;
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_nh_parse(unit, eh, tbl_info, &nh_cfg));

        fv = nh_cfg.flds;
        value = fv[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID];

        /* Don't recover from entry that not eligible for LT operation. */
        if ((value < L3_NHOP_MIN(unit)) ||
            (value > L3_NHOP_MAX(unit))) {
            continue;
        }
        L3_NHOP_FBMP_CLR(unit, value);
        L3_NHOP_TYPE(unit, value) = L3_IFP_ACTION_NHOP_TYPE;
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp_cfg));

        /* Don't recover from entry that not eligible for LT operation. */
        if ((ecmp_cfg.ecmp_id < L3_UL_ECMP_GRP_MIN(unit)) ||
            (ecmp_cfg.ecmp_id > L3_UL_ECMP_GRP_MAX(unit))) {
            continue;
        }
        L3_UL_ECMP_GRP_FBMP_CLR(unit, ecmp_cfg.ecmp_id);
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp_cfg));

        /* Don't recover from entry that not eligible for LT operation. */
        if ((ecmp_cfg.ecmp_id < L3_UL_ECMP_GRP_MIN(unit)) ||
            (ecmp_cfg.ecmp_id > L3_UL_ECMP_GRP_MAX(unit))) {
            continue;
        }
        L3_UL_ECMP_GRP_FBMP_CLR(unit, ecmp_cfg.ecmp_id);
        L3_UL_ECMP_GRP_WGT_FBMP_CLR(unit, ecmp_cfg.ecmp_id);
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));

    eh = BCMLT_INVALID_HDL;

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;

        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

        ltname = tbl_info->name;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

            SHR_IF_ERR_VERBOSE_EXIT
                (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp_cfg));

            /* Don't recover from entry that not eligible for LT operation. */
            if ((ecmp_cfg.ecmp_id < L3_OL_ECMP_GRP_MIN(unit)) ||
                (ecmp_cfg.ecmp_id > L3_OL_ECMP_GRP_MAX(unit))) {
                continue;
            }
            L3_OL_ECMP_GRP_FBMP_CLR(unit, ecmp_cfg.ecmp_id);
        }

        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(eh));

        eh = BCMLT_INVALID_HDL;

        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;

        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

        ltname = tbl_info->name;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

            SHR_IF_ERR_VERBOSE_EXIT
                (lt_ecmp_level_parse(unit, eh, tbl_info, &ecmp_cfg));

            /* Don't recover from entry that not eligible for LT operation. */
            if ((ecmp_cfg.ecmp_id < L3_OL_ECMP_GRP_MIN(unit)) ||
                (ecmp_cfg.ecmp_id > L3_OL_ECMP_GRP_MAX(unit))) {
                continue;
            }
            L3_OL_ECMP_GRP_FBMP_CLR(unit, ecmp_cfg.ecmp_id);
            L3_OL_ECMP_GRP_WGT_FBMP_CLR(unit, ecmp_cfg.ecmp_id);
        }

        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(eh));

        eh = BCMLT_INVALID_HDL;

    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Special init of L3 nexthop table.
 *
 * This function is to init entry 0 of ING_L3_NEXT_HOP_1/2 table and
 * associated strength profile table for L2 flow.
 * This is a S/W WAR for SDKLT-11311 and shall be removed once it is fixed.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
l3_nexthop_table_special_init(int unit)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field1[] =
    {
        /*0*/ {NHOP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field1;
    lt_entry.nfields = sizeof(field1)/sizeof(field1[0]);
    lt_entry.attr = 0;
    field1[0].u.val = 0;
    /* Insert entry 0 of L3_UC_NHOP. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, L3_UC_NHOPs, &lt_entry, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init num of Overlay intf/nexthop/ecmp member.
 *
 * This function is to init entries for overlay intf/nexthop/ecmp member.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
l3_overlay_table_init(int unit)
{
    int ol_ecmp_member_num = 0, ol_egr_obj_num = 0;
    bcmint_ltsw_l3_ol_tbl_info_t info;

    SHR_FUNC_ENTER(unit);

    ol_egr_obj_num = bcmi_ltsw_property_get(unit, BCMI_CPN_RIOT_OVERLAY_L3_EGRESS_MEM_SIZE, 0);
    ol_ecmp_member_num = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ECMP_MEMBER_FIRST_LKUP_MEM_SIZE, 0);

    info.ol_ecmp_member_num = ol_ecmp_member_num;
    info.ol_egr_obj_num = ol_egr_obj_num;
    info.ol_eif_num = -1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_overlay_table_init(unit, &info));

    L3_OL_NHOP_ENT(unit) = ol_egr_obj_num;
    L3_OL_ECMP_MEMBER_ENT(unit) = ol_ecmp_member_num;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
xgs_ltsw_l3_egress_init(int unit)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    uint32_t sz, nbits, width;
    uint32_t alloc_sz;
    uint64_t min, max;
    bcmi_ltsw_profile_hdl_t profile_hdl;
    int ent_idx_min, ent_idx_max;
    uint64_t idx_min, idx_max;
    int i;
    const char *lt_name;
    const char *lt_key_name;
    const char *lt_name_w;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (ei->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_egress_deinit(unit));
    }

    sal_memset(ei, 0, sizeof(l3_egr_info_t));

    ei->mutex = sal_mutex_create("bcmXgsl3EgrInfo");
    SHR_NULL_CHECK(ei->mutex, SHR_E_MEMORY);

    /* UC NHOP. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_UC_NHOPs,
                                       NHOP_IDs, &min, &max));
    
    ei->nh_min = min ? min : 1;
    ei->nh_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    SHR_ALLOC(ei->nh_fbmp, sz, "bcmXgsl3EgrNhFbmp");
    SHR_NULL_CHECK(ei->nh_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(ei->nh_fbmp, 0, L3_NHOP_MIN(unit));
    nbits = ei->nh_max - ei->nh_min + 1;
    SHR_BITSET_RANGE(ei->nh_fbmp, L3_NHOP_MIN(unit), nbits);

    sz = (max + 1) * sizeof(nh_entry_info_t);
    SHR_ALLOC(ei->nh_ei, sz, "bcmXgsl3EgrNhInfo");
    SHR_NULL_CHECK(ei->nh_ei, SHR_E_MEMORY);
    sal_memset(ei->nh_ei, 0, sz);

    tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
    lt_name = tbl_info->name;

    tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL;
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
    lt_name_w = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name, ECMP_IDs,
                                       &min, &max));

    
    /* Group free bitmap. */
    ei->ulecmp_grp_min = min ? min : 1;
    ei->ulecmp_grp_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    SHR_ALLOC(ei->ulecmp_grp_fbmp, sz, "bcmXgsUlEcmpFbmp");
    SHR_NULL_CHECK(ei->ulecmp_grp_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(ei->ulecmp_grp_fbmp, 0, ei->ulecmp_grp_min);
    nbits = ei->ulecmp_grp_max - ei->ulecmp_grp_min + 1;
    SHR_BITSET_RANGE(ei->ulecmp_grp_fbmp, ei->ulecmp_grp_min, nbits);

    /* Group flags. */
    sz = (max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    ei->ulecmp_grp_flags = bcmi_ltsw_ha_mem_alloc(unit,
                                                  BCMI_HA_COMP_ID_L3_EGR,
                                                  BCMINT_LTSW_L3_EGR_HA_UL_ECMP_FLAGS,
                                                  "bcmL3UlEcmpFlags",
                                                  &alloc_sz);
    SHR_NULL_CHECK(ei->ulecmp_grp_flags, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    /* ECMP weighted group. */
    
    sz = SHR_BITALLOCSIZE(max + 1);
    SHR_ALLOC(ei->ulecmp_grp_weighted_fbmp, sz, "bcmXgsUlEcmpWeightedFbmp");
    SHR_NULL_CHECK(ei->ulecmp_grp_weighted_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(ei->ulecmp_grp_weighted_fbmp, 0, ei->ulecmp_grp_min);
    nbits = ei->ulecmp_grp_max - ei->ulecmp_grp_min + 1;
    SHR_BITSET_RANGE(ei->ulecmp_grp_weighted_fbmp, ei->ulecmp_grp_min, nbits);

    /* ECMP lb mode. */
    sz = (max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    ei->ulecmp_grp_lb_mode = bcmi_ltsw_ha_mem_alloc(unit,
                                                    BCMI_HA_COMP_ID_L3_EGR,
                                                    BCMINT_LTSW_L3_EGR_HA_UL_ECMP_LB_MODE,
                                                    "bcmL3UlEcmpLbMode",
                                                    &alloc_sz);
    SHR_NULL_CHECK(ei->ulecmp_grp_lb_mode, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    /* ECMP max paths. */
    sz = (max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    ei->ulecmp_grp_max_paths = bcmi_ltsw_ha_mem_alloc(unit,
                                                      BCMI_HA_COMP_ID_L3_EGR,
                                                      BCMINT_LTSW_L3_EGR_HA_UL_RH_ECMP_MAX_PATHS,
                                                      "bcmL3UlEcmpRhMaxPaths",
                                                      &alloc_sz);
    SHR_NULL_CHECK(ei->ulecmp_grp_max_paths, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    /* Max paths */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name, MAX_PATHSs,
                                       &min, &max));
    ei->ecmp_path_max = max;

    /* Max RH paths */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name, RH_NUM_PATHSs,
                                       &min, &max));
    ei->ecmp_rh_path_max = max;

    /* Max paths of weighted ECMP. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name_w, NUM_PATHSs,
                                       &min, &max));
    ei->wgt_ecmp_path_max = max;

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
        lt_name = tbl_info->name;

        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
        lt_name_w = tbl_info->name;

        /* Overlay ECMP Max paths */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, lt_name, MAX_PATHSs,
                                           &min, &max));
        ei->ol_ecmp_path_max = max;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, lt_name, ECMP_IDs,
                                           &min, &max));

        /* Group free bitmap. */
        ei->olecmp_grp_min = min;
        ei->olecmp_grp_max = max;
        sz = SHR_BITALLOCSIZE(max + 1);
        SHR_ALLOC(ei->olecmp_grp_fbmp, sz, "bcmXgsOlEcmpFbmp");
        SHR_NULL_CHECK(ei->olecmp_grp_fbmp, SHR_E_MEMORY);
        nbits = ei->olecmp_grp_max - ei->olecmp_grp_min + 1;
        SHR_BITSET_RANGE(ei->olecmp_grp_fbmp, ei->olecmp_grp_min, nbits);

        /* Group flags. */
        sz = (max + 1) * sizeof(uint32_t);
        alloc_sz = sz;
        ei->olecmp_grp_flags = bcmi_ltsw_ha_mem_alloc(unit,
                                                      BCMI_HA_COMP_ID_L3_EGR,
                                                      BCMINT_LTSW_L3_EGR_HA_OL_ECMP_FLAGS,
                                                      "bcmL3OlEcmpFlags",
                                                      &alloc_sz);
        SHR_NULL_CHECK(ei->olecmp_grp_flags, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

        /* ECMP weighted group. */
        sz = SHR_BITALLOCSIZE(max + 1);
        SHR_ALLOC(ei->olecmp_grp_weighted_fbmp, sz, "bcmXgsOlEcmpWeightedFbmp");
        SHR_NULL_CHECK(ei->olecmp_grp_weighted_fbmp, SHR_E_MEMORY);
        SHR_BITCLR_RANGE(ei->olecmp_grp_weighted_fbmp, 0, ei->olecmp_grp_min);
        nbits = ei->olecmp_grp_max - ei->olecmp_grp_min + 1;
        SHR_BITSET_RANGE(ei->olecmp_grp_weighted_fbmp, ei->olecmp_grp_min, nbits);

        /* ECMP lb mode. */
        sz = (max + 1) * sizeof(uint32_t);
        alloc_sz = sz;
        ei->olecmp_grp_lb_mode = bcmi_ltsw_ha_mem_alloc(unit,
                                                        BCMI_HA_COMP_ID_L3_EGR,
                                                        BCMINT_LTSW_L3_EGR_HA_OL_ECMP_LB_MODE,
                                                        "bcmL3OlEcmpLbMode",
                                                        &alloc_sz);
        SHR_NULL_CHECK(ei->olecmp_grp_lb_mode, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

        /* ECMP max paths. */
        sz = (max + 1) * sizeof(uint32_t);
        alloc_sz = sz;
        ei->olecmp_grp_max_paths = bcmi_ltsw_ha_mem_alloc(unit,
                                                          BCMI_HA_COMP_ID_L3_EGR,
                                                          BCMINT_LTSW_L3_EGR_HA_OL_RH_ECMP_MAX_PATHS,
                                                          "bcmL3OlEcmpRhMaxPaths",
                                                          &alloc_sz);
        SHR_NULL_CHECK(ei->olecmp_grp_max_paths, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

        /* Max paths of weighted ECMP. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, lt_name_w, NUM_PATHSs,
                                           &min, &max));
        ei->wgt_ol_ecmp_path_max = max;
    }

    /* Maximum number of entries for port based hash output selection instance. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs,
                              &sz));
    ei->ecmp_hash_flow_sel_ent_num = sz;

    /* Maximum number of entries for flow based hash output selection instance. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, LB_HASH_PORT_L3_ECMP_OUTPUT_SELECTIONs,
                              &sz));
    ei->ecmp_hash_port_sel_ent_num = sz;

    /* Width of next hop index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_width_get(unit, L3_UC_NHOPs, NHOP_IDs, &width));
    ei->nhidx_width = width;

    /* L3MC NHOP FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_MC_NHOP_CTR_EGR_EFLEX_ACTIONs,
                                       L3_MC_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_L3MC_NHOP_FLEX_ACTION;
    if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, profile_hdl));
    }

    /* Reserve profile 0 for non-flex counter use. */
    ent_idx_min = (int)idx_min + 1;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    l3_egress_flex_action_profile_entry_hash_cb,
                                    l3_mc_nh_flex_action_profile_entry_cmp_cb));
    /***************************************************************************/

    /* L3UC NHOP FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_UC_NHOP_CTR_EGR_EFLEX_ACTIONs,
                                       L3_UC_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_L3UC_NHOP_FLEX_ACTION;
    if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, profile_hdl));
    }

    /* Reserve profile 0 for non-flex counter use. */
    ent_idx_min = (int)idx_min + 1;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    l3_egress_flex_action_profile_entry_hash_cb,
                                    l3_uc_nh_flex_action_profile_entry_cmp_cb));
    /***************************************************************************/

    /* MPLS nhop FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTIONs,
                                       TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_MPLS_NHOP_FLEX_ACTION;
    if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, profile_hdl));
    }

    /* Reserve profile 0 for non-flex counter use. */
    ent_idx_min = (int)idx_min + 1;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    l3_egress_flex_action_profile_entry_hash_cb,
                                    l3_mpls_nh_flex_action_profile_entry_cmp_cb));

    /***************************************************************************/
    tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL;
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));
    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID].fld_name;

    /* ECMP_CTR_ING_EFLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name,
                                       lt_key_name,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION;
    if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, profile_hdl));
    }

    /* Reserve profile 0 for non-flex counter use. */
    ent_idx_min = (int)idx_min + 1;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    ecmp_flex_action_profile_entry_hash_cb,
                                    ulecmp_flex_action_profile_entry_cmp_cb));

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
            tbl_id = BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL;
            SHR_IF_ERR_EXIT
                (bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(unit, tbl_id, &tbl_info));
            lt_name = tbl_info->name;
            lt_key_name = tbl_info->flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID].fld_name;
        /* ECMP_OVERLAY_CTR_ING_EFLEX_ACTION profile. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_value_range_get(unit, lt_name,
                                               lt_key_name,
                                               &idx_min, &idx_max));

        profile_hdl = BCMI_LTSW_PROFILE_L3_OL_ECMP_FLEX_ACTION;
        if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_unregister(unit, profile_hdl));
        }

        /* Reserve profile 0 for non-flex counter use. */
        ent_idx_min = (int)idx_min + 1;
        ent_idx_max = (int)idx_max;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                        &ent_idx_min, &ent_idx_max, 1,
                                        ecmp_flex_action_profile_entry_hash_cb,
                                        olecmp_flex_action_profile_entry_cmp_cb));
    }

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_recover(unit));

        for (i = 0; i < BCMINT_LTSW_L3_EGRESS_FLEXCTR_ACTION_CNT; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_egress_flex_action_profile_recover(unit, i));
        }

        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ecmp_flex_action_profile_recover(unit, false));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_flex_action_profile_recover(unit, true));
    } else {

        sz = (ei->ulecmp_grp_max + 1) * sizeof(uint32_t);
        sal_memset(ei->ulecmp_grp_flags, 0, sz);
        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
            sz = (ei->olecmp_grp_max + 1) * sizeof(uint32_t);
            sal_memset(ei->olecmp_grp_flags, 0, sz);
        }

        /*
         * Don't return error even if the special init fails in order
         * to let other module init and system can boot up.
         * This is just a S/W WAR for L3 flow.
         */
        (void)l3_nexthop_table_special_init(unit);
    }

    if (ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ecmp_dlb_init(unit));
    }

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_overlay_table_init(unit));
    }

    ei->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(ei->nh_fbmp);
        SHR_FREE(ei->nh_ei);
        SHR_FREE(ei->ulecmp_grp_fbmp);
        SHR_FREE(ei->ulecmp_grp_weighted_fbmp);
        SHR_FREE(ei->olecmp_grp_fbmp);
        SHR_FREE(ei->olecmp_grp_weighted_fbmp);
        if (ei->mutex) {
            sal_mutex_destroy(ei->mutex);
        }
        profile_hdl = BCMI_LTSW_PROFILE_L3MC_NHOP_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_L3UC_NHOP_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_MPLS_NHOP_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_L3_OL_ECMP_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        if (!bcmi_warmboot_get(unit)) {
            if (ei->olecmp_grp_flags) {
                (void)bcmi_ltsw_ha_mem_free(unit, ei->olecmp_grp_flags);
            }
            if (ei->olecmp_grp_lb_mode) {
                (void)bcmi_ltsw_ha_mem_free(unit, ei->olecmp_grp_lb_mode);
            }
            if (ei->ulecmp_grp_flags) {
                (void)bcmi_ltsw_ha_mem_free(unit, ei->ulecmp_grp_flags);
            }
            if (ei->ulecmp_grp_lb_mode) {
                (void)bcmi_ltsw_ha_mem_free(unit, ei->ulecmp_grp_lb_mode);
            }
        }
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_deinit(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl;
    l3_egr_info_t *ei = &l3_egr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!ei->inited || !ei->mutex) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(ei->mutex, SAL_MUTEX_FOREVER);

    ei->inited = 0;

    SHR_IF_ERR_EXIT
        (l3_ecmp_dlb_deinit(unit));

    SHR_IF_ERR_EXIT
        (l3_egress_tables_clear(unit));

    SHR_IF_ERR_EXIT
        (l3_egress_flex_action_profile_deinit(unit));

    SHR_IF_ERR_EXIT
        (ecmp_flex_action_profile_deinit(unit));

    profile_hdl = BCMI_LTSW_PROFILE_L3MC_NHOP_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_L3UC_NHOP_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_MPLS_NHOP_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_L3_ECMP_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    SHR_FREE(ei->nh_fbmp);
    SHR_FREE(ei->nh_ei);
    SHR_FREE(ei->ulecmp_grp_fbmp);
    SHR_FREE(ei->ulecmp_grp_weighted_fbmp);
    SHR_FREE(ei->olecmp_grp_fbmp);
    SHR_FREE(ei->olecmp_grp_weighted_fbmp);

    sal_mutex_give(ei->mutex);

    sal_mutex_destroy(ei->mutex);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_id_range_get(int unit, int type, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    switch (type) {
    case BCMI_LTSW_L3_EGR_OBJ_T_OL:
        *min = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_UL_NHOP_ENT(unit);
        *max = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_NHOP_MAX(unit);
        break;
    case BCMI_LTSW_L3_EGR_OBJ_T_UL:
        *min = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_NHOP_MIN(unit);
        *max = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_UL_NHOP_ENT(unit) - 1;
        break;
    case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL:
            *min = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
                   L3_UL_ECMP_GRP_MIN(unit);
            *max = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
                   L3_UL_ECMP_GRP_MAX(unit);
        break;
    case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL:
        *min = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
               L3_OL_ECMP_GRP_MIN(unit);
        *max = BCMI_LTSW_L3_EGR_OBJ_BASE + L3_EGR_OBJ_SCALE +
               L3_OL_ECMP_GRP_MAX(unit);
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_obj_id_resolve(int unit, bcm_if_t intf, int *nh_ecmp_idx,
                                  bcmi_ltsw_l3_egr_obj_type_t *type)
{
    bcmi_ltsw_l3_egr_obj_type_t i;
    int min = 0;
    int max = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    for (i = 0; i < BCMI_LTSW_L3_EGR_OBJ_T_CNT; i++) {
        (void)xgs_ltsw_l3_egress_id_range_get(unit, i, &min, &max);
        if ((intf >= min) && (intf <= max)) {
            break;
        }
    }
    if ( i >= BCMI_LTSW_L3_EGR_OBJ_T_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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
xgs_ltsw_l3_egress_obj_id_construct(int unit, int nh_ecmp_idx,
                                    bcmi_ltsw_l3_egr_obj_type_t type,
                                    bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) &&
         (nh_ecmp_idx >= L3_NHOP_MIN(unit)) &&
         (nh_ecmp_idx < L3_UL_NHOP_ENT(unit))) ||
         ((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
         (nh_ecmp_idx >= L3_UL_NHOP_ENT(unit)) &&
         (nh_ecmp_idx <= L3_NHOP_MAX(unit))) ||
         ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
         (nh_ecmp_idx >= L3_OL_ECMP_GRP_MIN(unit)) &&
         (nh_ecmp_idx <= L3_OL_ECMP_GRP_MAX(unit))) ||
        ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) &&
         (nh_ecmp_idx >= L3_UL_ECMP_GRP_MIN(unit)) &&
         (nh_ecmp_idx <= L3_UL_ECMP_GRP_MAX(unit)))) {
        *intf = nh_ecmp_idx + BCMI_LTSW_L3_EGR_OBJ_BASE +
                (type / 2 * L3_EGR_OBJ_SCALE);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
                                int *max, int *used)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int max_cnt = 0, free_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    switch (type) {
        case BCMI_LTSW_L3_EGR_OBJ_T_OL:
            if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
                break;
            }
            max_cnt = L3_OL_NHOP_ENT(unit);
            SHR_BITCOUNT_RANGE(ei->nh_fbmp, free_cnt, L3_UL_NHOP_ENT(unit), max_cnt);
            break;
        case BCMI_LTSW_L3_EGR_OBJ_T_UL:
            max_cnt = L3_UL_NHOP_ENT(unit);
            SHR_BITCOUNT_RANGE(ei->nh_fbmp, free_cnt, 0, max_cnt);
            break;
        case BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL:
            if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
                break;
            }
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
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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
xgs_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp)
{
    int dunit, nhidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname;
    const char *keyname;
    bool locked = false;
    int nhop_type;

    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        SHR_EXIT();
    }

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_EGR_LOCK(unit);
    locked = true;



    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &nhidx, &type));

    if (L3_NHOP_FBMP_GET(unit, nhidx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    nhop_type = L3_NHOP_TYPE(unit, nhidx);

    dunit = bcmi_ltsw_dev_dunit(unit);

    switch (nhop_type) {
        case L3_UC_NHOP_TYPE:
            ltname = L3_UC_NHOPs;
            break;
        case L3_MC_NHOP_TYPE:
            ltname = L3_MC_NHOPs;
            break;
        case L3_IFP_ACTION_NHOP_TYPE:
            ltname = FP_ING_ACTION_NHOPs;
            break;
        case L3_MPLS_ENCAP_NHOP_TYPE:
            ltname = TNL_MPLS_ENCAP_NHOPs;
            break;
        case L3_MPLS_TRANSIT_NHOP_TYPE:
            ltname = TNL_MPLS_TRANSIT_NHOPs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    keyname = NHOP_IDs;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, keyname, nhidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, DVPs, dvp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_create(int unit, bcm_l3_egress_t *egr, int *idx)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int nhidx, max, min;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint16_t prot_offset;
    SHR_BITDCL *fbmp;
    bool ul = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!egr || !idx) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_failover_prot_offset_get(unit, &prot_offset));

    if (!bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port)) {
        ul = true;
    }

    fbmp = ei->nh_fbmp;
    /* Underlay nexthop always located before overlay nexthop. */
    max = prot_offset ? (prot_offset - 1) :
                        (L3_OL_NHOP_ENT(unit) ? (ul ? (L3_UL_NHOP_ENT(unit) - 1) :
                                                      L3_NHOP_MAX(unit)):
                                                L3_NHOP_MAX(unit));
    min = ul ? 0 : L3_UL_NHOP_ENT(unit);

    L3_EGR_LOCK(unit);
    locked = true;

    if (egr->flags & BCM_L3_WITH_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_obj_id_to_ltidx(unit, *idx, &nhidx, &type));

        if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ^ ul) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!SHR_BITGET(fbmp, nhidx) && !(egr->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }

        if (SHR_BITGET(fbmp, nhidx)) {
            egr->flags &= ~ BCM_L3_REPLACE;
        }

        /* When Failover with fixed egress nh offset is enable,
         * egr->failover_id is used internally to distinguish between user
         * created primary intf (should be zero) and prot_intf created from
         * bcm_failover_egress_set (BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON)
         */
        if (prot_offset > 0) {
            if (!(egr->failover_id & BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON) &&
                (nhidx >= prot_offset)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            egr->failover_id &= ~BCMI_LTSW_FAILOVER_FIXED_NH_OFFSET_ON;
        }
    } else {
        egr->flags &= ~ BCM_L3_REPLACE;

        /* Get the first free index. */
        SHR_BIT_ITER_RANGE(fbmp, min, (max + 1), nhidx) {
            break;
        }

        if (nhidx > max) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (nh_set(unit, ul, nhidx, egr));

    if (!(egr->flags & BCM_L3_WITH_ID)) {
        type = ul ? BCMI_LTSW_L3_EGR_OBJ_T_UL : BCMI_LTSW_L3_EGR_OBJ_T_OL;
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx, type, idx));
    }

    L3_NHOP_FBMP_CLR(unit, nhidx);

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_egress_destroy(int unit, int idx)
{
    int nhidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, idx, &nhidx, &type));

    if (L3_NHOP_FBMP_GET(unit, nhidx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    L3_EGR_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT(nh_delete(unit, nhidx));

    L3_NHOP_FBMP_SET(unit, nhidx);
exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_egress_get(int unit, int idx, bcm_l3_egress_t *egr)
{
    int nhidx = 0;
    bcmi_ltsw_l3_egr_obj_type_t type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, idx, &nhidx, &type));

    if (L3_NHOP_FBMP_GET(unit, nhidx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT(nh_get(unit, nhidx, egr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, int *idx)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int nhidx, min, max;
    SHR_BITDCL *fbmp;
    bcm_l3_egress_t int_egr;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_trunk_t tid;
    int id;
    bcmi_ltsw_l3_egr_obj_type_t type;

    SHR_FUNC_ENTER(unit);

    fbmp = ei->nh_fbmp;
    min = ei->nh_min;
    max = ei->nh_max;

    for (nhidx = min; nhidx <= max; nhidx ++) {
        if (SHR_BITGET(fbmp, nhidx)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (nh_get(unit, nhidx, &int_egr));
        if (egr->intf != int_egr.intf) {
            continue;
        }

        if (sal_memcmp(egr->mac_addr, int_egr.mac_addr, sizeof(bcm_mac_t))) {
            continue;
        }
        if (egr->flags & BCM_L3_TGID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, int_egr.trunk,
                                              &modid, &port,
                                              &tid, &id));
            if (tid != egr->trunk) {
                continue;
            }
        } else {
            if (BCM_GPORT_IS_SET(int_egr.port)) {
                SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, int_egr.port,
                                              &modid, &port,
                                              &tid, &id));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                     (bcmi_ltsw_port_gport_validate(unit, int_egr.port, &port));
            }

            if (port != egr->port) {
                continue;
            }
        }
        break;
    }

    if (nhidx > max ) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    type = (nhidx >= L3_UL_NHOP_ENT(unit)) ? BCMI_LTSW_L3_EGR_OBJ_T_OL :
                                             BCMI_LTSW_L3_EGR_OBJ_T_UL;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_ltidx_to_obj_id(unit, nhidx, type, idx));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_traverse(int unit,
                            bcm_l3_egress_traverse_cb trav_fn,
                            void *user_data)
{
    bcm_l3_egress_t egr;
    bcm_if_t intf;
    int nhidx, rv;
    bool locked = false;
    bcmi_ltsw_l3_egr_obj_type_t type;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    L3_EGR_LOCK(unit);
    locked = true;

    for (nhidx = L3_NHOP_MIN(unit); nhidx <= L3_NHOP_MAX(unit);
         nhidx ++) {
        /* Skip unused entries. */
        if (L3_NHOP_FBMP_GET(unit, nhidx)) {
            continue;
        }

        bcm_l3_egress_t_init(&egr);

        SHR_IF_ERR_VERBOSE_EXIT
            (nh_get(unit, nhidx, &egr));

        type = (nhidx >= L3_UL_NHOP_ENT(unit)) ? BCMI_LTSW_L3_EGR_OBJ_T_OL :
                                                 BCMI_LTSW_L3_EGR_OBJ_T_UL;
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_ltidx_to_obj_id(unit, nhidx,
                                       type,
                                       &intf));

        rv = trav_fn(unit, intf, &egr, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_flexctr_attach(int unit, bcm_if_t intf_id,
                                  bcmi_ltsw_flexctr_counter_info_t *info)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_ACTION;
    flexctr.action = info->action_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_flexctr_detach(int unit, bcm_if_t intf_id)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_ACTION;
    flexctr.action = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_flexctr_info_get(int unit, bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname, *fldname, *keyname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t val;
    int nhop_type;
    l3_egress_flex_action_profile_t action_profile;
    l3_egress_flexctr_action_t action_type;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    const bcmint_ltsw_l3_egress_fld_t *fi = NULL;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf_id, &ltidx, &type));

    if (L3_NHOP_FBMP_GET(unit, ltidx)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    info->source = bcmFlexctrSourceL3EgressUnderlay;

    nhop_type = L3_NHOP_TYPE(unit, ltidx);

    switch (nhop_type) {
        case L3_UC_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;
            action_type = BCMINT_LTSW_L3UC_NHOP_FLEXCTR_ACTION;
            break;
        case L3_MC_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;
            action_type = BCMINT_LTSW_L3MC_NHOP_FLEXCTR_ACTION;
            break;
        case L3_MPLS_ENCAP_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP;
            action_type = BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION;
            break;
        case L3_MPLS_TRANSIT_NHOP_TYPE:
            tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;
            action_type = BCMINT_LTSW_MPLS_NHOP_FLEXCTR_ACTION;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));
    fi = tbl_info->flds;

    ltname = tbl_info->name;
    fldname = fi[BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID].fld_name;

    keyname = NHOP_IDs;

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, keyname, ltidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, fldname, &val));

    if (val == 0) {
        info->action_index = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_flex_action_profile_get(unit,
                                               action_type,
                                               val,
                                               &action_profile));
        info->action_index = action_profile.action;
    }
    info->stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;
    info->table_name = ltname;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_INDEX;
    flexctr.index = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_GET;
    flexctr.flags = L3_EGR_FLXCTR_F_INDEX;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_flexctr_op(unit, intf_id, &flexctr));

    *value = flexctr.index;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_create(int unit, bcm_l3_egress_ecmp_t *ecmp_info,
                        int ecmp_member_count,
                        bcm_l3_ecmp_member_t *ecmp_member_array)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int idx, min, max;
    bcmi_ltsw_l3_egr_obj_type_t type;
    SHR_BITDCL *fbmp, *wgt_fbmp;
    uint32_t *flags;
    bool locked = false, ul = false, dlb_en = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_info || !ecmp_member_array) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL) ||
        (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) ||
        (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL) ||
        (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM)) {
        if (!ltsw_feature(unit, LTSW_FT_DLB)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        dlb_en = true;
    }

    if ((ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) &&
        (ecmp_member_count > L3_ECMP_RH_MAX_PATHS(unit))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    ul = (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_OVERLAY) ? false : true;

    if (dlb_en && !ul) {
        /* A DLB group can only be programmed at ECMP Level 2 */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!ul &&
        (ecmp_info->ecmp_group_flags &
         BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED)) {
         /* Overlay ECMP do not support  replication WECMP. */
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fbmp = ul ? ei->ulecmp_grp_fbmp : ei->olecmp_grp_fbmp;
    wgt_fbmp = ul ? ei->ulecmp_grp_weighted_fbmp :
                    ei->olecmp_grp_weighted_fbmp;
    flags = ul ? ei->ulecmp_grp_flags : ei->olecmp_grp_flags;
    min = ul ? ei->ulecmp_grp_min : ei->olecmp_grp_min;
    max = ul ? ei->ulecmp_grp_max : ei->olecmp_grp_max;

    L3_EGR_LOCK(unit);
    locked = true;

    if (ecmp_info->flags & BCM_L3_WITH_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_obj_id_to_ltidx(unit, ecmp_info->ecmp_intf, &idx,
                                       &type));

        if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) ^ ul) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!SHR_BITGET(fbmp, idx) && !(ecmp_info->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }

        if (dlb_en && (idx < L3_ECMP_DLB_GRP_MIN(unit))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (ecmp_info->flags & BCM_L3_REPLACE) {
            if (SHR_BITGET(fbmp, idx)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }

            /*
                         * ECMP & ECMP_WEIGHTED share same PT, thus need
                         * to free the index from one LT before inserting it to another LT.
                         */
            if (((flags[idx] ^ ecmp_info->ecmp_group_flags) &
                BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
                (flags[idx] ^ ecmp_info->ecmp_group_flags) &
                BCM_L3_ECMP_MEMBER_WEIGHTED) {
                if (ul) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (ulecmp_del(unit, idx));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (olecmp_del(unit, idx));
                }
                ecmp_info->flags &= ~BCM_L3_REPLACE;
            }
            /* When LB mode chang from/to RESILIENT, need to clear the fields first. */
            if (ul) {
                if (((L3_UL_ECMP_GRP_LB_MODE(unit, idx) != ECMP_LB_MODE_RESILIENT) &&
                    (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT == ecmp_info->dynamic_mode)) ||
                    ((L3_UL_ECMP_GRP_LB_MODE(unit, idx) == ECMP_LB_MODE_RESILIENT) &&
                    (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT != ecmp_info->dynamic_mode))) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (ulecmp_del(unit, idx));
                        ecmp_info->flags &= ~BCM_L3_REPLACE;
                }

            } else {
                if (((L3_OL_ECMP_GRP_LB_MODE(unit, idx) != ECMP_LB_MODE_RESILIENT) &&
                    (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT == ecmp_info->dynamic_mode)) ||
                    ((L3_OL_ECMP_GRP_LB_MODE(unit, idx) == ECMP_LB_MODE_RESILIENT) &&
                    (BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT != ecmp_info->dynamic_mode))) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (olecmp_del(unit, idx));
                        ecmp_info->flags &= ~BCM_L3_REPLACE;
                }
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
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
    }

    if (ul) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_set(unit, idx, ecmp_info, ecmp_member_count,
                        ecmp_member_array, dlb_en));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (olecmp_set(unit, idx, ecmp_info, ecmp_member_count,
                        ecmp_member_array));
    }

    if (!(ecmp_info->flags & BCM_L3_WITH_ID)) {
        type = ul ? BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL :
                    BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL;
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_ltidx_to_obj_id(unit, idx, type, &ecmp_info->ecmp_intf));
    }

    SHR_BITCLR(fbmp, idx);
    if ((ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        SHR_BITCLR(wgt_fbmp, idx);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ecmp_destroy(int unit, bcm_if_t ecmp_group_id)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_del(unit, ltidx));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (olecmp_del(unit, ltidx));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_get(int unit, bcm_l3_egress_ecmp_t *ecmp_info,
                     int ecmp_member_size,
                     bcm_l3_ecmp_member_t *ecmp_member_array,
                     int *ecmp_member_count)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_info || !ecmp_member_count) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_info->ecmp_intf, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_get(unit, ltidx, ecmp_info, ecmp_member_size,
                        ecmp_member_array, ecmp_member_count));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (olecmp_get(unit, ltidx, ecmp_info, ecmp_member_size,
                        ecmp_member_array, ecmp_member_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_member_add(int unit, bcm_if_t ecmp_group_id,
                            bcm_l3_ecmp_member_t *ecmp_member)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;
    bcm_l3_egress_ecmp_t ecmp_info;
    int ecmp_member_count = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ecmp_member, SHR_E_PARAM);

    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_intf = ecmp_group_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_l3_ecmp_get(unit, &ecmp_info, 0, NULL, &ecmp_member_count));

    if (ecmp_info.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        if (ecmp_member_count == L3_ECMP_RH_MAX_PATHS(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
    }

    if (ecmp_member_count == ecmp_info.max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't add member to weighted ECMP group. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_member_add(unit, ltidx, ecmp_member));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't add member to weighted ECMP group. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (olecmp_member_add(unit, ltidx, ecmp_member));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ecmp_member_del(int unit, bcm_if_t ecmp_group_id,
                            bcm_l3_ecmp_member_t *ecmp_member)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ecmp_member, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_member_del(unit, ltidx, ecmp_member));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (olecmp_member_del(unit, ltidx, ecmp_member));
    }else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ecmp_member_del_all(int unit, bcm_if_t ecmp_group_id)
{
    int ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    L3_EGR_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_UL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_UL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_member_del_all(unit, ltidx));
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        if (L3_OL_ECMP_GRP_FLAGS(unit, ltidx) & BCM_L3_ECMP_WEIGHTED) {
            /* Can't delete member from weighted ECMP group. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (olecmp_member_del_all(unit, ltidx));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ecmp_find(int unit, int ecmp_member_count,
                      bcm_l3_ecmp_member_t *ecmp_member_array,
                      bcm_l3_egress_ecmp_t *ecmp_info)
{
    ecmp_member_info_t *emi_in = NULL;
    int pri_cnt, alt_cnt;
    bool locked = false, ul_skip = false;
    bool dgm_en = false;
    uint32_t dlb_ft, egr_types;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_member_count || !ecmp_member_array || !ecmp_info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_member_info_array_alloc(unit, ecmp_member_count,
                                      ecmp_member_array, true, &emi_in,
                                      &pri_cnt, &egr_types));

    if (pri_cnt < ecmp_member_count) {
        if (!ltsw_feature(unit, LTSW_FT_DLB)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_feature_get(unit, &dlb_ft));
        if (!(dlb_ft & BCMI_LTSW_DLB_F_ECMP_DGM)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        dgm_en = true;
    }

    if (egr_types & (1 << BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        ul_skip = true;
    }

    if (dgm_en && ul_skip) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    alt_cnt = ecmp_member_count - pri_cnt;

    /* Sort primary paths. */
    if (pri_cnt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_member_sort(unit, emi_in, pri_cnt,
                              sizeof(ecmp_member_info_t)));
    }
    /* Sort alternate paths. */
    if (alt_cnt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_member_sort(unit, &emi_in[pri_cnt], alt_cnt,
                              sizeof(ecmp_member_info_t)));
    }

    L3_EGR_LOCK(unit);
    locked = true;

    if (!dgm_en && !ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        rv = olecmp_config_find(unit, ecmp_member_count, emi_in, ecmp_info);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    if (ul_skip) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ulecmp_config_find(unit, ecmp_member_count, emi_in, pri_cnt, ecmp_info));

exit:
    SHR_FREE(emi_in);
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ecmp_trav(int unit, bcm_l3_ecmp_traverse_cb trav_fn,
                      void *user_data)
{
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_ecmp_member_t *ecmp_member_array = NULL;
    int ltidx, max_paths, sz, ecmp_member_count, rv;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    max_paths = L3_ECMP_WEIGHTED_MAX_PATHS(unit);
    sz = sizeof(bcm_l3_ecmp_member_t) * max_paths;
    SHR_ALLOC(ecmp_member_array, sz, "bcmLtswXgsEcmpMember");
    SHR_NULL_CHECK(ecmp_member_array, SHR_E_MEMORY);

    L3_EGR_LOCK(unit);
    locked = true;

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        for (ltidx = L3_OL_ECMP_GRP_MIN(unit); ltidx <= L3_OL_ECMP_GRP_MAX(unit);
             ltidx++) {
            /* Skip unused entries. */
            if (L3_OL_ECMP_GRP_FBMP_GET(unit, ltidx)) {
                continue;
            }

            bcm_l3_egress_ecmp_t_init(&ecmp_info);
            sal_memset(ecmp_member_array, 0, sz);

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                           BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                           &ecmp_info.ecmp_intf));

            SHR_IF_ERR_VERBOSE_EXIT
                (olecmp_get(unit, ltidx, &ecmp_info, max_paths, ecmp_member_array,
                            &ecmp_member_count));

            rv = trav_fn(unit, &ecmp_info, ecmp_member_count, ecmp_member_array,
                         user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
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

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_egress_ltidx_to_obj_id(unit, ltidx,
                                       BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                       &ecmp_info.ecmp_intf));

        SHR_IF_ERR_VERBOSE_EXIT
            (ulecmp_get(unit, ltidx, &ecmp_info, max_paths,
                        ecmp_member_array, &ecmp_member_count));

        rv = trav_fn(unit, &ecmp_info, ecmp_member_count, ecmp_member_array,
                     user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
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
xgs_ltsw_l3_ecmp_max_paths_get(int unit, uint32_t attr, int *max_paths)
{
    bcmi_ltsw_dlb_capability_t cap;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(max_paths, SHR_E_PARAM);

    if (attr & BCMI_LTSW_ECMP_ATTR_DLB) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_capability_get(unit, bcmi_dlb_type_ecmp, &cap));
        *max_paths = cap.max_members_per_group;
        if (attr & BCMI_LTSW_ECMP_ATTR_DGM) {
            *max_paths += cap.max_alt_members_per_group;
        }
    } else if (attr & BCMI_LTSW_ECMP_ATTR_WGT) {
        *max_paths = (attr & BCMI_LTSW_ECMP_ATTR_OL) ?
                     L3_OL_ECMP_WEIGHTED_MAX_PATHS(unit) : L3_ECMP_WEIGHTED_MAX_PATHS(unit);
    } else if (attr & BCMI_LTSW_ECMP_ATTR_RH) {
        *max_paths = L3_ECMP_RH_MAX_PATHS(unit);
    } else {
        *max_paths = (attr & BCMI_LTSW_ECMP_ATTR_OL) ?
                     L3_OL_ECMP_MAX_PATHS(unit) : L3_ECMP_MAX_PATHS(unit);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_member_status_set(int unit, bcm_if_t intf, int status)
{
    int nhidx, local_port;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcm_l3_egress_t egr;


    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &nhidx, &type));

    if (type != BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    bcm_l3_egress_t_init(&egr);

    SHR_IF_ERR_VERBOSE_EXIT
        (nh_get(unit, nhidx, &egr));

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_dlb_port_get(unit, &egr, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_member_status_set(unit, local_port, bcmi_dlb_type_ecmp,
                                         status));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_member_status_get(int unit, bcm_if_t intf, int *status)
{
    int nhidx, local_port;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcm_l3_egress_t egr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf, &nhidx, &type));

    if (type != BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    bcm_l3_egress_t_init(&egr);

    SHR_IF_ERR_VERBOSE_EXIT
        (nh_get(unit, nhidx, &egr));

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_dlb_port_get(unit, &egr, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_member_status_get(unit, local_port, bcmi_dlb_type_ecmp,
                                         status));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_ethertype_set(
    int unit,
    uint32_t flags,
    int ethertype_count,
    int *ethertype_array)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_ethertype_set(unit,
                                     flags,
                                     ethertype_count,
                                     ethertype_array));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_ethertype_get(
    int unit,
    int ethertype_max,
    uint32_t *flags,
    int *ethertype_array,
    int *ethertype_count)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_ethertype_get(unit,
                                     ethertype_max,
                                     flags,
                                     ethertype_array,
                                     ethertype_count));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_dlb_mon_config_set(
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

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    dlb_id = ltidx - L3_ECMP_DLB_GRP_MIN(unit);

    dlb_monitor.enable = dlb_mon_cfg->enable;
    dlb_monitor.sample_rate = dlb_mon_cfg->sample_rate_64;
    switch (dlb_mon_cfg->action) {
        case BCM_L3_ECMP_DLB_MON_COPY_TO_CPU:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_COPY_TO_CPU;
            break;
        case BCM_L3_ECMP_DLB_MON_MIRROR:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_MIRROR;
            break;
        case BCM_L3_ECMP_DLB_MON_COPY_TO_CPU_AND_MIRROR:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_COPY_TO_CPU_AND_MIRROR;
            break;
        case BCM_L3_ECMP_DLB_MON_ACTION_NONE:
            dlb_monitor.action = BCMI_LTSW_DLB_MON_ACTION_NONE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_monitor_set(unit, dlb_type, dlb_id, &dlb_monitor));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_dlb_mon_config_get(
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

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, ecmp_group_id, &ltidx, &type));

    dlb_id = ltidx - L3_ECMP_DLB_GRP_MIN(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_monitor_get(unit, dlb_type, dlb_id, &dlb_monitor));

    dlb_mon_cfg->enable = dlb_monitor.enable;
    dlb_mon_cfg->sample_rate_64 = dlb_monitor.sample_rate;
    switch (dlb_monitor.action) {
        case BCMI_LTSW_DLB_MON_COPY_TO_CPU:
            dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_COPY_TO_CPU;
            break;
        case BCMI_LTSW_DLB_MON_MIRROR:
            dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_MIRROR;
            break;
        case BCMI_LTSW_DLB_MON_COPY_TO_CPU_AND_MIRROR:
            dlb_mon_cfg->action = BCM_L3_ECMP_DLB_MON_COPY_TO_CPU_AND_MIRROR;
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
xgs_ltsw_l3_ecmp_dlb_stat_set(
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
xgs_ltsw_l3_ecmp_dlb_stat_get(
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
xgs_ltsw_l3_ecmp_dlb_stat_sync_get(
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
xgs_ltsw_l3_egress_size_get(
    int unit,
    int *size)
{
    uint32_t size_tmp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, L3_UC_NHOPs, &size_tmp));
    *size = size_tmp;
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_count_get(
    int unit,
    int *count)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Check the current number of entries inserted in this LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_INFOs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, L3_UC_NHOPs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ENTRY_INUSE_CNTs, &value));

    *count = value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_size_get(
    int unit,
    int *size)
{
    uint32_t size_tmp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, ECMPs, &size_tmp));
    *size = size_tmp;
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_count_get(
    int unit,
    int *count)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Check the current number of entries inserted in this LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_INFOs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TABLE_IDs, ECMPs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ENTRY_INUSE_CNTs, &value));

    *count = value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_flexctr_attach(int unit, bcm_if_t intf_id,
                                bcmi_ltsw_flexctr_counter_info_t *info)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_ACTION;
    flexctr.action = info->action_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_flexctr_detach(int unit, bcm_if_t intf_id)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_ACTION;
    flexctr.action = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_flexctr_info_get(int unit, bcm_if_t intf_id,
                                  bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit, ltidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    const char *ltname, *fldname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t val;
    uint32_t ecmp_group_flags;
    l3_egress_flex_action_profile_t action_profile;
    bool ul = false;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_egress_obj_id_to_ltidx(unit, intf_id, &ltidx, &type));

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        if (L3_NHOP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        info->source = bcmFlexctrSourceL3EgressOverlay;
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        if (L3_NHOP_FBMP_GET(unit, ltidx)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        info->source = bcmFlexctrSourceL3EgressUnderlay;
        ul = true;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ecmp_group_flags = L3_UL_ECMP_GRP_FLAGS(unit, ltidx);

    if ((ecmp_group_flags &
        BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) ||
        (ecmp_group_flags &
        BCM_L3_ECMP_MEMBER_WEIGHTED)) {
        tbl_id = ul ? BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL :
                      BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL;
    } else {
        tbl_id = ul ? BCMINT_LTSW_L3_ECMP_TBL_UL :
                      BCMINT_LTSW_L3_ECMP_TBL_OL;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;
    fldname = ECMP_IDs;

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, fldname, ltidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, ECMP_CTR_ING_EFLEX_ACTION_IDs, &val));

    if (val == 0) {
        info->action_index = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ecmp_flex_action_profile_get(unit, val, ul, &action_profile));

        info->action_index = action_profile.action;
    }
    info->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->table_name = ltname;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_SET;
    flexctr.flags = L3_EGR_FLXCTR_F_INDEX;
    flexctr.index = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flexctr_op(unit, intf_id, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ecmp_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value)
{
    l3_egr_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_egr_flexctr_cfg_t));
    flexctr.op = L3_EGR_FLXCTR_OP_GET;
    flexctr.flags = L3_EGR_FLXCTR_F_INDEX;

    SHR_IF_ERR_VERBOSE_EXIT
        (ecmp_flexctr_op(unit, intf_id, &flexctr));

    *value = flexctr.index;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_add_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    int nhop_type;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&nh, 0, sizeof(nh));

    nhop_type = L3_NHOP_TYPE(unit, nhidx);

    if (nhop_type == L3_UC_NHOP_TYPE) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;
    } else if (nhop_type == L3_MC_NHOP_TYPE) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
    nh.fld_bmp = tbl_info->fld_bmp;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_parse(unit, eh, tbl_info, &nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));
    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    nh.fld_bmp = tbl_info->fld_bmp;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = nhidx;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_PRESENT_ACTION] = l2_tag_info->vlan_tag_present_action;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_NOT_PRESENT_ACTION] = l2_tag_info->vlan_tag_not_present_action;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_PRESENT_ACTION] = l2_tag_info->opaque_tag_present_action;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_NOT_PRESENT_ACTION] = l2_tag_info->opaque_tag_not_present_action;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID] = l2_tag_info->is_phb_map_id;
    if (l2_tag_info->is_phb_map_id) {
        nh.flds[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID] = l2_tag_info->phb_egr_l2_int_pri_to_otag_id;
        nh.fld_bmp &= ~ (BCMINT_XGS_L3_EGRESS_TBL_OCFI | BCMINT_XGS_L3_EGRESS_TBL_OPRI);
    } else {
        nh.flds[BCMINT_XGS_L3_EGRESS_TBL_OCFI] = l2_tag_info->cfi;
        nh.flds[BCMINT_XGS_L3_EGRESS_TBL_OPRI] = l2_tag_info->pri;
        nh.fld_bmp &= ~ BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID;
    }

    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_REMARK_CFI] = l2_tag_info->remark_cfi;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_OVID] = l2_tag_info->vlan;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_VLAN_OUTER_TPID_ID] = l2_tag_info->tpid_id;
    nh.flds[BCMINT_XGS_L3_EGRESS_TBL_ES_IDENTIFIER] = l2_tag_info->es_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_fill(unit, eh, tbl_info, &nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_get_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&nh, 0, sizeof(nh));

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    nh.fld_bmp = BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID;

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_parse(unit, eh, tbl_info, &nh));

    l2_tag_info->is_phb_map_id = nh.flds[BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID];
    nh.fld_bmp = tbl_info->fld_bmp;
    if (l2_tag_info->is_phb_map_id) {
        nh.fld_bmp &= ~ (BCMINT_XGS_L3_EGRESS_TBL_OCFI | BCMINT_XGS_L3_EGRESS_TBL_OPRI);
    } else {
        nh.fld_bmp &= ~ BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_parse(unit, eh, tbl_info, &nh));

    l2_tag_info->phb_egr_l2_int_pri_to_otag_id =
        (uint16)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID];
    l2_tag_info->cfi = nh.flds[BCMINT_XGS_L3_EGRESS_TBL_OCFI];
    l2_tag_info->pri = nh.flds[BCMINT_XGS_L3_EGRESS_TBL_OPRI];
    l2_tag_info->vlan_tag_present_action =
        (bcmi_ltsw_vlan_tag_present_action_t)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_PRESENT_ACTION];
    l2_tag_info->vlan_tag_not_present_action =
        (bcmi_ltsw_vlan_tag_not_present_action_t)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_NOT_PRESENT_ACTION];
    l2_tag_info->opaque_tag_present_action =
        (bcmi_ltsw_opaque_tag_present_action_t)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_PRESENT_ACTION];
    l2_tag_info->opaque_tag_not_present_action =
        (bcmi_ltsw_opaque_tag_not_present_action_t)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_NOT_PRESENT_ACTION];
    l2_tag_info->is_phb_map_id = (uint8)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID];
    l2_tag_info->remark_cfi = (uint8)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_REMARK_CFI];
    l2_tag_info->vlan = (bcm_vlan_t)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_OVID];
    l2_tag_info->tpid_id = (uint16)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_VLAN_OUTER_TPID_ID];
    l2_tag_info->es_id = (uint32)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_ES_IDENTIFIER];

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_delete_l2tag(
    int unit,
    int nhidx)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    int nhop_type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_EGR_LOCK(unit);
    locked = true;

    sal_memset(&nh, 0, sizeof(nh));

    tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
    nh.fld_bmp = tbl_info->fld_bmp;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_parse(unit, eh, tbl_info, &nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(eh));
    eh = BCMLT_INVALID_HDL;

    nhop_type = L3_NHOP_TYPE(unit, nhidx);

    if (nhop_type == L3_UC_NHOP_TYPE) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;
    } else if (nhop_type == L3_MC_NHOP_TYPE) {
        tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MC;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

    lt_name = tbl_info->name;
    lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
    nh.fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_nh_fill(unit, eh, tbl_info, &nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egress_php_action_add(
    int unit,
    bcm_if_t intf)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    int nhop_type;
    int mac_da_profile_id;
    bool locked = false;
    int nhidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    int ecmp_member_count = 0;
    int i, sz;
    bcm_l3_ecmp_member_t *ecmp_member_array = NULL;
    bcm_l3_egress_ecmp_t ecmp_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_EGR_LOCK(unit);
    locked = true;

    sal_memset(&nh, 0, sizeof(nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit,
                                            intf,
                                            &nhidx,
                                            &type));
    if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        ecmp_member_count = 1;
        sz = ecmp_member_count * sizeof(bcm_l3_ecmp_member_t);
        SHR_ALLOC(ecmp_member_array, sz, "bcmXgsEcmpMember");
        SHR_NULL_CHECK(ecmp_member_array, SHR_E_MEMORY);
        sal_memset(ecmp_member_array, 0, sz);
        ecmp_member_array->egress_if = intf;
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        sal_memset(&ecmp_info, 0, sizeof(ecmp_info));
        ecmp_info.ecmp_intf = intf;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l3_ecmp_get(unit,
                                 &ecmp_info,
                                 0,
                                 NULL,
                                 &ecmp_member_count));
        if (!ecmp_member_count) {
            SHR_EXIT();
        }
        sz = ecmp_member_count * sizeof(bcm_l3_ecmp_member_t);
        SHR_ALLOC(ecmp_member_array, sz, "bcmXgsEcmpMember");
        SHR_NULL_CHECK(ecmp_member_array, SHR_E_MEMORY);
        sal_memset(ecmp_member_array, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l3_ecmp_get(unit,
                                  &ecmp_info,
                                  ecmp_member_count,
                                  ecmp_member_array,
                                  &ecmp_member_count));
    }

    for (i = 0; i < ecmp_member_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_resolve(unit,
                                                ecmp_member_array[i].egress_if,
                                                &nhidx,
                                                &type));
        nhop_type = L3_NHOP_TYPE(unit, nhidx);

        if (nhop_type == L3_MPLS_TRANSIT_NHOP_TYPE) {
            continue;
        }

        if (nhop_type != L3_UC_NHOP_TYPE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

         tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;

         SHR_IF_ERR_EXIT
             (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

         lt_name = tbl_info->name;
         lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
         nh.fld_bmp = tbl_info->fld_bmp;

         dunit = bcmi_ltsw_dev_dunit(unit);

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_allocate(dunit, lt_name, &eh));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                   BCMLT_PRIORITY_NORMAL));

         SHR_IF_ERR_VERBOSE_EXIT
             (lt_nh_parse(unit, eh, tbl_info, &nh));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                   BCMLT_PRIORITY_NORMAL));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_free(eh));
         eh = BCMLT_INVALID_HDL;

         tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;

         SHR_IF_ERR_EXIT
             (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

         lt_name = tbl_info->name;
         nh.fld_bmp = tbl_info->fld_bmp;
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_ltsw_mpls_tnl_dst_mac_profile_add(unit,
                                                     nh.flds[BCMINT_XGS_L3_EGRESS_TBL_DST_MAC],
                                                     &mac_da_profile_id));
         nh.flds[BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID] = mac_da_profile_id;

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_allocate(dunit, lt_name, &eh));

         SHR_IF_ERR_VERBOSE_EXIT
             (lt_nh_fill(unit, eh, tbl_info, &nh));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

        L3_NHOP_TYPE(unit, nhidx) = L3_MPLS_TRANSIT_NHOP_TYPE;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    if (ecmp_member_array) {
        SHR_FREE(ecmp_member_array);
    }

    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_egress_php_action_del(
    int unit,
    bcm_if_t intf)
{
    nh_cfg_t nh;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *lt_name = NULL;
    const char *lt_key_name = NULL;
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    int nhop_type;
    uint64_t mac_da;
    bool locked = false;
    int nhidx;
    bcmi_ltsw_l3_egr_obj_type_t type;
    int i, sz;
    bcm_l3_ecmp_member_t *ecmp_member_array = NULL;
    bcm_l3_egress_ecmp_t ecmp_info;
    int ecmp_member_count = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGR_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_EGR_LOCK(unit);
    locked = true;
    sal_memset(&nh, 0, sizeof(nh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit,
                                            intf,
                                            &nhidx,
                                            &type));
    if (type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        ecmp_member_count = 1;
        sz = ecmp_member_count * sizeof(bcm_l3_ecmp_member_t);
        SHR_ALLOC(ecmp_member_array, sz, "bcmXgsEcmpMember");
        SHR_NULL_CHECK(ecmp_member_array, SHR_E_MEMORY);
        sal_memset(ecmp_member_array, 0, sz);
        ecmp_member_array->egress_if = intf;
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        sal_memset(&ecmp_info, 0, sizeof(ecmp_info));
        ecmp_info.ecmp_intf = intf;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l3_ecmp_get(unit,
                                 &ecmp_info,
                                 0,
                                 NULL,
                                 &ecmp_member_count));
        if (!ecmp_member_count) {
            SHR_EXIT();
        }

        sz = ecmp_member_count * sizeof(bcm_l3_ecmp_member_t);
        SHR_ALLOC(ecmp_member_array, sz, "bcmXgsEcmpMember");
        SHR_NULL_CHECK(ecmp_member_array, SHR_E_MEMORY);
        sal_memset(ecmp_member_array, 0, sz);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_l3_ecmp_get(unit,
                                  &ecmp_info,
                                  ecmp_member_count,
                                  ecmp_member_array,
                                  &ecmp_member_count));
    }
    for (i = 0; i < ecmp_member_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_resolve(unit,
                                                ecmp_member_array->egress_if,
                                                &nhidx,
                                                &type));
         nhop_type = L3_NHOP_TYPE(unit, nhidx);

         if (nhop_type == L3_UC_NHOP_TYPE) {
             continue;
         }

         if (nhop_type != L3_MPLS_TRANSIT_NHOP_TYPE) {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
         }

         tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT;

         SHR_IF_ERR_EXIT
             (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

         lt_name = tbl_info->name;
         lt_key_name = tbl_info->flds[BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID].fld_name;
         nh.fld_bmp = tbl_info->fld_bmp;

         dunit = bcmi_ltsw_dev_dunit(unit);

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_allocate(dunit, lt_name, &eh));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_add(eh, lt_key_name, nhidx));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                   BCMLT_PRIORITY_NORMAL));

         SHR_IF_ERR_VERBOSE_EXIT
             (lt_nh_parse(unit, eh, tbl_info, &nh));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                   BCMLT_PRIORITY_NORMAL));

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_free(eh));
         eh = BCMLT_INVALID_HDL;

         tbl_id = BCMINT_LTSW_L3_EGRESS_TBL_UC;

         SHR_IF_ERR_EXIT
             (bcmint_ltsw_l3_egress_tbl_get(unit, tbl_id, &tbl_info));

         lt_name = tbl_info->name;
         nh.fld_bmp = tbl_info->fld_bmp;
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_ltsw_mpls_tnl_dst_mac_profile_get(unit,
                                                     (int)nh.flds[BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID],
                                                     &mac_da));
         nh.flds[BCMINT_XGS_L3_EGRESS_TBL_DST_MAC] = mac_da;

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_allocate(dunit, lt_name, &eh));

         SHR_IF_ERR_VERBOSE_EXIT
             (lt_nh_fill(unit, eh, tbl_info, &nh));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

        L3_NHOP_TYPE(unit, nhidx) = L3_UC_NHOP_TYPE;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (ecmp_member_array) {
        SHR_FREE(ecmp_member_array);
    }
    if (locked) {
        L3_EGR_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


