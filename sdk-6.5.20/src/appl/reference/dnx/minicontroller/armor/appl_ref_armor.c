/** \file appl_ref_minicon.c
 * 
 *
 * Minicontroller refference applications.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <src/appl/reference/dnx/minicontroller/armor/appl_ref_armor.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_string_apis.h>

typedef struct
{
    rhentry_t entry;
    char key_string[RHFILE_MAX_SIZE];
    char rename_string[RHFILE_MAX_SIZE];
} armor_rename_str_struct_t;

typedef struct
{
    rhlist_t *dbal_tables;
    rhlist_t *table_rename_list;
    rhlist_t *field_rename_list;
    rhlist_t *field_add_list;
} armor_struct_t;

extern shr_error_e dbal_tables_table_get(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t ** table);

static armor_struct_t **minicon_armor_appl_info = NULL;

int
minicon_armor_is_initialized(
    int unit)
{
    return ((minicon_armor_appl_info != NULL) && (minicon_armor_appl_info[unit] != NULL));
}

int
minicon_armor_get_num_of_tables(
    int unit)
{
    if (!minicon_armor_is_initialized(unit))
    {
        return 0;
    }
    return minicon_armor_appl_info[unit]->dbal_tables->num;
}

char *
minicon_armor_get_table_name_by_index(
    int unit,
    uint32 index)
{
    if (!minicon_armor_is_initialized(unit) || index >= minicon_armor_appl_info[unit]->dbal_tables->num)
    {
        return NULL;
    }
    return utilex_rhlist_name_get_by_id(minicon_armor_appl_info[unit]->dbal_tables, index);
}

shr_error_e
minicon_armor_prepare_dbal_tables_list(
    int unit)
{
    dbal_tables_e table_id;
    int index = 0;
    CONST dbal_logical_table_t *table;
    rhhandle_t rhhandle;
    char *name;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_id++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        if (sal_strstr(table->table_name, MINICON_ARMOR_NAME_STR) != NULL)
        {
            name = (char *) table->table_name;
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                            (minicon_armor_appl_info[unit]->dbal_tables, name, index++, &rhhandle));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
minicon_armor_add_default_compressed_entry(
    int unit)
{
    uint32 entry_handle_id;
    uint32 core = BCM_CORE_ALL;
    uint32 priority = 1;
    uint32 entry_access_id;
    uint32 l4_dst_port = 0xbbbb;
    uint32 l4_dst_mask = 0xffff;
    uint32 l4_src_port = 0xaaaa;
    uint32 l4_src_mask = 0xffff;
    uint32 ip4_proto = 6;
    uint32 ip4_proto_mask = 0xff;
    uint32 tcam_qalifier = 0;
    uint32 tcam_qalifier_mask = 0x3fff;
    uint32 ip_group = 0;
    uint32 ip_mask = 0x1fff;
    uint32 stat_atr = 0;
    uint32 stat_id = 7;
    uint32 dst_data = 0x7fff;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_entry_handle_take(unit, "ARMOR_TCAM_COMPRESSED", &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_handle_access_id_allocate_and_set
                    (unit, entry_handle_id, core, priority, &entry_access_id));

    SHR_IF_ERR_EXIT(dnx_dbal_entry_key_field_arr32_masked_set
                    (unit, entry_handle_id, "L4_DST_PORT", &l4_dst_port, &l4_dst_mask));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_key_field_arr32_masked_set
                    (unit, entry_handle_id, "L4_SRC_PORT", &l4_src_port, &l4_src_mask));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_key_field_arr32_masked_set
                    (unit, entry_handle_id, "IPV4_PROTOCOL", &ip4_proto, &ip4_proto_mask));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_key_field_arr32_masked_set
                    (unit, entry_handle_id, "TCAM_OVF_INDEX_QUALIFIER", &tcam_qalifier, &tcam_qalifier_mask));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_key_field_arr32_masked_set
                    (unit, entry_handle_id, "vw_ARMOR___dip_group", &ip_group, &ip_mask));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_key_field_arr32_masked_set
                    (unit, entry_handle_id, "vw_ARMOR___sip_group", &ip_group, &ip_mask));

    SHR_IF_ERR_EXIT(dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "USER_STATISTICS_ATR_0", 0, &stat_atr));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "USER_STATISTICS_ID_0", 0, &stat_id));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "USER_DST_DATA", 0, &dst_data));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT"));
    SHR_IF_ERR_EXIT(dnx_dbal_entry_handle_free(unit, entry_handle_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
minicon_armor_pmf_compressed_tcam(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_group_t fg_id = 5;
    bcm_field_group_t fg_id2 = 6;
    bcm_field_name_to_id_info_t name_to_id_info;
    uint32 id_get;
    bcm_field_qualify_t vw_dip_qual;
    bcm_field_qualify_t vw_sip_qual;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_t action_ovs_index;
    int action_offset;
    bcm_field_qualify_t qual_id;
    bcm_field_action_t user_action_dst;
    bcm_field_action_t user_action_stat;
    bcm_field_action_t user_action_stat_profile;
    int nof_ids;
    void *dest_char;
    char *proc_name;

    SHR_FUNC_INIT_VARS(unit);

    proc_name = "pmf_compressed_tcam";

    /*
     * create user action
     */
    bcm_field_action_info_t_init(&action_info);

    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "TCAM_OVF_INDEX_ACTION", sizeof(action_info.name));

    action_info.action_type = bcmFieldActionVoid;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = 14;
    action_info.stage = bcmFieldStageIngressPMF1;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_ovs_index));

    /*
     * create fg1 for tcam ovf
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 5;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.qual_types[2] = bcmFieldQualifyIp4Protocol;
    fg_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    fg_info.qual_types[4] = bcmFieldQualifyL4DstPort;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = action_ovs_index;
    fg_info.action_with_valid_bit[0] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy(dest_char, "ARMOR_TCAM_OVF", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add
                    (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_GROUP_CREATE_WITH_ID, &fg_info, &fg_id));

    /*
     * create fg attach for ovf tcam
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 1;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[2].input_arg = 1;
    attach_info.key_info.qual_info[2].offset = 0;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 2;
    attach_info.key_info.qual_info[3].offset = 0;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[4].input_arg = 2;
    attach_info.key_info.qual_info[4].offset = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    /*
     * create qualifier for vw_ARMOR___sip_group
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "vw_ARMOR___sip_group", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_get));

    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s bcm_field_name_to_id returned %d IDs.\n", proc_name, nof_ids);
    }
    vw_sip_qual = id_get;

    /*
     * create qualifier for vw_ARMOR___dip_group
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "vw_ARMOR___dip_group", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_get));
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s bcm_field_name_to_id returned %d IDs.\n", proc_name, nof_ids);
    }
    vw_dip_qual = id_get;

    /*
     * create user qualifier
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 14;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "TCAM_OVF_INDEX_QUALIFIER", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id));

    /*
     * create user action for destination
     */
    bcm_field_action_info_t_init(&action_info);
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "USER_DST_DATA", sizeof(action_info.name));

    action_info.action_type = bcmFieldActionForward;
    action_info.prefix_size = 16;
    action_info.prefix_value = 0xc;
    action_info.size = 16;
    action_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &user_action_dst));

    /*
     * create user action for statid
     */
    bcm_field_action_info_t_init(&action_info);
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "USER_STATISTICS_ID_0", sizeof(action_info.name));

    action_info.action_type = bcmFieldActionStatId0;
    action_info.prefix_size = 8;
    action_info.prefix_value = 0x0;
    action_info.size = 12;
    action_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &user_action_stat));

    /*
     * create user action for statprofile
     */
    bcm_field_action_info_t_init(&action_info);
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "USER_STATISTICS_ATR_0", sizeof(action_info.name));

    action_info.action_type = bcmFieldActionStatProfile0Raw;
    action_info.prefix_size = 4;
    action_info.prefix_value = 0x8;
    action_info.size = 1;
    action_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &user_action_stat_profile));

    /*
     * create fg for tcam compressed
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 6;
    fg_info.qual_types[0] = vw_sip_qual;
    fg_info.qual_types[1] = vw_dip_qual;
    fg_info.qual_types[2] = qual_id;
    fg_info.qual_types[3] = bcmFieldQualifyIp4Protocol;
    fg_info.qual_types[4] = bcmFieldQualifyL4SrcPort;
    fg_info.qual_types[5] = bcmFieldQualifyL4DstPort;

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = user_action_dst;
    fg_info.action_types[1] = user_action_stat;
    fg_info.action_types[2] = user_action_stat_profile;

    dest_char = &(fg_info.name[0]);
    sal_strncpy(dest_char, "ARMOR_TCAM_COMPRESSED", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add
                    (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_GROUP_CREATE_WITH_ID, &fg_info, &fg_id2));

    /*
     * create fg attach for tcam compressed
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info.key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info.key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info.key_info.qual_types[5] = fg_info.qual_types[5];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeCascaded;
    attach_info.key_info.qual_info[2].input_arg = fg_id;
    bcm_field_group_action_offset_get(unit, 0, fg_id, action_ovs_index, &action_offset);
    attach_info.key_info.qual_info[2].offset = action_offset;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 0;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[4].input_arg = 2;
    attach_info.key_info.qual_info[4].offset = 0;
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[5].input_arg = 2;
    attach_info.key_info.qual_info[5].offset = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id2, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
minicon_armor_dbs_create(
    int unit,
    bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    rv = minicon_armor_pmf_compressed_tcam(unit, context_id);
    if (rv == BCM_E_EXISTS)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }
    else if (rv != BCM_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Error, in minicon_armor_dbs_create\n");
    }

    SHR_IF_ERR_EXIT(minicon_armor_add_default_compressed_entry(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
minicon_armor_load_config(
    int unit,
    char *filename)
{
    void *cur_top, *cur_res, *cur_field, *cur_sub;
    char field_name[RHNAME_MAX_SIZE];
    char field_val[RHNAME_MAX_SIZE];
    char old_str[RHFILE_MAX_SIZE];
    rhhandle_t rhhandle;
    armor_rename_str_struct_t *rename_handle;

    SHR_FUNC_INIT_VARS(unit);

    cur_top = NULL;

    cur_top = dbx_file_get_xml_top(unit, filename, "Root", CONF_OPEN_CURRENT_LOC);
    if (cur_top == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Cannot find input xml file in: %s \n", filename);
    }

    /*
     * check for null pointer?
     */
    cur_res = dbx_xml_child_get_first(cur_top, "Table");
    if (cur_res != NULL)
    {

        RHDATA_ITERATOR(cur_sub, cur_res, "Field")
        {

            RHDATA_GET_STR_STOP(cur_sub, "Name", field_name);
            sprintf(old_str, "<Entries Table=\"%s\">", field_name);

            if ((cur_field = dbx_xml_child_get_first(cur_res, "NewField")) != NULL)
            {
                RHDATA_GET_STR_DEF_NULL(cur_field, "Name", field_name);
                if (!ISEMPTY(field_name))
                {
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                                    (minicon_armor_appl_info[unit]->table_rename_list, NULL, RHID_TO_BE_GENERATED,
                                     &rhhandle));
                    rename_handle = rhhandle;
                    sal_strcpy(rename_handle->key_string, old_str);
                    sprintf(rename_handle->rename_string, "<Entries Table=\"%s\">", field_name);
                }
            }
        }
    }

    /*
     * check for null pointer?
     */
    cur_res = dbx_xml_child_get_first(cur_top, "Result");
    if (cur_res != NULL)
    {
        RHDATA_ITERATOR(cur_sub, cur_res, "Field")
        {

            RHDATA_GET_STR_DEF_NULL(cur_sub, "Name", field_name);
            RHDATA_GET_STR_DEF_NULL(cur_sub, "Value", field_val);

            sprintf(old_str, "Name=\"%s\" Value=\"%s\"", field_name, field_val);

            if ((cur_field = dbx_xml_child_get_first(cur_sub, "NewField")) != NULL)
            {
                RHDATA_GET_STR_DEF_NULL(cur_field, "Name", field_name);
                RHDATA_GET_STR_DEF_NULL(cur_field, "Value", field_val);
                if (!ISEMPTY(field_name) && !ISEMPTY(field_val))
                {
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                                    (minicon_armor_appl_info[unit]->field_rename_list, NULL, RHID_TO_BE_GENERATED,
                                     &rhhandle));
                    rename_handle = rhhandle;
                    sal_strcpy(rename_handle->key_string, old_str);
                    sprintf(rename_handle->rename_string, "Name=\"%s\" Value=\"%s\"", field_name, field_val);
                }
            }

            if ((cur_field = dbx_xml_child_get_first(cur_sub, "AddField")) != NULL)
            {
                RHDATA_GET_STR_DEF_NULL(cur_field, "Name", field_name);
                RHDATA_GET_STR_DEF_NULL(cur_field, "Value", field_val);
                if (!ISEMPTY(field_name) && !ISEMPTY(field_val))
                {
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                                    (minicon_armor_appl_info[unit]->field_add_list, NULL, RHID_TO_BE_GENERATED,
                                     &rhhandle));
                    rename_handle = rhhandle;
                    sal_strcpy(rename_handle->key_string, old_str);
                    sprintf(rename_handle->rename_string, "<Field Name=\"%s\" Value=\"%s\"/>", field_name, field_val);
                }
            }

        }
    }

