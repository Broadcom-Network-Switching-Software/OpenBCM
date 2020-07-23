/*! \file l3_fib.c
 *
 * XFS L3 Forwarding Information Base management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/l3.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/ipmc.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_aacl.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_fib_int.h>
#include <bcm_int/ltsw/xfs/types.h>
#include <bcm_int/ltsw/xfs/l3_fib.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/failover.h>

#include <sal/sal_mutex.h>
#include <sal/sal_time.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* ALPM MODE. */
#define ALPM_MODE_COMBINED  0
#define ALPM_MODE_PARALLEL  1
#define ALPM_MODE_NONE      2

/* IP type count. */
#define IP_TYPE_V4          0
#define IP_TYPE_V6          1
#define IP_TYPE_CNT         2

/* Encoding for misc_ctrl_1 for destination lookup:
 *     misc_ctrl_1[0:0]: If set, Packet discard;
 *     misc_ctrl_1[1:1]: If set, Packet copy to CPU;
 *     misc_ctrl_1[2:2]: If set, Destination IP is a local address;
 *     misc_ctrl_1[3:3]: If set, IPMC interface/rpa_id check fail to CPU;
 */
#define DST_DISCARD           (1 << 0)
#define DST_CP2CPU            (1 << 1)
#define DST_LOCAL             (1 << 2)
#define DST_IPMC_RPF_FAIL2CPU (1 << 3)

/* Encoding for misc_ctrl_1 for source lookup:
 *     misc_ctrl_1[2:0]: Unused;
 *     misc_ctrl_1[3:3]: If set, this entry is a default route for uRPF check;
 */
#define SRC_DEFROUTE          (1 << 3)

/* Route entry priority offset for IPV4. */
#define L3_ENT_PRI_OFFSET_IPV4     33
/* Route entry priority offset for IPV6 64. */
#define L3_ENT_PRI_OFFSET_IPV6_64  65
/* Route entry priority offset for IPV6 128. */
#define L3_ENT_PRI_OFFSET_IPV6_128 129

/* Drop reason = 0x62, drop strength = 0x4. */
#define L3_UC_IIF_EQ_OIF_DROP_CODE 0x462

/* Data structure to save the info of UC entry fields. */
typedef struct l3_uc_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT];
} l3_uc_cfg_t;

/* Data structure to save the info of MC entry fields. */
typedef struct l3_mc_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT];
} l3_mc_cfg_t;

/* Generic data structure contains the info of unicst/multicast entry. */
typedef struct l3_cfg_s {
    /* Indicate if cfg is for unicast or not. */
    bool uc;
    union {
        l3_uc_cfg_t uc;
        l3_mc_cfg_t mc;
    } cfg;
} l3_cfg_t;

/* L3 filter criteria info. */
typedef struct l3_filter_criteria_s {
    /* The starting valid entry number to be traversed. */
    uint32_t start;

    /* The last valid entry number to be traversed.*/
    uint32_t end;

    /* Negate a set. */
    int negate;

    /* Overlay L3 interface. */
    bcm_if_t ol_intf;

    /* Underlay L3 interface. */
    bcm_if_t ul_intf;

    /* VRF. */
    bcm_vrf_t vrf;

    /* IPv4 address. */
    uint32_t ip_addr;

    /* IPv4 address mask. */
    uint32_t ip_addr_mask;

    /* IPv6 address. */
    uint64_t ip6_addr[2];

    /* IPv6 address mask. */
    uint64_t ip6_addr_mask[2];

    /* NAT entry. */
    bool nat;

    /* L3 Flags. */
    uint32_t flags;

} l3_filter_criteria_t;

/* L3 filter resuslt. */
typedef enum l3_filter_cmd_e {
    /* Match the filter criteria. */
    BCMINT_LTSW_L3_FLT_PASS = 0,

    /* Mismatch the filter criteria, skip operation on the entry. */
    BCMINT_LTSW_L3_FLT_SKIP = 1,

    /* Mismatch the filter criteria, skip the entry without count. */
    BCMINT_LTSW_L3_FLT_SKIP_NO_CNT = 2,

    /* Mismatch the filter criteria, stop traverse. */
    BCMINT_LTSW_L3_FLT_STOP = 3
} l3_filter_cmd_t;

/* L3 filter callback definition. */
typedef int (*l3_filter_f)(int unit,
                           uint32_t index,
                           l3_filter_criteria_t *flt_crtr,
                           l3_cfg_t *cfg,
                           l3_filter_cmd_t *flt_cmd);

/* Definition of callback that constructs entry info from handle. */
typedef int (*l3_entry_hdl_to_cfg_f)(int unit,
                                     bcmlt_entry_handle_t eh,
                                     const bcmint_ltsw_l3_fib_tbl_t *ti,
                                     l3_cfg_t *cfg);

/* Definition of callback that constructs bcmi_ltsw_l3_fib_t. */
typedef int (*l3_cfg_to_fib_f)(int unit,
                               const bcmint_ltsw_l3_fib_tbl_t *ti,
                               l3_cfg_t *cfg,
                               bcmi_ltsw_l3_fib_t *fib);

/* L3 traverse control info. */
typedef struct l3_trav_t {

    /* Traverse operation. */
    uint32_t op;
#define L3_TRAV_OP_DEL  1
#define L3_TRAV_OP_AGE  2

    /* Filter callback function. */
    l3_filter_f filter;

    /* Filter criteria. */
    l3_filter_criteria_t flt_crtr;

    /* Callback to construct entry info from handle. */
    l3_entry_hdl_to_cfg_f hdl_to_cfg_cb;

    /* Callback to construct bcmi_ltsw_l3_fib_t. */
    l3_cfg_to_fib_f cfg_to_fib_cb;

    /* User callback function */
    bcm_l3_host_traverse_cb host_trav_cb;

    /* User callback function */
    bcm_l3_route_traverse_cb route_trav_cb;

    /* User callback function */
    bcm_ipmc_traverse_cb ipmc_trav_cb;

    /* Callback to construct ipmc info from fib. */
    bcmi_ltsw_l3_fib_to_ipmc_cb ipmc_addr_t_cb;

    /* User callback function */
    bcm_l3_nat_ingress_traverse_cb nat_trav_cb;

    /* Callback to construct nat info from fib. */
    bcmi_ltsw_l3_fib_to_nat_cb fib_to_nat_cb;

    /* Data provided by the user, cookie */
    void *user_data;

} l3_trav_t;

/* L3 FIB manager info. */
typedef struct l3_fib_info_s {
    /* L3 fib manager initialized. */
    uint8_t inited;

    /* Mutex */
    sal_mutex_t mutex;

    /* Max VRF. */
    uint16_t max_vrf_id;

    /* ALPM enabled. */
    bool alpm_enabled;

    /* ALPM mode. */
    int alpm_mode;

    /* URPF enabled. */
    bool urpf_enabled;

    /* Indicates which IPV6 MC defip tables are available. */
    uint8_t ipv6_mc_defip_tbl_bmp;
#define L3_FIB_IPV6_MC_DEFIP_64   (1 << 0)
#define L3_FIB_IPV6_MC_DEFIP_128  (1 << 1)

    /* ALPM route count per VRF per IP-type. */
    uint32_t *alpm_route_cnt[IP_TYPE_CNT];

    /* The max ECMP group index that could be used in an ALPM reduced entry.
     * The value must be a (power of 2) - 1. */
    uint32_t reduced_route_max_ecmp_grp;

} l3_fib_info_t;

static l3_fib_info_t l3_fib_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_FIB_INITED(_u) (l3_fib_info[_u].inited)

