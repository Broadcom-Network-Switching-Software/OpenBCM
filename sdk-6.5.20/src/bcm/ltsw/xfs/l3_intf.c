/*! \file l3_intf.c
 *
 * XFS L3 ingress/egress interface management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/xfs/l3_intf.h>
#include <bcm_int/ltsw/generated/l3_intf_ha.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* Flex editor control ID for Layer 3 forwarded packet */
#define L3_EDIT_CTRL_ID_NONE                0
#define L3_EDIT_CTRL_ID_DO_NOT_MODIFY       1
#define L3_EDIT_CTRL_ID_L3_ROUTE            4
#define L3_EDIT_CTRL_ID_L3_RESERVED         5
#define L3_EDIT_CTRL_ID_L3_MPLS_TNL_LB_PUSH 6
#define L3_EDIT_CTRL_ID_INIT_L3_TNL         8

/* Encoding for l3_oif_process_ctrl.  */
#define L3_OIF_PROCESS_CTRL(_intf_chk_en, _intf_chk_sw, _chk, _dec, _thrd)  \
    ((_intf_chk_en & 0x1) | ((_intf_chk_sw & 0x1) << 1) |                   \
     ((_chk & 0x1) << 4) | ((_dec & 0x7) << 5) | ((_thrd & 0xff) << 8))

#define L3_OIF_PROCESS_CTRL_IPMC_INTF_CHECK_EN(_c)  ((_c) & 0x1)
#define L3_OIF_PROCESS_CTRL_IPMC_INTF_CHECK_SW(_c)  (((_c) >> 1) & 0x1)
#define L3_OIF_PROCESS_CTRL_TTL_CHK(_c)  (((_c) >> 4) & 0x1)
#define L3_OIF_PROCESS_CTRL_TTL_DEC(_c)  (((_c) >> 5) & 0x7)
#define L3_OIF_PROCESS_CTRL_TTL_THRD(_c) (((_c) >> 8) & 0xff)

/*! Encoding for OBJ_TABLE_SEL_1 control ID */
#define L3_OIF_OBJ_TABLE_SEL_1_NONE              0x0
#define L3_OIF_OBJ_TABLE_SEL_1_L2_OIF            0x1
#define L3_OIF_OBJ_TABLE_SEL_1_L3_OIF_2          0x2
#define L3_OIF_OBJ_TABLE_SEL_1_EGR_L3_TUNNEL     0x4
#define L3_OIF_OBJ_TABLE_SEL_1_EGR_VC_LABEL      0x8

/* Generic data structure for L3 interface entry info. */
typedef struct l3_intf_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t *fv;

    /* Field array size. */
    uint32_t fcnt;
} l3_intf_cfg_t;

typedef struct l3_intf_vfi_s {
    /* Reference count of a VLAN used in L3 intf. */
    int refcnt;

    /* VFI information. */
    bcmi_ltsw_l3_intf_vfi_t info;
} l3_intf_vfi_t;

typedef struct l3_intf_info_s {
    /* L3 Interface manager initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Minimum iif index. */
    uint32_t iif_min;

    /* Maximum iif index. */
    uint32_t iif_max;

    /* Bitmap of free iif index. */
    SHR_BITDCL *iif_fbmp;

    /* Minimum overlay oif index. */
    uint32_t ol_oif_min;

    /* Maximum overlay oif index. */
    uint32_t ol_oif_max;

    /* Bitmap of free overlay oif index. */
    SHR_BITDCL *ol_oif_fbmp;

    /* Minimum underlay oif index. */
    uint32_t ul_oif_min;

    /* Maximum underlay oif index. */
    uint32_t ul_oif_max;

    /* Bitmap of free underlay oif index. */
    SHR_BITDCL *ul_oif_fbmp;

    /* Underlay OIF info. */
    bcmint_l3_ul_oif_info_t *ul_oif_info;

    /* VFI info associated to interface. */
    l3_intf_vfi_t *vfi_info;
} l3_intf_info_t;

static l3_intf_info_t l3_intf_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_INTF_INITED(_u) (l3_intf_info[_u].inited)

