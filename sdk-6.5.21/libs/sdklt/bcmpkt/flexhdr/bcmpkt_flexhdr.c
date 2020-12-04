/*! \file bcmpkt_flexhdr.c
 *
 * Flexhdr access interface.
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb_format.h>
#include <bsl/bsl.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmpkt/bcmpkt_flexhdr.h>
#include <bcmpkt/bcmpkt_flexhdr_internal.h>
#include <bcmpkt/bcmpkt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMPKT_FLEX_HDR

/* Define stub functions for base variant. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
bcmpkt_flex_pmd_info_t *  _bc##_flex_pmd_info_get(uint32_t hid) {return  NULL;}
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
shr_enum_map_t *  _bc##_flexhdr_map_get(void) {return NULL;}
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

/* Array of device variant specific data */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_flex_pmd_info_get,
static bcmpkt_flex_pmd_info_t * (*flex_pmd_info_get[])(uint32_t hid) = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_flexhdr_map_get,
static shr_enum_map_t * (*flexhdr_map_get[])(void) = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

/* Private variables */
int bcmpkt_flexhdr_support_map[BCMDRD_CONFIG_MAX_UNITS][BCMPKT_PMD_COUNT];

int
bcmpkt_flexhdr_header_id_get(bcmlrd_variant_t variant,
                             char *name, uint32_t *hid)
{
    shr_enum_map_t *id_map = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(hid, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flexhdr_map_get[variant], SHR_E_UNAVAIL);

    id_map = flexhdr_map_get[variant]();
    SHR_NULL_CHECK(id_map, SHR_E_UNAVAIL);
    while (sal_strcasecmp(id_map->name, "flexhdr count") != 0 ) {
        if (sal_strcasecmp(id_map->name, name) == 0) {
            *hid = id_map->val;
            SHR_EXIT();
        }
        id_map++;
    }
    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_is_supported(bcmlrd_variant_t variant, uint32_t hid,
                            bool *is_supported)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(is_supported, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    *is_supported = pmd_info->is_supported;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_field_get(bcmlrd_variant_t variant, uint32_t hid,
                         uint32_t *flexhdr, int profile, int fid, uint32_t *val)
{
    int32_t ret = SHR_E_NONE;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(flexhdr, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);
    if (fid <= BCMPKT_FID_INVALID || fid >= pmd_info->field_info->num_fields) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(pmd_info->flex_fget[fid], SHR_E_UNAVAIL);

    ret = (pmd_info->flex_fget[fid])(flexhdr, profile, val);
    if (ret < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_field_set(bcmlrd_variant_t variant, uint32_t hid,
                         uint32_t *flexhdr, int profile, int fid, uint32_t val)
{
    int32_t ret = SHR_E_NONE;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(flexhdr, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);
    if (fid <= BCMPKT_FID_INVALID || fid >= pmd_info->field_info->num_fields) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(pmd_info->flex_fset[fid], SHR_E_UNAVAIL);

    ret = (pmd_info->flex_fset[fid])(flexhdr, profile, val);
    if (ret < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_field_name_get(bcmlrd_variant_t variant, uint32_t hid,
                              int fid, char **name)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);
    if (fid <= BCMPKT_FID_INVALID || fid >= pmd_info->field_info->num_fields) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(pmd_info->field_info->field_names, SHR_E_UNAVAIL);
    *name = pmd_info->field_info->field_names[fid].name;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_field_id_get(bcmlrd_variant_t variant, uint32_t hid,
                            char *name, int *fid)
{
    int i;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);

    SHR_NULL_CHECK(pmd_info->field_info->field_names, SHR_E_UNAVAIL);
    for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
        if (sal_strcasecmp(pmd_info->field_info->field_names[i].name, name) == 0) {
            *fid = pmd_info->field_info->field_names[i].val;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_field_info_get(bcmlrd_variant_t variant, uint32_t hid,
                              bcmpkt_flex_field_info_t *info)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);

    *info = *(pmd_info->field_info);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_field_list_dump(bcmlrd_variant_t variant, uint32_t hid,
                               char *view_name, shr_pb_t *pb)
{
    int i, j;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->view_types, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->view_infos, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
        if (pmd_info->view_infos[i] == -1) {
            shr_pb_printf(pb, "    %s\n",
                          pmd_info->field_info->field_names[i].name);
        }
    }

    /* Print view fields. */
    for (j = 0; pmd_info->view_types[j].name != NULL; j++) {
        if (view_name) {
            if (sal_strcasecmp(view_name, pmd_info->view_types[j].name)) {
                continue;
            }
        }
        for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
            if (pmd_info->view_infos[i] == pmd_info->view_types[j].val) {
                shr_pb_printf(pb, "    %s\n",
                              pmd_info->field_info->field_names[i].name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_dump(bcmlrd_variant_t variant, uint32_t hid,
                    uint32_t *flexhdr, uint32_t flags, shr_pb_t *pb)
{
    int i, ret;
    int view_enc;
    uint32_t val;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(flexhdr, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE || variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->view_infos, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
        if (pmd_info->view_infos[i] == -1) {
            ret = (pmd_info->flex_fget[i])(flexhdr,
                   BCMPKT_FLEXHDR_PROFILE_NONE, &val);
            if (((val > 0) || (flags == BCMPKT_DUMP_F_ALL)) && (ret >= 0)) {
                shr_pb_printf(pb, "\t%-32s:0x%x(%d)\n",
                              pmd_info->field_info->field_names[i].name,
                              val, val);
            }
        }
    }

    /* Print view fields. */
    if (pmd_info->view_type_get) {
        view_enc = pmd_info->view_type_get(flexhdr);
        for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
            if (pmd_info->view_infos[i] == view_enc) {
                ret = (pmd_info->flex_fget[i])(flexhdr,
                       BCMPKT_FLEXHDR_PROFILE_NONE, &val);
                if (((val > 0) || (flags == BCMPKT_DUMP_F_ALL)) && (ret >= 0)) {
                    shr_pb_printf(pb, "\t%-32s:0x%x(%d)\n",
                                  pmd_info->field_info->field_names[i].name,
                                  val, val);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flex_dump(bcmlrd_variant_t variant, uint32_t *rxpmd_flex,
                       uint32_t profile, uint32_t flags, shr_pb_t *pb)
{
    int i, ret;
    int view_enc;
    uint32_t val;
    uint32_t hid;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd_flex, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->view_infos, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->field_info, SHR_E_UNAVAIL);

    /* Print common fields. */
    for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
        if (pmd_info->view_infos[i] == -1) {
            ret = (pmd_info->flex_fget[i])(rxpmd_flex, profile, &val);
            if (((val > 0) || (flags == BCMPKT_DUMP_F_ALL)) && (ret >= 0)) {
                shr_pb_printf(pb, "\t%s=",
                              pmd_info->field_info->field_names[i].name);
                shr_pb_format_uint32(pb, NULL, &val, 1, 0);
                shr_pb_printf(pb, "\n");
            }
        }
    }

    /* Print view fields. */
    if (pmd_info->view_type_get) {
        view_enc = pmd_info->view_type_get(rxpmd_flex);
        for (i = BCMPKT_FID_INVALID + 1; i < pmd_info->field_info->num_fields; i++) {
            if (pmd_info->view_infos[i] == view_enc) {
                ret = (pmd_info->flex_fget[i])(rxpmd_flex, profile, &val);
                if (((val > 0) || (flags == BCMPKT_DUMP_F_ALL)) && (ret >= 0)) {
                    shr_pb_printf(pb, "\t%s=",
                                  pmd_info->field_info->field_names[i].name);
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
bcmpkt_rxpmd_flex_reasons_get(bcmlrd_variant_t variant,
                              uint32_t *rxpmd_flex, bcmpkt_bitmap_t *reasons)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;
    uint32_t hid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd_flex, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->reasons_info, SHR_E_UNAVAIL);

    SHR_BITCLR_RANGE
        (((*reasons).pbits), 0, pmd_info->reasons_info->num_reasons);
    pmd_info->reasons_info->reason_decode (rxpmd_flex, reasons);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flex_reasons_set(bcmlrd_variant_t variant,
                              bcmpkt_bitmap_t *reasons, uint32_t *rxpmd_flex)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;
    uint32_t hid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd_flex, SHR_E_PARAM);
    SHR_NULL_CHECK(reasons, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->reasons_info, SHR_E_UNAVAIL);

    pmd_info->reasons_info->reason_encode(reasons, rxpmd_flex);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flex_reason_dump(bcmlrd_variant_t variant,
                              uint32_t *rxpmd_flex, shr_pb_t *pb)
{
    int reason;
    bcmpkt_bitmap_t reasons;
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;
    uint32_t hid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(rxpmd_flex, SHR_E_PARAM);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->reasons_info, SHR_E_UNAVAIL);

    SHR_BITCLR_RANGE
        (((reasons).pbits), 0, pmd_info->reasons_info->num_reasons);
    SHR_IF_ERR_EXIT
        (bcmpkt_rxpmd_flex_reasons_get(variant, rxpmd_flex, &reasons));

    for (reason = 0;
        reason < pmd_info->reasons_info->num_reasons; reason++) {
        if (BCMPKT_RXPMD_FLEX_REASON_GET(reasons, reason)) {
            shr_pb_printf(pb, "\t%s\n",
                pmd_info->reasons_info->reason_names[reason].name);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flex_reason_name_get(bcmlrd_variant_t variant,
                                  int reason, char **name)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;
    uint32_t hid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->reasons_info, SHR_E_UNAVAIL);

    if (reason <= 0 ||
        reason > pmd_info->reasons_info->num_reasons) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(pmd_info->reasons_info->reason_names, SHR_E_UNAVAIL);
    *name = pmd_info->reasons_info->reason_names[reason].name;
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flex_reason_max_get(bcmlrd_variant_t variant, uint32_t *num)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;
    uint32_t hid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(num, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->reasons_info, SHR_E_UNAVAIL);

    *num = pmd_info->reasons_info->num_reasons;
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_rxpmd_flex_reason_id_get(bcmlrd_variant_t variant,
                                char *name, int *rid)
{
    bcmpkt_flex_pmd_info_t *pmd_info = NULL;
    int i;
    uint32_t hid;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(name, SHR_E_PARAM);
    SHR_NULL_CHECK(rid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_flexhdr_header_id_get(variant, "RXPMD_FLEX_T", &hid));
    SHR_NULL_CHECK(flex_pmd_info_get[variant], SHR_E_UNAVAIL);
    pmd_info = flex_pmd_info_get[variant](hid);
    SHR_NULL_CHECK(pmd_info, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(pmd_info->reasons_info, SHR_E_UNAVAIL);

    SHR_NULL_CHECK(pmd_info->reasons_info->reason_names, SHR_E_UNAVAIL);
    for (i = 0; i < pmd_info->reasons_info->num_reasons; i++) {
        if (sal_strcasecmp(pmd_info->reasons_info->reason_names[i].name,
            name) == 0) {
            *rid = pmd_info->reasons_info->reason_names[i].val;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_flexhdr_init(int unit)
{
    bcmlrd_variant_t variant;
    shr_enum_map_t *id_map = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize header IDs */
    for (i = 0; i < BCMPKT_PMD_COUNT; i++) {
        bcmpkt_flexhdr_support_map[unit][i] = -1;
    }

    if (flexhdr_map_get[variant] == NULL) {
        /* Nothing to be done when flexhdr not supported. */
        SHR_EXIT();
    }

    id_map = flexhdr_map_get[variant]();
    if (id_map) {
        while (sal_strcasecmp(id_map->name, "flexhdr count") != 0 ) {
            if (sal_strcasecmp(id_map->name, "GENERIC_LOOPBACK_T") == 0) {
                bcmpkt_flexhdr_support_map[unit]
                    [BCMPKT_GENERIC_LOOPBACK_T] = id_map->val;
            } else if (sal_strcasecmp(id_map->name, "HG3_BASE_T") == 0) {
                bcmpkt_flexhdr_support_map[unit][BCMPKT_HG3_BASE_T] = id_map->val;
            } else if (sal_strcasecmp(id_map->name, "HG3_EXTENSION_0_T") == 0) {
                bcmpkt_flexhdr_support_map[unit]
                    [BCMPKT_HG3_EXTENSION_0_T] = id_map->val;
            } else if (sal_strcasecmp(id_map->name, "RXPMD_FLEX_T") == 0) {
                bcmpkt_flexhdr_support_map[unit]
                    [BCMPKT_RXPMD_FLEX_T] = id_map->val;
            }
            id_map++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

const int *
bcmpkt_flexhdr_support_map_get(int unit)
{
    return &bcmpkt_flexhdr_support_map[unit][0];
}