#define L3_FIB_LOCK(_u)     \
    sal_mutex_take(l3_fib_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_FIB_UNLOCK(_u)   \
    sal_mutex_give(l3_fib_info[_u].mutex)

#define L3_FIB_VRF_MAX(_u) (l3_fib_info[_u].max_vrf_id)
#define L3_FIB_VRF_GLO(_u) (L3_FIB_VRF_MAX(_u) + 1)
#define L3_FIB_VRF_GHI(_u) (L3_FIB_VRF_MAX(_u) + 2)
#define L3_FIB_VRF_CNT(_u) (L3_FIB_VRF_MAX(_u) + 3)

#define L3_FIB_URPF_ENABLED(_u) (l3_fib_info[_u].urpf_enabled)

#define L3_FIB_IPV6_MC_DEFIP_64_ENABLED(_u) \
    (l3_fib_info[_u].ipv6_mc_defip_tbl_bmp & L3_FIB_IPV6_MC_DEFIP_64)

#define L3_FIB_IPV6_MC_DEFIP_128_ENABLED(_u) \
    (l3_fib_info[_u].ipv6_mc_defip_tbl_bmp & L3_FIB_IPV6_MC_DEFIP_128)

#define L3_ALPM_ENABLED(_u) (l3_fib_info[_u].alpm_enabled)

#define L3_ALPM_MODE(_u) (l3_fib_info[_u].alpm_mode)

#define VRF_ALPM_ROUTE_CNT(_u, _vrf_id, _ipt)   \
    l3_fib_info[_u].alpm_route_cnt[_ipt][_vrf_id]

#define VRF_ALPM_ROUTE_INC(_u, _vrf_id, _ipt)   \
    l3_fib_info[_u].alpm_route_cnt[_ipt][_vrf_id]++

#define VRF_ALPM_ROUTE_DEC(_u, _vrf_id, _ipt)   \
    l3_fib_info[_u].alpm_route_cnt[_ipt][_vrf_id]--

#define L3_REDUCED_ROUTE_MAX_ECMP_GRP(_u)   \
    l3_fib_info[_u].reduced_route_max_ecmp_grp

/* Encoding for misc_ctrl_0:
 *      misc_ctrl_0[0:0]: If set, entry is valid;
 *      misc_ctrl_0[3:1]: Opaque info;
 */
#define L3_MISC_CTRL_0_OPQ_CTRL_ID_MAX (0x7)
#define L3_MISC_CTRL_0_OPQ_CTRL_ID(_c) (((_c) >> 1) & 0x7)
#define L3_MISC_CTRL_0(_opaque) (((_opaque & 0x7) << 1) | 1)


/******************************************************************************
 * Private functions
 */

/*!
 * \brief Route mode and default route validation.
 *
 * \param [in] unit Unit number.
 * \param [in] attrib Table attributes.
 * \param [in] flds ALPM entry fields.
 * \param [in] op Operaction code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_def_check(int unit, uint32_t attrib, uint64_t *flds, bcmlt_opcode_t op)
{
    int ipt, masklen;
    bcm_ip6_t ip6_mask;
    bcm_ip_t ip4_mask;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if ((L3_ALPM_MODE(unit) != ALPM_MODE_COMBINED) ||
        !(attrib & BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF)) {
        SHR_EXIT();
    }

    fv = &flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK];
    if (attrib & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
        ipt = IP_TYPE_V6;
        bcmi_ltsw_util_uint64_to_ip6(&ip6_mask, fv);
        masklen = bcm_ip6_mask_length(ip6_mask);
    } else {
        ipt = IP_TYPE_V4;
        ip4_mask = *fv;
        masklen = bcm_ip_mask_length(ip4_mask);
    }

    fv = &flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF];
    if (op == BCMLT_OPCODE_INSERT) {
        if (!VRF_ALPM_ROUTE_CNT(unit, (*fv), ipt) && masklen) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "First route in VRF %d has to be a default "
                                  "route in this ALPM mode.\n"), ((int)*fv)));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (op == BCMLT_OPCODE_DELETE) {
        if ((VRF_ALPM_ROUTE_CNT(unit, (*fv), ipt) > 1)  && !masklen) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Default route in VRF %d has to be the last "
                                  "route to delete in this ALPM mode.\n"),
                       ((int)*fv)));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the tables in FIB.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_tables_clear(int unit)
{
    int i, num;
    const char *ltname[] = {
        L3_IP_LOOKUP0_DEFAULT_TABLEs,
        L3_IP_LOOKUP1_DEFAULT_TABLEs,
        L3_IPV4_UNICAST_TABLEs,
        L3_IPV4_UNICAST_DEFIP_TABLEs,
        L3_IPV6_UNICAST_TABLEs,
        L3_IPV6_UNICAST_DEFIP_64_TABLEs,
        L3_IPV6_UNICAST_DEFIP_128_TABLEs,
        L3_IPV4_MULTICAST_1_TABLEs,
        L3_IPV4_MULTICAST_2_TABLEs,
        L3_IPV4_MULTICAST_DEFIP_TABLEs,
        L3_IPV6_MULTICAST_1_TABLEs,
        L3_IPV6_MULTICAST_2_TABLEs,
        L3_IPV6_MULTICAST_DEFIP_64_TABLEs,
        L3_IPV6_MULTICAST_DEFIP_128_TABLEs,
        L3_IPV4_UC_ROUTEs,
        L3_IPV4_UC_ROUTE_VRFs,
        L3_IPV4_UC_ROUTE_OVERRIDEs,
        L3_IPV6_UC_ROUTEs,
        L3_IPV6_UC_ROUTE_VRFs,
        L3_IPV6_UC_ROUTE_OVERRIDEs,
        L3_SRC_IPV4_UC_ROUTEs,
        L3_SRC_IPV4_UC_ROUTE_VRFs,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEs,
        L3_SRC_IPV6_UC_ROUTEs,
        L3_SRC_IPV6_UC_ROUTE_VRFs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEs
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

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill LT entry handle.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [in] cfg Data of unicast route entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_eh_fill(int unit, bcmlt_entry_handle_t eh, const bcmint_ltsw_l3_fib_tbl_t *ti,
           l3_cfg_t *cfg)
{
    const bcmint_ltsw_l3_fib_fld_t *fi = ti->flds;
    uint64_t *fv;
    const char *sym_val;
    uint32_t i, cnt;
    uint64_t fld_bmp;

    SHR_FUNC_ENTER(unit);

    if (cfg->uc) {
        cnt = BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT;
        fv = cfg->cfg.uc.flds;
        fld_bmp = cfg->cfg.uc.fld_bmp;
    } else {
        cnt = BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT;
        fv = cfg->cfg.mc.flds;
        fld_bmp = cfg->cfg.mc.fld_bmp;
    }

    for (i = 0; i < cnt; i++) {
        /* Skip fields that are invalid for the given LT. */
        if (!(ti->fld_bmp & (1 << i))) {
            continue;
        }

        /*
         * Skip fields that are not intended to be operated.
         * Key fields are always required.
         */
        if (!(fld_bmp & (1 << i)) && !fi[i].key) {
            continue;
        }

        if (fi[i].symbol) {
            SHR_IF_ERR_EXIT
                (fi[i].scalar_to_symbol(unit, fv[i], &sym_val));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_add_by_id(eh,
                                                    fi[i].fld_id,
                                                    sym_val));
        } else {
            if (fi[i].elements) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_add_by_id(eh,
                                                       fi[i].fld_id,
                                                       0,
                                                       &fv[i],
                                                       fi[i].elements));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add_by_id(eh, fi[i].fld_id, fv[i]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry handle of logical tables.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [out] cfg Data of unicast route entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_eh_parse(int unit, bcmlt_entry_handle_t eh, const bcmint_ltsw_l3_fib_tbl_t *ti,
            l3_cfg_t *cfg)
{
    const bcmint_ltsw_l3_fib_fld_t *fi = ti->flds;
    uint64_t *fv;
    uint32_t i, cnt, sz;
    const char *sym_val;
    uint64_t fld_bmp;

    SHR_FUNC_ENTER(unit);

    if (cfg->uc) {
        cnt = BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT;
        fv = cfg->cfg.uc.flds;
        fld_bmp = cfg->cfg.uc.fld_bmp;
    } else {
        cnt = BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT;
        fv = cfg->cfg.mc.flds;
        fld_bmp = cfg->cfg.mc.fld_bmp;
    }


    for (i = 0; i < cnt; i++) {
        /* Skip fields that are invalid for the given LT. */
        if (!(ti->fld_bmp & (1 << i))) {
            continue;
        }

        /*
         * Skip fields that are not intended to be operated.
         * Key fields are always required.
         */
        if (!(fld_bmp & (1 << i)) && !fi[i].key) {
            continue;
        }

        if (fi[i].symbol) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_get_by_id(eh, fi[i].fld_id,
                                                    &sym_val));
            SHR_IF_ERR_EXIT
                (fi[i].symbol_to_scalar(unit, sym_val, &fv[i]));
        } else {
            if (fi[i].elements) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_get_by_id(eh,
                                                       fi[i].fld_id,
                                                       0,
                                                       &fv[i],
                                                       fi[i].elements,
                                                       &sz));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_get_by_id(eh, fi[i].fld_id, &fv[i]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Filter NAT host entry.
 *
 * \param [in] unit Unit number
 * \param [in] index The valid entry number.
 * \param [in] flt_crtr Filter criteria info.
 * \param [in] cfg L3 entry info.
 * \param [out] flt_cmd Filter result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_host_filter_nat(int unit, uint32_t index,
                   l3_filter_criteria_t *flt_crtr,
                   l3_cfg_t *cfg,
                   l3_filter_cmd_t *flt_cmd)
{
    uint64 *fv;
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!cfg->uc) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fv = cfg->cfg.uc.flds;

    if ((fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] ||
         fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL]) ^ flt_crtr->nat) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP_NO_CNT;
    } else {
        *flt_cmd = BCMINT_LTSW_L3_FLT_PASS;

        if (flt_crtr->flags) {
            if (fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] & DST_DISCARD) {
                flags |= BCM_L3_DST_DISCARD;
            }

            if (fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] ==
                BCMI_XFS_DEST_TYPE_ECMP) {
                flags |= BCM_L3_MULTIPATH;
            }

            if ((flags & flt_crtr->flags) != flt_crtr->flags) {
                *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Filter with IPv4 prefix.
 *
 * \param [in] unit Unit number
 * \param [in] index The valid entry number.
 * \param [in] flt_crtr Filter criteria info.
 * \param [in] cfg L3 entry info.
 * \param [out] flt_cmd Filter result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_host_trav_filter_prefix_v4(int unit, uint32_t index,
                              l3_filter_criteria_t *flt_crtr,
                              l3_cfg_t *cfg,
                              l3_filter_cmd_t *flt_cmd)
{
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!cfg->uc) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fv = cfg->cfg.uc.flds;

    if ((fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] ||
         fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL]) ^ flt_crtr->nat) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP_NO_CNT;
    } else if ((flt_crtr->vrf == fv[BCMINT_LTSW_L3_FIB_TBL_UC_VRF]) &&
               ((flt_crtr->ip_addr & flt_crtr->ip_addr_mask) ==
                (fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] &
                 flt_crtr->ip_addr_mask))) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_PASS;
    } else {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Filter with IPv6 prefix.
 *
 * \param [in] unit Unit number
 * \param [in] index The valid entry number.
 * \param [in] flt_crtr Filter criteria info.
 * \param [in] cfg L3 entry info.
 * \param [out] flt_cmd Filter result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_host_trav_filter_prefix_v6(int unit, uint32_t index,
                              l3_filter_criteria_t *flt_crtr,
                              l3_cfg_t *cfg,
                              l3_filter_cmd_t *flt_cmd)
{
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!cfg->uc) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fv = cfg->cfg.uc.flds;

    if (((flt_crtr->ip6_addr[0] & flt_crtr->ip6_addr_mask[0]) ==
         (fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] & flt_crtr->ip6_addr_mask[0]))
        &&
        ((flt_crtr->ip6_addr[1] & flt_crtr->ip6_addr_mask[1]) ==
         (fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] & flt_crtr->ip6_addr_mask[1]))
        &&
        (flt_crtr->vrf == fv[BCMINT_LTSW_L3_FIB_TBL_UC_VRF])) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_PASS;
    } else {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Filter with given L3 interface.
 *
 * \param [in] unit Unit number
 * \param [in] index The valid entry number.
 * \param [in] flt_crtr Filter criteria info.
 * \param [in] cfg L3 entry info.
 * \param [out] flt_cmd Filter result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_uc_trav_filter_intf(int unit, uint32_t index,
                       l3_filter_criteria_t *flt_crtr,
                       l3_cfg_t *cfg,
                       l3_filter_cmd_t *flt_cmd)
{
    uint32_t ol_nhidx, ul_nhidx, dest_type;
    bcmi_ltsw_l3_egr_obj_type_t ol_type, ul_type;
    int ol_intf, ul_intf = 0;
    bcmi_ltsw_ecmp_member_dest_info_t dest_info;
    bool ecmp_member_dest = false;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!cfg->uc) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fv = cfg->cfg.uc.flds;

    if ((fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX]||
         fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL]) ^ flt_crtr->nat) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP_NO_CNT;
        SHR_EXIT();
    }

    dest_type = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
    if (dest_type == BCMI_XFS_DEST_TYPE_NHOP) {
        ol_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
        ol_type = BCMI_LTSW_L3_EGR_OBJ_T_OL;
    } else if (dest_type == BCMI_XFS_DEST_TYPE_ECMP) {
        ol_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
        ol_type = BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL;
    } else if (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER) {
        ecmp_member_dest = true;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (fv[BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1]) {
        ul_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1];
        ul_type = (fv[BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX]
                   & (1 << 2)) ?
                  BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL :
                  BCMI_LTSW_L3_EGR_OBJ_T_UL;
    } else {
        ul_nhidx = 0;
        ul_type = BCMI_LTSW_L3_EGR_OBJ_T_CNT;
    }

    if (ecmp_member_dest) {
        sal_memset(&dest_info, 0, sizeof(bcmi_ltsw_ecmp_member_dest_info_t));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_ecmp_member_dest_get(unit,
                                            fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST],
                                            &dest_info));
        ol_intf = dest_info.ol_egr_obj;
        ul_intf = dest_info.ul_egr_obj;
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, ol_nhidx, ol_type,
                                                  &ol_intf));

        if (flt_crtr->ul_intf && ul_nhidx) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit, ul_nhidx, ul_type,
                                                      &ul_intf));
        }
    }

    if (((flt_crtr->ol_intf == ol_intf) && (flt_crtr->ul_intf == ul_intf) &&
         !flt_crtr->negate) ||
        (((flt_crtr->ol_intf != ol_intf) || (flt_crtr->ul_intf != ul_intf)) &&
         flt_crtr->negate)) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_PASS;
    } else {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Filter with ranger of valid entry index.
 *
 * \param [in] unit Unit number
 * \param [in] index The valid entry number.
 * \param [in] flt_crtr Filter criteria info.
 * \param [in] cfg L3 entry info.
 * \param [out] flt_cmd Filter result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_trav_filter_range(int unit, uint32_t index,
                     l3_filter_criteria_t *flt_crtr,
                     l3_cfg_t *cfg,
                     l3_filter_cmd_t *flt_cmd)
{
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (cfg->uc) {
        fv = cfg->cfg.uc.flds;
        if ((fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] ||
             fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL]) ^ flt_crtr->nat) {
            *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP_NO_CNT;
            SHR_EXIT();
        }
    }

    if (index < flt_crtr->start) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_SKIP;
    } else if (index > flt_crtr->end) {
        *flt_cmd = BCMINT_LTSW_L3_FLT_STOP;
    } else {
        *flt_cmd = BCMINT_LTSW_L3_FLT_PASS;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct L3 entry info from handle.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [out] l3_cfg L3 entry info l3_cfg_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_eh_to_cfg(int unit, bcmlt_entry_handle_t eh,
             const bcmint_ltsw_l3_fib_tbl_t *ti,
             l3_cfg_t *l3_cfg)
{
    SHR_FUNC_ENTER(unit);

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_UC) {
        l3_cfg->uc = true;
        l3_cfg->cfg.uc.fld_bmp = ti->fld_bmp;
    } else {
        l3_cfg->cfg.mc.fld_bmp = ti->fld_bmp;
    }

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, l3_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill L3 forwarding info from unicast route entry data fields.
 *
 * \param [in] unit Unit number
 * \param [in] ti LT info.
 * \param [in] cfg Unicast route entry data.
 * \param [out] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_uc_cfg_to_fib(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, l3_uc_cfg_t *cfg,
                 bcmi_ltsw_l3_fib_t *fib)
{
    bcmi_ltsw_l3_egr_obj_type_t ol_type, ul_type;
    uint32_t dest_type, ol_nhidx, ul_nhidx;
    bcmi_ltsw_ecmp_member_dest_info_t dest_info;
    bool ecmp_member_dest = false;
    uint64_t *fv = cfg->flds;
    bcmi_ltsw_l3_egr_obj_type_t type;
    int nh_ecmp_idx, prot_swt_prf_idx = BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP1;

    SHR_FUNC_ENTER(unit);

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
        fib->flags |= BCM_L3_IP6;
        bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr,
                                     &fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0]);
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
            bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr_mask,
                                &fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK]);
        }
    } else {
        fib->ip_addr = fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0];
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
            fib->ip_addr_mask = fv[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK];
        }
    }

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
        fib->vrf = BCM_L3_VRF_GLOBAL;
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GH) {
        fib->vrf = BCM_L3_VRF_OVERRIDE;
    } else {
        fib->vrf = fv[BCMINT_LTSW_L3_FIB_TBL_UC_VRF];
    }

    fib->alpm_data_mode = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE];
    if (fib->alpm_data_mode == bcmL3VrfRouteDataModeFull) {
        fib->flags3 |= BCM_L3_FLAGS3_DATA_MODE_FULL;
    } else {
        fib->flags3 |= BCM_L3_FLAGS3_DATA_MODE_REDUCED;
    }

    if (!(ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) ||
        (fib->alpm_data_mode == bcmL3VrfRouteDataModeFull)) {
        dest_type = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
        if (dest_type == BCMI_XFS_DEST_TYPE_NHOP) {
            ol_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
            ol_type = BCMI_LTSW_L3_EGR_OBJ_T_OL;
        } else if (dest_type == BCMI_XFS_DEST_TYPE_ECMP) {
            ol_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
            ol_type = BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL;
        } else if (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER) {
            ecmp_member_dest = true;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {
        /* ALPM reduced view. */
        ol_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
        if (ol_nhidx > L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit)) {
            ol_type = BCMI_LTSW_L3_EGR_OBJ_T_OL;
        } else {
            ol_type = BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL;
        }
    }

    ul_nhidx = fv[BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1];
    if (ul_nhidx) {
        ul_type = (fv[BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX]
                   & (1 << 2)) ?
                  BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL : BCMI_LTSW_L3_EGR_OBJ_T_UL;
        prot_swt_prf_idx =
            fv[BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX] & 0x3;
    } else {
        ul_type = BCMI_LTSW_L3_EGR_OBJ_T_CNT;
    }

    if (ecmp_member_dest) {
        sal_memset(&dest_info, 0, sizeof(bcmi_ltsw_ecmp_member_dest_info_t));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_ecmp_member_dest_get(unit,
                                        fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST],
                                        &dest_info));
        if (dest_info.type == BCMI_LTSW_ECMP_MEMBER_DEST_T_ALPM_NO_CASCADED) {
            fib->ol_intf = dest_info.ol_egr_obj;
            fib->ul_intf = dest_info.ul_egr_obj;
        } else if (dest_info.type == BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED) {
            fib->ol_intf = dest_info.ol_egr_obj;
        } else {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        prot_swt_prf_idx = dest_info.prot_swt_prfl_idx;
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, ol_nhidx, ol_type,
                                                  &fib->ol_intf));

        if (ul_nhidx) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit, ul_nhidx, ul_type,
                                                      &fib->ul_intf));
        }
    }

    /* Recover MULTIPATH flag. */
    if(fib->ul_intf) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_id_resolve(unit, fib->ul_intf,
                                                &nh_ecmp_idx, &type));
        if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
            fib->flags |= BCM_L3_MULTIPATH;
        }
    }
    if (fib->ol_intf) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_id_resolve(unit, fib->ol_intf,
                                                &nh_ecmp_idx, &type));
        if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
            fib->flags |= BCM_L3_MULTIPATH;
        }
    }

    fib->nat_id = fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX];
    fib->realm_id = fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] >> 2;
    fib->class_id = fv[BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID];
    fib->opaque_ctrl_id =
        L3_MISC_CTRL_0_OPQ_CTRL_ID(fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0]);

    if (fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] & DST_DISCARD) {
        fib->flags |= BCM_L3_DST_DISCARD;
    }

    if (fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] & DST_CP2CPU) {
        fib->flags |= BCM_L3_COPY_TO_CPU;
    }

    if (fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] & DST_LOCAL) {
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
            fib->flags |= BCM_L3_DEFIP_LOCAL;
        } else {
            fib->flags |= BCM_L3_HOST_LOCAL;
        }
    }

    if (prot_swt_prf_idx == BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP2) {
        fib->flags2 |= BCM_L3_FLAGS2_FAILOVER_UNDERLAY;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct unicast route entry data fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] ti LT info.
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Unicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_to_uc_cfg(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti,
                 bcmi_ltsw_l3_fib_t *fib, l3_uc_cfg_t *cfg)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    bcmi_ltsw_ecmp_member_dest_info_t dest_info = {0};
    bcmi_ltsw_l3_egr_obj_info_t egr_obj_info = {0};
    int dest, dest_type, nh_ecmp_idx, ecmp_and_prot;
    int max_nhop_intf, min_nhop_intf, max_ecmp_intf, min_ecmp_intf;
    uint64_t *fv = cfg->flds;
    int is_ecmp = 0;
    uint16_t prot_en, prot_swt_prf_idx;

    SHR_FUNC_ENTER(unit);

    /* Select protection switching profile index. */
    prot_en = bcmi_ltsw_property_get(unit,
                                     BCMI_FAILOVER_FIXED_NH_OFFSET_ENABLE,
                                     0);
    if (fib->flags2 & BCM_L3_FLAGS2_FAILOVER_UNDERLAY) {
        if (!prot_en) {
            SHR_ERR_MSG_EXIT(SHR_E_UNAVAIL,
                             (BSL_META_U(unit,
                                  "BCM_L3_FLAGS2_FAILOVER_UNDERLAY only "
                                  "applies with L3 failover enabled.\n")));
        }
        prot_swt_prf_idx = BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP2;
    } else {
        prot_swt_prf_idx = BCMI_XFS_PROT_SWT_PROFILE_IDX_NHOP1;
    }

    if (!(ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID)) &&
        (fib->class_id > 0)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "ALPM routing doesn't support explicit customer "
                              "specified class ID.\n")));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if ((ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE)) &&
        (fib->alpm_data_mode == bcmL3VrfRouteDataModeReduced)) {
        if (fib->flags & (BCM_L3_DST_DISCARD | BCM_L3_COPY_TO_CPU |
                          BCM_L3_HOST_LOCAL | BCM_L3_DEFIP_LOCAL)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (fib->opaque_ctrl_id > L3_MISC_CTRL_0_OPQ_CTRL_ID_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, fib->ol_intf, &nh_ecmp_idx,
                                            &type));
    if ((ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE)) &&
        (fib->alpm_data_mode == bcmL3VrfRouteDataModeReduced) &&
        !((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
          (nh_ecmp_idx > L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit))) &&
        !((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
          (nh_ecmp_idx <= L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit)))) {
        if (L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit)) {
            SHR_IF_ERR_CONT
                (bcmi_ltsw_l3_egress_id_range_get(unit,
                                        BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                        NULL, &max_nhop_intf));
            SHR_IF_ERR_CONT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit,
                                        L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit) + 1,
                                        BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                        &min_nhop_intf));
            SHR_IF_ERR_CONT
                (bcmi_ltsw_l3_egress_id_range_get(unit,
                                        BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                        &min_ecmp_intf, NULL));
            SHR_IF_ERR_CONT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit,
                                        L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit),
                                        BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                        &max_ecmp_intf));
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Reduced ALPM route entry supports: \n\t"
                                  "Overlay nexthop in range %d ~ %d\n\t"
                                  "Overlay ECMP in range %d ~ %d\n"),
                       min_nhop_intf, max_nhop_intf, min_ecmp_intf,
                       max_ecmp_intf));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Reduced ALPM route entry doesn't support "
                                  "ECMP egress objects.\n"
                                  "l3_reduced_route_max_ecmp_group_index "
                                  "is 0.\n")));
        }

        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_info_get(unit, fib->ol_intf,
                                              &egr_obj_info));
        if (egr_obj_info.flags & BCMI_LTSW_L3_EGR_OBJ_INFO_F_CASCADED) {
            if (fib->ul_intf) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Unexpected underlay egress object. "
                                      "The overlay egress object %d is in "
                                      "cascaded mode .\n"), fib->ol_intf));
                SHR_IF_ERR_EXIT(SHR_E_CONFIG);
            }
            sal_memset(&dest_info, 0, sizeof(bcmi_ltsw_ecmp_member_dest_info_t));
            dest_info.type = BCMI_LTSW_ECMP_MEMBER_DEST_T_L3_CASCADED;
            dest_info.ol_egr_obj = fib->ol_intf;
            dest_info.ul_egr_obj = egr_obj_info.ul_egr_obj;
            dest_info.dvp = egr_obj_info.dvp;
            dest_info.prot_swt_prfl_idx = prot_swt_prf_idx;
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_ecmp_member_dest_alloc(unit, &dest_info, &dest));
            dest_type = BCMI_XFS_DEST_TYPE_ECMP_MEMBER;
        } else {
            dest = nh_ecmp_idx;
            dest_type = BCMI_XFS_DEST_TYPE_NHOP;
        }
    } else if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) {
        dest = nh_ecmp_idx;
        dest_type = BCMI_XFS_DEST_TYPE_ECMP;
        is_ecmp = 1;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    nh_ecmp_idx = 0;
    ecmp_and_prot = 0;

    if (fib->ul_intf > 0) {

        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_id_resolve(unit, fib->ul_intf,
                                                &nh_ecmp_idx, &type));
        if ((type != BCMI_LTSW_L3_EGR_OBJ_T_UL) &&
            (type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
        if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
            is_ecmp = 1;
        }

        /* The UC entry provides nhop_2_or_ecmp_grp_1. */
        if (ti->fld_bmp & (1<<BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1)) {
            if (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
                ecmp_and_prot = (1 << 2);
            }
            ecmp_and_prot |= prot_swt_prf_idx;
        } else {
            /*
             * Implicit single pointer model.
             */
             if (dest_type == BCMI_XFS_DEST_TYPE_ECMP) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }

            nh_ecmp_idx = 0;
            ecmp_and_prot = 0;

            sal_memset(&dest_info, 0, sizeof(bcmi_ltsw_ecmp_member_dest_info_t));
            dest_info.type = BCMI_LTSW_ECMP_MEMBER_DEST_T_ALPM_NO_CASCADED;
            dest_info.ol_egr_obj = fib->ol_intf;
            dest_info.ul_egr_obj = fib->ul_intf;
            dest_info.prot_swt_prfl_idx = prot_swt_prf_idx;
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_ecmp_member_dest_alloc(unit, &dest_info, &dest));
            dest_type = BCMI_XFS_DEST_TYPE_ECMP_MEMBER;
        }
    }

    if (!(fib->flags & BCM_L3_MULTIPATH) && is_ecmp) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if ((fib->flags & BCM_L3_MULTIPATH) && !is_ecmp) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = dest;
    fv[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = dest_type;
    fv[BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1] = nh_ecmp_idx;
    fv[BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX] = ecmp_and_prot;
    fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] = fib->nat_id;

    /* The encoding for nat_ctrl:
     *   nat_ctrl[0:0]: NAT;
     *   nat_ctrl[1:1]: NAPT;
     *   nat_ctrl[3:2]: Realm_id
     */
    fv[BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL] = (fib->realm_id << 2) |
                                             (fib->nat_id ? 1 : 0);

    fv[BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID] = fib->class_id;

    fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] =
                                L3_MISC_CTRL_0(fib->opaque_ctrl_id);

    if (fib->flags & BCM_L3_DST_DISCARD) {
        fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] |= DST_DISCARD;
    }

    if (fib->flags & BCM_L3_COPY_TO_CPU) {
        fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] |= DST_CP2CPU;
    }

    if (fib->flags & (BCM_L3_HOST_LOCAL | BCM_L3_DEFIP_LOCAL)) {
        fv[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] |= DST_LOCAL;
    }

    fv[BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = fib->alpm_data_mode;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate L3 host unicast tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] op Logical table operation code.
 * \param [in] cfg Unicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_host_uc_op(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti,
                  bcmlt_opcode_t op, l3_cfg_t *cfg)
{
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    l3_cfg_t pre_cfg;
    int dest, dest_type;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (op != BCMLT_OPCODE_INSERT) {
        /* Get the previous route entry info. */
        sal_memcpy(&pre_cfg, cfg, sizeof(l3_cfg_t));
        pre_cfg.cfg.uc.fld_bmp = 0;

        SHR_IF_ERR_EXIT
            (lt_eh_fill(unit, eh, ti, &pre_cfg));

        rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }

        if (op == BCMLT_OPCODE_LOOKUP) {
            if (SHR_SUCCESS(rv)) {
                SHR_IF_ERR_EXIT
                    (lt_eh_parse(unit, eh, ti, cfg));
            }
            SHR_ERR_EXIT(rv);
        } else if ((rv == SHR_E_NOT_FOUND) && (op == BCMLT_OPCODE_UPDATE)) {
            /*
             * BCM_L3_REPLACE: replace existing entry if it exists,
             * othereise, just insert the entry.
             */
            op = BCMLT_OPCODE_INSERT;
        } else {
            /* Resolve the DIP route info for UPDATE/DELETE operation. */
            pre_cfg.cfg.uc.fld_bmp = ti->fld_bmp;
            SHR_IF_ERR_EXIT
                (lt_eh_parse(unit, eh, ti, &pre_cfg));
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(eh));

    SHR_IF_ERR_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, op, BCMLT_PRIORITY_NORMAL));

    dest_type = pre_cfg.cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
    dest = pre_cfg.cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
    /* Free the ECMP_MEMBER_DEST of previous DIP route entry. */
    if ((op == BCMLT_OPCODE_DELETE || op == BCMLT_OPCODE_UPDATE) &&
        (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_ecmp_member_dest_free(unit, dest));
    }

