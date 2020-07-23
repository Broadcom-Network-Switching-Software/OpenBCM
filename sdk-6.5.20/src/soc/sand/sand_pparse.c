
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

#include <sal/appl/sal.h>

#include <shared/bitop.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>

#include <soc/sand/sand_pparse.h>
#include <soc/drv.h>

#include <soc/sand/auto_generated/dbx_pre_compiled_common.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#endif

#include "sand_signals_internal.h"

shr_error_e
pparse_init(
    int unit,
    parsing_db_t * pdb)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(pdb, 0, sizeof(parsing_db_t));

#if (!defined(NO_FILEIO))
    SHR_IF_ERR_EXIT(pparse_pdb_init(unit, "DNX-Devices.xml", 0, pdb));
#else
    SHR_EXIT_WITH_LOG(_SHR_E_UNAVAIL, "PParse DB is not available when compiled without file system. %s%s%s\n", EMPTY,
                      EMPTY, EMPTY);
#endif

exit:
    SHR_FUNC_EXIT;
}

void
pparse_destroy(
    parsing_db_t * pdb)
{
    pparse_db_entry_t *obj = NULL;

    utilex_rhlist_free_all(pdb->next_headers_lst);
    RHITERATOR(obj, pdb->obj_lst)
    {
        int ext_i;
        for (ext_i = 0; ext_i < obj->extensions_nof; ++ext_i)
        {
            int cond_i;
            pparse_object_extension_t *ext = &obj->extensions[ext_i];
            for (cond_i = 0; cond_i < ext->conditions_nof; ++cond_i)
            {
                pparse_condition_t *cond = &ext->conditions[cond_i];
                if (cond->type == PPARSE_CONDITION_TYPE_PLUGIN)
                {
                    if (cond->content.plugin.args)
                    {
                        utilex_rhlist_free_all(cond->content.plugin.args);
                    }
                }
            }
        }
    }
    utilex_rhlist_free_all(pdb->obj_lst);
    sal_memset(pdb, 0, sizeof(parsing_db_t));
}

shr_error_e
pparse_packet_process(
    int unit,
    int core,
    parsing_db_t * parsing_db,
    pparse_packet_info_t * packet_info,
    rhlist_t ** pit)
{

    rhlist_t *parse_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");
    
    if (NULL == *pit)
    {
        SHR_IF_ERR_EXIT(pparse_parsed_info_tree_create(unit, pit));
    }
    
    SHR_IF_ERR_EXIT(pparse_list_create(unit, &parse_list));
    
    SHR_IF_ERR_EXIT(pparse_list_push(unit, packet_info->data_desc, parse_list));
    
    {
        rhhandle_t orig_last = utilex_rhlist_entry_get_last(*pit);
        
        SHR_IF_ERR_EXIT(pparse_list_process(unit, core, parsing_db, packet_info, parse_list, *pit));
        
        SHR_IF_ERR_EXIT(pparse_parsed_list_duplicates_enumerate(unit, *pit, orig_last));
    }
exit:
    pparse_list_free(unit, parse_list);
    SHR_FUNC_EXIT;
}

shr_error_e
pparse_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE])
{
    parsing_db_t pdb;
    pparse_packet_info_t pkt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(pparse_init(unit, &pdb));

    SHR_BITCOPY_RANGE(pkt.data, 0, data, 0, data_size_bits);
    pkt.data_desc = desc;
    pkt.data_size = data_size_bits;

    SHR_IF_ERR_EXIT(pparse_packet_process(unit, core, &pdb, &pkt, parsed_info));

exit:
    pparse_destroy(&pdb);
    SHR_FUNC_EXIT;
}

static shr_error_e pparse_packet_compose(
    int unit,
    parsing_db_t * pdb,
    rhlist_t * packet_objects,
    uint32 packet_size_bytes,
    uint32 max_data_size_bits,
    uint32 *data,
    uint32 *data_size_bits);

shr_error_e
pparse_compose(
    int unit,
    rhlist_t * packet_objects,
    uint32 packet_size_bytes,
    uint32 max_data_size_bits,
    uint32 *data,
    uint32 *data_size_bits)
{
    parsing_db_t pdb = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_objects, _SHR_E_PARAM, "packet_objects");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_NULL_CHECK(data_size_bits, _SHR_E_PARAM, "data_size_bits");

    SHR_IF_ERR_EXIT(pparse_init(unit, &pdb));

    SHR_IF_ERR_EXIT(pparse_packet_compose(unit, &pdb, packet_objects, packet_size_bytes,
                                          max_data_size_bits, data, data_size_bits));

