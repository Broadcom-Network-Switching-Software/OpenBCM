/*! \file bcmpkt_rxpmd.c
 *
 * RX Packet Meta Data (RXPMD, called EP_TO_CPU in hardware) access interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb_format.h>
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_rxpmd.h>
#include <bcmpkt/bcmpkt_rxpmd_internal.h>
#include <bcmlrd/bcmlrd_internal.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_rxpmd_fget_t _bd##_rxpmd_fget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_fget,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static const bcmpkt_rxpmd_fget_t *rxpmd_fget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_rxpmd_fset_t _bd##_rxpmd_fset;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_fset,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static const bcmpkt_rxpmd_fset_t *rxpmd_fset[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_rxpmd_figet_t _bd##_rxpmd_figet;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_figet,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static const bcmpkt_rxpmd_figet_t *rxpmd_figet[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rx_reason_decode,
/*! This sequence should be same as bcmdrd_cm_dev_type_t */
static void (*reason_fdecode[])(const uint32_t*, bcmpkt_rx_reasons_t*) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rx_reason_encode,
static void (*reason_fencode[])(const bcmpkt_rx_reasons_t*, uint32_t*) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_rxpmd_view_info_get,
static void (*view_info_get[])(bcmpkt_pmd_view_info_t *) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BSL_LOG_MODULE BSL_LS_BCMPKT_RXPMD

static const shr_enum_map_t field_names[] =
{
    BCMPKT_RXPMD_FIELD_NAME_MAP_INIT
};

static const shr_enum_map_t reason_names[] =
{
    BCMPKT_REASON_NAME_MAP_INIT
};

int
bcmpkt_rxpmd_len_get(bcmdrd_dev_type_t dev_type, uint32_t *len)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(len, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_SIZE] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_SIZE](NULL, NULL) * 4;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                       int fid, uint32_t *val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_RXPMD_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_fget[dev_type] == NULL ||
        rxpmd_fget[dev_type]->fget[fid] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *val = rxpmd_fget[dev_type]->fget[fid](rxpmd);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_set(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                       int fid, uint32_t val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_RXPMD_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_fset[dev_type] == NULL ||
        rxpmd_fset[dev_type]->fset[fid] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    rxpmd_fset[dev_type]->fset[fid](rxpmd, val);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_mh_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                    uint32_t **hg_hdr)
{
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(hg_hdr, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_MODULE_HDR] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_MODULE_HDR](rxpmd, hg_hdr);
    if (len <= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flexdata_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                          uint32_t **flexdata, uint32_t *len)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(flexdata, SHR_E_PARAM);
    SHR_NULL_CHECK(len, SHR_E_PARAM);

    *len = 0;
    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_FLEX_DATA] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_FLEX_DATA](rxpmd, flexdata);
    if (*len == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_reasons_get(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd,
                         bcmpkt_rx_reasons_t *reasons)
{
    uint32_t *reason = NULL;
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_REASON] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    BCMPKT_RX_REASON_CLEAR_ALL(*reasons);
    len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_REASON](rxpmd, &reason);
    if (len <= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    reason_fdecode[dev_type](reason, reasons);
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_reasons_set(bcmdrd_dev_type_t dev_type,
                         bcmpkt_rx_reasons_t *reasons, uint32_t *rxpmd)
{
    uint32_t *reason = NULL;
    int len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE || dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_figet[dev_type] == NULL ||
        rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_REASON] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    len = rxpmd_figet[dev_type]->fget[BCMPKT_RXPMD_I_REASON](rxpmd, &reason);
    if (len <= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(reason, 0, len * 4);
    reason_fencode[dev_type](reasons, reason);
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_name_get(int fid, char **name)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    if (fid <= BCMPKT_RXPMD_FID_INVALID ||
        fid >= BCMPKT_RXPMD_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *name = field_names[fid].name;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_id_get(char* name, int *fid)
{
    int i;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (sal_strcasecmp(field_names[i].name, name) == 0) {
            *fid = field_names[i].val;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_rxpmd_fid_support_t *support)
{
    int i;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (view_info_get[dev_type] == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_NULL_CHECK(support, SHR_E_PARAM);
    sal_memset(support, 0, sizeof(*support));

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] >= -1) {
            SHR_BITSET(support->fbits, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
                             shr_pb_t *pb)
{
    int i, j;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    if (view_info_get[dev_type] == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            shr_pb_printf(pb, "    %s\n", field_names[i].name);
        }
    }

    /* Print view fields. */
    for (j = 0; view_info.view_types[j].name != NULL; j++) {
        if (view_name) {
            if (sal_strcasecmp(view_name, view_info.view_types[j].name)) {
                continue;
            }
        }
        for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_info.view_types[j].val) {
                shr_pb_printf(pb, "    %s\n", field_names[i].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_dump(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd, uint32_t flags,
                  shr_pb_t *pb)
{
    int i;
    int view_enc;
    uint32_t val;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (rxpmd_fget[dev_type] == NULL ||
        view_info_get[dev_type] == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            val = rxpmd_fget[dev_type]->fget[i](rxpmd);
            if ((val != 0) || (flags == BCMPKT_RXPMD_DUMP_F_ALL)) {
                shr_pb_printf(pb, "\t%s=", field_names[i].name);
                shr_pb_format_uint32(pb, NULL, &val, 1, 0);
                shr_pb_printf(pb, "\n");
            }
        }
    }

    /* Print view fields. */
    if (view_info.view_type_get) {
        view_enc = view_info.view_type_get(rxpmd);
        for (i = BCMPKT_RXPMD_FID_INVALID + 1; i < BCMPKT_RXPMD_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_enc) {
                val = rxpmd_fget[dev_type]->fget[i](rxpmd);
                if ((val != 0) || (flags == BCMPKT_RXPMD_DUMP_F_ALL)) {
                    shr_pb_printf(pb, "\t%s=", field_names[i].name);
                    shr_pb_format_uint32(pb, NULL, &val, 1, 0);
                    shr_pb_printf(pb, "\n");
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_reason_dump(bcmdrd_dev_type_t dev_type, uint32_t *rxpmd, shr_pb_t *pb)
{
    int reason;
    bcmpkt_rx_reasons_t reasons;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (dev_type < 0 ||
        dev_type >= BCMDRD_DEV_T_COUNT ||
        rxpmd_fget[dev_type] == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMPKT_RX_REASON_CLEAR_ALL(reasons);
    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_reasons_get(dev_type, rxpmd, &reasons));

    BCMPKT_RX_REASON_ITER(reasons, reason) {
        shr_pb_printf(pb, "\t%s\n", reason_names[reason].name);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rx_reason_name_get(int reason, char **name)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    if (reason <= BCMPKT_RX_REASON_NONE ||
        reason > BCMPKT_RX_REASON_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *name = reason_names[reason].name;
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_match_id_data_get(bcmlrd_variant_t variant,
                               const char *spec,
                               const bcmlrd_match_id_db_t **info)
{
    const bcmlrd_match_id_db_info_t **match_id_info_all = NULL;
    const bcmlrd_match_id_db_info_t *match_id_info_variant;
    uint32_t id;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(spec, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    match_id_info_all = bcmlrd_dev_match_id_data_all_info_get();
    if (match_id_info_all == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    match_id_info_variant = match_id_info_all[variant];
    if (match_id_info_variant == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (id = 0; id < match_id_info_variant->num_entries; id++) {
        if (!sal_strcmp(match_id_info_variant->db[id].name, spec)) {
            *info = &match_id_info_variant->db[id];
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}