exit:
    if (SHR_FUNC_ERR()) {
        dest_type = cfg->cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
        dest = cfg->cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
        /* Free the dest index in case fail to insert new route. */
        if ((op == BCMLT_OPCODE_INSERT || op == BCMLT_OPCODE_UPDATE) &&
            (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER)) {
            (void)bcmi_ltsw_ecmp_member_dest_free(unit, dest);
        }
    }
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct source unicast alpm entry data fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Unicast alpm entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_to_src_uc_alpm_cfg(int unit, bcmi_ltsw_l3_fib_t *fib,
                          l3_uc_cfg_t *cfg)
{
    int mask_length = 0;

    SHR_FUNC_ENTER(unit);

    if (fib->alpm_data_mode == bcmL3VrfRouteDataModeFull) {

        if (fib->flags & BCM_L3_IP6) {
            mask_length = bcm_ip6_mask_length(fib->ip6_addr_mask);
        } else {
            mask_length = bcm_ip_mask_length(fib->ip_addr_mask);
        }

        cfg->flds[BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = (mask_length == 0) ?
                                                           SRC_DEFROUTE : 0;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate L3 unicast alpm tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] op Logical table operation code.
 * \param [in] lpm LPM lookup.
 * \param [in] cfg Unicast alpm entry data.
 * \param [out] new_added Indicate a newly added entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_uc_alpm_op(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti,
                  bcmlt_opcode_t op, bool lpm, l3_cfg_t *cfg, bool *new_added)
{
    int dunit, rv, ipt;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    l3_cfg_t pre_cfg;
    uint16_t vrf_id;
    int dest_type, dest;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (op != BCMLT_OPCODE_INSERT) {
        /* Get the previous route entry info. */
        sal_memcpy(&pre_cfg, cfg, sizeof(l3_cfg_t));
        pre_cfg.cfg.uc.fld_bmp = 0;

        SHR_IF_ERR_EXIT
            (lt_eh_fill(unit, eh, ti, &pre_cfg));

        if ((op == BCMLT_OPCODE_LOOKUP) && lpm) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_LPM_LOOKUP));
        }

        rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }

        if (op == BCMLT_OPCODE_LOOKUP) {
            if (SHR_SUCCESS(rv)) {
                SHR_IF_ERR_EXIT
                    (lt_eh_parse(unit, eh, ti, cfg));
            }
            SHR_ERR_EXIT(rv);
        } else if ((rv == SHR_E_NOT_FOUND) && (op == BCMLT_OPCODE_UPDATE)) {
            /*
             * BCM_L3_REPLACE: replace existing entry if it exists,
             * othereise, just insert the entry.
             */
            op = BCMLT_OPCODE_INSERT;
        } else {
            /* Resolve the DIP route info for UPDATE/DELETE operation. */
            pre_cfg.cfg.uc.fld_bmp = ti->fld_bmp;
            SHR_IF_ERR_EXIT
                (lt_eh_parse(unit, eh, ti, &pre_cfg));
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(eh));
    }

    /*
     * In ALPM combined mode, the user must insert a real default VRF route as
     * the first route within any VRF in order to prevent an unexpected lookup
     * miss.
     */
    SHR_IF_ERR_EXIT
        (l3_alpm_def_check(unit, ti->attr, cfg->cfg.uc.flds, op));

    SHR_IF_ERR_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, op, BCMLT_PRIORITY_NORMAL));

    /* Update the ALPM route counter. */
    ipt = (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) ? IP_TYPE_V6 : IP_TYPE_V4;
    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF) {
        vrf_id = (uint16_t)cfg->cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF];
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
        vrf_id = L3_FIB_VRF_GLO(unit);
    } else {
        vrf_id = L3_FIB_VRF_GHI(unit);
    }

    if (op == BCMLT_OPCODE_INSERT) {
        VRF_ALPM_ROUTE_INC(unit, vrf_id, ipt);
        if (new_added) {
            *new_added = true;
        }
    } else if (op == BCMLT_OPCODE_DELETE) {
        VRF_ALPM_ROUTE_DEC(unit, vrf_id, ipt);
    }

    dest_type = pre_cfg.cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
    dest = pre_cfg.cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
    /* Free the ECMP_MEMBER_DEST of previous DIP route entry. */
    if ((op == BCMLT_OPCODE_DELETE || op == BCMLT_OPCODE_UPDATE) &&
        (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_ecmp_member_dest_free(unit, dest));
    }