#define L3_INTF_LOCK(_u)            \
    sal_mutex_take(l3_intf_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_INTF_UNLOCK(_u)          \
    sal_mutex_give(l3_intf_info[_u].mutex)

#define L3_IIF_MIN(_u) l3_intf_info[_u].iif_min
#define L3_IIF_MAX(_u) l3_intf_info[_u].iif_max
#define L3_IIF_TBL_SZ(_u) (L3_IIF_MAX(_u) - L3_IIF_MIN(_u) + 1)
#define L3_IIF_VALID(_u, _id)       \
    (((_id) >= L3_IIF_MIN(_u)) && ((_id) <= L3_IIF_MAX(_u)))

#define L3_OL_OIF_MIN(_u) l3_intf_info[_u].ol_oif_min
#define L3_OL_OIF_MAX(_u) l3_intf_info[_u].ol_oif_max
#define L3_OL_OIF_TBL_SZ(_u) (L3_OL_OIF_MAX(_u) - L3_OL_OIF_MIN(_u) + 1)
#define L3_OL_OIF_VALID(_u, _id)    \
    (((_id) >= L3_OL_OIF_MIN(_u)) && ((_id) <= L3_OL_OIF_MAX(_u)))

#define L3_UL_OIF_MIN(_u) l3_intf_info[_u].ul_oif_min
#define L3_UL_OIF_MAX(_u) l3_intf_info[_u].ul_oif_max
#define L3_UL_OIF_TBL_SZ(_u) (L3_UL_OIF_MAX(_u) - L3_UL_OIF_MIN(_u) + 1)
#define L3_UL_OIF_OFFSET(_u) (L3_OL_OIF_MAX(_u) + 1)
#define L3_UL_OIF_VALID(_u, _id)    \
    (((_id) >= L3_UL_OIF_MIN(_u)) && ((_id) <= L3_UL_OIF_MAX(_u)))

#define L3_IIF_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_intf_info[_u].iif_fbmp, (_id))

#define L3_IIF_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_intf_info[_u].iif_fbmp, (_id))

#define L3_IIF_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_intf_info[_u].iif_fbmp, (_id))

#define L3_IIF_FBMP_ITER(_u, _id)   \
    SHR_BIT_ITER(l3_intf_info[_u].iif_fbmp, (L3_IIF_MAX(unit) + 1), _id)

#define L3_OL_OIF_FBMP_GET(_u, _id) \
    SHR_BITGET(l3_intf_info[_u].ol_oif_fbmp, (_id))

#define L3_OL_OIF_FBMP_SET(_u, _id) \
    SHR_BITSET(l3_intf_info[_u].ol_oif_fbmp, (_id))

#define L3_OL_OIF_FBMP_CLR(_u, _id) \
    SHR_BITCLR(l3_intf_info[_u].ol_oif_fbmp, (_id))

#define L3_UL_OIF_FBMP_GET(_u, _id) \
    SHR_BITGET(l3_intf_info[_u].ul_oif_fbmp, (_id))

#define L3_UL_OIF_FBMP_SET(_u, _id) \
    SHR_BITSET(l3_intf_info[_u].ul_oif_fbmp, (_id))

#define L3_UL_OIF_FBMP_CLR(_u, _id) \
    SHR_BITCLR(l3_intf_info[_u].ul_oif_fbmp, (_id))

#define L3_UL_OIF_INFO(_u, _id) (l3_intf_info[_u].ul_oif_info[_id])

#define L3_VFI_INFO(_u, _vid) (l3_intf_info[_u].vfi_info[_vid].info)

#define L3_VFI_REF_CNT(_u, _vid) (l3_intf_info[_u].vfi_info[_vid].refcnt)

#define L3_VFI_REF_CNT_INC(_u, _vid) (L3_VFI_REF_CNT(_u, _vid)++)

#define L3_VFI_REF_CNT_DEC(_u, _vid)       \
    do {                                    \
        if (L3_VFI_REF_CNT(_u, _vid)) {    \
            L3_VFI_REF_CNT(_u, _vid)--;    \
        }                                   \
    } while(0)

/* Reserve max idx of overlay egress interface for CPU. */
#define L3_INTF_L2CPU_IDX(_u) L3_OL_OIF_MAX(_u)

#define CLASS_ID_WIDTH 16

/******************************************************************************
* Private functions
 */

/*!
 * \brief Fill LT entry handle.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [in] cfg Data of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_eh_fill(int unit, bcmlt_entry_handle_t eh,
           const bcmint_ltsw_l3_intf_tbl_t *ti, l3_intf_cfg_t *cfg)
{
    const bcmint_ltsw_l3_intf_fld_t *fi = ti->flds;
    const char *sym_val;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);

    for (i = 0; i < cfg->fcnt; i++) {
        /* Skip fields that are invalid for the given LT. */
        if (!(ti->fld_bmp & (1 << i))) {
            continue;
        }

        /*
         * Skip fields that are not intended to be operated.
         * Key fields are always required.
         */
        if (!(cfg->fld_bmp & (1 << i)) && !fi[i].key) {
            continue;
        }

        if (fi[i].symbol) {
            SHR_IF_ERR_EXIT
                (fi[i].scalar_to_symbol(unit, cfg->fv[i], &sym_val));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_add_by_id(eh,
                                                    fi[i].fld_id,
                                                    sym_val));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add_by_id(eh, fi[i].fld_id, cfg->fv[i]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry of ING_L3_IIF_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [in/out] cfg Data of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_eh_parse(int unit, bcmlt_entry_handle_t eh,
            const bcmint_ltsw_l3_intf_tbl_t *ti, l3_intf_cfg_t *cfg)
{
    const bcmint_ltsw_l3_intf_fld_t *fi = ti->flds;
    uint32_t i, vt = 0;
    bool mult_view = false;
    const char *sym_val;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);

    for (i = 0; i < cfg->fcnt; i++) {
        /* Skip fields that are invalid for the given LT. */
        if (!(ti->fld_bmp & (1 << i))) {
            continue;
        }

        /*
         * Skip fields that are not intended to be operated.
         * Key and view fields are always required.
         */
        if (!(cfg->fld_bmp & (1 << i)) && !fi[i].key && !fi[i].view_t) {
            continue;
        }

        if (mult_view) {
            if (!(fi[i].vt_bmp & (1UL << vt))) {
                continue;
            }
        }

        if (fi[i].symbol) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_get_by_id(eh, fi[i].fld_id,
                                                    &sym_val));
            SHR_IF_ERR_EXIT
                (fi[i].symbol_to_scalar(unit, sym_val, &cfg->fv[i]));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get_by_id(eh, fi[i].fld_id, &cfg->fv[i]));
        }

        if (fi[i].view_t) {
            vt = cfg->fv[i];
            mult_view = true;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate L3 interface LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Table info.
 * \param [in] op LT operation code.
 * \param [in/out] cfg LT entry info
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_intf_lt_op(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti, bcmlt_opcode_t op,
              l3_intf_cfg_t *cfg)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t fbmp = cfg->fld_bmp;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (op == BCMLT_OPCODE_LOOKUP) {
        /* Only fill key for LOOKUP. */
        cfg->fld_bmp = 0;
    }

    SHR_IF_ERR_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    if (op == BCMLT_OPCODE_LOOKUP) {
        /* Set back fbmp to be parsed. */
        cfg->fld_bmp = fbmp;

        SHR_IF_ERR_EXIT
            (lt_eh_parse(unit, eh, ti, cfg));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct L3 ingress interface info from l3_intf_cfg_t.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Table info.
 * \param [in] cfg LT entry info
 * \param [out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_cfg_to_info(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                   l3_intf_cfg_t *cfg, bcm_l3_ingress_t *intf_info)
{
    uint64_t *fv = cfg->fv;
    int val, rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (fv[XFS_LTSW_L3_INTF_TBL_ING_URPF_DEFAULT_ROUTE_CHECK_ENABLE]) {
        intf_info->flags |= BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK;
    }

    if (fv[XFS_LTSW_L3_INTF_TBL_ING_USE_DSCP_FOR_PHB]) {
        intf_info->flags |= BCM_L3_INGRESS_DSCP_TRUST;
        val = fv[XFS_LTSW_L3_INTF_TBL_ING_DSCP_PTR];
        rv = bcmi_ltsw_qos_map_id_construct(unit, val,
                                            bcmiQosMapTypeL3Ingress,
                                            &intf_info->qos_map_id);
        if (SHR_FAILURE(rv)) {
            intf_info->qos_map_id = 0;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Failed to construct QoS map id for "
                                    "intf %d (DSCP ptr %d): rv = %d\n"),
                         (int)fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX], val, rv));
        }
    }

    if (!fv[XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST;
    }

    if (!fv[XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;
    }

    if (!fv[XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST;
    }

    if (!fv[XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }

    if (fv[XFS_LTSW_L3_INTF_TBL_ING_IP_MCAST_TYPE] ==
            XFS_LTSW_L3_INTF_IPMC_TYPE_PIM_BIDIR) {
        intf_info->flags |= BCM_L3_INGRESS_PIM_BIDIR;
    }

    if (!fv[XFS_LTSW_L3_INTF_TBL_ING_USE_L3_IIF_FOR_IPMC_LOOKUP]) {
        intf_info->flags |= BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE;
    }

    intf_info->urpf_mode = fv[XFS_LTSW_L3_INTF_TBL_ING_URPF_MODE];
    intf_info->vrf = fv[XFS_LTSW_L3_INTF_TBL_ING_VRF];
    intf_info->intf_class = fv[XFS_LTSW_L3_INTF_TBL_ING_CLASS_ID];
    intf_info->nat_realm_id = fv[XFS_LTSW_L3_INTF_TBL_ING_SRC_REALM_ID];
    intf_info->opaque_ctrl_id = fv[XFS_LTSW_L3_INTF_TBL_ING_OPAQUE_CTRL_ID];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct l3_intf_cfg_t from L3 ingress interface info.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Table info.
 * \param [in] cfg LT entry info
 * \param [out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_info_to_cfg(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                   bcm_l3_ingress_t *intf_info, l3_intf_cfg_t *cfg)
{
    uint64_t *fv = cfg->fv;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_L3_IIF;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NO_CLASS_ID;
    int sbr_index, dscp_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    fv[XFS_LTSW_L3_INTF_TBL_ING_VRF] = intf_info->vrf;
    fv[XFS_LTSW_L3_INTF_TBL_ING_OPAQUE_CTRL_ID] = intf_info->opaque_ctrl_id;
    fv[XFS_LTSW_L3_INTF_TBL_ING_IP_MCAST_TYPE] =
        (intf_info->flags & BCM_L3_INGRESS_PIM_BIDIR) ?
        XFS_LTSW_L3_INTF_IPMC_TYPE_PIM_BIDIR :
        XFS_LTSW_L3_INTF_IPMC_TYPE_PIM_SM;
    fv[XFS_LTSW_L3_INTF_TBL_ING_USE_L3_IIF_FOR_IPMC_LOOKUP] =
        (intf_info->flags & BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE) ? 0 : 1;
    fv[XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE] =
        (intf_info->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) ? 0 : 1;
    fv[XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE] =
        (intf_info->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST) ? 0 : 1;
    fv[XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE] =
        (intf_info->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST) ? 0 : 1;
    fv[XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE] =
        (intf_info->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST) ? 0 : 1;

    if (intf_info->flags & BCM_L3_INGRESS_DSCP_TRUST) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_qos_map_id_resolve(unit, intf_info->qos_map_id,
                                          NULL, &dscp_ptr));
        fv[XFS_LTSW_L3_INTF_TBL_ING_DSCP_PTR] = dscp_ptr;
        fv[XFS_LTSW_L3_INTF_TBL_ING_USE_DSCP_FOR_PHB] = 1;
    }

    if (intf_info->intf_class) {
        ent_type = BCMI_LTSW_SBR_PET_DEF;
        fv[XFS_LTSW_L3_INTF_TBL_ING_CLASS_ID] = intf_info->intf_class;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));
    fv[XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX] = sbr_index;

    cfg->fld_bmp =
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_INDEX) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_VRF) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_CLASS_ID) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_DSCP_PTR) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_OPAQUE_CTRL_ID) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_USE_DSCP_FOR_PHB) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IP_MCAST_TYPE) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_USE_L3_IIF_FOR_IPMC_LOOKUP) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE) |
        (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE);

    if (ltsw_feature(unit, LTSW_FT_URPF)) {
        fv[XFS_LTSW_L3_INTF_TBL_ING_URPF_DEFAULT_ROUTE_CHECK_ENABLE] =
            (intf_info->flags & BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK) ? 1:0;
        fv[XFS_LTSW_L3_INTF_TBL_ING_URPF_MODE] = intf_info->urpf_mode;

        cfg->fld_bmp |=
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_URPF_MODE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_URPF_DEFAULT_ROUTE_CHECK_ENABLE);
    }

    if (ltsw_feature(unit, LTSW_FT_NAT)) {
        fv[XFS_LTSW_L3_INTF_TBL_ING_SRC_REALM_ID] = intf_info->nat_realm_id;
        cfg->fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_ING_SRC_REALM_ID);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L3 ingress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] cfg LT entry info
 * \param [out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_get(int unit, bcm_if_t intf_id, bcm_l3_ingress_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));

    /* Key. */
    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = ti->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_iif_cfg_to_info(unit, ti, &cfg, intf_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 ingress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_iif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_set(int unit, int l3_iif_idx, bcm_l3_ingress_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    bcmlt_opcode_t op;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));

    /* Key. */
    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = l3_iif_idx;

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;

    /* Populate data fields and bitmap of field to be operated. */
    SHR_IF_ERR_EXIT
        (l3_iif_info_to_cfg(unit, ti, intf_info, &cfg));

    op = L3_IIF_FBMP_GET(unit, l3_iif_idx) ? BCMLT_OPCODE_INSERT :
                                             BCMLT_OPCODE_UPDATE;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, op, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update flex counter action in L3_IIF.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_iif_idx Entry index.
 * \param [in] action Flex counter action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_flexctr_update(int unit, int l3_iif_idx, uint32_t action)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));

    /* Fill key and flexctr_action. */
    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = l3_iif_idx;
    fv[XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION] = action;

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get field bitmap for the given view.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Table info.
 * \param [in] view View type.
 * \param [out] fbmp Field bitmap.
 *
 * \retval None
 */