exit:
    pparse_destroy(&pdb);
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_field_list_append_flattened(
    int unit,
    signal_output_t * hdr,
    int start_bit,
    const char *prefix,
    rhlist_t * res_list,
    uint8 is_network)
{
    signal_output_t *field;

    SHR_FUNC_INIT_VARS(unit);
    
    RHITERATOR(field, hdr->field_list)
    {
        char full_name[RHNAME_MAX_SIZE];
        int field_start;
        int field_end;
        sal_snprintf(full_name, RHNAME_MAX_SIZE, "%s.%s", prefix, RHNAME(field));

        if (is_network)
        {
            field_start = start_bit + (hdr->size - 1 - field->end_bit);
            field_end = start_bit + (hdr->size - 1 - field->start_bit);
        }
        else
        {
            field_start = start_bit + field->start_bit;
            field_end = start_bit + field->end_bit;
        }

        if ((!field->field_list) || (field->field_list->num == 0))
        {
            
            signal_output_t *added_field;
            rhhandle_t rhhandle;
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(res_list, full_name, RHID_TO_BE_GENERATED, &rhhandle));
            added_field = rhhandle;
            added_field->start_bit = field_start;
            added_field->end_bit = field_end;
        }
        else
        {
            
            SHR_IF_ERR_EXIT(pparse_field_list_append_flattened(unit, field, field_start,
                                                               full_name, res_list, is_network));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_combine(
    int unit,
    rhlist_t * sig_list,
    signal_output_t * combined_sig,
    uint8 is_network)
{
    signal_output_t *sig;
    uint32 fake_data[DSIG_MAX_SIZE_UINT32];
    rhlist_t *headers_list = NULL;

    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(sig_list, _SHR_E_PARAM, "sig_list");
    SHR_NULL_CHECK(combined_sig, _SHR_E_PARAM, "combined_sig");
    
    if (!combined_sig->field_list)
    {
        if ((combined_sig->field_list = utilex_rhlist_create("headers", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "Could not allocate field list. %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
    }
    
    sal_memset(fake_data, 0, sizeof(fake_data));
    
    if ((headers_list = utilex_rhlist_create("headers_list", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "Couldn't allocate headers list %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    
    RHITERATOR(sig, sig_list)
    {
        signal_output_t *header;
        SHR_IF_ERR_EXIT(sand_signal_parse_get(unit, RHNAME(sig), NULL, NULL, NULL, PRINT_BIG_ENDIAN, fake_data,
                                              DSIG_MAX_SIZE_BITS, headers_list));
        header = (signal_output_t *) headers_list->tail;
        if (!ISEMPTY(sig->expansion))
        {
            
            char prefix[RHNAME_MAX_SIZE];
            sal_snprintf(prefix, RHNAME_MAX_SIZE, "%s_%s", sig->expansion, RHNAME(header));
            SHR_IF_ERR_EXIT(pparse_field_list_append_flattened(unit, header, combined_sig->size,
                                                               prefix, combined_sig->field_list, is_network));
        }
        else
        {
            SHR_IF_ERR_EXIT(pparse_field_list_append_flattened(unit, header, combined_sig->size,
                                                               RHNAME(sig), combined_sig->field_list, is_network));
        }
        combined_sig->size += header->size;
    }

exit:
    if (headers_list)
    {
        sand_signal_list_free(headers_list);
    }
    SHR_FUNC_EXIT;
}


#if (!defined(NO_FILEIO))


static shr_error_e
pparse_pdb_cond_type_get(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t * cond)
{
    char type_name[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_DEF_NULL(cond_node, "type", type_name);
    
    if (ISEMPTY(type_name))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY,
                          "Condition on parsing object %s has no type (supported: 'soc' or 'field'). %s%s\n",
                          obj->entry.name, EMPTY, EMPTY);
    }
    
    
    if (!sal_strcasecmp(type_name, "soc"))
    {
        cond->type = PPARSE_CONDITION_TYPE_SOC;
    }
    else if (!sal_strcasecmp(type_name, "field"))
    {
        cond->type = PPARSE_CONDITION_TYPE_FIELD;
    }
    else if (!sal_strcasecmp(type_name, "plugin"))
    {
        cond->type = PPARSE_CONDITION_TYPE_PLUGIN;
    }
    else
    {
        
        SHR_EXIT_WITH_LOG(_SHR_E_UNAVAIL, "Condition on parsing object %s has unsupported type: %s.%s\n",
                          obj->entry.name, type_name, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_pdb_cond_mode_get(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t * cond)
{
    char mode_s[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_DEF_NULL(cond_node, "mode", mode_s);
    
    
    if (ISEMPTY(mode_s) || !sal_strcasecmp(mode_s, "enable"))
    {
        cond->mode = PPARSE_CONDITION_MODE_ENABLE;
    }
    else if (!sal_strcasecmp(mode_s, "disable"))
    {
        cond->mode = PPARSE_CONDITION_MODE_DISABLE;
    }
    else
    {
        
        SHR_EXIT_WITH_LOG(_SHR_E_UNAVAIL, "Condition on parsing object %s has unsupported mode: %s.%s\n",
                          obj->entry.name, mode_s, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_pdb_cond_field_parse(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t * cond)
{
    xml_node field_node, enable_node;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == (field_node = dbx_xml_child_get_first(cond_node, "field")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Field type condition has no <field> node %s%s", obj->entry.name,
                          EMPTY, EMPTY);
    }
    RHDATA_GET_STR_DEF_NULL(field_node, "path", cond->content.field.path);
    if (ISEMPTY(cond->content.field.path))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - object with Field condition has no path %s%s", obj->entry.name,
                          EMPTY, EMPTY);
    }

    if (NULL == (enable_node = dbx_xml_child_get_first(cond_node, "enable")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Enable condition has no <enable> node %s%s", obj->entry.name,
                          EMPTY, EMPTY);
    }
    RHDATA_GET_INT_DEF(enable_node, "val", cond->content.field.enable_value, 1);
    RHDATA_GET_INT_DEF(enable_node, "mask", cond->content.field.enable_mask, -1);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_pdb_cond_soc_get(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t * cond)
{
    xml_node soc_node, enable_node;

    char *socval;
    char socname[RHSTRING_MAX_SIZE];
    char expval[RHSTRING_MAX_SIZE];
    uint32 defval = 0;          

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == (soc_node = dbx_xml_child_get_first(cond_node, "soc_property")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - SOC type condition has no <soc_property> node %s%s",
                          obj->entry.name, EMPTY, EMPTY);
    }
    RHDATA_GET_STR_DEF_NULL(soc_node, "name", socname);
    if (ISEMPTY(socname))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - SOC type condition has empty <soc_property> node %s%s",
                          obj->entry.name, EMPTY, EMPTY);
    }

    if (NULL == (enable_node = dbx_xml_child_get_first(cond_node, "enable")))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Enable condition has no <enable> node %s%s", obj->entry.name,
                          EMPTY, EMPTY);
    }
    RHDATA_GET_STR_DEF_NULL(enable_node, "val", expval);
    if (ISEMPTY(expval))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_EMPTY, "Parsing: %s - Enable condition has empty <enable> node %s%s", obj->entry.name,
                          EMPTY, EMPTY);
    }
    socval = soc_property_get_str(unit, socname);
    cond->content.soc.enabled = ((NULL == socval) ? defval : !strcmp(socval, expval));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_pdb_cond_plugin_add(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_condition_t * cond)
{
    xml_node arg_node;
    rhlist_t *arg_list = NULL;
    char cb_name[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    RHDATA_GET_STR_DEF_NULL(cond_node, "callback", cb_name);
    if (ISEMPTY(cb_name))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, Plugin condition without callback.");
    }
#ifdef BCM_DNX_SUPPORT
    if (!sal_strcasecmp(cb_name, "packet_ptch2"))
    {
        cond->content.plugin.callback = packet_ptch2;
    }
    if (!sal_strcasecmp(cb_name, "packet_ptch1"))
    {
        cond->content.plugin.callback = packet_ptch1;
    }
    if (!sal_strcasecmp(cb_name, "packet_rch"))
    {
        cond->content.plugin.callback = packet_rch;
    }
    if (!sal_strcasecmp(cb_name, "packet_itmh"))
    {
        cond->content.plugin.callback = packet_itmh;
    }
    if (!sal_strcasecmp(cb_name, "packet_ftmh_opt"))
    {
        cond->content.plugin.callback = packet_ftmh_opt;
    }
#endif
    if (cond->content.plugin.callback == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown callback: '%s'", cb_name);
    }
    
    if (NULL == (arg_list = utilex_rhlist_create("arg_list", sizeof(packet_plugin_arg_t), 1)))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Couldn't allocate argument list for plugin");
    }
    RHDATA_ITERATOR(arg_node, cond_node, "plugin-arg")
    {
        rhhandle_t rhhandle;
        packet_plugin_arg_t *arg_entry;
        char arg_name[RHSTRING_MAX_SIZE + 1];
        char arg_value[RHSTRING_MAX_SIZE];
        RHDATA_GET_LSTR_CONT(arg_node, "Name", arg_name);
        if (sal_strnlen(arg_name, RHSTRING_MAX_SIZE) > RHNAME_MAX_SIZE)
        {
            arg_name[RHSTRING_MAX_SIZE] = '\0';
            SHR_ERR_EXIT(_SHR_E_PARAM, "Argument name too long: '%s'", arg_name);
        }
        RHDATA_GET_LSTR_CONT(arg_node, "Value", arg_value);
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(arg_list, arg_name, RHID_TO_BE_GENERATED, &rhhandle));
        arg_entry = rhhandle;
        sal_strncpy_s(arg_entry->value, arg_value, RHNAME_MAX_SIZE);
    }
    cond->content.plugin.args = arg_list;
exit:
    
    
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_parse_db_add_condition(
    int unit,
    xml_node cond_node,
    pparse_db_entry_t * obj,
    pparse_object_extension_t * ext)
{
    pparse_condition_t *cond = &ext->conditions[ext->conditions_nof];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cond_node, _SHR_E_EMPTY, "cond_node");

    SHR_IF_ERR_EXIT(pparse_pdb_cond_type_get(unit, cond_node, obj, cond));
    SHR_IF_ERR_EXIT(pparse_pdb_cond_mode_get(unit, cond_node, obj, cond));

    switch (ext->conditions[ext->conditions_nof].type)
    {
        case PPARSE_CONDITION_TYPE_FIELD:
            SHR_IF_ERR_EXIT(pparse_pdb_cond_field_parse(unit, cond_node, obj, cond));
            break;
        case PPARSE_CONDITION_TYPE_SOC:
            SHR_IF_ERR_EXIT(pparse_pdb_cond_soc_get(unit, cond_node, obj, cond));
            break;
        case PPARSE_CONDITION_TYPE_PLUGIN:
            SHR_IF_ERR_EXIT(pparse_pdb_cond_plugin_add(unit, cond_node, obj, cond));
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Something went wrong. %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    ext->conditions_nof++;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_pdb_includes_add(
    int unit,
    xml_node cur_top,
    parsing_db_t * pdb)
{
    xml_node cur_include = NULL;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_ITERATOR(cur_include, cur_top, "include")
    {
        char filename[RHSTRING_MAX_SIZE];
        char type[RHSTRING_MAX_SIZE];
        int dev_specific = 0;
        RHDATA_GET_STR_CONT(cur_include, "file", filename);
        RHDATA_GET_INT_DEF(cur_include, "device_specific", dev_specific, 0);
        RHDATA_GET_STR_DEF(cur_include, "type", type, "objects");
        
        if (!sal_strcasecmp(type, "objects"))
        {
            SHR_IF_ERR_EXIT(pparse_pdb_init(unit, filename, dev_specific, pdb));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_pdb_objects_add(
    int unit,
    xml_node cur_top,
    parsing_db_t * pdb)
{
    xml_node obj_node;
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_ITERATOR(obj_node, cur_top, "object")
    {
        pparse_db_entry_t *obj;
        rhhandle_t rhhandle;
        char name[RHNAME_MAX_SIZE] = { 0 };
        char parsing_cb_n[RHNAME_MAX_SIZE] = { 0 };
        xml_node ext_node, desc_node, post_compose_node;
        int override = 0;
        
        RHDATA_GET_STR_CONT(obj_node, "name", name);
        RHDATA_GET_INT_DEF(obj_node, "override", override, 0);
        if(utilex_rhlist_name_exists(pdb->obj_lst, name, &rhhandle) == _SHR_E_NONE)
        {
            if (override)
            {
                SHR_IF_ERR_EXIT(utilex_rhlist_entry_del_free(pdb->obj_lst, rhhandle));
            }
            else
            {
                obj = rhhandle;
                if (obj->override)
                {
                    continue;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "object %s was already defined", name);
                }
            }
        }
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(pdb->obj_lst, name, RHID_TO_BE_GENERATED, &rhhandle));
        obj = rhhandle;
        obj->override = override;
        obj->extensions_nof = 0;
        
        RHDATA_GET_INT_CONT(obj_node, "size", obj->size);
        RHDATA_GET_STR_DEF_NULL(obj_node, "structure", obj->struct_name);
        if (ISEMPTY(obj->struct_name))
        {
            sal_strncpy_s(obj->struct_name, name, RHNAME_MAX_SIZE);
        }
        RHDATA_GET_STR_DEF_NULL(obj_node, "block", obj->block_name);
        RHDATA_GET_STR_DEF_NULL(obj_node, "from", obj->stage_from_name);

         
        RHDATA_GET_INT_DEF(obj_node, "port_skip", obj->port_skip, 0);

        obj->parsing_cb = pparse_sig_parse_wrapper_cb;
        RHDATA_GET_STR_DEF_NULL(obj_node, "plugin", parsing_cb_n);
        if (!ISEMPTY(parsing_cb_n))
        {
            
            if (!sal_strcasecmp(parsing_cb_n, "eth_parser"))
            {
                obj->parsing_cb = pparse_eth_parser_cb;
            }
            else if (!sal_strcasecmp(parsing_cb_n, "ipv4_parser"))
            {
                obj->parsing_cb = pparse_ipv4_parser_cb;
            }
            else if (!sal_strcasecmp(parsing_cb_n, "2nd_parser"))
            {
                obj->parsing_cb = pparse_2nd_parser_cb;
            }
            else if (!sal_strcasecmp(parsing_cb_n, "srv6_parser"))
            {
                obj->parsing_cb = pparse_srv6_parser_cb;
            }
            else if (!sal_strcasecmp(parsing_cb_n, "ipv6_ext_parser"))
            {
                obj->parsing_cb = pparse_ipv6_ext_parser_cb;
            }
            else if (!sal_strcasecmp(parsing_cb_n, "ipv6_ext_ah_parser"))
            {
                obj->parsing_cb = pparse_ipv6_ext_ah_parser_cb;
            }
        }
        if (NULL != (desc_node = dbx_xml_child_get_first(obj_node, "description")))
        {
            if (!dbx_xml_node_get_content_str(desc_node, obj->desc_str, RHSTRING_MAX_SIZE))
            {
                
                obj->desc_str[0] = '\0';
            }
        }
        
        RHDATA_ITERATOR(ext_node, obj_node, "extension")
        {
            xml_node cond_node;
            pparse_object_extension_t *ext = &obj->extensions[obj->extensions_nof];
            ext->conditions_nof = 0;
            RHDATA_GET_STR_CONT(ext_node, "name", ext->object_name);
            RHDATA_ITERATOR(cond_node, ext_node, "condition")
            {
                SHR_IF_ERR_EXIT(pparse_parse_db_add_condition(unit, cond_node, obj, ext));
            }
            obj->extensions_nof++;
        }
        
        RHDATA_ITERATOR(ext_node, obj_node, "next-header")
        {
            char next_headers_list_name[RHNAME_MAX_SIZE];
            char next_header_field_path[RHSTRING_MAX_SIZE];

            if (obj->next_header_options)
            {
                
                SHR_ERR_EXIT(_SHR_E_EXISTS, "Next header already defined for parsing object '%s'", RHNAME(obj));
            }
            RHDATA_GET_STR_CONT(ext_node, "options", next_headers_list_name);
            RHDATA_GET_STR_CONT(ext_node, "field", next_header_field_path);
            obj->next_header_options = utilex_rhlist_entry_get_by_name(pdb->next_headers_lst, next_headers_list_name);
            sal_strncpy_s(obj->next_header_field, next_header_field_path, RHSTRING_MAX_SIZE);
        }
        
        if (NULL != (post_compose_node = dbx_xml_child_get_first(obj_node, "post-compose")))
        {
            char post_process_cb_name[RHNAME_MAX_SIZE] = { 0 };
            RHDATA_GET_STR_DEF_NULL(post_compose_node, "plugin", post_process_cb_name);
            
            if (!sal_strcasecmp(post_process_cb_name, "ipv4_post_processing"))
            {
                obj->post_processing_cb = pparse_ipv4_post_processor;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_pdb_next_headers_add(
    int unit,
    xml_node cur_top,
    parsing_db_t * pdb)
{
    xml_node next_hdr_options;
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_ITERATOR(next_hdr_options, cur_top, "next-header-options")
    {
        xml_node option_node;
        char name[RHNAME_MAX_SIZE];
        rhhandle_t rhhandle;
        next_headers_t *nxt_headers = NULL;

        RHDATA_GET_STR_CONT(next_hdr_options, "name", name);
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(pdb->next_headers_lst, name, RHID_TO_BE_GENERATED, &rhhandle));
        nxt_headers = rhhandle;
        if ((nxt_headers->option_lst = utilex_rhlist_create("option_list", sizeof(next_header_option_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsing DB strucutre.\n");
        }

        RHDATA_ITERATOR(option_node, next_hdr_options, "option")
        {
            char obj_name[RHNAME_MAX_SIZE];
            int field_val = 0;
            next_header_option_t *option_obj = NULL;
            RHDATA_GET_STR_CONT(option_node, "object", obj_name);
            RHDATA_GET_INT_CONT(option_node, "val", field_val);
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                            (nxt_headers->option_lst, obj_name, RHID_TO_BE_GENERATED, &rhhandle));
            option_obj = rhhandle;
            option_obj->field_value = field_val;
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_pdb_init(
    int unit,
    char *filename,
    uint8 device_specific,
    parsing_db_t * pdb)
{
    xml_node cur_top = NULL, parsing_objs_node = NULL, next_headers_node = NULL;
    int flags;

    SHR_FUNC_INIT_VARS(unit);
    
    flags = CONF_OPEN_ALTER_LOC | CONF_OPEN_NO_ERROR_REPORT;
    
    if (device_specific)
    {
        flags |= CONF_OPEN_PER_DEVICE;
		cur_top = dbx_pre_compiled_devices_top_get(unit, filename, "top", flags);
   	}
	else
	{
		cur_top = dbx_pre_compiled_common_top_get(unit, filename, "top", flags);
	}
	if (cur_top == NULL)
	{
        
        SHR_EXIT();
    }
    
    if (NULL == pdb->obj_lst)
    {
        if ((pdb->obj_lst = utilex_rhlist_create("pdb_obj_lst", sizeof(pparse_db_entry_t), 1)) == NULL)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Could not allocate parsing DB strucutre. %s%s%s\n", EMPTY, EMPTY,
                              EMPTY);
        }
    }
    if (NULL == pdb->next_headers_lst)
    {
        if ((pdb->next_headers_lst = utilex_rhlist_create("pdb_next_hdrs_lst", sizeof(next_headers_t), 1)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not allocate parsing DB strucutre.\n");
        }
    }
    RHDATA_ITERATOR(next_headers_node, cur_top, "next-headers")
    {
        
        SHR_IF_ERR_EXIT(pparse_pdb_next_headers_add(unit, next_headers_node, pdb));
    }
    RHDATA_ITERATOR(parsing_objs_node, cur_top, "parsing-objects")
    {
        
        SHR_IF_ERR_EXIT(pparse_pdb_includes_add(unit, parsing_objs_node, pdb));
        
        SHR_IF_ERR_EXIT(pparse_pdb_objects_add(unit, parsing_objs_node, pdb));
    }

exit:
    dbx_xml_top_close(cur_top);
    SHR_FUNC_EXIT;
}

#endif 



shr_error_e
pparse_buffer_prepare(
    int unit,
    pparse_parsing_object_t * pobj,
    pparse_packet_info_t * packet_info,
    uint32 *buffer)
{
    uint32 obj_size;
    uint32 *data = packet_info->data;

    SHR_FUNC_INIT_VARS(unit);

    obj_size = pobj->pdb_entry->size;
    if (obj_size > packet_info->data_size)
    {
        uint32 diff = obj_size - packet_info->data_size;
        SHR_BITCLR_RANGE(buffer, 0, diff);
        SHR_BITCOPY_RANGE(buffer, diff, data, 0, (packet_info->data_size));
    }
    else
    {
        SHR_BITCOPY_RANGE(buffer, 0, data, (packet_info->data_size - obj_size), obj_size);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static signal_output_t *
pparse_field_list_entry_get_last_by_name(
    rhlist_t * field_list,
    char *name)
{
    signal_output_t *field = NULL;

    if (!field_list)
    {
        return NULL;
    }

    for (field = utilex_rhlist_entry_get_last(field_list); field; field = utilex_rhlist_entry_get_prev(field))
    {
        if ((!ISEMPTY(field->expansion)) && (!sal_strncasecmp(name, field->expansion, RHNAME_MAX_SIZE)))
        {
            return field;
        }
        if (!sal_strncasecmp(name, RHNAME(field), RHNAME_MAX_SIZE))
        {
            return field;
        }
    }

    return NULL;
}

shr_error_e
pparse_field_val_get(
    int unit,
    char *field_path,
    rhlist_t * pit,
    uint32 *val,
    uint8 *found)
{
    char **path;
    uint32 part_i, parts;

    rhlist_t *field_list = pit;
    signal_output_t *part_sig = NULL;

    uint32 size_to_copy = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == (path = utilex_str_split(field_path, ".", 10, &parts)))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "PParse - Could not parse field path: %s\n", field_path);
    }

    for (part_i = 0; part_i < parts; part_i++)
    {
        if (NULL == (part_sig = pparse_field_list_entry_get_last_by_name(field_list, path[part_i])))
        {
            *found = 0;
            SHR_EXIT();
        }
        

        field_list = part_sig->field_list;
    }
    if (NULL != part_sig)
    {
        *found = 1;
        size_to_copy = part_sig->size > 32 ? 32 : part_sig->size;
        SHR_BITCOPY_RANGE(val, 0, part_sig->value, 0, size_to_copy);
    }
    else
    {
        
        *found = 0;
    }

exit:
    utilex_str_split_free(path, parts);
    SHR_FUNC_EXIT;
}

shr_error_e
pparse_extension_condition_apply(
    int unit,
    int core,
    pparse_condition_t * cond,
    rhlist_t * pit,
    uint8 *valid)
{
    uint32 val = 0, mask = 0;
    uint8 field_found;

    SHR_FUNC_INIT_VARS(unit);

    *valid = 1;
    switch (cond->type)
    {
        case PPARSE_CONDITION_TYPE_SOC:
            if (cond->content.soc.enabled == 0)
            {
                *valid = 0;
            }
            break;
        case PPARSE_CONDITION_TYPE_FIELD:
            mask = cond->content.field.enable_mask;
            SHR_IF_ERR_EXIT(pparse_field_val_get(unit, cond->content.field.path, pit, &val, &field_found));
            if ((!field_found) || ((val & mask) != cond->content.field.enable_value))
            {
                *valid = 0;
            }
            break;
        case PPARSE_CONDITION_TYPE_PLUGIN:
            SHR_IF_ERR_EXIT(cond->content.plugin.callback(unit, core ,
                                                          cond->content.plugin.args, &val));
            *valid = val & 0xff;
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Something went wrong %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_object_extension_check_conditions(
    int unit,
    int core,
    pparse_object_extension_t * db_ext,
    rhlist_t * pit,
    uint8 *valid)
{
    int cond_i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(valid, _SHR_E_INTERNAL, "valid");

    *valid = 1;
    for (cond_i = 0; cond_i < db_ext->conditions_nof; cond_i++)
    {
        pparse_condition_t * cond = &db_ext->conditions[cond_i];
        uint8 cond_val = 0;
        
        SHR_IF_ERR_EXIT(pparse_extension_condition_apply(unit, core, cond, pit, &cond_val));
        if (cond->mode == PPARSE_CONDITION_MODE_ENABLE)
        {
            if (cond_val == 0)
            {
                *valid = 0;
                SHR_EXIT();
            }
        }
        else
        {
            
            if (cond_val)
            {
                *valid = 0;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pparse_object_get(
    int unit,
    parsing_db_t * pdb,
    pparse_parsing_object_t * obj)
{

    SHR_FUNC_INIT_VARS(unit);

    if ((obj->pdb_entry = utilex_rhlist_entry_get_by_name(pdb->obj_lst, RHNAME(obj))) == NULL)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_object_next_header_find(
    int unit,
    pparse_db_entry_t * pdb_entry,
    rhlist_t * pit,
    next_header_option_t ** next_header)
{
    uint32 field_val = 0;
    uint8 found = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(pparse_field_val_get(unit, pdb_entry->next_header_field, pit, &field_val, &found));
    if (found)
    {
        
        RHITERATOR(*next_header, pdb_entry->next_header_options->option_lst)
        {
            if ((*next_header)->field_value == field_val)
            {
                break;
            }
        }
        
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_list_next_extensions_add(
    int unit,
    int core,
    pparse_parsing_object_t * next,
    rhlist_t * pit,
    rhlist_t * parse_list)
{
    int ext_i;

    SHR_FUNC_INIT_VARS(unit);

    
    if (next->pdb_entry->next_header_options)
    {
        next_header_option_t *next_header = NULL;
        SHR_IF_ERR_EXIT(pparse_object_next_header_find(unit, next->pdb_entry, pit, &next_header));
        if (next_header)
        {
            SHR_IF_ERR_EXIT(pparse_list_push(unit, RHNAME(next_header), parse_list));
        }
    }
    
    for (ext_i = next->pdb_entry->extensions_nof - 1; ext_i >= 0; --ext_i)
    {
        pparse_object_extension_t *ext = &next->pdb_entry->extensions[ext_i];
        uint8 valid = 0;
        SHR_IF_ERR_EXIT(pparse_object_extension_check_conditions(unit, core, ext, pit, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(pparse_list_push(unit, ext->object_name, parse_list));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT

static shr_error_e
pparse_add_non_parsed_entry(
    int unit,
    int blob_size_bits,
    int data_size_bits,
    uint32 *data,
    rhlist_t * pit,
    char *entry_name,
    char *expansion_name)
{

    
    signal_output_t * blob_sig = NULL;
    rhhandle_t rhhandle = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(pit, entry_name, RHID_TO_BE_GENERATED, &rhhandle));
    blob_sig = rhhandle;
    SHR_BITCOPY_RANGE(blob_sig->value, 0, data, data_size_bits - blob_size_bits, blob_size_bits);
    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_UINT32, blob_sig->value, blob_sig->print_value, blob_size_bits,
                             PRINT_BIG_ENDIAN);
    blob_sig->start_bit = 0;
    blob_sig->end_bit = blob_size_bits - 1;
    blob_sig->size = blob_size_bits;
    sal_snprintf(blob_sig->expansion, RHNAME_MAX_SIZE, expansion_name);

exit:
    SHR_FUNC_EXIT;
}
#endif

#ifdef BCM_DNX_SUPPORT

static shr_error_e
pparse_data_parse_by_struct_name_and_size_custom(
    int unit,
    int data_size_bits,
    const uint32 * data,
    const char * struct_n,
    const int struct_size_bits,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    signal_output_t * orig_tail = utilex_rhlist_entry_get_last(pit);
    signal_output_t * added_sig = NULL;
    uint32 buffer[DSIG_MAX_SIZE_UINT32];
    char sig_name[RHNAME_MAX_SIZE]; 

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy_s(sig_name, struct_n, RHNAME_MAX_SIZE);
    SHR_BITCOPY_RANGE(buffer, 0, data, data_size_bits - struct_size_bits, struct_size_bits);
    SHR_IF_ERR_EXIT_WITH_LOG(sand_signal_parse_get
                             (unit, sig_name, EMPTY, EMPTY, EMPTY,
                              PRINT_BIG_ENDIAN, buffer, struct_size_bits, pit),
                             "Couldn't find structure '%s'.%s%s", sig_name, EMPTY, EMPTY);

    if (((added_sig = utilex_rhlist_entry_get_last(pit)) == NULL) || (added_sig == orig_tail))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected a new signal in the parsing tree but non was found.");
    }
    *parsed_size += added_sig->size;

exit:
    SHR_FUNC_EXIT;
}
#endif

#define PPARSE_TSH_SIZE             32
#define PPARSE_TSH_SIG              "TSH"


shr_error_e
pparse_list_process(
    int unit,
    int core,
    parsing_db_t * pdb,
    pparse_packet_info_t * packet_info,
    rhlist_t * parse_list,
    rhlist_t * pit)
{

    pparse_parsing_object_t *next = NULL;
    shr_error_e rv;
    signal_output_t *layer_offset_sig = NULL;
    signal_output_t *ptc_sig = NULL;

#ifdef BCM_DNX_SUPPORT
    
    uint8 met_real_header = FALSE;
    
    uint8 port_skip_support = FALSE;

    
    int ptch_skip_after_size_bytes = 0;
    int ptch_skip_before_size_bytes = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

    while (NULL != (next = pparse_list_get_next(unit, parse_list)))
    {
        uint32 buffer[PPARSE_MAX_DATA_WORDS];
        
        rv = pparse_object_get(unit, pdb, next);
        if (rv == _SHR_E_NOT_FOUND)
        {
            
            SHR_EXIT();
        }
        else
        {
            
            SHR_IF_ERR_EXIT(rv);
        }

#ifdef BCM_DNX_SUPPORT
        
        if (next->pdb_entry->port_skip)
        {
            port_skip_support = TRUE;
        }
#endif

        if (next->pdb_entry->size > 0)
        {
            uint32 parsed_size = 0;
            char *expansion_name;

#ifdef BCM_DNX_SUPPORT
            

            if (port_skip_support && met_real_header == FALSE)
            {

                dnx_algo_skip_first_header_info_t port_first_header_size_data;
                dbal_enum_value_field_port_termination_ptc_profile_e ptc_profile = 0;
                int ref_count;
                bcm_port_t tm_port = -1;
                bcm_port_t logical_port;

                int valid;

                
                int is_header_vis_global = dnx_data_headers.internal.feature_get(unit, dnx_data_headers_internal_header_visibilty_mode_global);

                

                
                rv = sand_signal_output_find(unit, core, SIGNALS_MATCH_NO_FETCH_ALL, "IRPP", is_header_vis_global ? "NIF" : "VTT5",
                                             is_header_vis_global ? "PRT" : NULL , "ptc", &ptc_sig);

                
                if ((rv == _SHR_E_NONE) && sal_strcasecmp(ptc_sig->print_value, "N/A"))
                {
                    tm_port = ptc_sig->value[0];

                    
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));
                    if (valid)
                    {

                        
                        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
                        
                        SHR_IF_ERR_EXIT(dnx_port_prt_ptc_profile_get(unit, logical_port, &ptc_profile));

                        
                        if (ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1 ||
                            ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD2 ||
                            ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD3 ||
                            ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD4 ||
                            ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD5 ||
                            ptc_profile == DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD6)
                        {

                            
                            SHR_IF_ERR_EXIT(dnx_algo_port_db.skip_first_user_defined_header_size_profile_data.profile_data_get(
                                                                                                             unit,
                                                                                                             ptc_profile,
                                                                                                             &ref_count,
                                                                                                             &port_first_header_size_data));

                            
                            if (port_first_header_size_data.first_header_size_to_skip > 0)
                            {

                                
                                if ((port_first_header_size_data.is_before) && (port_first_header_size_data.is_after))
                                {
                                    SHR_ERR_EXIT(_SHR_E_NONE,
                                                  "Parsing failed because on port: %d skip size: %d is marked both as After and Before - must be one of them"
                                                  "Stopping.\r\n", tm_port, port_first_header_size_data.first_header_size_to_skip);

                                } 
                                else if (port_first_header_size_data.is_before)
                                {
                                    ptch_skip_before_size_bytes = port_first_header_size_data.first_header_size_to_skip;

                                } 
                                else if (port_first_header_size_data.is_after)
                                {
                                    ptch_skip_after_size_bytes = port_first_header_size_data.first_header_size_to_skip;
                                } 

                            } 

                        } 

                    } 

                } 

                met_real_header = TRUE;

                
                 if (ptch_skip_before_size_bytes > 0)
                 {
                     

                     SHR_IF_ERR_EXIT(pparse_add_non_parsed_entry(unit, ptch_skip_before_size_bytes * 8,
                                                                 packet_info->data_size,
                                                                 packet_info->data, pit,
                                                                 "port skip before", "Skipped"));

                     packet_info->data_size -= ptch_skip_before_size_bytes * 8;
                 }

            } 
#endif

            
            SHR_IF_ERR_EXIT(pparse_buffer_prepare(unit, next, packet_info, buffer));
            
            rv = next->pdb_entry->parsing_cb(unit, next->pdb_entry, buffer, pit, &parsed_size);
            if (rv != _SHR_E_NONE)
            {
                
                SHR_ERR_EXIT(_SHR_E_NONE,
                             "Parsing failed when trying to process object '%s'. Stopping.\r\n", RHNAME(next));
            }

#ifdef BCM_DNX_SUPPORT


            
            if (sal_strcasestr(next->rhentry.name, "PTCH_1") != NULL)
            {
                
                int ptch1_tsh_present = 0;
                signal_output_t * ptch1_sig = utilex_rhlist_entry_get_last_by_name(pit, "PTCH_1");
                SHR_NULL_CHECK(ptch1_sig, _SHR_E_NOT_FOUND, "ptch1_sig");

                
                {
                    signal_output_t * ptch1_tsh_present_sig = utilex_rhlist_entry_get_by_name(ptch1_sig->field_list,
                                                                                               "TS_Present");
                    SHR_NULL_CHECK(ptch1_tsh_present_sig, _SHR_E_NOT_FOUND, "ptch1_tsh_present_sig");
                    ptch1_tsh_present = ptch1_tsh_present_sig->value[0];
                }

                
                if (ptch1_tsh_present)
                {

                    SHR_IF_ERR_EXIT(pparse_data_parse_by_struct_name_and_size_custom(
                                               unit, packet_info->data_size - parsed_size, packet_info->data,
                                               PPARSE_TSH_SIG, PPARSE_TSH_SIZE, pit, &parsed_size));

                }
            }

            
            if (sal_strcasestr(next->rhentry.name, "PTCH") != NULL)
            {

                
                if (ptch_skip_after_size_bytes > 0)
                {
                    SHR_IF_ERR_EXIT(pparse_add_non_parsed_entry(unit, ptch_skip_after_size_bytes * 8,
                                                                packet_info->data_size - parsed_size,
                                                                packet_info->data, pit,
                                                                "port skip after", "Skipped"));

                    parsed_size += ptch_skip_after_size_bytes * 8;
                }

            }

            
            if (!sal_strcasecmp(next->rhentry.name, "RCH"))
            {
                
                int data_size_bits = packet_info->data_size - parsed_size;
                int rch_extension_size_bytes = 0;
                signal_output_t * rch_sig = utilex_rhlist_entry_get_last_by_name(pit, "RCH");
                SHR_NULL_CHECK(rch_sig, _SHR_E_NOT_FOUND, "rch_sig");

                
                {
                    signal_output_t * rch_extension_size_sig = utilex_rhlist_entry_get_by_name(rch_sig->field_list,
                                                                                           "Extension_size_in_Bytes");
                    SHR_NULL_CHECK(rch_extension_size_sig, _SHR_E_NOT_FOUND, "rch_extension_size_sig");
                    rch_extension_size_bytes = rch_extension_size_sig->value[0];
                }

                
                if (rch_extension_size_bytes > 0)
                {

                    SHR_IF_ERR_EXIT(pparse_add_non_parsed_entry(unit, rch_extension_size_bytes * 8,
                                                                data_size_bits,
                                                                packet_info->data, pit,
                                                                "RCH Extension", "Skipped"));

                    parsed_size += rch_extension_size_bytes * 8;
                }

            }
#endif

            
            if (packet_info->data_size > parsed_size)
            {
                packet_info->data_size -= parsed_size;
            }
            else
            {
                
                packet_info->data_size = 0;
                break;
            }
            expansion_name = ((signal_output_t *) (pit->tail))->expansion;
            if ((!ISEMPTY(expansion_name))
                && (sal_strncmp(expansion_name, next->pdb_entry->struct_name, RHNAME_MAX_SIZE)))
            {
                
                pparse_db_entry_t *entry = NULL;
                RHITERATOR(entry, pdb->obj_lst)
                {
                    if (!sal_strncmp(entry->struct_name, expansion_name, RHNAME_MAX_SIZE))
                    {
                        
                        next->pdb_entry = entry;
                        break;
                    }
                }
            }
        } 
        
        SHR_IF_ERR_EXIT(pparse_list_next_extensions_add(unit, core, next, pit, parse_list));
        
        SHR_IF_ERR_EXIT(pparse_list_pop(unit, parse_list));

    }
exit:
    sand_signal_output_free(layer_offset_sig);
    sand_signal_output_free(ptc_sig);
    SHR_FUNC_EXIT;
}

shr_error_e pparse_parsed_list_duplicates_enumerate(
    int unit,
    rhlist_t * list,
    signal_output_t * start_item)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!start_item)
    {
        start_item = utilex_rhlist_entry_get_first(list);
    }
    for (; start_item; start_item = utilex_rhlist_entry_get_next(start_item))
    {
        int next_id = 0;
        rhhandle_t iter_item;
        
        for (iter_item = utilex_rhlist_entry_get_next(start_item); iter_item;
                iter_item = utilex_rhlist_entry_get_next(iter_item))
        {
            if (!sal_strncasecmp(RHNAME(start_item), RHNAME(iter_item), RHNAME_MAX_SIZE))
            {
                sal_snprintf(RHNAME(iter_item), RHNAME_MAX_SIZE, "%s_%d", RHNAME(start_item), ++next_id);
            }
        }
        if (next_id > 0)
        {
            
            sal_strncat_s(RHNAME(start_item), "_0", sizeof(RHNAME(start_item)));
        }
        
        SHR_IF_ERR_EXIT(pparse_parsed_list_duplicates_enumerate(unit, start_item->field_list, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
pparse_parsed_info_tree_create(
    int unit,
    rhlist_t ** pit)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((*pit = utilex_rhlist_create("PParse Information", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE,
                          "Could not allocate parsed information buffer to parse the required packet. %s%s%s\n", EMPTY,
                          EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

void
pparse_parsed_info_tree_free(
    int unit,
    rhlist_t * pit)
{
    if (pit != NULL)
    {
        utilex_rhlist_free_all(pit);
    }
}

shr_error_e
pparse_list_create(
    int unit,
    rhlist_t ** plist)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((*plist = utilex_rhlist_create("plist", sizeof(pparse_parsing_object_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Could not allocate parsing stack to parse the required packet. %s%s%s\n",
                          EMPTY, EMPTY, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

void
pparse_list_free(
    int unit,
    rhlist_t * plist)
{
    utilex_rhlist_free_all(plist);
}

shr_error_e
pparse_list_push(
    int unit,
    char *object_name,
    rhlist_t * plist)
{
    rhhandle_t entry_handle;
    rhhandle_t top_handle;

    SHR_FUNC_INIT_VARS(unit);

    top_handle = utilex_rhlist_entry_get_first(plist);
    
    if (top_handle != NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_alloc(plist, object_name, RHID_TO_BE_GENERATED, &entry_handle));
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_insert_before(plist, entry_handle, ((rhentry_t *) top_handle)->next));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_top(plist, object_name, RHID_TO_BE_GENERATED, &entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pparse_list_add(
    int unit,
    char *object_name,
    rhlist_t * plist,
    pparse_parsing_object_t ** new_item_p)
{
    rhhandle_t entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(plist, object_name, RHID_TO_BE_GENERATED, &entry_handle));

    *new_item_p = entry_handle;

exit:
    SHR_FUNC_EXIT;
}

pparse_parsing_object_t *
pparse_list_get_next(
    int unit,
    rhlist_t * plist)
{
    return utilex_rhlist_entry_get_first(plist);
}

shr_error_e
pparse_list_pop(
    int unit,
    rhlist_t * plist)
{
    rhhandle_t entry;

    SHR_FUNC_INIT_VARS(unit);

    if ((entry = utilex_rhlist_entry_get_first(plist)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Something went wrong - parsing stack emptied too soon. %s%s%s\n", EMPTY,
                          EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_del_free(plist, entry));

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
pparse_compose_field_from_condition_add(
    int unit,
    char *path,
    uint32 value,
    signal_output_t * header)
{

    SHR_FUNC_INIT_VARS(unit);

    
    if (header->field_list == NULL)
    {
        if (NULL == (header->field_list = utilex_rhlist_create("header_fields", sizeof(signal_output_t), 0)))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Could not allocate field list for header '%s' %s%s", RHNAME(header),
                              EMPTY, EMPTY);
        }
    }
    
    if (NULL == utilex_rhlist_entry_get_by_name(header->field_list, path))
    {
        signal_output_t *new_field;
        rhhandle_t rhhandle;
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header->field_list, path, RHID_TO_BE_GENERATED, &rhhandle));
        new_field = rhhandle;
        new_field->value[0] = value;
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_compose_post_processing_cb_add_for_current_packet(
    int unit,
    packet_compose_info_t *compose_info,
    signal_output_t *header,
    pparse_post_processor_cb_t cb)
{
    rhhandle_t rhhandle = NULL;
    post_processing_entry_t *post_proc_entry = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
        (compose_info->post_processors, RHNAME(header), RHID_TO_BE_GENERATED, &rhhandle));
    post_proc_entry = rhhandle;
    post_proc_entry->post_processing_cb = cb;
    post_proc_entry->header = header;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_compose_fields_from_conditions_add(
    int unit,
    parsing_db_t * pdb,
    packet_compose_info_t *compose_info)
{
    signal_output_t *pkt_obj;

    SHR_FUNC_INIT_VARS(unit);

    RHITERATOR(pkt_obj, compose_info->packet)
    {
        signal_output_t *next_obj = NULL;
        pparse_parsing_object_t pdb_obj;
        sal_strncpy_s(RHNAME(&pdb_obj), RHNAME(pkt_obj), RHNAME_MAX_SIZE);
        
        SHR_IF_ERR_EXIT(pparse_object_get(unit, pdb, &pdb_obj));
        
        sal_strncpy_s(RHNAME(pkt_obj), pdb_obj.pdb_entry->struct_name, RHNAME_MAX_SIZE);
        if ((sand_signal_struct_get(sand_signal_device_get(unit), RHNAME(pkt_obj), NULL, NULL, NULL)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Object '%s' could not be resolved to a signal/header structure.\n"
                         "\tObject description: %s\n", RHNAME(&pdb_obj), pdb_obj.pdb_entry->desc_str);
        }
        
        if (pdb_obj.pdb_entry->post_processing_cb)
        {
            SHR_IF_ERR_EXIT(pparse_compose_post_processing_cb_add_for_current_packet
                            (unit, compose_info, pkt_obj, pdb_obj.pdb_entry->post_processing_cb));
        }
        
        if (NULL != (next_obj = utilex_rhlist_entry_get_next(pkt_obj)))
        {
            
            int ext_i;
            for (ext_i = 0; ext_i < pdb_obj.pdb_entry->extensions_nof; ext_i++)
            {
                pparse_object_extension_t *ext = &(pdb_obj.pdb_entry->extensions[ext_i]);
                if (0 == sal_strncasecmp(ext->object_name, RHNAME(next_obj), RHNAME_MAX_SIZE))
                {
                    
                    int cond_i;
                    for (cond_i = 0; cond_i < ext->conditions_nof; cond_i++)
                    {
                        pparse_condition_t *cond = &(ext->conditions[cond_i]);
                        if (cond->type == PPARSE_CONDITION_TYPE_FIELD)
                        {
                            
                            SHR_IF_ERR_EXIT(pparse_compose_field_from_condition_add(unit, cond->content.field.path,
                                                                                    cond->content.field.enable_value,
                                                                                    pkt_obj));
                        }
                    }
                }
            }
            
            if (pdb_obj.pdb_entry->next_header_options)
            {
                next_header_option_t *option = NULL;
                RHITERATOR(option, pdb_obj.pdb_entry->next_header_options->option_lst)
                {
                    if (0 == sal_strncasecmp(RHNAME(option), RHNAME(next_obj), RHNAME_MAX_SIZE))
                    {
                        
                        SHR_IF_ERR_EXIT(pparse_compose_field_from_condition_add(unit,
                                                                                pdb_obj.pdb_entry->next_header_field,
                                                                                option->field_value, pkt_obj));
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_compose_post_processing(
    int unit,
    packet_compose_info_t *compose_info)
{
    post_processing_entry_t * post_proc_entry;
    SHR_FUNC_INIT_VARS(unit);

    RHITERATOR(post_proc_entry, compose_info->post_processors)
    {
        SHR_IF_ERR_EXIT(post_proc_entry->post_processing_cb(unit, compose_info, post_proc_entry->header));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_compose_info_init(
    int unit,
    packet_compose_info_t *compose_info)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(compose_info, 0, sizeof(packet_compose_info_t));
    if (NULL == (compose_info->post_processors = utilex_rhlist_create("pdb_post_proc_lst", sizeof(post_processing_entry_t), 0)))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Could not allocate post processing list");
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_packet_compose(
    int unit,
    parsing_db_t * pdb,
    rhlist_t * packet_objects,
    uint32 packet_size_bytes,
    uint32 max_data_size_bits,
    uint32 *data,
    uint32 *data_size_bits)
{
    signal_output_t *pkt_obj;
    packet_compose_info_t compose_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(pparse_compose_info_init(unit, &compose_info));
    compose_info.packet = packet_objects;
    compose_info.total_packet_size = packet_size_bytes;
    
    SHR_IF_ERR_EXIT(pparse_compose_fields_from_conditions_add(unit, pdb, &compose_info));
    
    RHITERATOR(pkt_obj, compose_info.packet)
    {
        SHR_IF_ERR_EXIT(sand_signal_compose(unit, pkt_obj, NULL, NULL, NULL, PRINT_BIG_ENDIAN));
    }
    
    SHR_IF_ERR_EXIT(pparse_compose_post_processing(unit, &compose_info));

    
    for (pkt_obj = utilex_rhlist_entry_get_last(packet_objects);
         pkt_obj != NULL; pkt_obj = utilex_rhlist_entry_get_prev(pkt_obj))
    {
        if (pkt_obj->size + (*data_size_bits) > max_data_size_bits)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Packet size has grown too much while adding header '%s' %s%s",
                              RHNAME(pkt_obj), EMPTY, EMPTY);
        }
        SHR_BITCOPY_RANGE(data, *data_size_bits, pkt_obj->value, 0, pkt_obj->size);
        *data_size_bits += pkt_obj->size;
    }

exit:
    SHR_FUNC_EXIT;
}









#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/dbal.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#define PPARSE_PTCH_2_SIZE_IN_BYTES     2

shr_error_e
packet_ptch2(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled)
{

    signal_output_t *ptc_sig = NULL;

    bcm_port_t tm_port;
    bcm_port_t logical_port;

    int switch_header_type;
    int valid;

    
    int is_header_vis_global = dnx_data_headers.internal.feature_get(unit, dnx_data_headers_internal_header_visibilty_mode_global);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enabled, _SHR_E_PARAM, "enabled");

    *enabled = 0;

    
    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, SIGNALS_MATCH_NO_FETCH_ALL, "IRPP", is_header_vis_global ? "NIF" : "VTT5",
                                            is_header_vis_global ? "PRT" : NULL , "ptc", &ptc_sig));

    tm_port = ptc_sig->value[0];

    
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));
    if (!valid)
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));

    
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, logical_port, &switch_header_type));

    if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
         || (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP))
    {
        *enabled = 1;
    }

exit:
    sand_signal_output_free(ptc_sig);
    SHR_FUNC_EXIT;

}

shr_error_e
packet_ptch1(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled)
{


    signal_output_t *ptc_sig = NULL;

    bcm_port_t tm_port;
    bcm_port_t logical_port;

    int switch_header_type;
    int valid;

    
    int is_header_vis_global = dnx_data_headers.internal.feature_get(unit, dnx_data_headers_internal_header_visibilty_mode_global);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enabled, _SHR_E_PARAM, "enabled");

    *enabled = 0;

    
    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, SIGNALS_MATCH_NO_FETCH_ALL, "IRPP", is_header_vis_global ? "NIF" : "VTT5",
                                            is_header_vis_global ? "PRT" : NULL , "ptc", &ptc_sig));

    tm_port = ptc_sig->value[0];

    
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));
    if (!valid)
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));

    
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, logical_port, &switch_header_type));

    if (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED)
    {
        *enabled = 1;
    }

exit:
    sand_signal_output_free(ptc_sig);
    SHR_FUNC_EXIT;

}


shr_error_e
packet_itmh(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled)
{


    signal_output_t *ptc_sig = NULL;

    bcm_port_t tm_port;
    bcm_port_t logical_port;

    int switch_header_type;
    int valid;

    
    int is_header_vis_global = dnx_data_headers.internal.feature_get(unit, dnx_data_headers_internal_header_visibilty_mode_global);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enabled, _SHR_E_PARAM, "enabled");

    *enabled = 0;

    
    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, SIGNALS_MATCH_NO_FETCH_ALL, "IRPP", is_header_vis_global ? "NIF" : "VTT5",
                                            is_header_vis_global ? "PRT" : NULL , "ptc", &ptc_sig));

    tm_port = ptc_sig->value[0];

    
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));
    if (!valid)
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));

    
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, logical_port, &switch_header_type));

    if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_TM) ||
        (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP) ||
        (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP) ||
        (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP))
    {
        *enabled = 1;
    }

exit:
    sand_signal_output_free(ptc_sig);
    SHR_FUNC_EXIT;

}


shr_error_e
packet_rch(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled)
{
    signal_output_t *ptc_sig = NULL;

    bcm_port_t tm_port;
    bcm_port_t logical_port;

    int switch_header_type;
    int valid;

    
    int is_header_vis_global = dnx_data_headers.internal.feature_get(unit, dnx_data_headers_internal_header_visibilty_mode_global);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enabled, _SHR_E_PARAM, "enabled");

    *enabled = 0;

    
    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, SIGNALS_MATCH_NO_FETCH_ALL, "IRPP", is_header_vis_global ? "NIF" : "VTT5",
                                            is_header_vis_global ? "PRT" : NULL , "ptc", &ptc_sig));

    tm_port = ptc_sig->value[0];

    
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));
    if (!valid)
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));

    
    SHR_IF_ERR_EXIT(dnx_switch_header_type_in_get(unit, logical_port, &switch_header_type));

    if ((switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_RCH_0) ||
        (switch_header_type == BCM_SWITCH_PORT_HEADER_TYPE_RCH_1))
    {
        *enabled = 1;
    }