exit:
    if (SHR_FUNC_ERR()) {
        dest_type = cfg->cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
        dest = cfg->cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
        /* Free the dest index in case fail to insert new route. */
        if ((op == BCMLT_OPCODE_INSERT || op == BCMLT_OPCODE_UPDATE) &&
            (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER)) {
            (void)bcmi_ltsw_ecmp_member_dest_free(unit, dest);
        }
    }
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate L3 source unicast tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] op Logical table operation code.
 * \param [in] cfg Unicast alpm entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_src_uc_alpm_op(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, bcmlt_opcode_t op,
                      l3_cfg_t *cfg)
{
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (op == BCMLT_OPCODE_LOOKUP) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    SHR_IF_ERR_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    rv = bcmlt_entry_commit(eh, op, BCMLT_PRIORITY_NORMAL);

    if ((rv == SHR_E_NOT_FOUND) && (op == BCMLT_OPCODE_UPDATE)) {
        /* BCM_L3_REPLACE: replace existing entry if it exists,
         * othereise, just insert the entry. */
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL));
    } else if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback to clear the hitbit of MC entry.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [out] l3_cfg L3 entry info l3_cfg_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mc_eh_to_cfg_hitclr(int unit, bcmlt_entry_handle_t eh,
                       const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *l3_cfg)
{
    l3_mc_cfg_t *mc = &l3_cfg->cfg.mc;

    SHR_FUNC_ENTER(unit);

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);
    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, l3_cfg));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_ipmc_flexctr_hit_get(unit,
                            mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID],
                            mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION],
                            true, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback to recover software database from entry during warmboot.
 *
 * The callback is used to fetch the mtu_profile_ptr from entry and
 * increase the reference of this mtu ptr.
 *
 * The flexctr reference info is kept in HA, thus no need to recover it.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [out] l3_cfg L3 entry info l3_cfg_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mc_eh_to_cfg_recover(int unit, bcmlt_entry_handle_t eh,
                        const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *l3_cfg)
{
    l3_mc_cfg_t *mc = &l3_cfg->cfg.mc;

    SHR_FUNC_ENTER(unit);

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX);
    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, l3_cfg));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_mtu_recover(unit,
                          mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX]));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill L3 forwarding info from multicast route entry data fields.
 *
 * \param [in] unit Unit number
 * \param [in] ti LT info.
 * \param [in] cfg Multicast route entry data.
 * \param [out] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mc_cfg_to_fib(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, l3_mc_cfg_t *cfg,
                 bcmi_ltsw_l3_fib_t *fib)
{
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    uint64_t *fv = cfg->flds;
    int pri_offset = 0;

    SHR_FUNC_ENTER(unit);

    fib->flags |= BCM_L3_IPMC;

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
        fib->flags |= BCM_L3_IP6;
        bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr,
                                     &fv[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR]);
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
            bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr_mask,
                                 &fv[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK]);
            if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_LPM_128) {
                pri_offset = L3_ENT_PRI_OFFSET_IPV6_128;
            }else {
                pri_offset = L3_ENT_PRI_OFFSET_IPV6_64;
            }
            fib->network = 1;
        }
        if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR)) {
            bcmi_ltsw_util_uint64_to_ip6(&fib->sip6_addr,
                                 &fv[BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR]);
        }
    } else {
        fib->ip_addr = fv[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR];
        if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR)) {
            fib->sip_addr = fv[BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR];
        }
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
            fib->ip_addr_mask = fv[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK];
            pri_offset = L3_ENT_PRI_OFFSET_IPV4;
            fib->network = 1;
        }
    }

    fib->vrf_mask = fv[BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK];
    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
        if (fv[BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] < pri_offset) {
            fib->vrf = BCM_L3_VRF_GLOBAL;
        } else if (fv[BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] <
                   (2 * pri_offset)) {
            fib->vrf = fv[BCMINT_LTSW_L3_FIB_TBL_MC_VRF];
        } else {
            fib->vrf = BCM_L3_VRF_OVERRIDE;
        }
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST) {
        fib->vrf = fv[BCMINT_LTSW_L3_FIB_TBL_MC_VRF];
    }

    if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF)) {
        fib->l3_iif = fv[BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF];
    } else {
        fib->l3_iif = BCM_IF_INVALID;
    }

    if (fv[BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] ==
        BCMI_XFS_DEST_TYPE_L3MC_GROUP) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_multicast_l3_grp_id_get(unit,
                                               fv[BCMINT_LTSW_L3_FIB_TBL_MC_DEST],
                                               &fib->mc_group));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(&mtu_cfg, 0, sizeof(bcmi_ltsw_l3_mtu_cfg_t));
    mtu_cfg.index = fv[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX];
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_mtu_get(unit, &mtu_cfg));
    fib->mtu = mtu_cfg.mtu;

    fib->rpa_id = fv[BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID];
    fib->expected_l3_iif = fv[BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF];

    fib->class_id = fv[BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID];
    fib->flex_ctr_action_index = fv[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION];
    fib->opaque_ctrl_id =
        L3_MISC_CTRL_0_OPQ_CTRL_ID(fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0]);

    if (fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] & DST_DISCARD) {
        fib->flags |= BCM_L3_DST_DISCARD;
    }

    if (fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] & DST_CP2CPU) {
        fib->flags |= BCM_L3_COPY_TO_CPU;
    }

    if (fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] & DST_IPMC_RPF_FAIL2CPU) {
        fib->int_flags |= BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct IPv4 host multicast entry key fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Multicast route entry key.
 * \param [out] tbl_id LT id.
 *
 * \retval None.
 */
static void
l3_fib_to_mc_cfg_key_host_v4(int unit, bcmi_ltsw_l3_fib_t *fib,
                             l3_mc_cfg_t *cfg,
                             bcmint_ltsw_l3_fib_tbl_id_t *tbl_id)
{
    if (fib->sip_addr) {
        cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR] = fib->sip_addr;
        *tbl_id = BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG;
    } else {
        *tbl_id = BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G;
    }

    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = fib->ip_addr;
    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = fib->vrf;
    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] =
                        (fib->l3_iif == BCM_IF_INVALID) ? 0 : fib->l3_iif;
}

/*!
 * \brief Construct IPv6 host multicast entry key fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Multicast route entry key.
 * \param [out] tbl_id LT id.
 *
 * \retval None.
 */
static void
l3_fib_to_mc_cfg_key_host_v6(int unit, bcmi_ltsw_l3_fib_t *fib,
                             l3_mc_cfg_t *cfg,
                             bcmint_ltsw_l3_fib_tbl_id_t *tbl_id)
{
    bcm_ip6_t ip6_zero;
    uint64_t *fv = cfg->flds;

    sal_memset(ip6_zero, 0, BCM_IP6_ADDRLEN);

    if (sal_memcmp(fib->sip6_addr, ip6_zero, BCM_IP6_ADDRLEN)) {
        bcmi_ltsw_util_ip6_to_uint64(&fv[BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR],
                                     &fib->sip6_addr);
        *tbl_id = BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG;
    } else {
        *tbl_id = BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G;
    }

    bcmi_ltsw_util_ip6_to_uint64(&fv[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR],
                                 &fib->ip6_addr);
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = fib->vrf;
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = (fib->l3_iif == BCM_IF_INVALID) ?
                                           0 : fib->l3_iif;
}

/*!
 * \brief Construct IPv4 multicast entry key fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Multicast route entry key.
 *
 * \retval None.
 */
static void
l3_fib_to_mc_cfg_key_route_v4(int unit, bcmi_ltsw_l3_fib_t *fib,
                              l3_mc_cfg_t *cfg)
{
    int masklen, pri;

    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = fib->ip_addr;
    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = fib->ip_addr_mask;

    masklen = bcm_ip_mask_length(fib->ip_addr);

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        pri = masklen;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        pri = masklen + 2 * L3_ENT_PRI_OFFSET_IPV4;
    } else {
        pri = masklen + L3_ENT_PRI_OFFSET_IPV4;
        cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = fib->vrf;
        cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = L3_FIB_VRF_MAX(unit);
    }

    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = pri;
}

/*!
 * \brief Construct IPv6 multicast entry key fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Multicast route entry key.
 * \param [out] tbl_id Table ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNVAIL IPv6 MC DEFIP table is not available.
 */
static int
l3_fib_to_mc_cfg_key_route_v6(int unit, bcmi_ltsw_l3_fib_t *fib,
                              l3_mc_cfg_t *cfg,
                              bcmint_ltsw_l3_fib_tbl_id_t *tbl_id)
{
    int masklen, pri, offset;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit) &&
        !L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    bcmi_ltsw_util_ip6_to_uint64
        (&cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK],
         &fib->ip6_addr_mask);

    masklen = bcm_ip6_mask_length(fib->ip6_addr_mask);
    if (masklen > 64) {
        if (!L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
        }
        *tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
        offset = L3_ENT_PRI_OFFSET_IPV6_128;
    } else {
        if (L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit)) {
            *tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64;
            offset = L3_ENT_PRI_OFFSET_IPV6_64;
        } else {
            *tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
            offset = L3_ENT_PRI_OFFSET_IPV6_128;
        }
    }

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        pri = masklen;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        pri = masklen + 2 * offset;
    } else {
        pri = masklen + offset;
        cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = fib->vrf;
        cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = L3_FIB_VRF_MAX(unit);
    }

    cfg->flds[BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = pri;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct multicast route entry data fields from fib info.
 *
 * \param [in] unit Unit number.
 * \param [in] ti LT info.
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Multicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_to_mc_cfg(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti,
                 bcmi_ltsw_l3_fib_t *fib, l3_mc_cfg_t *cfg)
{
    uint64_t *fv = cfg->flds;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_DEF;
    int sbr_index;

    SHR_FUNC_ENTER(unit);

    if (fib->opaque_ctrl_id > L3_MISC_CTRL_0_OPQ_CTRL_ID_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fv[BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = _BCM_MULTICAST_ID_GET(fib->mc_group);
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = BCMI_XFS_DEST_TYPE_L3MC_GROUP;
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = fib->rpa_id;
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = fib->expected_l3_iif;
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = fib->class_id;
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = fib->flex_ctr_action_index;
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] =
                                            L3_MISC_CTRL_0(fib->opaque_ctrl_id);

    if (fib->flags & BCM_L3_DST_DISCARD) {
        fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] |= DST_DISCARD;
    }

    if (fib->flags & BCM_L3_COPY_TO_CPU) {
        fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] |= DST_CP2CPU;
    }

    if (fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU) {
        fv[BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] |= DST_IPMC_RPF_FAIL2CPU;
    }

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
            if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_LPM_128) {
                pth = BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_128;
            } else {
                pth = BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_64;
            }
        } else {
            pth = BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_DEFIP;
        }
    } else {
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
            if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR)) {
                pth = BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_2;
            } else {
                pth = BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_1;
            }
        } else {
            if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR)) {
                pth = BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_2;
            } else {
                pth = BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_1;
            }
        }
    }

    /* Get strength profile index. */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = sbr_index;

    /* Get MTU profile ptr. */
    sal_memset(&mtu_cfg, 0, sizeof(bcmi_ltsw_l3_mtu_cfg_t));
    mtu_cfg.mtu = fib->mtu ? fib->mtu : BCMI_LTSW_L3_MTU_DEFAULT_SIZE;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_mtu_set(unit, &mtu_cfg));
    fv[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = mtu_cfg.index;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate L3 multicast tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] op Logical table operation code.
 * \param [in] cfg Multicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_mc_op(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, bcmlt_opcode_t op,
             l3_cfg_t *cfg)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t fld_bmp = cfg->cfg.mc.fld_bmp;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (op == BCMLT_OPCODE_LOOKUP)  {
        /* Not fill in data fields for LOOKUP operation. */
        cfg->cfg.mc.fld_bmp = 0;
    }

    SHR_IF_ERR_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    if (op == BCMLT_OPCODE_LOOKUP) {
        /* Set bitmap of data fields to be parsed. */
        cfg->cfg.mc.fld_bmp = fld_bmp;
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
 * \brief Construct fib info from l3_cfg.
 *
 * \param [in] unit Unit number
 * \param [in] ti LT info.
 * \param [in] cfg L3 entry info.
 * \param [out] fib L3 forwarding info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_cfg_to_fib(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *cfg,
              bcmi_ltsw_l3_fib_t *fib)
{
    SHR_FUNC_ENTER(unit);

    if (cfg->uc) {
        SHR_IF_ERR_EXIT
            (l3_uc_cfg_to_fib(unit, ti, &cfg->cfg.uc, fib));
    } else {
        SHR_IF_ERR_EXIT
            (l3_mc_cfg_to_fib(unit, ti, &cfg->cfg.mc, fib));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill L3 host info from forwarding info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] info L3 host info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_host_entry_fill(int unit, bcmi_ltsw_l3_fib_t *fib, bcm_l3_host_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!fib || !info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    bcm_l3_host_t_init(info);

    info->l3a_vrf = fib->vrf;
    if (fib->flags & BCM_L3_IP6) {
        sal_memcpy(info->l3a_ip6_addr, fib->ip6_addr, sizeof(bcm_ip6_t));
    } else {
        sal_memcpy(&info->l3a_ip_addr, &fib->ip_addr, sizeof(bcm_ip_t));
    }
    info->l3a_flags = fib->flags;
    info->l3a_intf = fib->ol_intf;
    info->l3a_ul_intf = fib->ul_intf;
    info->l3a_lookup_class = fib->class_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill L3 route info from forwarding info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] info L3 route info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_route_entry_fill(int unit, bcmi_ltsw_l3_fib_t *fib, bcm_l3_route_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!fib || !info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    bcm_l3_route_t_init(info);

    info->l3a_vrf = fib->vrf;
    if (fib->flags & BCM_L3_IP6) {
        sal_memcpy(info->l3a_ip6_net, fib->ip6_addr, sizeof(bcm_ip6_t));
        sal_memcpy(info->l3a_ip6_mask, fib->ip6_addr_mask, sizeof(bcm_ip6_t));
    } else {
        sal_memcpy(&info->l3a_subnet, &fib->ip_addr, sizeof(bcm_ip_t));
        sal_memcpy(&info->l3a_ip_mask, &fib->ip_addr_mask, sizeof(bcm_ip_t));
    }
    info->l3a_flags = fib->flags;
    info->l3a_flags2 = fib->flags2;
    info->l3a_flags3 = fib->flags3;
    info->l3a_intf = fib->ol_intf;
    info->l3a_ul_intf = fib->ul_intf;
    info->l3a_lookup_class = fib->class_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Descrease the reference count for the UC entry.
 *
 * Decrease the reference of ECMP_MEMBER0 and ALPM route count.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] cfg Multicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_uc_refcnt_decr(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *cfg)
{
    l3_uc_cfg_t *uc = &cfg->cfg.uc;
    uint16_t vrf_id = 0;
    int dest_type, dest, ipt;

    SHR_FUNC_ENTER(unit);

    if (ti->attr & (BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                    BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC)) {
        SHR_EXIT();
    }

    /* Free an entry to ECMP_MEMBER0. */
    dest_type = uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE];
    dest = uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_DEST];
    if (dest_type == BCMI_XFS_DEST_TYPE_ECMP_MEMBER) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_ecmp_member_dest_free(unit, dest));
    }

    /* Decrease ALPM route cnt of a VRF. */
    if (ti->rm_type == BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM) {
        ipt = (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) ?
              IP_TYPE_V6 : IP_TYPE_V4;
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF) {
            vrf_id = uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF];
        } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
            vrf_id = L3_FIB_VRF_GLO(unit);
        } else {
            vrf_id = L3_FIB_VRF_GHI(unit);
        }
        VRF_ALPM_ROUTE_DEC(unit, vrf_id, ipt);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Decrease the reference count for the MC entry.
 *
 * Decrease the reference of MTU ptr and Flexctr action.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] cfg Multicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mc_refcnt_decr(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *cfg)
{
    l3_mc_cfg_t *mc = &cfg->cfg.mc;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    uint32_t ctr_id;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (!(ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_MC)) {
        SHR_EXIT();
    }

    /* MTU profile index de-reference. */
    mtu_cfg.index = mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX];
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg));

    /* Flexctr action de-reference. */
    ci.action_index = mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION];
    if (ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        ci.stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;

        rv = bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id);
        if (SHR_SUCCESS(rv)) {
            /* Clear the hit bit statisitc. */
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_ipmc_flexctr_hit_get(unit,
                    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID],
                    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION],
                    true, NULL));

            SHR_IF_ERR_EXIT
                (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                                   ctr_id));
        } else {
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an L3 fib entry.
 *
 * \param [in] unit Unit number.
 * \param [in] eh_dst Entry handle for DIP entry.
 * \param [in] ti_dst Logical table info DIP entry.
 * \param [in] eh_src Entry handle for SIP entry.
 * \param [in] ti_src Logical table info SIP entry.
 * \param [in] cfg Multicast route entry data.
 * \param [in] fill_eh_dst To fill the eh_dst or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_del(int unit,
           bcmlt_entry_handle_t eh_dst,
           const bcmint_ltsw_l3_fib_tbl_t *ti_dst,
           bcmlt_entry_handle_t eh_src,
           const bcmint_ltsw_l3_fib_tbl_t *ti_src,
           l3_cfg_t *cfg,
           bool fill_eh_dst)
{
    uint64_t fbmp, *fbmp_p = NULL;

    SHR_FUNC_ENTER(unit);

    fbmp_p = cfg->uc ? &cfg->cfg.uc.fld_bmp : &cfg->cfg.mc.fld_bmp;

    /* Not fill in data fields for delete operation. */
    fbmp = *fbmp_p;
    *fbmp_p = 0;

    /* Delete source route entry */
    if ((eh_src != BCMLT_INVALID_HDL) && (ti_src != NULL)) {
        SHR_IF_ERR_EXIT
            (lt_eh_fill(unit, eh_src, ti_src, cfg));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh_src, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL));
    }

    /* Delete dest route entry. */
    if (fill_eh_dst) {
        SHR_IF_ERR_EXIT
            (lt_eh_fill(unit, eh_dst, ti_dst, cfg));
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh_dst, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL));

    /* Descrease the reference. */
    if ((ti_dst->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_UC) &&
        (ti_dst->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET)) {
        SHR_IF_ERR_EXIT
            (l3_uc_refcnt_decr(unit, ti_dst, cfg));
    } else if (ti_dst->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_MC) {
        SHR_IF_ERR_EXIT
            (l3_mc_refcnt_decr(unit, ti_dst, cfg));
    }