static void
l3_intf_vt_fbmp_get(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                    uint32_t view, uint64_t *fbmp)
{
    const bcmint_ltsw_l3_intf_fld_t *fi = ti->flds;
    int i, cnt;

    *fbmp = 0;

    if (ti->attr & XFS_LTSW_L3_INTF_TBL_ATTR_ING) {
        cnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    } else {
        cnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    }

    for (i = 0; i < cnt; i++) {
        if ((ti->fld_bmp & (1ULL << i)) && (fi[i].vt_bmp & (1ULL << view))) {
            *fbmp |= (1ULL << i);
        }
    }
}

/*!
 * \brief Construct L3 egress interface info from l3_intf_cfg_t.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Table info.
 * \param [in] cfg LT entry info
 * \param [out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_cfg_to_info(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                   l3_intf_cfg_t *cfg, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    uint64_t *fv, fbmp;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1;
    bcmi_ltsw_sbr_profile_ent_type_t et;
    int sbr_idx, tnl_type = bcmiTunnelTypeInvalid;
    uint32_t view = 0, proc_ctrl = 0, edit_ctrl_id = 0;
    bcmi_ltsw_mpls_encap_label_info_t mpls_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    fv = cfg->fv;
    fbmp = ti->fld_bmp;

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T)) {
        /* Get the field bitmap valid in the given view. */
        view = fv[XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T];
        l3_intf_vt_fbmp_get(unit, ti, view, &fbmp);
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS)) {
        bcmi_ltsw_util_uint64_to_mac(intf_info->mac_addr,
            &fv[XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS]);
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VFI)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vpnid_get(unit,
                                         fv[XFS_LTSW_L3_INTF_TBL_EGR_VFI],
                                         &intf_info->vid));
    } else if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vpnid_get(unit,
                L3_UL_OIF_INFO(unit, fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX]).vfi,
                &intf_info->vid));
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL)) {
        proc_ctrl = fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL];
        intf_info->ttl = L3_OIF_PROCESS_CTRL_TTL_THRD(proc_ctrl);
        intf_info->ttl_dec = L3_OIF_PROCESS_CTRL_TTL_DEC(proc_ctrl);

        if (L3_OIF_PROCESS_CTRL_IPMC_INTF_CHECK_EN(proc_ctrl)) {
            intf_info->intf_flags |= BCM_L3_INTF_IPMC_SAME_INTF_CHECK;

            if (!L3_OIF_PROCESS_CTRL_IPMC_INTF_CHECK_SW(proc_ctrl)) {
                intf_info->intf_flags |= BCM_L3_INTF_IPMC_SAME_INTF_DROP;
            }
        }

        if (L3_OIF_PROCESS_CTRL_TTL_CHK(proc_ctrl)) {
            intf_info->intf_flags |= BCM_L3_INTF_TTL_CHECK;
        }
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID)) {
        edit_ctrl_id = fv[XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID];
        if (edit_ctrl_id == L3_EDIT_CTRL_ID_NONE) {
            intf_info->flags |= BCM_L3_L2ONLY;
        } else if (edit_ctrl_id == L3_EDIT_CTRL_ID_INIT_L3_TNL) {
            if (view == XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER) {
                tnl_type = bcmiTunnelTypeMpls;
            } else {
                tnl_type = bcmiTunnelTypeIpL3;
            }
        } else if (edit_ctrl_id == L3_EDIT_CTRL_ID_L3_MPLS_TNL_LB_PUSH) {
            tnl_type = bcmiTunnelTypeMpls;
        } else if (edit_ctrl_id == L3_EDIT_CTRL_ID_DO_NOT_MODIFY) {
            intf_info->intf_flags |= BCM_L3_INTF_PACKET_DO_NOT_MODIFY;
        }

        if (tnl_type != bcmiTunnelTypeInvalid) {
            BCMI_LTSW_GPORT_TUNNEL_ID_SET(intf_info->tunnel_idx, tnl_type,
                                      fv[XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX]);
        }
    }

    /* L3 MPLS Tunnel initiator. */
    if ((fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX)) &&
        (view == XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER) &&
        (edit_ctrl_id == L3_EDIT_CTRL_ID_INIT_L3_TNL)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_mpls_encap_label_get(unit,
                fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX], &mpls_info));
        intf_info->mpls_label = mpls_info.vc_label;
        intf_info->mpls_ttl = mpls_info.ttl;
        intf_info->mpls_exp = mpls_info.exp;
        if (mpls_info.flags & BCMI_LTSW_MPLS_ENCAP_LABL_USE_PLD_TTL) {
            intf_info->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        } else {
            
            intf_info->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
    } else {
        intf_info->mpls_label = BCM_MPLS_LABEL_INVALID;
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX)) {
        et = BCMI_LTSW_SBR_PET_NO_MAC_SA;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, et, &sbr_idx));
        if (fv[XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX] == sbr_idx) {
            intf_info->flags |= BCM_L3_KEEP_SRCMAC;
        }

        et = BCMI_LTSW_SBR_PET_NO_L3_VPN_INDEX_NO_TUNNEL_IDX_NO_VFI;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, et, &sbr_idx));
        if (fv[XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX] == sbr_idx) {
            intf_info->flags |= BCM_L3_KEEP_VLAN;
        }

        et = BCMI_LTSW_SBR_PET_NONE;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, et, &sbr_idx));
        if (fv[XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX] == sbr_idx) {
            intf_info->flags |= BCM_L3_KEEP_SRCMAC | BCM_L3_KEEP_VLAN;
        }
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID)) {
        intf_info->opaque_ctrl_id = fv[XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID];
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct l3_intf_cfg_t from L3 egress interface info.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Table info.
 * \param [in] cfg LT entry info
 * \param [out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_info_to_cfg(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                   bcmint_ltsw_l3_egr_intf_t *intf_info, l3_intf_cfg_t *cfg)
{
    uint64_t *fv, fbmp;
    uint32_t vfi;
    bcmi_ltsw_l3_intf_vfi_t *vi;
    int tnl_idx, edit_ctrl_id = 0, obj_tbl_sel_1 = 0, view = 0, l3_vpn_idx = 0;
    bcmi_ltsw_tunnel_type_t tnl_type = bcmiTunnelTypeInvalid;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1;
    bcmi_ltsw_sbr_profile_ent_type_t et = BCMI_LTSW_SBR_PET_DEF;
    int sbr_idx;
    bool ttl_check, chk_en, chk_sw, mpls_label_added = false;
    uint8_t ttl_dec, ttl;
    bcmi_ltsw_mpls_encap_label_info_t mpls_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    fv = cfg->fv;
    fbmp = ti->fld_bmp;

    BCMI_LTSW_GPORT_TUNNEL_ID_GET(intf_info->tunnel_idx, tnl_type, tnl_idx);

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T)) {
        /* Get the field bitmap valid in the given view. */
        if ((tnl_type == bcmiTunnelTypeMpls) &&
            (intf_info->mpls_label != BCM_MPLS_LABEL_INVALID)) {
            view = XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER;
        } else {
            view = XFS_LTSW_L3_INTF_EGR_VT_L3_OIF;
        }
        l3_intf_vt_fbmp_get(unit, ti, view, &fbmp);
        fv[XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T] = view;
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS)) {
        bcmi_ltsw_util_mac_to_uint64
            (&fv[XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS],
             intf_info->mac_addr);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, intf_info->vid, &vfi));
    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VFI)) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_VFI] = vfi;
    } else if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG)) {
        vi = &L3_VFI_INFO(unit, vfi);
        fv[XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG] = vi->tag;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_L2_HDR_ACTION] = vi->tag_action << 1;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_MEMBERSHIP_PROFILE_IDX] =
            vi->membership_prfl_idx;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_UNTAG_PROFILE_INDEX] = vi->untag_prfl_idx;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_STG] = vi->stg;
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL)) {
        if (intf_info->ttl_dec > 7) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        chk_en = (intf_info->intf_flags & BCM_L3_INTF_IPMC_SAME_INTF_CHECK) ?
                 1 : 0;
        chk_sw = (intf_info->intf_flags & BCM_L3_INTF_IPMC_SAME_INTF_DROP) ?
                 0 : 1;
        ttl_check = intf_info->intf_flags & BCM_L3_INTF_TTL_CHECK ? 1 : 0;
        ttl = intf_info->ttl;
        ttl_dec = intf_info->ttl_dec;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL] =
            L3_OIF_PROCESS_CTRL(chk_en, chk_sw, ttl_check, ttl_dec, ttl);
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID)) {
        obj_tbl_sel_1 = L3_OIF_OBJ_TABLE_SEL_1_L2_OIF;

        if (intf_info->flags & BCM_L3_L2ONLY) {
            edit_ctrl_id = L3_EDIT_CTRL_ID_NONE;
        } else {
            if (tnl_type == bcmiTunnelTypeIpL3) {
                obj_tbl_sel_1 |= L3_OIF_OBJ_TABLE_SEL_1_L3_OIF_2 |
                                 L3_OIF_OBJ_TABLE_SEL_1_EGR_L3_TUNNEL;
                edit_ctrl_id = L3_EDIT_CTRL_ID_INIT_L3_TNL;
            } else if (tnl_type == bcmiTunnelTypeMpls) {
                obj_tbl_sel_1 |= L3_OIF_OBJ_TABLE_SEL_1_EGR_L3_TUNNEL;
                edit_ctrl_id = L3_EDIT_CTRL_ID_L3_MPLS_TNL_LB_PUSH;
            } else if (tnl_type == bcmiTunnelTypeInvalid) {
                edit_ctrl_id = L3_EDIT_CTRL_ID_L3_ROUTE;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
        }
        if (intf_info->intf_flags & BCM_L3_INTF_PACKET_DO_NOT_MODIFY) {
            edit_ctrl_id = L3_EDIT_CTRL_ID_DO_NOT_MODIFY;
        }
    }

    /* L3 MPLS Tunnel initiator. */
    if ((fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX)) &&
        (intf_info->mpls_label != BCM_MPLS_LABEL_INVALID)) {
        mpls_info.vc_label = intf_info->mpls_label;
        mpls_info.exp = intf_info->mpls_exp;
        if (intf_info->mpls_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            if (!intf_info->mpls_ttl) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
            mpls_info.ttl = intf_info->mpls_ttl;
        } else {
            mpls_info.flags |= BCMI_LTSW_MPLS_ENCAP_LABL_USE_PLD_TTL;
        }
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_mpls_encap_label_add(unit, &mpls_info, &l3_vpn_idx));
        mpls_label_added = true;
        obj_tbl_sel_1 |= L3_OIF_OBJ_TABLE_SEL_1_L3_OIF_2 |
                         L3_OIF_OBJ_TABLE_SEL_1_EGR_VC_LABEL;
        edit_ctrl_id = L3_EDIT_CTRL_ID_INIT_L3_TNL;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX] = l3_vpn_idx;
    }

    fv[XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1] = obj_tbl_sel_1;
    fv[XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID] = edit_ctrl_id;
    fv[XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX] = tnl_idx;

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX)) {
        if ((intf_info->flags & BCM_L3_KEEP_SRCMAC) &&
            (intf_info->flags & BCM_L3_KEEP_VLAN)) {
            et = BCMI_LTSW_SBR_PET_NONE;
        } else if (intf_info->flags & BCM_L3_KEEP_SRCMAC) {
            et = BCMI_LTSW_SBR_PET_NO_MAC_SA;
        } else if (intf_info->flags & BCM_L3_KEEP_VLAN) {
            et = BCMI_LTSW_SBR_PET_NO_L3_VPN_INDEX_NO_TUNNEL_IDX_NO_VFI;
        }

        SHR_IF_ERR_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, et, &sbr_idx));
        fv[XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX] = sbr_idx;
    }

    if (fbmp & (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID)) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID] = intf_info->opaque_ctrl_id;
    }

    /* Don't operate flexctr_action. */
    cfg->fld_bmp = fbmp & (~(1ULL << XFS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION));

