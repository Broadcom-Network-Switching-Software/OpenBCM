/*! \file bcmltm_db_info_dump.c
 *
 * Logical Table Manager - Logical Table Database Information Dump.
 *
 * This file contains routines to display the internal database for
 * logical tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_db_internal.h>

#include "bcmltm_db_info.h"

/*******************************************************************************
 * Local definitions
 */

/* Buffer size for prefix string */
#define PREFIX_STRING_SIZE         30

/* Buffer size for field prefix string */
#define FIELD_PREFIX_STRING_SIZE   80

/* Display for working buffer coordinates */
#define WBC_DUMP(_wbc, _pb)                                      \
    shr_pb_printf(pb, "wbc={ofs=%-5d min=%-5d max=%-5d}",        \
                  _wbc.buffer_offset, _wbc.minbit, _wbc.maxbit)


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Display given list of uint32_t values.
 *
 * This routine displays the given array of uint32_t values.
 *
 * \param [in] count Number of elements in array.
 * \param [in] value Array to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
array_dump(uint32_t count, const uint32_t *value, const char *prefix,
           shr_pb_t *pb)
{
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    if (count == 0) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    for (i = 0; i < count; i++) {
        if (i == 0) {
            shr_pb_printf(pb, "%s", prefix);
        } else {
            if ((i % 4) == 0) {
                shr_pb_printf(pb, "\n%s", prefix);
            } else {
                shr_pb_printf(pb, " ");
            }
        }
        shr_pb_printf(pb, "[%3d]=%-8d", i, value[i]);
    }

    shr_pb_printf(pb, "\n");

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display table attributes structure.
 *
 * This routine displays given table attributes structure.
 *
 * \param [in] unit Unit number.
 * \param [in] attr Table attribute to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
table_attr_dump(int unit,
                const bcmltm_table_attr_t *attr,
                shr_pb_t *pb)
{
    char *str;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Table Attributes ---\n");

    if (attr == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Table Type */
    if (BCMLTM_TABLE_TYPE_IS_SIMPLE_INDEX(attr->type)) {
        str = "Simple Index";
    } else if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type)) {
        str = "Index with Allocation";
    } else if (BCMLTM_TABLE_TYPE_IS_HASH(attr->type)) {
        str = "Hash";
    } else if (BCMLTM_TABLE_TYPE_IS_TCAM(attr->type)) {
        str = "TCAM";
    } else if (BCMLTM_TABLE_TYPE_IS_CONFIG(attr->type)) {
        str = "Config";
    } else {
        str = "None";
    }
    shr_pb_printf(pb, "Table Type: %s (%d)\n", str, attr->type);

    /* Table Map */
    if (BCMLTM_TABLE_MAP_IS_DM(attr->map_type)) {
        str = "Direct";
    } else if (BCMLTM_TABLE_MAP_IS_CTH(attr->map_type)) {
        str = "Custom Table Handler";
    } else if (BCMLTM_TABLE_MAP_IS_LTM(attr->map_type)) {
        str = "LTM";
    } else {
        str = "None";
    }
    shr_pb_printf(pb, "Table Map: %s (%d)\n", str, attr->map_type);

    /* Operating Mode */
    if (attr->mode == BCMLTM_TABLE_MODE_INTERACTIVE) {
        str = "Interactive";
    } else {
        str = "Modeled";
    }
    shr_pb_printf(pb, "Operating Mode: %s (%d)\n", str, attr->mode);

    /* Table flags */
    shr_pb_printf(pb, "Flags: 0x%x\n", attr->flag);
    if (attr->flag & BCMLTM_TABLE_FLAG_READ_ONLY) {
        shr_pb_printf(pb, "  Read only\n");
    }
    if (attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        shr_pb_printf(pb, "  Overlay\n");
    }

    /* Table limits */
    shr_pb_printf(pb, "Index: min=%d max=%d size=%d\n",
                  attr->index_min, attr->index_max, attr->index_size);
    shr_pb_printf(pb, "Memory Parameter: min=%d max=%d size=%d\n",
                  attr->mem_param_min, attr->mem_param_max,
                  attr->mem_param_size);

    /* Global LT ID (for overlay) */
    shr_pb_printf(pb, "Global LT ID: %d",
            attr->global_ltid);
    if (attr->flag & BCMLTM_TABLE_FLAG_OVERLAY) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, attr->global_ltid);
        shr_pb_printf(pb, " %s", table_name);
    }
    shr_pb_printf(pb, "\n");

    /* Opcodes */
    shr_pb_printf(pb, "Opcodes:");
    if (attr->opcodes & BCMLTM_OPCODE_FLAG_INSERT) {
        shr_pb_printf(pb, " Insert");
    }
    if (attr->opcodes & BCMLTM_OPCODE_FLAG_LOOKUP) {
        shr_pb_printf(pb, " Lookup");
    }
    if (attr->opcodes & BCMLTM_OPCODE_FLAG_UPDATE) {
        shr_pb_printf(pb, " Update");
    }
    if (attr->opcodes & BCMLTM_OPCODE_FLAG_DELETE) {
        shr_pb_printf(pb, " Delete");
    }
    if (attr->opcodes & BCMLTM_OPCODE_FLAG_TRAVERSE) {
        shr_pb_printf(pb, " Traverse");
    }
    shr_pb_printf(pb, "\n");

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display working buffer information for given table ID.
 *
 * This routine displays the working buffer information for a given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
