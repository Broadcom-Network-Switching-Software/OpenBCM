/*! \file bcmpkt_gih.c
 *
 * Generic Internal header(gih) access interface.
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
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmpkt/bcmpkt_gih.h>
#include <bcmpkt/bcmpkt_gih_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_GENERIC_LOOPBACK

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern const bcmpkt_gih_fget_t _bd##_vu##_va##_gih_fget;
#include <bcmlrd/chip/bcmlrd_variant.h>

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_gih_fget,
static const bcmpkt_gih_fget_t *gih_fget[] = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern const bcmpkt_gih_fset_t _bd##_vu##_va##_gih_fset;
#include <bcmlrd/chip/bcmlrd_variant.h>

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_gih_fset,
static const bcmpkt_gih_fset_t *gih_fset[] = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern const bcmpkt_gih_figet_t _bd##_vu##_va##_gih_figet;
#include <bcmlrd/chip/bcmlrd_variant.h>

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_gih_figet,
static const bcmpkt_gih_figet_t *gih_figet[] = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_gih_view_info_get,
static void (*view_info_get[])(bcmpkt_pmd_view_info_t *info) = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

static const shr_enum_map_t field_names[] =
{
    BCMPKT_GIH_FIELD_NAME_MAP_INIT
};

int
bcmpkt_gih_is_supported(int unit, bcmdrd_dev_type_t dev_type, bool *is_supported)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(is_supported, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    variant = bcmlrd_variant_get(unit);

    if (gih_figet[variant] == NULL ||
        gih_figet[variant]->fget[BCMPKT_GIH_I_SUPPORT] == NULL) {
        *is_supported = false;
    } else {
        *is_supported = gih_figet[variant]->fget[BCMPKT_GIH_I_SUPPORT](NULL, NULL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_field_get(int unit, bcmdrd_dev_type_t dev_type, uint32_t *gih,
                     int fid, uint32_t *val)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(gih, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    variant = bcmlrd_variant_get(unit);

    if (fid <= BCMPKT_GIH_FID_INVALID || fid >= BCMPKT_GIH_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (gih_fget[variant] == NULL ||
        gih_fget[variant]->fget[fid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    *val = gih_fget[variant]->fget[fid](gih);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_field_set(int unit, bcmdrd_dev_type_t dev_type, uint32_t *gih,
                     int fid, uint32_t val)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(gih, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    variant = bcmlrd_variant_get(unit);

    if (fid <= BCMPKT_GIH_FID_INVALID || fid >= BCMPKT_GIH_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (gih_fset[variant] == NULL ||
        gih_fset[variant]->fset[fid] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_UNAVAIL);
    }

    gih_fset[variant]->fset[fid](gih, val);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_field_name_get(int fid, char **name)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    if (fid <= BCMPKT_GIH_FID_INVALID ||
        fid >= BCMPKT_GIH_FID_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    *name = field_names[fid].name;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_field_id_get(char *name, int *fid)
{
    int i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    for (i = BCMPKT_GIH_FID_INVALID + 1; i < BCMPKT_GIH_FID_COUNT; i++) {
        if (sal_strcasecmp(field_names[i].name, name) == 0) {
            *fid = field_names[i].val;
            SHR_RETURN_VAL_EXIT(SHR_E_NONE);
        }
    }
    SHR_RETURN_VAL_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_fid_support_get(int unit, bcmdrd_dev_type_t dev_type,
                           bcmpkt_gih_fid_support_t *support)
{
    int i;
    bcmlrd_variant_t variant;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(unit);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    variant = bcmlrd_variant_get(unit);

    if (view_info_get[variant] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }
    SHR_NULL_CHECK(support, SHR_E_PARAM);
    sal_memset(support, 0, sizeof(*support));

    view_info_get[variant](&view_info);
    SHR_NULL_VERBOSE_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_VERBOSE_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    for (i = BCMPKT_GIH_FID_INVALID + 1; i < BCMPKT_GIH_FID_COUNT; i++) {
        if (view_info.view_infos[i] >= -1) {
            SHR_BITSET(support->fbits, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_field_list_dump(int unit, bcmdrd_dev_type_t dev_type,
                           char *view_name, shr_pb_t *pb)
{
    int i, j;
    bcmlrd_variant_t variant;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(unit);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    variant = bcmlrd_variant_get(unit);

    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    if (view_info_get[variant] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[variant](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_GIH_FID_INVALID + 1; i < BCMPKT_GIH_FID_COUNT; i++) {
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
        for (i = BCMPKT_GIH_FID_INVALID + 1; i < BCMPKT_GIH_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_info.view_types[j].val) {
                shr_pb_printf(pb, "    %s\n", field_names[i].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_gih_dump(int unit, bcmdrd_dev_type_t dev_type, uint32_t *gih,
                uint32_t flags, shr_pb_t *pb)
{
    int i;
    int view_enc, val;
    bcmlrd_variant_t variant;
    bcmpkt_pmd_view_info_t view_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(gih, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (dev_type <= BCMDRD_DEV_T_NONE ||
        dev_type >= BCMDRD_DEV_T_COUNT) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    variant = bcmlrd_variant_get(unit);

    if (gih_fget[variant] == NULL ||
        view_info_get[variant] == NULL) {
        SHR_RETURN_VAL_EXIT(SHR_E_INTERNAL);
    }

    view_info_get[variant](&view_info);
    SHR_NULL_CHECK(view_info.view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(view_info.view_infos, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_GIH_FID_INVALID + 1; i < BCMPKT_GIH_FID_COUNT; i++) {
        if (view_info.view_infos[i] == -1) {
            val = gih_fget[variant]->fget[i](gih);
            if ((val != 0) || (flags == BCMPKT_GIH_DUMP_F_ALL)) {
                shr_pb_printf(pb, "\t%-32s:0x%x(%d)\n",
                              field_names[i].name, val, val);
            }
        }
    }

    /* Print view fields. */
    if (view_info.view_type_get) {
        view_enc = view_info.view_type_get(gih);
        for (i = BCMPKT_GIH_FID_INVALID + 1; i < BCMPKT_GIH_FID_COUNT; i++) {
            if (view_info.view_infos[i] == view_enc) {
                val = gih_fget[variant]->fget[i](gih);
                if ((val != 0) || (flags == BCMPKT_GIH_DUMP_F_ALL)) {
                    shr_pb_printf(pb, "\t%-32s:0x%x(%d)\n",
                                  field_names[i].name, val, val);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