exit:
    if (SHR_FUNC_ERR() && mpls_label_added) {
        (void)bcmi_ltsw_mpls_encap_label_delete(unit, l3_vpn_idx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L3 overlay egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_oif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_ol_get(int unit, int l3_oif_idx, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    /* Key. */
    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = ti->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_oif_cfg_to_info(unit, ti, &cfg, intf_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 overlay egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_oif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_ol_set(int unit, int l3_oif_idx, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    int old_vpn_idx = 0, rv;
    bool old_mpls_en = false;
    bcmlt_opcode_t op;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    if (L3_OL_OIF_FBMP_GET(unit, l3_oif_idx)) {
        op = BCMLT_OPCODE_INSERT;
    } else {
        op = BCMLT_OPCODE_UPDATE;

        /* Fetch previous l3_vpn_index. */
        fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
        cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX) |
                      (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1);
        SHR_IF_ERR_EXIT
            (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));
        if (fv[XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1] &
                L3_OIF_OBJ_TABLE_SEL_1_EGR_VC_LABEL) {
            old_mpls_en = true;
            old_vpn_idx = fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX];
        }
    }

    /* Populate field array with new intf_info. */
    sal_memset(fv, 0, sizeof(fv));
    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    SHR_IF_ERR_EXIT
        (l3_oif_info_to_cfg(unit, ti, intf_info, &cfg));

    rv = l3_intf_lt_op(unit, ti, op, &cfg);
    if (SHR_FAILURE(rv)) {
        /* Free the newly added MPLS VC label. */
        if (fv[XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1] &
                L3_OIF_OBJ_TABLE_SEL_1_EGR_VC_LABEL) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_mpls_encap_label_delete(unit,
                    fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX]));
        }
        SHR_IF_ERR_EXIT(rv);
    }

    /* Delete previous label from Egress MPLS VPN table. */
    if (old_mpls_en) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_mpls_encap_label_delete(unit, old_vpn_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 overlay egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_oif_idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_ol_del(int unit, int l3_oif_idx)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    int l3_vpn_idx = 0;
    bool mpls_en = false;

    SHR_FUNC_ENTER(unit);

    if (L3_OL_OIF_FBMP_GET(unit, l3_oif_idx)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    /* Fetch l3_vpn_index. */
    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1) |
                  (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX);
    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    /* Delete label from Egress MPLS VPN table. */
    if (fv[XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1] &
            L3_OIF_OBJ_TABLE_SEL_1_EGR_VC_LABEL) {
        l3_vpn_idx = fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX];
        mpls_en = true;
    }

    cfg.fld_bmp = 0;
    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_DELETE, &cfg));

    L3_OL_OIF_FBMP_SET(unit, l3_oif_idx);

    if (mpls_en) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_mpls_encap_label_delete(unit, l3_vpn_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L3 underlay egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_oif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_ul_get(int unit, int l3_oif_idx, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_UL, &ti));

    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = ti->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_oif_cfg_to_info(unit, ti, &cfg, intf_info));

    intf_info->intf_flags |= BCM_L3_INTF_UNDERLAY;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 underlay egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_oif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_ul_set(int unit, int l3_oif_idx, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bcmlt_opcode_t op;
    uint32_t vfi = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_UL, &ti));

    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    SHR_IF_ERR_EXIT
        (l3_oif_info_to_cfg(unit, ti, intf_info, &cfg));

    if (L3_UL_OIF_FBMP_GET(unit, l3_oif_idx)) {
        op = BCMLT_OPCODE_INSERT;
    } else {
        if (L3_UL_OIF_INFO(unit, l3_oif_idx).vfi) {
            L3_VFI_REF_CNT_DEC(unit, L3_UL_OIF_INFO(unit, l3_oif_idx).vfi);
            L3_UL_OIF_INFO(unit, l3_oif_idx).vfi = 0;
        }
        op = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, op, &cfg));

    if (intf_info->vid) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vfi_idx_get(unit, intf_info->vid, &vfi));
        L3_UL_OIF_INFO(unit, l3_oif_idx).vfi = (int)vfi;
        L3_VFI_REF_CNT_INC(unit, vfi);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 underlay egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_oif_idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_ul_del(int unit, int l3_oif_idx)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    if (L3_UL_OIF_FBMP_GET(unit, l3_oif_idx)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_UL, &ti));

    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_DELETE, &cfg));

    if (L3_UL_OIF_INFO(unit, l3_oif_idx).vfi) {
        L3_VFI_REF_CNT_DEC(unit, L3_UL_OIF_INFO(unit, l3_oif_idx).vfi);
        L3_UL_OIF_INFO(unit, l3_oif_idx).vfi = 0;
    }

    L3_UL_OIF_FBMP_SET(unit, l3_oif_idx);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the VFI information for a given vid.
 *
 * \param [in] unit Unit number.
 * \param [in] vid VFI ID.
 * \param [in] vfi_info VFI information.
 *
 * \retval None.
 */
