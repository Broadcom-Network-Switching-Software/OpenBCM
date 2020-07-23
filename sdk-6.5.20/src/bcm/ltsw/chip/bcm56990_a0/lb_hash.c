/*! \file lb_hash.c
 *
 * LB hash management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/control.h>

#include <bcm/types.h>
#include <bcm_int/ltsw/mbcm/lb_hash.h>
#include <bcm_int/ltsw/xgs/lb_hash.h>
#include <bcm_int/ltsw/xgs/lb_flex_hash.h>
#include <bcm_int/ltsw/issu.h>
#include "sub_dispatch.h"

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

/******************************************************************************
 * Private functions
 */

static int
bcm56990_a0_ltsw_lb_hash_flex_fields_sel_deinit(int unit)
{
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_fields_sel_deinit(unit));


    if (LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit) == NULL) {
        SHR_ERR_EXIT(SHR_E_NONE);
    }

    /* Cleanup LT info */
    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);
    if (lt_key_info->lt_key_data) {
        LB_HASH_MEM_FREE(lt_key_info->lt_key_data);
        lt_key_info->lt_key_data = NULL;
    }
    if (lt_key_info->lt_key_mask) {
        LB_HASH_MEM_FREE(lt_key_info->lt_key_mask);
        lt_key_info->lt_key_mask = NULL;
    }

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);
    if (lt_data_info->field_info) {
        LB_HASH_MEM_FREE(lt_data_info->field_info);
        lt_data_info->field_info = NULL;
    }

    LB_HASH_MEM_FREE(LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit));
    LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit) = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_fields_sel_init(int unit)
{
    int idx, j;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info;
    bcmi_lb_hash_flex_fields_select_info_t *field_info;
    bcmi_lb_hash_flex_udf_chunks_info_t *udf_chunks_info;

    const char *lt_key_field_list[][2] = {
        { UDF0_DATAs, UDF0_DATA_MASKs },
        { UDF1_DATAs, UDF1_DATA_MASKs },
        { UDF2_DATAs, UDF2_DATA_MASKs },
        { UDF3_DATAs, UDF3_DATA_MASKs }
    };
    const char *lt_data_field_list[][4] = {
        {BIN3_FIELD_SELECTs, BIN3_L4_OFFSETs,
            BIN3_UDF_CHUNKs, BIN3_FIELD_MASKs },
        {BIN2_FIELD_SELECTs, BIN2_L4_OFFSETs,
            BIN2_UDF_CHUNKs, BIN2_FIELD_MASKs }
    };

    int lt_data_array_fields = 11;
    const char *lt_data_array_field_list[] = {
        BIN_FIELD_SELECTs, BIN_UDF_CHUNKs, BIN_FIELD_MASKs,
    };

    SHR_FUNC_ENTER(unit);

    if (LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit) != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_a0_ltsw_lb_hash_flex_fields_sel_deinit(unit));
    }

    LB_HASH_MEM_ALLOC(LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit),
            sizeof (bcmi_lb_hash_flex_fields_sel_lt_info_t),
            "lbHashFlexFieldsSelLtInfo");
    SHR_NULL_CHECK(LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit), SHR_E_MEMORY);

    LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit)->sid =
        LB_HASH_FLEX_FIELDS_SELECTIONs;

    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);

    lt_key_info->fields = COUNTOF(lt_key_field_list);

    LB_HASH_MEM_ALLOC(lt_key_info->lt_key_data,
            sizeof (char *) * lt_key_info->fields,
            "lbHashFlexFieldsSelLtKeyInfo");
    SHR_NULL_CHECK(lt_key_info->lt_key_data, SHR_E_MEMORY);

    LB_HASH_MEM_ALLOC(lt_key_info->lt_key_mask,
            sizeof (char *) * lt_key_info->fields,
            "lbHashFlexFieldsSelLtKeyMaskInfo");
    SHR_NULL_CHECK(lt_key_info->lt_key_mask, SHR_E_MEMORY);

    for (idx = 0; idx < lt_key_info->fields; idx++) {
        lt_key_info->lt_key_data[idx] = lt_key_field_list[idx][0];
        lt_key_info->lt_key_mask[idx] = lt_key_field_list[idx][1];
    }

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

    lt_data_info->fields = COUNTOF(lt_data_field_list) +
        lt_data_array_fields;

    LB_HASH_MEM_ALLOC(lt_data_info->field_info,
            sizeof (bcmi_lb_hash_flex_fields_select_info_t) * \
            lt_data_info->fields, "lbHahsFlexFieldsSelLtFieldInfo");
    SHR_NULL_CHECK(lt_data_info->field_info, SHR_E_MEMORY);

    for (idx = 0; idx < COUNTOF(lt_data_field_list); idx++) {
        field_info = &lt_data_info->field_info[idx];

        sal_memset(field_info, 0,
                sizeof (bcmi_lb_hash_flex_fields_select_info_t));

        field_info->bin_field_extr  = lt_data_field_list[idx][0];
        field_info->bin_l4_offset   = lt_data_field_list[idx][1];
        field_info->bin_udf_offset  = lt_data_field_list[idx][2];
        field_info->bin_field_mask  = lt_data_field_list[idx][3];

        field_info->flags |=
            (BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_SYM |
             BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_L4_SUPPORT |
             BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_UDF_SUPPORT);
    }

    /* Array data fields */
    for (j = 0; j < lt_data_array_fields; j++, idx++) {
        field_info = &lt_data_info->field_info[idx];

        sal_memset(field_info, 0,
                sizeof (bcmi_lb_hash_flex_fields_select_info_t));

        field_info->index          = j;
        field_info->bin_field_extr = lt_data_array_field_list[0];
        field_info->bin_l4_offset  = NULL;
        field_info->bin_udf_offset = lt_data_array_field_list[1];
        field_info->bin_field_mask = lt_data_array_field_list[2];

        field_info->flags |=
            (BCMI_LB_HASH_FLEX_LT_DATA_FIELD_TYPE_ARRAY |
             BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_UDF_SUPPORT);
    }

    lt_data_info->mask_length = 2;

    /* UDF chunks info */
    udf_chunks_info = LB_HASH_FLEX_FIELDS_SEL_LT_UDF_CHUNKS_INFO(unit);
    udf_chunks_info->chunks_count = 4;
    udf_chunks_info->chunk_ids[0] = 12;
    udf_chunks_info->chunk_ids[1] = 13;
    udf_chunks_info->chunk_ids[2] = 14;
    udf_chunks_info->chunk_ids[3] = 15;

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_fields_sel_init(unit));