exit:
    sand_signal_output_free(ptc_sig);
    SHR_FUNC_EXIT;
}


shr_error_e
packet_ftmh_opt(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled)
{
    uint32 entry_handle_id;
    uint32 egress_tdm_mode, optimized_ftmh_tdm_mode;
    signal_output_t *is_tdm_sig = NULL;
    int rv_sig = _SHR_E_NONE;
    int ftmh_opt_enabled = dnx_data_headers.optimized_ftmh.feature_get(unit, dnx_data_headers_optimized_ftmh_ftmh_opt_parse);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *enabled = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_EGRESS_MODE, INST_SINGLE, &egress_tdm_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_FTMH_OPTIMIZED, INST_SINGLE, &optimized_ftmh_tdm_mode);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    rv_sig = sand_signal_output_find(unit, core, 0, "ERPP", "ERparser", NULL, "is_TDM", &is_tdm_sig);
    
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv_sig, _SHR_E_NOT_FOUND);
    if(rv_sig == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, 0, "ETPP", "Applet", NULL, "is_TDM", &is_tdm_sig));
    }

    if(is_tdm_sig->value[0] && egress_tdm_mode && optimized_ftmh_tdm_mode && ftmh_opt_enabled)
    {
        *enabled = 1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#endif

shr_error_e
pparse_sig_parse_wrapper_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    shr_error_e rv = 0;
    char* struct_skip_no_err = "Invalid";
    signal_output_t * orig_tail = NULL;
    signal_output_t * added_sig = NULL;

    SHR_FUNC_INIT_VARS(unit);
    orig_tail = utilex_rhlist_entry_get_last(pit);

    
    rv = sand_signal_parse_get(unit, pdb_entry->struct_name, pdb_entry->block_name, pdb_entry->stage_from_name, NULL,
                               PRINT_BIG_ENDIAN, data, pdb_entry->size, pit);

    if(rv != _SHR_E_NONE)
    {
        if(rv == _SHR_E_NOT_FOUND)
        {
            
            
            if(sal_strcmp(pdb_entry->struct_name, struct_skip_no_err) == 0)
            {
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(rv , "Couldn't find structure '%s'.%s%s", pdb_entry->struct_name, EMPTY, EMPTY);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "Error when trying to find structure '%s'.%s%s", pdb_entry->struct_name, EMPTY, EMPTY);
        }
    }

    
    if (((added_sig = utilex_rhlist_entry_get_last(pit)) == NULL) || (orig_tail == added_sig))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Parsing did not add a header to the parsed info tree.");
    }
    *parsed_size = added_sig->size;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