wb_dump(int unit, bcmlrd_sid_t sid,
        shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    bcmltm_wb_dump(bcmltm_wb_lt_handle_get(unit, sid), pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Map Groups information for given table ID.
 *
 * This routine displays the map group count structure
 * for a given table ID.
 *
 * \param [in] group_count Map group structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
map_groups_dump(const bcmltm_db_map_group_count_t *group_count,
                shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Map Groups ---\n");

    if (group_count == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Direct Map Physical Table Groups */
    shr_pb_printf(pb, "Direct Map Groups: %d\n", group_count->dm);

    /* Field Validation Groups */
    shr_pb_printf(pb, "Field Validation Groups: %d\n", group_count->fv);

    /* Table Commit Groups */
    shr_pb_printf(pb, "Table Commit Groups: %d\n", group_count->tc);

    /* Custom Table Handler Groups */
    shr_pb_printf(pb, "Custom Table Handler Groups: %d\n", group_count->cth);

    /* LTM Groups */
    shr_pb_printf(pb, "LTM Groups: %d\n", group_count->ltm);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display given field flags.
 *
 * This routine displays the given field property flags.
 *
 * \param [in] flags Field flags.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
field_flags_dump(uint32_t flags, const char *prefix,
                 shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);


    shr_pb_printf(pb, "%sflags=0x%x {",
                  prefix, flags);
    shr_pb_printf(pb, "%s",
                  (flags & BCMLTM_FIELD_KEY) ? "key" : "value");
    shr_pb_printf(pb, "%s",
                  (flags & BCMLTM_FIELD_READ_ONLY) ? " read-only" : "");
    shr_pb_printf(pb, "}\n");

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display direct field mapping structure.
 *
 * This routine displays given mapping field structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_mapping Fields mapping structure to display.
 * \param [in] type Field type string.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
field_mapping_dump(int unit,
                   bcmlrd_sid_t sid,
                   const bcmltm_field_select_mapping_t *field_mapping,
                   char *type,
                   shr_pb_t *pb)
{
    uint32_t i;
    char *field_prefix1;
    char *field_prefix2;
    char *field_prefix3;
    uint32_t prev_fid = 0xffffffff;
    const char *fname = "";
    bcmltm_field_map_t *field;
    bcmltm_field_map_list_t *field_list;
    uint32_t map_idx;
    SHR_PB_LOCAL_DECL(pb);

    if (field_mapping == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s Fields:\n",
             type);

    shr_pb_printf(pb, "  pt_suppress_ofs=%d\n",
                  field_mapping->pt_suppress_offset);
    shr_pb_printf(pb, "  map_index_ofs=%d\n",
                  field_mapping->map_index_offset);

    /* Display each field select map list */
    shr_pb_printf(pb, "  Field Maps: num_maps=%d\n",
                  field_mapping->num_maps);
    for (map_idx = 0; map_idx < field_mapping->num_maps; map_idx++) {
        field_list = field_mapping->field_map_list[map_idx];

        shr_pb_printf(pb, "  [%3d]: index_absent_ofs=%d num_fields=%d\n",
                      map_idx,
                      field_list->index_absent_offset,
                      field_list->num_fields);

        field_prefix1 = "         ";
        field_prefix2 = "           ";
        field_prefix3 = "             ";
        for (i = 0; i < field_list->num_fields; i++) {
            field = &field_list->field_maps[i];

            while (field != NULL) {
                if (field->field_id != prev_fid) {
                    fname = bcmltm_lt_field_fid_to_name(unit, sid,
                                                        field->field_id);
                    shr_pb_printf(pb, "%s%s\n",
                                  field_prefix1, fname);
                }

                shr_pb_printf(pb, "%sid=%-5d index=%-3d\n",
                              field_prefix2,
                              field->field_id, field->field_idx);

                field_flags_dump(field->flags, field_prefix3, pb);
                shr_pb_printf(pb, "%sdef=0x%-8" PRIx64
                              " min=0x%-8" PRIx64 " max=0x%-8" PRIx64 "\n",
                              field_prefix3,
                              field->default_value,
                              field->minimum_value, field->maximum_value);
                shr_pb_printf(pb, "%spt_chg_ofs=%-5d\n",
                              field_prefix3,
                              field->pt_changed_offset);
                shr_pb_printf(pb, "%sapic_ofs=%-5d\n",
                              field_prefix3,
                              field->apic_offset);
                shr_pb_printf(pb, "%s", field_prefix3);
                WBC_DUMP(field->wbc, pb);
                shr_pb_printf(pb, "\n");

                prev_fid = field->field_id;
                field = field->next_map;
            }
        }
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display fixed field list structure.
 *
 * This routine displays given fixed field list structure.
 *
 * \param [in] fixed Fixed field list structure to display.
 * \param [in] prefix Prefix output.
 * \param [in] type Field type string.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fixed_field_list_dump(const bcmltm_fixed_field_list_t *fixed,
                      char *prefix,
                      char *type,
                      shr_pb_t *pb)
{
    uint32_t i;
    bcmltm_fixed_field_t *field;
    SHR_PB_LOCAL_DECL(pb);

    if (fixed == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%sFixed %s Field List: %d\n",
                  prefix, type,
                  fixed->num_fixed_fields);
    for (i = 0; i < fixed->num_fixed_fields; i++) {
        field = &fixed->fixed_fields[i];
        shr_pb_printf(pb, "%s[%3d]:", prefix, i);

        shr_pb_printf(pb, " value=0x%-8x",
                      field->field_value);
        shr_pb_printf(pb, " delete_value=0x%-8x\n",
                      field->delete_value);
        shr_pb_printf(pb, "%s       pt_chg_ofs=%-5d\n",
                      prefix, field->pt_changed_offset);
        shr_pb_printf(pb, "%s       ", prefix);
        WBC_DUMP(field->wbc, pb);
        shr_pb_printf(pb, "\n");
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display fixed field mapping information.
 *
 * This routine displays given fixed field mapping information structure.
 *
 * \param [in] sid Table ID.
 * \param [in] field_mapping Fixed field mapping structure to display.
 * \param [in] type Field type string.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fixed_field_mapping_dump(bcmlrd_sid_t sid,
                         const bcmltm_fixed_field_mapping_t *field_mapping,
                         char *type,
                         shr_pb_t *pb)
{
    const bcmltm_fixed_field_list_t *field_list;
    uint32_t map_idx;
    char *prefix;
    SHR_PB_LOCAL_DECL(pb);

    if (field_mapping == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "Fixed %s Field Mapping:\n",
            type);

    shr_pb_printf(pb, "  map_index_ofs=%d\n",
                  field_mapping->map_index_offset);
    shr_pb_printf(pb, "  num_maps=%d\n",
                  field_mapping->num_maps);

    prefix = "         ";
    for (map_idx = 0; map_idx < field_mapping->num_maps; map_idx++) {
        field_list = field_mapping->fixed_field_list[map_idx];

        shr_pb_printf(pb, "  [%3d]: num_fields=%d\n",
                      map_idx,
                      field_list->num_fixed_fields);
        fixed_field_list_dump(field_list, prefix, type, pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display read-only field list.
 *
 * This routine displays the given read-only field list.
 *
 * \param [in] list Read-only field list to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
read_only_field_list_dump(const bcmltm_ro_field_list_t *list,
                          shr_pb_t *pb)
{
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    if (list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "LTM Read-Only Field List:\n");

    shr_pb_printf(pb, "  Field Specifiers: %d\n", list->num_field_specs);
    for (i = 0; i < list->num_field_specs; i++) {
        shr_pb_printf(pb, "    id=%-5d index=%-3d\n",
                      list->field_specs[i].field_id,
                      list->field_specs[i].field_idx);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display wide field list.
 *
 * This routine displays the given wide field list.
 *
 * \param [in] list Wide field list to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
wide_field_list_dump(const bcmltm_wide_field_list_t *list,
                     shr_pb_t *pb)
{
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    if (list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "LTM Wide Field List:\n");

    shr_pb_printf(pb, "  Wide Fields: %d\n", list->num_wide_fields);
    for (i = 0; i < list->num_wide_fields; i++) {
        shr_pb_printf(pb, "    id=%-5d elements=%-3d\n",
                      list->wide_field[i].field_id,
                      list->wide_field[i].num_field_idx);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTD generic argument structure.
 *
 * This routine displays given LTD generic argument information.
 *
 * \param [in] arg LTD generic argument structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ltd_generic_arg_dump(const bcmltd_generic_arg_t *arg, char *prefix,
                     shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sltd_generic_arg:\n", prefix);
    if (arg == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s  sid=%d user_data=0x%lx\n",
                  prefix, arg->sid, (unsigned long)arg->user_data);

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s    ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    shr_pb_printf(pb, "%s  values: %d\n", prefix, arg->values);
    array_dump(arg->values, arg->value, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTD transform argument structure.
 *
 * This routine displays given LTD transform argument information.
 *
 * \param [in] arg LTD transform structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ltd_transform_arg_dump(const bcmltd_transform_arg_t *arg, char *prefix,
                       shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sltd_transform_arg:\n", prefix);
    if (arg == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s    ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }

    shr_pb_printf(pb, "%s  values: %d\n", prefix, arg->values);
    array_dump(arg->values, arg->value, arg_prefix, pb);

    shr_pb_printf(pb, "%s  tables: %d\n", prefix, arg->tables);
    array_dump(arg->tables, arg->table, arg_prefix, pb);

    shr_pb_printf(pb, "%s  fields: %d\n", prefix, arg->fields);
    array_dump(arg->fields, arg->field, arg_prefix, pb);

    shr_pb_printf(pb, "%s  kfields: %d\n", prefix, arg->kfields);
    array_dump(arg->kfields, arg->kfield, arg_prefix, pb);

    shr_pb_printf(pb, "%s  rfields: %d\n", prefix, arg->rfields);
    array_dump(arg->rfields, arg->rfield, arg_prefix, pb);

    print_size = sal_snprintf(arg_prefix, buf_size, "%s  ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    ltd_generic_arg_dump(arg->comp_data, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display given LTD field validation argument structure.
 *
 * This routine displays given LTD field validation argument information.
 *
 * \param [in] arg LTD field validation structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ltd_field_val_arg_dump(const bcmltd_field_val_arg_t *arg, char *prefix,
                       shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sltd_field_val_arg:\n", prefix);
    if (arg == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s    ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }

    shr_pb_printf(pb, "%s  values: %d\n", prefix, arg->values);
    array_dump(arg->values, arg->value, arg_prefix, pb);

    shr_pb_printf(pb, "%s  fields: %d\n", prefix, arg->fields);
    array_dump(arg->fields, arg->field, arg_prefix, pb);

    print_size = sal_snprintf(arg_prefix, buf_size, "%s  ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    ltd_generic_arg_dump(arg->comp_data, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTA field list.
 *
 * This routine displays given LTA field list structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] list LTA field list to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in] type Field type string.
 * \param [in] logical Indicates if field list refers to logical fields.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
lta_field_list_dump(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_lta_field_list_t *list,
                    char *prefix, char *type, bool logical,
                    shr_pb_t *pb)
{
    uint32_t i;
    char field_prefix[FIELD_PREFIX_STRING_SIZE];
    uint32_t prev_fid = 0xffffffff;
    const char *fname = "";
    bcmltm_lta_field_map_t *field;
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%s%s Fields:\n", prefix, type);
    if (list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s  fields_param_ofs=%-5d api_fields_param_ofs=%-5d\n",
                  prefix,
                  list->fields_parameter_offset,
                  list->api_fields_parameter_offset);
    shr_pb_printf(pb, "%s  target_fields_param_ofs=%-5d\n",
                  prefix,
                  list->target_fields_parameter_offset);
    shr_pb_printf(pb, "%s  fields_ptrs_ofs=%-5d\n",
                  prefix,
                  list->fields_ptrs_offset);
    shr_pb_printf(pb, "%s  index_absent_ofs=%-5d pt_suppress_ofs=%-5d\n",
                  prefix,
                  list->index_absent_offset,
                  list->pt_suppress_offset);

    shr_pb_printf(pb, "%s  Fields: max=%d num=%d\n",
                  prefix, list->max_fields, list->num_maps);

    buf_size = sizeof(field_prefix);
    print_size = sal_snprintf(field_prefix, buf_size, "%s        ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump field prefix buffer too small\n");
    }

    for (i = 0; i < list->max_fields; i++) {
        field = &list->field_maps[i];

        if (logical) {
            if (field->api_field_id != prev_fid) {
                fname = bcmltm_lt_field_fid_to_name(unit, sid,
                                                    field->api_field_id);
                shr_pb_printf(pb, "%s    %s\n", prefix, fname);
            }
        }

        shr_pb_printf(pb, "%s      id=%-5d index=%-3d\n",
                      prefix,
                      field->api_field_id,
                      field->field_idx);

        field_flags_dump(field->flags, field_prefix, pb);

        shr_pb_printf(pb, "%sdef=0x%-8" PRIx64
                      " min=0x%-8" PRIx64 " max=0x%-8" PRIx64 "\n",
                      field_prefix,
                      field->default_value,
                      field->minimum_value,
                      field->maximum_value);

        shr_pb_printf(pb, "%spt_chg_ofs=%-5d apic_ofs=%-5d field_ofs=%-5d\n",
                      field_prefix,
                      field->pt_changed_offset,
                      field->apic_offset,
                      field->field_offset);

        shr_pb_printf(pb, "%s", field_prefix);
        WBC_DUMP(field->wbc, pb);
        shr_pb_printf(pb, "\n");

        prev_fid = field->api_field_id;
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTA select field list.
 *
 * This routine displays given LTA select field list structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] list LTA field list to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in] type Field type string.
 * \param [in] logical Indicates if field list refers to logical fields.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
lta_select_field_list_dump(int unit,
                           bcmlrd_sid_t sid,
                           const bcmltm_lta_select_field_list_t *list,
                           char *prefix, char *type, bool logical,
                           shr_pb_t *pb)
{
    uint32_t i;
    char field_prefix[FIELD_PREFIX_STRING_SIZE];
    uint32_t prev_fid = 0xffffffff;
    const char *fname = "";
    bcmltm_lta_select_field_map_t *field;
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%s%s Fields:\n", prefix, type);
    if (list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s  fields_param_ofs=%-5d api_fields_param_ofs=%-5d\n",
                  prefix,
                  list->fields_parameter_offset,
                  list->api_fields_parameter_offset);
    shr_pb_printf(pb, "%s  fields_ptrs_ofs=%-5d\n",
                  prefix,
                  list->fields_ptrs_offset);
    shr_pb_printf(pb, "%s  lta_suppress_ofs=%-5d\n",
                  prefix,
                  list->lta_suppress_offset);
    shr_pb_printf(pb, "%s  select_map_index_ofs=%-5d select_map_index=%-5d\n",
                  prefix,
                  list->select_map_index_offset,
                  list->select_map_index);

    shr_pb_printf(pb, "%s  Fields: max=%d num=%d\n",
                  prefix, list->max_fields, list->num_maps);

    buf_size = sizeof(field_prefix);
    print_size = sal_snprintf(field_prefix, buf_size, "%s        ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump field prefix buffer too small\n");
    }

    for (i = 0; i < list->max_fields; i++) {
        field = &list->lta_select_field_maps[i];

        if (logical) {
            if (field->field_id != prev_fid) {
                fname = bcmltm_lt_field_fid_to_name(unit, sid,
                                                    field->field_id);
                shr_pb_printf(pb, "%s    %s\n", prefix, fname);
            }
        }

        shr_pb_printf(pb, "%s      id=%-5d index=%-3d\n",
                      prefix,
                      field->field_id,
                      field->field_idx);

        field_flags_dump(field->flags, field_prefix, pb);

        shr_pb_printf(pb, "%spt_chg_ofs=%-5d apic_ofs=%-5d field_ofs=%-5d\n",
                      field_prefix,
                      field->pt_changed_offset,
                      field->apic_offset,
                      field->field_offset);

        shr_pb_printf(pb, "%s", field_prefix);
        WBC_DUMP(field->wbc, pb);
        shr_pb_printf(pb, "\n");

        prev_fid = field->field_id;
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTA transform params structure.
 *
 * This routine displays given LTA transform params structure.
 *
 * \param [in] params LTA transform params structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
xfrm_params_dump(const bcmltm_lta_transform_params_t *params, char *prefix,
                 shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sParams:\n", prefix);
    if (params == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s  val_f=0x%lx  val_ext_f=0x%lx\n",
                  prefix,
                  (unsigned long)params->lta_transform,
                  (unsigned long)params->lta_ext_transform);
    shr_pb_printf(pb, "%s  fields_in_ofs=%-5d fields_in_key_ofs=%-5d "
                  "fields_out_ofs=%-5d\n",
                  prefix,
                  params->fields_input_offset,
                  params->fields_input_key_offset,
                  params->fields_output_offset);
    shr_pb_printf(pb, "%s  pt_sid_sel_ofs=%-5d pt_sid_over_ofs=%-5d\n",
                  prefix,
                  params->pt_sid_selector_offset,
                  params->pt_sid_override_offset);
    shr_pb_printf(pb, "%s  transform_suppress_ofs=%-5d\n",
                  prefix,
                  params->transform_suppress_offset);

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s  ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    ltd_transform_arg_dump(params->lta_args, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTA validate params structure.
 *
 * This routine displays given LTA validate params structure.
 *
 * \param [in] params LTA validate params structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
validate_params_dump(const bcmltm_lta_validate_params_t *params, char *prefix,
                     shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sParams:\n", prefix);
    if (params == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s  validate_f=0x%lx\n",
                  prefix,
                  (unsigned long)params->lta_validate);
    shr_pb_printf(pb, "%s  fields_in_ofs=%-5d\n",
                  prefix,
                  params->fields_input_offset);

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s  ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    ltd_field_val_arg_dump(params->lta_args, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Field Transform list.
 *
 * This routine displays given Field Transform list structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info_list List of field transforms to display.
 * \param [in] type Field Transform type string.
 * \param [in] forward Indicates transform is forward or reverse.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
xfrm_list_dump(int unit,
               bcmlrd_sid_t sid,
               const bcmltm_db_xfrm_list_t *info_list,
               char *type,
               bool forward,
               shr_pb_t *pb)
{
    bcmltm_db_xfrm_t *info;
    uint32_t i;
    char *prefix;
    bool in_logical;
    bool out_logical;
    SHR_PB_LOCAL_DECL(pb);

    if (info_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    if (forward) {
        in_logical = TRUE;
        out_logical = FALSE;
    } else {
        in_logical = FALSE;
        out_logical = TRUE;
    }

    shr_pb_printf(pb, "%s Field Transforms: %d\n", type, info_list->num_xfrms);

    prefix = "         ";
    for (i = 0; i < info_list->num_xfrms; i++) {
        info = &info_list->xfrms[i];

        shr_pb_printf(pb, "  [%3d]: ", i);

        shr_pb_printf(pb, "Working Buffer Block ID: %d\n", info->wb_block_id);

        xfrm_params_dump(info->xfrm_params, prefix, pb);

        lta_field_list_dump(unit, sid, info->fin, prefix, "Input",
                            in_logical, pb);
        lta_field_list_dump(unit, sid, info->fin_key, prefix, "Input Key",
                            in_logical, pb);
        lta_field_list_dump(unit, sid, info->fout, prefix, "Output",
                            out_logical, pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field transform list.
 *
 * This routine displays given field transform list structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info_list List of field transforms to display.
 * \param [in] type Field Transform type string.
 * \param [in] forward Indicates transform is forward or reverse.
 */
static void
dm_xfrm_list_dump(int unit,
                  bcmlrd_sid_t sid,
                  const bcmltm_db_dm_xfrm_list_t *info_list,
                  char *type,
                  bool forward,
                  shr_pb_t *pb)
{
    bcmltm_db_dm_xfrm_t *info;
    uint32_t i;
    char *prefix;
    bool in_logical;
    bool out_logical;
    SHR_PB_LOCAL_DECL(pb);

    if (info_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    if (forward) {
        in_logical = TRUE;
        out_logical = FALSE;
    } else {
        in_logical = FALSE;
        out_logical = TRUE;
    }

    shr_pb_printf(pb, "%s Field Transforms: %d\n", type, info_list->num_xfrms);

    prefix = "         ";
    for (i = 0; i < info_list->num_xfrms; i++) {
        info = info_list->xfrms[i];

        shr_pb_printf(pb, "  [%3d]: ", i);

        shr_pb_printf(pb, "Working Buffer Block ID: %d\n", info->lta_wb_block_id);
        shr_pb_printf(pb, "%sis_alloc_keys=%d\n", prefix, info->is_alloc_keys);

        xfrm_params_dump(info->params, prefix, pb);

        lta_select_field_list_dump(unit, sid, info->in_fields,
                                   prefix, "Input", in_logical, pb);
        lta_select_field_list_dump(unit, sid, info->in_key_fields,
                                   prefix, "Input Key",
                                   in_logical, pb);
        lta_select_field_list_dump(unit, sid, info->out_fields,
                                   prefix, "Output", out_logical, pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field validation list.
 *
 * This routine displays given field validation list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] list List of field validations.
 * \param [in] type Field type string.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
validate_list_dump(int unit,
                   bcmlrd_sid_t sid,
                   const bcmltm_db_validate_list_t *list,
                   char *type,
                   shr_pb_t *pb)
{
    uint32_t i;
    char *prefix;
    bcmltm_db_validate_t *info;
    SHR_PB_LOCAL_DECL(pb);

    if (list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s Field Validations: %d\n", type, list->num_validations);

    prefix = "         ";
    for (i = 0; i < list->num_validations; i++) {
        info = &list->validations[i];

        shr_pb_printf(pb, "  [%3d]: ", i);

        shr_pb_printf(pb, "Working Buffer Block ID: %d\n", info->wb_block_id);

        validate_params_dump(info->params, prefix, pb);

        lta_select_field_list_dump(unit, sid, info->in_fields,
                                   prefix, "Input", TRUE, pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display PT information.
 *
 * This routine displays given PT information.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info PT information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
pt_info_dump(int unit, const bcmltm_db_dm_pt_info_t *pt_info,
             shr_pb_t *pb)
{
    const bcmltm_db_pt_t *pt;
    uint32_t i;
    uint32_t num_pts;
    const char *pt_name;
    SHR_PB_LOCAL_DECL(pb);

    if (pt_info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "PT Information:\n");
    num_pts = pt_info->num_pts;
    for (i = 0; i < num_pts; i++) {
        pt = &pt_info->pts[i];
        pt_name = bcmltm_pt_table_sid_to_name(unit, pt->sid);

        shr_pb_printf(pb, "  [%3d]: ptid=%d %s\n", i, pt->sid, pt_name);
        shr_pb_printf(pb, "         flags=0x%x num_ops=%d wb_block_id=%d\n",
                      pt->flags, pt->num_ops, pt->wb_block_id);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Track Index structure.
 *
 * This routine displays the given Track Index structure.
 *
 * \param [in] track_index Track index structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
track_index_dump(const bcmltm_track_index_t *track_index,
                 shr_pb_t *pb)
{
    uint32_t i;
    bcmltm_track_index_field_t *field;
    SHR_PB_LOCAL_DECL(pb);

    if (track_index == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "Track Index:\n");
    shr_pb_printf(pb, "  track_index_ofs=%-5d  track_index_max=%-5d "
                  "track_alloc_mask=0x%x\n",
                  track_index->track_index_offset, track_index->track_index_max,
                  track_index->track_alloc_mask);
    shr_pb_printf(pb, "  index_absent_ofs=%-5d\n",
                  track_index->index_absent_offset);

    shr_pb_printf(pb, "  field_num=%d:\n", track_index->track_index_field_num);
    for (i = 0; i < track_index->track_index_field_num; i++) {
        field = &track_index->track_index_fields[i];
        shr_pb_printf(pb, "    ofs=%-5d mask=0x%-8x shift=%-2d alloc=%s\n",
                      field->field_offset,
                      field->field_mask,
                      field->field_shift,
                      field->allocatable ? "t" : "f");
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display EE index information.
 *
 * This routine displays given EE LT index structure.
 *
 * \param [in] ee_index EE index structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ee_lt_index_dump(const bcmltm_ee_index_info_t *ee_index,
                 shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    if (ee_index == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "EE LT Index:\n");
    shr_pb_printf(pb, "  index_ofs=%d\n", ee_index->index_offset);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display given WB copy list structure.
 *
 * This routine displays the given WB copy list structure.
 *
 * \param [in] copy_list WB copy list to display.
 * \param [in] str_type String type for WB copy list.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
wb_copy_list_dump(const bcmltm_wb_copy_list_t *copy_list, char *str_type,
                  shr_pb_t *pb)
{
    uint32_t i;
    bcmltm_wb_copy_t *copy;
    SHR_PB_LOCAL_DECL(pb);

    if (copy_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s WB Copy:\n", str_type);
    shr_pb_printf(pb, "  index_absent_ofs=%d\n", copy_list->index_absent_offset);
    shr_pb_printf(pb, "  num_wb_copy=%d\n", copy_list->num_wb_copy);
    shr_pb_printf(pb, "  From                                : To\n");
    for (i = 0; i < copy_list->num_wb_copy; i++) {
        copy = &copy_list->wb_copy[i];
        shr_pb_printf(pb, "  ");
        WBC_DUMP(copy->wbc_source, pb);
        shr_pb_printf(pb, " : ");
        WBC_DUMP(copy->wbc_destination, pb);
        shr_pb_printf(pb, "\n");
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Track Index information.
 *
 * This routine displays given track index information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Track information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
track_info_dump(int unit,
                bcmlrd_sid_t sid,
                const bcmltm_db_dm_track_info_t *info,
                shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "Track Info:\n");

    field_mapping_dump(unit, sid, info->track_to_api, "Track Index", pb);

    dm_xfrm_list_dump(unit, sid, info->track_rev_xfrms,
                      "Track Index Reverse Key", FALSE, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display PT memory arguments structure.
 *
 * This routine displays given PT memory arguments structure.
 *
 * \param [in] mem_arg PT memory arguments structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
pt_mem_args_dump(const bcmltm_pt_mem_args_t *mem_args, char *prefix,
                 shr_pb_t *pb)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%sMemory args:\n", prefix);
    if (mem_args == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    pt_dyn_info = mem_args->pt_dyn_info;
    pt_dyn_hash_info = mem_args->pt_dyn_hash_info;

    shr_pb_printf(pb, "%s  flags=0x%" PRIx64 " ptid=%d\n",
                  prefix, mem_args->flags, mem_args->pt);

    shr_pb_printf(pb, "%s  pt_dyn_info: ", prefix);
    if (pt_dyn_info != NULL) {
        shr_pb_printf(pb, " index=%d tbl_inst=%d\n",
                      pt_dyn_info->index, pt_dyn_info->tbl_inst);
    } else {
        shr_pb_printf(pb, "\n");
    }

    shr_pb_printf(pb, "%s  pt_dyn_hash_info: ", prefix);
    if (pt_dyn_hash_info != NULL) {
        shr_pb_printf(pb, " tbl_inst=%d bank=%d\n",
                      pt_dyn_hash_info->tbl_inst, pt_dyn_hash_info->bank);
    } else {
        shr_pb_printf(pb, "\n");
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display PT list structure.
 *
 * This routine displays given PT list structure.
 *
 * \param [in] pt_list PT list structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
pt_list_dump(const bcmltm_pt_list_t *pt_list,
             shr_pb_t *pb)
{
    uint32_t i;
    uint32_t op;
    char *prefix;
    bcmltm_pt_mem_args_t *mem_args;
    bcmltm_pt_op_list_t *op_list;
    SHR_PB_LOCAL_DECL(pb);

    if (pt_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "PT Index List: %d\n", pt_list->num_pt_view);

    prefix = "         ";

    shr_pb_printf(pb, "  pt_suppress_ofs=%-5d\n",
            pt_list->pt_suppress_offset);

    for (i = 0; i < pt_list->num_pt_view; i++) {
        shr_pb_printf(pb, "  [%3d]:\n", i);

        mem_args = pt_list->mem_args[i];
        pt_mem_args_dump(mem_args, prefix, pb);

        op_list = pt_list->memop_lists[i];
        shr_pb_printf(pb, "%sMemory ops:\n", prefix);
        shr_pb_printf(pb, "%s  flags=0x%x num_ops=%d pt_wsize=%d "
                      "keys=%d entries=%d\n",
                      prefix,
                      op_list->flags, op_list->num_pt_ops, op_list->word_size,
                      op_list->key_entries, op_list->data_entries);
        for (op = 0; op < op_list->num_pt_ops; op++) {
            shr_pb_printf(pb, "%s  op[%3d]: param_ofs=%-5d buffer_ofs=%-5d\n",
                          prefix, op,
                          op_list->param_offsets[op],
                          op_list->buffer_offsets[op]);
        }
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display list of PT keyed operations information.
 *
 * This routine displays given list of PT keyed operations structure.
 *
 * \param [in] pt_list PT keyed list structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
pt_keyed_list_dump(bcmltm_pt_keyed_op_info_t * const * pt_list,
                   shr_pb_t *pb)
{
    uint32_t idx;
    uint32_t i;
    char *prefix;
    char *opcode = "";
    const bcmltm_pt_keyed_op_info_t *info;
    const bcmltm_pt_mem_args_t *mem_args;
    SHR_PB_LOCAL_DECL(pb);

    if (pt_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "PT Keyed Operations Info:\n");

    for (idx = 0; idx < BCMLTM_DB_DM_PT_KEYED_NUM; idx++) {
        info = pt_list[idx];
        if (info == NULL) {
            continue;
        }

        prefix = "  ";
        if (idx == BCMLTM_DB_DM_PT_KEYED_INS_DEL_IDX) {
            opcode = "Insert/Delete";
        } else if (idx == BCMLTM_DB_DM_PT_KEYED_LK_TRV_IDX) {
            opcode = "Lookup/Get-First/Get-Next";
        }
        shr_pb_printf(pb, "%s[%d]: %s\n", prefix, idx, opcode);

        prefix = "       ";
        mem_args = info->mem_args;
        pt_mem_args_dump(mem_args, prefix, pb);

        shr_pb_printf(pb, "%spt_suppress_ofs=%-5d\n",
                      prefix,
                      info->pt_suppress_offset);
        shr_pb_printf(pb, "%sflags=0x%x pt_key_wsize=%d pt_data_wsize=%d\n",
                      prefix,
                      info->flags, info->key_word_size, info->data_word_size);
        shr_pb_printf(pb, "%sparam_ofs=%-5d\n",
                      prefix,
                      info->param_offset);

        shr_pb_printf(pb, "%sinput_key_ofs  ", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            shr_pb_printf(pb, "  [%d]=%-5d", i, info->input_key_buffer_offset[i]);
        }
        shr_pb_printf(pb, "\n");

        shr_pb_printf(pb, "%sinput_data_ofs ", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            shr_pb_printf(pb, "  [%d]=%-5d", i, info->input_data_buffer_offset[i]);
        }
        shr_pb_printf(pb, "\n");

        shr_pb_printf(pb, "%soutput_key_ofs ", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            shr_pb_printf(pb, "  [%d]=%-5d", i,
                          info->output_key_buffer_offset[i]);
        }
        shr_pb_printf(pb, "\n");

        shr_pb_printf(pb, "%soutput_data_ofs", prefix);
        for (i = 0; i < BCMLTM_PT_ENTRY_LIST_MAX; i++) {
            shr_pb_printf(pb, "  [%d]=%-5d", i, info->output_data_buffer_offset[i]);
        }
        shr_pb_printf(pb, "\n");
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display direct field mapping structures.
 *
 * This routine displays the direct field mapping structures
 * in given DM information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info DM info structure.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
field_mappings_dump(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_db_dm_info_t *info,
                    shr_pb_t *pb)
{
    char *str;
    const bcmltm_field_select_mapping_t *key_in_map;
    const bcmltm_field_select_mapping_t *key_out_map;
    SHR_PB_LOCAL_DECL(pb);

    /* Key fields list */
    key_in_map = info->in_key_fields;
    key_out_map = info->out_key_fields;
    if (key_in_map != key_out_map) {
        str = "Input Key";
    } else {
        str = "Input/Output Key";
    }
    field_mapping_dump(unit, sid, key_in_map, str, pb);
    if (key_out_map != key_in_map){
        field_mapping_dump(unit, sid, key_out_map, "Output Key", pb);
    }

    /* Required/Allocatable Key fields list */
    field_mapping_dump(unit, sid, info->req_key_fields,
                       "IwA Required Key", pb);
    field_mapping_dump(unit, sid, info->alloc_key_fields,
                       "IwA Allocatable Key (Optional)", pb);

    /* Value fields list */
    field_mapping_dump(unit, sid, info->value_fields, "Input/Output Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Field Transform information.
 *
 * This routine displays given Field Transform information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm Field Transform information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
xfrm_info_dump(int unit,
               bcmlrd_sid_t sid,
               const bcmltm_db_xfrm_info_t *xfrm,
               shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    /* Forward Key/Value Field Transform list */
    xfrm_list_dump(unit, sid, xfrm->fwd_key_xfrm_list,
                   "Forward Key", TRUE, pb);
    xfrm_list_dump(unit, sid, xfrm->fwd_value_xfrm_list,
                   "Forward Value", TRUE, pb);

    /* Reverse Key/Value Field Transform list */
    xfrm_list_dump(unit, sid, xfrm->rev_key_xfrm_list,
                   "Reverse Key", FALSE, pb);
    xfrm_list_dump(unit, sid, xfrm->rev_value_xfrm_list,
                   "Reverse Value", FALSE, pb);

    /* Copy Field Transform list */
    xfrm_list_dump(unit, sid, xfrm->rkey_to_fkey_xfrm_list,
                   "Copy Reverse Key to Forward Key", TRUE, pb);
    xfrm_list_dump(unit, sid, xfrm->rvalue_to_fvalue_xfrm_list,
                   "Copy Reverse Value to Forward Value", TRUE, pb);
    xfrm_list_dump(unit, sid, xfrm->rkey_to_rvalue_xfrm_list,
                   "Copy Reverse Key to Reverse Value In Key", FALSE, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field select maps information.
 *
 * This routine displays given field select maps information structure.
 *
 * \param [in] sid Table ID.
 * \param [in] fsm Field select maps structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
field_select_maps_dump(bcmlrd_sid_t sid,
                       const bcmltm_field_select_maps_t *fsm,
                       shr_pb_t *pb)
{
    const bcmltm_select_map_t *select_map;
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "Field Select Maps:\n");

    if (fsm == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "  select_field_ofs=%d pt_subentry_ofs=%d "
                  "map_index_ofs=%d\n",
                  fsm->select_field_offset,
                  fsm->pt_subentry_offset,
                  fsm->map_index_offset);
    shr_pb_printf(pb, "  parent_select_ofs=%d parent_select_map_index=%d\n",
                  fsm->parent_select_offset,
                  fsm->parent_select_map_index);
    shr_pb_printf(pb, "  num_maps=%d\n",
                  fsm->num_maps);

    for (i = 0; i < fsm->num_maps; i++) {
        select_map = &fsm->select_maps[i];
        shr_pb_printf(pb, "    selector_field_value=%" PRIu64 " map_index=%d\n",
                      select_map->selector_field_value,
                      select_map->map_index);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display API cache field maps information.
 *
 * This routine displays given API cache field maps information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info API cache field maps information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
apic_field_info_dump(int unit,
                     bcmlrd_sid_t sid,
                     const bcmltm_db_dm_apic_field_info_t *info,
                     shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "API Cache Fields Information:\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Key fields list */
    field_mapping_dump(unit, sid, info->keys,
                       "API Cache Input/Output Key", pb);

    /* Required/Allocatable Key fields list */
    field_mapping_dump(unit, sid, info->req_keys,
                       "API Cache IwA Required Key", pb);
    field_mapping_dump(unit, sid, info->alloc_keys,
                       "API Cache IwA Allocatable Key (Optional)", pb);

    /* Value fields list */
    field_mapping_dump(unit, sid, info->values,
                       "API Cache Input/Output Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display direct field maps information.
 *
 * This routine displays given direct field maps information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Direct field maps information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
field_map_info_dump(int unit,
                    bcmlrd_sid_t sid,
                    const bcmltm_db_dm_field_map_info_t *info,
                    shr_pb_t *pb)
{
    const bcmltm_field_select_mapping_t *in_keys;
    const bcmltm_field_select_mapping_t *out_keys;
    char *str;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "Direct Field Maps Information:\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Key fields list */
    in_keys = info->in_keys;
    out_keys = info->out_keys;
    if (in_keys != out_keys) {
        str = "Direct Input Key";
    } else {
        str = "Direct Input/Output Key";
    }

    field_mapping_dump(unit, sid, in_keys, str, pb);
    if (out_keys != in_keys){
        field_mapping_dump(unit, sid, out_keys,
                           "Direct Output Key", pb);
    }

    /* Required/Allocatable Key fields list */
    field_mapping_dump(unit, sid, info->req_keys,
                       "Direct IwA Required Key", pb);
    field_mapping_dump(unit, sid, info->alloc_keys,
                       "Direct IwA Allocatable Key (Optional)", pb);

    /* Value fields list */
    field_mapping_dump(unit, sid, info->values,
                       "Direct Input/Output Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display fixed field information.
 *
 * This routine displays given fixed field information structure.
 *
 * \param [in] sid Table ID.
 * \param [in] info Fixed field information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fixed_field_info_dump(bcmlrd_sid_t sid,
                      const bcmltm_db_dm_fixed_field_info_t *info,
                      shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "Fixed Fields Information:\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Fixed key fields */
    fixed_field_mapping_dump(sid, info->keys, "Key", pb);

    /* Fixed value fields */
    fixed_field_mapping_dump(sid, info->values, "Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field transform information.
 *
 * This routine displays given field transform information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Field transform information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
dm_xfrm_info_dump(int unit,
                  bcmlrd_sid_t sid,
                  const bcmltm_db_dm_xfrm_info_t *info,
                  shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "Transforms Information:\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Forward Key/Value Field Transform list */
    dm_xfrm_list_dump(unit, sid, info->fwd_keys, "Forward Key", TRUE, pb);
    dm_xfrm_list_dump(unit, sid, info->fwd_values, "Forward Value", TRUE, pb);

    /* Reverse Key/Value Field Transform list */
    dm_xfrm_list_dump(unit, sid, info->rev_keys, "Reverse Key", FALSE, pb);
    dm_xfrm_list_dump(unit, sid, info->rev_values, "Reverse Value", FALSE, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field validation information.
 *
 * This routine displays given field validation information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Field validation information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
validate_info_dump(int unit,
                   bcmlrd_sid_t sid,
                   const bcmltm_db_validate_info_t *info,
                   shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "Field Validations Information:\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Field Validation Lists */
    validate_list_dump(unit, sid, info->keys, "Key", pb);
    validate_list_dump(unit, sid, info->values, "Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field selections information.
 *
 * This routine displays given field selections information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Field selections information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fs_info_dump(int unit,
             bcmlrd_sid_t sid,
             const bcmltm_db_dm_fs_info_t *info,
             shr_pb_t *pb)
{
    const bcmltm_db_dm_fs_t *fs;
    uint32_t i;
    char *selector_type_str;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Field Select Information ---\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "num_levels=%d num_with_selectors=%d num_selections=%d\n",
                  info->num_levels, info->num_with_selectors,
                  info->num_selections);

    /* Display various information for each selection group*/
    for (i = 0; i < info->num_selections; i++) {
        fs = &info->selections[i];

        if (fs->selector_type == BCMLTM_FIELD_SELECTOR_TYPE_KEY) {
            selector_type_str = "Key";
        } else if (fs->selector_type == BCMLTM_FIELD_SELECTOR_TYPE_VALUE) {
            selector_type_str = "Value";
        } else {
            selector_type_str = "None";
        }

        shr_pb_printf(pb, "\n");
        shr_pb_printf(pb, "--- Field Select [%d]: ", i);

        shr_pb_printf(pb, "selection_id=%d selector_type=%s level=%d\n",
                      fs->selection_id, selector_type_str, fs->level);

        field_select_maps_dump(sid, fs->fsm, pb);
        apic_field_info_dump(unit, sid, fs->apic_fields, pb);
        field_map_info_dump(unit, sid, fs->field_maps, pb);
        fixed_field_info_dump(sid, fs->fixed_fields, pb);
        dm_xfrm_info_dump(unit, sid, fs->xfrms, pb);
        validate_info_dump(unit, sid, fs->field_validations, pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Direct Map information.
 *
 * This routine displays given Direct Map information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Custom Table Handler information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
dm_info_dump(int unit, bcmlrd_sid_t sid, const bcmltm_db_dm_info_t *info,
             shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Direct Map Information ---\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* PT information */
    pt_info_dump(unit, info->pt_info, pb);

    /* LT Private working buffer block ID */
    shr_pb_printf(pb, "LT Private Working Buffer Block ID: %d\n",
                  info->lt_pvt_wb_block_id);

    /* PT suppress */
    shr_pb_printf(pb, "PT Suppress: ");
    if (info->pt_suppress) {
        shr_pb_printf(pb, "TRUE\n");
    } else {
        shr_pb_printf(pb, "FALSE\n");
    }

    /* Track index */
    if (info->track_index != NULL) {
        shr_pb_printf(pb, "Track Index Map: ");
        if (info->track_index_map) {
            shr_pb_printf(pb, "TRUE\n");
        } else {
            shr_pb_printf(pb, "FALSE\n");
        }
    }
    track_index_dump(info->track_index, pb);

    /* EE LT index */
    ee_lt_index_dump(info->ee_lt_index, pb);

    /* Track to PT wb copy */
    wb_copy_list_dump(info->track_to_pt_copy, "Track to PT", pb);

    /* Track index information */
    track_info_dump(unit, sid, info->track_info, pb);

    /* PT Index List */
    pt_list_dump(info->pt_list, pb);

    /* PT Keyed List */
    pt_keyed_list_dump(info->pt_keyed_list, pb);

    /* Direct Field Mappings */
    field_mappings_dump(unit, sid, info, pb);

    /* Fixed fields */
    fixed_field_list_dump(info->fixed_key_fields, "", "Key", pb);
    fixed_field_list_dump(info->fixed_value_fields, "", "Value", pb);

    /* Read-Only fields */
    read_only_field_list_dump(info->read_only_fields, pb);

    /* Wide fields */
    wide_field_list_dump(info->wide_fields, pb);

    /* Field Transforms */
    xfrm_info_dump(unit, sid, info->xfrm, pb);

    /* Field select information */
    fs_info_dump(unit, sid, info->fs, pb);

    /* API Cache all value field mappings information */
    field_mapping_dump(unit, sid, info->apic_all_value_fields,
                       "API Cache All Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Field Validation list.
 *
 * This routine displays given field validation list.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fv_list List of field validations.
 * \param [in] type Field type string.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fv_list_dump(int unit,
             bcmlrd_sid_t sid,
             const bcmltm_db_fv_list_t *fv_list,
             char *type,
             shr_pb_t *pb)
{
    uint32_t i;
    char *prefix;
    bcmltm_db_fv_t *info;
    SHR_PB_LOCAL_DECL(pb);

    if (fv_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s Field Validations: %d\n", type, fv_list->num_fvs);

    prefix = "         ";
    for (i = 0; i < fv_list->num_fvs; i++) {
        info = &fv_list->fvs[i];

        shr_pb_printf(pb, "  [%3d]: ", i);

        shr_pb_printf(pb, "Working Buffer Block ID: %d\n", info->wb_block_id);

        validate_params_dump(info->validate_params, prefix, pb);

        lta_field_list_dump(unit, sid, info->fsrc, prefix, "Input", TRUE, pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Field Validation information for given table ID.
 *
 * This routine displays given Field Validation information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Field Validation information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fv_info_dump(int unit, bcmlrd_sid_t sid, const bcmltm_db_fv_info_t *info,
             shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Field Validation Information ---\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Field Validation Lists */
    fv_list_dump(unit, sid, info->validate_keys, "Key", pb);
    fv_list_dump(unit, sid, info->validate_values, "Value", pb);
    fv_list_dump(unit, sid, info->rvalue_to_value,
                 "Copy Reverse Value Transform to Value", pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTD table commit handler structure.
 *
 * This routine displays the given LTD table commit handler structure.
 *
 * \param [in] handler LTD table commit handler structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ltd_table_commit_handler_dump(const bcmltd_table_commit_handler_t *handler,
                              char *prefix,
                              shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "handler (0x%lx):\n", (unsigned long)handler);
    if (handler == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "%s  prepare_f=0x%lx commit_f=0x%lx commit_f=0x%lx\n",
                  prefix,
                  (unsigned long)handler->prepare,
                  (unsigned long)handler->commit,
                  (unsigned long)handler->abort);

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s  ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    ltd_generic_arg_dump(handler->arg, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Table Commit list.
 *
 * This routine displays the given Table Commit interface list.
 *
 * \param [in] tc_list Table commit list to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
tc_handler_list_dump(const bcmltm_table_commit_list_t *tc_list,
                     shr_pb_t *pb)
{
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    if (tc_list == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "Table Commit List: %d\n", tc_list->num_handlers);
    for (i = 0; i < tc_list->num_handlers; i++) {
        shr_pb_printf(pb, "  [%3d]: ", i);
        ltd_table_commit_handler_dump(tc_list->handlers[i], "         ", pb);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Table Commit information.
 *
 * This routine displays given Table Commit information structure.
 *
 * \param [in] info Table Commit information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
tc_info_dump(const bcmltm_db_tc_info_t *info,
             shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Table Commit Information ---\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* Table Commit Handlers */
    tc_handler_list_dump(info->tc_list, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTD table handler structure.
 *
 * This routine displays the given LTD table handler structure.
 *
 * \param [in] handler LTD table handler structure to display.
 * \param [in] prefix Prefix indentation for output.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ltd_table_handler_dump(const bcmltd_table_handler_t *handler, char *prefix,
                       shr_pb_t *pb)
{
    char arg_prefix[PREFIX_STRING_SIZE];
    bool ext_op = FALSE;
    int buf_size, print_size;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "%shandler (0x%lx):\n", prefix, (unsigned long)handler);
    if (handler == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    if ((handler->op_insert != NULL) || (handler->op_insert_alloc != NULL)) {
        ext_op = TRUE;
    }

    shr_pb_printf(pb, "%s  validate_f=0x%lx\n",
                  prefix,
                  (unsigned long)handler->validate);
    if (ext_op) {
        shr_pb_printf(pb, "%s  insert_f=0x%lx insert_alloc_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->op_insert,
                      (unsigned long)handler->op_insert_alloc);
        shr_pb_printf(pb, "%s  lookup_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->op_lookup);
        shr_pb_printf(pb, "%s  delete_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->op_delete);
        shr_pb_printf(pb, "%s  update_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->op_update);
        shr_pb_printf(pb, "%s  traverse_first_f=0x%lx traverse_next_f=0x%lx \n",
                      prefix,
                      (unsigned long)handler->op_first,
                      (unsigned long)handler->op_next);
    } else {
        shr_pb_printf(pb, "%s  insert_f=0x%lx insert_alloc_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->insert,
                      (unsigned long)handler->insert_alloc);
        shr_pb_printf(pb, "%s  lookup_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->lookup);
        shr_pb_printf(pb, "%s  delete_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->delete);
        shr_pb_printf(pb, "%s  update_f=0x%lx\n",
                      prefix,
                      (unsigned long)handler->update);
        shr_pb_printf(pb, "%s  traverse_hdl=0x%lx\n",
                      prefix,
                      (unsigned long)handler->traverse);
    }

    shr_pb_printf(pb, "%s  ha_hdl=0x%lx\n",
                  prefix,
                  (unsigned long)handler->ha);
    shr_pb_printf(pb, "%s  entry_limit_hdl=0x%lx entry_usage_hdl=0x%lx\n",
                  prefix,
                  (unsigned long)handler->entry_limit,
                  (unsigned long)handler->entry_usage);

    buf_size = sizeof(arg_prefix);
    print_size = sal_snprintf(arg_prefix, buf_size, "%s  ", prefix);
    if (print_size >= buf_size) {
        shr_pb_printf(pb, "LTM DB info dump prefix buffer too small\n");
    }
    ltd_generic_arg_dump(handler->arg, arg_prefix, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display Custom Table Handler information.
 *
 * This routine displays given Custom Table Handler information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info Custom Table Handler information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
cth_info_dump(int unit,
              bcmlrd_sid_t sid,
              const bcmltm_db_cth_info_t *info,
              shr_pb_t *pb)
{
    const bcmltm_lta_table_params_t *table_params;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- Custom Table Handler Information ---\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* CTH Working Buffer */
    shr_pb_printf(pb, "Working Buffer Block ID: %d\n",
                  info->wb_block_id);

    /* CTH Table Parameters */
    table_params = info->table_params;
    if (table_params != NULL) {
        shr_pb_printf(pb, "Table Parameters:\n");
        shr_pb_printf(pb, "  fields_in_ofs=%-5d fields_out_ofs=%-5d\n",
                      table_params->fields_input_offset,
                      table_params->fields_output_offset);

        ltd_table_handler_dump(table_params->handler, "  ", pb);
    }

    /* CTH Input field list */
    lta_field_list_dump(unit, sid, info->fin, "", "Input", TRUE, pb);

    /* CTH Input key field list */
    lta_field_list_dump(unit, sid, info->fin_keys, "", "Input Key", TRUE, pb);

    /* CTH Output field list */
    lta_field_list_dump(unit, sid, info->fout, "", "Output", TRUE, pb);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display given FA LTM parameters structure.
 *
 * This routine displays given FA LTM internal node cookie.
 *
 * \param [in] f_ltm FA LTM parameters structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
fa_ltm_dump(const bcmltm_fa_ltm_t *fa_ltm,
            shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    if (fa_ltm == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    shr_pb_printf(pb, "FA LTM:\n");
    shr_pb_printf(pb, "  ltid_ofs=%d  lt_field_id_ofs=%d  select_group_ofs=%d "
                  "resource_info_ofs=%d\n",
                  fa_ltm->key_offset[BCMLTM_TABLE_KEYS_LTID],
                  fa_ltm->key_offset[BCMLTM_TABLE_KEYS_LT_FIELD_ID],
                  fa_ltm->key_offset[BCMLTM_TABLE_KEYS_SELECT_GROUP],
                  fa_ltm->key_offset[BCMLTM_TABLE_KEYS_RESOURCE_INFO]);

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display field spec list.
 *
 * This routine displays the given field spec list.
 *
 * \param [in] list Field spec list to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
field_spec_list_dump(const bcmltm_field_spec_list_t *list,
                     shr_pb_t *pb)
{
    uint32_t i;
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "LTM Group Field Specifier List:\n");
    shr_pb_printf(pb, "  ltid_ofs=%d\n",
                  list->key_offset[BCMLTM_TABLE_KEYS_LTID]);
    shr_pb_printf(pb, "  lt_field_id_ofs=%d\n",
                  list->key_offset[BCMLTM_TABLE_KEYS_LT_FIELD_ID]);
    shr_pb_printf(pb, "  select_group_ofs=%d\n",
                  list->key_offset[BCMLTM_TABLE_KEYS_SELECT_GROUP]);
    shr_pb_printf(pb, "  resource_info_ofs=%d\n",
                  list->key_offset[BCMLTM_TABLE_KEYS_RESOURCE_INFO]);

    shr_pb_printf(pb, "  Field Specifiers: %d\n", list->num_field_specs);
    for (i = 0; i < list->num_field_specs; i++) {
        shr_pb_printf(pb, "    id=%-5d index=%-3d\n",
                      list->field_specs[i].field_id,
                      list->field_specs[i].field_idx);
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LTM Managed Table information.
 *
 * This routine displays given LTM Managed Table information structure.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] info LTM Managed information structure to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
static void
ltm_info_dump(int unit,
              bcmlrd_sid_t sid,
              const bcmltm_db_ltm_info_t *info,
              shr_pb_t *pb)
{
    SHR_PB_LOCAL_DECL(pb);

    shr_pb_printf(pb, "--- LTM Managed Table Information ---\n");

    if (info == NULL) {
        SHR_PB_LOCAL_DONE();
        return;
    }

    /* LTM Working Buffer */
    shr_pb_printf(pb, "Working Buffer Block ID: %d\n",
                  info->wb_block_id);

    /* LTM FA parameters */
    fa_ltm_dump(info->fa_ltm, pb);

    /* LTM keys */
    field_mapping_dump(unit, sid, info->ltm_key, "LTM Group Key", pb);

    /* LTM Field spec list */
    field_spec_list_dump(info->fs_list, pb);

    SHR_PB_LOCAL_DONE();
}

/*******************************************************************************
 * Public functions
 */

void
bcmltm_db_lt_info_dump(int unit, bcmlrd_sid_t sid,
                       const bcmltm_db_lt_info_t *info,
                       shr_pb_t *pb)
{
    const char *table_name;
    SHR_PB_LOCAL_DECL(pb);

    table_name = bcmltm_lt_table_sid_to_name(unit, sid);

    shr_pb_printf(pb, "=== LT ID %d: %s ===\n", sid, table_name);

    /* Table Attributes */
    table_attr_dump(unit, &info->attr, pb);
    shr_pb_printf(pb, "\n");

    /* Working Buffer information */
    wb_dump(unit, sid, pb);
    shr_pb_printf(pb, "\n");

    /* Map Groups Information */
    map_groups_dump(&info->map_group_count, pb);
    shr_pb_printf(pb, "\n");

    /* Direct Mapped Information */
    dm_info_dump(unit, sid, info->dm, pb);
    shr_pb_printf(pb, "\n");

    /* Field Validation Info */
    fv_info_dump(unit, sid, info->fv, pb);
    shr_pb_printf(pb, "\n");

    /* Table Commit Information */
    tc_info_dump(info->tc, pb);
    shr_pb_printf(pb, "\n");

    /* Custom Table Handler Information */
    cth_info_dump(unit, sid, info->cth, pb);
    shr_pb_printf(pb, "\n");

    /* LTM Information */
    ltm_info_dump(unit, sid, info->ltm, pb);
    shr_pb_printf(pb, "\n");

    SHR_PB_LOCAL_DONE();
}