exit:
    /* Recover the fld_bmp. */
    if (fbmp_p) {
        *fbmp_p = fbmp;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 table traverse.
 *
 * \param [in] unit Unit number
 * \param [in] tbl_id Logical table ID.
 * \param [in] trav_info Traverse control info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_traverse(int unit, bcmint_ltsw_l3_fib_tbl_id_t tbl_id, l3_trav_t *trav_info)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    bcm_l3_host_t host;
    bcm_l3_route_t route;
    bcm_ipmc_addr_t ipmc;
    bcm_l3_nat_ingress_t nat;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t eh_del_dst = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t eh_del_src = BCMLT_INVALID_HDL;
    bcmint_ltsw_l3_fib_tbl_id_t src_tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *src_tbl_info = NULL;
    bcmi_ltsw_l3_fib_t fib;
    l3_cfg_t cfg, next_cfg;
    l3_filter_cmd_t flt_cmd;
    uint32_t idx = 0, inuse_cnt = 0;
    int dunit;
    int rv = SHR_E_NONE, cb_rv;
    bool del, hit, hit_en, in_order;

    SHR_FUNC_ENTER(unit);

    if (!trav_info || !trav_info->hdl_to_cfg_cb) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, tbl_info->name, &inuse_cnt));
    if (!inuse_cnt) {
        SHR_EXIT();
    }

    /*
     * In the traverse without snapshot scenario, the sequence of
     * "GET_FIRST - DELETE - GET_NEXT - DELETE" doesn't work for LTs managed
     * by RM-HASH/RM_TCAM, as it is not able to return the NEXT entry from
     * a deleted entry without snapshot. For RM-HASH based LTs, the sequence
     * should be GET_NEXT before deleting the entry.
     * However RM-ALPM did some improvment to support the sequence of
     * "GET_FIRST - DELETE - GET_NEXT - DELETE" for a better performance.
     */
    if (tbl_info->rm_type == BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM) {
        /* GET_FIRST - DELETE - GET_NEXT - DELETE in order. */
        in_order = true;
    } else {
        /* Get next entry before deleting the entry. */
        in_order = false;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    if ((trav_info->op == L3_TRAV_OP_DEL) ||
        (trav_info->op == L3_TRAV_OP_AGE)) {
        /* Allocate entry handle for deleting source IP entry. */
        if ((tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_UC) &&
            (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) &&
            ltsw_feature(unit, LTSW_FT_URPF) && L3_FIB_URPF_ENABLED(unit)) {
            if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
                if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
                    src_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
                } else if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GH) {
                    src_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
                } else {
                    src_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
                }
            } else {
                if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
                    src_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
                } else if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GH) {
                    src_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
                } else {
                    src_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
                }
            }

            SHR_IF_ERR_EXIT
                (bcmint_ltsw_l3_fib_tbl_get(unit, src_tbl_id, &src_tbl_info));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate_by_id(dunit, src_tbl_info->tbl_id,
                                            src_tbl_info->fld_cnt,
                                            &eh_del_src));
        }

        /* Allocate entry handle for deleting Dest IP entry. */
        if (!in_order) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id,
                                            tbl_info->fld_cnt, &eh_del_dst));
        }
    }

    if (!in_order) {
        /* Get the first entry. */
        rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL);
        if (SHR_SUCCESS(rv)) {
            sal_memset(&next_cfg, 0, sizeof(l3_cfg_t));
            SHR_IF_ERR_EXIT
                (trav_info->hdl_to_cfg_cb(unit, eh, tbl_info, &next_cfg));
        }
    }

    while (SHR_SUCCESS(rv)) {
        hit_en = false;
        hit = false;
        del = (trav_info->op == L3_TRAV_OP_DEL) ? true : false;

        if (!in_order) {
             /* Copy next_cfg info fetched in the previous cycle into the cfg */
            sal_memcpy(&cfg, &next_cfg, sizeof(l3_cfg_t));
        }

        rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE, BCMLT_PRIORITY_NORMAL);
        if (SHR_SUCCESS(rv)) {
            if (in_order) {
                /* Save the info in cfg, operated in current cycle. */
                sal_memset(&cfg, 0, sizeof(l3_cfg_t));
                SHR_IF_ERR_EXIT
                    (trav_info->hdl_to_cfg_cb(unit, eh, tbl_info, &cfg));
            } else {
                /* Save the info in next_cfg, to be operated in next cycle. */
                sal_memset(&next_cfg, 0, sizeof(l3_cfg_t));
                SHR_IF_ERR_EXIT
                    (trav_info->hdl_to_cfg_cb(unit, eh, tbl_info, &next_cfg));
            }
        } else if (in_order) {
            break;
        }

        if (trav_info->filter) {
            SHR_IF_ERR_EXIT
                (trav_info->filter(unit, idx, &trav_info->flt_crtr,
                                   &cfg, &flt_cmd));

            if (flt_cmd == BCMINT_LTSW_L3_FLT_STOP) {
                break;
            } else if (flt_cmd == BCMINT_LTSW_L3_FLT_SKIP) {
                idx++;
                continue;
            } else if (flt_cmd == BCMINT_LTSW_L3_FLT_SKIP_NO_CNT) {
                continue;
            }
        }

        /* Resolve entry handle into forwarding info. */
        sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
        if (trav_info->cfg_to_fib_cb) {
            SHR_IF_ERR_EXIT
                (trav_info->cfg_to_fib_cb(unit, tbl_info, &cfg, &fib));
        }

        if (trav_info->op == L3_TRAV_OP_AGE) {
            /* Clear hit bit on used entry. */
            fib.flags |= BCM_L3_HIT_CLEAR;
        }

        /* Resolve fib into IPMC info. */
        if (trav_info->ipmc_addr_t_cb) {
            bcm_ipmc_addr_t_init(&ipmc);
            SHR_IF_ERR_EXIT
                (trav_info->ipmc_addr_t_cb(unit, &fib, &ipmc));
            hit_en = (ipmc.flags & BCM_IPMC_HIT_ENABLE) ? true : false;
            hit = (ipmc.flags & BCM_IPMC_HIT) ? true : false;
        }

        /* Don't age out the entry that without hitbit enabled. */
        if ((trav_info->op == L3_TRAV_OP_AGE) && !hit_en) {
            idx++;
            continue;
        }

        /* Resolve fib into NAT info. */
        if (trav_info->fib_to_nat_cb) {
            SHR_IF_ERR_EXIT
                (trav_info->fib_to_nat_cb(unit, &fib, &nat));
        }

        if ((trav_info->op == L3_TRAV_OP_AGE && !hit) ||
            (trav_info->op == L3_TRAV_OP_DEL)) {
            del = true;
        }

        if (del) {
            SHR_IF_ERR_EXIT
                (l3_fib_del(unit, (in_order ? eh : eh_del_dst), tbl_info,
                            eh_del_src, src_tbl_info, &cfg,
                            (in_order ? false : true)));
        }

        if (trav_info->host_trav_cb) {
            SHR_IF_ERR_EXIT
                (l3_host_entry_fill(unit, &fib, &host));
            cb_rv = trav_info->host_trav_cb(unit, idx, &host,
                                            trav_info->user_data);
            if (SHR_FAILURE(cb_rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_ERR_EXIT(cb_rv);
            }
        }

        if (trav_info->route_trav_cb) {
            SHR_IF_ERR_EXIT
                (l3_route_entry_fill(unit, &fib, &route));
            cb_rv = trav_info->route_trav_cb(unit, idx, &route,
                                             trav_info->user_data);
            if (SHR_FAILURE(cb_rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_ERR_EXIT(cb_rv);
            }
        }

        if (trav_info->ipmc_trav_cb && trav_info->ipmc_addr_t_cb &&
            !(trav_info->op == L3_TRAV_OP_AGE && hit)) {
            cb_rv = trav_info->ipmc_trav_cb(unit, &ipmc, trav_info->user_data);
            if (SHR_FAILURE(cb_rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_ERR_EXIT(cb_rv);
            }
        }

        if (trav_info->nat_trav_cb && trav_info->fib_to_nat_cb) {
            cb_rv = trav_info->nat_trav_cb(unit, idx, &nat,
                                           trav_info->user_data);
            if (SHR_FAILURE(cb_rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_ERR_EXIT(cb_rv);
            }
        }

        idx++;
    }

    if ((rv != SHR_E_NONE) && (rv != SHR_E_NOT_FOUND)) {
        SHR_ERR_EXIT(rv);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (eh_del_dst != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh_del_dst);
    }
    if (eh_del_src != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh_del_src);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover MTU profile reference.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_recover(int unit)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.hdl_to_cfg_cb = l3_mc_eh_to_cfg_recover;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG,
                     &trav_info));

    if (L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit)) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64,
                         &trav_info));
    }

    if (L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                         &trav_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3_ALPM_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 * \param [in] uft_mode UFT mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_control_set(int unit, int alpm_temp, bcmi_ltsw_uft_mode_t uft_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_alpm_control_set(unit, alpm_temp, uft_mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 Route table control info.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 * \param [in] uft_mode UFT mode.
 *
 * Set the TABLE_CONTROL.MAX_ENTRIES with TABLE_INFO.ENTRY_LIMIT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_table_control_set(int unit, int alpm_temp,
                          bcmi_ltsw_uft_mode_t uft_mode)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int i, num, thres, dunit, alpm_level;
    uint64_t max;
    static const struct {
        const char *str;
        const char *name;
        bool thres_en;
    } alpm_lt[] = {
        { BCMI_CPN_IP4_UC_ROUTE_TABLE_ENTRIES_MAX, L3_IPV4_UC_ROUTEs, true },
        { BCMI_CPN_IP4_UC_ROUTE_VRF_TABLE_ENTRIES_MAX, L3_IPV4_UC_ROUTE_VRFs, true },
        { BCMI_CPN_IP4_UC_ROUTE_OVERRIDE_TABLE_ENTRIES_MAX, L3_IPV4_UC_ROUTE_OVERRIDEs, false },
        { BCMI_CPN_IP6_UC_ROUTE_TABLE_ENTRIES_MAX, L3_IPV6_UC_ROUTEs, true },
        { BCMI_CPN_IP6_UC_ROUTE_VRF_TABLE_ENTRIES_MAX, L3_IPV6_UC_ROUTE_VRFs, true },
        { BCMI_CPN_IP6_UC_ROUTE_OVERRIDE_TABLE_ENTRIES_MAX, L3_IPV6_UC_ROUTE_OVERRIDEs, false },
        { NULL, L3_SRC_IPV4_UC_ROUTEs, true },
        { NULL, L3_SRC_IPV4_UC_ROUTE_VRFs, true },
        { NULL, L3_SRC_IPV4_UC_ROUTE_OVERRIDEs, false },
        { NULL, L3_SRC_IPV6_UC_ROUTEs, true },
        { NULL, L3_SRC_IPV6_UC_ROUTE_VRFs, true },
        { NULL, L3_SRC_IPV6_UC_ROUTE_OVERRIDEs, false },
        { NULL, L3_IPV4_COMP_DSTs, false },
        { NULL, L3_IPV4_COMP_SRCs, false },
        { NULL, L3_IPV6_COMP_DSTs, false },
        { NULL, L3_IPV6_COMP_SRCs, false }
    };

    SHR_FUNC_ENTER(unit);

    if ((uft_mode == bcmiUftMode3) ||
        (uft_mode == bcmiUftMode7) ||
        (uft_mode == bcmiUftMode9) ||
        (uft_mode == bcmiUftMode10) ||
        (uft_mode == bcmiUftMode12)) {
        alpm_level = 2;
    } else {
        alpm_level = 3;
    }

    thres = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_BNK_THRESHOLD, 95);
    if (thres <= 0 || thres > 100) {
        thres = 95;
    }

    num = COUNTOF(alpm_lt);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, TABLE_INFOs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, alpm_lt[i].name));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, ENTRY_LIMITs, &max));
        max = bcmi_ltsw_property_get(unit, alpm_lt[i].str, max);

        if (alpm_level == 3 && alpm_lt[i].thres_en) {
            max = max * thres / 100;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, TABLE_CONTROLs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, alpm_lt[i].name));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, MAX_ENTRIESs, max));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ALPM strength information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_strength_set(int unit)
{
    int dunit = 0;
    int sbr_index = 0;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_DEF;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, L3_ALPM_CONTROLs, &eh));

    
    pth = BCMI_LTSW_SBR_PTH_L3_IPV4_ALPM;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV4_UC_STRENGTH_PROFILE_INDEXs, sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV4_UC_VRF_STRENGTH_PROFILE_INDEXs,
                               sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXs,
                               sbr_index));

    
    pth = BCMI_LTSW_SBR_PTH_L3_IPV6_ALPM;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV6_UC_STRENGTH_PROFILE_INDEXs,sbr_index));


    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV6_UC_VRF_STRENGTH_PROFILE_INDEXs,
                               sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXs,
                               sbr_index));

    pth = BCMI_LTSW_SBR_PTH_L3_IPV4_ADDRESS_COMPRESSION;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXs,
                               sbr_index));

    pth = BCMI_LTSW_SBR_PTH_L3_IPV6_ADDRESS_COMPRESSION;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_index));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXs,
                               sbr_index));

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
 * \brief Callback to recover ALPM software database from entry during warmboot.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] ti LT info.
 * \param [out] l3_cfg L3 entry info l3_cfg_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_eh_to_cfg_recover(int unit, bcmlt_entry_handle_t eh,
                          const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *l3_cfg)
{
    uint64_t *fv = l3_cfg->cfg.uc.flds;
    int ipt = (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) ? IP_TYPE_V6 :
                                                            IP_TYPE_V4;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, l3_cfg));

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF) {
        VRF_ALPM_ROUTE_INC(unit, fv[BCMINT_LTSW_L3_FIB_TBL_UC_VRF], ipt);
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
        VRF_ALPM_ROUTE_INC(unit, L3_FIB_VRF_GLO(unit), ipt);
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GH) {
        VRF_ALPM_ROUTE_INC(unit, L3_FIB_VRF_GHI(unit), ipt);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover ALPM info from route table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_recover(int unit)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.hdl_to_cfg_cb = l3_alpm_eh_to_cfg_recover;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init ALPM control info.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_info_init(int unit, int alpm_temp)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];
    int i, sz;
    bcmi_lt_entry_t lt_ent;
    bcmi_lt_field_t flds[] = {
        {
            OPERATIONAL_STATEs,
            BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        }
    };

    SHR_FUNC_ENTER(unit);

    /* Fetch the operational state */
    lt_ent.fields = flds;
    lt_ent.nfields = sizeof(flds) / sizeof(flds[0]);
    lt_ent.attr = 0;
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_get(unit, L3_ALPM_CONTROLs, &lt_ent, NULL, NULL));

    if (sal_strcasecmp(flds[0].u.sym_val, VALIDs)) {
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }

    fi->alpm_enabled = true;

    if ((alpm_temp == 1) || (alpm_temp == 3) || (alpm_temp == 5) ||
        (alpm_temp == 7) || (alpm_temp == 9) || (alpm_temp == 11)) {
        fi->alpm_mode = ALPM_MODE_COMBINED;
    } else if ((alpm_temp == 2) || (alpm_temp == 4) || (alpm_temp == 6) ||
                (alpm_temp == 10)) {
        fi->alpm_mode = ALPM_MODE_PARALLEL;
    } else {
        fi->alpm_mode = ALPM_MODE_NONE;
    }

    if (ltsw_feature(unit, LTSW_FT_URPF) &&
        ((alpm_temp == 5) || (alpm_temp == 6) || (alpm_temp == 7) ||
         (alpm_temp == 11))) {
        fi->urpf_enabled = true;
    } else {
        fi->urpf_enabled = false;
    }

    sz = sizeof(uint32_t) * L3_FIB_VRF_CNT(unit);
    for (i = 0; i < IP_TYPE_CNT; i++) {
        SHR_ALLOC(fi->alpm_route_cnt[i], sz, "bcmLtswAlpmRouteCnt");
        sal_memset(fi->alpm_route_cnt[i], 0, sz);
    }

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (l3_alpm_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init ALPM control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_init(int unit)
{
    bcmi_ltsw_uft_mode_t uft_mode = bcmiUftModeInvalid;
    l3_fib_info_t *fi = &l3_fib_info[unit];
    int alpm_temp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_uft_mode_get(unit, &uft_mode));

    if (uft_mode < bcmiUftMode1) {
        fi->alpm_enabled = false;
        fi->urpf_enabled = false;
        SHR_EXIT();
    }

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (l3_alpm_control_set(unit, alpm_temp, uft_mode));

        SHR_IF_ERR_EXIT
            (l3_alpm_table_control_set(unit, alpm_temp, uft_mode));

        SHR_IF_ERR_EXIT
            (l3_alpm_strength_set(unit));
    }

    SHR_IF_ERR_EXIT
        (l3_alpm_info_init(unit, alpm_temp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-Init ALPM.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
l3_alpm_deinit(int unit)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];
    int i;

    for (i = 0; i < IP_TYPE_CNT; i++) {
        SHR_FREE(fi->alpm_route_cnt[i]);
    }
}