pparse_data_parse_by_struct_name_and_size(
    int unit,
    pparse_db_entry_t * pdb_entry,
    const uint32 * data,
    const char * struct_n,
    const int struct_size_bits,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    signal_output_t * orig_tail = utilex_rhlist_entry_get_last(pit);
    signal_output_t * added_sig = NULL;
    uint32 buffer[DSIG_MAX_SIZE_UINT32];
    char sig_name[RHNAME_MAX_SIZE]; 
    int data_size_bits = pdb_entry->size - (*parsed_size);

    SHR_FUNC_INIT_VARS(unit);


    sal_strncpy_s(sig_name, struct_n, RHNAME_MAX_SIZE);
    SHR_BITCOPY_RANGE(buffer, 0, data, data_size_bits - struct_size_bits, struct_size_bits);
    SHR_IF_ERR_EXIT_WITH_LOG(sand_signal_parse_get
                             (unit, sig_name, pdb_entry->block_name, pdb_entry->stage_from_name, NULL,
                              PRINT_BIG_ENDIAN, buffer, struct_size_bits, pit),
                             "Couldn't find structure '%s'.%s%s", sig_name, EMPTY, EMPTY);
    if (((added_sig = utilex_rhlist_entry_get_last(pit)) == NULL) || (added_sig == orig_tail))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected a new signal in the parsing tree but non was found.");
    }
    *parsed_size += added_sig->size;