static void
l3_intf_vfi_info_update(int unit, int vid, bcmi_ltsw_l3_intf_vfi_t *vfi_info)
{
    bcmi_ltsw_l3_intf_vfi_t *vi = &L3_VFI_INFO(unit, vid);

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_TAG) {
        vi->tag = vfi_info->tag;
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_TAG_ACTION) {
        vi->tag_action = vfi_info->tag_action;
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX) {
        vi->membership_prfl_idx = vfi_info->membership_prfl_idx;
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX) {
        vi->untag_prfl_idx = vfi_info->untag_prfl_idx;
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_STG) {
        vi->stg = vfi_info->stg;
    }
}

/*!
 * \brief Callback function to update the VFI information during init.
 *
 * \param [in] unit Unit number.
 * \param [in] vid VFI ID.
 * \param [in] vfi_info VFI information.
 * \param [in] user_data User data of callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_intf_vfi_info_update_cb(int unit, int vid,
                           bcmi_ltsw_l3_intf_vfi_t *vfi_info,
                           void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!vfi_info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    l3_intf_vfi_info_update(unit, vid, vfi_info);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover L3 iif database from LTs.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_recover_cb(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                  void *cookie)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};

    SHR_FUNC_ENTER(unit);

    ti = (const bcmint_ltsw_l3_intf_tbl_t *)cookie;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, &cfg));

    L3_IIF_FBMP_CLR(unit, fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX]);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover L3 oif_1 database from LTs.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_1_recover_cb(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                    void *cookie)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};

    SHR_FUNC_ENTER(unit);

    ti = (const bcmint_ltsw_l3_intf_tbl_t *)cookie;

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, &cfg));

    L3_OL_OIF_FBMP_CLR(unit, fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX]);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover L3 oif_2 database from LTs.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_oif_2_recover_cb(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                    void *cookie)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    int vfi;

    SHR_FUNC_ENTER(unit);

    ti = (const bcmint_ltsw_l3_intf_tbl_t *)cookie;

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, &cfg));

    L3_UL_OIF_FBMP_CLR(unit, fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX]);

    vfi = L3_UL_OIF_INFO(unit, fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX]).vfi;
    if (vfi) {
        L3_VFI_REF_CNT_INC(unit, vfi);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the tables managed by L3 interface module.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_intf_tables_clear(int unit)
{
    int i, dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    union {
        uint64_t iif[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT];
        uint64_t oif[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT];
    } fv;
    l3_intf_cfg_t cfg = {0};
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t stat_ctr_id, l3_vpn_idx;

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < XFS_LTSW_L3_INTF_TBL_CNT; i++) {
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_intf_tbl_get(unit, i, &ti));

        if (ti->attr & XFS_LTSW_L3_INTF_TBL_ATTR_ING) {
            cfg.fv = fv.iif;
            cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
            cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION);
        } else {
            cfg.fv = fv.oif;
            cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
            cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1) |
                          (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

        while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            sal_memset(&fv, 0, sizeof(fv));

            SHR_IF_ERR_EXIT
                (lt_eh_parse(unit, eh, ti, &cfg));

            if (ti->attr & XFS_LTSW_L3_INTF_TBL_ATTR_ING) {
                if (cfg.fv[XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION]) {
                    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
                    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
                    ci.action_index =
                        cfg.fv[XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION];
                    ci.source = bcmFlexctrSourceIngL3Intf;
                    rv = bcmi_ltsw_flexctr_counter_id_get(unit, &ci,
                                                          &stat_ctr_id);
                    if (SHR_SUCCESS(rv)) {
                        rv = bcmi_ltsw_flexctr_detach_counter_id_status_update(
                                unit, stat_ctr_id);
                        if (SHR_FAILURE(rv)) {
                            LOG_VERBOSE(BSL_LOG_MODULE,
                                        (BSL_META_U(unit,
                                                    "Failed to update flex "
                                                    "counter id %d (%d).\n"),
                                         stat_ctr_id, rv));
                        }
                    }
                }
            } else {
                /* Delete label from Egress MPLS VPN table. */
                if (cfg.fv[XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1] &
                        L3_OIF_OBJ_TABLE_SEL_1_EGR_VC_LABEL) {
                    l3_vpn_idx = cfg.fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX];
                    rv = bcmi_ltsw_mpls_encap_label_delete(unit, l3_vpn_idx);
                    if (SHR_FAILURE(rv)) {
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U(unit,
                                                "Failed to delete mpls label "
                                                "%d (%d).\n"),
                                     l3_vpn_idx, rv));
                    }
                }
            }
            SHR_IF_ERR_CONT
                (bcmlt_entry_commit(eh, BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL));
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initailzie L3 IIF/OIF tables.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_intf_table_init(int unit)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    l3_intf_cfg_t cfg = {0};
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth;
    bcmi_ltsw_sbr_profile_ent_type_t et;
    int sbr_idx = 0;
    union {
        uint64_t iif[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT];
        uint64_t oif[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT];
    } fv;

    SHR_FUNC_ENTER(unit);

    /*
     * For simple bridged flow, program L3_IIF and L3_OIF entry 0 for
     * handling pure bridged VFIs.
     */

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));

    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE) |
                  (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE) |
                  (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE) |
                  (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE) |
                  (1ULL << XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX);
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fv = fv.iif;
    sal_memset(&fv, 0, sizeof(fv));

    pth = BCMI_LTSW_SBR_PTH_L3_IIF;
    et = BCMI_LTSW_SBR_PET_NONE;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, et, &sbr_idx));
    cfg.fv[XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX] = sbr_idx;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_INSERT, &cfg));

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL) |
                  (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX);
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fv = fv.oif;
    sal_memset(&fv, 0, sizeof(fv));

    /*
     * bit 0: ipmc_intf_check_en
     * bit 1: ipmc_intf_check_sw
     * bit 2: ipmc_always_bridge
     *        Downgrade forwarding_type to L2 bridging unconditionally.
     *        (For certain IPMC-VXLAN use-cases when all MC flows are bridged
     *        into the tunnel and we wish to optimize ccbe_index remap table
     *        usage in EPRE)
     */
    cfg.fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL] = 0x7;

    pth = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1;
    et = BCMI_LTSW_SBR_PET_NONE;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, et, &sbr_idx));
    cfg.fv[XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX] = sbr_idx;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_INSERT, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
