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
#include <bcm/switch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/xgs/types.h>
#include <bcm_int/ltsw/xgs/l3_fib.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/mbcm/l3_egress.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* Mask for all data fields. */
#define L3_FIB_FLD_ALL ((uint64_t)-1)

/* ALPM MODE. */
#define ALPM_MODE_COMBINED  0
#define ALPM_MODE_PARALLEL  1
#define ALPM_MODE_NONE      2

/* IP type count. */
#define IP_TYPE_V4          0
#define IP_TYPE_V6          1
#define IP_TYPE_CNT         2

/* Data structure to save the info of entry for below LT:
 * L3_IPV4_MC_ROUTEt
 * L3_IPV6_MC_ROUTEt
 */
typedef struct l3_mc_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t flds[BCMINT_XGS_L3_FIB_TBL_MC_FLD_CNT];
} l3_mc_cfg_t;

/* Data structure to save the info of entry for below LT:
 * L3_IPV4_UC_ROUTEt
 * L3_IPV4_UC_ROUTE_VRFt
 * L3_IPV4_UC_ROUTE_OVERRIDEt
 * L3_IPV6_UC_ROUTEt
 * L3_IPV6_UC_ROUTE_VRFt
 * L3_IPV6_UC_ROUTE_OVERRIDEt
 */
typedef struct l3_uc_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT];
} l3_uc_cfg_t;

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
    bcm_if_t intf;

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
typedef int (*l3_filter_f)(int unit, uint32_t index,
                           l3_filter_criteria_t *flt_crtr,
                           l3_cfg_t *cfg, l3_filter_cmd_t *flt_cmd);

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
#define L3_TRAV_OP_DEL      1
#define L3_TRAV_OP_AGE      2
#define L3_TRAV_OP_CLR_HIT  3


    /* Table attributes. */
    uint32_t tbl_attrib;

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

    /* ALPM mode. */
    int alpm_mode;

    /* SIP lookup enabled. */
    bool src_lookup_enabled;

    /* ALPM route count per VRF per IP-type. */
    uint32_t *alpm_route_cnt[IP_TYPE_CNT];

    /* Route INT action enabled. */
    bool alpm_int_action_enabled;

    /* Hit enabled. */
    bool alpm_hit_enabled;
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

#define L3_ALPM_ENABLED(_u) (l3_fib_info[_u].alpm_enabled)
#define L3_ALPM_INT_ACTION_ENABLED(_u) (l3_fib_info[_u].alpm_int_action_enabled)
#define L3_ALPM_HIT_ENABLED(_u) (l3_fib_info[_u].alpm_hit_enabled)

#define L3_ALPM_MODE(_u) (l3_fib_info[_u].alpm_mode)

#define VRF_ALPM_ROUTE_CNT(_u, _vrf_id, _ipt)   \
    l3_fib_info[_u].alpm_route_cnt[_ipt][_vrf_id]

#define VRF_ALPM_ROUTE_INC(_u, _vrf_id, _ipt)   \
    l3_fib_info[_u].alpm_route_cnt[_ipt][_vrf_id]++

#define VRF_ALPM_ROUTE_DEC(_u, _vrf_id, _ipt)   \
    l3_fib_info[_u].alpm_route_cnt[_ipt][_vrf_id]--

static uint8 ip6_zero[BCM_IP6_ADDRLEN] = {0};
static uint8 ip6_full[BCM_IP6_ADDRLEN] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                          0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

typedef struct l3_fib_flexctr_cfg_s {
    /* Operation of flexctr fields: 0 = set, 1 = get. */
    bool op;
#define L3_FIB_FLXCTR_OP_SET   0
#define L3_FIB_FLXCTR_OP_GET   1

    /* Flags indicate which flexctr field to be operated. */
    uint8_t flags;
#define L3_FIB_FLXCTR_F_ACTION     (1 << 0)
#define L3_FIB_FLXCTR_F_INDEX      (1 << 1)

    /* Flex counter action. */
    uint32_t action;

    /*! Distinct flex counter index. */
    uint32_t index;
} l3_fib_flexctr_cfg_t;

/* L3 fib flex counter action type. */
typedef enum l3_fib_flexctr_action_e {
    /* V4MC . */
    BCMINT_XGS_V4MC_FLEXCTR_ACTION = 0,

    /* V4SRC. */
    BCMINT_XGS_V4SRC_FLEXCTR_ACTION = 1,

    /* V4UC. */
    BCMINT_XGS_V4UC_FLEXCTR_ACTION = 2,

    /* V6MC . */
    BCMINT_XGS_V6MC_FLEXCTR_ACTION = 3,

    /* V6SRC. */
    BCMINT_XGS_V6SRC_FLEXCTR_ACTION = 4,

    /* V6UC. */
    BCMINT_XGS_V6UC_FLEXCTR_ACTION = 5,

    /* Cnt. */
    BCMINT_XGS_L3_FIB_FLEXCTR_ACTION_CNT = 6,

} l3_fib_flexctr_action_t;