exit:
    SHR_FUNC_EXIT;
}


#define PPARSE_ETH_PARSER_GLOBAL_TPID_NOF       7
#define PPARSE_ETH_PARSER_GLOBAL_TPID_INVALID   0x0000

#define PPARSE_ETH_PARSER_MAC_SIZE 48
#define PPARSE_ETH_PARSER_VLAN_SIZE 32
#define PPARSE_ETH_PARSER_TPID_SIZE 16
#define PPARSE_ETH_PARSER_TYPE_SIZE 16

#define PPARSE_ETH_PARSER_ETH2_SIZE \
    ((2 * PPARSE_ETH_PARSER_MAC_SIZE) + (2 * PPARSE_ETH_PARSER_VLAN_SIZE) + PPARSE_ETH_PARSER_TYPE_SIZE)
#define PPARSE_ETH_PARSER_ETH1_SIZE \
    ((2 * PPARSE_ETH_PARSER_MAC_SIZE) + (1 * PPARSE_ETH_PARSER_VLAN_SIZE) + PPARSE_ETH_PARSER_TYPE_SIZE)
#define PPARSE_ETH_PARSER_ETH0_SIZE \
    ((2 * PPARSE_ETH_PARSER_MAC_SIZE) + (0 * PPARSE_ETH_PARSER_VLAN_SIZE) + PPARSE_ETH_PARSER_TYPE_SIZE)

