/*! \file lb_flex_hash.c
 *
 * XGS LB hash management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm/switch.h>
#include <bcm_int/ltsw/xgs/lb_hash.h>
#include <bcm_int/ltsw/xgs/lb_flex_hash.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/lb_hash.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_lb_hash_constants.h>

#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/xgs/udf.h>
#include <shr/shr_util_pack.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

/* Unpack to variable */
#define LB_HASH_FLEX_DATA_UNPACK(buf, data, length)             \
    do {                                                        \
        if (length == 1) {                                      \
            SHR_UTIL_UNPACK_U8(buf, data);                      \
        } else if (length == 2) {                               \
            SHR_UTIL_UNPACK_U16(buf, data);                     \
        }                                                       \
    } while (0)

/* Pack to buffer from variable */
#define LB_HASH_FLEX_DATA_PACK(buf, data, length)                   \
    do {                                                        \
        if (length == 1) {                                      \
            SHR_UTIL_PACK_U8(buf, data);                        \
        } else if (length == 2) {                               \
            SHR_UTIL_PACK_U16(buf, data);                       \
        }                                                       \
    } while (0)


/* LB Hash Flex fields sel LT info */
bcmi_lb_hash_flex_fields_sel_lt_info_t
            *xgs_lb_hash_flex_fields_sel_lt_info[BCM_MAX_NUM_UNITS];

/* LB Hash flex fields select control */
bcmi_lb_hash_flex_fields_sel_ctrl_t
            *xgs_lb_hash_flex_fields_sel_ctrl[BCM_MAX_NUM_UNITS];
/******************************************************************************
 * Private functions
 */