exit:

    if (SHR_FUNC_ERR()) {
        (void) xgs_ltsw_lb_hash_flex_fields_sel_deinit(unit);
    }

    SHR_FUNC_EXIT();
}




static int
bcm56990_a0_ltsw_lb_hash_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ltsw_lb_hash_flex_fields_sel_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_lb_hash_init(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ltsw_lb_hash_flex_fields_sel_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_os_port_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_os_port_control_set(unit, type, port, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_os_port_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_os_port_control_get(unit, type, port, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_os_flow_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_os_flow_control_set(unit, type, field, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_os_flow_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_os_flow_control_get(unit, type, field, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_fields_select_set(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_fields_select_set(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_fields_select_get(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_fields_select_get(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_bins_set(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_bins_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_bins_get(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_bins_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_ltsw_lb_hash_pkt_hdr_sel_set(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_pkt_hdr_sel_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_pkt_hdr_sel_get(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_pkt_hdr_sel_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_ip6_collapse_set(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_ip6_collapse_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_ip6_collapse_get(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_ip6_collapse_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_algorithm_set(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_algorithm_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_algorithm_get(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_algorithm_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_symmetric_control_set(
    int unit,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_symmetric_control_set(unit, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_symmetric_control_get(
    int unit,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_symmetric_control_get(unit, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_control_set(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_control_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_control_get(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_control_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_bins_flex_set(
    int unit,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_bins_flex_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_entry_alloc(
    int unit,
    int qual_count,
    int *qual_list,
    bcm_hash_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_alloc(unit, qual_count,
                                           qual_list, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_entry_free(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_free(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    int r,
    uint32 offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_install(unit, entry,
                                             r, offset));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_entry_remove(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_remove(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_set(unit, entry, config));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_get(unit, entry, config));

exit:
    SHR_FUNC_EXIT();
}

#if 0
static int
bcm56990_a0_ltsw_lb_hash_flex_entry_traverse(
    int unit,
    int flags,
    bcm_switch_hash_entry_traverse_cb cb_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_entry_traverse(unit, flags,
                                              cb_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}
#endif

static int
bcm56990_a0_ltsw_lb_hash_flex_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_qualify_udf(unit, entry,
                                           udf_id, length,
                                           data, mask));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_lb_hash_flex_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_flex_qualify_udf_get(unit, entry,
                                               udf_id, max_length,
                                               data, mask,
                                               actual_length));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_lb_hash_control_seed_set(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_control_seed_set(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_lb_hash_control_seed_get(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_control_seed_get(unit, control, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_lb_hash_macro_flow_seed_set(
    int unit,
    bcmi_ltsw_macro_flow_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_lb_hash_macro_flow_seed_set(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lb_hash driver function variable for bcm56990_a0 device.
 */
static mbcm_ltsw_lb_hash_drv_t bcm56990_a0_ltsw_lb_hash_drv = {
    .lb_hash_deinit = bcm56990_a0_ltsw_lb_hash_deinit,
    .lb_hash_init = bcm56990_a0_ltsw_lb_hash_init,
    .lb_hash_os_port_control_set = bcm56990_a0_ltsw_lb_hash_os_port_control_set,
    .lb_hash_os_port_control_get = bcm56990_a0_ltsw_lb_hash_os_port_control_get,
    .lb_hash_os_flow_control_set = bcm56990_a0_ltsw_lb_hash_os_flow_control_set,
    .lb_hash_os_flow_control_get = bcm56990_a0_ltsw_lb_hash_os_flow_control_get,
    .lb_hash_os_tbl_db_get = BCM56990_A0_SUB_DRV,
    .lb_hash_fields_select_set = bcm56990_a0_ltsw_lb_hash_fields_select_set,
    .lb_hash_fields_select_get = bcm56990_a0_ltsw_lb_hash_fields_select_get,
    .lb_hash_bins_set = bcm56990_a0_ltsw_lb_hash_bins_set,
    .lb_hash_bins_get = bcm56990_a0_ltsw_lb_hash_bins_get,
    .lb_hash_pkt_hdr_sel_set = bcm56990_a0_ltsw_lb_hash_pkt_hdr_sel_set,
    .lb_hash_pkt_hdr_sel_get = bcm56990_a0_ltsw_lb_hash_pkt_hdr_sel_get,
    .lb_hash_ip6_collapse_set = bcm56990_a0_ltsw_lb_hash_ip6_collapse_set,
    .lb_hash_ip6_collapse_get = bcm56990_a0_ltsw_lb_hash_ip6_collapse_get,
    .lb_hash_algorithm_set = bcm56990_a0_ltsw_lb_hash_algorithm_set,
    .lb_hash_algorithm_get = bcm56990_a0_ltsw_lb_hash_algorithm_get,
    .lb_hash_symmetric_control_set = bcm56990_a0_ltsw_lb_hash_symmetric_control_set,
    .lb_hash_symmetric_control_get = bcm56990_a0_ltsw_lb_hash_symmetric_control_get,
    .lb_hash_control_set = bcm56990_a0_ltsw_lb_hash_control_set,
    .lb_hash_control_get = bcm56990_a0_ltsw_lb_hash_control_get,
    .lb_hash_bins_flex_set = bcm56990_a0_ltsw_lb_hash_bins_flex_set,
    .lb_hash_flex_entry_alloc = bcm56990_a0_ltsw_lb_hash_flex_entry_alloc,
    .lb_hash_flex_entry_free = bcm56990_a0_ltsw_lb_hash_flex_entry_free,
    .lb_hash_flex_entry_set = bcm56990_a0_ltsw_lb_hash_flex_entry_set,
    .lb_hash_flex_entry_get = bcm56990_a0_ltsw_lb_hash_flex_entry_get,
    .lb_hash_flex_qualify_udf = bcm56990_a0_ltsw_lb_hash_flex_qualify_udf,
    .lb_hash_flex_qualify_udf_get = bcm56990_a0_ltsw_lb_hash_flex_qualify_udf_get,
    .lb_hash_flex_entry_install = bcm56990_a0_ltsw_lb_hash_flex_entry_install,
    .lb_hash_flex_entry_remove = bcm56990_a0_ltsw_lb_hash_flex_entry_remove,
    .lb_hash_versatile_control_set = BCM56990_A0_SUB_DRV,
    .lb_hash_versatile_control_get = BCM56990_A0_SUB_DRV,
    .lb_hash_control_seed_set = bcm56990_a0_ltsw_lb_hash_control_seed_set,
    .lb_hash_control_seed_get = bcm56990_a0_ltsw_lb_hash_control_seed_get,
    .lb_hash_macro_flow_seed_set = bcm56990_a0_ltsw_lb_hash_macro_flow_seed_set,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_ltsw_lb_hash_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv_set(unit, &bcm56990_a0_ltsw_lb_hash_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_sub_drv_attach(unit, &bcm56990_a0_ltsw_lb_hash_drv,
                                    BCM56990_A0_SUB_DRV_BCM56990_Ax,
                                    BCM56990_A0_SUB_MOD_LB_HASH));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach functions for other members.
 */
#define BCM56990_A0_DRV_ATTACH_ENTRY(_dn,_vn,_pf,_pd,_r0) \
int _vn##_ltsw_lb_hash_drv_attach(int unit) \
{ \
    SHR_FUNC_ENTER(unit); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (mbcm_ltsw_lb_hash_drv_set(unit, &bcm56990_a0_ltsw_lb_hash_drv)); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (bcm56990_a0_sub_drv_attach(unit, &bcm56990_a0_ltsw_lb_hash_drv, \
                                    BCM56990_A0_SUB_DRV_BCM5699x, \
                                    BCM56990_A0_SUB_MOD_LB_HASH)); \
exit: \
    SHR_FUNC_EXIT(); \
}
#include "sub_devlist.h"
