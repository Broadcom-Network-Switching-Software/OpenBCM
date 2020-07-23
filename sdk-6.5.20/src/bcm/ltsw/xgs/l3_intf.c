/*! \file l3_intf.c
 *
 * XGS L3 ingress/egress interface management.
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
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/xgs/l3_intf.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/types.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/l3_egress_int.h>
#include <bcm_int/ltsw/ecn.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* Invalid DSCP ptr. */
#define INVALID_DSCP_PTR 63

/* Mask for all data fields. */
#define L3_INTF_FLD_ALL ((uint64_t)-1)

/* Generic data structure for L3 interface entry info. */
typedef struct l3_intf_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t *fv;

    /* Field array size. */
    uint32_t fcnt;
} l3_intf_cfg_t;

/* Data structure to save  L3_EIF  entry. */
typedef struct l3_eif_entry_s {
    /* Index. */
    uint64_t l3_eif_id;

    /* Bitmap of data fields to be operated. */
    uint64_t fld_bmp;
#define L3EIF_F_SRC_MAC                       (1<<0)
#define L3EIF_F_VLAN                          (1<<1)
#define L3EIF_F_TTL                           (1<<2)
#define L3EIF_F_L2_SWITCH                     (1<<3)
#define L3EIF_F_CLASS_ID                      (1<<4)
#define L3EIF_F_TNL_IPV4_ENCAP_INDEX          (1<<5)
#define L3EIF_F_TNL_IPV6_ENCAP_INDEX          (1<<6)
#define L3EIF_F_TNL_MPLS_ENCAP_INDEX          (1<<7)
#define L3EIF_F_TNL_TYPE                      (1<<8)
#define L3EIF_F_PHB_EGR_DSCP_ACTION           (1<<9)
#define L3EIF_F_DSCP                          (1<<10)
#define L3EIF_F_PHB_EGR_IP_INT_PRI_TO_DSCP_ID (1<<11)
#define L3EIF_F_PHB_EGR_L2_OTAG_ACTION        (1<<12)
#define L3EIF_F_PHB_OPRI                      (1<<13)
#define L3EIF_F_PHB_OCFI                      (1<<14)
#define L3EIF_F_PHB_EGR_L2_INT_PRI_TO_OTAG_ID (1<<15)
#define L3EIF_F_FLEX_CTR_ACTION               (1<<16)

    /* L2 header source MAC address of this L3 interface. */
    uint64_t src_mac;

    /* Layer 2 forwarding domain assigned based upon this L3 interface. */
    uint64_t vlan;

    /* TTL value. */
    uint64_t ttl;

    /* Enable l2 switch. */
    uint64_t l2_switch;

    /* Class ID. */
    uint64_t class_id;

    /* IPv4 tunnel encapsulation instance. */
    uint64_t tnl_ipv4_encap_id;

    /* IPv6 tunnel encapsulation instance. */
    uint64_t tnl_ipv6_encap_id;

    /* MPLS tunnel encapsulation instance. */
    uint64_t tnl_mpls_encap_id;

    /* tunnel type. */
    uint64_t tnl_type;

    /* DSCP action. */
    uint64_t dscp_action;

    /* DSCP value. */
    uint64_t dscp;

    /* DSCP map ID. */
    uint64_t dscp_map_id;

    /* DSCP action. */
    uint64_t l2_otag_action;

    /* Outer priority value. */
    uint64_t opri;

    /* Outer CFI value. */
    uint64_t ocfi;

    /* l2 otag map ID. */
    uint64_t l2_otag_map_id;

    /* Flex counter action. */
    uint64_t flex_ctr_action;
}  l3_eif_entry_t;

typedef struct l3_iif_ent_info_s {
    /* L3_IIF_PROFILE logical tbale index. */
    uint64_t l3_iif_profile_id;
} l3_iif_ent_info_t;

