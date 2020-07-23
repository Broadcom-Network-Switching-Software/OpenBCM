/*! \file l3_aacl.c
 *
 * XGS L3 Driver.
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
#include <bcm_int/ltsw/l3_aacl.h>
#include <bcm_int/ltsw/l3_aacl_int.h>
#include <bcm_int/ltsw/xgs/l3_aacl.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3
/* Generic data structure contains the info of l3 AACL entry. */
typedef struct l3_aacl_cfg_s {
    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;

    /* Fields array. */
    uint64_t flds[BCMINT_XGS_L3_AACL_TBL_FLD_CNT];
} l3_aacl_cfg_t;

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
lt_eh_fill(int unit, bcmlt_entry_handle_t eh, const bcmint_ltsw_l3_aacl_tbl_t *ti,
           l3_aacl_cfg_t *cfg)
{
    const bcmint_ltsw_l3_aacl_fld_t *fi = ti->flds;
    uint64_t *fv;
    const char *sym_val;
    uint32_t i, cnt;
    uint64_t fld_bmp;

    SHR_FUNC_ENTER(unit);

    fv = cfg->flds;
    fld_bmp = cfg->fld_bmp;
    cnt = BCMINT_XGS_L3_AACL_TBL_FLD_CNT;

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
lt_eh_parse(int unit, bcmlt_entry_handle_t eh, const bcmint_ltsw_l3_aacl_tbl_t *ti,
            l3_aacl_cfg_t *cfg)
{
    const bcmint_ltsw_l3_aacl_fld_t *fi = ti->flds;
    uint64_t *fv;
    uint32_t i, cnt, sz;
    const char *sym_val;
    uint64_t fld_bmp;
    int rv;

    SHR_FUNC_ENTER(unit);

    fv = cfg->flds;
    fld_bmp = cfg->fld_bmp;
    cnt = BCMINT_XGS_L3_AACL_TBL_FLD_CNT;

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


/******************************************************************************
 * Public Internal HSDK functions
 */
int
xgs_ltsw_l3_aacl_init(int unit)
{
    return SHR_E_NONE;
}

int
xgs_ltsw_l3_aacl_deinit(int unit)
{
    return SHR_E_NONE;
}

int
xgs_ltsw_l3_aacl_validate(
    int unit,
    bcm_l3_aacl_t *aacl,
    bool strict)
{
    return SHR_E_NONE;
}

int
xgs_ltsw_l3_aacl_add(
    int unit,
    uint32_t options,
    bcm_l3_aacl_t *aacl)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_aacl_tbl_t *tbl_info = NULL;
    bcmint_ltsw_l3_aacl_tbl_id_t tbl_id;
    bcmlt_opcode_t opcode;
    l3_aacl_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(aacl, SHR_E_PARAM);

    sal_memset(&cfg, 0, sizeof(l3_aacl_cfg_t));

    if (options & BCM_L3_AACL_OPTIONS_REPLACE) {
        opcode = BCMLT_OPCODE_UPDATE;
    } else {
        opcode = BCMLT_OPCODE_INSERT;
    }
    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV6_DST;
    } else {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;
    }

    SHR_IF_ERR_EXIT
           (bcmint_ltsw_l3_aacl_tbl_get(unit, tbl_id, &tbl_info));

    cfg.fld_bmp = tbl_info->fld_bmp;
    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        sal_memcpy(&cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0], aacl->ip6, BCM_IP6_ADDRLEN);
        sal_memcpy(&cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK], aacl->ip6_mask, BCM_IP6_ADDRLEN);
    } else {
        cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = aacl->ip;
        cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = aacl->ip_mask;
    }
    cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT] = aacl->l4_port;
    cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID] = aacl->class_id;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_eh_fill(unit, eh, tbl_info, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));


exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_aacl_delete(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_aacl_tbl_t *tbl_info = NULL;
    bcmint_ltsw_l3_aacl_tbl_id_t tbl_id;
    bcmlt_opcode_t opcode;
    l3_aacl_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(aacl, SHR_E_PARAM);

    sal_memset(&cfg, 0, sizeof(l3_aacl_cfg_t));

    opcode = BCMLT_OPCODE_DELETE;

    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV6_DST;
    } else {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;
    }

    SHR_IF_ERR_EXIT
           (bcmint_ltsw_l3_aacl_tbl_get(unit, tbl_id, &tbl_info));

    cfg.fld_bmp = tbl_info->fld_bmp;
    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        sal_memcpy(&cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0], aacl->ip6, BCM_IP6_ADDRLEN);
        sal_memcpy(&cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK], aacl->ip6_mask, BCM_IP6_ADDRLEN);
    } else {
        cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = aacl->ip;
        cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = aacl->ip_mask;
    }
    cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT] = aacl->l4_port;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_eh_fill(unit, eh, tbl_info, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));


exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_aacl_delete_all(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_aacl_tbl_t *tbl_info = NULL;
    bcmint_ltsw_l3_aacl_tbl_id_t tbl_id;
    l3_aacl_cfg_t cfg;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(aacl, SHR_E_PARAM);

    sal_memset(&cfg, 0, sizeof(l3_aacl_cfg_t));

    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV6_DST;
    } else {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;
    }

    SHR_IF_ERR_EXIT
           (bcmint_ltsw_l3_aacl_tbl_get(unit, tbl_id, &tbl_info));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(eh,
                                BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_aacl_find(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_aacl_tbl_t *tbl_info = NULL;
    bcmint_ltsw_l3_aacl_tbl_id_t tbl_id;
    bcmlt_opcode_t opcode;
    l3_aacl_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(aacl, SHR_E_PARAM);

    sal_memset(&cfg, 0, sizeof(l3_aacl_cfg_t));

    opcode = BCMLT_OPCODE_LOOKUP;

    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV6_DST;
    } else {
        tbl_id = (aacl->flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_aacl_tbl_get(unit, tbl_id, &tbl_info));

    cfg.fld_bmp = tbl_info->fld_bmp;
    if (aacl->flags & BCM_L3_AACL_FLAGS_IP6) {
        sal_memcpy(&cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0], aacl->ip6, BCM_IP6_ADDRLEN);
        sal_memcpy(&cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK], aacl->ip6_mask, BCM_IP6_ADDRLEN);
    } else {
        cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = aacl->ip;
        cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = aacl->ip_mask;
    }
    cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT] = aacl->l4_port;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_eh_fill(unit, eh, tbl_info, &cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (lt_eh_parse(unit, eh, tbl_info, &cfg));

    aacl->class_id = (uint32)(cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID]);

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}



int
xgs_ltsw_l3_aacl_traverse(
    int unit,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_aacl_tbl_t *tbl_info = NULL;
    bcmint_ltsw_l3_aacl_tbl_id_t tbl_id;
    l3_aacl_cfg_t cfg;
    int rv;
    bcm_l3_aacl_t aacl;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(l3_aacl_cfg_t));
    sal_memset(&aacl, 0, sizeof(bcm_l3_aacl_t));
    tbl_id = BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;

    SHR_IF_ERR_EXIT
           (bcmint_ltsw_l3_aacl_tbl_get(unit, tbl_id, &tbl_info));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        if (trav_fn) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lt_eh_parse(unit, eh, tbl_info, &cfg));
            aacl.ip = (bcm_ip_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0];
            aacl.ip_mask = (bcm_ip_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK];
            aacl.l4_port = (bcm_l4_port_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT];
            aacl.class_id = (uint32)cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID];
            rv = trav_fn(unit, &aacl, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }
    bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        if (trav_fn) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lt_eh_parse(unit, eh, tbl_info, &cfg));
            aacl.ip = (bcm_ip_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0];
            aacl.ip_mask = (bcm_ip_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK];
            aacl.l4_port = (bcm_l4_port_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT];
            aacl.class_id = (uint32)cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID];
            aacl.flags = BCM_L3_AACL_FLAGS_IP_SRC;
            rv = trav_fn(unit, &aacl, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }
    bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_AACL_TBL_IPV6_DST;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        if (trav_fn) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lt_eh_parse(unit, eh, tbl_info, &cfg));
            sal_memcpy(aacl.ip6, &cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0], BCM_IP6_ADDRLEN);
            sal_memcpy(aacl.ip6_mask, &cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK], BCM_IP6_ADDRLEN);
            aacl.l4_port = (bcm_l4_port_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT];
            aacl.class_id = (uint32)cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID];
            aacl.flags = BCM_L3_AACL_FLAGS_IP6;
            rv = trav_fn(unit, &aacl, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }
    bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    tbl_id = BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        if (trav_fn) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lt_eh_parse(unit, eh, tbl_info, &cfg));
            sal_memcpy(aacl.ip6, &cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0], BCM_IP6_ADDRLEN);
            sal_memcpy(aacl.ip6_mask, &cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK], BCM_IP6_ADDRLEN);
            aacl.l4_port = (bcm_l4_port_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT];
            aacl.class_id = (uint32)cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID];
            aacl.flags = BCM_L3_AACL_FLAGS_IP6 | BCM_L3_AACL_FLAGS_IP_SRC;
            rv = trav_fn(unit, &aacl, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }
    bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}



int
xgs_ltsw_l3_aacl_matched_traverse(
    int unit,
    uint32_t flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_aacl_tbl_t *tbl_info = NULL;
    bcmint_ltsw_l3_aacl_tbl_id_t tbl_id;
    l3_aacl_cfg_t cfg;
    int rv;
    bcm_l3_aacl_t aacl;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(l3_aacl_cfg_t));
    sal_memset(&aacl, 0, sizeof(bcm_l3_aacl_t));

    if (flags & BCM_L3_AACL_FLAGS_IP6) {
        tbl_id = (flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV6_DST;
    } else {
        tbl_id = (flags & BCM_L3_AACL_FLAGS_IP_SRC) ?
                 BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC : BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;
    }

    tbl_id = BCMINT_LTSW_L3_AACL_TBL_IPV4_DST;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_aacl_tbl_get(unit, tbl_id, &tbl_info));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate_by_id(dunit, tbl_info->tbl_id, tbl_info->fld_cnt,
                                    &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        if (trav_fn) {
            SHR_IF_ERR_VERBOSE_EXIT
                (lt_eh_parse(unit, eh, tbl_info, &cfg));
            aacl.ip = (bcm_ip_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0];
            aacl.ip_mask = (bcm_ip_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK];
            aacl.l4_port = (bcm_l4_port_t)cfg.flds[BCMINT_XGS_L3_AACL_TBL_L4_PORT];
            aacl.class_id = (uint32)cfg.flds[BCMINT_XGS_L3_AACL_TBL_CLASS_ID];
            aacl.flags = flags;
            rv = trav_fn(unit, &aacl, user_data);
            if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