/*!
 * \brief Get the bitmap of supported ipv6 mc defip table.
 *
 * \param [in] unit Unit number.
 * \param [out] bmp Bitmap of ipv6 mc defip table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mc_ipv6_defip_tbl_get(int unit, uint8 *bmp)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t th = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ei;
    int i, num;
    int dunit;
    uint64_t max[] = {0, 0};
    const char *ltname[] = {
        L3_IPV6_MULTICAST_DEFIP_64_TABLEs,
        L3_IPV6_MULTICAST_DEFIP_128_TABLEs,
    };

    SHR_FUNC_ENTER(unit);

    num = COUNTOF(ltname);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &th));

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, TABLE_INFOs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, ltname[i]));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(th, BCMLT_OPCODE_LOOKUP, eh));
        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, th, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_num_get(th, i, &ei));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl, ENTRY_LIMITs, &max[i]));
    }

    *bmp = 0;

    if (max[0] > 0) {
        *bmp |= L3_FIB_IPV6_MC_DEFIP_64;
    }

    if (max[1] > 0) {
        *bmp |= L3_FIB_IPV6_MC_DEFIP_128;
    }

exit:

    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    if (th != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(th);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init IP lookup default tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip_lookup_default_table_init(int unit)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname[2] = {
        L3_IP_LOOKUP0_DEFAULT_TABLEs,
        L3_IP_LOOKUP1_DEFAULT_TABLEs
    };
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth[2] = {
        BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP0_DEFAULT,
        BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP1_DEFAULT
    };
    int i, strength_idx;
    uint32_t max;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < 2; i++) {

        /* In some UFT mode, no UFT resource assigned to these two LTs. */
        SHR_IF_ERR_EXIT
            (bcmi_lt_capacity_get(unit, ltname[i], &max));

        if (!max) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ltname[i], &eh));

        SHR_IF_ERR_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth[i],
                                                 BCMI_LTSW_SBR_PET_DEF,
                                                 &strength_idx));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, VALUE_ZEROs, 0));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, STRENGTH_PROFILE_INDEXs, strength_idx));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));

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
 * \brief Init L3_UC_CONTROL table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_uc_control_table_init(int unit)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, L3_UC_CONTROLs, &eh));

    /* Disable drop on L3_IIF_EQ_L3_OIF. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, URPF_DROPs, 0));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

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
xfs_ltsw_l3_fib_init(int unit)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    const char *tbl_name, *fld_name;
    uint64_t max_vrf = 0;
    int max_egr_obj, max_idx;

    SHR_FUNC_ENTER(unit);

    if (fi->inited) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_l3_fib_deinit(unit));
    }

    sal_memset(fi, 0, sizeof(l3_fib_info_t));

    fi->mutex = sal_mutex_create("xfsltswl3FibInfo");
    SHR_NULL_CHECK(fi->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit,
                                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                                    &tbl_info));
    tbl_name = tbl_info->name;
    fld_name = tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF].fld_name;
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, tbl_name, fld_name,
                                       NULL, &max_vrf));
    fi->max_vrf_id = (uint16_t)max_vrf;

    fi->reduced_route_max_ecmp_grp = bcmi_ltsw_property_get(unit,
                                BCMI_CPN_L3_REDUCED_ROUTE_MAX_ECMP_GROUP_INDEX,
                                0);
    if (fi->reduced_route_max_ecmp_grp) {
        if (((fi->reduced_route_max_ecmp_grp) &
             (fi->reduced_route_max_ecmp_grp + 1)) != 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "l3_reduced_route_max_ecmp_group_index %d is "
                                  "not ((power of 2) - 1).\n "),
                       fi->reduced_route_max_ecmp_grp));
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }

        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_id_range_get(unit,
                                              BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                              NULL, &max_egr_obj));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_egress_obj_id_resolve(unit, max_egr_obj, &max_idx,
                                                NULL));
        if (fi->reduced_route_max_ecmp_grp > max_idx) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "l3_reduced_route_max_ecmp_group_index %d "
                                  "exceeds the maximum overlay ECMP(%d).\n "),
                       fi->reduced_route_max_ecmp_grp,
                       max_idx));
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    SHR_IF_ERR_EXIT
        (l3_mc_ipv6_defip_tbl_get(unit, &fi->ipv6_mc_defip_tbl_bmp));

    SHR_IF_ERR_EXIT
        (l3_alpm_init(unit));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (l3_mtu_recover(unit));
    } else {
        SHR_IF_ERR_EXIT
            (l3_ip_lookup_default_table_init(unit));

        SHR_IF_ERR_EXIT
            (l3_uc_control_table_init(unit));
    }

    fi->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        l3_alpm_deinit(unit);
        if (fi->mutex) {
            sal_mutex_destroy(fi->mutex);
        }
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_fib_deinit(int unit)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!fi->inited || !fi->mutex) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(fi->mutex, SAL_MUTEX_FOREVER);

    fi->inited = 0;

    sal_mutex_give(fi->mutex);

    SHR_IF_ERR_CONT
        (l3_fib_tables_clear(unit));

    l3_alpm_deinit(unit);

    sal_mutex_destroy(fi->mutex);

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_fib_vrf_max_get(int unit, uint32_t *max_vrf)
{
    uint64_t max;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    const char *tbl_name, *fld_name;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit,
                                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                                    &tbl_info));
    tbl_name = tbl_info->name;
    fld_name = tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF].fld_name;
    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, tbl_name, fld_name,
                                       NULL, &max));

    *max_vrf = max;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bcmlt_opcode_t op;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;
    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    /* Data fields. */
    SHR_IF_ERR_EXIT
        (l3_fib_to_uc_cfg(unit, tbl_info, fib, uc));

    op = (fib->flags & BCM_L3_REPLACE) ? BCMLT_OPCODE_UPDATE :
                                         BCMLT_OPCODE_INSERT;

    SHR_IF_ERR_EXIT
        (l3_fib_host_uc_op(unit, tbl_info, op, &cfg));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    /* Key. */
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    SHR_IF_ERR_EXIT
        (l3_fib_host_uc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.filter = l3_host_trav_filter_prefix_v4;
    trav_info.flt_crtr.vrf = fib->vrf;
    trav_info.flt_crtr.ip_addr = fib->ip_addr;
    trav_info.flt_crtr.ip_addr_mask = fib->ip_addr_mask;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;
    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    SHR_IF_ERR_EXIT
        (l3_fib_host_uc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_uc_traverse(int unit, uint32_t flags,
                                  uint32_t start, uint32_t end,
                                  bcm_l3_host_traverse_cb cb, void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.filter = l3_trav_filter_range;
    trav_info.flt_crtr.start = start;
    trav_info.flt_crtr.end = end;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.host_trav_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg, pre_cfg;
    int rv;
    bcmlt_opcode_t op;
    bool locked = false;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    uint32_t hit_act;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Key. */
    l3_fib_to_mc_cfg_key_host_v4(unit, fib, &cfg.cfg.mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    L3_FIB_LOCK(unit);
    locked = true;

    op = BCMLT_OPCODE_INSERT;

    if (fib->flags & BCM_L3_REPLACE) {
        rv = bcmi_ltsw_ipmc_hit_flexctr_action_id_get(unit, &hit_act);
        if (SHR_FAILURE(rv)) {
            hit_act = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        }

        /* Copy the Key fileds. */
        sal_memcpy(&pre_cfg, &cfg, sizeof(l3_cfg_t));

        /* Get the previous MTU pointer and flexctr action. */
        pre_cfg.cfg.mc.fld_bmp =
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);

        rv = l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &pre_cfg);
        if (SHR_SUCCESS(rv)) {
            if ((pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != BCMI_LTSW_FLEXCTR_ACTION_INVALID) &&
                (pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != hit_act)) {
                SHR_ERR_MSG_EXIT(SHR_E_BUSY,
                                 (BSL_META_U(unit,
                                            "Can't replace the entry which "
                                            "has flexctr running.\n")));
            }
            op = BCMLT_OPCODE_UPDATE;
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
    }

    cfg.cfg.mc.fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_fib_to_mc_cfg(unit, tbl_info, fib, &cfg.cfg.mc));

    rv = l3_fib_mc_op(unit, tbl_info, op, &cfg);
    if (SHR_FAILURE(rv)) {
        mtu_cfg.index =
            cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX];
        (void)bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg);
        SHR_ERR_EXIT(rv);
    }

    /* Decrease the reference count for the previous MC entry. */
    if (op == BCMLT_OPCODE_UPDATE) {
        SHR_IF_ERR_EXIT
            (l3_mc_refcnt_decr(unit, tbl_info, &pre_cfg));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v4(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    L3_FIB_LOCK(unit);
    locked = true;

    /* Get the previous MTU pointer and flexctr action. */
    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    mc->fld_bmp = 0;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));

    SHR_IF_ERR_EXIT
        (l3_mc_refcnt_decr(unit, tbl_info, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v4(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = tbl_info->fld_bmp;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = trav_fn;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G,
                         &trav_info));
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG,
                         &trav_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v4(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);
    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = ci->action_index;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v4(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv4_mc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                          bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v4(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    ci->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    ci->action_index = mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION];
    ci->table_name = tbl_info->name;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bcmlt_opcode_t op;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;
    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    bcmi_ltsw_util_ip6_to_uint64(&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0],
                                 &fib->ip6_addr);
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    /* Data. */
    SHR_IF_ERR_EXIT
        (l3_fib_to_uc_cfg(unit, tbl_info, fib, uc));

    op = (fib->flags & BCM_L3_REPLACE) ? BCMLT_OPCODE_UPDATE :
                                         BCMLT_OPCODE_INSERT;

    SHR_IF_ERR_EXIT
        (l3_fib_host_uc_op(unit, tbl_info, op, &cfg));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    bcmi_ltsw_util_ip6_to_uint64(&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0],
                                 &fib->ip6_addr);
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    SHR_IF_ERR_EXIT
        (l3_fib_host_uc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_uc_del_by_prefix(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.filter = l3_host_trav_filter_prefix_v6;
    trav_info.flt_crtr.vrf = fib->vrf;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    bcmi_ltsw_util_ip6_to_uint64(trav_info.flt_crtr.ip6_addr, &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64(trav_info.flt_crtr.ip6_addr_mask,
                                 &fib->ip6_addr_mask);

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_uc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;
    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    bcmi_ltsw_util_ip6_to_uint64(&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0],
                                 &fib->ip6_addr);
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    SHR_IF_ERR_EXIT
        (l3_fib_host_uc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_host_traverse_cb cb,
                                   void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.filter = l3_trav_filter_range;
    trav_info.flt_crtr.start = start;
    trav_info.flt_crtr.end = end;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.host_trav_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg, pre_cfg;
    int rv;
    bcmlt_opcode_t op;
    bool locked = false;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    uint32_t hit_act;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Key. */
    l3_fib_to_mc_cfg_key_host_v6(unit, fib, &cfg.cfg.mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    L3_FIB_LOCK(unit);
    locked = true;

    op = BCMLT_OPCODE_INSERT;

    if (fib->flags & BCM_L3_REPLACE) {
        rv = bcmi_ltsw_ipmc_hit_flexctr_action_id_get(unit, &hit_act);
        if (SHR_FAILURE(rv)) {
            hit_act = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        }

        /* Copy the Key fileds. */
        sal_memcpy(&pre_cfg, &cfg, sizeof(l3_cfg_t));

        /* Get the previous MTU pointer and flexctr action. */
        pre_cfg.cfg.mc.fld_bmp =
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);

        rv = l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &pre_cfg);
        if (SHR_SUCCESS(rv)) {
            if ((pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != BCMI_LTSW_FLEXCTR_ACTION_INVALID) &&
                (pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != hit_act)) {
                SHR_ERR_MSG_EXIT(SHR_E_BUSY,
                                 (BSL_META_U(unit,
                                            "Can't replace the entry which "
                                            "has flexctr running.\n")));
            }
            op = BCMLT_OPCODE_UPDATE;
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
    }

    cfg.cfg.mc.fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_fib_to_mc_cfg(unit, tbl_info, fib, &cfg.cfg.mc));

    rv = l3_fib_mc_op(unit, tbl_info, op, &cfg);
    if (SHR_FAILURE(rv)) {
        mtu_cfg.index =
            cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX];
        (void)bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg);
        SHR_ERR_EXIT(rv);
    }

    /* Decrease the reference count for the previous MC entry. */
    if (op == BCMLT_OPCODE_UPDATE) {
        SHR_IF_ERR_EXIT
            (l3_mc_refcnt_decr(unit, tbl_info, &pre_cfg));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v6(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    L3_FIB_LOCK(unit);
    locked = true;

    /* Get the previous MTU pointer and flexctr action. */
    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    mc->fld_bmp = 0;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));

    SHR_IF_ERR_EXIT
        (l3_mc_refcnt_decr(unit, tbl_info, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v6(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = trav_fn;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G,
                         &trav_info));
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG,
                         &trav_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                        bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v6(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);
    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = ci->action_index;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v6(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_ipv6_mc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                          bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_host_v6(unit, fib, mc, &tbl_id);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    ci->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    ci->action_index = mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION];
    ci->table_name = tbl_info->name;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.filter = l3_uc_trav_filter_intf;
    trav_info.flt_crtr.ol_intf = fib->ol_intf;
    trav_info.flt_crtr.ul_intf = fib->ul_intf;
    trav_info.flt_crtr.negate = negate;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;

    if (fib->flags & BCM_L3_IP6) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC,
                         &trav_info));
    } else {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
                         &trav_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_del_all(int unit, uint32_t attrib)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_UC) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            trav_info.filter = l3_host_filter_nat;
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_NAT) {
                trav_info.flt_crtr.nat = 1;
            }

            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC,
                             &trav_info));
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC,
                             &trav_info));
        }
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_MC) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G,
                                 &trav_info));
            }
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG,
                                 &trav_info));
            }
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G,
                                 &trav_info));
            }
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG,
                                 &trav_info));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti_d = NULL, *ti_s = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bcmlt_opcode_t op;
    bool new_added = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    op = (fib->flags & BCM_L3_REPLACE) ? BCMLT_OPCODE_UPDATE :
                                         BCMLT_OPCODE_INSERT;

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti_d));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = ti_d->fld_bmp;

    /* Key. */
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
    if (ti_d->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    /* Data fields. */
    SHR_IF_ERR_EXIT
        (l3_fib_to_uc_cfg(unit, ti_d, fib, uc));

    SHR_IF_ERR_EXIT
        (l3_fib_uc_alpm_op(unit, ti_d, op, false, &cfg, &new_added));

    /* Source route. */
    if (L3_FIB_URPF_ENABLED(unit) && ltsw_feature(unit, LTSW_FT_URPF)) {
        SHR_IF_ERR_EXIT
            (l3_fib_to_src_uc_alpm_cfg(unit, fib, uc));

        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
        }

        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti_s));

        uc->fld_bmp = ti_s->fld_bmp;

        SHR_IF_ERR_EXIT
            (l3_fib_src_uc_alpm_op(unit, ti_s, op, &cfg));
   }