typedef struct l3_intf_info_s {
    /* L3 Interface manager initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Minimum iif index. */
    uint32_t iif_min;

    /* Maximum iif index. */
    uint32_t iif_max;

    /* Minimum vrf index. */
    uint32_t vrf_id_min;

    /* Maximum vrf index. */
    uint32_t vrf_id_max;

    /* Bitmap of free iif index. */
    SHR_BITDCL *iif_fbmp;

    /* L3 inteface entry info. */
    l3_iif_ent_info_t *l3_iif_ent;

    /* Minimum eif index. */
    uint32_t eif_min;

    /* Maximum eif index. */
    uint32_t eif_max;

    /* Bitmap of free overlay oif index. */
    SHR_BITDCL *eif_fbmp;

    /* Overlay L3 interface entries. */
    uint32_t ol_eif_ent;
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


#define L3_OL_EIF_SZ(_u) l3_intf_info[_u].ol_eif_ent
#define L3_EIF_MIN(_u) l3_intf_info[_u].eif_min
#define L3_EIF_MAX(_u) l3_intf_info[_u].eif_max
#define L3_UL_EIF_MAX(_u)   (L3_EIF_MAX(_u) - L3_OL_EIF_SZ(_u))
#define L3_UL_EIF_CHECK(_u, _idx) (_idx <= L3_UL_EIF_MAX(_u) ? true : false)

#define L3_EIF_TBL_SZ(_u) (L3_EIF_MAX(_u) - L3_EIF_MIN(_u) + 1)
#define L3_EIF_VALID(_u, _id)    \
    (((_id) >= L3_EIF_MIN(_u)) && ((_id) <= L3_EIF_MAX(_u)))

#define L3_IIF_FBMP_GET(_u, _id)    \
    SHR_BITGET(l3_intf_info[_u].iif_fbmp, (_id))

#define L3_IIF_FBMP_SET(_u, _id)    \
    SHR_BITSET(l3_intf_info[_u].iif_fbmp, (_id))

#define L3_IIF_FBMP_CLR(_u, _id)    \
    SHR_BITCLR(l3_intf_info[_u].iif_fbmp, (_id))

#define L3_IIF_FBMP_ITER(_u, _id)   \
    SHR_BIT_ITER(l3_intf_info[_u].iif_fbmp, (L3_IIF_MAX(unit) + 1), _id)

#define L3_EIF_FBMP_GET(_u, _id) \
    SHR_BITGET(l3_intf_info[_u].eif_fbmp, (_id))

#define L3_EIF_FBMP_SET(_u, _id) \
    SHR_BITSET(l3_intf_info[_u].eif_fbmp, (_id))

#define L3_EIF_FBMP_CLR(_u, _id) \
    SHR_BITCLR(l3_intf_info[_u].eif_fbmp, (_id))

/* _min should be alway 32 bit align . */
#define SHR_BIT_ITER_RANGE(_a, _min, _max, _b)               \
    for ((_b) = _min; (_b) < (_max); (_b)++)        \
        if ((_a)[(_b) / SHR_BITWID] == 0)        \
            (_b) += (SHR_BITWID - 1);            \
        else if (SHR_BITGET((_a), (_b)))

#define L3_VRF_MIN(_u) l3_intf_info[_u].vrf_id_min
#define L3_VRF_MAX(_u) l3_intf_info[_u].vrf_id_max


#define L3_VLAN_REF_CNT(_u, _vid) (l3_intf_info[_u].vlan_info[_vid].refcnt)

#define L3_VLAN_REF_CNT_INC(_u, _vid) (L3_VLAN_REF_CNT(_u, _vid)++)

#define L3_VLAN_REF_CNT_DEC(_u, _vid)       \
    do {                                    \
        if (L3_VLAN_REF_CNT(_u, _vid)) {    \
            L3_VLAN_REF_CNT(_u, _vid)--;    \
        }                                   \
    } while(0)


#define L3_IIF_ENT_PROFILE_ID(_u, _id) (l3_intf_info[_u].l3_iif_ent[_id].l3_iif_profile_id)

/* Reserve max idx of underlay egress interface for CPU. */
#define L3_INTF_L2CPU_IDX(_u) (L3_OL_EIF_SZ(_u) ? L3_UL_EIF_MAX(_u) : L3_EIF_MAX(_u))

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
    uint32_t i;
    const char *sym_val;
    int rv;

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
        if (!(cfg->fld_bmp & (1 << i)) && !fi[i].key) {
            continue;
        }

        if (fi[i].symbol) {
            rv = bcmlt_entry_field_symbol_get_by_id(eh, fi[i].fld_id,
                                                    &sym_val);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT(rv);
            }
            SHR_IF_ERR_EXIT
                (fi[i].symbol_to_scalar(unit, sym_val, &cfg->fv[i]));
        } else {
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmlt_entry_field_get_by_id(eh, fi[i].fld_id, &cfg->fv[i]),
                SHR_E_NOT_FOUND);
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
    int l3_ip_option_control_profile_id;
    int ecn_map_id;
    uint32_t prof_id;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (fv[XGS_LTSW_L3_INTF_TBL_ING_L3_OVERRIDE_IP_MC_DO_VLAN]) {
        intf_info->flags |= BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE;
    }

    if (!fv[XGS_LTSW_L3_INTF_TBL_ING_IPV4UC]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST;
    }

    if (!fv[XGS_LTSW_L3_INTF_TBL_ING_IPV6UC]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;
    }

    if (!fv[XGS_LTSW_L3_INTF_TBL_ING_IPV4MC]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST;
    }

    if (!fv[XGS_LTSW_L3_INTF_TBL_ING_IPV6MC]) {
        intf_info->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }

    if (fv[XGS_LTSW_L3_INTF_TBL_ING_INT_IFA]) {
        intf_info->flags |= BCM_L3_INGRESS_IFA;
    }

    if (fv[XGS_LTSW_L3_INTF_TBL_ING_INT_IOAM]) {
        intf_info->flags |= BCM_L3_INGRESS_IOAM;
    }

    if (fv[XGS_LTSW_L3_INTF_TBL_ING_INT_DATAPLANE]) {
            intf_info->flags |= BCM_L3_INGRESS_INT;
    }

    l3_ip_option_control_profile_id =
        fv[XGS_LTSW_L3_INTF_TBL_ING_L3_IP_OPTION_CONTROL_PROFILE_ID];
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_l3_ip4_options_profile_idx2id(unit,
                                                l3_ip_option_control_profile_id,
                                                &intf_info->ip4_options_profile_id));

    intf_info->vrf = (bcm_vrf_t)fv[XGS_LTSW_L3_INTF_TBL_ING_VRF];
    intf_info->intf_class = (int)fv[XGS_LTSW_L3_INTF_TBL_ING_CLASS_ID];
    intf_info->ipmc_intf_id = (bcm_vlan_t)fv[XGS_LTSW_L3_INTF_TBL_ING_L3_MC_IIF_ID];
    prof_id = (uint32_t)fv[XGS_LTSW_L3_INTF_TBL_ING_ECN_TNL_DECAP_ID];
    rv = bcmi_ltsw_ecn_map_id_construct(unit,
                                        BCMI_LTSW_ECN_TUNNEL_ECN_MAP_TYPE_TERM,
                                        prof_id,
                                        &ecn_map_id);
    if (SHR_SUCCESS(rv)) {
        intf_info->tunnel_term_ecn_map_id = ecn_map_id;
        intf_info->flags |= BCM_L3_INGRESS_TUNNEL_TERM_ECN_MAP;
    }

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
    int l3_ip_option_control_profile_id;
    uint32_t flags = intf_info->flags;
    int ecn_map_type;
    uint32_t prof_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (intf_info->flags & BCM_L3_INGRESS_TUNNEL_TERM_ECN_MAP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_ecn_map_id_resolve(unit, intf_info->tunnel_term_ecn_map_id,
                                          &ecn_map_type,
                                          &prof_id));
        if (ecn_map_type != BCMI_LTSW_ECN_TUNNEL_ECN_MAP_TYPE_TERM) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        fv[XGS_LTSW_L3_INTF_TBL_ING_ECN_TNL_DECAP_ID] = prof_id;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_ip4_options_profile_id2idx(unit,
                                                 intf_info->ip4_options_profile_id,
                                                 &l3_ip_option_control_profile_id));
    fv[XGS_LTSW_L3_INTF_TBL_ING_L3_IP_OPTION_CONTROL_PROFILE_ID] =
        l3_ip_option_control_profile_id;

    fv[XGS_LTSW_L3_INTF_TBL_ING_VRF] = intf_info->vrf;
    fv[XGS_LTSW_L3_INTF_TBL_ING_CLASS_ID] = intf_info->intf_class;
    fv[XGS_LTSW_L3_INTF_TBL_ING_L3_MC_IIF_ID] = intf_info->ipmc_intf_id;
    fv[XGS_LTSW_L3_INTF_TBL_ING_L3_OVERRIDE_IP_MC_DO_VLAN] =
        (flags & BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE) ? 1 : 0;
    fv[XGS_LTSW_L3_INTF_TBL_ING_IPV4UC] =
        (flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST) ? 0 : 1;
    fv[XGS_LTSW_L3_INTF_TBL_ING_IPV6UC] =
        (flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) ? 0 : 1;
    fv[XGS_LTSW_L3_INTF_TBL_ING_IPV4MC] =
        (flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST) ? 0 : 1;
    fv[XGS_LTSW_L3_INTF_TBL_ING_IPV6MC] =
        (flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST) ? 0 : 1;

    fv[XGS_LTSW_L3_INTF_TBL_ING_INT_IFA] =
        (flags & BCM_L3_INGRESS_IFA) ? 1 : 0;

    fv[XGS_LTSW_L3_INTF_TBL_ING_INT_IOAM] =
        (flags & BCM_L3_INGRESS_IOAM) ? 1 : 0;

    fv[XGS_LTSW_L3_INTF_TBL_ING_INT_DATAPLANE] =
        (flags & BCM_L3_INGRESS_INT) ? 1 : 0;

    cfg->fld_bmp =
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_INDEX) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_VRF) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_L3_IP_OPTION_CONTROL_PROFILE_ID) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_IPV4UC) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_IPV6UC) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_IPV4MC) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_IPV6MC) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_L3_MC_IIF_ID) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_L3_OVERRIDE_IP_MC_DO_VLAN) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_CLASS_ID) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_ECN_TNL_DECAP_ID) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_INT_IFA) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_INT_IOAM) |
        (1ULL << XGS_LTSW_L3_INTF_TBL_ING_INT_DATAPLANE);

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
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_IIF, &ti));

    /* Fill key and flexctr_action. */
    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = l3_iif_idx;
    fv[XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION] = action;

    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Update flex counter action in L3_EIF.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_eif_idx Entry index.
 * \param [in] action Flex counter action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_flexctr_update(int unit, int l3_eif_idx, uint32_t action)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));

    /* Fill key and flexctr_action. */
    fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_eif_idx;
    fv[XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION] = action;

    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L3 egress interface tunnel initiator.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_eif_idx Entry index.
 * \param [out] tnl_type Tunnel type.
 * \param [out] tnl_index Tunnel index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_tnl_initiator_get(
    int unit,
    int l3_eif_idx,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_index)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));
    fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = (unsigned int)l3_eif_idx;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = ti->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    if (fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] == BCMINT_LTSW_L3_INTF_TNL_TYPE_IPV4) {
        *tnl_type = bcmiTunnelTypeIpL3;
    } else if (fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] == BCMINT_LTSW_L3_INTF_TNL_TYPE_IPV6) {
        *tnl_type = bcmiTunnelTypeIp6L3;
    } else if (fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] == BCMINT_LTSW_L3_INTF_TNL_TYPE_MPLS) {
        *tnl_type = bcmiTunnelTypeMpls;
    } else {
        *tnl_type = bcmiTunnelTypeInvalid;
    }

    if (*tnl_type == bcmiTunnelTypeInvalid) {
        *tnl_index = 0;
    } else if (*tnl_type == bcmiTunnelTypeIpL3) {
        *tnl_index = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX];
    } else if (*tnl_type == bcmiTunnelTypeIp6L3) {
        *tnl_index = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX];
    } else if (*tnl_type == bcmiTunnelTypeMpls) {
        *tnl_index = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX];
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 egress interface tunnel initiator.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_eif_idx Entry index.
 * \param [in] tnl_type Tunnel type.
 * \param [in] tnl_index Tunnel index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_tnl_initiator_set(
    int unit,
    int l3_eif_idx,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_index)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));
    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = (unsigned int)l3_eif_idx;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    if (tnl_type == bcmiTunnelTypeIpL3) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] = BCMINT_LTSW_L3_INTF_TNL_TYPE_IPV4;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX] = tnl_index;
        cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX) |
                      (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE);
    } else if (tnl_type == bcmiTunnelTypeIp6L3) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] = BCMINT_LTSW_L3_INTF_TNL_TYPE_IPV6;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX] = tnl_index;
        cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX) |
                      (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE);
    } else if (tnl_type == bcmiTunnelTypeMpls) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] = BCMINT_LTSW_L3_INTF_TNL_TYPE_MPLS;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX] = tnl_index;
        cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX) |
                      (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE);
    } else {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] = BCMINT_LTSW_L3_INTF_TNL_TYPE_NONE;
        cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE);
    }

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    SHR_FUNC_EXIT();
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
l3_eif_cfg_to_info(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                   l3_intf_cfg_t *cfg, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    uint64_t *fv, fbmp;
    int qos_map_id;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    fv = cfg->fv;
    fbmp = ti->fld_bmp;

    fbmp &= (~((1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX) |
               (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX) |
               (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX) |
               (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE) |
               (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION) |
               (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE)));

    bcmi_ltsw_util_uint64_to_mac(intf_info->mac_addr, &fv[XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA]);
    if (ltsw_feature(unit, LTSW_FT_VFI)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_virtual_vpnid_get(unit,
                                         (bcm_vlan_t)fv[XGS_LTSW_L3_INTF_TBL_EGR_VFI],
                                         &intf_info->vid));
    } else {
        intf_info->vid = (bcm_vlan_t)fv[XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID];
    }
    intf_info->ttl = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_TTL];
    intf_info->class_id = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_CLASS_ID];

    intf_info->flags |= fv[XGS_LTSW_L3_INTF_TBL_EGR_L2_SWITCH] ? BCM_L3_L2ONLY : 0;

    if (fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] ==
        BCMINT_LTSW_L3_INTF_DSCP_ACTION_NONE) {
        intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_COPY;
    } else if (fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] ==
               BCMINT_LTSW_L3_INTF_DSCP_ACTION_FIXED) {
        intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_SET;
        intf_info->dscp_qos.dscp = fv[XGS_LTSW_L3_INTF_TBL_EGR_DSCP];
    } else if (fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] ==
               BCMINT_LTSW_L3_INTF_DSCP_ACTION_MAP) {
        intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_REMARK;
        qos_map_id = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_construct(unit,
                                            qos_map_id,
                                            bcmiQosMapTypeL3Egress,
                                            &intf_info->dscp_qos.qos_map_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] ==
        BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_NONE) {
        intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY;
    } else if (fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] ==
               BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_FIXED) {
        intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET;
        intf_info->vlan_qos.cfi = (uint8_t)fv[XGS_LTSW_L3_INTF_TBL_EGR_OCFI];
        intf_info->vlan_qos.pri = (uint8_t)fv[XGS_LTSW_L3_INTF_TBL_EGR_OPRI];
    } else if (fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] ==
               BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_MAP) {
        intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK;
        intf_info->vlan_qos.qos_map_id =
            (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID];
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
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
l3_eif_info_to_cfg(int unit, const bcmint_ltsw_l3_intf_tbl_t *ti,
                   bcmint_ltsw_l3_egr_intf_t *intf_info, l3_intf_cfg_t *cfg)
{
    uint64_t *fv, fbmp;
    int qos_map_id;
    uint32_t vfi;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ti, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(cfg->fv, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    fv = cfg->fv;
    fbmp = ti->fld_bmp;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, intf_info->vid, &vfi));
    if (vfi < 4096) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_VFI] = vfi;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID] = intf_info->vid;
    } else {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_VFI] = vfi;
    }
    bcmi_ltsw_util_mac_to_uint64(&fv[XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA], intf_info->mac_addr);
    fv[XGS_LTSW_L3_INTF_TBL_EGR_TTL] = intf_info->ttl;
    fv[XGS_LTSW_L3_INTF_TBL_EGR_L2_SWITCH] = intf_info->flags & BCM_L3_L2ONLY ? 1 : 0;
    fv[XGS_LTSW_L3_INTF_TBL_EGR_CLASS_ID] = intf_info->class_id;

    if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_COPY) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] = BCMINT_LTSW_L3_INTF_DSCP_ACTION_NONE;
        fbmp &= (~((1ULL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID) |
                   (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_DSCP)));
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_SET) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] = BCMINT_LTSW_L3_INTF_DSCP_ACTION_FIXED;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_DSCP] = intf_info->dscp_qos.dscp;
        fbmp &= (~(1ULL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID));
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_REMARK) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] = BCMINT_LTSW_L3_INTF_DSCP_ACTION_MAP;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_resolve(unit,
                                          intf_info->dscp_qos.qos_map_id,
                                          NULL,
                                          &qos_map_id));
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID] = qos_map_id;
        fbmp &= (~(1ULL << XGS_LTSW_L3_INTF_TBL_EGR_DSCP));
    }

    if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_OTAG_ACTION] =
            BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_NONE;
        fbmp &= (~((1ULL << XGS_LTSW_L3_INTF_TBL_EGR_OPRI) |
                   (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_OCFI) |
                   (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID)));
    } else if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_OTAG_ACTION] =
            BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_FIXED;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_OPRI] = intf_info->vlan_qos.cfi;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_OCFI] = intf_info->vlan_qos.pri;
        fbmp &= (~(1ULL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID));
    } else if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK) {
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_OTAG_ACTION] = BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_MAP;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_resolve(unit,
                                          intf_info->vlan_qos.qos_map_id,
                                          NULL,
                                          &qos_map_id));
        fv[XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID] = qos_map_id;
        fbmp &= (~((1ULL << XGS_LTSW_L3_INTF_TBL_EGR_OPRI) |
                   (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_OCFI) ));
    }

    /* Don't operate flexctr_action and tunnel index and key mode. */
    cfg->fld_bmp = fbmp & (~((1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX) |
                             (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX) |
                             (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX) |
                             (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE) |
                             (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION) |
                             (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE)));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get L3 egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_eif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_get(int unit, int l3_eif_idx, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));

    fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_eif_idx;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = ti->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_eif_cfg_to_info(unit, ti, &cfg, intf_info));

    if (ltsw_feature(unit, LTSW_FT_L3_HIER)) {
        intf_info->intf_flags |= L3_UL_EIF_CHECK(unit, l3_eif_idx) ?
                                 0 : BCM_L3_INTF_OVERLAY;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_eif_idx Entry index.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_set(int unit, int l3_eif_idx, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bcmlt_opcode_t op;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));

    fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_eif_idx;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    SHR_IF_ERR_EXIT
        (l3_eif_info_to_cfg(unit, ti, intf_info, &cfg));

    if (L3_EIF_FBMP_GET(unit, l3_eif_idx)) {
        op = BCMLT_OPCODE_INSERT;
    } else {
        op = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, op, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_eif_idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_del(int unit, int l3_eif_idx)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    if (L3_EIF_FBMP_GET(unit, l3_eif_idx)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));

    fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = l3_eif_idx;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_DELETE, &cfg));

    L3_EIF_FBMP_SET(unit, l3_eif_idx);

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
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    int intf_id;

    SHR_FUNC_ENTER(unit);

    ti = (const bcmint_ltsw_l3_intf_tbl_t *)cookie;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = 1 << XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID;

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, &cfg));

    intf_id = (int)cfg.fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX];
    L3_IIF_ENT_PROFILE_ID(unit, intf_id) =
        (int)cfg.fv[XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID];

    L3_IIF_FBMP_CLR(unit, (int)fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX]);

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
l3_eif_recover_cb(int unit, const char *lt_name, bcmlt_entry_handle_t eh,
                  void *cookie)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti;
    l3_intf_cfg_t cfg = {0};
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};

    SHR_FUNC_ENTER(unit);

    ti = (const bcmint_ltsw_l3_intf_tbl_t *)cookie;

    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, &cfg));

    L3_EIF_FBMP_CLR(unit, fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX]);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Hash callback function for L3_IIF profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries L3_IIF profile entry.
 * \param [in] entries_per_set Number of entries per set.
 * \param [out] hash Hash value returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_iif_profile_entry_hash_cb(
    int unit,
    void *entries,
    int entries_per_set,
    uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(hash, SHR_E_PARAM);

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(bcmi_ltsw_l3_iif_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*! TM stat type strings. */
static const char *tm_stat_type_str[] = {
    NONEs,
    CURRENT_USAGE_CELLSs,
    MAX_USAGE_CELLSs,
    CURRENT_AVAILABLE_CELLSs,
    MIN_AVAILABLE_CELLSs
};

typedef enum tm_stat_type_s {
    BCMINT_LTSW_TM_STAT_TYPE_NONE = 0,
    BCMINT_LTSW_TM_STAT_TYPE_CURRENT_USAGE = 1,
    BCMINT_LTSW_TM_STAT_TYPE_MAX_USAGE = 2,
    BCMINT_LTSW_TM_STAT_TYPE_CURRENT_AVAIL = 3,
    BCMINT_LTSW_TM_STAT_TYPE_MIN_AVAIL = 4
} tm_stat_type_t;


/*!
 * \brief Retrieve a L3_IIF profile.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Index.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_iif_profile_entry_get(
    int unit,
    int idx,
    bcmi_ltsw_l3_iif_profile_t *prof_entry)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t data;
    const char *tm_stat[BCMI_LTSW_L3_INTF_MAX_TM_STAT];
    uint32_t tm_stat_cnt;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IIF_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L3_IIF_PROFILE_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDs, &data));
    prof_entry->dscp_ptr = (uint32_t) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ALLOW_GLOBAL_ROUTEs, &data));
    prof_entry->global_route = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, IPV4_UNKNOWN_MC_TO_CPUs, &data));
    prof_entry->unknown_ipmcv4_to_cpu = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, IPV6_UNKNOWN_MC_TO_CPUs, &data));
    prof_entry->unknown_ipmcv6_to_cpu = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, IPV6_ROUTING_HDR_TYPE_0_DROPs, &data));
    prof_entry->ipv6_type0_drop = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UNRESOLVED_SIP_TO_CPUs, &data));
    prof_entry->unknown_src_to_cpu = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ICMP_REDIRECT_PKT_TO_CPUs, &data));
    prof_entry->icmp_redirect_to_cpu = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, IP_UNKNOWN_MC_AS_L2_MCs, &data));
    prof_entry->unknown_ipmc_as_l2mc = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(entry_hdl, TM_STATs, 0, tm_stat,
                                            BCMI_LTSW_L3_INTF_MAX_TM_STAT,
                                            &tm_stat_cnt));
    for (i = 0; i < tm_stat_cnt; i++) {
        if (!sal_strcmp(tm_stat_type_str[0], tm_stat[i])) {
            prof_entry->tm_stat_type[i] =
                BCMINT_LTSW_TM_STAT_TYPE_NONE;
        } else if (!sal_strcmp(tm_stat_type_str[1], tm_stat[i])) {
            prof_entry->tm_stat_type[i] =
                BCMINT_LTSW_TM_STAT_TYPE_CURRENT_USAGE;
        } else if (!sal_strcmp(tm_stat_type_str[2], tm_stat[i])) {
            prof_entry->tm_stat_type[i] =
                BCMINT_LTSW_TM_STAT_TYPE_MAX_USAGE;
        } else if (!sal_strcmp(tm_stat_type_str[3], tm_stat[i])) {
            prof_entry->tm_stat_type[i] =
                BCMINT_LTSW_TM_STAT_TYPE_CURRENT_AVAIL;
        } else if (!sal_strcmp(tm_stat_type_str[4], tm_stat[i])) {
            prof_entry->tm_stat_type[i] =
                BCMINT_LTSW_TM_STAT_TYPE_MIN_AVAIL;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for L3_IIF profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries L3_IIF profile entry.
 * \param [in] entries_per_set Number of entries per set.
 * \param [index] Index of LT entry.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_iif_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int index,
    int *cmp_result)
{
    bcmi_ltsw_l3_iif_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    sal_memset(&prof_entry, 0, sizeof(prof_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_entry_get(unit, index, &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set a L3_IIF profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Index.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_iif_profile_entry_set(
    int unit,
    int idx,
    bcmi_ltsw_l3_iif_profile_t *prof_entry)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;
    const char *tm_stat[BCMI_LTSW_L3_INTF_MAX_TM_STAT];
    uint32_t tm_stat_cnt;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IIF_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L3_IIF_PROFILE_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDs,
                               prof_entry->dscp_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ALLOW_GLOBAL_ROUTEs,
                               prof_entry->global_route));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IPV4_UNKNOWN_MC_TO_CPUs,
                               prof_entry->unknown_ipmcv4_to_cpu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IPV6_UNKNOWN_MC_TO_CPUs,
                               prof_entry->unknown_ipmcv6_to_cpu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IPV6_ROUTING_HDR_TYPE_0_DROPs,
                               prof_entry->ipv6_type0_drop));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UNRESOLVED_SIP_TO_CPUs,
                               prof_entry->unknown_src_to_cpu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ICMP_REDIRECT_PKT_TO_CPUs,
                               prof_entry->icmp_redirect_to_cpu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IP_UNKNOWN_MC_AS_L2_MCs,
                               prof_entry->unknown_ipmc_as_l2mc));
    tm_stat_cnt = BCMI_LTSW_L3_INTF_MAX_TM_STAT;
    for (i = 0; i < tm_stat_cnt; i++) {
        if (prof_entry->tm_stat_type[i] ==
            BCMINT_LTSW_TM_STAT_TYPE_NONE) {
            tm_stat[i] = tm_stat_type_str[0];
        } else if (prof_entry->tm_stat_type[i] ==
                   BCMINT_LTSW_TM_STAT_TYPE_CURRENT_USAGE) {
            tm_stat[i] = tm_stat_type_str[1];
        } else if (prof_entry->tm_stat_type[i] ==
                   BCMINT_LTSW_TM_STAT_TYPE_MAX_USAGE) {
            tm_stat[i] = tm_stat_type_str[2];
        } else if (prof_entry->tm_stat_type[i] ==
                   BCMINT_LTSW_TM_STAT_TYPE_CURRENT_AVAIL) {
            tm_stat[i] = tm_stat_type_str[3];
        } else if (prof_entry->tm_stat_type[i] ==
                   BCMINT_LTSW_TM_STAT_TYPE_MIN_AVAIL) {
            tm_stat[i] = tm_stat_type_str[4];
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_add(entry_hdl, TM_STATs, 0, tm_stat,
                                            tm_stat_cnt));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L3_IIF profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_iif_profile_entry_del(
    int unit,
    int idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IIF_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L3_IIF_PROFILE_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L3_IIF profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entry L3_IIF profile.
 * \param [out] index Index of L3_IIF profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
l3_iif_profile_add(
    int unit,
    bcmi_ltsw_l3_iif_profile_t *entry,
    int *index)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_L3_IIF,
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

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_entry_set(unit, *index, entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L3 ingress interface profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_iif_profile_id Profile ID.
 * \param [in] L3 intf info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
l3_iif_profile_get(
    int unit,
    int l3_iif_profile_id,
    bcm_l3_ingress_t *intf_info)
{
    bcmi_ltsw_l3_iif_profile_t entry;
    int rv;
    int val;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_entry_get(unit, l3_iif_profile_id, &entry));

    if (entry.dscp_ptr != INVALID_DSCP_PTR) {

        intf_info->flags |= BCM_L3_INGRESS_DSCP_TRUST;
        val = entry.dscp_ptr;
        rv = bcmi_ltsw_qos_map_id_construct(unit, val,
                                            bcmiQosMapTypeL3Ingress,
                                            &intf_info->qos_map_id);
        if (SHR_FAILURE(rv)) {
            intf_info->qos_map_id = 0;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Failed to construct QoS map id for "
                                    "DSCP ptr %d: rv = %d\n"),
                         val, rv));
        }
    }

    if (entry.global_route) {
        intf_info->flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    if (entry.unknown_ipmcv4_to_cpu) {
        intf_info->flags |= BCM_L3_INGRESS_UNKNOWN_IP4_MCAST_TOCPU;
    }

    if (entry.unknown_ipmcv6_to_cpu) {
        intf_info->flags |= BCM_L3_INGRESS_UNKNOWN_IP6_MCAST_TOCPU;
    }

    if (entry.ipv6_type0_drop) {
        intf_info->flags |= BCM_L3_INGRESS_IPV6_ROUTING_HEADER_TYPE_0_DROP;
    }

    if (entry.unknown_src_to_cpu) {
        intf_info->flags |= BCM_L3_INGRESS_UNKNOWN_SRC_TOCPU;
    }

    if (entry.icmp_redirect_to_cpu) {
        intf_info->flags |= BCM_L3_INGRESS_ICMP_REDIRECT_TOCPU;
    }

    if (entry.unknown_ipmc_as_l2mc) {
        intf_info->flags |= BCM_L3_INGRESS_UNKNOWN_IPMC_AS_L2MC;
    }

    for (i = 0; i < BCMI_LTSW_L3_INTF_MAX_TM_STAT; i++) {
        intf_info->int_cosq_stat[i] = entry.tm_stat_type[i];
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress encapsulation sequence number profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
l3_iif_profile_delete(
    int unit,
    int index)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_L3_IIF,
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
        (l3_iif_profile_entry_del(unit, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover egress encapsulation sequence number profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
l3_iif_profile_recover(
    int unit,
    int index)
{
    uint32_t ref_cnt;
    uint64_t idx_min, idx_max;
    bcmi_ltsw_l3_iif_profile_t profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IIF_PROFILEs,
                                       L3_IIF_PROFILE_IDs,
                                       &idx_min, &idx_max));

    if ((index < idx_min) || (index > idx_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, BCMI_LTSW_PROFILE_L3_IIF,
                                         index, &ref_cnt));
    if (ref_cnt == 0) {
        /* If profile entry has not been initialized. */
        sal_memset(&profile, 0, sizeof(profile));
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_iif_profile_entry_get(unit, index, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit,
                                           BCMI_LTSW_PROFILE_L3_IIF,
                                           &profile, 1, index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit,
                                              BCMI_LTSW_PROFILE_L3_IIF,
                                              1, index, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 ingress interface profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] L3 intf info.
 * \param [in/out] l3_iif_profile_id Profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_profile_set(
    int unit,
    bcm_l3_ingress_t *intf_info,
    int *l3_iif_profile_id)
{
    int dscp_ptr;
    uint32_t flags;
    bcmi_ltsw_l3_iif_profile_t entry;
    int new_l3_iif_profile_id;
    int old_l3_iif_profile_id;
    int i;

    SHR_FUNC_ENTER(unit);

    flags = intf_info->flags;

    /* Retrieve L3_IIF_PROFILE_ID. */
    old_l3_iif_profile_id = *l3_iif_profile_id;
    sal_memset(&entry, 0, sizeof(entry));
    /* Update profile entry info. */
    entry.global_route = (flags & BCM_L3_INGRESS_GLOBAL_ROUTE) ? 1 : 0;
    entry.unknown_ipmcv4_to_cpu = (flags & BCM_L3_INGRESS_UNKNOWN_IP4_MCAST_TOCPU) ? 1 : 0;
    entry.unknown_ipmcv6_to_cpu = (flags & BCM_L3_INGRESS_UNKNOWN_IP6_MCAST_TOCPU) ? 1 : 0;
    entry.ipv6_type0_drop = (flags & BCM_L3_INGRESS_IPV6_ROUTING_HEADER_TYPE_0_DROP) ? 1 : 0;
    entry.unknown_src_to_cpu = (flags & BCM_L3_INGRESS_UNKNOWN_SRC_TOCPU) ? 1 : 0;
    entry.icmp_redirect_to_cpu = (flags & BCM_L3_INGRESS_ICMP_REDIRECT_TOCPU) ? 1 : 0;
    entry.unknown_ipmc_as_l2mc = (flags & BCM_L3_INGRESS_UNKNOWN_IPMC_AS_L2MC) ? 1 : 0;

    for (i = 0; i < BCMI_LTSW_L3_INTF_MAX_TM_STAT; i++) {
        entry.tm_stat_type[i] = intf_info->int_cosq_stat[i];
    }
    if (flags & BCM_L3_INGRESS_DSCP_TRUST) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_resolve(unit, intf_info->qos_map_id,
                                          NULL, &dscp_ptr));
        entry.dscp_ptr = dscp_ptr;
    } else {
        /* value = 63 means not trust dscp_ptr. */
        entry.dscp_ptr = INVALID_DSCP_PTR;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_add(unit, &entry, &new_l3_iif_profile_id));
    if (old_l3_iif_profile_id != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_iif_profile_delete(unit, old_l3_iif_profile_id));
    }

    /* Update l3_iif_profile_id. */
    *l3_iif_profile_id = new_l3_iif_profile_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 ingress interface profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] cfg Entry info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_iif_profile_del(int unit, uint64_t l3_iif_profile_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_delete(unit, (int)l3_iif_profile_id));
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Initialize L3_IIF profiles.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_iif_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_L3_IIF;
    uint64_t idx_max, idx_min;
    int ent_idx_min, ent_idx_max;
    int idx;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    int dunit;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IIF_PROFILEs,
                                       L3_IIF_PROFILE_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    l3_iif_profile_entry_hash_cb,
                                    l3_iif_profile_entry_cmp_cb));

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IIF_PROFILEs));
    }

    if (bcmi_warmboot_get(unit)) {
        /* Recover */
        dunit = bcmi_ltsw_dev_dunit(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_IIFs, &entry_hdl));
        while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                          BCMLT_OPCODE_TRAVERSE,
                                          BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, L3_IIF_PROFILE_IDs, &data));
            idx = data;
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_iif_profile_recover(unit, idx));
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
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
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_IIF, &ti));

    /* Key. */
    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;

    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = ti->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_iif_cfg_to_info(unit, ti, &cfg, intf_info));

    /* Get info from L3_IIF_PROFILE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_get(unit,
                            L3_IIF_ENT_PROFILE_ID(unit, intf_id),
                            intf_info));

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
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    bcmlt_opcode_t op;
    int l3_iif_profile_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_IIF, &ti));

    /* Key. */
    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = l3_iif_idx;

    /* Retrieve old L3_IIF_PROFILE_ID. */
    l3_iif_profile_id = (int)(L3_IIF_ENT_PROFILE_ID(unit, l3_iif_idx));

    /* Get L3_IIF_PROFILE index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_set(unit, intf_info, &l3_iif_profile_id));

    fv[XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID] = l3_iif_profile_id;

    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;

    /* Populate data fields and bitmap of field to be operated. */
    SHR_IF_ERR_EXIT
        (l3_iif_info_to_cfg(unit, ti, intf_info, &cfg));

    op = L3_IIF_FBMP_GET(unit, l3_iif_idx) ? BCMLT_OPCODE_INSERT :
                                             BCMLT_OPCODE_UPDATE;

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, op, &cfg));

    L3_IIF_ENT_PROFILE_ID(unit, l3_iif_idx) = l3_iif_profile_id;

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
        uint64_t iif[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT];
        uint64_t eif[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT];
    } fv;
    l3_intf_cfg_t cfg = {0};
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t stat_ctr_id;

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < XGS_LTSW_L3_INTF_TBL_CNT; i++) {
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_intf_tbl_get(unit, i, &ti));

        if (ti->attr & XGS_LTSW_L3_INTF_TBL_ATTR_ING) {
            cfg.fv = fv.iif;
            cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
            cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION);
        } else {
            cfg.fv = fv.eif;
            cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
            cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION);
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

        while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            sal_memset(&fv, 0, sizeof(fv));

            SHR_IF_ERR_EXIT
                (lt_eh_parse(unit, eh, ti, &cfg));

            if (ti->attr & XGS_LTSW_L3_INTF_TBL_ATTR_ING) {
                if (cfg.fv[XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION]) {
                    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
                    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
                    ci.action_index =
                        cfg.fv[XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION];
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
                if (cfg.fv[XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION]) {
                    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
                    ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;
                    ci.action_index =
                        cfg.fv[XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION];
                    ci.source = bcmFlexctrSourceEgrL3Intf;
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
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set L3 egress interface adaptation key field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] key_type Key type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eif_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));
    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = (unsigned int)intf_id;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;

    fv[XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE] = key_type;
    cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