typedef struct l3_fib_flex_action_profile_s {
    /*! Action. */
    uint32_t action;
} l3_fib_flex_action_profile_t;

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

    fv = &flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK];
    if (attrib & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
        ipt = IP_TYPE_V6;
        bcmi_ltsw_util_uint64_to_ip6(&ip6_mask, fv);
        masklen = bcm_ip6_mask_length(ip6_mask);
    } else {
        ipt = IP_TYPE_V4;
        ip4_mask = *fv;
        masklen = bcm_ip_mask_length(ip4_mask);
    }

    fv = &flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF];
    if (op == BCMLT_OPCODE_INSERT) {
        if (!VRF_ALPM_ROUTE_CNT(unit, (*fv), ipt) && masklen) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "First route in VRF %d has to be a default "
                                  "route in this ALPM mode.\n"), ((int)*fv)));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (op == BCMLT_OPCODE_DELETE) {
        if ((VRF_ALPM_ROUTE_CNT(unit, (*fv), ipt) > 1)  && !masklen) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Default route in VRF %d has to be the last "
                                  "route to delete in this ALPM mode.\n"),
                       ((int)*fv)));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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
        L3_IPV4_UC_ROUTEs,
        L3_IPV4_UC_ROUTE_VRFs,
        L3_IPV4_UC_ROUTE_OVERRIDEs,
        L3_IPV4_MC_ROUTEs,
        L3_IPV6_UC_ROUTEs,
        L3_IPV6_UC_ROUTE_VRFs,
        L3_IPV6_UC_ROUTE_OVERRIDEs,
        L3_IPV6_MC_ROUTEs,
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
        cnt = BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT;
        fv = cfg->cfg.uc.flds;
        fld_bmp = cfg->cfg.uc.fld_bmp;
    } else {
        cnt = BCMINT_XGS_L3_FIB_TBL_MC_FLD_CNT;
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
            SHR_IF_ERR_VERBOSE_EXIT
                (fi[i].scalar_to_symbol(unit, fv[i], &sym_val));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add_by_id(eh,
                                                    fi[i].fld_id,
                                                    sym_val));
        } else {
            if (fi[i].elements) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add_by_id(eh,
                                                       fi[i].fld_id,
                                                       0,
                                                       &fv[i],
                                                       fi[i].elements));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
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
    int rv;

    SHR_FUNC_ENTER(unit);

    if (cfg->uc) {
        cnt = BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT;
        fv = cfg->cfg.uc.flds;
        fld_bmp = cfg->cfg.uc.fld_bmp;
    } else {
        cnt = BCMINT_XGS_L3_FIB_TBL_MC_FLD_CNT;
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
            rv = bcmlt_entry_field_symbol_get_by_id(eh, fi[i].fld_id,
                                                    &sym_val);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (fi[i].symbol_to_scalar(unit, sym_val, &fv[i]));
        } else {
            if (fi[i].elements) {
                rv = bcmlt_entry_field_array_get_by_id(eh,
                                                       fi[i].fld_id,
                                                       0,
                                                       &fv[i],
                                                       fi[i].elements,
                                                       &sz);
                if (rv == SHR_E_NOT_FOUND) {
                    continue;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
            } else {
                rv = bcmlt_entry_field_get_by_id(eh, fi[i].fld_id, &fv[i]);
                if (rv == SHR_E_NOT_FOUND) {
                    continue;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
            }
        }
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
    uint32_t nhidx;
    int intf = 0;
    uint64_t *fv;

    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!cfg->uc) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    fv = cfg->cfg.uc.flds;
    nhidx = fv[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID];

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_construct(unit,
                                              nhidx,
                                              BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                              &intf));

    if (((flt_crtr->intf == intf) && !flt_crtr->negate) ||
        ((flt_crtr->intf != intf) && flt_crtr->negate)) {
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
    SHR_FUNC_ENTER(unit);

    if (!flt_crtr || !cfg || !flt_cmd) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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
    uint64_t *fv = cfg->flds;
    bool src_route = false;
    int class_id;
    bcmi_ltsw_l3_egr_obj_type_t type;
    int idx, min, max;

    SHR_FUNC_ENTER(unit);

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;
    fib->class_id = (int)fv[BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID];

    if (src_route) {
        SHR_EXIT();
    }

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
        fib->flags |= BCM_L3_IP6;
        bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr,
                                     &fv[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0]);
        bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr_mask,
                                     &fv[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK]);
    } else {
        fib->ip_addr = fv[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0];
        if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) {
            fib->ip_addr_mask = fv[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK];
        }
    }

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
        fib->vrf = BCM_L3_VRF_GLOBAL;
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GH) {
        fib->vrf = BCM_L3_VRF_OVERRIDE;
    } else {
        fib->vrf = (bcm_vrf_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_VRF];
    }

    if (fv[BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] || fv[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID]) {
        if (fv[BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP]) {
            idx = (int)(fv[BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID]);
            type = BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_id_range_get(unit, type, &min, &max));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, min, &min, &type));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, max, &max, &type));
            type = ((idx <= max) && (idx >= min)) ?
                   BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL : BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit,
                                                      idx,
                                                      type,
                                                      &fib->ul_intf));

        } else {
            idx = (int)(fv[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID]);
            type = BCMI_LTSW_L3_EGR_OBJ_T_UL;
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_l3_egress_id_range_get(unit, type, &min, &max));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, min, &min, &type));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, max, &max, &type));
            type = ((idx <= max) && (idx >= min)) ?
                   BCMI_LTSW_L3_EGR_OBJ_T_UL : BCMI_LTSW_L3_EGR_OBJ_T_OL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit,
                                                      idx,
                                                      type,
                                                      &fib->ul_intf));
        }
    }

    if (fv[BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH]) {
        fib->flags |= BCM_L3_DST_DISCARD;
    }

    if (!L3_ALPM_INT_ACTION_ENABLED(unit)) {
        if (fv[BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI]) {
            fib->flags |= BCM_L3_RPE;
            fib->int_pri = (int)fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI];
        } else {
            class_id = (int)fv[BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID];
            fib->class_id = class_id;
            class_id = (int)fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI];
            class_id = (class_id & 0xf) << 6;
            fib->class_id |= class_id;
        }
    } else {
        if (!fv[BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI]) {
            fib->int_act_profile_id =
                (uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID];
            fib->int_act_profile_id <<= 1;
            fib->int_act_profile_id |=
                ((uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] >> 3) & 0x1;
            fib->int_opaque_data_profile_id =
                (uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID];
            fib->int_opaque_data_profile_id |=
                ((uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] & 0x7) << 6;
            fib->flags3 |= BCM_L3_FLAGS3_INT_ACTION;
        }
    }

    if (fv[BCMINT_XGS_L3_FIB_TBL_UC_HIT]) {
        fib->flags |= BCM_L3_HIT;
    } else {
        fib->flags &= ~BCM_L3_HIT;
    }

    fib->flex_ctr_action_index = (uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID];
    fib->flex_ctr_object_index = (uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Construct unicast alpm entry data fields from fib info.
 *
 * \param [in] unit Unit number
 * \param [in] fib L3 forwarding info.
 * \param [out] cfg Unicast alpm entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_to_uc_cfg(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti,
                 bcmi_ltsw_l3_fib_t *fib, l3_uc_cfg_t *cfg)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    int nh_ecmp_idx;
    uint64_t *fv = cfg->flds;

    SHR_FUNC_ENTER(unit);

    fv[BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = fib->class_id;

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, fib->ul_intf, &nh_ecmp_idx,
                                            &type));
    if ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ||
        (type == BCMI_LTSW_L3_EGR_OBJ_T_OL)) {
        fv[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = nh_ecmp_idx;
        fv[BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = 0;
        cfg->fld_bmp &= ~BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID;
    } else if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) ||
               (type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL)){
        fv[BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = nh_ecmp_idx;
        fv[BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = 1;
        cfg->fld_bmp &= ~BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    if (!L3_ALPM_INT_ACTION_ENABLED(unit)) {
        if (fib->flags3 & BCM_L3_FLAGS3_INT_ACTION) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (fib->flags & BCM_L3_RPE) {
            fv[BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = 1;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = fib->int_pri;
        } else {
            fv[BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = 0;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = fib->class_id & 0x3f;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = (fib->class_id >> 6) & 0xf;
        }

    } else {
        if (fib->flags & BCM_L3_RPE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (fib->flags3 & BCM_L3_FLAGS3_INT_ACTION) {
            fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] =
                fib->int_act_profile_id >> 1;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] =
                (fib->int_act_profile_id & 0x1) << 3;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] =
                fib->int_opaque_data_profile_id & 0x3f;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = 0;
            fv[BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] |=
                (fib->int_opaque_data_profile_id >> 6) & 0x7;
        } else {
            fv[BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = 1;
        }
    }

    if (fib->flags & BCM_L3_DST_DISCARD) {
        fv[BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = 1;
    }

    if (fib->flags & BCM_L3_HIT) {
        fv[BCMINT_XGS_L3_FIB_TBL_UC_HIT] = 1;
    } else if (fib->flags & BCM_L3_HIT_CLEAR) {
        fv[BCMINT_XGS_L3_FIB_TBL_UC_HIT] = 0;
    }

exit:
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
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_uc_alpm_op(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, bcmlt_opcode_t op,
                  bool lpm, l3_cfg_t *cfg)
{
    int dunit, rv, ipt;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    l3_cfg_t pre_cfg;
    uint16_t vrf_id;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (op == BCMLT_OPCODE_LOOKUP) {
        /* Get the previous route entry info. */
        sal_memcpy(&pre_cfg, cfg, sizeof(l3_cfg_t));
        pre_cfg.cfg.uc.fld_bmp = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_eh_fill(unit, eh, ti, &pre_cfg));

        if (lpm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_LPM_LOOKUP));
        }

        if (L3_ALPM_HIT_ENABLED(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_GET_FROM_HW));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (lt_eh_parse(unit, eh, ti, cfg));

        SHR_EXIT();
    }

    /*
     * In ALPM combined mode, the user must insert a real default VRF route as
     * the first route within any VRF in order to prevent an unexpected lookup
     * miss.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_alpm_def_check(unit, ti->attr, cfg->cfg.uc.flds, op));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    rv = bcmlt_entry_commit(eh, op, BCMLT_PRIORITY_NORMAL);
    if ((rv == SHR_E_NOT_FOUND) && (op == BCMLT_OPCODE_UPDATE)) {
        op = BCMLT_OPCODE_INSERT;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh, op, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Update the ALPM route counter. */
    ipt = (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) ? IP_TYPE_V6 : IP_TYPE_V4;
    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF) {
        vrf_id = (uint16_t)cfg->cfg.uc.flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF];
    } else if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
        vrf_id = L3_FIB_VRF_GLO(unit);
    } else {
        vrf_id = L3_FIB_VRF_GHI(unit);
    }

    if (op == BCMLT_OPCODE_INSERT) {
        VRF_ALPM_ROUTE_INC(unit, vrf_id, ipt);
    } else if (op == BCMLT_OPCODE_DELETE) {
        VRF_ALPM_ROUTE_DEC(unit, vrf_id, ipt);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback to parse the entry during delete_all operation.
 *
 * The callback is used to fetch the mtu_profile_ptr from entry and
 * decrease the reference of this mtu ptr.
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
l3_mc_entry_hdl_to_cfg_del(int unit, bcmlt_entry_handle_t eh,
                           const bcmint_ltsw_l3_fib_tbl_t *ti, l3_cfg_t *l3_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (lt_eh_parse(unit, eh, ti, l3_cfg));
exit:
    SHR_FUNC_EXIT();
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
    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0] = fib->ip_addr;
    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_VRF] = fib->vrf;
    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR] = fib->sip_addr;
    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK] = fib->sip_addr_mask;
    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = fib->l3_iif;
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
static void
l3_fib_to_mc_cfg_key_route_v6(int unit, bcmi_ltsw_l3_fib_t *fib,
                              l3_mc_cfg_t *cfg)
{
    bcmi_ltsw_util_ip6_to_uint64
        (&cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0], &fib->ip6_addr);

    bcmi_ltsw_util_ip6_to_uint64
        (&cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR], &fib->sip6_addr);

    bcmi_ltsw_util_ip6_to_uint64
        (&cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK], &fib->sip6_addr_mask);

    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_VRF] = fib->vrf;
    cfg->flds[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = fib->l3_iif;
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
    uint64_t *fv = cfg->flds;
    SHR_FUNC_ENTER(unit);

    fib->flags |= BCM_L3_IPMC;

    if (ti->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) {
        fib->flags |= BCM_L3_IP6;
        bcmi_ltsw_util_uint64_to_ip6(&fib->ip6_addr,
                                     &fv[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0]);
        if (ti->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR)) {
            bcmi_ltsw_util_uint64_to_ip6(&fib->sip6_addr,
                                         &fv[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR]);
        }
        if (ti->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK)) {
            bcmi_ltsw_util_uint64_to_ip6(&fib->sip6_addr_mask,
                                         &fv[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK]);
        }
    } else {
        fib->ip_addr = fv[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0];
        if (ti->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR)) {
            fib->sip_addr = fv[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR];
        }
        if (ti->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK)) {
            fib->sip_addr_mask = fv[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK];
        }
    }

    fib->vrf = (bcm_vrf_t)fv[BCMINT_XGS_L3_FIB_TBL_MC_VRF];
    fib->l3_iif = (bcm_if_t)fv[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID];
    if (fv[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_l3_grp_id_get(unit,
                                               fv[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID],
                                               &fib->mc_group));
    }
    fib->expected_l3_iif = fv[BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID] ?
                           (bcm_if_t)fv[BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID] :
                           BCM_IF_INVALID;
    fib->int_flags |= fv[BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID] ?
                      BCMI_LTSW_L3_FIB_F_IPMC_POST_LOOKUP_RPF_CHECK : 0;

    fib->flags |= fv[BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH] ?
                  BCM_L3_DST_DISCARD : 0;

    fib->int_flags |= fv[BCMINT_XGS_L3_FIB_TBL_MC_DROP] ?
                      BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_DROP : 0;
    fib->int_flags |= fv[BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU] ?
                      BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU : 0;

    fib->int_flags |= fv[BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID] ?
                      0 : BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK;

    fib->flags |= fv[BCMINT_XGS_L3_FIB_TBL_MC_HIT] ? BCM_L3_HIT : 0;

    fib->is_trunk = (bool)fv[BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK];
    fib->port_tgid = fv[BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK] ?
                     (int)fv[BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID] :
                     (int)fv[BCMINT_XGS_L3_FIB_TBL_MC_PORT];

    if (fv[BCMINT_XGS_L3_FIB_TBL_MC_HIT]) {
        fib->flags |= BCM_L3_HIT;
    } else {
        fib->flags &= ~BCM_L3_HIT;
    }

    fib->class_id = (int)fv[BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID];
    fib->flex_ctr_action_index = (uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID];
    fib->flex_ctr_object_index = (uint32_t)fv[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT];

    SHR_EXIT();

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
    bool rpf_check_enable;
    bool no_src_port_check;

    SHR_FUNC_ENTER(unit);
    no_src_port_check = (fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK);
    rpf_check_enable = (fib->expected_l3_iif != BCM_IF_INVALID) &&
                       (fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_POST_LOOKUP_RPF_CHECK);
    fv[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID] = _BCM_MULTICAST_ID_GET(fib->mc_group);
    fv[BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID] = fib->class_id;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU] =
        ((fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU) && rpf_check_enable) ? 1 : 0;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_DROP] =
        ((fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_DROP) && rpf_check_enable) ? 1 : 0;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID] =
        rpf_check_enable ? fib->expected_l3_iif : 0;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH] =
        (fib->flags & BCM_L3_DST_DISCARD) ? 1 : 0;

    fv[BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID] = !no_src_port_check;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK] = fib->is_trunk;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID] = fib->is_trunk ? fib->port_tgid : 0;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_PORT] = fib->is_trunk ? 0: fib->port_tgid;

    if (fib->flags & BCM_L3_HIT) {
        fv[BCMINT_XGS_L3_FIB_TBL_MC_HIT] = 1;
    } else if (fib->flags & BCM_L3_HIT_CLEAR) {
        fv[BCMINT_XGS_L3_FIB_TBL_MC_HIT] = 0;
    }

    fv[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID] = fib->flex_ctr_action_index;
    fv[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT] = fib->flex_ctr_object_index;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate L3 multicast tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ti Logical table info.
 * \param [in] op Logical table operation code.
  * \param [in] lpm LPM lookup.
 * \param [in] cfg Multicast route entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_mc_op(int unit, const bcmint_ltsw_l3_fib_tbl_t *ti, bcmlt_opcode_t op,
             bool lpm, l3_cfg_t *cfg)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t fld_bmp = cfg->cfg.mc.fld_bmp;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (op == BCMLT_OPCODE_LOOKUP)  {
        /* Not fill in data fields for LOOKUP operation. */
        cfg->cfg.mc.fld_bmp = 0;
    }

    if ((op == BCMLT_OPCODE_LOOKUP) && lpm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_LPM_LOOKUP));
    }

    if ((op == BCMLT_OPCODE_LOOKUP) &&
        L3_ALPM_HIT_ENABLED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_GET_FROM_HW));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_eh_fill(unit, eh, ti, cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    if (op == BCMLT_OPCODE_LOOKUP) {
        /* Set bitmap of data fields to be parsed. */
        cfg->cfg.mc.fld_bmp = fld_bmp;
        SHR_IF_ERR_VERBOSE_EXIT
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
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_uc_cfg_to_fib(unit, ti, &cfg->cfg.uc, fib));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_mc_cfg_to_fib(unit, ti, &cfg->cfg.mc, fib));
    }

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
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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
    info->l3a_intf = fib->ul_intf;