exit:
    if (SHR_FUNC_ERR() && new_added) {
        uc->fld_bmp = 0;
        (void)l3_fib_uc_alpm_op(unit, ti_d, BCMLT_OPCODE_DELETE, false, &cfg,
                                NULL);
    }
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    /* Key. */
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
    if (tbl_info->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    SHR_IF_ERR_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_DELETE, false, &cfg,
                           NULL));

    /* Source route. */
    if (ltsw_feature(unit, LTSW_FT_URPF) && L3_FIB_URPF_ENABLED(unit)) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
        }

        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

        SHR_IF_ERR_EXIT
            (l3_fib_src_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));
   }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
    if (tbl_info->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    SHR_IF_ERR_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, lpm, &cfg,
                           NULL));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.filter = l3_trav_filter_range;
    trav_info.flt_crtr.start = start;
    trav_info.flt_crtr.end = end;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.route_trav_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg, pre_cfg;
    int rv;
    bcmlt_opcode_t op;
    bool locked = false;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    uint32_t hit_act;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Key. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, &cfg.cfg.mc);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                    &tbl_info));

    op = BCMLT_OPCODE_INSERT;

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->flags & BCM_L3_REPLACE) {
        rv = bcmi_ltsw_ipmc_hit_flexctr_action_id_get(unit, &hit_act);
        if (SHR_FAILURE(rv)) {
            hit_act = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        }

        /* Copy the Key fileds. */
        sal_memcpy(&pre_cfg, &cfg, sizeof(l3_cfg_t));

        /* Get the previous MTU pointer and flexctr action. */
        pre_cfg.cfg.mc.fld_bmp =
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);

        rv = l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &pre_cfg);
        if (SHR_SUCCESS(rv)) {
            if ((pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != BCMI_LTSW_FLEXCTR_ACTION_INVALID) &&
                (pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != hit_act)) {
                SHR_ERR_MSG_EXIT(SHR_E_BUSY,
                                 (BSL_META_U(unit,
                                            "Can't replace the entry which "
                                            "has flexctr running.\n")));
            }
            op = BCMLT_OPCODE_UPDATE;
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
    }

    cfg.cfg.mc.fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_fib_to_mc_cfg(unit, tbl_info, fib, &cfg.cfg.mc));

    rv = l3_fib_mc_op(unit, tbl_info, op, &cfg);
    if (SHR_FAILURE(rv)) {
        mtu_cfg.index =
            cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX];
        (void)bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg);
        SHR_ERR_EXIT(rv);
    }

    /* Decrease the reference count for the previous MC entry. */
    if (op == BCMLT_OPCODE_UPDATE) {
        SHR_IF_ERR_EXIT
            (l3_mc_refcnt_decr(unit, tbl_info, &pre_cfg));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    L3_FIB_LOCK(unit);
    locked = true;

    /* Get the previous MTU pointer and flexctr action. */
    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    mc->fld_bmp = 0;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));

    SHR_IF_ERR_EXIT
        (l3_mc_refcnt_decr(unit, tbl_info, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (lpm) {
        /* MC Route managed by RM-TCAM, which doesn't support LPM option. */
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    mc->fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = trav_fn;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;
    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                         bcmi_ltsw_flexctr_counter_info_t *ci)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);
    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = ci->action_index;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv4_mc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                           bcmi_ltsw_flexctr_counter_info_t *ci)
{
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                    &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    ci->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    ci->action_index = mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION];
    ci->table_name = tbl_info->name;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti_d = NULL, *ti_s = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bcmlt_opcode_t op;
    bool new_added = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    op = (fib->flags & BCM_L3_REPLACE) ? BCMLT_OPCODE_UPDATE :
                                         BCMLT_OPCODE_INSERT;

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti_d));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = ti_d->fld_bmp;

    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
         &fib->ip6_addr_mask);
    if (ti_d->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF))
        uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;

    SHR_IF_ERR_EXIT
        (l3_fib_to_uc_cfg(unit, ti_d, fib, uc));

    SHR_IF_ERR_EXIT
        (l3_fib_uc_alpm_op(unit, ti_d, op, false, &cfg, &new_added));

    /* Source route. */
    if (ltsw_feature(unit, LTSW_FT_URPF) && L3_FIB_URPF_ENABLED(unit)) {
        SHR_IF_ERR_EXIT
            (l3_fib_to_src_uc_alpm_cfg(unit, fib, uc));

        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
        }

        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti_s));

        uc->fld_bmp = ti_s->fld_bmp;

        SHR_IF_ERR_EXIT
            (l3_fib_src_uc_alpm_op(unit, ti_s, op, &cfg));
    }

