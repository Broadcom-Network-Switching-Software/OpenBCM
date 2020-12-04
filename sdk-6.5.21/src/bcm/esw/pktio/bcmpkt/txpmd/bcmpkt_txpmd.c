/*! \file bcmpkt_txpmd.c
 *
 * TX Packet MetaData (TXPMD, called SOBMH in hardware) access interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#endif
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_txpmd_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_TXPMD

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_txpmd_fget_t _bd##_txpmd_fget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_txpmd_fget,
static const bcmpkt_txpmd_fget_t *txpmd_fget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_txpmd_fset_t _bd##_txpmd_fset;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_txpmd_fset,
static const bcmpkt_txpmd_fset_t *txpmd_fset[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_txpmd_figet_t _bd##_txpmd_figet;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_txpmd_figet,
static const bcmpkt_txpmd_figet_t *txpmd_figet[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_txpmd_view_info_get,
static void (*view_info_get[])(bcmpkt_pmd_view_info_t *info) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

static const shr_enum_map_t field_names[] =
{
    BCMPKT_TXPMD_FIELD_NAME_MAP_INIT
};

int
bcmpkt_txpmd_len_get(bcmdrd_dev_type_t dev_type, uint32_t *len)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(len, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (txpmd_figet[dev_type] == NULL ||
        txpmd_figet[dev_type]->fget[BCMPKT_TXPMD_I_SIZE] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    *len = txpmd_figet[dev_type]->fget[BCMPKT_TXPMD_I_SIZE](NULL, NULL) * 4;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_field_get(bcmdrd_dev_type_t dev_type, uint32_t *txpmd,
                       int fid, uint32_t *val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(txpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_TXPMD_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (txpmd_fget[dev_type] == NULL ||
        txpmd_fget[dev_type]->fget[fid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    *val = txpmd_fget[dev_type]->fget[fid](txpmd);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_field_set(bcmdrd_dev_type_t dev_type, uint32_t *txpmd,
                       int fid, uint32_t val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(txpmd, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_TXPMD_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (txpmd_fset[dev_type] == NULL ||
        txpmd_fset[dev_type]->fset[fid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    txpmd_fset[dev_type]->fset[fid](txpmd, val);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_field_name_get(int fid, char **name)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    if (fid <= BCMPKT_TXPMD_FID_INVALID ||
        fid >= BCMPKT_TXPMD_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *name = field_names[fid].name;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_field_id_get(char* name, int *fid)
{
    int i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    for (i = BCMPKT_TXPMD_FID_INVALID + 1; i < BCMPKT_TXPMD_FID_COUNT; i++) {
        if (sal_strcasecmp(field_names[i].name, name) == 0) {
            *fid = field_names[i].val;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
            return SHR_E_NONE;
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_txpmd_fid_support_t *support)
{
    int i;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    if (view_info_get[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    SHR_NULL_CHECK(support, SHR_E_PARAM);
    sal_memset(support, 0, sizeof(*support));

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    for (i = BCMPKT_TXPMD_FID_INVALID + 1; i < BCMPKT_TXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] >= -1) {
            SHR_BITSET(support->fbits, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
                             shr_pb_t *pb)
{
    int i, j;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    if (view_info_get[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_TXPMD_FID_INVALID + 1; i < BCMPKT_TXPMD_FID_COUNT; i++) {
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
        for (i = BCMPKT_TXPMD_FID_INVALID + 1; i < BCMPKT_TXPMD_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_info.view_types[j].val) {
                shr_pb_printf(pb, "    %s\n", field_names[i].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_txpmd_dump(bcmdrd_dev_type_t dev_type, uint32_t *txpmd, uint32_t flags,
                  shr_pb_t *pb)
{
    int i;
    int view_enc, val;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(txpmd, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (txpmd_fget[dev_type] == NULL ||
        view_info_get[dev_type] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_TXPMD_FID_INVALID + 1; i < BCMPKT_TXPMD_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            val = txpmd_fget[dev_type]->fget[i](txpmd);
            if ((val != 0) || (flags == BCMPKT_TXPMD_DUMP_F_ALL)) {
                shr_pb_printf(pb, "\t%-32s:0x%x(%d)\n",
                              field_names[i].name, val, val);
            }
        }
    }

    /* Print view fields. */
    if (view_info.view_type_get) {
        view_enc = view_info.view_type_get(txpmd);
        for (i = BCMPKT_TXPMD_FID_INVALID + 1; i < BCMPKT_TXPMD_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_enc) {
                val = txpmd_fget[dev_type]->fget[i](txpmd);
                if ((val != 0) || (flags == BCMPKT_TXPMD_DUMP_F_ALL)) {
                    shr_pb_printf(pb, "\t%-32s:0x%x(%d)\n",
                                  field_names[i].name, val, val);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
