/*! \file bcmpkt_lbhdr.c
 *
 * Loopback header (LBHDR, called LOOPBACK_MH in hardware) access interface.
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
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_lbhdr.h>
#include <bcmpkt/bcmpkt_lbhdr_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_LBHDR

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_lbhdr_fget_t _bd##_lbhdr_fget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_lbhdr_fget,
static const bcmpkt_lbhdr_fget_t *lbhdr_fget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_lbhdr_fset_t _bd##_lbhdr_fset;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_lbhdr_fset,
static const bcmpkt_lbhdr_fset_t *lbhdr_fset[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_lbhdr_figet_t _bd##_lbhdr_figet;
#include <bcmdrd/bcmdrd_devlist.h>

#if 0
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_lbhdr_figet,
static const bcmpkt_lbhdr_figet_t *lbhdr_figet[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};
#endif

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_lbhdr_view_info_get,
static void (*view_info_get[])(bcmpkt_pmd_view_info_t *info) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

static const shr_enum_map_t field_names[] =
{
    BCMPKT_LBHDR_FIELD_NAME_MAP_INIT
};

int
bcmpkt_lbhdr_field_get(bcmdrd_dev_type_t dev_type, uint32_t *lbhdr,
                       int fid, uint32_t *val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(lbhdr, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_LBHDR_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (lbhdr_fget[dev_type] == NULL ||
        lbhdr_fget[dev_type]->fget[fid] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *val = lbhdr_fget[dev_type]->fget[fid](lbhdr);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_field_set(bcmdrd_dev_type_t dev_type, uint32_t *lbhdr,
                       int fid, uint32_t val)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(lbhdr, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fid < 0 || fid >= BCMPKT_LBHDR_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (lbhdr_fset[dev_type] == NULL ||
        lbhdr_fset[dev_type]->fset[fid] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    lbhdr_fset[dev_type]->fset[fid](lbhdr, val);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_field_name_get(int fid, char **name)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    if (fid <= BCMPKT_LBHDR_FID_INVALID ||
        fid >= BCMPKT_LBHDR_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *name = field_names[fid].name;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_field_id_get(char* name, int *fid)
{
    int i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    for (i = BCMPKT_LBHDR_FID_INVALID + 1; i < BCMPKT_LBHDR_FID_COUNT; i++) {
        if (sal_strcasecmp(field_names[i].name, name) == 0) {
            *fid = field_names[i].val;
            SHR_EXIT();
            return SHR_E_NONE;
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_fid_support_get(bcmdrd_dev_type_t dev_type,
                             bcmpkt_lbhdr_fid_support_t *support)
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

    for (i = BCMPKT_LBHDR_FID_INVALID + 1; i < BCMPKT_LBHDR_FID_COUNT; i++) {
        if (view_info.view_infos[i] >= -1) {
            SHR_BITSET(support->fbits, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_field_list_dump(bcmdrd_dev_type_t dev_type, char *view_name,
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
    for (i = BCMPKT_LBHDR_FID_INVALID + 1; i < BCMPKT_LBHDR_FID_COUNT; i++) {
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
        for (i = BCMPKT_LBHDR_FID_INVALID + 1; i < BCMPKT_LBHDR_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_info.view_types[j].val) {
                shr_pb_printf(pb, "    %s\n", field_names[i].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_lbhdr_dump(bcmdrd_dev_type_t dev_type, uint32_t *lbhdr, uint32_t flags,
                  shr_pb_t *pb)
{
    int i;
    int view_enc;
    uint32_t val;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(lbhdr, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    if (lbhdr_fget[dev_type] == NULL ||
        view_info_get[dev_type] == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[dev_type](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_LBHDR_FID_INVALID + 1; i < BCMPKT_LBHDR_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            val = lbhdr_fget[dev_type]->fget[i](lbhdr);
            if ((val != 0) || (flags == BCMPKT_LBHDR_DUMP_F_ALL)) {
                shr_pb_printf(pb, "\t%s=", field_names[i].name);
                shr_pb_format_uint32(pb, NULL, &val, 1, 0);
                shr_pb_printf(pb, "\n");
            }
        }
    }

    /* Print view fields. */
    if (view_info.view_type_get) {
        view_enc = view_info.view_type_get(lbhdr);
        for (i = BCMPKT_LBHDR_FID_INVALID + 1; i < BCMPKT_LBHDR_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_enc) {
                val = lbhdr_fget[dev_type]->fget[i](lbhdr);
                if ((val != 0) || (flags == BCMPKT_LBHDR_DUMP_F_ALL)) {
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