exit:
    if (SHR_FUNC_ERR() && new_added) {
        uc->fld_bmp = 0;
        (void)l3_fib_uc_alpm_op(unit, ti_d, BCMLT_OPCODE_DELETE, false, &cfg,
                                NULL);
    }
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    /* Key. */
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
         &fib->ip6_addr_mask);
    if (tbl_info->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    SHR_IF_ERR_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_DELETE, false, &cfg,
                           NULL));

    /* Source route. */
    if (ltsw_feature(unit, LTSW_FT_URPF) && L3_FIB_URPF_ENABLED(unit)) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
        }

        SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

        SHR_IF_ERR_EXIT
            (l3_fib_src_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));
        }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (fib->vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
    } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = tbl_info->fld_bmp;

    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
         &fib->ip6_addr_mask);
    if (tbl_info->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    SHR_IF_ERR_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, lpm, &cfg,
                           NULL));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.filter = l3_trav_filter_range;
    trav_info.flt_crtr.start = start;
    trav_info.flt_crtr.end = end;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.route_trav_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg, pre_cfg;
    int rv;
    bcmlt_opcode_t op;
    bool locked = false;
    bcmi_ltsw_l3_mtu_cfg_t mtu_cfg;
    uint32_t hit_act;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Key. */
    if (SHR_FAILURE(l3_fib_to_mc_cfg_key_route_v6(unit, fib, &cfg.cfg.mc,
                                                  &tbl_id))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    op = BCMLT_OPCODE_INSERT;

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->flags & BCM_L3_REPLACE) {
        rv = bcmi_ltsw_ipmc_hit_flexctr_action_id_get(unit, &hit_act);
        if (SHR_FAILURE(rv)) {
            hit_act = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        }

        /* Copy the Key fileds. */
        sal_memcpy(&pre_cfg, &cfg, sizeof(l3_cfg_t));

        /* Get the previous MTU pointer and flexctr action. */
        pre_cfg.cfg.mc.fld_bmp =
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
            (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);

        rv = l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &pre_cfg);
        if (SHR_SUCCESS(rv)) {
            if ((pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != BCMI_LTSW_FLEXCTR_ACTION_INVALID) &&
                (pre_cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION]
                 != hit_act)) {
                SHR_ERR_MSG_EXIT(SHR_E_BUSY,
                                 (BSL_META_U(unit,
                                            "Can't replace the entry which "
                                            "has flexctr running.\n")));
            }
            op = BCMLT_OPCODE_UPDATE;
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
    }

    cfg.cfg.mc.fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_EXIT
        (l3_fib_to_mc_cfg(unit, tbl_info, fib, &cfg.cfg.mc));

    rv = l3_fib_mc_op(unit, tbl_info, op, &cfg);
    if (SHR_FAILURE(rv)) {
        mtu_cfg.index =
            cfg.cfg.mc.flds[BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX];
        (void)bcmi_ltsw_l3_mtu_delete(unit, &mtu_cfg);
        SHR_ERR_EXIT(rv);
    }

    /* Decrease the reference count for the previous MC entry. */
    if (op == BCMLT_OPCODE_UPDATE) {
        SHR_IF_ERR_EXIT
            (l3_mc_refcnt_decr(unit, tbl_info, &pre_cfg));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    if (SHR_FAILURE(l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc, &tbl_id))) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    L3_FIB_LOCK(unit);
    locked = true;

    /* Get the previous MTU pointer and flexctr action. */
    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                  (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID);
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    mc->fld_bmp = 0;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, &cfg));

    SHR_IF_ERR_EXIT
        (l3_mc_refcnt_decr(unit, tbl_info, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (lpm) {
        /* MC Route managed by RM-TCAM, which doesn't support LPM option. */
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    if (SHR_FAILURE(l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc, &tbl_id))) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));


    mc->fld_bmp = tbl_info->fld_bmp;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    SHR_IF_ERR_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = trav_fn;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;

    if (L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit)) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64,
                         &trav_info));
    }

    if (L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                         &trav_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                         bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    if (SHR_FAILURE(l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc, &tbl_id))) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);
    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = ci->action_index;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    if (SHR_FAILURE(l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc, &tbl_id))) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);
    mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = 0;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_UPDATE, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_ipv6_mc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                           bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    if (SHR_FAILURE(l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc, &tbl_id))) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION);

    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, &cfg));

    ci->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    ci->action_index = mc->flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION];
    ci->table_name = tbl_info->name;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    l3_trav_t trav_info;
    bool ipv6 = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.filter = l3_uc_trav_filter_intf;
    trav_info.flt_crtr.ol_intf = fib->ol_intf;
    trav_info.flt_crtr.ul_intf = fib->ul_intf;
    trav_info.flt_crtr.negate = negate;

    if (fib->flags & BCM_L3_IP6) {
        ipv6 = true;
    }

    tbl_id = ipv6 ? BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH :
                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
    SHR_IF_ERR_EXIT
        (l3_traverse(unit, tbl_id, &trav_info));

    tbl_id = ipv6 ? BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF :
                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
    SHR_IF_ERR_EXIT
        (l3_traverse(unit, tbl_id, &trav_info));

    tbl_id = ipv6 ? BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL :
                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
    SHR_IF_ERR_EXIT
        (l3_traverse(unit, tbl_id, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_del_all(int unit, uint32_t attrib)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_UC) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                             &trav_info));

            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                             &trav_info));

            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL,
                             &trav_info));
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH,
                             &trav_info));

            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                             &trav_info));

            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                             &trav_info));
        }
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_MC) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            SHR_IF_ERR_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                             &trav_info));
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {

            if (L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit)) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64,
                                 &trav_info));
            }

            if (L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                                 &trav_info));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_vrf_flexctr_attach(int unit, bcm_vrf_t vrf,
                               bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (info->source != bcmFlexctrSourceIngVrf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_VRF_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, VRFs, vrf));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, FLEX_CTR_ACTIONs, info->action_index));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_vrf_flexctr_detach(int unit, bcm_vrf_t vrf)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_VRF_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, VRFs, vrf));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();

}

int
xfs_ltsw_l3_vrf_flexctr_info_get(int unit, bcm_vrf_t vrf,
                                 bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit, rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!info || (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_VRF_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, VRFs, vrf));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_EXIT(rv);
    }

    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, FLEX_CTR_ACTIONs, &val));
    } else {
        val = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    }

    info->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->action_index = val;
    info->source = bcmFlexctrSourceIngVrf;
    info->table_name = ING_VRF_TABLEs;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_vrf_route_cnt_get(int unit, bcm_vrf_t vrf, bool ipv6, uint32_t *cnt)
{
    uint16_t vrf_id;
    int ipt;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_ALPM_ENABLED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    ipt = ipv6 ? IP_TYPE_V6 : IP_TYPE_V4;

    if (vrf == BCM_L3_VRF_OVERRIDE) {
        vrf_id = L3_FIB_VRF_GHI(unit);
    } else if (vrf == BCM_L3_VRF_GLOBAL) {
        vrf_id = L3_FIB_VRF_GLO(unit);
    } else {
        vrf_id = (uint16_t) vrf;
    }

    *cnt = VRF_ALPM_ROUTE_CNT(unit, vrf_id, ipt);

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_nat_host_traverse(int unit, uint32_t flags,
                              bcm_l3_nat_ingress_traverse_cb trav_fn,
                              bcmi_ltsw_l3_fib_to_nat_cb cb,
                              void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.filter = l3_host_filter_nat;
    trav_info.flt_crtr.nat = 1;
    trav_info.flt_crtr.flags = flags;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.nat_trav_cb = trav_fn;
    trav_info.fib_to_nat_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_same_intf_drop_set(int unit, int enable)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, L3_UC_CONTROLs, &eh));

    val = (enable > 0) ? L3_UC_IIF_EQ_OIF_DROP_CODE : 0;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, URPF_DROPs, val));

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
xfs_ltsw_l3_same_intf_drop_get(int unit, int *enable)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, L3_UC_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, URPF_DROPs, &val));
    *enable = (val > 0) ? 1 : 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_ipmc_age(int unit, uint32_t attrib,
                     bcm_ipmc_traverse_cb age_cb,
                     bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                     void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_AGE;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = age_cb;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_HOST) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G,
                                 &trav_info));
            }

            if (attrib & BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG,
                                 &trav_info));
            }
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            if (attrib & BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G,
                                 &trav_info));
            }

            if (attrib & BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG,
                                 &trav_info));
            }
        }
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_NETWORK) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                                 &trav_info));
        }

        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            if (L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit)) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64,
                                 &trav_info));
            }

            if (L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
                SHR_IF_ERR_EXIT
                    (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                                 &trav_info));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_host_hit_clear(int unit)
{
    l3_trav_t trav_info;
    uint32_t act_id;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (SHR_FAILURE(bcmi_ltsw_ipmc_hit_flexctr_action_id_get(unit, &act_id)) ||
        (act_id == BCMI_LTSW_FLEXCTR_ACTION_INVALID)) {
        SHR_EXIT();
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.hdl_to_cfg_cb = l3_mc_eh_to_cfg_hitclr;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG,
                     &trav_info));
    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G,
                     &trav_info));
    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_route_hit_clear(int unit)
{
    l3_trav_t trav_info;
    uint32_t act_id;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (SHR_FAILURE(bcmi_ltsw_ipmc_hit_flexctr_action_id_get(unit, &act_id)) ||
        (act_id == BCMI_LTSW_FLEXCTR_ACTION_INVALID)) {
        SHR_EXIT();
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.hdl_to_cfg_cb = l3_mc_eh_to_cfg_hitclr;

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                     &trav_info));

    SHR_IF_ERR_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64,
                     &trav_info));

    if (L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit)) {
        SHR_IF_ERR_EXIT
            (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                         &trav_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_aacl_add(
    int unit,
    uint32_t options,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_aacl_add(unit, options, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_aacl_delete(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_aacl_delete(unit, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_aacl_delete_all(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_aacl_delete_all(unit, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_aacl_find(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_aacl_find(unit, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_aacl_traverse(
    int unit,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_aacl_traverse(unit, trav_fn, user_data));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_aacl_matched_traverse(
    int unit,
    uint32 flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_aacl_matched_traverse(unit, flags, trav_fn, user_data));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

void
xfs_ltsw_l3_mc_defip_get(int unit, int *v6_64_en, int *v6_128_en)
{
    *v6_64_en = L3_FIB_IPV6_MC_DEFIP_64_ENABLED(unit);
    *v6_128_en = L3_FIB_IPV6_MC_DEFIP_128_ENABLED(unit);
}

uint32_t
xfs_ltsw_l3_reduced_route_max_ecmp_group_get(int unit)
{
    return L3_REDUCED_ROUTE_MAX_ECMP_GRP(unit);
}

int
xfs_ltsw_l3_route_perf_v4_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint32 ip,
    uint32 ip_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    int dunit;
    const char *alpm_dmode_str = NULL;
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti = NULL;
    const bcmint_ltsw_l3_fib_fld_t *fi;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t lt_op;
    int idx = 0; /*idx order defined in setup: normal vrf, global, override.  */
    int rv = BCM_E_NONE;

    if (vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
        idx = 1;
    } else if (vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
        idx = 2;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
        idx = 0;
    }

    rv = bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    fi = ti->flds;

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0].fld_id,
            ip);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK].fld_id,
            ip_mask);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_LTSW_L3_FIB_TBL_UC_VRF].fld_id,
                vrf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
    }

    switch(op) {
    case BCMI_LTSW_RP_OPCODE_ADD:
        lt_op = BCMLT_OPCODE_INSERT;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_LTSW_L3_FIB_TBL_UC_DEST].fld_id,
                intf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
        if (alpm_dmode[idx][0] != -1) {
            rv = fi[BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE].scalar_to_symbol(unit,
                                                                          alpm_dmode[idx][0],
                                                                          &alpm_dmode_str);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }
            rv = bcmlt_entry_field_symbol_add_by_id(eh,
                    fi[BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE].fld_id,
                    alpm_dmode_str);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }
        }
        break;
    case BCMI_LTSW_RP_OPCODE_UPD:
        lt_op = BCMLT_OPCODE_UPDATE;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_LTSW_L3_FIB_TBL_UC_DEST].fld_id,
                intf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
        break;
    case BCMI_LTSW_RP_OPCODE_GET:
        lt_op = BCMLT_OPCODE_LOOKUP;
        break;
    case BCMI_LTSW_RP_OPCODE_DEL:
        lt_op = BCMLT_OPCODE_DELETE;
        break;
    default:
        goto exit;
    }

    rv = bcmlt_entry_commit(eh, lt_op, BCMLT_PRIORITY_NORMAL);

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    return rv;
}

int
xfs_ltsw_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    int dunit;
    const char *alpm_dmode_str = NULL;
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti = NULL;
    const bcmint_ltsw_l3_fib_fld_t *fi;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t lt_op;
    int idx = 0; /*idx order defined in setup: normal vrf, global, override.  */
    int rv = BCM_E_NONE;

    if (vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
        idx = 1;
    } else if (vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
        idx = 2;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
        idx = 0;
    }

    rv = bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    fi = ti->flds;

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0].fld_id,
            v6[0]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK].fld_id,
            v6_mask[0]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1].fld_id,
            v6[1]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK].fld_id,
            v6_mask[1]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_LTSW_L3_FIB_TBL_UC_VRF].fld_id,
                vrf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
    }

    switch(op) {
    case BCMI_LTSW_RP_OPCODE_ADD:
        lt_op = BCMLT_OPCODE_INSERT;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_LTSW_L3_FIB_TBL_UC_DEST].fld_id,
                intf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
        if (alpm_dmode[idx][1] != -1) {
            rv = fi[BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE].scalar_to_symbol(unit,
                                                                          alpm_dmode[idx][0],
                                                                          &alpm_dmode_str);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }
            rv = bcmlt_entry_field_symbol_add_by_id(eh,
                    fi[BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE].fld_id,
                    alpm_dmode_str);
            if (BCM_FAILURE(rv)) {
                goto exit;
            }
        }
        break;
    case BCMI_LTSW_RP_OPCODE_UPD:
        lt_op = BCMLT_OPCODE_UPDATE;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_LTSW_L3_FIB_TBL_UC_DEST].fld_id,
                intf);
        break;
    case BCMI_LTSW_RP_OPCODE_GET:
        lt_op = BCMLT_OPCODE_LOOKUP;
        break;
    case BCMI_LTSW_RP_OPCODE_DEL:
        lt_op = BCMLT_OPCODE_DELETE;
        break;
    default:
        goto exit;
    }

    rv = bcmlt_entry_commit(eh, lt_op, BCMLT_PRIORITY_NORMAL);

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    return rv;
}