#ifdef BCM_DNX_SUPPORT

static shr_error_e
pparse_dnx_eth_parser_tpid_val_check(
    int unit,
    uint32 tpid_val,
    int * is_tpid)
{
    uint32 entry_handle_id;
    uint32 tpid_entry;
    uint32 tpid_i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    *is_tpid = 0;

    if (tpid_val == PPARSE_ETH_PARSER_GLOBAL_TPID_INVALID)
    {
        
        *is_tpid = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_TPID, &entry_handle_id));

    for (tpid_i = 0; tpid_i < PPARSE_ETH_PARSER_GLOBAL_TPID_NOF; ++tpid_i)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, tpid_i);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TPID_VALUE, INST_SINGLE, &tpid_entry);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (tpid_entry == tpid_val)
        {
            
            *is_tpid = 1;
            SHR_EXIT();
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif


static shr_error_e
pparse_dpp_eth_parser_tpid_val_check(
    int unit,
    uint32 tpid_val,
    int * is_tpid)
{
    
    *is_tpid = ((tpid_val == 0x8100) ||
            (tpid_val == 0x9100) ||
            (tpid_val == 0x88a8));
    return _SHR_E_NONE;
}


static shr_error_e
pparse_eth_parser_tpid_val_check(
    int unit,
    uint32 tpid_val,
    int * is_tpid)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        uint8 is_default_image = 0;
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_default_image));
        if (is_default_image)
        {
            SHR_IF_ERR_EXIT(pparse_dnx_eth_parser_tpid_val_check(unit, tpid_val, is_tpid));
            SHR_EXIT();
        }
    }
#endif
    SHR_IF_ERR_EXIT(pparse_dpp_eth_parser_tpid_val_check(unit, tpid_val, is_tpid));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_eth_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    int nof_tpids = 0;
    int structure_size;
    char * structure;
    int data_size_bits = pdb_entry->size - (*parsed_size);

    SHR_FUNC_INIT_VARS(unit);

    
    if (data_size_bits < PPARSE_ETH_PARSER_ETH2_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need at least %db to parse an ETH header. %db given.",
                     PPARSE_ETH_PARSER_ETH2_SIZE, data_size_bits);
    }

    
    {
        
        uint32 tpid_val = 0;
        int tpid_offset = data_size_bits - (2 * PPARSE_ETH_PARSER_MAC_SIZE) - (0 * PPARSE_ETH_PARSER_VLAN_SIZE) - PPARSE_ETH_PARSER_TPID_SIZE;
        int is_tpid = 0;
        SHR_BITCOPY_RANGE(&tpid_val, 0, data, tpid_offset, PPARSE_ETH_PARSER_TPID_SIZE);
        SHR_IF_ERR_EXIT(pparse_eth_parser_tpid_val_check(unit, tpid_val, &is_tpid));
        if (is_tpid)
        {
            nof_tpids++;
        }
    }
    {
        
        uint32 tpid_val = 0;
        int tpid_offset = data_size_bits - (2 * PPARSE_ETH_PARSER_MAC_SIZE) - (1 * PPARSE_ETH_PARSER_VLAN_SIZE) - PPARSE_ETH_PARSER_TPID_SIZE;
        int is_tpid = 0;
        SHR_BITCOPY_RANGE(&tpid_val, 0, data, tpid_offset, PPARSE_ETH_PARSER_TPID_SIZE);
        SHR_IF_ERR_EXIT(pparse_eth_parser_tpid_val_check(unit, tpid_val, &is_tpid));
        if (is_tpid)
        {
            nof_tpids++;
        }
    }
    
    switch(nof_tpids)
    {
    case 2:
        structure = "ETH2";
        structure_size = PPARSE_ETH_PARSER_ETH2_SIZE;
        break;
    case 1:
        structure = "ETH1";
        structure_size = PPARSE_ETH_PARSER_ETH1_SIZE;
        break;
    default:
        structure = "ETH0";
        structure_size = PPARSE_ETH_PARSER_ETH0_SIZE;
        break;
    }
    
    SHR_IF_ERR_EXIT(pparse_data_parse_by_struct_name_and_size(unit, pdb_entry, data, structure, structure_size,
                                                              pit, parsed_size));

exit:
    SHR_FUNC_EXIT;
}


#define PPARSE_IPV4_PARSER_IPV4_BASE_STRUCT_NAME        "IPv4"
#define PPARSE_IPV4_PARSER_IPV4_BASE_SIZE_IN_BITS       (20*8)
#define PPARSE_IPV4_PARSER_IHL_FIELD_NAME               "IHL"