xfs_ltsw_l3_intf_init(int unit)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];
    const bcmint_ltsw_l3_intf_tbl_t *ti_iif, *ti_oif_ol, *ti_oif_ul;
    const char *key_iif, *key_oif_ol, *key_oif_ul;
    uint64_t min, max;
    uint32_t num, sz, alloc_sz;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (l3ii->inited) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_intf_deinit(unit));
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti_iif));
    key_iif = ti_iif->flds[XFS_LTSW_L3_INTF_TBL_ING_INDEX].fld_name;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL,
                                     &ti_oif_ol));
    key_oif_ol = ti_oif_ol->flds[XFS_LTSW_L3_INTF_TBL_EGR_INDEX].fld_name;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_UL,
                                     &ti_oif_ul));
    key_oif_ul = ti_oif_ul->flds[XFS_LTSW_L3_INTF_TBL_EGR_INDEX].fld_name;

    sal_memset(l3ii, 0, sizeof(l3_intf_info_t));

    l3ii->mutex = sal_mutex_create("bcmXfsL3intfInfo");
    SHR_NULL_CHECK(l3ii->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ti_iif->name, key_iif,
                                       &min, &max));
    /* Reserve entry 0 for all NPL-defined index LT in DNA. */
    l3ii->iif_min = min ? min : 1;
    l3ii->iif_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    l3ii->iif_fbmp = sal_alloc(sz, "bcmXfsL3IifFreeBmp");
    SHR_NULL_CHECK(l3ii->iif_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(l3ii->iif_fbmp, 0, L3_IIF_MIN(unit));
    SHR_BITSET_RANGE(l3ii->iif_fbmp, L3_IIF_MIN(unit), L3_IIF_TBL_SZ(unit));

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ti_oif_ol->name, key_oif_ol,
                                       &min, &max));
    /* Reserve entry 0 for all NPL-defined index LT in DNA. */
    l3ii->ol_oif_min = min ? min : 1;
    l3ii->ol_oif_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    l3ii->ol_oif_fbmp = sal_alloc(sz, "bcmXfsL3OlOifFreeBmp");
    SHR_NULL_CHECK(l3ii->ol_oif_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(l3ii->ol_oif_fbmp, 0, L3_OL_OIF_MIN(unit));
    SHR_BITSET_RANGE(l3ii->ol_oif_fbmp, L3_OL_OIF_MIN(unit),
                     L3_OL_OIF_TBL_SZ(unit));

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ti_oif_ul->name, key_oif_ul,
                                       &min, &max));
    /* Reserve entry 0 for all NPL-defined index LT in DNA. */
    l3ii->ul_oif_min = min ? min : 1;
    l3ii->ul_oif_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    l3ii->ul_oif_fbmp = sal_alloc(sz, "bcmXfsL3UlOifFreeBmp");
    SHR_NULL_CHECK(l3ii->ul_oif_fbmp, SHR_E_MEMORY);
    SHR_BITCLR_RANGE(l3ii->ul_oif_fbmp, 0, L3_UL_OIF_MIN(unit));
    SHR_BITSET_RANGE(l3ii->ul_oif_fbmp, L3_UL_OIF_MIN(unit),
                     L3_UL_OIF_TBL_SZ(unit));

    sz = sizeof(bcmint_l3_ul_oif_info_t) * (L3_UL_OIF_MAX(unit) + 1);
    alloc_sz = sz;
    l3ii->ul_oif_info = bcmi_ltsw_ha_mem_alloc(unit,
                            BCMI_HA_COMP_ID_L3_INTF,
                            BCMINT_LTSW_L3_INTF_HA_UL_OIF_INFO,
                            "bcmXfsL3UlOifInfo",
                            &alloc_sz);
    SHR_NULL_CHECK(l3ii->ul_oif_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                           BCMI_HA_COMP_ID_L3_INTF,
                                           BCMINT_LTSW_L3_INTF_HA_UL_OIF_INFO,
                                           0, sz, 1,
                                           BCMINT_L3_UL_OIF_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_capacity_get(unit, ING_VFI_TABLEs, &num));
    sz = num * sizeof(l3_intf_vfi_t);
    l3ii->vfi_info = sal_alloc(sz, "bcmXfsL3vlaninfo");
    SHR_NULL_CHECK(l3ii->vfi_info, SHR_E_MEMORY);
    sal_memset(l3ii->vfi_info, 0, sz);

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_traverse(unit, ti_iif->name, (void *)ti_iif,
                                    l3_iif_recover_cb));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_traverse(unit, ti_oif_ol->name, (void *)ti_oif_ol,
                                    l3_oif_1_recover_cb));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_traverse(unit, ti_oif_ul->name, (void *)ti_oif_ul,
                                    l3_oif_2_recover_cb));
    } else {
        sz = sizeof(bcmint_l3_ul_oif_info_t) * (L3_UL_OIF_MAX(unit) + 1);
        sal_memset(l3ii->ul_oif_info, 0, sz);

        /* Init L3 IIF/OIF tables. */
        SHR_IF_ERR_EXIT
            (l3_intf_table_init(unit));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_vlan_l3_intf_update_all(unit,
                                           l3_intf_vfi_info_update_cb,
                                           NULL));

    l3ii->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(l3ii->iif_fbmp);
        SHR_FREE(l3ii->ol_oif_fbmp);
        SHR_FREE(l3ii->ul_oif_fbmp);
        SHR_FREE(l3ii->vfi_info);
        if (!bcmi_warmboot_get(unit) && l3ii->ul_oif_info) {
            (void)bcmi_ltsw_ha_mem_free(unit, l3ii->ul_oif_info);
        }
        if (l3ii->mutex) {
            sal_mutex_destroy(l3ii->mutex);
        }
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_intf_deinit(int unit)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!l3ii->inited || !l3ii->mutex) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(l3ii->mutex, SAL_MUTEX_FOREVER);

    l3ii->inited = 0;
    SHR_FREE(l3ii->iif_fbmp);
    SHR_FREE(l3ii->ol_oif_fbmp);
    SHR_FREE(l3ii->ul_oif_fbmp);
    SHR_FREE(l3ii->vfi_info);

    sal_mutex_give(l3ii->mutex);
    sal_mutex_destroy(l3ii->mutex);

    SHR_IF_ERR_EXIT
        (l3_intf_tables_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    *min = L3_IIF_MIN(unit);
    *max = L3_IIF_MAX(unit);

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width)
{
    SHR_FUNC_ENTER(unit);

    if (!min && !max && !width) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (min) {
        *min = 0;
    }

    if (max) {
        *max = (1 << CLASS_ID_WIDTH) - 1;
    }

    if (width) {
        *width = CLASS_ID_WIDTH;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_create(int unit, bcm_l3_ingress_t *intf_info,
                            bcm_if_t *intf_id)
{
    int l3_iif_idx;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if (intf_info->flags & (BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_REPLACE)) {
        l3_iif_idx = *intf_id;

        if (!L3_IIF_VALID(unit, l3_iif_idx)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (!(L3_IIF_FBMP_GET(unit, l3_iif_idx)) &&
            !(intf_info->flags & BCM_L3_INGRESS_REPLACE)) {
            /* Cannot overwrite if REPLACE flag is not set. */
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }

    } else {
        /* Get the first free index. */
        L3_IIF_FBMP_ITER(unit, l3_iif_idx) {
            break;
        }
        if (l3_iif_idx > L3_IIF_MAX(unit)) {
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }
    }

    SHR_IF_ERR_EXIT
        (l3_iif_set(unit, l3_iif_idx, intf_info));

    L3_IIF_FBMP_CLR(unit, l3_iif_idx);

    *intf_id = l3_iif_idx;

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_destroy(int unit, bcm_if_t intf_id)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));

    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_DELETE, &cfg));

    L3_IIF_FBMP_SET(unit, intf_id);

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_get(int unit, bcm_if_t intf_id,
                         bcm_l3_ingress_t *intf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (l3_iif_get(unit, intf_id, intf_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_find(int unit, bcm_l3_ingress_t *ing_intf,
                          bcm_if_t *intf_id)
{
    bcm_l3_ingress_t intf;
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ing_intf || !intf_id) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    *intf_id = BCM_IF_INVALID;

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_IIF_MIN(unit); i <= L3_IIF_MAX(unit); i++) {
        if (L3_IIF_FBMP_GET(unit, i)) {
            continue;
        }

        bcm_l3_ingress_t_init(&intf);
        SHR_IF_ERR_EXIT
            (l3_iif_get(unit, i, &intf));
        if ((intf.vrf == ing_intf->vrf) &&
            (intf.urpf_mode == ing_intf->urpf_mode)) {
            *intf_id = i;
            break;
        }
    }

    if (i > L3_IIF_MAX(unit)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_traverse(int unit,
                              bcm_l3_ingress_traverse_cb trav_fn,
                              void *user_data)
{
    bcm_l3_ingress_t intf_info;
    int i, rv;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!trav_fn) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_IIF_MIN(unit); i <= L3_IIF_MAX(unit); i++) {
        if (L3_IIF_FBMP_GET(unit, i)) {
            continue;
        }

        bcm_l3_ingress_t_init(&intf_info);
        SHR_IF_ERR_EXIT
            (l3_iif_get(unit, i, &intf_info));

        rv = trav_fn(unit, i, &intf_info, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_rp_op(int unit, int intf_id, int rp_id, int set)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));
    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE);

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    if (set) {
        fv[XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE] |= (1 << rp_id);
    } else {
        fv[XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE] &= (~(1 << rp_id));
    }

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_rp_get(int unit, int intf_id, uint32_t *rp_bmp)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));

    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    *rp_bmp = fv[XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE];

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_flexctr_attach(int unit, bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id) || !info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (info->source != bcmFlexctrSourceIngL3Intf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (l3_iif_flexctr_update(unit, intf_id, info->action_index));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ing_intf_flexctr_detach(int unit, bcm_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (l3_iif_flexctr_update(unit, intf_id,
                               BCMI_LTSW_FLEXCTR_ACTION_INVALID));

exit:
    SHR_FUNC_EXIT();

}

int
xfs_ltsw_l3_ing_intf_flexctr_info_get(int unit, bcm_if_t intf_id,
                                      bcmi_ltsw_flexctr_counter_info_t *info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id) || !info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_IIF, &ti));
    fv[XFS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    info->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->action_index = fv[XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION];
    info->source = bcmFlexctrSourceIngL3Intf;
    info->table_name = ti->name;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_l2cpu_intf_id_get(int unit, int *cpu_intf_id)
{

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    *cpu_intf_id = L3_INTF_L2CPU_IDX(unit);

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_id_range_get(int unit,
                                  bcmi_ltsw_l3_egr_intf_type_t type,
                                  int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    switch (type) {
    case BCMI_LTSW_L3_EGR_INTF_T_OL:
        *min = L3_OL_OIF_MIN(unit);
        *max = L3_OL_OIF_MAX(unit);
        break;
    case BCMI_LTSW_L3_EGR_INTF_T_UL:
        *min = L3_UL_OIF_OFFSET(unit) + L3_UL_OIF_MIN(unit);
        *max = L3_UL_OIF_OFFSET(unit) + L3_UL_OIF_MAX(unit);
        break;
    default:
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf, int *oif,
                              bcmi_ltsw_l3_egr_intf_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    if (!oif || !type || (intf <= BCM_IF_INVALID)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (L3_OL_OIF_VALID(unit, intf)) {
        *oif = intf;
        *type = BCMI_LTSW_L3_EGR_INTF_T_OL;
    } else if ((intf >= (L3_UL_OIF_OFFSET(unit) + L3_UL_OIF_MIN(unit))) &&
               (intf <= (L3_UL_OIF_OFFSET(unit) + L3_UL_OIF_MAX(unit)))) {
        *oif = intf - L3_UL_OIF_OFFSET(unit);
        *type = BCMI_LTSW_L3_EGR_INTF_T_UL;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_index_to_intf(int unit, int oif,
                              bcmi_ltsw_l3_egr_intf_type_t type,
                              bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf, SHR_E_MEMORY);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    switch (type) {
    case BCMI_LTSW_L3_EGR_INTF_T_OL:
        if (!L3_OL_OIF_VALID(unit, oif)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        *intf = oif;
        break;
    case BCMI_LTSW_L3_EGR_INTF_T_UL:
        if (!L3_UL_OIF_VALID(unit, oif)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        *intf = oif + L3_UL_OIF_OFFSET(unit);
        break;
    default:
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_create(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];
    int l3_oif_idx;
    bool locked = false;
    bool ul = false;
    SHR_BITDCL *fbmp;
    uint32_t max;
    bcmi_ltsw_l3_egr_intf_type_t type;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    ul = (intf_info->intf_flags & BCM_L3_INTF_UNDERLAY) ? true : false;
    fbmp = ul ? l3ii->ul_oif_fbmp : l3ii->ol_oif_fbmp;
    max = ul ? (l3ii->ul_oif_max + 1) : (l3ii->ol_oif_max + 1);

    L3_INTF_LOCK(unit);
    locked = true;
    if (intf_info->flags & (BCM_L3_WITH_ID | BCM_L3_REPLACE)) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egr_intf_to_index(unit, intf_info->intf_id,
                                           &l3_oif_idx, &type));

        if ((type == BCMI_LTSW_L3_EGR_INTF_T_UL) ^ ul) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        if (!(SHR_BITGET(fbmp, l3_oif_idx)) &&
            !(intf_info->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_EXIT(SHR_E_EXISTS);
        }

    } else {
        /* Get the first free index. */
        SHR_BIT_ITER(fbmp, max, l3_oif_idx) {
            break;
        }

        if (l3_oif_idx == max) {
            SHR_IF_ERR_EXIT(SHR_E_FULL);
        }
    }

    if (ul) {
        SHR_IF_ERR_EXIT
            (l3_oif_ul_set(unit, l3_oif_idx, intf_info));
    } else {
        SHR_IF_ERR_EXIT
            (l3_oif_ol_set(unit, l3_oif_idx, intf_info));
    }

    if (!(intf_info->flags & (BCM_L3_WITH_ID | BCM_L3_REPLACE))) {
        type = ul ? BCMI_LTSW_L3_EGR_INTF_T_UL : BCMI_LTSW_L3_EGR_INTF_T_OL;
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_egr_index_to_intf(unit, l3_oif_idx, type,
                                           &intf_info->intf_id));
    }

    SHR_BITCLR(fbmp, l3_oif_idx);

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_get(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    bcmi_ltsw_l3_egr_intf_type_t type;
    int l3_oif_idx;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (xfs_ltsw_l3_egr_intf_to_index(unit, intf_info->intf_id, &l3_oif_idx, &type));

    if (type == BCMI_LTSW_L3_EGR_INTF_T_OL) {
        SHR_IF_ERR_EXIT
            (l3_oif_ol_get(unit, l3_oif_idx, intf_info));
    } else if (type == BCMI_LTSW_L3_EGR_INTF_T_UL) {
        SHR_IF_ERR_EXIT
            (l3_oif_ul_get(unit, l3_oif_idx, intf_info));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_find(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    uint32_t vfi;
    uint64_t mac;
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, intf_info->vid, &vfi));

    bcmi_ltsw_util_mac_to_uint64(&mac, intf_info->mac_addr);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VFI);
    if (mac) {
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_OL_OIF_MIN(unit); i <= L3_OL_OIF_MAX(unit); i++) {
        if (L3_OL_OIF_FBMP_GET(unit, i)) {
            continue;
        }

        fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = i;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS] = 0;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_VFI] = 0;

        SHR_IF_ERR_EXIT
            (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

        if ((fv[XFS_LTSW_L3_INTF_TBL_EGR_VFI] == vfi) &&
            (fv[XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS] == mac)) {
            SHR_IF_ERR_EXIT
                (xfs_ltsw_l3_egr_index_to_intf(unit, i,
                                               BCMI_LTSW_L3_EGR_INTF_T_OL,
                                               &intf_info->intf_id));

            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_UL, &ti));

    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG);
    if (mac) {
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS);
    }

    for (i = L3_UL_OIF_MIN(unit); i <= L3_UL_OIF_MAX(unit); i++) {
        if (L3_UL_OIF_FBMP_GET(unit, i)) {
            continue;
        }

        fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = i;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS] = 0;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG] = 0;

        SHR_IF_ERR_EXIT
            (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

        if ((BCM_VLAN_CTRL_ID(fv[XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG]) == vfi)
            &&
            (fv[XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS] == mac)) {
            intf_info->intf_flags |= BCM_L3_INTF_UNDERLAY;
            SHR_IF_ERR_EXIT
                (xfs_ltsw_l3_egr_index_to_intf(unit, i,
                                               BCMI_LTSW_L3_EGR_INTF_T_UL,
                                               &intf_info->intf_id));

            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_delete(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    bcmi_ltsw_l3_egr_intf_type_t type;
    int l3_oif_idx;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (xfs_ltsw_l3_egr_intf_to_index(unit, intf_info->intf_id,
                                       &l3_oif_idx, &type));

    L3_INTF_LOCK(unit);
    locked = true;

    if (type == BCMI_LTSW_L3_EGR_INTF_T_OL) {
        SHR_IF_ERR_EXIT
            (l3_oif_ol_del(unit, l3_oif_idx));
    } else if (type == BCMI_LTSW_L3_EGR_INTF_T_UL) {
        SHR_IF_ERR_EXIT
            (l3_oif_ul_del(unit, l3_oif_idx));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_egr_intf_delete_all(int unit)
{
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_OL_OIF_MIN(unit); i <= L3_OL_OIF_MAX(unit); i++) {
        if (L3_OL_OIF_FBMP_GET(unit, i) || i == L3_INTF_L2CPU_IDX(unit)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (l3_oif_ol_del(unit, i));
    }

    for (i = L3_UL_OIF_MIN(unit); i <= L3_UL_OIF_MAX(unit); i++) {
        if (L3_UL_OIF_FBMP_GET(unit, i)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (l3_oif_ul_del(unit, i));
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    int l3_oif_idx;
    bcmi_ltsw_l3_egr_intf_type_t type;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (xfs_ltsw_l3_egr_intf_to_index(unit, intf_id, &l3_oif_idx, &type));

    SHR_IF_ERR_EXIT
        ((type != BCMI_LTSW_L3_EGR_INTF_T_OL) ? SHR_E_PARAM : SHR_E_NONE);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_oif_idx;
    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX);
    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));
    *tnl_idx = fv[XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX];

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_intf_vfi_update(int unit, int vid,
                            bcmi_ltsw_l3_intf_vfi_t *vfi_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bool locked = false;
    int i;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_UL, &ti));

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    L3_INTF_LOCK(unit);
    locked = true;

    l3_intf_vfi_info_update(unit, vid, vfi_info);

    if (!L3_VFI_REF_CNT(unit, vid)) {
        SHR_EXIT();
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_TAG) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG] = vfi_info->tag;
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG);
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_TAG_ACTION) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_L2_HDR_ACTION] = vfi_info->tag_action << 1;
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L2_HDR_ACTION);
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_MEMBERSHIP_PROFILE_IDX] =
            vfi_info->membership_prfl_idx;
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_MEMBERSHIP_PROFILE_IDX);
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_UNTAG_PROFILE_INDEX] =
            vfi_info->untag_prfl_idx;
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_UNTAG_PROFILE_INDEX);
    }

    if (vfi_info->flags & BCMI_LTSW_L3_INTF_VFI_F_STG) {
        fv[XFS_LTSW_L3_INTF_TBL_EGR_STG] = vfi_info->stg;
        cfg.fld_bmp |= (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_STG);
    }

    for (i = L3_UL_OIF_MIN(unit); i <= L3_UL_OIF_MAX(unit); i++) {
        if (L3_UL_OIF_FBMP_GET(unit, i) ||
            L3_UL_OIF_INFO(unit, i).vfi != vid) {
            continue;
        }

        fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = i;
        SHR_IF_ERR_EXIT
            (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_intf_mpls_recover_all(int unit,
                                  bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                  void *user_data)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bcmi_ltsw_l3_intf_mpls_t mpls_info;
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XFS_LTSW_L3_INTF_TBL_OIF_OL, &ti));

    cfg.fv = fv;
    cfg.fcnt = XFS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX);

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_OL_OIF_MIN(unit); i <= L3_OL_OIF_MAX(unit); i++) {
        if (L3_OL_OIF_FBMP_GET(unit, i) || i == L3_INTF_L2CPU_IDX(unit)) {
            continue;
        }

        fv[XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = i;
        fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX] = 0;

        SHR_IF_ERR_EXIT
            (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

        if (fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX] == 0) {
            continue;
        }

        sal_memset(&mpls_info, 0, sizeof(mpls_info));
        mpls_info.vc_label_index = fv[XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX];

        SHR_IF_ERR_EXIT
            (cb(unit, &mpls_info, user_data));
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