exit:
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
    bcm_l3_route_t route;
    bcm_ipmc_addr_t ipmc;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmi_ltsw_l3_fib_t fib;
    l3_cfg_t cfg;
    l3_filter_cmd_t flt_cmd;
    uint32_t cnt = 0;
    int rv = SHR_E_NONE;
    uint64_t hit_bit = 0;
    int dunit, ipt;
    uint16_t vrf_id = 0;
    uint8_t *def_route = NULL;
    uint64_t ip_addr_mask0 = 0;
    uint64_t ip_addr_mask1 = 0;
    uint32_t *ip_addr_private_cnt = NULL;
    uint16_t sz = 0;
    int alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);

    SHR_FUNC_ENTER(unit);

    if (!trav_info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (trav_info->op == L3_TRAV_OP_AGE) {
        sz = (L3_FIB_VRF_MAX(unit) + 1) *sizeof(uint32_t);
        SHR_ALLOC(ip_addr_private_cnt, sz, "bcmXgsL3VrfIpCnt");
        SHR_NULL_CHECK(ip_addr_private_cnt, SHR_E_MEMORY);
        sal_memset(ip_addr_private_cnt, 0, sz);

        sz = L3_FIB_VRF_MAX(unit) + 1;
        SHR_ALLOC(def_route, sz, "bcmXgsL3DefRoute");
        SHR_NULL_CHECK(def_route, SHR_E_MEMORY);
        sal_memset(def_route, 0, sz);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        cnt++;

        sal_memset(&cfg, 0, sizeof(l3_cfg_t));
        if (trav_info->hdl_to_cfg_cb) {
            SHR_IF_ERR_VERBOSE_EXIT
                (trav_info->hdl_to_cfg_cb(unit, eh, tbl_info, &cfg));
        }

        if (trav_info->filter) {
            SHR_IF_ERR_VERBOSE_EXIT
                (trav_info->filter(unit, cnt - 1, &trav_info->flt_crtr,
                                   &cfg, &flt_cmd));

            if (flt_cmd == BCMINT_LTSW_L3_FLT_STOP) {
                break;
            } else if (flt_cmd == BCMINT_LTSW_L3_FLT_SKIP) {
                continue;
            } else if (flt_cmd == BCMINT_LTSW_L3_FLT_SKIP_NO_CNT) {
                cnt--;
                continue;
            }
        }

        sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
        if (trav_info->cfg_to_fib_cb) {
            SHR_IF_ERR_VERBOSE_EXIT
                (trav_info->cfg_to_fib_cb(unit, tbl_info, &cfg, &fib));
        }

        if (trav_info->route_trav_cb) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_route_entry_fill(unit, &fib, &route));
            rv = trav_info->route_trav_cb(unit, cnt, &route,
                                          trav_info->user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }

        if (trav_info->ipmc_trav_cb && trav_info->ipmc_addr_t_cb) {
            bcm_ipmc_addr_t_init(&ipmc);
            SHR_IF_ERR_VERBOSE_EXIT
                (trav_info->ipmc_addr_t_cb(unit, &fib, &ipmc));
            rv = trav_info->ipmc_trav_cb(unit, &ipmc, trav_info->user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }

        if (trav_info->op == L3_TRAV_OP_DEL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_commit(eh,
                                    BCMLT_OPCODE_DELETE,
                                    BCMLT_PRIORITY_NORMAL));
            if (!(tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC) &&
                tbl_info->rm_type == BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM) {
                ipt = (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_V6) ?
                      IP_TYPE_V6 : IP_TYPE_V4;
                if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF) {
                    vrf_id = (uint16_t)cfg.cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF];
                } else if (tbl_info->attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_GL) {
                    vrf_id = L3_FIB_VRF_GLO(unit);
                } else {
                    vrf_id = L3_FIB_VRF_GHI(unit);
                }
                VRF_ALPM_ROUTE_DEC(unit, vrf_id, ipt);
            }
        } else if (trav_info->op == L3_TRAV_OP_AGE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_GET_FROM_HW));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_commit(eh,
                                    BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));
            hit_bit = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, HITs, &hit_bit));

            ip_addr_mask0 = 0;
            ip_addr_mask1 = 0;
            (void)bcmlt_entry_field_get(eh,
                                        tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK].fld_name,
                                        &ip_addr_mask0);
            (void)bcmlt_entry_field_get(eh,
                                        tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK].fld_name,
                                        &ip_addr_mask1);
            vrf_id = (uint16_t)cfg.cfg.uc.flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF];
            if (!ip_addr_mask0 && !ip_addr_mask1) {
                def_route[vrf_id] = 1;
            }

            if (hit_bit) {
                if (ip_addr_mask0 || ip_addr_mask1) {
                    ip_addr_private_cnt[vrf_id]++;
                }
                (void)bcmlt_entry_field_remove(eh,
                                               MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, HITs, 0));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_commit(eh,
                                        BCMLT_OPCODE_UPDATE,
                                        BCMLT_PRIORITY_NORMAL));
            } else {
                if ((ip_addr_mask0 || ip_addr_mask1) ||
                    (!ip_addr_mask0 && !ip_addr_mask1 && (alpm_temp % 2 == 0))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_commit(eh,
                                            BCMLT_OPCODE_DELETE,
                                            BCMLT_PRIORITY_NORMAL));
                }
            }
        } else if (trav_info->op == L3_TRAV_OP_CLR_HIT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_attrib_set(eh, BCMLT_ENT_ATTR_GET_FROM_HW));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_commit(eh,
                                    BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));
            hit_bit = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, HITs, &hit_bit));

            if (hit_bit) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, HITs, 0));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_commit(eh,
                                    BCMLT_OPCODE_UPDATE,
                                    BCMLT_PRIORITY_NORMAL));
        }
    }

    /* Handle the default routes aging. */
    if ((trav_info->op == L3_TRAV_OP_AGE) && (alpm_temp % 2 != 0)) {
        uint64_t value = 0;
        for (vrf_id = 0; vrf_id <= L3_FIB_VRF_MAX(unit); vrf_id++) {
            if (!ip_addr_private_cnt[vrf_id] && def_route[vrf_id]) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add_by_id(eh,
                                                 tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0].fld_id,
                                                 value));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add_by_id(eh,
                                                 tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK].fld_id,
                                                 value));
                if (tbl_id == BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add_by_id(eh,
                                                     tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1].fld_id,
                                                     value));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add_by_id(eh,
                                                     tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK].fld_id,
                                                     value));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add_by_id(eh,
                                                 tbl_info->flds[BCMINT_LTSW_L3_FIB_TBL_UC_VRF].fld_id,
                                                 vrf_id));
                SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_commit(eh,
                                            BCMLT_OPCODE_DELETE,
                                            BCMLT_PRIORITY_NORMAL));
            }
        }
    }
    if ((rv != SHR_E_NONE) && (rv != SHR_E_NOT_FOUND) &&
        (rv != SHR_E_UNAVAIL)) {
        SHR_ERR_EXIT(rv);
    } else {
         SHR_EXIT();
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    if (ip_addr_private_cnt) {
        SHR_FREE(ip_addr_private_cnt);
    }
    if (def_route) {
        SHR_FREE(def_route);
    }
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
    bcmi_lt_entry_t lt_ent;
    int i, sz;
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
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, L3_ALPM_CONTROLs, &lt_ent, NULL, NULL));

    if (sal_strcasecmp(flds[0].u.sym_val, VALIDs)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    if ((alpm_temp == 1) || (alpm_temp == 3) || (alpm_temp == 5) ||
        (alpm_temp == 7) || (alpm_temp == 9) || (alpm_temp == 11)) {
        fi->alpm_mode = ALPM_MODE_COMBINED;
    } else if ((alpm_temp == 2) || (alpm_temp == 4) || (alpm_temp == 6) ||
                (alpm_temp == 10)) {
        fi->alpm_mode = ALPM_MODE_PARALLEL;
    } else {
        fi->alpm_mode = ALPM_MODE_NONE;
    }

    if (alpm_temp >= 5) {
        fi->src_lookup_enabled = true;
    } else {
        fi->src_lookup_enabled = false;
    }

    sz = sizeof(uint32_t) * L3_FIB_VRF_CNT(unit);
    for (i = 0; i < IP_TYPE_CNT; i++) {
        SHR_ALLOC(fi->alpm_route_cnt[i], sz, "bcmLtswAlpmRouteCnt");
        sal_memset(fi->alpm_route_cnt[i], 0, sz);
    }

    fi->alpm_int_action_enabled =
        bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ROUTE_INT_ACTION_VALID, 0) ?
        true : false;

    fi->alpm_hit_enabled =
        bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_HIT_MODE, 0) ?
        true : false;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3_ALPM_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_control_set(int unit, int alpm_temp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_alpm_control_set(unit, alpm_temp, bcmiUftModeInvalid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set L3 Route table control info.
 *
 * \param [in] unit Unit number.
 * \param [in] alpm_temp ALPM template.
 *
 * Set the TABLE_CONTROL.MAX_ENTRIES with TABLE_INFO.ENTRY_LIMIT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_table_control_set(int unit, int alpm_temp)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int i, num, thres, dunit;
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
        { NULL, L3_IPV6_COMP_SRCs, false },
        { NULL, L3_IPV4_MC_ROUTEs, true },
        { NULL, L3_IPV6_MC_ROUTEs, true },
    };

    SHR_FUNC_ENTER(unit);

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

        if (alpm_lt[i].thres_en) {
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
    int alpm_temp;

    SHR_FUNC_ENTER(unit);

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_alpm_control_set(unit, alpm_temp));

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_alpm_table_control_set(unit, alpm_temp));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_alpm_info_init(unit, alpm_temp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a l3 fib flex action profile handle.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [out] profile_hdl Profile handle.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_fib_flex_action_profile_handle_get(
    int unit,
    l3_fib_flexctr_action_t type,
    bcmi_ltsw_profile_hdl_t *profile_hdl)
{
    SHR_FUNC_ENTER(unit);
    switch (type) {
        case BCMINT_XGS_V4MC_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_V4MC_FLEX_ACTION;
            break;
        case BCMINT_XGS_V4SRC_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_V4SRC_FLEX_ACTION;
            break;
        case BCMINT_XGS_V4UC_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_V4UC_FLEX_ACTION;
            break;
        case BCMINT_XGS_V6MC_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_V6MC_FLEX_ACTION;
            break;
        case BCMINT_XGS_V6SRC_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_V6SRC_FLEX_ACTION;
            break;
        case BCMINT_XGS_V6UC_FLEXCTR_ACTION:
            *profile_hdl = BCMI_LTSW_PROFILE_V6UC_FLEX_ACTION;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Retrieve a l3 fib flex action profile table name.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [out] ltname Logical table name.
 * \param [out] keyname Logical table key.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_fib_flex_action_profile_table_name_get(
    int unit,
    l3_fib_flexctr_action_t type,
    const char **ltname,
    const char **keyname)
{
    SHR_FUNC_ENTER(unit);
    switch (type) {
        case BCMINT_XGS_V4MC_FLEXCTR_ACTION:
            *ltname = L3_IPV4_MC_CTR_ING_EFLEX_ACTIONs;
            *keyname = L3_IPV4_MC_CTR_ING_EFLEX_ACTION_IDs;
            break;
        case BCMINT_XGS_V4SRC_FLEXCTR_ACTION:
            *ltname = L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTIONs;
            *keyname = L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDs;
            break;
        case BCMINT_XGS_V4UC_FLEXCTR_ACTION:
            *ltname = L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTIONs;
            *keyname = L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs;
            break;
        case BCMINT_XGS_V6MC_FLEXCTR_ACTION:
            *ltname = L3_IPV6_MC_CTR_ING_EFLEX_ACTIONs;
            *keyname = L3_IPV6_MC_CTR_ING_EFLEX_ACTION_IDs;
            break;
        case BCMINT_XGS_V6SRC_FLEXCTR_ACTION:
            *ltname = L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTIONs;
            *keyname = L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDs;
            break;
        case BCMINT_XGS_V6UC_FLEXCTR_ACTION:
            *ltname = L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTIONs;
            *keyname = L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Retrieve a l3 fib flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] profile_idx Profile Index.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_fib_flex_action_profile_entry_get(
    int unit,
    l3_fib_flexctr_action_t type,
    int profile_idx,
    l3_fib_flex_action_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *ltname = NULL;
    const char *keyname = NULL;
    int dunit;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_table_name_get(unit,
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
 * \brief Set a l3 fib flex action profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] profile_idx Profile Index.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_fib_flex_action_profile_entry_set(
    int unit,
    l3_fib_flexctr_action_t type,
    int profile_idx,
    l3_fib_flex_action_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *ltname = NULL;
    const char *keyname = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_table_name_get(unit,
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
 * \brief Delete a l3 fib flex action profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] profile_idx Profile Index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_fib_flex_action_profile_entry_del(
    int unit,
    l3_fib_flexctr_action_t type,
    int profile_idx)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    const char *ltname = NULL;
    const char *keyname = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_table_name_get(unit,
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
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add l3 fib flex action profile.
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
l3_fib_flex_action_profile_add(
    int unit,
    l3_fib_flexctr_action_t type,
    l3_fib_flex_action_profile_t *entry,
    int *index)
{
    int rv;
    int profile_idx;
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_handle_get(unit, type, &profile_hdl));

    rv = bcmi_ltsw_profile_index_allocate(unit, profile_hdl,
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
        (l3_fib_flex_action_profile_entry_set(unit,
                                              type,
                                              profile_idx,
                                              entry));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get entries from LT l3 fib flex action.
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
l3_fib_flex_action_profile_get(
    int unit,
    l3_fib_flexctr_action_t type,
    int index,
    l3_fib_flex_action_profile_t *prof_entry)
{
    int profile_idx;

    SHR_FUNC_ENTER(unit);
    profile_idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_entry_get(unit, type,
                                              profile_idx,
                                              prof_entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete entries from LT l3 fib flex action.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 * \param [in] index Base Index of LT l3 flex action.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_flex_action_profile_delete(
    int unit,
    l3_fib_flexctr_action_t type,
    int index)
{
    int rv;
    int profile_idx;
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    profile_idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_handle_get(unit, type, &profile_hdl));

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
        (l3_fib_flex_action_profile_entry_del(unit,
                                              type,
                                              profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash callback function for l3 fib flex action profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries L3  fib flex action profile entries.
 * \param [in] entries_per_set      Number of entries in the profile set.
 * \param [out] hash Hash value returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_fib_flex_action_profile_entry_hash_cb(
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
                      sizeof(l3_fib_flex_action_profile_t));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Comparison callback function for l3 v4mc flex action profile.
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
l3_v4mc_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_fib_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_get(unit,
                                        BCMINT_XGS_V4MC_FLEXCTR_ACTION,
                                        profile_idx,
                                        &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 v4src flex action profile.
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
l3_v4src_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_fib_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_get(unit,
                                        BCMINT_XGS_V4SRC_FLEXCTR_ACTION,
                                        profile_idx,
                                        &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 v4uc flex action profile.
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
l3_v4uc_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_fib_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_get(unit,
                                        BCMINT_XGS_V4UC_FLEXCTR_ACTION,
                                        profile_idx,
                                        &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 v6mc flex action profile.
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
l3_v6mc_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_fib_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_get(unit,
                                        BCMINT_XGS_V6MC_FLEXCTR_ACTION,
                                        profile_idx,
                                        &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 v6srcflex action profile.
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
l3_v6src_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_fib_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_get(unit,
                                        BCMINT_XGS_V6SRC_FLEXCTR_ACTION,
                                        profile_idx,
                                        &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for l3 v6uc flex action profile.
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
l3_v6uc_flex_action_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    l3_fib_flex_action_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_get(unit,
                                        BCMINT_XGS_V6UC_FLEXCTR_ACTION,
                                        profile_idx,
                                        &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover l3 fib flex action.
 *
 * \param [in] unit Unit number.
 * \param [in] type L3 flex action type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_flex_action_profile_recover(int unit, l3_fib_flexctr_action_t type)
{
    uint32_t ref_cnt;
    uint32_t index;
    uint64_t idx_min, idx_max;
    int dunit;
    l3_fib_flex_action_profile_t profile_entry = {0};
    bcmi_ltsw_profile_hdl_t profile_hdl;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int rv;
    uint64_t id;
    const char *ltname = NULL;
    const char *keyname = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_handle_get(unit,
                                               type,
                                               &profile_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_table_name_get(unit,
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
                (l3_fib_flex_action_profile_get(unit,
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
 * \brief Clear the information of L3 fib flex action profile.
 *
 * This function is used to clear the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_flex_action_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IPV4_MC_CTR_ING_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IPV6_MC_CTR_ING_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTIONs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTIONs));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Deinit LT l3 fib flex action information.
 *
 * This function is used to deinit LT l3 flex action information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_fib_flex_action_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_clear(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate flex counter related fields of L3 route entry.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 route information.
 * \param [in/out] flexctr Pointer to flex counter config info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Failure.
 */
static int
l3_fib_uc_flexctr_op(int unit, bcmi_ltsw_l3_fib_t *fib, l3_fib_flexctr_cfg_t *flexctr)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    bcmlt_opcode_t op;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    int index;
    l3_fib_flex_action_profile_t action_profile;
    l3_fib_flexctr_action_t type;
    bool src_route = false;

    SHR_FUNC_ENTER(unit);

     sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    cfg.uc = true;

    if (fib->flags & BCM_L3_IP6) {
        /* Key. */
        bcmi_ltsw_util_ip6_to_uint64
            (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0],
            &fib->ip6_addr);
        bcmi_ltsw_util_ip6_to_uint64
            (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
            &fib->ip6_addr_mask);

        if (src_route) {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
            }
        } else {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
            }
        }

        type = src_route ? BCMINT_XGS_V6SRC_FLEXCTR_ACTION :
                           BCMINT_XGS_V6UC_FLEXCTR_ACTION;
    } else {
        /* Key. */
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
        if (src_route) {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
            }
        } else {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
            }
        }

        type = src_route ? BCMINT_XGS_V4SRC_FLEXCTR_ACTION :
                           BCMINT_XGS_V4UC_FLEXCTR_ACTION;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    if (tbl_info->fld_bmp & ((1LL) << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    if (flexctr->op == L3_FIB_FLXCTR_OP_SET) {
        if (flexctr->flags & L3_FIB_FLXCTR_F_ACTION) {
            uc->fld_bmp = 1LL << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID;
            action_profile.action = flexctr->action;
            if (flexctr->action) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_fib_flex_action_profile_add(unit,
                                                    type,
                                                    &action_profile,
                                                    &index));
                uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = index;
            } else {
                /* Get the old action and delete it. */
                op = BCMLT_OPCODE_LOOKUP;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (l3_fib_uc_alpm_op(unit, tbl_info, op, false, &cfg));
                index = uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID];
                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_fib_flex_action_profile_delete(unit, type, index));
                uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = 0;
            }
        }

        if (flexctr->flags & L3_FIB_FLXCTR_F_INDEX) {
            uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = flexctr->index;
            uc->fld_bmp = 1LL << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT;
        }

        op = BCMLT_OPCODE_UPDATE;
    } else if (flexctr->op == L3_FIB_FLXCTR_OP_GET) {
        uc->fld_bmp = (1LL << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                      (1LL << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID);
        op = BCMLT_OPCODE_LOOKUP;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, op, false, &cfg));

    if (flexctr->op == L3_FIB_FLXCTR_OP_GET) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

        if (flexctr->flags & L3_FIB_FLXCTR_F_ACTION) {
            index = fib->flex_ctr_action_index;
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_fib_flex_action_profile_get(unit, index, type, &action_profile));
            flexctr->action = action_profile.action;
        }

        if (flexctr->flags & L3_FIB_FLXCTR_F_INDEX) {
            flexctr->index = fib->flex_ctr_object_index;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Operate flex counter related fields of IPMC route entry.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 route information.
 * \param [in/out] flexctr Pointer to flex counter config info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Failure.
 */
static int
l3_fib_mc_flexctr_op(int unit, bcmi_ltsw_l3_fib_t *fib, l3_fib_flexctr_cfg_t *flexctr)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    bcmlt_opcode_t op;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    int index;
    l3_fib_flex_action_profile_t action_profile;
    l3_fib_flexctr_action_t type;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(l3_mc_cfg_t));

    if (fib->flags & BCM_L3_IP6) {
        /* Key. */
        bcmi_ltsw_util_ip6_to_uint64
            (&mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0],
            &fib->ip6_addr);
        if (sal_memcmp(fib->sip6_addr, ip6_zero, BCM_IP6_ADDRLEN) == 0) {
            sal_memcpy(&mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR], fib->sip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(&mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK], ip6_full, BCM_IP6_ADDRLEN);
        }
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_VRF] = fib->vrf;
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = fib->l3_iif;
        type = BCMINT_XGS_V6MC_FLEXCTR_ACTION;
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
    } else {
        /* Key. */
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0] = fib->ip_addr;
        if (fib->sip_addr) {
            mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK] = fib->sip_addr_mask;
            mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR] = fib->sip_addr;
        }
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_VRF] = fib->vrf;
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = fib->l3_iif;
        type = BCMINT_XGS_V4MC_FLEXCTR_ACTION;
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    if (flexctr->op == L3_FIB_FLXCTR_OP_SET) {
        if (flexctr->flags & L3_FIB_FLXCTR_F_ACTION) {
            action_profile.action = flexctr->action;
            mc->fld_bmp = 1LL << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID;
            if (flexctr->action) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_fib_flex_action_profile_add(unit,
                                                    type,
                                                    &action_profile,
                                                    &index));
                mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID] = index;
            } else {
                /* Get the old action and delete it. */
                op = BCMLT_OPCODE_LOOKUP;
                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_fib_mc_op(unit, tbl_info, op, false, &cfg));
                index = mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID];
                SHR_IF_ERR_VERBOSE_EXIT
                    (l3_fib_flex_action_profile_delete(unit, type, index));
                mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID] = 0;
            }
        }

        if (flexctr->flags & L3_FIB_FLXCTR_F_INDEX) {
            mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT] = flexctr->index;
            mc->fld_bmp = 1LL << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT;
        }

        op = BCMLT_OPCODE_UPDATE;
    } else if (flexctr->op == L3_FIB_FLXCTR_OP_GET) {
        mc->fld_bmp = (1LL << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT) |
                      (1LL << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID);
        op = BCMLT_OPCODE_LOOKUP;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, op, false, &cfg));

    if (flexctr->op == L3_FIB_FLXCTR_OP_GET) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

        if (flexctr->flags & L3_FIB_FLXCTR_F_ACTION) {
            index = fib->flex_ctr_action_index;
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_fib_flex_action_profile_get(unit, index, type, &action_profile));
            flexctr->action = fib->flex_ctr_action_index;
        }

        if (flexctr->flags & L3_FIB_FLXCTR_F_INDEX) {
            flexctr->index = fib->flex_ctr_object_index;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
xgs_ltsw_l3_fib_init(int unit)
{
    int i;
    int ent_idx_min, ent_idx_max;
    uint64_t idx_min, idx_max;
    bcmi_ltsw_profile_hdl_t profile_hdl;
    uint64_t max_vrf = 0;
    l3_fib_info_t *fi = &l3_fib_info[unit];

    SHR_FUNC_ENTER(unit);

    if (fi->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_fib_deinit(unit));
    }

    sal_memset(fi, 0, sizeof(l3_fib_info_t));

    fi->mutex = sal_mutex_create("xgsltswl3FibInfo");
    SHR_NULL_CHECK(fi->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IIFs, VRF_IDs,
                                       NULL, &max_vrf));
    fi->max_vrf_id = (uint16_t)max_vrf;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_alpm_init(unit));

    /* V4MC FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IPV4_MC_CTR_ING_EFLEX_ACTIONs,
                                       L3_IPV4_MC_CTR_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_V4MC_FLEX_ACTION;
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
                                    l3_fib_flex_action_profile_entry_hash_cb,
                                    l3_v4mc_flex_action_profile_entry_cmp_cb));
    /***************************************************************************/

    /* V4SRC FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTIONs,
                                       L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_V4SRC_FLEX_ACTION;
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
                                    l3_fib_flex_action_profile_entry_hash_cb,
                                    l3_v4src_flex_action_profile_entry_cmp_cb));
    /***************************************************************************/

    /* V4UC FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTIONs,
                                       L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_V4UC_FLEX_ACTION;
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
                                    l3_fib_flex_action_profile_entry_hash_cb,
                                    l3_v4uc_flex_action_profile_entry_cmp_cb));
    /***************************************************************************/

    /* V6MC FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IPV6_MC_CTR_ING_EFLEX_ACTIONs,
                                       L3_IPV6_MC_CTR_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_V6MC_FLEX_ACTION;
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
                                    l3_fib_flex_action_profile_entry_hash_cb,
                                    l3_v6mc_flex_action_profile_entry_cmp_cb));
    /***************************************************************************/

    /* V6SRC FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTIONs,
                                       L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_V6SRC_FLEX_ACTION;
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
                                    l3_fib_flex_action_profile_entry_hash_cb,
                                    l3_v6src_flex_action_profile_entry_cmp_cb));

    /***************************************************************************/

    /* V6UC FLEX_ACTION profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTIONs,
                                       L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    profile_hdl = BCMI_LTSW_PROFILE_V6UC_FLEX_ACTION;
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
                                    l3_fib_flex_action_profile_entry_hash_cb,
                                    l3_v6uc_flex_action_profile_entry_cmp_cb));

    if (bcmi_warmboot_get(unit)) {
        for (i = 0; i < BCMINT_XGS_L3_FIB_FLEXCTR_ACTION_CNT; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_fib_flex_action_profile_recover(unit, i));
        }
    }

    fi->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        profile_hdl = BCMI_LTSW_PROFILE_V4MC_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_V4SRC_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_V4UC_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_V6MC_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_V6SRC_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        profile_hdl = BCMI_LTSW_PROFILE_V6UC_FLEX_ACTION;
        (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

        if (fi->mutex) {
            sal_mutex_destroy(fi->mutex);
        }
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_fib_deinit(int unit)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];
    bcmi_ltsw_profile_hdl_t profile_hdl;

    SHR_FUNC_ENTER(unit);

    if (!fi->inited || !fi->mutex) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(fi->mutex, SAL_MUTEX_FOREVER);

    fi->inited = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_flex_action_profile_deinit(unit));

    profile_hdl = BCMI_LTSW_PROFILE_V4MC_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_V4SRC_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_V4UC_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_V6MC_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_V6SRC_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    profile_hdl = BCMI_LTSW_PROFILE_V6UC_FLEX_ACTION;
    (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);

    sal_mutex_give(fi->mutex);

    SHR_IF_ERR_CONT
        (l3_fib_tables_clear(unit));

    sal_mutex_destroy(fi->mutex);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_fib_vrf_max_get(int unit, uint32_t *max_vrf)
{
    uint64_t min, max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, PORT_SYSTEMs, VRF_IDs,
                                       &min, &max));

    *max_vrf = max;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bcmlt_opcode_t op;
    bool src_route = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    op = (fib->flags & BCM_L3_REPLACE) ? BCMLT_OPCODE_UPDATE :
                                         BCMLT_OPCODE_INSERT;

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    if (src_route) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
        }
    } else {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
    if (tbl_info->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    L3_FIB_LOCK(unit);
    locked = true;

    /* Data fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_to_uc_cfg(unit, tbl_info, fib, uc));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, op, false, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bool src_route = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    if (src_route) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
        }
    } else {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    /* Key. */
    uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
    if (tbl_info->fld_bmp & ((1LL) << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_DELETE, false, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bool src_route = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    if (src_route) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
        }
    } else {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
        }
    }

    SHR_IF_ERR_EXIT
            (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = tbl_info->fld_bmp;

    /* Key. */
    uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
    uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
    if (tbl_info->fld_bmp & ((1LL) << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, lpm, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.filter = l3_trav_filter_range;
    trav_info.flt_crtr.start = start;
    trav_info.flt_crtr.end = end;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.route_trav_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_route_ipv4_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    int rv;
    bcmlt_opcode_t op;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    op = BCMLT_OPCODE_INSERT;

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->flags & BCM_L3_REPLACE) {
        rv = l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, false, &cfg);
        if (SHR_SUCCESS(rv)) {
            op = BCMLT_OPCODE_UPDATE;
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_to_mc_cfg(unit, tbl_info, fib, mc));

    mc->fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, op, false, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    L3_FIB_LOCK(unit);
    locked = true;

    mc->fld_bmp = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, false, &cfg));
exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v4(unit, fib, mc);

    mc->fld_bmp = tbl_info->fld_bmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, lpm, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv4_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = trav_fn;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC, &trav_info));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_route_ipv6_uc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bcmlt_opcode_t op;
    bool src_route = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    op = (fib->flags & BCM_L3_REPLACE) ? BCMLT_OPCODE_UPDATE :
                                         BCMLT_OPCODE_INSERT;

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    if (src_route) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
        }
    } else {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = tbl_info->fld_bmp;

    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
         &fib->ip6_addr_mask);
    if (tbl_info->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_UC_VRF))
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_to_uc_cfg(unit, tbl_info, fib, uc));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, op, false, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv6_uc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bool src_route = false;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    if (src_route) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
        }
    } else {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    /* Key. */
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
         &fib->ip6_addr_mask);
    if (tbl_info->fld_bmp & (1LL << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_DELETE, false, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv6_uc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    bool src_route = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    if (src_route) {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
        }
    } else {
        if (fib->vrf == BCM_L3_VRF_GLOBAL) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
        } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
        } else {
            tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    uc->fld_bmp = tbl_info->fld_bmp;

    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0], &fib->ip6_addr);
    bcmi_ltsw_util_ip6_to_uint64
        (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
         &fib->ip6_addr_mask);
    if (tbl_info->fld_bmp & (1 << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, lpm, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv6_uc_traverse(int unit, uint32_t flags,
                                   uint32_t start, uint32_t end,
                                   bcm_l3_route_traverse_cb cb,
                                   void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.filter = l3_trav_filter_range;
    trav_info.flt_crtr.start = start;
    trav_info.flt_crtr.end = end;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.route_trav_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_route_ipv6_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    int rv;
    bcmlt_opcode_t op;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    op = BCMLT_OPCODE_INSERT;

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->flags & BCM_L3_REPLACE) {
        rv = l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, false, &cfg);
        if (SHR_SUCCESS(rv)) {
            op = BCMLT_OPCODE_UPDATE;
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (l3_fib_to_mc_cfg(unit, tbl_info, fib, mc));

    mc->fld_bmp = tbl_info->fld_bmp;
    SHR_IF_ERR_EXIT
        (l3_fib_mc_op(unit, tbl_info, op, false, &cfg));


exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv6_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }
    sal_memset(&cfg, 0, sizeof(l3_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    L3_FIB_LOCK(unit);
    locked = true;

    mc->fld_bmp = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_DELETE, false, &cfg));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv6_mc_find(int unit, bool lpm, bcmi_ltsw_l3_fib_t *fib)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&cfg, 0, sizeof(l3_mc_cfg_t));

    /* Keys. */
    l3_fib_to_mc_cfg_key_route_v6(unit, fib, mc);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = tbl_info->fld_bmp;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, BCMLT_OPCODE_LOOKUP, lpm, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));


exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_ipv6_mc_traverse(int unit, uint32_t attrib,
                                  bcm_ipmc_traverse_cb trav_fn,
                                  bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                                  void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.cfg_to_fib_cb = l3_cfg_to_fib;
    trav_info.ipmc_trav_cb = trav_fn;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                     &trav_info));
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_del_by_intf(int unit, bcmi_ltsw_l3_fib_t *fib, int negate)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    l3_trav_t trav_info;
    bool ipv6 = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
    trav_info.filter = l3_uc_trav_filter_intf;
    trav_info.flt_crtr.intf = fib->ul_intf;
    trav_info.flt_crtr.negate = negate;

    if (fib->flags & BCM_L3_IP6) {
        ipv6 = true;
    }

    tbl_id = ipv6 ? BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH :
                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, tbl_id, &trav_info));

    tbl_id = ipv6 ? BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF :
                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, tbl_id, &trav_info));

    tbl_id = ipv6 ? BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL :
                    BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, tbl_id, &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_del_all(int unit, uint32_t attrib)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_DEL;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_UC) {
        trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL,
                             &trav_info));
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF,
                             &trav_info));

            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL,
                             &trav_info));
        }
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_MC) {
        trav_info.hdl_to_cfg_cb = l3_mc_entry_hdl_to_cfg_del;

        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                             &trav_info));
        }

        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                             &trav_info));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_age(int unit, bcm_l3_route_traverse_cb age_out,
                      void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_AGE;
    trav_info.route_trav_cb = age_out;
    trav_info.user_data = user_data;
    trav_info.hdl_to_cfg_cb = l3_eh_to_cfg;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                 bcmi_ltsw_flexctr_counter_info_t *info)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_SET;
    flexctr.flags = L3_FIB_FLXCTR_F_ACTION;
    flexctr.action = info->action_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_flexctr_op(unit, fib, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_SET;
    flexctr.flags = L3_FIB_FLXCTR_F_ACTION;
    flexctr.action = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_flexctr_op(unit, fib, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                   bcmi_ltsw_flexctr_counter_info_t *info)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    bcmlt_opcode_t op;
    l3_cfg_t cfg;
    l3_uc_cfg_t *uc = &cfg.cfg.uc;
    l3_fib_flexctr_action_t type;
    bool src_route = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!info || (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    src_route = fib->flags2 & BCM_L3_FLAGS2_SRC_ROUTE;

    op = BCMLT_OPCODE_LOOKUP;

    sal_memset(&cfg, 0, sizeof(l3_cfg_t));
    cfg.uc = true;

    if (fib->flags & BCM_L3_IP6) {
        /* Key. */
        bcmi_ltsw_util_ip6_to_uint64
            (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0],
            &fib->ip6_addr);
        bcmi_ltsw_util_ip6_to_uint64
            (&uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK],
            &fib->ip6_addr_mask);

        if (src_route) {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF;
            }
        } else {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
            }
        }

        type = src_route ? BCMINT_XGS_V6SRC_FLEXCTR_ACTION :
                           BCMINT_XGS_V6UC_FLEXCTR_ACTION;

    } else {
        /* Key. */
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = fib->ip_addr;
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = fib->ip_addr_mask;
        if (src_route) {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF;
            }
        } else {
            if (fib->vrf == BCM_L3_VRF_GLOBAL) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
            } else if (fib->vrf == BCM_L3_VRF_OVERRIDE) {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
            } else {
                tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
            }
        }

        type = src_route ? BCMINT_XGS_V4SRC_FLEXCTR_ACTION :
                           BCMINT_XGS_V4UC_FLEXCTR_ACTION;

    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    if (tbl_info->fld_bmp & ((1LL) << BCMINT_XGS_L3_FIB_TBL_UC_VRF)) {
        uc->flds[BCMINT_XGS_L3_FIB_TBL_UC_VRF] = fib->vrf;
    }

    uc->fld_bmp |= 1LL << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_alpm_op(unit, tbl_info, op, false, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

    if (fib->flex_ctr_action_index == 0) {
        info->action_index = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    } else {
        l3_fib_flex_action_profile_t action_profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_fib_flex_action_profile_get(unit,
                                            type,
                                            fib->flex_ctr_action_index,
                                            &action_profile));
        info->action_index = action_profile.action;
    }
    info->source = bcmFlexctrSourceL3Route;
    info->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->table_name = tbl_info->name;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_flexctr_object_set(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_SET;
    flexctr.flags = L3_FIB_FLXCTR_F_INDEX;
    flexctr.index = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_flexctr_op(unit, fib, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_GET;
    flexctr.flags = L3_FIB_FLXCTR_F_INDEX;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_uc_flexctr_op(unit, fib, &flexctr));

    *value = flexctr.index;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipmc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                             bcmi_ltsw_flexctr_counter_info_t *info)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_SET;
    flexctr.flags = L3_FIB_FLXCTR_F_ACTION;
    flexctr.action = info->action_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_flexctr_op(unit, fib, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipmc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_SET;
    flexctr.flags = L3_FIB_FLXCTR_F_ACTION;
    flexctr.action = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_flexctr_op(unit, fib, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipmc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                               bcmi_ltsw_flexctr_counter_info_t *info)
{
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *tbl_info = NULL;
    bcmlt_opcode_t op;
    l3_cfg_t cfg;
    l3_mc_cfg_t *mc = &cfg.cfg.mc;
    l3_fib_flexctr_action_t type;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!info || (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    op = BCMLT_OPCODE_LOOKUP;

    sal_memset(&cfg, 0, sizeof(l3_mc_cfg_t));

    if (fib->flags & BCM_L3_IP6) {
        /* Key. */
        bcmi_ltsw_util_ip6_to_uint64
            (&mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0],
            &fib->ip6_addr);
        if (sal_memcmp(fib->sip6_addr, ip6_zero, BCM_IP6_ADDRLEN) == 0) {
            sal_memcpy(&mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR], fib->sip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(&mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK], ip6_full, BCM_IP6_ADDRLEN);
        }
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_VRF] = fib->vrf;
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = fib->l3_iif;
        type = BCMINT_XGS_V6MC_FLEXCTR_ACTION;
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128;
    } else {
        /* Key. */
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0] = fib->ip_addr;
        if (fib->sip_addr) {
            mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK] = fib->sip_addr_mask;
            mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR] = fib->sip_addr;
        }
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_VRF] = fib->vrf;
        mc->flds[BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = fib->l3_iif;
        type = BCMINT_XGS_V4MC_FLEXCTR_ACTION;
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &tbl_info));

    mc->fld_bmp = 1LL << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_op(unit, tbl_info, op, false, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_cfg_to_fib(unit, tbl_info, &cfg, fib));

    if (fib->flex_ctr_action_index == 0) {
        info->action_index = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    } else {
        l3_fib_flex_action_profile_t action_profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_fib_flex_action_profile_get(unit,
                                            type,
                                            fib->flex_ctr_action_index,
                                            &action_profile));
        info->action_index = action_profile.action;
    }

    info->stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->source = bcmFlexctrSourceIpmc;
    info->table_name = tbl_info->name;

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ipmc_flexctr_object_set(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t value)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_SET;
    flexctr.flags = L3_FIB_FLXCTR_F_INDEX;
    flexctr.index = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_flexctr_op(unit, fib, &flexctr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipmc_flexctr_object_get(
    int unit,
    bcmi_ltsw_l3_fib_t *fib,
    uint32_t *value)
{
    l3_fib_flexctr_cfg_t flexctr;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&flexctr, 0, sizeof(l3_fib_flexctr_cfg_t));
    flexctr.op = L3_FIB_FLXCTR_OP_GET;
    flexctr.flags = L3_FIB_FLXCTR_F_INDEX;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_fib_mc_flexctr_op(unit, fib, &flexctr));

    *value = flexctr.index;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipmc_age(int unit, uint32_t attrib, bcm_ipmc_traverse_cb age_cb,
                     bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                     void *user_data)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));
    trav_info.op = L3_TRAV_OP_AGE;
    trav_info.ipmc_trav_cb = age_cb;
    trav_info.ipmc_addr_t_cb = cb;
    trav_info.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                     &trav_info));
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_hit_clear(int unit)
{
    l3_trav_t trav_info;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(&trav_info, 0, sizeof(l3_trav_t));

    trav_info.op = L3_TRAV_OP_CLR_HIT;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC,
                     &trav_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_traverse(unit, BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128,
                     &trav_info));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_route_perf_v4_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint32 ip,
    uint32 ip_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    int dunit;
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti = NULL;
    const bcmint_ltsw_l3_fib_fld_t *fi;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t lt_op;
    int rv = BCM_E_NONE;

    if (vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
    } else if (vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
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
            fi[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0].fld_id,
            ip);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK].fld_id,
            ip_mask);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_XGS_L3_FIB_TBL_UC_VRF].fld_id,
                vrf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
    }

    switch(op) {
    case BCMI_LTSW_RP_OPCODE_ADD:
        lt_op = BCMLT_OPCODE_INSERT;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID].fld_id,
                intf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
        break;
    case BCMI_LTSW_RP_OPCODE_UPD:
        lt_op = BCMLT_OPCODE_UPDATE;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID].fld_id,
                3);
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
xgs_ltsw_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    int dunit;
    bcmint_ltsw_l3_fib_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti = NULL;
    const bcmint_ltsw_l3_fib_fld_t *fi;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_opcode_t lt_op;
    int rv = BCM_E_NONE;

    if (vrf == BCM_L3_VRF_GLOBAL) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
    } else if (vrf == BCM_L3_VRF_OVERRIDE) {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
    } else {
        tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
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
            fi[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0].fld_id,
            v6[0]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK].fld_id,
            v6_mask[0]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1].fld_id,
            v6[1]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    rv = bcmlt_entry_field_add_by_id(eh,
            fi[BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK].fld_id,
            v6_mask[1]);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    if (ti->fld_bmp & (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF)) {
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_XGS_L3_FIB_TBL_UC_VRF].fld_id,
                vrf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
    }

    switch(op) {
    case BCMI_LTSW_RP_OPCODE_ADD:
        lt_op = BCMLT_OPCODE_INSERT;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID].fld_id,
                intf);
        if (BCM_FAILURE(rv)) {
            goto exit;
        }
        break;
    case BCMI_LTSW_RP_OPCODE_UPD:
        lt_op = BCMLT_OPCODE_UPDATE;
        rv = bcmlt_entry_field_add_by_id(eh,
                fi[BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID].fld_id,
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