shr_error_e
pparse_ipv4_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    int data_size_bits = pdb_entry->size - (*parsed_size);
    uint32 ihl_val;
    signal_output_t *ipv4_header_sig;

    SHR_FUNC_INIT_VARS(unit);

    
    if (data_size_bits < PPARSE_IPV4_PARSER_IPV4_BASE_SIZE_IN_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need at least %db to parse an IPv4 header. %db given.",
                     PPARSE_IPV4_PARSER_IPV4_BASE_SIZE_IN_BITS, data_size_bits);
    }

    
    SHR_IF_ERR_EXIT(pparse_data_parse_by_struct_name_and_size
                    (unit, pdb_entry, data, PPARSE_IPV4_PARSER_IPV4_BASE_STRUCT_NAME,
                     PPARSE_IPV4_PARSER_IPV4_BASE_SIZE_IN_BITS, pit, parsed_size));

    
    {
        signal_output_t *ihl_field_sig;
        ipv4_header_sig = utilex_rhlist_entry_get_last(pit);
        ihl_field_sig = utilex_rhlist_entry_get_by_name(ipv4_header_sig->field_list,PPARSE_IPV4_PARSER_IHL_FIELD_NAME);
        ihl_val = ihl_field_sig->value[0];
    }

    if (ihl_val > 5)
    {
        signal_output_t * options_sig = NULL;
        rhhandle_t rhhandle = NULL;
        int options_size_bits = (ihl_val - 5) * 32;
        int options_lsb_offset = data_size_bits - PPARSE_IPV4_PARSER_IPV4_BASE_SIZE_IN_BITS - options_size_bits;
        if (options_lsb_offset < 0)
        {
            
            options_size_bits += options_lsb_offset;
            options_lsb_offset = 0;
        }
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                        (ipv4_header_sig->field_list, "Options", RHID_TO_BE_GENERATED, &rhhandle));
        options_sig = rhhandle;
        SHR_BITCOPY_RANGE(options_sig->value, 0, data, options_lsb_offset, options_size_bits);
        sand_signal_value_to_str(unit, SAL_FIELD_TYPE_UINT32, options_sig->value, options_sig->print_value, options_size_bits,
                                 PRINT_BIG_ENDIAN);
        options_sig->start_bit = 0;
        options_sig->end_bit = options_size_bits - 1;
        options_sig->size = options_size_bits;
        *parsed_size += options_size_bits;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
post_processing_verify_field_list_exist(
    int unit,
    signal_output_t *header)
{
    SHR_FUNC_INIT_VARS(unit);
    if (header->field_list == NULL)
    {
        if (NULL == (header->field_list = utilex_rhlist_create("header_fields", sizeof(signal_output_t), 0)))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Could not allocate field list for header '%s' %s%s", RHNAME(header),
                              EMPTY, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#define CHECKSUM_CHUNK_SIZE     16
#define CHECKSUM_CHUNK_MASK     ((1<<CHECKSUM_CHUNK_SIZE)-1)

static uint32
calc_ip_checksum(
    uint32 *data,
    uint32 size_bits)
{
    uint32 res;
    uint32 sum = 0;
    int offset = 0;

    for (offset = 0; offset < size_bits; offset += CHECKSUM_CHUNK_SIZE)
    {
        uint32 next_word = 0;
        int bits_to_copy = CHECKSUM_CHUNK_SIZE;
        int dst_offset = 0;
        if (size_bits - offset < CHECKSUM_CHUNK_SIZE)
        {
            bits_to_copy = size_bits - offset;
            dst_offset = CHECKSUM_CHUNK_SIZE - bits_to_copy;
        }
        SHR_BITCOPY_RANGE(&next_word, dst_offset, data, offset, bits_to_copy);
        sum += next_word;
        if (sum & ~CHECKSUM_CHUNK_MASK)
        {
            sum = (sum & CHECKSUM_CHUNK_MASK) + 1;
        }
    }

    res = (~sum) & CHECKSUM_CHUNK_MASK;

    return res;
}
static shr_error_e
ipv4_checksum_post_processing(
    int unit,
    signal_output_t *header)
{
    char *checksum_path = "IPv4.Checksum";
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(post_processing_verify_field_list_exist(unit, header));
    if (!utilex_rhlist_entry_get_by_name(header->field_list, checksum_path))
    {
        rhhandle_t rhhandle = NULL;
        signal_output_t *checksum_field = NULL;
        uint32 checksum_val = calc_ip_checksum(header->value, header->size);
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header->field_list, checksum_path,
                                                     RHID_TO_BE_GENERATED, &rhhandle));
        checksum_field = rhhandle;
        checksum_field->value[0] = checksum_val;
        SHR_IF_ERR_EXIT(sand_signal_compose(unit, header, NULL, NULL, NULL, PRINT_BIG_ENDIAN));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ipv4_total_length_post_processing(
    int unit,
    packet_compose_info_t *compose_info,
    signal_output_t *header)
{
    char *length_path = "IPv4.Total_Length";
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(post_processing_verify_field_list_exist(unit, header));
    if (!utilex_rhlist_entry_get_by_name(header->field_list, length_path))
    {
        signal_output_t *checksum_field = NULL;
        signal_output_t *pkt_header = NULL;
        rhhandle_t rhhandle = NULL;
        uint32 total_length_val = compose_info->total_packet_size;
        RHITERATOR(pkt_header, compose_info->packet)
        {
            if (pkt_header == header)
            {
                break;
            }
            total_length_val -= (pkt_header->size >> 3);
        }

        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header->field_list, length_path,
                                                     RHID_TO_BE_GENERATED, &rhhandle));
        checksum_field = rhhandle;
        checksum_field->value[0] = total_length_val;
        SHR_IF_ERR_EXIT(sand_signal_compose(unit, header, NULL, NULL, NULL, PRINT_BIG_ENDIAN));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pparse_ipv4_post_processor(
    int unit,
    packet_compose_info_t *compose_info,
    signal_output_t *header)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ipv4_total_length_post_processing(unit, compose_info, header));
    SHR_IF_ERR_EXIT(ipv4_checksum_post_processing(unit, header));

exit:
    SHR_FUNC_EXIT;
}


#define PPARSE_MPLS_SPECUL_HEADER_SIZE      320

#define PPARSE_MPLS_SPECUL_MPLS_SIG_NAME            "MPLS"
#define PPARSE_MPLS_SPECUL_MPLS_LABEL_FIELD_NAME    "LABEL"

#define PPARSE_MPLS_SPECUL_VERSION_SIZE     4

typedef enum
{
    NWK_PROTO_IPV4 = 0,
    NWK_PROTO_IPV6 = 1,
    NWK_PROTO_BIER = 2,
    NWK_PROTO_GACH = 3,
    NWK_PROTO_CW = 4,
    NWK_PROTO_MPLS = 5,
    
    NOF_NWK_PROTOCOLS
} pparse_simple_nwk_protocols_e;

typedef struct
{
    int size;
    char sig_name[RHNAME_MAX_SIZE];
} pparse_nwk_sig_option_t;

static const pparse_nwk_sig_option_t nwk_sig_options[NOF_NWK_PROTOCOLS] = {
     
        [NWK_PROTO_IPV4] = {160,  "IPv4"},
        [NWK_PROTO_IPV6] = {320,  "IPv6"},
        [NWK_PROTO_BIER] = {320,  "BIER"},
        [NWK_PROTO_GACH] = {32,   "GACH"},
        [NWK_PROTO_CW  ] = {32,   "Control_Word"},
        [NWK_PROTO_MPLS] = {32,   "MPLS"},
};

#define PPARSE_MPLS_SPECUL_SPECIAL_LABEL_IPV4_NULL      0
#define PPARSE_MPLS_SPECUL_SPECIAL_LABEL_IPV6_NULL      2
#define PPARSE_MPLS_SPECUL_SPECIAL_LABEL_GAL            13

#define PPARSE_MPLS_SPECUL_IPV4_VERSION 4
#define PPARSE_MPLS_SPECUL_BIER_VERSION 5
#define PPARSE_MPLS_SPECUL_IPV6_VERSION 6
#define PPARSE_MPLS_SPECUL_GACH_VERSION 1
#define PPARSE_MPLS_SPECUL_CW_VERSION   0

#ifdef BCM_DNX_SUPPORT
static shr_error_e
pparse_mpls_speculative_cw_enable_get_dnx(
    int unit,
    int * cw_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PARSERIN1_MPLS_SPECULATIVE, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CW_ENABLE, INST_SINGLE, (uint32*)cw_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));


exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif

static shr_error_e
pparse_mpls_speculative_cw_enable_get(
    int unit,
    int * cw_enable)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(pparse_mpls_speculative_cw_enable_get_dnx(unit, cw_enable));
    }
    else
#endif
    {
        *cw_enable = 0;
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_2nd_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    const pparse_nwk_sig_option_t * non_eth_option = NULL;
    uint32 label_value;
    int cw_enable = 0;
    uint32 version = 0;
    int version_offset;
    int data_size_bits = pdb_entry->size - (*parsed_size);

    SHR_FUNC_INIT_VARS(unit);

    
    if (data_size_bits < PPARSE_MPLS_SPECUL_HEADER_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need at least %db for post-MPLS speculative parsing. %db given.",
                     PPARSE_MPLS_SPECUL_HEADER_SIZE, data_size_bits);
    }

    
    {
        signal_output_t * mpls_bos_sig = NULL;
        signal_output_t * label_field = NULL;
        if (((mpls_bos_sig = utilex_rhlist_entry_get_last(pit)) == NULL) ||
                sal_strcasecmp(RHNAME(mpls_bos_sig), PPARSE_MPLS_SPECUL_MPLS_SIG_NAME))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Expected to have MPLS before speculative parser, but no such header found.");
        }
        label_field = utilex_rhlist_entry_get_by_name(mpls_bos_sig->field_list, PPARSE_MPLS_SPECUL_MPLS_LABEL_FIELD_NAME);
        if (label_field == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "MPLS header has no '%s' field.", PPARSE_MPLS_SPECUL_MPLS_LABEL_FIELD_NAME);
        }
        label_value = label_field->value[0];
    }

    
    switch (label_value)
    {
    case PPARSE_MPLS_SPECUL_SPECIAL_LABEL_IPV4_NULL:
        non_eth_option = &nwk_sig_options[NWK_PROTO_IPV4];
        break;
    case PPARSE_MPLS_SPECUL_SPECIAL_LABEL_IPV6_NULL:
        non_eth_option = &nwk_sig_options[NWK_PROTO_IPV6];
        break;
    case PPARSE_MPLS_SPECUL_SPECIAL_LABEL_GAL:
        non_eth_option = &nwk_sig_options[NWK_PROTO_GACH];
        break;
    default:
        
        version_offset = data_size_bits - PPARSE_MPLS_SPECUL_VERSION_SIZE;
        SHR_IF_ERR_EXIT(pparse_mpls_speculative_cw_enable_get(unit, &cw_enable));
        SHR_BITCOPY_RANGE(&version, 0, data, version_offset, PPARSE_MPLS_SPECUL_VERSION_SIZE);
        switch (version)
        {
        case PPARSE_MPLS_SPECUL_IPV4_VERSION:
            non_eth_option = &nwk_sig_options[NWK_PROTO_IPV4];
            break;
        case PPARSE_MPLS_SPECUL_IPV6_VERSION:
            non_eth_option = &nwk_sig_options[NWK_PROTO_IPV6];
            break;
        case PPARSE_MPLS_SPECUL_BIER_VERSION:
            non_eth_option = &nwk_sig_options[NWK_PROTO_BIER];
            break;
        case PPARSE_MPLS_SPECUL_GACH_VERSION:
            if (cw_enable)
            {
                non_eth_option = &nwk_sig_options[NWK_PROTO_GACH];
            }
            break;
        case PPARSE_MPLS_SPECUL_CW_VERSION:
            if (cw_enable)
            {
                non_eth_option = &nwk_sig_options[NWK_PROTO_CW];
            }
            break;
        default:
            
            break;
        }
    }
    
    if (non_eth_option != NULL)
    {
        SHR_IF_ERR_EXIT(pparse_data_parse_by_struct_name_and_size(unit, pdb_entry, data, non_eth_option->sig_name,
                                                                  non_eth_option->size, pit, parsed_size));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(pparse_eth_parser_cb(unit, pdb_entry, data, pit, parsed_size));
    }