static int
ltsw_lb_hash_flex_ha_deinit(int unit)
{
    bcmi_lb_hash_flex_fields_sel_ctrl_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    ctrl = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit);
    if (ctrl) {
        if (ctrl->entry_md_array != NULL) {
            (void) bcmi_ltsw_ha_mem_free(unit, ctrl->entry_md_array);
            ctrl->entry_md_array = NULL;
        }
        if (ctrl->key_info_array != NULL) {
            (void) bcmi_ltsw_ha_mem_free(unit, ctrl->key_info_array);
            ctrl->key_info_array = NULL;
        }
        if (ctrl->data_info_array != NULL) {
            (void) bcmi_ltsw_ha_mem_free(unit, ctrl->data_info_array);
            ctrl->data_info_array = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_ha_init(int unit, int warm)
{
    int rv = SHR_E_NONE;
    int count_per_entry = 0;
    uint32 ha_alloc_size = 0, ha_req_size = 0;
    uint32 max_entries = 0, ha_instance_size = 0;
    bcmi_lb_hash_flex_fields_sel_ctrl_t *ctrl = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);

    ctrl = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit);
    max_entries = ctrl->max_entries;

    /* Allocate HA memory for Entry MD */
    ha_instance_size = sizeof (bcmi_lb_hash_flex_fields_sel_entry_md_t);
    ha_req_size = ha_alloc_size = ha_instance_size * max_entries;

    ctrl->entry_md_array =
        (bcmi_lb_hash_flex_fields_sel_entry_md_t *) bcmi_ltsw_ha_mem_alloc(
                unit,
                BCMI_HA_COMP_ID_LB_HASH,
                BCMI_LB_HASH_FLEX_ENTRY_MD_SUB_COMP_ID,
                "xgsLbHashFlexFieldEntryMd",
                &ha_alloc_size);
    SHR_NULL_CHECK(ctrl->entry_md_array, SHR_E_MEMORY);
    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    if (!warm) {
        sal_memset(ctrl->entry_md_array, 0, ha_alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
            BCMI_HA_COMP_ID_LB_HASH,
            BCMI_LB_HASH_FLEX_ENTRY_MD_SUB_COMP_ID,
            0, ha_instance_size, max_entries,
            BCMI_LB_HASH_FLEX_FIELDS_SEL_ENTRY_MD_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Allocate HA memory for Key info */
    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);
    count_per_entry = lt_key_info->fields;
    ha_instance_size = sizeof (bcmi_lb_hash_flex_fields_sel_entry_key_info_t);
    ha_req_size = ha_instance_size * max_entries * count_per_entry;
    ha_alloc_size = ha_req_size;

    ctrl->key_info_array =
        (bcmi_lb_hash_flex_fields_sel_entry_key_info_t *) bcmi_ltsw_ha_mem_alloc(
                unit,
                BCMI_HA_COMP_ID_LB_HASH,
                BCMI_LB_HASH_FLEX_ENTRY_KEY_SUB_COMP_ID,
                "xgsLbHashFlexFieldEntryKeyInfo",
                &ha_alloc_size);
    SHR_NULL_CHECK(ctrl->key_info_array, SHR_E_MEMORY);
    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        sal_memset(ctrl->key_info_array, 0, ha_alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
            BCMI_HA_COMP_ID_LB_HASH,
            BCMI_LB_HASH_FLEX_ENTRY_KEY_SUB_COMP_ID,
            0, ha_instance_size, max_entries * count_per_entry,
            BCMI_LB_HASH_FLEX_FIELDS_SEL_ENTRY_KEY_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Allocate HA memory for Data info */
    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);
    count_per_entry = lt_data_info->fields;
    ha_instance_size = sizeof (bcmi_lb_hash_flex_fields_sel_entry_data_info_t);
    ha_req_size = ha_instance_size * max_entries * count_per_entry;
    ha_alloc_size = ha_req_size;

    ctrl->data_info_array =
        (bcmi_lb_hash_flex_fields_sel_entry_data_info_t *) bcmi_ltsw_ha_mem_alloc(
                unit,
                BCMI_HA_COMP_ID_LB_HASH,
                BCMI_LB_HASH_FLEX_ENTRY_DATA_SUB_COMP_ID,
                "xgsLbHashFlexFieldEntryDataInfo",
                &ha_alloc_size);
    SHR_NULL_CHECK(ctrl->data_info_array, SHR_E_MEMORY);
    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        sal_memset(ctrl->data_info_array, 0, ha_alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
            BCMI_HA_COMP_ID_LB_HASH,
            BCMI_LB_HASH_FLEX_ENTRY_DATA_SUB_COMP_ID,
            0, ha_instance_size, max_entries * count_per_entry,
            BCMI_LB_HASH_FLEX_FIELDS_SEL_ENTRY_DATA_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:

    if (SHR_FUNC_ERR()) {
        (void) ltsw_lb_hash_flex_ha_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_entry_deinit(int unit)
{
    bcmi_lb_hash_flex_fields_sel_ctrl_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    ctrl = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit);
    if (ctrl) {
        if (ctrl->hash_entry_array != NULL) {
            LB_HASH_MEM_FREE(ctrl->hash_entry_array);
            ctrl->hash_entry_array = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_entry_init(int unit)
{
    int idx = 0, j = 0;
    uint32 max_entries = 0;
    bcmi_lb_hash_flex_fields_sel_ctrl_t *ctrl = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);

    ctrl = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit);
    max_entries = ctrl->max_entries;

    LB_HASH_MEM_ALLOC(ctrl->hash_entry_array,
            sizeof (bcmi_lb_hash_flex_fields_sel_entry_t) * max_entries,
            "xgsLbHashFlexFieldsSelEntry");
    SHR_NULL_CHECK(ctrl->hash_entry_array, SHR_E_MEMORY);

    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);
    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

    for (idx = 0; idx < max_entries; idx++) {
        entry_p = &ctrl->hash_entry_array[idx];

        entry_p->md = &ctrl->entry_md_array[idx];

        j = idx * lt_key_info->fields;
        entry_p->key_info = &ctrl->key_info_array[j];

        j = idx * lt_data_info->fields;
        entry_p->data_info = &ctrl->data_info_array[j];
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void) ltsw_lb_hash_flex_fields_sel_entry_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_ctrl_recover(int unit)
{
    int idx = 0;
    uint8 max_entries = 0;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *hash_entry_array = NULL;

    SHR_FUNC_ENTER(unit);

    max_entries = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->max_entries;
    hash_entry_array = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->hash_entry_array;

    LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->num_installed = 0;

    for (idx = 0; idx < max_entries; idx++) {
        entry_p = &hash_entry_array[idx];

        if (entry_p->md->flags & BCMI_LB_HASH_FLEX_ENTRY_INSTALLED) {
            LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->num_installed += 1;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_entry_idx_alloc(
    int unit,
    bcm_hash_entry_t *entry)
{
    int idx = 0;
    uint8 max_entries = 0;
    bcmi_lb_hash_flex_fields_sel_entry_t *temp = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *hash_entry_array = NULL;

    SHR_FUNC_ENTER(unit);

    max_entries = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->max_entries;
    hash_entry_array = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->hash_entry_array;

    for (idx = 0; idx < max_entries; idx++) {
        temp = &hash_entry_array[idx];

        if (!(temp->md->flags & BCMI_LB_HASH_FLEX_ENTRY_IN_USE)) {
            break;
        }
    }
    if (idx >= max_entries) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    temp->md->flags |= BCMI_LB_HASH_FLEX_ENTRY_IN_USE;
    temp->md->entry_id = idx;
    *entry = idx;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcmi_lb_hash_flex_fields_sel_entry_t **entry_p)
{
    bcmi_lb_hash_flex_fields_sel_entry_t *temp = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *hash_entry_array = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry_p, SHR_E_PARAM);

    hash_entry_array = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->hash_entry_array;

    temp = &hash_entry_array[entry];
    if (!(temp->md->flags & BCMI_LB_HASH_FLEX_ENTRY_IN_USE)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *entry_p = temp;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_entry_idx_free(
    int unit,
    bcm_hash_entry_t entry)
{
    bcmi_lb_hash_flex_fields_sel_entry_t *temp = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *hash_entry_array = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);

    hash_entry_array = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->hash_entry_array;

    temp = &hash_entry_array[entry];

    if (!(temp->md->flags & BCMI_LB_HASH_FLEX_ENTRY_IN_USE)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memset(temp->md, 0,
            sizeof (bcmi_lb_hash_flex_fields_sel_entry_md_t));

    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);
    sal_memset(temp->key_info, 0, lt_key_info->fields * \
            sizeof(bcmi_lb_hash_flex_fields_sel_entry_key_info_t));

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);
    sal_memset(temp->data_info, 0, lt_data_info->fields * \
            sizeof(bcmi_lb_hash_flex_fields_sel_entry_data_info_t));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_entry_free_offset_get(
    int unit,
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p,
    int *free_idx)
{
    int idx = 0;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry_p, SHR_E_PARAM);
    SHR_NULL_CHECK(free_idx, SHR_E_PARAM);

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

    for (idx = 0; idx < lt_data_info->fields; idx++) {
        if (entry_p->data_info[idx].extr ==
                BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED) {
            *free_idx = idx;
            break;
        }
    }

    if (idx >= lt_data_info->fields) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_lt_key_fields_add(
    int unit,
    bcmlt_entry_handle_t *entry_hdl,
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p)
{
    int idx = 0;
    const char *field;
    uint64_t u64data, u64mask;
    bcmlt_field_def_t fdef;
    bcmi_lb_hash_flex_fields_sel_lt_info_t *lt_info = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry_p, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_hdl, SHR_E_PARAM);

    lt_info = LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit);
    lt_key_info = &(lt_info->lt_key_info);

    for (idx = 0; idx < lt_key_info->fields; idx++) {

        if (entry_p->key_info[idx].mask != 0)  {
            u64data = entry_p->key_info[idx].data;
            u64mask = entry_p->key_info[idx].mask;
        } else {
            field = lt_key_info->lt_key_data[idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_info->sid, field, &fdef));
            u64data = fdef.def;

            field = lt_key_info->lt_key_mask[idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_info->sid, field, &fdef));
            u64mask = fdef.def;
        }

        field = lt_key_info->lt_key_data[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, field, u64data));

        field = lt_key_info->lt_key_mask[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, field, u64mask));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_lt_data_fields_add(
    int unit,
    bcmlt_entry_handle_t *entry_hdl,
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p)
{
    int idx = 0;
    const char *field_extr_type_str;
    uint64_t field_sel, offset, data_mask;
    bcmi_lb_hash_flex_fields_select_info_t *field_info = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_p, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_hdl, SHR_E_PARAM);

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

    for (idx = 0; idx < lt_data_info->fields; idx++) {

        field_info = &lt_data_info->field_info[idx];

        if (field_info->flags &
                BCMI_LB_HASH_FLEX_LT_DATA_FIELD_TYPE_ARRAY) {

            /* Field Extr */
            if (entry_p->data_info[idx].extr ==
                    BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_UDF) {
                field_sel = 1;
                offset = entry_p->data_info[idx].offset;
                data_mask = entry_p->data_info[idx].mask;
            } else if (entry_p->data_info[idx].extr ==
                    BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED) {
                field_sel = 0;
                offset = 0;
                data_mask = 0;
            } else {
                /* Skip if L4_DATA set */
                continue;
            }
            /* Selector field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(*entry_hdl,
                                             field_info->bin_field_extr,
                                             field_info->index,
                                             &field_sel, 1));

            /*  Offset field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(*entry_hdl,
                                             field_info->bin_udf_offset,
                                             field_info->index,
                                             &offset, 1));

            /* Mask field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(*entry_hdl,
                                             field_info->bin_field_mask,
                                             field_info->index,
                                             &data_mask, 1));
        } else {

            /* Field Extr */
            if (field_info->flags &
                    BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_SYM) {
                if (entry_p->data_info[idx].extr ==
                        BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_UDF) {
                    field_extr_type_str = UDFs;
                } else { /* L4 or disabled */
                    field_extr_type_str = L4_DATAs;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(*entry_hdl,
                                                  field_info->bin_field_extr,
                                                  field_extr_type_str));
            }

            if (entry_p->data_info[idx].extr ==
                    BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED) {
                offset = 0;
                data_mask = 0;
            } else {
                offset = entry_p->data_info[idx].offset;
                data_mask = entry_p->data_info[idx].mask;
            }

            /*  Offset field */
            if (entry_p->data_info[idx].extr ==
                    BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_UDF) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(*entry_hdl,
                                           field_info->bin_udf_offset,
                                           offset));
            } else { /* L4 or disabled */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(*entry_hdl,
                                           field_info->bin_l4_offset,
                                           offset));
            }

            /* Mask field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(*entry_hdl,
                                       field_info->bin_field_mask,
                                       data_mask));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_lb_hash_flex_fields_sel_lt_entry_commit(
    int unit,
    bcmlt_opcode_t opcode,
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p)
{
    int dunit;
    bcmlt_entry_info_t entry_info;
    bcmlt_entry_handle_t entry_hdl;
    bcmi_lb_hash_flex_fields_sel_lt_info_t *lt_info = NULL;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    lt_info = LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->sid, &entry_hdl));

    /* Set Priority */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ENTRY_PRIORITYs, 0));

    switch(opcode) {
        case BCMLT_OPCODE_INSERT:
        case BCMLT_OPCODE_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_lb_hash_flex_fields_sel_lt_key_fields_add(unit,
                                                                &entry_hdl,
                                                                entry_p));

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_lb_hash_flex_fields_sel_lt_data_fields_add(unit,
                                                                 &entry_hdl,
                                                                 entry_p));
            break;
        case BCMLT_OPCODE_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_lb_hash_flex_fields_sel_lt_key_fields_add(unit,
                                                                &entry_hdl,
                                                                entry_p));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Commit */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, opcode, BCMLT_PRIORITY_NORMAL));

    /* Get commit status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_info_get(entry_hdl, &entry_info));

    SHR_ERR_EXIT(entry_info.status);

exit:

    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

int
xgs_ltsw_lb_hash_flex_fields_sel_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_deinit(unit));

    if (LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)) {
        LB_HASH_MEM_FREE(LB_HASH_FLEX_FIELDS_SEL_CTRL(unit));
        LB_HASH_FLEX_FIELDS_SEL_CTRL(unit) = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_fields_sel_init(int unit)
{
    int warm = 0;
    uint32 max_entries = 0;
    bool ha_init = false, entry_init = false;
    bcmi_lb_hash_flex_fields_sel_ctrl_t *ctrl = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_info_t *lt_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)) {
        (void) xgs_ltsw_lb_hash_flex_fields_sel_deinit(unit);
    }

    LB_HASH_MEM_ALLOC(LB_HASH_FLEX_FIELDS_SEL_CTRL(unit),
            sizeof (bcmi_lb_hash_flex_fields_sel_ctrl_t),
            "xgsLbHashFlexFieldsSelCtrl");
    SHR_NULL_CHECK(LB_HASH_FLEX_FIELDS_SEL_CTRL(unit), SHR_E_MEMORY);

    warm = bcmi_warmboot_get(unit);
    ctrl = LB_HASH_FLEX_FIELDS_SEL_CTRL(unit);
    lt_info = LB_HASH_FLEX_FIELDS_SEL_LT_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, lt_info->sid, &max_entries));

    ctrl->min_id = 0;
    ctrl->max_id = (max_entries - 1);
    ctrl->max_entries = max_entries;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_ha_init(unit, warm));
    ha_init = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_init(unit));
    entry_init = true;

    if (warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_lb_hash_flex_fields_sel_ctrl_recover(unit));
    }

exit:

    if (SHR_FUNC_ERR()) {
        if (entry_init == true) {
            (void) ltsw_lb_hash_flex_fields_sel_entry_deinit(unit);
        }
        if (ha_init == true) {
            (void) ltsw_lb_hash_flex_ha_deinit(unit);
        }

        if (LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)) {
            LB_HASH_MEM_FREE(LB_HASH_FLEX_FIELDS_SEL_CTRL(unit));
            LB_HASH_FLEX_FIELDS_SEL_CTRL(unit) = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_entry_alloc(
    int unit,
    int qual_count,
    int *qual_array,
    bcm_hash_entry_t *entry)
{
    int idx = 0, j = 0;
    int chunk_id = 0;
    bool entry_idx_alloc = false;
    uint32 chunk_bmap = 0;
    bcm_udf_id_t udf_id = 0;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmint_udf_offset_info_t *offset_info = NULL;
    bcmi_lb_hash_flex_udf_chunks_info_t *udf_chunks_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_idx_alloc(unit, entry));
    entry_idx_alloc = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, *entry, &entry_p));

    udf_chunks_info = LB_HASH_FLEX_FIELDS_SEL_LT_UDF_CHUNKS_INFO(unit);

    for (idx = 0; idx < qual_count; idx++) {
        udf_id = qual_array[idx];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_obj_offset_info_get(unit, udf_id, &offset_info));

        if (!(offset_info->flags & BCMI_UDF_OFFSET_INFO_FLEXHASH)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        XGS_UDF_OBJ_OFFSET_HW_BMAP_GET(unit, offset_info, chunk_bmap);

        for (j = 0; j < udf_chunks_info->chunks_count; j++) {
            chunk_id = udf_chunks_info->chunk_ids[j];

            if (chunk_bmap & (1 << chunk_id)) {
                entry_p->key_info[j].udf_id = udf_id;
            }
        }
    }

exit:

    if (SHR_FUNC_ERR()) {
        if (entry_idx_alloc == true) {
            (void) ltsw_lb_hash_flex_fields_sel_entry_idx_free(unit, *entry);
        }
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_entry_free(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_idx_free(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    uint8 extr = 0, offset = 0, gran = 0;
    uint8 chunk_id = 0, chunk_pos = 0, pos = 0;
    uint8 *mask_ptr = NULL;
    uint16 data_mask = 0;
    uint32 chunk_bmap = 0;
    bcm_udf_id_t udf_id = 0;
    bcmint_udf_offset_info_t *offset_info = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_select_info_t *field_info = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(config, SHR_E_PARAM);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

    if (config->field >= lt_data_info->fields) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field_info = &lt_data_info->field_info[config->field];

    if (config->flags & BCM_SWITCH_HASH_FIELD_TYPE_UDF) {
        if (!(field_info->flags &
                    BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_UDF_SUPPORT)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        if (!(field_info->flags &
                    BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_L4_SUPPORT)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    if (config->mask_length > lt_data_info->mask_length) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mask_ptr = config->mask;
    LB_HASH_FLEX_DATA_UNPACK(mask_ptr, data_mask, config->mask_length);

    if (data_mask == 0) {
        extr = BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED;
        udf_id = 0;
        offset = 0;
    } else if (config->flags & BCM_SWITCH_HASH_FIELD_TYPE_UDF) {

        extr =  BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_UDF;
        udf_id = config->udf_id;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_obj_offset_info_get(unit, udf_id, &offset_info));

        gran = XGS_UDF_CHUNK_GRAN(unit);

        if ((offset_info->width < (config->offset / 8) + gran) ||
                (offset_info->start + (config->offset / 8)) % gran) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        XGS_UDF_OBJ_OFFSET_HW_BMAP_GET(unit, offset_info, chunk_bmap);

        /*get the chunk id which the offset points to*/
        chunk_pos = 0;
        pos = (offset_info->start % gran) + ((config->offset / 8)) / gran;
        for (chunk_id = 0, chunk_pos = 0;
                chunk_id < BCMINT_UDF_MAX_CHUNKS(unit); chunk_id++) {

            if (chunk_bmap & (1 << chunk_id)) {
                if (chunk_pos == pos) {
                    break;
                }
                chunk_pos++;
            }
        }
        offset = chunk_id;
    } else {
        extr = BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_L4;
        udf_id = 0;
        offset = config->offset & 0xf;
    }

    entry_p->data_info[config->field].extr = extr;
    entry_p->data_info[config->field].udf_id = udf_id;
    entry_p->data_info[config->field].offset = offset;
    entry_p->data_info[config->field].mask = data_mask;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    int idx = 0, mask_length = 0;
    uint8 offset = 0, gran = 0;
    uint8 chunk_id = 0, chunk_pos = 0;
    uint8 *mask_ptr = NULL;
    uint16 data_mask = 0;
    uint32 flags = 0, chunk_bmap = 0;
    bcm_udf_id_t udf_id = 0;
    bcmint_udf_offset_info_t *offset_info = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(config, SHR_E_PARAM);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

    idx = config->field;
    if (idx >= lt_data_info->fields) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    data_mask = entry_p->data_info[idx].mask;

    if ((data_mask == 0) || (entry_p->data_info[idx].extr ==
                BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else if (entry_p->data_info[idx].extr ==
            BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_L4) {
        flags = 0;
        udf_id = 0;
        offset = entry_p->data_info[config->field].offset;
    } else { /* BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_UDF */

        flags = BCM_SWITCH_HASH_FIELD_TYPE_UDF;
        udf_id = entry_p->data_info[idx].udf_id;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_obj_offset_info_get(unit, udf_id, &offset_info));

        gran = XGS_UDF_CHUNK_GRAN(unit);

        XGS_UDF_OBJ_OFFSET_HW_BMAP_GET(unit, offset_info, chunk_bmap);

        for (chunk_id = 0, chunk_pos = 0;
                chunk_id < entry_p->data_info[idx].offset;
                chunk_id++) {
            if (SHR_BITGET(&chunk_bmap, chunk_id)) {
                chunk_pos++;
            }
        }
        offset = (chunk_pos * gran - offset_info->start % gran) * 8;
    }

    mask_length = ((data_mask >> 8) & 0xFF) ? 2 : 1;

    config->flags = flags;
    config->offset = offset;
    config->mask_length = mask_length;
    config->udf_id = udf_id;

    mask_ptr = config->mask;
    LB_HASH_FLEX_DATA_PACK(mask_ptr, data_mask, mask_length);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    int idx = 0, j = 0;
    int key_len = 0, key_idx = 0;
    int chunk_cnt = 0;
    uint8 gran = 0, *ptr = NULL;
    uint16 qual_data = 0, qual_mask = 0;
    bcmint_udf_offset_info_t *offset_info = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    if (entry_p->md->flags & BCMI_LB_HASH_FLEX_ENTRY_INSTALLED) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_udf_obj_offset_info_get(unit, udf_id, &offset_info));

    gran = XGS_UDF_CHUNK_GRAN(unit);

    if (!(offset_info->flags & BCMI_UDF_OFFSET_INFO_FLEXHASH)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (length > offset_info->width) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < lt_key_info->fields; idx++) {
        if (entry_p->key_info[idx].udf_id == udf_id) {
            break;
        }
    }
    chunk_cnt  = (offset_info->width + gran - 1) / gran;
    if ((idx + chunk_cnt) > lt_key_info->fields) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (j = 0; j < length; j += gran) {
        key_idx = idx + (j / gran);
        key_len = ((length - j) >= gran) ? gran : (length - j);

        qual_data = qual_mask = 0;
        ptr = &data[j];
        LB_HASH_FLEX_DATA_UNPACK(ptr, qual_data, key_len);
        ptr = &mask[j];
        LB_HASH_FLEX_DATA_UNPACK(ptr, qual_mask, key_len);

        entry_p->key_info[key_idx].data = qual_data;
        entry_p->key_info[key_idx].mask = qual_mask;
        entry_p->key_info[key_idx].length = key_len;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    int idx = 0, j = 0;
    int key_len = 0, key_idx = 0;
    int chunk_cnt = 0;
    uint8 gran = 0;
    uint8 *ptr_data = NULL, *ptr_mask = NULL;
    uint16 qual_data = 0, qual_mask = 0;
    bcmint_udf_offset_info_t *offset_info = NULL;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t *lt_key_info = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);
    SHR_NULL_CHECK(actual_length, SHR_E_PARAM);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    lt_key_info = LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_udf_obj_offset_info_get(unit, udf_id, &offset_info));

    gran = XGS_UDF_CHUNK_GRAN(unit);

    if (!(offset_info->flags & BCMI_UDF_OFFSET_INFO_FLEXHASH)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (max_length < offset_info->width) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < lt_key_info->fields; idx++) {
        if (entry_p->key_info[idx].udf_id == udf_id) {
            break;
        }
    }
    if (idx >= lt_key_info->fields) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    chunk_cnt  = (offset_info->width + gran - 1) / gran;
    if ((idx + chunk_cnt) > lt_key_info->fields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ptr_data = data;
    ptr_mask = mask;
    *actual_length = 0;
    for (j = 0; j < offset_info->width; j += gran) {
        key_idx = idx + (j / gran);
        key_len = entry_p->key_info[key_idx].length;

        if (key_len != 0) {
            qual_data = entry_p->key_info[key_idx].data;
            qual_mask = entry_p->key_info[key_idx].mask;

            LB_HASH_FLEX_DATA_PACK(ptr_data, qual_data, key_len);
            LB_HASH_FLEX_DATA_PACK(ptr_mask, qual_mask, key_len);

            *actual_length += key_len;
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_lb_hash_flex_entry_update(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset)
{
    int idx = 0;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t *lt_data_info = NULL;

    SHR_FUNC_ENTER(unit);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    if (offset != BCM_SWITCH_HASH_USE_CONFIG) {

        lt_data_info = LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(unit);

        for (idx = 0; idx < lt_data_info->fields; idx++) {
            if (entry_p->data_info[idx].extr !=
                    BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED) {
                SHR_EXIT();
            }
        }

        /* Reset all indices to default */
        for (idx = 0; idx < lt_data_info->fields; idx++) {
            entry_p->data_info[idx].extr =
                BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    int r,
    uint32 offset)
{
    int free_idx = 0;
    bcmlt_opcode_t opcode = BCMLT_OPCODE_NOP;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;

    SHR_FUNC_ENTER(unit);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    if (r) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_lb_hash_flex_entry_update(unit, entry, offset));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    if (offset != BCM_SWITCH_HASH_USE_CONFIG) {
        SHR_IF_ERR_EXIT
            (ltsw_lb_hash_flex_fields_sel_entry_free_offset_get(unit,
                                                                entry_p,
                                                                &free_idx));

        entry_p->data_info[free_idx].extr =
            BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_L4;
        entry_p->data_info[free_idx].offset = offset & 0xF;
        entry_p->data_info[free_idx].mask = 0xFFFF;
    }

    opcode = BCMLT_OPCODE_INSERT;
    if (entry_p->md->flags & BCMI_LB_HASH_FLEX_ENTRY_INSTALLED) {
        opcode = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_lt_entry_commit(unit, opcode, entry_p));

    entry_p->md->flags |= BCMI_LB_HASH_FLEX_ENTRY_INSTALLED;

    if (opcode == BCMLT_OPCODE_INSERT) {

        LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->num_installed++;

        /* Enable hash bins */
        if (LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->num_installed == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_bins_flex_set(unit, 1));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_control_set(unit,
                    bcmiHashControlFlexHashLookupStatus, 1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_flex_entry_remove(
    int unit,
    bcm_hash_entry_t entry)
{
    bcmlt_opcode_t opcode = BCMLT_OPCODE_NOP;
    bcmi_lb_hash_flex_fields_sel_entry_t *entry_p = NULL;

    SHR_FUNC_ENTER(unit);
    LB_HASH_FLEX_ENTRY_ID_VALIDATE(unit, entry);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_lb_hash_flex_fields_sel_entry_get(unit, entry, &entry_p));

    if (entry_p->md->flags & BCMI_LB_HASH_FLEX_ENTRY_INSTALLED) {
        opcode = BCMLT_OPCODE_DELETE;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_lb_hash_flex_fields_sel_lt_entry_commit(unit, opcode, entry_p));

        LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->num_installed--;

        /* Disable hash bins */
        if (LB_HASH_FLEX_FIELDS_SEL_CTRL(unit)->num_installed == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_bins_flex_set(unit, 0));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_control_set(unit,
                    bcmiHashControlFlexHashLookupStatus, 0));
        }
        entry_p->md->flags &= ~BCMI_LB_HASH_FLEX_ENTRY_INSTALLED;
    }

exit:
    SHR_FUNC_EXIT();
}