xgs_ltsw_l3_intf_init(int unit)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];
    const bcmint_ltsw_l3_intf_tbl_t *ti_iif, *ti_eif;
    const char *key_iif, *key_eif, *fld_vrf;
    uint64_t min, max, sz;
    bcmint_ltsw_l3_ol_tbl_info_t info;

    SHR_FUNC_ENTER(unit);

    if (l3ii->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_intf_deinit(unit));
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_IIF, &ti_iif));
    key_iif = ti_iif->flds[XGS_LTSW_L3_INTF_TBL_ING_INDEX].fld_name;
    fld_vrf = ti_iif->flds[XGS_LTSW_L3_INTF_TBL_ING_VRF].fld_name;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti_eif));
    key_eif = ti_eif->flds[XGS_LTSW_L3_INTF_TBL_EGR_INDEX].fld_name;

    sal_memset(l3ii, 0, sizeof(l3_intf_info_t));

    l3ii->mutex = sal_mutex_create("bcmXgsL3intfMutex");
    SHR_NULL_CHECK(l3ii->mutex, SHR_E_MEMORY);
    /* alloc memory for L3_IIF info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, ti_iif->name, fld_vrf,
                                       &min, &max));
    l3ii->vrf_id_min = min;
    l3ii->vrf_id_max = max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, ti_iif->name, key_iif,
                                       &min, &max));
    l3ii->iif_min = min + 1;
    l3ii->iif_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    SHR_ALLOC(l3ii->iif_fbmp, sz, "bcmXgsL3IifFreeBmp");
    SHR_NULL_CHECK(l3ii->iif_fbmp, SHR_E_MEMORY);
    SHR_BITSET_RANGE(l3ii->iif_fbmp, L3_IIF_MIN(unit), L3_IIF_TBL_SZ(unit));
    
    SHR_BITCLR_RANGE(l3ii->iif_fbmp, 0, 1);

    /* Alloc memory for l3_iif entry info. */
    sz = L3_IIF_TBL_SZ(unit) * sizeof(l3_iif_ent_info_t);
    SHR_ALLOC(l3ii->l3_iif_ent, sz, "bcmXgsL3IifEntInfo");
    SHR_NULL_CHECK(l3ii->l3_iif_ent, SHR_E_MEMORY);
    sal_memset(l3ii->l3_iif_ent, 0xff, sz);

    /* alloc memory for L3_EIF info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, ti_eif->name, key_eif,
                                       &min, &max));
    l3ii->eif_min = min + 1;
    l3ii->eif_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    SHR_ALLOC(l3ii->eif_fbmp, sz, "bcmXgsL3EifFreeBmp");
    SHR_NULL_CHECK(l3ii->eif_fbmp, SHR_E_MEMORY);
    SHR_BITSET_RANGE(l3ii->eif_fbmp, L3_EIF_MIN(unit),
                     L3_EIF_TBL_SZ(unit));
    
    SHR_BITCLR_RANGE(l3ii->eif_fbmp, 0, 1);

    /* Init L3_IIF_PROFILE. */
    SHR_IF_ERR_VERBOSE_EXIT(l3_iif_profile_init(unit));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_traverse(unit, ti_iif->name, (void *)ti_iif,
                                    l3_iif_recover_cb));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_traverse(unit, ti_eif->name, (void *)ti_eif,
                                    l3_eif_recover_cb));
    }

    if (!bcmi_warmboot_get(unit)) {
        if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
            L3_OL_EIF_SZ(unit) = bcmi_ltsw_property_get(unit, BCMI_CPN_RIOT_OVERLAY_L3_INTF_MEM_SIZE, 0);
            info.ol_ecmp_member_num = -1;
            info.ol_egr_obj_num = -1;
            info.ol_eif_num = L3_OL_EIF_SZ(unit);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_l3_overlay_table_init(unit, &info));
        }
    }

    l3ii->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(l3ii->iif_fbmp);
        SHR_FREE(l3ii->eif_fbmp);
        if (l3ii->mutex) {
            sal_mutex_destroy(l3ii->mutex);
        }
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_intf_deinit(int unit)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!l3ii->inited || !l3ii->mutex) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(l3ii->mutex, SAL_MUTEX_FOREVER);

    l3ii->inited = 0;
    SHR_FREE(l3ii->iif_fbmp);
    SHR_FREE(l3ii->eif_fbmp);
    sal_mutex_give(l3ii->mutex);
    sal_mutex_destroy(l3ii->mutex);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_intf_tables_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    *min = L3_IIF_MIN(unit);
    *max = L3_IIF_MAX(unit);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width)
{
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_def_get(unit, L3_IIFs, CLASS_IDs, &fld_def));

    if (min) {
        *min = fld_def.min;
    }

    if (max) {
        *max = fld_def.max;
    }

    if (width) {
        *width = fld_def.width;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ing_intf_create(int unit, bcm_l3_ingress_t *intf_info,
                            bcm_if_t *intf_id)
{
    int l3_iif_idx = 0;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
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
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }

    } else {
        /* Get the first free index. */
        L3_IIF_FBMP_ITER(unit, l3_iif_idx) {
            break;
        }
        if (l3_iif_idx == L3_IIF_TBL_SZ(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
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
xgs_ltsw_l3_ing_intf_destroy(int unit, bcm_if_t intf_id)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_IIF, &ti));

    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;

    L3_INTF_LOCK(unit);
    locked = true;

    /* Delete related L3_IIF_PROFILE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_profile_del(unit, L3_IIF_ENT_PROFILE_ID(unit, intf_id)));

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_DELETE, &cfg));

    L3_IIF_ENT_PROFILE_ID(unit, intf_id) = -1;

    L3_IIF_FBMP_SET(unit, intf_id);

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ing_intf_get(int unit, bcm_if_t intf_id,
                         bcm_l3_ingress_t *intf_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_get(unit, intf_id, intf_info));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ing_intf_find(int unit, bcm_l3_ingress_t *ing_intf,
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
        if (intf.vrf == ing_intf->vrf) {
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
xgs_ltsw_l3_ing_intf_traverse(int unit,
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

        /* Get info from L3_IIF_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_iif_profile_get(unit, L3_IIF_ENT_PROFILE_ID(unit, i), &intf_info));

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
xgs_ltsw_l3_ing_intf_flexctr_attach(int unit, bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id) || !info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (info->source != bcmFlexctrSourceIngL3Intf) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_flexctr_update(unit, intf_id, info->action_index));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ing_intf_flexctr_detach(int unit, bcm_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_iif_flexctr_update(unit, intf_id,
                               BCMI_LTSW_FLEXCTR_ACTION_INVALID));

exit:
    SHR_FUNC_EXIT();

}

int
xgs_ltsw_l3_ing_intf_flexctr_info_get(int unit, bcm_if_t intf_id,
                                      bcmi_ltsw_flexctr_counter_info_t *info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_IIF_VALID(unit, intf_id) || !info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (L3_IIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_IIF, &ti));
    fv[XGS_LTSW_L3_INTF_TBL_ING_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT;
    cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));


    info->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->action_index = (uint32_t)fv[XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION];
    info->source = bcmFlexctrSourceIngL3Intf;
    info->table_name = ti->name;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_l2cpu_intf_id_get(int unit, int *cpu_intf_id)
{

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    *cpu_intf_id = L3_INTF_L2CPU_IDX(unit);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_id_range_get(int unit,
                                  bcmi_ltsw_l3_egr_intf_type_t type,
                                  int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    switch (type) {
    /* TH4 not support RIOT.  Just only have underlay egress interface*/
    case BCMI_LTSW_L3_EGR_INTF_T_OL:
        *min = L3_OL_EIF_SZ(unit) ? (L3_UL_EIF_MAX(unit) + 1) : -1;
        *max = L3_OL_EIF_SZ(unit) ? L3_EIF_MAX(unit) : -1;
        break;
    case BCMI_LTSW_L3_EGR_INTF_T_UL:
        *min = 0;
        *max = L3_OL_EIF_SZ(unit) ?
               L3_UL_EIF_MAX(unit) : L3_EIF_MAX(unit);
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf, int *eif,
                              bcmi_ltsw_l3_egr_intf_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    if (!eif || !type || (intf <= BCM_IF_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (L3_EIF_VALID(unit, intf)) {
        *eif = intf;
        if (L3_OL_EIF_SZ(unit)) {
            *type = L3_UL_EIF_CHECK(unit, intf) ? BCMI_LTSW_L3_EGR_INTF_T_UL : BCMI_LTSW_L3_EGR_INTF_T_OL;
        } else {
            *type = BCMI_LTSW_L3_EGR_INTF_T_UL;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_index_to_intf(int unit, int eif,
                              bcmi_ltsw_l3_egr_intf_type_t type,
                              bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf, SHR_E_MEMORY);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    switch (type) {
    case BCMI_LTSW_L3_EGR_INTF_T_UL:
    case BCMI_LTSW_L3_EGR_INTF_T_OL:
        if (!L3_EIF_VALID(unit, eif)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        *intf = eif;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_create(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];
    int l3_eif_idx;
    bool locked = false;
    bool ul = false;
    SHR_BITDCL *fbmp;
    uint32_t max, min;
    bcmi_ltsw_l3_egr_intf_type_t type;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    bool l3_hier = !ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY);

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (l3_hier) {
        ul = (intf_info->intf_flags & BCM_L3_INTF_OVERLAY) ? false : true;
        fbmp = l3ii->eif_fbmp;
        max = L3_OL_EIF_SZ(unit) ? (ul ? (L3_UL_EIF_MAX(unit) + 1) : (L3_EIF_MAX(unit) + 1)):
                                   (L3_EIF_MAX(unit) + 1);
        min = ul ? 0 : (L3_UL_EIF_MAX(unit) + 1);
    } else {
        fbmp = l3ii->eif_fbmp;
        max = l3ii->eif_max + 1;
        min = 0;
    }

    L3_INTF_LOCK(unit);
    locked = true;
    if (intf_info->flags & (BCM_L3_WITH_ID | BCM_L3_REPLACE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_egr_intf_to_index(unit, intf_info->intf_id,
                                           &l3_eif_idx, &type));

        if (((type == BCMI_LTSW_L3_EGR_INTF_T_UL) ^ ul) && l3_hier) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (!(SHR_BITGET(fbmp, l3_eif_idx)) &&
            !(intf_info->flags & BCM_L3_REPLACE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }

    } else {
        /* Get the first free index. */
        SHR_BIT_ITER_RANGE(fbmp, min, max, l3_eif_idx) {
            break;
        }

        if (l3_eif_idx == max) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_set(unit, l3_eif_idx, intf_info));

    if (!(intf_info->flags & (BCM_L3_WITH_ID | BCM_L3_REPLACE))) {
        type = (l3_eif_idx > L3_UL_EIF_MAX(unit)) ? BCMI_LTSW_L3_EGR_INTF_T_UL :
                                                    BCMI_LTSW_L3_EGR_INTF_T_OL;
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_egr_index_to_intf(unit, l3_eif_idx, type,
                                           &intf_info->intf_id));
    }

    mtu_cfg.index = l3_eif_idx;
    mtu_cfg.type = L3_MTU_TYPE_TUNNEL;
    mtu_cfg.mtu = intf_info->mtu ? intf_info->mtu : BCMI_LTSW_L3_MTU_DEFAULT_SIZE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_mtu_set(unit, &mtu_cfg));

    SHR_BITCLR(fbmp, l3_eif_idx);

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_get(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    bcmi_ltsw_l3_egr_intf_type_t type;
    int l3_eif_idx;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_l3_egr_intf_to_index(unit, intf_info->intf_id, &l3_eif_idx, &type));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_get(unit, l3_eif_idx, intf_info));

    mtu_cfg.index = l3_eif_idx;
    mtu_cfg.type = L3_MTU_TYPE_TUNNEL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_mtu_get(unit, &mtu_cfg));
    intf_info->mtu = (mtu_cfg.mtu != BCMI_LTSW_L3_MTU_DEFAULT_SIZE) ? mtu_cfg.mtu : 0;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_find(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};
    uint32_t vfi;
    uint64_t mac;
    int i;
    bool locked = false;
    bcmi_ltsw_l3_egr_intf_type_t type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, intf_info->vid, &vfi));

    bcmi_ltsw_util_mac_to_uint64(&mac, intf_info->mac_addr);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));

    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID);
    if (mac) {
        cfg.fld_bmp |= (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_EIF_MIN(unit); i <= L3_EIF_MAX(unit); i++) {
        if (L3_EIF_FBMP_GET(unit, i)) {
            continue;
        }

        fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = i;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA] = 0;
        fv[XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID] = 0;

        SHR_IF_ERR_EXIT
            (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

        if ((fv[XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID] == vfi) &&
            (fv[XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA] == mac)) {
            type = (i > L3_UL_EIF_MAX(unit)) ? BCMI_LTSW_L3_EGR_INTF_T_UL :
                                               BCMI_LTSW_L3_EGR_INTF_T_OL;
            SHR_IF_ERR_EXIT
                (xgs_ltsw_l3_egr_index_to_intf(unit, i,
                                               type,
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
xgs_ltsw_l3_egr_intf_delete(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info)
{
    bcmi_ltsw_l3_egr_intf_type_t type;
    int l3_eif_idx;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(intf_info, SHR_E_PARAM);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_l3_egr_intf_to_index(unit, intf_info->intf_id,
                                       &l3_eif_idx, &type));

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_del(unit, l3_eif_idx));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_delete_all(int unit)
{
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    for (i = L3_EIF_MIN(unit); i < L3_EIF_TBL_SZ(unit); i++) {
        if (L3_EIF_FBMP_GET(unit, i) ||
            (i == L3_INTF_L2CPU_IDX(unit)) ||
            (i == 0)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_eif_del(unit, i));
    }
exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_flexctr_attach(int unit, bcm_if_t intf_id,
                                    bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_EIF_VALID(unit, intf_id) || !info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (info->source != bcmFlexctrSourceEgrL3Intf) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_flexctr_update(unit, intf_id, info->action_index));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_egr_intf_flexctr_detach(int unit, bcm_if_t intf_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_EIF_VALID(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_flexctr_update(unit, intf_id,
                               BCMI_LTSW_FLEXCTR_ACTION_INVALID));

exit:
    SHR_FUNC_EXIT();

}

int
xgs_ltsw_l3_egr_intf_flexctr_info_get(int unit, bcm_if_t intf_id,
                                      bcmi_ltsw_flexctr_counter_info_t *info)
{
    const bcmint_ltsw_l3_intf_tbl_t *ti = NULL;
    uint64_t fv[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {0};
    l3_intf_cfg_t cfg = {0};

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!L3_EIF_VALID(unit, intf_id) || !info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_intf_tbl_get(unit, XGS_LTSW_L3_INTF_TBL_EIF, &ti));
    fv[XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = intf_id;
    cfg.fv = fv;
    cfg.fcnt = XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT;
    cfg.fld_bmp = (1ULL << XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_intf_lt_op(unit, ti, BCMLT_OPCODE_LOOKUP, &cfg));

    info->stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;
    info->action_index = (int)fv[XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION];
    info->source = bcmFlexctrSourceEgrL3Intf;
    info->table_name = ti->name;

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_intf_tnl_init_set(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_tnl_initiator_set(unit, intf_id, tnl_type, tnl_idx));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_tnl_initiator_get(unit, intf_id, tnl_type, tnl_idx));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_intf_tnl_init_reset(
     int unit,
     int intf_id)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_tnl_initiator_set(unit, intf_id,
                                  bcmiTunnelTypeInvalid, 0));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_intf_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if (L3_EIF_FBMP_GET(unit, intf_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_eif_adapt_lookup_key_set(unit, intf_id, key_type));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

