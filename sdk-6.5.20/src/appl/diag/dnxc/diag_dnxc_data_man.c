/** \file diag_dnxc_data_man.c
 * 
 * DEVICE DATA DIAG - diagnostic pack for module dnx_data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <soc/types.h>
#include <soc/memory.h>
#include <soc/register.h>
#include <soc/feature.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*soc*/
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
/*shared*/
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */
/* *INDENT-OFF* */
sh_sand_option_t dnxc_data_man_options[] = {
    {"label",   SAL_FIELD_TYPE_STR,   "Soc properties that should be included in the soc prop manual",       ""     , NULL, NULL, SH_SAND_ARG_QUIET},
    {"release", SAL_FIELD_TYPE_BOOL,  "Determins if the manual should be generated in release mode or not",  "true", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};
/* *INDENT-ON* */

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
sh_sand_man_t dnxc_data_man_man = {
    .brief    = "export xml format manual of all soc properties supported by device",
    .full     = "export xml format manual of all soc properties supported by device",
    .synopsis = "[file=<filename.xml>]",
    .examples = ""
};
/* *INDENT-ON* */

/**
 * \brief - checks if the soc property has a label and if this label matches the one given to the command;
 */
static shr_error_e
diag_dnxc_data_label_check(
        int unit,
        dnxc_data_property_t *property,
        char *label,
        uint8* add_soc_prop,
        int release)
{
    char **label_list = NULL;
    uint32 num_of_labels = 0;

    SHR_FUNC_INIT_VARS(unit);
    *add_soc_prop = FALSE;
    if(property->label == 0 || sal_strnlen(label, SH_SAND_MAX_TOKEN_SIZE) == 0)
    {
        if(release)
        {
            *add_soc_prop = TRUE;
        }
        SHR_EXIT();
    }
    if(sal_strnlen(label, SH_SAND_MAX_TOKEN_SIZE) == 0 && !release){
        *add_soc_prop = TRUE;
        SHR_EXIT();
    }
    utilex_str_to_lower(label);
    utilex_str_white_spaces_remove(label);
    if(sal_strstr(label, "|") != NULL)
    {
        if ((label_list =
                utilex_str_split(label, "|",32,&num_of_labels)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Wrong label format. Labels should be split with \"|\" .");
        }
    }
    else
    {
        label_list = sal_alloc(sizeof(char *), "label_list");
        label_list[0] = sal_alloc(sal_strlen(label) + 1, "label");
        sal_memcpy(label_list[0], label, strlen(label));
        num_of_labels = 1;
    }

    for(int i=0; i < num_of_labels; i++){

        if(!strncmp(label_list[i], "flexe",5))
        {
            if(_SHR_IS_FLAG_SET(property->label, DNXC_DATA_LABEL_FLEXE) && !release)
            {
                *add_soc_prop = TRUE;
            }
        }
    }
exit:
    if (label_list != NULL && num_of_labels != 0)
    {
        utilex_str_split_free(label_list, num_of_labels);
    }
    SHR_FUNC_EXIT;
}


/**
 * \brief - print property info according to the input structure 
 */
static shr_error_e
diag_dnxc_data_man_property_add(
    int unit,
    void *submodule_node,
    int depth,
    dnxc_data_property_t * property,
    char *key_name,
    char *default_val_str,
    char *label,
    int release,
    int *num_of_prop)
{

    void *property_node, *description_node, *synopsis_node, *default_val_node;
    char *property_name = NULL;
    char *synopsis_str = NULL;
    char *val_str = NULL;
    char *default_val_str_actual = NULL;
    uint8 add_soc_prop = FALSE;
    int is_suffix, len, offset, map_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(property_name, dnx_drv_soc_property_name_max(), "property_name", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(synopsis_str, dnx_drv_soc_property_name_max() * 2, "synopsis_str", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(val_str, dnx_drv_soc_property_name_max(), "val_str", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(default_val_str_actual, dnx_drv_soc_property_name_max(), "default_val_str_actual", "%s%s%s", EMPTY, EMPTY,
              EMPTY);

    /** check if the soc property should be added to the manual */
    diag_dnxc_data_label_check(unit, property, label, &add_soc_prop, release);
    if(!add_soc_prop)
    {
        SHR_EXIT();
    }

    /** property node */
    if ((property_node = dbx_xml_child_add(submodule_node, "property", depth++)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "property");
    }
    *num_of_prop += 1;

    /** get property name */
    is_suffix = property->suffix != NULL && property->suffix[0] != 0 ? 1 : 0;
    len = sal_snprintf(property_name, dnx_drv_soc_property_name_max(),
                       "%s%s%s", property->name, is_suffix ? "_" : "", is_suffix ? property->suffix : "");
    if (len >= dnx_drv_soc_property_name_max())
    {
        cli_out("soc property length unsupported\n");
        SHR_EXIT();
    }
    RHDATA_SET_STR(property_node, "name", property_name);

    /** description */
    if ((description_node =
         dbx_xml_child_set_content_str(property_node, "description", property->doc, depth++)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "description");
    }
    dbx_xml_node_end(description_node, --depth);        /* inline end */

    /** synopsis - not defined in a case of custom method*/
    if (property->method != dnxc_data_property_method_custom)
    {
        /** initialize buffers strings*/
        val_str[0] = 0;
        default_val_str_actual[0] = 0;
        switch (property->method)
        {
            case dnxc_data_property_method_enable:
            case dnxc_data_property_method_port_enable:
            case dnxc_data_property_method_suffix_enable:
            case dnxc_data_property_method_port_suffix_enable:
                sal_snprintf(val_str, dnx_drv_soc_property_name_max(), "<0 | 1>");
                sal_snprintf(default_val_str_actual, dnx_drv_soc_property_name_max(), "%s", default_val_str);
                break;
            case dnxc_data_property_method_range:
            case dnxc_data_property_method_port_range:
            case dnxc_data_property_method_suffix_range:
            case dnxc_data_property_method_port_suffix_range:
                sal_snprintf(val_str, dnx_drv_soc_property_name_max(), "<[%u - %u]>", property->range_min,
                             property->range_max);
                sal_snprintf(default_val_str_actual, dnx_drv_soc_property_name_max(), "%s", default_val_str);
                break;
            case dnxc_data_property_method_range_signed:
            case dnxc_data_property_method_port_range_signed:
            case dnxc_data_property_method_suffix_range_signed:
            case dnxc_data_property_method_port_suffix_range_signed:
                sal_snprintf(val_str, dnx_drv_soc_property_name_max(), "<[%d - %d]>", property->range_min,
                             property->range_max);
                sal_snprintf(default_val_str_actual, dnx_drv_soc_property_name_max(), "%s", default_val_str);
                break;
            case dnxc_data_property_method_direct_map:
            case dnxc_data_property_method_port_direct_map:
            case dnxc_data_property_method_suffix_direct_map:
            case dnxc_data_property_method_port_suffix_direct_map:
                offset = sal_snprintf(val_str, dnx_drv_soc_property_name_max(), "<");
                for (map_index = 0; map_index < property->nof_mapping; map_index++)
                {
                    if (property->mapping[map_index].is_invalid != TRUE)
                    {
                        offset +=
                            sal_snprintf(val_str + offset, dnx_drv_soc_property_name_max() - offset, "%s | ",
                                         property->mapping[map_index].name);

                        /** check if it the default value */
                        if (property->mapping[map_index].is_default)
                        {
                            sal_snprintf(default_val_str_actual, dnx_drv_soc_property_name_max(), "%s",
                                         property->mapping[map_index].name);
                        }
                    }
                }
                sal_snprintf(val_str + offset - 3, dnx_drv_soc_property_name_max() - offset, ">");
                break;
            case dnxc_data_property_method_pbmp:
            case dnxc_data_property_method_port_pbmp:
            case dnxc_data_property_method_suffix_pbmp:
                sal_snprintf(val_str, dnx_drv_soc_property_name_max(), "<port bitmap>");
                sal_snprintf(default_val_str_actual, dnx_drv_soc_property_name_max(), "%s", default_val_str);
                break;
            case dnxc_data_property_method_str:
            case dnxc_data_property_method_port_str:
            case dnxc_data_property_method_suffix_str:
            case dnxc_data_property_method_port_suffix_str:
                sal_snprintf(val_str, dnx_drv_soc_property_name_max(), "<string>");
                sal_snprintf(default_val_str_actual, dnx_drv_soc_property_name_max(), "%s", default_val_str);
                break;
            default:
                /*
                 * do nothing
                 */
                break;
        }

        if (is_suffix)
        {
            if (key_name == NULL)
            {
                sal_sprintf(synopsis_str, "%s[_%s]=%s", property->name, property->suffix, val_str);
            }
            else
            {
                sal_sprintf(synopsis_str, "%s[_%s<%s>]=%s", property->name, property->suffix, key_name, val_str);
            }
        }
        else
        {
            sal_sprintf(synopsis_str, "%s=%s", property->name, val_str);
        }
        if ((synopsis_node = dbx_xml_child_set_content_str(property_node, "synopsis", synopsis_str, depth++)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "synopsis");
        }
        dbx_xml_node_end(synopsis_node, 0);     /* inline end */
        depth--;
        if ((default_val_node =
             dbx_xml_child_set_content_str(property_node, "default", default_val_str_actual, depth++)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "default");
        }
        dbx_xml_node_end(default_val_node, 0);  /* inline end */
        depth--;
    }

    dbx_xml_node_end(property_node, --depth);

exit:
    SHR_FREE(property_name);
    SHR_FREE(synopsis_str);
    SHR_FREE(val_str);
    SHR_FREE(default_val_str_actual);
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_submodule_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *parent_node,
    dnxc_data_module_t * module,
    int submodule_index,
    int silent,
    int *should_print,
    char *label,
    int release,
    int *module_nof_prop)
{
    dnxc_data_submodule_t *submodule;
    dnxc_data_table_t *table;
    dnxc_data_feature_t *feature;
    dnxc_data_define_t *define;
    void *submodule_node = NULL;
    int dump;
    int depth = 2;
    int table_index, feature_index, define_index, value_index;
    int submodule_nof_prop = 0;
    char *default_val_str = NULL;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(default_val_str, dnx_drv_soc_property_name_max(), "default_val_str", "%s%s%s", EMPTY, EMPTY, EMPTY);
    submodule = &module->submodules[submodule_index];

    if (!silent)
    {
        submodule_node = dbx_xml_child_add(parent_node, "submodule", depth++);
        if (submodule_node == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "submodule");
        }
        RHDATA_SET_STR(submodule_node, "name", submodule->name);
        RHDATA_SET_STR(submodule_node, "description", submodule->doc);
    }

    /*
     * iterate of tables
     */
    for (table_index = 0; table_index < submodule->nof_tables; table_index++)
    {
        table = &submodule->tables[table_index];
        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, table->flags, table->labels, 0, NULL, &dump);
        SHR_IF_ERR_EXIT(rv);
        /** filter internal properties */
        if (table->flags & DNXC_DATA_F_PROPERTY_INTERNAL)
        {
            continue;
        }

        if (dump)
        {
            /*
             * check if data loaded by SoC property and soc property is documented
             */
            if (table->property.name != NULL && table->property.doc != NULL)
            {
                if (silent)
                {
                    *should_print = 1;
                    SHR_EXIT();
                }

                SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                (unit, submodule_node, depth, &table->property, table->keys[0].name,
                                 table->values[0].default_val, label, release, &submodule_nof_prop));
            }

            for (value_index = 0; value_index < table->nof_values; value_index++)
            {
                /*
                 * check if data loaded be SoC property and documented
                 */
                if (table->values[value_index].property.name != NULL && table->values[value_index].property.doc != NULL)
                {
                    if (silent)
                    {
                        *should_print = 1;
                        SHR_EXIT();
                    }

                    SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                    (unit, submodule_node, depth, &table->values[value_index].property,
                                     table->keys[0].name, table->values[value_index].default_val, label, release, &submodule_nof_prop));
                }
            }
        }       /* Dump table */
    }   /* table iterator */

    /*
     * iterate of features
     */
    for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
    {
        feature = &submodule->features[feature_index];
        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, feature->flags, feature->labels, 0, NULL, &dump);
        SHR_IF_ERR_EXIT(rv);
        /** filter internal properties */
        if (feature->flags & DNXC_DATA_F_PROPERTY_INTERNAL)
        {
            continue;
        }

        if (dump)
        {
            /*
             * check if data loaded be SoC property and documented
             */
            if (feature->property.name != NULL && feature->property.doc != NULL)
            {
                if (silent)
                {
                    *should_print = 1;
                    SHR_EXIT();
                }

                SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                (unit, submodule_node, depth, &feature->property, NULL,
                                 feature->default_data ? "1" : "0", label, release, &submodule_nof_prop));
            }
        }       /* Dump feature */
    }   /* feature iterator */

    /*
     * iterate of numerics
     */
    for (define_index = 0; define_index < submodule->nof_defines; define_index++)
    {
        define = &submodule->defines[define_index];
        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, define->flags, define->labels, 0, NULL, &dump);
        SHR_IF_ERR_EXIT(rv);
        /** filter internal properties */
        if (define->flags & DNXC_DATA_F_PROPERTY_INTERNAL)
        {
            continue;
        }

        if (dump)
        {

            /*
             * check if data loaded be SoC property and documented
             */
            if (define->property.name != NULL && define->property.doc != NULL)
            {
                if (silent)
                {
                    *should_print = 1;
                    SHR_EXIT();
                }

                sal_snprintf(default_val_str, dnx_drv_soc_property_name_max(), "%d", define->default_data);
                SHR_IF_ERR_EXIT(diag_dnxc_data_man_property_add
                                (unit, submodule_node, depth, &define->property, NULL, default_val_str, label, release, &submodule_nof_prop));
            }
        }       /* Dump numeric */
    }   /* numeric iterator */
    *module_nof_prop += submodule_nof_prop;
    if (!silent)
    {
        dbx_xml_node_end(submodule_node, --depth);

        if(submodule_nof_prop == 0)
        {
            dbx_xml_node_delete(submodule_node);
        }
    }