exit:
    if (cur_top != NULL)
    {
        dbx_xml_top_close(cur_top);
    }
    SHR_FUNC_EXIT;
}

/** Counter config */
shr_error_e
minicon_armor_counter_config(
    int unit,
    int core)
{
    bcm_stat_counter_interface_key_t interface_key;
    int command_id = 0x0;
    bcm_stat_expansion_select_t expansion_select;
    bcm_stat_counter_database_t database;
    int database_id = 0x0;
    bcm_stat_engine_t engine;
    bcm_stat_eviction_t eviction;
    int eviction_flags = 0;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_expansion_data_mapping_t data_mapping_array[1];
    bcm_stat_counter_enable_t enable_config;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * configure counter expension
     */
    bcm_stat_counter_interface_key_t_init(&interface_key);
    interface_key.core_id = core;
    interface_key.command_id = command_id;
    interface_key.interface_source = bcmStatCounterInterfaceIngressReceivePp;
    interface_key.type_id = 0;

    bcm_stat_expansion_select_t_init(&expansion_select);

    SHR_IF_ERR_EXIT(bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select));

    /*
     * configure counter db
     */
    bcm_stat_counter_database_t_init(&database);
    database.database_id = database_id;
    database.core_id = core;

    SHR_IF_ERR_EXIT(bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, &database_id));

    /*
     * associate counter db with engine
     */
    bcm_stat_engine_t_init(&engine);
    engine.core_id = core;
    engine.engine_id = 0;

    SHR_IF_ERR_EXIT(bcm_stat_counter_engine_attach(unit, 0, &database, engine.engine_id));

    /*
     * configure counter eviction
     */
    bcm_stat_eviction_t_init(&eviction);
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
    eviction.type = bcmStatEvictionDestinationLocalHost;

    SHR_IF_ERR_EXIT(bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction));

    /*
     * configure counter interface
     */
    bcm_stat_counter_interface_t_init(&cnt_interface);
    cnt_interface.source = bcmStatCounterInterfaceIngressReceivePp;
    cnt_interface.command_id = command_id;
    cnt_interface.format_type = bcmStatCounterFormatPackets;
    cnt_interface.counter_set_size = 1;
    cnt_interface.type_config[0].valid = 1;
    cnt_interface.type_config[0].object_type_offset = 0;
    cnt_interface.type_config[0].start = 0;
    cnt_interface.type_config[0].end = 0x0fff;

    SHR_IF_ERR_EXIT(bcm_stat_counter_interface_set(unit, 0, &database, &cnt_interface));

    /*
     * configure counter set
     */
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_expansion_data_mapping_t_init(data_mapping_array);
    data_mapping_array[0].nof_key_conditions = 0;
    data_mapping_array[0].value.counter_set_offset = 0;
    data_mapping_array[0].value.valid = TRUE;
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    ctr_set_map.nof_entries = 1;
    SHR_IF_ERR_EXIT(bcm_stat_counter_set_mapping_set(unit, 0, &database, &ctr_set_map));

    /*
     * enable counter db
     */
    bcm_stat_counter_enable_t_init(&enable_config);
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    SHR_IF_ERR_EXIT(bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
minicon_armor_counter_get(
    int unit,
    uint32 core_id,
    uint32 counter_id,
    uint64 *val)
{
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    int stat_array[1];
    bcm_stat_counter_value_t counter_val[1];

    SHR_FUNC_INIT_VARS(unit);

    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);

    stat_array[0] = BCM_STAT_COUNTER_STAT_SET(0, 0);

    input_data.core_id = core_id;
    input_data.database_id = 0;
    input_data.type_id = 0;
    input_data.counter_source_id = counter_id;
    input_data.nstat = 1;
    input_data.stat_arr = stat_array;
    output_data.value_arr = counter_val;

    SHR_IF_ERR_EXIT(bcm_stat_counter_get(unit, 0, &input_data, &output_data));

    *val = counter_val[0].value;

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
minicon_armor_init(
    int unit,
    bcm_field_context_t tcams_context_id,
    char *config_file)
{

    int i;
    SHR_FUNC_INIT_VARS(unit);

    if (minicon_armor_appl_info == NULL)
    {
        if ((minicon_armor_appl_info =
             sal_alloc(BCM_MAX_NUM_UNITS * sizeof(armor_struct_t *), "minicon_armor_appl_info")) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - No memory for armor info\n");
        }
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++)
        {
            minicon_armor_appl_info[i] = NULL;
        }
    }
    if (minicon_armor_appl_info[unit] != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "ERROR - already initialized\n");
    }

    if ((minicon_armor_appl_info[unit] = sal_alloc(sizeof(armor_struct_t), "armor_appl_info_unit")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - No memory for armor info unit\n");
    }

    if ((minicon_armor_appl_info[unit]->table_rename_list =
         utilex_rhlist_create("TablesRename", sizeof(armor_rename_str_struct_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - No memory for table rename list\n");
    }
    if ((minicon_armor_appl_info[unit]->field_rename_list =
         utilex_rhlist_create("FieldsRename", sizeof(armor_rename_str_struct_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - No memory for field rename list\n");
    }
    if ((minicon_armor_appl_info[unit]->field_add_list =
         utilex_rhlist_create("FieldsAdd", sizeof(armor_rename_str_struct_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - No memory for field add list\n");
    }
    if ((minicon_armor_appl_info[unit]->dbal_tables = utilex_rhlist_create("DBs", sizeof(rhentry_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - No memory for dbal tables list\n");
    }

    if (ISEMPTY(config_file))
    {
        LOG_WARN_EX(BSL_LOG_MODULE, "Config file was not provided %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(minicon_armor_load_config(unit, config_file));
    }

    SHR_IF_ERR_EXIT(minicon_armor_dbs_create(unit, tcams_context_id));

    SHR_IF_ERR_EXIT(minicon_armor_prepare_dbal_tables_list(unit));

    SHR_IF_ERR_EXIT(minicon_armor_counter_config(unit, BCM_CORE_ALL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
armor_convert_to_dbal_readable(
    int unit,
    char *in_file,
    char *out_file)
{
    FILE *in_fp;
    FILE *out_fp;
    char read_str[RHFILE_MAX_SIZE];
    char copy_str[RHFILE_MAX_SIZE];
    char *str_ptr;
    int found;

    rhhandle_t rhhandle;
    armor_rename_str_struct_t *rename_handle;

    SHR_FUNC_INIT_VARS(unit);

    if ((in_fp = sal_fopen(in_file, "r")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - cannot open read file %s\n", in_file);
    }
    if ((out_fp = sal_fopen(out_file, "w")) == NULL)
    {
        fclose(in_fp);
        SHR_ERR_EXIT(_SHR_E_MEMORY, "ERROR - cannot open write file %s\n", out_file);
    }
    while (sal_fgets(read_str, RHSTRING_MAX_SIZE, in_fp) != NULL)
    {
        found = 0;
        str_ptr = read_str;
        utilex_str_shrink(str_ptr);
        sal_strcpy(copy_str, str_ptr);

        RHITERATOR(rhhandle, minicon_armor_appl_info[unit]->table_rename_list)
        {
            rename_handle = rhhandle;
            if (sal_strstr(str_ptr, rename_handle->key_string) != NULL)
            {
                utilex_sub_string_replace(str_ptr, rename_handle->key_string, rename_handle->rename_string);
                sal_fputs(str_ptr, out_fp);
                sal_fputs("\n", out_fp);
                found = 1;
                break;
            }
        }
        if (found)
        {
            continue;
        }
        RHITERATOR(rhhandle, minicon_armor_appl_info[unit]->field_rename_list)
        {
            rename_handle = rhhandle;
            if (sal_strstr(str_ptr, rename_handle->key_string) != NULL)
            {
                utilex_sub_string_replace(str_ptr, rename_handle->key_string, rename_handle->rename_string);
                sal_fputs(str_ptr, out_fp);
                sal_fputs("\n", out_fp);
                found = 1;
                break;
            }
        }
        if (!found)
        {
            sal_fputs(str_ptr, out_fp);
            sal_fputs("\n", out_fp);
        }
        RHITERATOR(rhhandle, minicon_armor_appl_info[unit]->field_add_list)
        {
            rename_handle = rhhandle;
            if (sal_strstr(copy_str, rename_handle->key_string) != NULL)
            {
                sal_fputs(rename_handle->rename_string, out_fp);
                sal_fputs("\n", out_fp);
                break;
            }
        }

    }

    fclose(in_fp);
    fclose(out_fp);

exit:

    SHR_FUNC_EXIT;
}