exit:
    SHR_FUNC_EXIT;
}


#define PPARSE_SRV6_BASE_SIZE       64
#define PPARSE_SRV6_BASE_SIG        "SRv6"

#define PPARSE_SRV6_SEGMENT_SIZE    128
#define PPARSE_SRV6_SEGMENT_SIG     "Segment_ID"

#define PPARSE_SRV6_LAST_ENTRY_SIG  "Last_Entry"


static shr_error_e
pparse_srv6_segments_parse(
    int unit,
    int nof_segments,
    uint32 * data,
    int data_size,
    signal_output_t * srv6_sig)
{
    int bits_left = data_size;
    int seg_i;
    rhlist_t * field_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    field_list = srv6_sig->field_list;
    for (seg_i = 0; (seg_i < nof_segments) && (bits_left >= PPARSE_SRV6_SEGMENT_SIZE); seg_i++)
    {
        
        uint32 buffer[4];
        int seg_offset = bits_left - PPARSE_SRV6_SEGMENT_SIZE;
        SHR_BITCOPY_RANGE(buffer, 0, data, seg_offset, PPARSE_SRV6_SEGMENT_SIZE);
        SHR_IF_ERR_EXIT_WITH_LOG(sand_signal_parse_get
                                 (unit, PPARSE_SRV6_SEGMENT_SIG, EMPTY, EMPTY, EMPTY, PRINT_BIG_ENDIAN, buffer,
                                  PPARSE_SRV6_SEGMENT_SIZE, field_list),
                                 "Couldn't find structure '%s'.%s%s", PPARSE_SRV6_SEGMENT_SIG, EMPTY, EMPTY);
        
        {
            signal_output_t * segment_sig = utilex_rhlist_entry_get_last(field_list);
            signal_output_t * addr_sig = NULL;
            
            if (!segment_sig || sal_strcasecmp(RHNAME(segment_sig), PPARSE_SRV6_SEGMENT_SIG))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Expected '%s' to be the last field in the parsed field list, but it isn't",
                             PPARSE_SRV6_SEGMENT_SIG);
            }
            sal_snprintf(segment_sig->expansion, RHNAME_MAX_SIZE, "Segment_%d", seg_i);
            addr_sig = utilex_rhlist_entry_get_first(segment_sig->field_list);
            
            if (addr_sig && !sal_strcasecmp(RHNAME(addr_sig), "Address"))
            {
                sal_strncpy_s(segment_sig->print_value, addr_sig->print_value, DSIG_MAX_SIZE_STR);
            }
            sand_signal_list_free(segment_sig->field_list);
            segment_sig->field_list = NULL;
        }
        bits_left -= PPARSE_SRV6_SEGMENT_SIZE;
        srv6_sig->size += PPARSE_SRV6_SEGMENT_SIZE;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
pparse_srv6_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    signal_output_t * srv6_sig = NULL;
    signal_output_t * last_entry_sig = NULL;
    int data_size_bits = pdb_entry->size - (*parsed_size);

    SHR_FUNC_INIT_VARS(unit);

    
    if (data_size_bits < PPARSE_SRV6_BASE_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need at least %db for SRv6 parsing. %db given.",
                     PPARSE_SRV6_BASE_SIZE, data_size_bits);
    }

    
    {
        uint32 buffer[2];
        int base_offset = data_size_bits - PPARSE_SRV6_BASE_SIZE;
        SHR_BITCOPY_RANGE(buffer, 0, data, base_offset, PPARSE_SRV6_BASE_SIZE);
        SHR_IF_ERR_EXIT_WITH_LOG(sand_signal_parse_get
                                 (unit, PPARSE_SRV6_BASE_SIG, pdb_entry->block_name, pdb_entry->stage_from_name, NULL,
                                  PRINT_BIG_ENDIAN, buffer, PPARSE_SRV6_BASE_SIZE, pit),
                                 "Couldn't find structure '%s'.%s%s", PPARSE_SRV6_BASE_SIG, EMPTY, EMPTY);
    }

    
    srv6_sig = utilex_rhlist_entry_get_last(pit);
    
    if (!srv6_sig || sal_strcasecmp(RHNAME(srv6_sig), PPARSE_SRV6_BASE_SIG))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Expected '%s' to be the last signal in the list, but it isn't.",
                     PPARSE_SRV6_BASE_SIG);
    }
    if ((last_entry_sig = utilex_rhlist_entry_get_by_name(srv6_sig->field_list, PPARSE_SRV6_LAST_ENTRY_SIG)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Couldn't find '%s' field in %s parsed header.", PPARSE_SRV6_LAST_ENTRY_SIG,
                     PPARSE_SRV6_BASE_SIG);
    }
    
    {
        
        int nof_segments = last_entry_sig->value[0]+1;
        int segments_size = data_size_bits - PPARSE_SRV6_BASE_SIZE;
        SHR_IF_ERR_EXIT(pparse_srv6_segments_parse(unit, nof_segments, data, segments_size, srv6_sig));
    }

    *parsed_size += srv6_sig->size;

exit:
    SHR_FUNC_EXIT;
}


#define IPV6_EXT_BASE_STRUCT                   "IPv6_EXT"
#define IPV6_EXT_CHUNK_SIZE_BITS               64
#define IPV6_EXT_BASE_SIZE_BITS                IPV6_EXT_CHUNK_SIZE_BITS
#define IPV6_EXT_HDR_LEN_FIELD_NAME            "Header_Ext_Len"
#define IPV6_EXT_OPTIONAL_DATA_FIELD_NAME      "Optional_Header_Specific_Data"

#define IPV6_AH_EXT_BASE_STRUCT                "IPv6_AH_EXT"
#define IPV6_AH_EXT_CHUNK_SIZE_BITS            32
#define IPV6_AH_EXT_BASE_SIZE_BITS             96
#define IPV6_AH_EXT_OPTIONAL_DATA_FIELD_NAME   "Integrity_Check_Value"


shr_error_e
pparse_ipv6_ext_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    signal_output_t *ipv6_ext_base_sig = NULL;
    uint32 hdr_ext_len_bits = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(pparse_data_parse_by_struct_name_and_size(unit, pdb_entry, data, IPV6_EXT_BASE_STRUCT,
                                                              IPV6_EXT_BASE_SIZE_BITS, pit, parsed_size));
    ipv6_ext_base_sig = utilex_rhlist_entry_get_last(pit);

    
    {
        signal_output_t *hdr_len_sig =
                utilex_rhlist_entry_get_by_name(ipv6_ext_base_sig->field_list, IPV6_EXT_HDR_LEN_FIELD_NAME);
        SHR_NULL_CHECK(hdr_len_sig, _SHR_E_FAIL, "hdr_len_sig");
        
        hdr_ext_len_bits = IPV6_EXT_BASE_SIZE_BITS + (hdr_len_sig->value[0] * IPV6_EXT_CHUNK_SIZE_BITS);
    }

    
    if (hdr_ext_len_bits > IPV6_EXT_BASE_SIZE_BITS)
    {
        signal_output_t * optional_hdr_specific_data_sig = NULL;
        rhhandle_t rhhandle = NULL;
        int data_size_bits = pdb_entry->size - (*parsed_size);
        int options_size_bits = hdr_ext_len_bits - IPV6_EXT_BASE_SIZE_BITS;
        int options_lsb_offset = data_size_bits - options_size_bits;
        if (options_lsb_offset < 0)
        {
            
            options_size_bits += options_lsb_offset;
            options_lsb_offset = 0;
        }
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                        (ipv6_ext_base_sig->field_list, IPV6_EXT_OPTIONAL_DATA_FIELD_NAME, RHID_TO_BE_GENERATED, &rhhandle));
        optional_hdr_specific_data_sig = rhhandle;
        SHR_BITCOPY_RANGE(optional_hdr_specific_data_sig->value, 0, data, options_lsb_offset, options_size_bits);
        sand_signal_value_to_str(unit, SAL_FIELD_TYPE_UINT32, optional_hdr_specific_data_sig->value,
                                 optional_hdr_specific_data_sig->print_value, options_size_bits,
                                 PRINT_BIG_ENDIAN);
        optional_hdr_specific_data_sig->start_bit = 0;
        optional_hdr_specific_data_sig->end_bit = options_size_bits - 1;
        optional_hdr_specific_data_sig->size = options_size_bits;
        *parsed_size += options_size_bits;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pparse_ipv6_ext_ah_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size)
{
    signal_output_t *ipv6_ext_base_sig = NULL;
    uint32 hdr_ext_len_bits = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(pparse_data_parse_by_struct_name_and_size(unit, pdb_entry, data, IPV6_AH_EXT_BASE_STRUCT,
                                                              IPV6_AH_EXT_BASE_SIZE_BITS, pit, parsed_size));
    ipv6_ext_base_sig = utilex_rhlist_entry_get_last(pit);

    
    {
        signal_output_t *hdr_len_sig =
                utilex_rhlist_entry_get_by_name(ipv6_ext_base_sig->field_list, IPV6_EXT_HDR_LEN_FIELD_NAME);
        SHR_NULL_CHECK(hdr_len_sig, _SHR_E_FAIL, "hdr_len_sig");
        
        hdr_ext_len_bits = (hdr_len_sig->value[0] + 2) * IPV6_AH_EXT_CHUNK_SIZE_BITS;
    }

    
    if (hdr_ext_len_bits > IPV6_AH_EXT_BASE_SIZE_BITS)
    {
        signal_output_t * optional_hdr_specific_data_sig = NULL;
        rhhandle_t rhhandle = NULL;
        int data_size_bits = pdb_entry->size - (*parsed_size);
        int options_size_bits = hdr_ext_len_bits - IPV6_AH_EXT_BASE_SIZE_BITS;
        int options_lsb_offset = data_size_bits - options_size_bits;
        if (options_lsb_offset < 0)
        {
            
            options_size_bits += options_lsb_offset;
            options_lsb_offset = 0;
        }
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail
                        (ipv6_ext_base_sig->field_list, IPV6_AH_EXT_OPTIONAL_DATA_FIELD_NAME, RHID_TO_BE_GENERATED, &rhhandle));
        optional_hdr_specific_data_sig = rhhandle;
        SHR_BITCOPY_RANGE(optional_hdr_specific_data_sig->value, 0, data, options_lsb_offset, options_size_bits);
        sand_signal_value_to_str(unit, SAL_FIELD_TYPE_UINT32, optional_hdr_specific_data_sig->value,
                                 optional_hdr_specific_data_sig->print_value, options_size_bits,
                                 PRINT_BIG_ENDIAN);
        optional_hdr_specific_data_sig->start_bit = 0;
        optional_hdr_specific_data_sig->end_bit = options_size_bits - 1;
        optional_hdr_specific_data_sig->size = options_size_bits;
        *parsed_size += options_size_bits;
    }
exit:
    SHR_FUNC_EXIT;
}