exit:
    SHR_FREE(default_val_str);
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_module_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *parent_node,
    int module_index,
    int silent,
    int *should_print,
    char* label,
    int release)
{
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    dnxc_data_module_t *module;
    void *module_node = NULL;
    int submodule_index;
    int module_nof_prop = 0;
    SHR_FUNC_INIT_VARS(unit);

    module = &modules[module_index];

    /** skip module with no soc properties */
    *should_print = 0;

    if (!silent)
    {
        if ((module_node = dbx_xml_child_add(parent_node, "module", 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "module");
        }
        RHDATA_SET_STR(module_node, "name", module->name);
    }
    /*
     * iterate of submodules
     */
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        *should_print = 0;

        SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_man
                        (unit, args, sand_control, module_node, module, submodule_index, 1, should_print, label, release, &module_nof_prop));

        if (*should_print)
        {
            if (silent)
            {
                SHR_EXIT();
            }
            else
            {
                SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_man
                                (unit, args, sand_control, module_node, module, submodule_index, silent, should_print, label, release, &module_nof_prop));
            }
        }
    }   /* submodule iterator */

    if (!silent)
    {
        dbx_xml_node_end(module_node, 1);
        if(module_nof_prop == 0)
        {
            dbx_xml_node_delete(module_node);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
shr_error_e
cmd_dnxc_data_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;
    void *top_node;
    void *device_node;
    int should_print;
    int module_index;
    char* label;
    int release;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file", filename);
    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_BOOL("release", release);
    /*
     * Validate input
     */
    if (ISEMPTY(filename))
    {
        filename = "manual.xml";
    }
    else
    {
        if (dbx_file_get_type(filename) != DBX_FILE_XML)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "File must have .xml suffix. File name was:\"%s\"\n", filename);
        }
    }
    /*
     * Create XML Document
     */
    if ((top_node = dbx_file_get_xml_top(unit, filename, "top",
                                         CONF_OPEN_CREATE | CONF_OPEN_OVERWRITE | CONF_OPEN_CURRENT_LOC)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Failed to create:\"%s\"\n", filename);
    }

    if ((device_node = dbx_xml_child_add(top_node, "device", 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "failed to add %s node\n", "module");
    }
    RHDATA_SET_STR(device_node, "name", (char *) dnx_drv_soc_dev_name(unit));

    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        should_print = 0;
        /** first check if should print */
        SHR_IF_ERR_EXIT(diag_dnxc_data_module_man
                        (unit, args, sand_control, device_node, module_index, 1, &should_print, label, release));

        /** print if required */
        if (should_print)
        {
            SHR_IF_ERR_EXIT(diag_dnxc_data_module_man
                            (unit, args, sand_control, device_node, module_index, 0, &should_print, label, release));
        }
    }   /* module iterator */

    dbx_xml_node_end(device_node, 0);
    dbx_xml_top_save(top_node, filename);
    dbx_xml_top_close(top_node);

exit:
    SHR_FUNC_EXIT;
}
