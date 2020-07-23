/** \file diag_dnxc_data.c
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
/*appl*/
#include <appl/diag/diag.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*soc*/
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
/*shared*/
#include <shared/utilex/utilex_integer_arithmetic.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/**
 * \brief
 * Avoid from output tables with to many values
 */
#define DIAG_DNXC_DATA_MAX_VALUES_TO_DISPLAY 20

/*
 * LOCAL FUNCTIONs:
 * {
 */
/**
 * \brief - Dump dnxc data 
 *          If specific data - will dump the requested data
 *          if "*" - will dump the all branch
 *          if NULL - will display list of supported data,
 * \par DIRECT INPUT:
 *   \param [in] unit -  Unit #
 *   \param [in] flags - data flags to get see DNXC_DATA_DATA_F_* for details
 *   \param [in] label - filter the according to label (use NULL or "" to skip label filtering)
 *   \param [in] module - module to get or "*" for all modules
 *   \param [in] submodule - submodule to get or "*" for all submodules
 *   \param [in] data - data to get or "*" for all data
 *   \param [in] sand_control - diag framework data
 *   
 * \par INDIRECT INPUT:
 *   * module global data - _dnxc_data[unit]
 * \par DIRECT OUTPUT:
 *   see shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Output to screen - see brief
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_data_dump(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data,
    sh_sand_control_t * sand_control);

/**
 * \brief - Display info about dnxc data
 *          If specific data - will display info the requested data
 *          if "*" - will display info the all branch
 *          if NULL - will display list of supported data,
 * \par DIRECT INPUT:
 *   \param [in] unit -  Unit #
 *   \param [in] flags - data flags to get see DNXC_DATA_DATA_F_* for details
 *   \param [in] label - filter the according to label (use NULL or "" to skip label filtering)
 *   \param [in] module - module to get or "*" for all modules
 *   \param [in] submodule - submodule to get or "*" for all submodules
 *   \param [in] data - data to get or "*" for all data    
 *   \param [in] sand_control - diag framework data
 *   
 * \par INDIRECT INPUT:
 *   * module global data - _dnxc_data[unit]
 * \par DIRECT OUTPUT:
 *   see shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Output to screen - see brief
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e diag_dnxc_data_info(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data,
    sh_sand_control_t * sand_control);

/**
 * \brief - Print property info
 *          If specific soc property - will display soc property info
 *          Otherwise will display list of soc property includes the requested soc property 
 * \par DIRECT INPUT:
 *   \param [in] unit -  Unit #
 *   \param [in] label - filter the according to label (use NULL or "" to skip label filtering)
 *   \param [in] property - property name or substring of property name
 *   \param [in] sand_control - diag framework
 *
 * \par INDIRECT INPUT:
 *   * module global data - _dnxc_data[unit]
 * \par DIRECT OUTPUT:
 *   see shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Output to screen - see brief
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e diag_dnxc_data_property(
    int unit,
    char *label,
    char *property,
    sh_sand_control_t * sand_control);

/*
 * man command extern
 */

extern shr_error_e cmd_dnxc_data_man(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
extern sh_sand_man_t dnxc_data_man_man;
extern sh_sand_option_t dnxc_data_man_options[];

/**
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */
/* *INDENT-OFF* */
static sh_sand_option_t dnxc_data_dump_options[] = {
    {"changed",  SAL_FIELD_TYPE_BOOL, "dump changed only properties",                     "no"},
    {"define",   SAL_FIELD_TYPE_BOOL, "dump defines only",                                "no"},
    {"numeric",  SAL_FIELD_TYPE_BOOL, "dump numerics only",                               "no"},
    {"feature",  SAL_FIELD_TYPE_BOOL, "dump features only",                               "no"},
    {"table",    SAL_FIELD_TYPE_BOOL, "dump table only",                                  "no"},
    {"property", SAL_FIELD_TYPE_BOOL, "dump properties only",                             "no"},
    {"label",    SAL_FIELD_TYPE_STR,  "dump data marked with specified label only",        ""},
    {"silence",  SAL_FIELD_TYPE_BOOL,  NULL,                                               "true"},
    {"name", SAL_FIELD_TYPE_STR,  "[<module_name>[.<submodule_name>[.<data_name>]]]",  "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
/* *INDENT-ON* */

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
static sh_sand_man_t dnxc_data_dump_man = {
    .brief    = "Dump data or get list of modules/submodules/data",
    .full     = "Please fill here full description for data dump",
    .synopsis = "[changed] [property] [define/numeric/feature/table] [label=<label>] [<module_name>[.<submodule_name>[.<data_name>]]]'",
    .examples = "fabric.pipes.map\n"
                "fabric.pipes.*\n"
                "fabric.pipes.\n"
                "fabric.\n"
                "chg *\n"
                "label=tm *\n"
                "define fabric.*\n"
                "numeric fabric.*\n"
                "feature fabric.*\n"
                "table fabric.*\n"
                "property fabric.*\n"
                "silence *\n"

};
/* *INDENT-ON* */

/**
 * \brief - parse args and call to diag_dnxc_data_dump() with the requested data params
 */
static shr_error_e
cmd_dnxc_data_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *module = NULL;
    char *submodule = NULL;
    char *data = NULL;
    uint32 flags = 0;
    uint32 nof_tokens = 0;
    char **datatokens = NULL;
    char *var, *label;
    int changed_flag, define_flag, feature_flag, numeric_flag, table_flag, property_flag;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get params
     */
    SH_SAND_GET_BOOL("changed", changed_flag);
    SH_SAND_GET_BOOL("define", define_flag);
    SH_SAND_GET_BOOL("numeric", numeric_flag);
    SH_SAND_GET_BOOL("feature", feature_flag);
    SH_SAND_GET_BOOL("table", table_flag);
    SH_SAND_GET_BOOL("property", property_flag);
    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_STR("name", var);

    if (changed_flag)
    {
        /*
         * Only property data can be marked as changed
         */
        flags |= DNXC_DATA_F_CHANGED;
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
        flags |= DNXC_DATA_F_DEFINE;
    }
    if (property_flag)
    {
        /*
         * Only numeric, feature and table support property
         */
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
    }
    if (feature_flag)
    {
        flags |= DNXC_DATA_F_FEATURE;
    }

    if (define_flag)
    {
        flags |= DNXC_DATA_F_DEFINE;
    }
    if (table_flag)
    {
        flags |= DNXC_DATA_F_TABLE;
    }
    if (numeric_flag)
    {
        flags |= DNXC_DATA_F_NUMERIC;
    }

    /*
     * Expected format - <module>.<submodule>.<data>
     */
    datatokens = utilex_str_split(var, ".", 3, &nof_tokens);
    if (datatokens != NULL)
    {
        if (nof_tokens > 0)
        {
            module = datatokens[0];
        }
        if (nof_tokens > 1)
        {
            submodule = datatokens[1];
        }
        if (nof_tokens > 2)
        {
            data = datatokens[2];
        }
        if (nof_tokens > 3)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, ": data dump - data format not supported - %s\n", var);
        }
    }
    /*
     * set empty token to null
     */
    if (module != NULL && module[0] == 0)
    {
        module = NULL;
    }
    if (submodule != NULL && submodule[0] == 0)
    {
        submodule = NULL;
    }
    if (data != NULL && data[0] == 0)
    {
        data = NULL;
    }
    /*
     * Set '*' along the chain
     */
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (module != NULL && !sal_strcasecmp("*", module))
    {
        submodule = "*";
        data = "*";
    }
    if (submodule != NULL && !sal_strcasecmp("*", submodule))
    {
        data = "*";
    }

    /*
     * Call to dnxc data dump with the relevant params
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_dump(unit, flags, label, module, submodule, data, sand_control));
exit:
/*
 * Free resources
 */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
static sh_sand_option_t dnxc_data_info_options[] = {
    {"changed",  SAL_FIELD_TYPE_BOOL, "dump changed only properties",                     "no"},
    {"define",   SAL_FIELD_TYPE_BOOL, "dump defines only",                                "no"},
    {"numeric",  SAL_FIELD_TYPE_BOOL, "dump numerics only",                               "no"},
    {"feature",  SAL_FIELD_TYPE_BOOL, "dump features only",                               "no"},
    {"table",    SAL_FIELD_TYPE_BOOL, "dump table only",                                  "no"},
    {"property", SAL_FIELD_TYPE_BOOL, "dump properties only",                             "no"},
    {"label",    SAL_FIELD_TYPE_STR,  "dump data marked with specified label only",        ""},
    {"name", SAL_FIELD_TYPE_STR,  "[<module_name>[.<submodule_name>[.<data_name>]]]", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
/* *INDENT-ON* */

/**
 * \brief - see definition on local function section (top of this file)
 */
/* *INDENT-OFF* */
static sh_sand_man_t dnxc_data_info_man = {
    .brief    = "Get list of modules/submodules/data or full description id data specified",
    .full     = "if '*' is specified will display info about the all branch",
    .synopsis = "[changed] [property] [define/numeric/feature/table] [label=<label>] [<module_name>[.<submodule_name>[.<data_name>]]]",
    .examples = "fabric.pipes.map\n"
                "fabric.pipes.*\n"
                "fabric.\n"
                "chg *\n"
                "label=tm *\n"
                "define fabric.*\n"
                "numeric fabric.*\n"
                "feature fabric.*\n"
                "table fabric.*\n"
                "property fabric.*\n"

};
/* *INDENT-ON* */

/**
 * \brief - parse args and call to diag_dnxc_data_info() with the requested data params
 */
static shr_error_e
cmd_dnxc_data_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *module = NULL;
    char *submodule = NULL;
    char *data = NULL;
    uint32 flags = 0;
    uint32 nof_tokens = 0;
    char **datatokens = NULL;
    char *var, *label;
    int changed_flag, define_flag, feature_flag, numeric_flag, table_flag, property_flag;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get params
     */
    SH_SAND_GET_BOOL("changed", changed_flag);
    SH_SAND_GET_BOOL("define", define_flag);
    SH_SAND_GET_BOOL("numeric", numeric_flag);
    SH_SAND_GET_BOOL("feature", feature_flag);
    SH_SAND_GET_BOOL("table", table_flag);
    SH_SAND_GET_BOOL("property", property_flag);
    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_STR("name", var);

    if (changed_flag)
    {
        /*
         * Only property data can be marked as changed
         */
        flags |= DNXC_DATA_F_CHANGED;
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
    }
    if (property_flag)
    {
        /*
         * Only numeric, feature and table support property
         */
        flags |= DNXC_DATA_F_PROPERTY;
        flags |= DNXC_DATA_F_NUMERIC;
        flags |= DNXC_DATA_F_FEATURE;
        flags |= DNXC_DATA_F_TABLE;
    }
    if (feature_flag)
    {
        flags |= DNXC_DATA_F_FEATURE;
    }

    if (define_flag)
    {
        flags |= DNXC_DATA_F_DEFINE;
    }
    if (table_flag)
    {
        flags |= DNXC_DATA_F_TABLE;
    }
    if (numeric_flag)
    {
        flags |= DNXC_DATA_F_NUMERIC;
    }

    /*
     * Expected format - <module>.<submodule>.<data>
     */
    datatokens = utilex_str_split(var, ".", 3, &nof_tokens);
    if (datatokens != NULL)
    {
        if (nof_tokens > 0)
        {
            module = datatokens[0];
        }
        if (nof_tokens > 1)
        {
            submodule = datatokens[1];
        }
        if (nof_tokens > 2)
        {
            data = datatokens[2];
        }
        if (nof_tokens > 3)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, ": data dump - data format not supported - %s\n", var);
        }
    }
    /*
     * set empty token to null
     */
    if (module != NULL && module[0] == 0)
    {
        module = NULL;
    }
    if (submodule != NULL && submodule[0] == 0)
    {
        submodule = NULL;
    }
    if (data != NULL && data[0] == 0)
    {
        data = NULL;
    }
    /*
     * Set '*' along the chain
     */
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (module != NULL && !sal_strcasecmp("*", module))
    {
        submodule = "*";
        data = "*";
    }
    if (submodule != NULL && !sal_strcasecmp("*", submodule))
    {
        data = "*";
    }

    SHR_IF_ERR_EXIT(diag_dnxc_data_info(unit, flags, label, module, submodule, data, sand_control));
exit:
/*
 * Free resources
 */
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }
    SHR_FUNC_EXIT;
}
/* *INDENT-OFF* */
static sh_sand_option_t dnxc_data_property_options[] = {
    {"label",    SAL_FIELD_TYPE_STR, "Filter soc properties according to data label", ""},
    {"name", SAL_FIELD_TYPE_STR, "Filter soc properties by this substring", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnx_data_property_man = {
    .brief    = "Get list of soc properties contains substring <property> or info about soc property named <property>",
    .synopsis = "<property>",
    .examples = "fabric_pcp_enable\n"
                "label=tm name=\n"
};

static sh_sand_man_t dnxf_data_property_man = {
    .brief    = "Get list of soc properties contains substring <property> or info about soc property named <property>",
    .synopsis = "<property>",
    .examples = "fabric_device_mode\n"
                "label=tm name=\n"
};
/* *INDENT-ON* */

/**
 * \brief - parse args and call to diag_dnxc_data_property() with the requested data params
 */
static shr_error_e
cmd_dnxc_data_property(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *property;
    char *label;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("label", label);
    SH_SAND_GET_STR("name", property);

    /*
     * Call to dnxc data dump with the relevant params
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_property(unit, label, property, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of defines according to flags
 */
static shr_error_e
diag_dnxc_data_list(
    int unit,
    char *module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    char *label,
    sh_sand_control_t * sand_control)
{
    int define_index, feature_index, table_index;

    int dump;
    shr_error_e rv;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Dump define / numeric info
     */

    PRT_TITLE_SET("LIST OF DATA IN SUBMODULE %s.%s:", module, submodule->name);
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Type");

    /*
     * Defines
     */
    for (define_index = 0; define_index < submodule->nof_defines; define_index++)
    {

        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->defines[define_index].flags,
                                         submodule->defines[define_index].labels, flags | DNXC_DATA_F_DEFINE, label,
                                         &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", submodule->defines[define_index].name);
            PRT_CELL_SET("%s", "DEFINE");
        }
    }

    /*
     * Numerics
     */
    for (define_index = 0; define_index < submodule->nof_defines; define_index++)
    {

        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->defines[define_index].flags,
                                         submodule->defines[define_index].labels, flags | DNXC_DATA_F_NUMERIC, label,
                                         &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", submodule->defines[define_index].name);
            PRT_CELL_SET("%s", "NUMERIC");
        }
    }

    /*
     * Features
     */
    for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
    {

        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->features[feature_index].flags,
                                         submodule->features[feature_index].labels, flags, label, &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", submodule->features[feature_index].name);
            PRT_CELL_SET("%s", "FEATURE");
        }
    }

    /*
     * Features
     */
    for (table_index = 0; table_index < submodule->nof_tables; table_index++)
    {

        /*
         * Check if data is supported
         */
        rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->tables[table_index].flags,
                                         submodule->tables[table_index].labels, flags, label, &dump);
        SHR_IF_ERR_EXIT(rv);

        if (dump)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", submodule->tables[table_index].name);
            PRT_CELL_SET("%s", "TABLE");
        }
    }

    PRT_COMMITX;
    cli_out("To dump all the data in a specified submodule, type: 'data dump <module>.<submodule>.*'\n");
    cli_out("To dump specified data, type: 'data dump <module>.<submodule>.<data>'\n");

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * \brief - print info about the define named 'data' 
 *          if 'data' == '*" print info of all defines according to flags. 
 */
static shr_error_e
diag_dnxc_submodule_data_info(
    int unit,
    char *module,
    dnxc_data_submodule_t * submodule,
    char *data,
    uint32 flags,
    char *label,
    sh_sand_control_t * sand_control)
{
    int define_index, feature_index, table_index, key_index, value_index;

    int dump;
    shr_error_e rv;
    char buffer[DNXC_DATA_UTILS_STRING_MAX];
    int label_index;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure that data exist
     */
    if (data == NULL)
    {
        SHR_IF_ERR_EXIT(diag_dnxc_data_list(unit, module, submodule, flags, label, sand_control));
    }
    else
    {
        /*
         * Dump define / numeric info
         */
        PRT_TITLE_SET("DATA IN SUBMODULE %s.%s:", module, submodule->name);
        PRT_COLUMN_ADD("Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

        /*
         * Defines
         */
        for (define_index = 0; define_index < submodule->nof_defines; define_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp("*", data) ||
                !sal_strncasecmp(submodule->defines[define_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
            {
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->defines[define_index].flags,
                                                 submodule->defines[define_index].labels, flags | DNXC_DATA_F_DEFINE,
                                                 label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Name");
                    PRT_CELL_SET("%s", submodule->defines[define_index].name);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("Type");
                    PRT_CELL_SET("%s", "DEFINE");

                    if (submodule->defines[define_index].doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "DOC");
                        PRT_CELL_SET("%s", submodule->defines[define_index].doc);
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Default Value");
                    PRT_CELL_SET("%d", submodule->defines[define_index].default_data);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "INIT ONLY");
                    PRT_CELL_SET("%s", (submodule->defines[define_index].flags & DNXC_DATA_F_INIT_ONLY) ? "Yes" : "No");

                    buffer[0] = 0;
                    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
                    {
                        if (submodule->defines[define_index].labels[label_index] != NULL)
                        {
                            sal_snprintf(buffer, DNXC_DATA_UTILS_STRING_MAX, "%s %s", buffer,
                                         submodule->defines[define_index].labels[label_index]);
                        }
                    }
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "LABELS");
                        PRT_CELL_SET("%s", buffer);
                    }
                }
            }
        }

        /*
         * Numerics
         */
        for (define_index = 0; define_index < submodule->nof_defines; define_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp("*", data) ||
                !sal_strncasecmp(submodule->defines[define_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
            {
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->defines[define_index].flags,
                                                 submodule->defines[define_index].labels, flags | DNXC_DATA_F_NUMERIC,
                                                 label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Name");
                    PRT_CELL_SET("%s", submodule->defines[define_index].name);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Type");
                    PRT_CELL_SET("%s", "NUMERIC");

                    if (submodule->defines[define_index].doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "DOC");
                        PRT_CELL_SET("%s", submodule->defines[define_index].doc);
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Default Value");
                    PRT_CELL_SET("%d", submodule->defines[define_index].default_data);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "INIT ONLY");
                    PRT_CELL_SET("%s", (submodule->defines[define_index].flags & DNXC_DATA_F_INIT_ONLY) ? "Yes" : "No");

                    buffer[0] = 0;
                    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
                    {
                        if (submodule->defines[define_index].labels[label_index] != NULL)
                        {
                            sal_snprintf(buffer, DNXC_DATA_UTILS_STRING_MAX, "%s %s", buffer,
                                         submodule->defines[define_index].labels[label_index]);
                        }
                    }
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "LABELS");
                        PRT_CELL_SET("%s", buffer);
                    }

                    if (submodule->defines[define_index].property.method_str != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY METHOD");
                        PRT_CELL_SET("%s", submodule->defines[define_index].property.method_str);
                    }
                    if (submodule->defines[define_index].property.name != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY NAME");
                        PRT_CELL_SET("%s", submodule->defines[define_index].property.name);
                    }
                    if (submodule->defines[define_index].property.suffix != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY SUFFIX");
                        PRT_CELL_SET("%s", submodule->defines[define_index].property.suffix);
                    }
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, &submodule->defines[define_index].property, buffer,
                                     DNXC_DATA_UTILS_STRING_MAX, 1));
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY VALUES");
                        PRT_CELL_SET("%s", buffer);
                    }
                    if (submodule->defines[define_index].property.doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY DOC");
                        PRT_CELL_SET("%s", submodule->defines[define_index].property.doc);
                    }
                }
            }
        }

        /*
         * Features
         */
        for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp("*", data) ||
                !sal_strncasecmp(submodule->features[feature_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
            {
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->features[feature_index].flags,
                                                 submodule->features[feature_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("%s", "Name");
                    PRT_CELL_SET("%s", submodule->features[feature_index].name);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Type");
                    PRT_CELL_SET("%s", "FEATURE");

                    if (submodule->features[feature_index].doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "DOC");
                        PRT_CELL_SET("%s", submodule->features[feature_index].doc);
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Default Value");
                    PRT_CELL_SET("%d", submodule->features[feature_index].default_data);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "INIT ONLY");
                    PRT_CELL_SET("%s",
                                 (submodule->features[feature_index].flags & DNXC_DATA_F_INIT_ONLY) ? "Yes" : "No");

                    buffer[0] = 0;
                    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
                    {
                        if (submodule->features[feature_index].labels[label_index] != NULL)
                        {
                            sal_snprintf(buffer, DNXC_DATA_UTILS_STRING_MAX, "%s %s", buffer,
                                         submodule->features[feature_index].labels[label_index]);
                        }
                    }
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "LABELS");
                        PRT_CELL_SET("%s", buffer);
                    }

                    if (submodule->features[feature_index].property.method_str != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY METHOD");
                        PRT_CELL_SET("%s", submodule->features[feature_index].property.method_str);
                    }
                    if (submodule->features[feature_index].property.name != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY NAME");
                        PRT_CELL_SET("%s", submodule->features[feature_index].property.name);
                    }
                    if (submodule->features[feature_index].property.suffix != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY SUFFIX");
                        PRT_CELL_SET("%s", submodule->features[feature_index].property.suffix);
                    }
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, &submodule->features[feature_index].property, buffer,
                                     DNXC_DATA_UTILS_STRING_MAX, 1));
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY VALUES");
                        PRT_CELL_SET("%s", buffer);
                    }
                    if (submodule->features[feature_index].property.doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY DOC");
                        PRT_CELL_SET("%s", submodule->features[feature_index].property.doc);
                    }
                }
            }

        }

        /*
         * Tables
         */
        for (table_index = 0; table_index < submodule->nof_tables; table_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp("*", data) ||
                !sal_strncasecmp(submodule->tables[table_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
            {
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->tables[table_index].flags,
                                                 submodule->tables[table_index].labels, flags, label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("%s", "Name");
                    PRT_CELL_SET("%s", submodule->tables[table_index].name);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Type");
                    PRT_CELL_SET("%s", "TABLE");

                    if (submodule->tables[table_index].doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "DOC");
                        PRT_CELL_SET("%s", submodule->tables[table_index].doc);
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "INIT ONLY");
                    PRT_CELL_SET("%s", (submodule->tables[table_index].flags & DNXC_DATA_F_INIT_ONLY) ? "Yes" : "No");

                    buffer[0] = 0;
                    for (label_index = 0; label_index < DNXC_DATA_LABELS_MAX; label_index++)
                    {
                        if (submodule->tables[table_index].labels[label_index] != NULL)
                        {
                            sal_snprintf(buffer, DNXC_DATA_UTILS_STRING_MAX, "%s %s", buffer,
                                         submodule->tables[table_index].labels[label_index]);
                        }
                    }
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "LABELS");
                        PRT_CELL_SET("%s", buffer);
                    }

                    /*
                     * Keys
                     */

                    for (key_index = 0; key_index < submodule->tables[table_index].nof_keys; key_index++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("KEY%d NAME", key_index);
                        PRT_CELL_SET("%s", submodule->tables[table_index].keys[key_index].name)
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("KEY%d DOC", key_index);
                        PRT_CELL_SET("%s", submodule->tables[table_index].keys[key_index].doc)
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("KEY%d SIZE", key_index);
                    PRT_CELL_SET("%d", submodule->tables[table_index].keys[key_index].size)}

                    /*
                     * Dump values info
                     */
                    for (value_index = 0; value_index < submodule->tables[table_index].nof_values; value_index++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "-");
                        PRT_CELL_SET("%s", "-");

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("VALUE%d NAME", value_index);
                        PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].name)
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("VALUE%d DOC", value_index);
                        PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].doc)
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("VALUE%d TYPE", value_index);
                        PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].type)
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("VALUE%d DEFAULT", value_index);
                        PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].default_val)
                            if (submodule->tables[table_index].values[value_index].property.method_str != NULL)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("VALUE%d PROPERTY METHOD", value_index);
                            PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].property.method_str);
                        }
                        if (submodule->tables[table_index].values[value_index].property.name != NULL)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("VALUE%d PROPERTY NAME", value_index);
                            PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].property.name);
                        }
                        if (submodule->tables[table_index].values[value_index].property.suffix != NULL)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("VALUE%d PROPERTY SUFFIX", value_index);
                            PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].property.suffix);
                        }
                        buffer[0] = 0;
                        SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                        (unit, &submodule->tables[table_index].values[value_index].property, buffer,
                                         DNXC_DATA_UTILS_STRING_MAX, 1));
                        if (buffer[0] != 0)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("VALUE%d PROPERTY VALUES", value_index);
                            PRT_CELL_SET("%s", buffer);
                        }
                        if (submodule->tables[table_index].values[value_index].property.doc != NULL)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("VALUE%d PROPERTY DOC", value_index);
                            PRT_CELL_SET("%s", submodule->tables[table_index].values[value_index].property.doc);
                        }

                    }

                    /*
                     * Table property
                     */
                    if (submodule->tables[table_index].property.method_str != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY METHOD");
                        PRT_CELL_SET("%s", submodule->tables[table_index].property.method_str);
                    }
                    if (submodule->tables[table_index].property.name != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY NAME");
                        PRT_CELL_SET("%s", submodule->tables[table_index].property.name);
                    }
                    if (submodule->tables[table_index].property.suffix != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY SUFFIX");
                        PRT_CELL_SET("%s", submodule->tables[table_index].property.suffix);
                    }
                    buffer[0] = 0;
                    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get
                                    (unit, &submodule->tables[table_index].property, buffer, DNXC_DATA_UTILS_STRING_MAX,
                                     1));
                    if (buffer[0] != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY VALUES");
                        PRT_CELL_SET("%s", buffer);
                    }
                    if (submodule->tables[table_index].property.doc != NULL)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "PROPERTY DOC");
                        PRT_CELL_SET("%s", submodule->tables[table_index].property.doc);
                    }
                }
            }
        }

        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - dump table
 */
static shr_error_e
diag_dnxc_data_table_dump(
    int unit,
    char *module,
    char *submodule,
    dnxc_data_table_t * table,
    int dump_flags,
    sh_sand_control_t * sand_control)
{
    int key1_index, key2_index, value_index, key_index;
    int key1_size, key2_size;
    int key1, key2;
    char *buffer = NULL;
    int changed;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** alloc buffer */
    SHR_ALLOC(buffer, sizeof(char) * DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "buffer for dnxc data table dump", "%s%s%s",
              EMPTY, EMPTY, EMPTY);

    /*
     * Dump table
     */
    /*
     * Get keys size
     */
    key1_size = 0;
    key2_size = 0;
    if (table->nof_keys > 0)
    {
        key1_size = table->keys[0].size;
        if (table->nof_keys > 1)
        {
            key2_size = table->keys[1].size;
        }
    }

    /*
     * define rows for table
     */
    PRT_TITLE_SET("TABLE: '%s.%s.%s'", module, submodule, table->name);
    for (key_index = 0; key_index < table->nof_keys; key_index++)
    {
        PRT_COLUMN_ADD("%s", table->keys[key_index].name);
    }
    PRT_COLUMN_ADD("#");
    for (value_index = 0; value_index < UTILEX_MIN(table->nof_values, DIAG_DNXC_DATA_MAX_VALUES_TO_DISPLAY);
         value_index++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", table->values[value_index].name);
    }
    /*
     * Iterate entries and print keys and values
     */
    for (key1_index = 0; key1_index < key1_size || ((table->nof_keys == 0) && (key1_index == 0)); key1_index++)
    {
        for (key2_index = 0; (key2_index < key2_size) || ((table->nof_keys <= 1) && (key2_index == 0)); key2_index++)
        {
            /*
             * Filter changed entries according to dump flags
             */
            changed = 1;
            if (dump_flags & DNXC_DATA_F_CHANGED)
            {
                SHR_IF_ERR_EXIT(dnxc_data_mgmt_table_entry_changed_get(unit, table, key1_index, key2_index, &changed));
            }
            if (!changed)
            {
                continue;
            }

            /*
             *  Print entry
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            /*
             * In table "map" modes. first map indices to keys
             */
            if (table->key_map_reverse != NULL)
            {
                SHR_IF_ERR_EXIT(table->key_map_reverse(unit, key1_index, key2_index, &key1, &key2));
            }
            else
            {
                key1 = key1_index;
                key2 = key2_index;
            }
            /*
             * Entry keys
             */
            if (table->nof_keys == 1)
            {
                PRT_CELL_SET("%d", key1);
            }
            else if (table->nof_keys == 2)
            {
                PRT_CELL_SET("%d", key1);
                PRT_CELL_SET("%d", key2);
            }
            PRT_CELL_SET("#");
            /*
             * Entry values
             */
            for (value_index = 0; value_index < UTILEX_MIN(table->nof_values, DIAG_DNXC_DATA_MAX_VALUES_TO_DISPLAY);
                 value_index++)
            {
                /*
                 * Get value as string
                 */
                table->entry_get(unit, buffer, key1, key2, value_index);
                PRT_CELL_SET("%s", buffer);
            }

        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FREE(buffer);
    SHR_FUNC_EXIT;
}

/**
 * \brief - dump data named 'data'
 *          if 'data' == '*" dump all data according to flags.
 */
static shr_error_e
diag_dnxc_submodule_data_dump(
    int unit,
    char *module,
    dnxc_data_submodule_t * submodule,
    char *data,
    uint32 flags,
    char *label,
    sh_sand_control_t * sand_control)
{
    int define_index, feature_index, table_index;

    int dump;
    shr_error_e rv;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Dump define / numeric info
     */
    PRT_TITLE_SET("DATA IN SUBMODULE %s.%s:", module, submodule->name);
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

    /*
     * Defines
     */
    for (define_index = 0; define_index < submodule->nof_defines; define_index++)
    {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strcasecmp("*", data) ||
            !sal_strncasecmp(submodule->defines[define_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
        {

            /*
             * Check if data is supported
             */
            rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->defines[define_index].flags,
                                             submodule->defines[define_index].labels, flags | DNXC_DATA_F_DEFINE, label,
                                             &dump);
            SHR_IF_ERR_EXIT(rv);

            if (dump)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", submodule->defines[define_index].name);
                PRT_CELL_SET("%s", "DEFINE");
                PRT_CELL_SET("%d", submodule->defines[define_index].data);
            }
        }
    }

    /*
     * Numerics
     */
    for (define_index = 0; define_index < submodule->nof_defines; define_index++)
    {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strcasecmp("*", data) ||
            !sal_strncasecmp(submodule->defines[define_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
        {
            /*
             * Check if data is supported
             */
            rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->defines[define_index].flags,
                                             submodule->defines[define_index].labels, flags | DNXC_DATA_F_NUMERIC,
                                             label, &dump);
            SHR_IF_ERR_EXIT(rv);

            if (dump)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", submodule->defines[define_index].name);
                PRT_CELL_SET("%s", "NUMERIC");
                PRT_CELL_SET("%d", submodule->defines[define_index].data);

            }
        }
    }

    /*
     * Features
     */
    for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
    {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strcasecmp("*", data) ||
            !sal_strncasecmp(submodule->features[feature_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
        {
            /*
             * Check if data is supported
             */
            rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->features[feature_index].flags,
                                             submodule->features[feature_index].labels, flags, label, &dump);
            SHR_IF_ERR_EXIT(rv);

            if (dump)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", submodule->features[feature_index].name);
                PRT_CELL_SET("%s", "FEATURE");
                PRT_CELL_SET("%d", submodule->features[feature_index].data);
            }
        }

    }

    PRT_COMMITX;

    /*
     * Tables
     */
    for (table_index = 0; table_index < submodule->nof_tables; table_index++)
    {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strcasecmp("*", data) ||
            !sal_strncasecmp(submodule->tables[table_index].name, data, DNXC_DATA_UTILS_STRING_MAX))
        {
            /*
             * Check if data is supported
             */
            rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, submodule->tables[table_index].flags,
                                             submodule->tables[table_index].labels, flags, label, &dump);
            SHR_IF_ERR_EXIT(rv);

            if (dump)
            {
                SHR_IF_ERR_EXIT(diag_dnxc_data_table_dump
                                (unit, module, submodule->name, &submodule->tables[table_index], flags, sand_control));
            }
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of submodules
 */
static shr_error_e
diag_dnxc_data_submodule_list(
    int unit,
    dnxc_data_module_t * module,
    sh_sand_control_t * sand_control)
{
    int submodule_index;
    dnxc_data_submodule_t *submodules = module->submodules;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("LIST OF SUBMODULES");
    PRT_COLUMN_ADD("Name");

    /*
     * Iterate over submodules
     */
    for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
    {
        /*
         * Print name
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", submodules[submodule_index].name);
    }
    PRT_COMMITX;
    cli_out("To dump all the data in a specified module, type: 'data dump <module>.*'\n");
    cli_out("To dump all the data in a specified submodule, type: 'data dump <module>.<submodule>.*'\n");

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - print info about the submodule named 'submodule' 
 *          if 'submodule' == "*" print info of all submodules according to flags. 
 */
shr_error_e
diag_dnxc_data_module_info(
    int unit,
    dnxc_data_module_t * module,
    uint32 flags,
    char *label,
    char *submodule,
    char *data,
    sh_sand_control_t * sand_control)
{
    int submodule_index, submodule_found = 0;
    dnxc_data_submodule_t *submodules = module->submodules;

    SHR_FUNC_INIT_VARS(unit);

    if (submodule == NULL)      /* list of submodules mode */
    {
        SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_list(unit, module, sand_control));
    }
    else
    {
        /*
         * Iterate over submodules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strncasecmp(submodules[submodule_index].name, submodule, DNXC_DATA_UTILS_STRING_MAX) ||
                !sal_strcasecmp("*", submodule))
            {
                submodule_found = 1;

                /** dump submodule */
                SHR_IF_ERR_EXIT(diag_dnxc_submodule_data_info
                                (unit, module->name, &submodules[submodule_index], data, flags, label, sand_control));

            }
        }

        if (submodule_found == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "submodule %s.%s not found\n", module->name, submodule);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print list of modules
 */
static shr_error_e
diag_dnxc_data_module_list(
    int unit,
    sh_sand_control_t * sand_control)
{
    int module_index;
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("LIST OF MODULES");
    PRT_COLUMN_ADD("Name");
    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        /*
         * Print name
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", modules[module_index].name);
    }
    PRT_COMMITX;
    cli_out("To dump all the data, type: 'data dump *'\n");
    cli_out("To dump all the data in a specified module, type: 'data dump <module>.*'\n");

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_info(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data,
    sh_sand_control_t * sand_control)
{
    int module_index;
    int module_found = 0;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("DNXC DATA INFO:\n");
    cli_out("==============\n");

    if (module == NULL) /* list of modules mode */
    {
        /*
         * Display module list
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_module_list(unit, sand_control));
    }
    else
    {
        /*
         * Iterate over modules
         */
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strncasecmp(modules[module_index].name, module, DNXC_DATA_UTILS_STRING_MAX) ||
                !sal_strcasecmp("*", module))
            {
                module_found = 1;

                SHR_IF_ERR_EXIT(diag_dnxc_data_module_info
                                (unit, &modules[module_index], flags, label, submodule, data, sand_control));
            }
        }
        if (module_found == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "module %s not found\n", module);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print data of submodule named 'submodule' 
 *          if 'submodule' == '*" print data of all submodules according to flags. 
 */
static shr_error_e
diag_dnxc_data_module_dump(
    int unit,
    dnxc_data_module_t * module,
    uint32 flags,
    char *label,
    char *submodule,
    char *data,
    sh_sand_control_t * sand_control)
{
    int submodule_index, submodule_found = 0;
    dnxc_data_submodule_t *submodules = module->submodules;
    SHR_FUNC_INIT_VARS(unit);

    if (submodule == NULL)      /* submodule list mode */
    {
        /*
         * Display list of submodules
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_list(unit, module, sand_control));
    }
    else
    {
        /*
         * Iterate over submodules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp("*", submodule) ||
                !sal_strncasecmp(submodules[submodule_index].name, submodule, DNXC_DATA_UTILS_STRING_MAX))
            {
                submodule_found = 1;

                if (data == NULL)
                {
                    SHR_IF_ERR_EXIT(diag_dnxc_data_list
                                    (unit, module->name, &submodules[submodule_index], flags, label, sand_control));
                }
                else
                {
                    /** dump submodule */
                    SHR_IF_ERR_EXIT(diag_dnxc_submodule_data_dump
                                    (unit, module->name, &submodules[submodule_index], data, flags, label,
                                     sand_control));
                }

            }
        }

        if (submodule_found == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "submodule %s.%s not found\n", module->name, submodule);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - see definition on local function section (top of this file)
 */
shr_error_e
diag_dnxc_data_dump(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data,
    sh_sand_control_t * sand_control)
{
    int module_index;
    int module_found = 0;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("DNXC DATA DUMP:\n");
    cli_out("==============\n");

    if (module == NULL) /* module list mode */
    {
        /*
         * Display list of modules
         */
        SHR_IF_ERR_EXIT(diag_dnxc_data_module_list(unit, sand_control));
    }
    else
    {
        /*
         * Iterate over modules
         */
        for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
        {
            /*
             * Make sure at least one of the strings is NULL terminated.
             */
            if (!sal_strcasecmp("*", module) ||
                !sal_strncasecmp(modules[module_index].name, module, DNXC_DATA_UTILS_STRING_MAX))
            {
                module_found = 1;

                SHR_IF_ERR_EXIT(diag_dnxc_data_module_dump
                                (unit, &modules[module_index], flags, label, submodule, data, sand_control));

            }
        }

        if (module_found == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "module %s not found\n", module);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_property_dump(
    int unit,
    dnxc_data_property_t * property,
    char *val_str,
    prt_control_t * prt_ctr)
{
    char buffer[DNXC_DATA_UTILS_STRING_MAX];
    SHR_FUNC_INIT_VARS(unit);

    if (property->name != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "NAME");
        PRT_CELL_SET("%s", property->name);
    }
    if (property->suffix != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "SUFFIX");
        PRT_CELL_SET("%s", property->suffix);
    }
    if (property->method_str != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "METHOD");
        PRT_CELL_SET("%s", property->method_str);
    }
    buffer[0] = 0;
    SHR_IF_ERR_EXIT(dnxc_data_utils_supported_values_get(unit, property, buffer, DNXC_DATA_UTILS_STRING_MAX, 0));
    if (buffer[0] != 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "VALUES");
        PRT_CELL_SET("%s", buffer);
    }
        /** for custom method */
    buffer[0] = 0;
    SHR_IF_ERR_EXIT(dnxc_data_utils_property_val_get(unit, property, val_str, buffer, DNXC_DATA_UTILS_STRING_MAX));
    if (buffer[0] != 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "DEFAULT");
        PRT_CELL_SET("%s", buffer);
    }

    if (property->doc != NULL)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", "DOC");
        PRT_CELL_SET("%s", property->doc);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see definition on local function section (top of this file)
 */
static shr_error_e
diag_dnxc_data_property(
    int unit,
    char *label,
    char *property,
    sh_sand_control_t * sand_control)
{
    int module_index, submodule_index, table_index, feature_index, define_index;

    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    dnxc_data_module_t *module;
    dnxc_data_submodule_t *submodule;
    dnxc_data_table_t *table;
    dnxc_data_feature_t *feature;
    dnxc_data_define_t *define;
    int dump;
    shr_error_e rv;
    int value_index;
    char property_name[SOC_PROPERTY_NAME_MAX];
    int is_suffix, len;
    char buffer[DNXC_DATA_UTILS_STRING_MAX];
    int found = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("DNXC DATA PROPERTY:\n");
    cli_out("==============\n");

    PRT_TITLE_SET("PROPERTY: %s", property);
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");
    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        module = &modules[module_index];
        /** skip module testing*/
        if (!sal_strncmp("module_testing", module->name, DNXC_DATA_UTILS_STRING_MAX))
        {
            continue;
        }

        /*
         * iterate of submodules
         */
        for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
        {
            submodule = &module->submodules[submodule_index];

            /*
             * iterate of tables
             */
            for (table_index = 0; table_index < submodule->nof_tables; table_index++)
            {
                table = &submodule->tables[table_index];
                /*
                 * Check if data is supported
                 */
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, table->flags, table->labels, 0, label,
                                                 &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * Check for per member property
                     */
                    for (value_index = 0; value_index < table->nof_values; value_index++)
                    {
                        if (table->values[value_index].property.name != NULL)
                        {
                            /*
                             * get property name
                             */
                            is_suffix = table->values[value_index].property.suffix != NULL
                                && table->values[value_index].property.suffix[0] != 0 ? 1 : 0;
                            len =
                                sal_snprintf(property_name, SOC_PROPERTY_NAME_MAX, "%s%s%s",
                                             table->values[value_index].property.name, is_suffix ? "_" : "",
                                             is_suffix ? table->values[value_index].property.suffix : "");
                            if (len >= SOC_PROPERTY_NAME_MAX)
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc property length unsupported\n");
                            }

                            /*
                             * check if substring
                             */
                            else if (strcaseindex(property_name, property) != NULL
                                     && table->values[value_index].property.doc != NULL)
                            {
                                found = 1;
                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                                PRT_CELL_SET("%s", "DATA");
                                PRT_CELL_SET("Table: %s.%s.%s (member %s)", module->name, submodule->name, table->name,
                                             table->values[value_index].name);

                                SHR_IF_ERR_EXIT(diag_dnxc_data_property_dump
                                                (unit, &table->values[value_index].property,
                                                 table->values[value_index].default_val, prt_ctr));
                            }
                        }
                    }
                    /*
                     * check for table global property
                     */
                    if (table->property.name != NULL)
                    {

                        /*
                         * get property name
                         */
                        is_suffix = table->property.suffix != NULL && table->property.suffix[0] != 0 ? 1 : 0;
                        len = sal_snprintf(property_name, SOC_PROPERTY_NAME_MAX,
                                           "%s%s%s", table->property.name, is_suffix ? "_" : "",
                                           is_suffix ? table->property.suffix : "");
                        if (len >= SOC_PROPERTY_NAME_MAX)
                        {
                            cli_out("soc property length unsupported in table %s.%s.%s\n", module->name,
                                    submodule->name, table->name);
                        }

                        /*
                         * check if substring
                         */
                        else if (strcaseindex(property_name, property) != NULL && table->property.doc != NULL)
                        {
                            found = 1;
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                            PRT_CELL_SET("%s", "DATA");
                            PRT_CELL_SET("Table: %s.%s.%s", module->name, submodule->name, table->name);

                            SHR_IF_ERR_EXIT(diag_dnxc_data_property_dump
                                            (unit, &table->property, table->values[0].default_val, prt_ctr));
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
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, feature->flags, feature->labels, 0,
                                                 label, &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * check if data loaded be SoC property
                     */
                    if (feature->property.name != NULL)
                    {
                        /*
                         * get property name 
                         */
                        is_suffix = feature->property.suffix != NULL && feature->property.suffix[0] != 0 ? 1 : 0;
                        len = sal_snprintf(property_name, SOC_PROPERTY_NAME_MAX,
                                           "%s%s%s", feature->property.name, is_suffix ? "_" : "",
                                           is_suffix ? feature->property.suffix : "");
                        if (len >= SOC_PROPERTY_NAME_MAX)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc property length unsupported\n");
                        }

                        /*
                         * check if substring
                         */
                        else if (strcaseindex(property_name, property) != NULL && feature->property.doc != NULL)
                        {
                            found = 1;
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                            PRT_CELL_SET("%s", "DATA");
                            PRT_CELL_SET("Feature: %s.%s.%s", module->name, submodule->name, feature->name);

                            sal_snprintf(buffer, DNXC_DATA_UTILS_STRING_MAX, "%d", feature->default_data);
                            SHR_IF_ERR_EXIT(diag_dnxc_data_property_dump(unit, &feature->property, buffer, prt_ctr));
                        }
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
                rv = dnxc_data_utils_dump_verify(unit, _dnxc_data[unit].state, define->flags, define->labels, 0, label,
                                                 &dump);
                SHR_IF_ERR_EXIT(rv);

                if (dump)
                {
                    /*
                     * check if data loaded be SoC property
                     */
                    if (define->property.name != NULL)
                    {
                        /*
                         * get property name 
                         */
                        is_suffix = define->property.suffix != NULL && define->property.suffix[0] != 0 ? 1 : 0;
                        len = sal_snprintf(property_name, SOC_PROPERTY_NAME_MAX,
                                           "%s%s%s", define->property.name, is_suffix ? "_" : "",
                                           is_suffix ? define->property.suffix : "");
                        if (len >= SOC_PROPERTY_NAME_MAX)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc property length unsupported\n");
                        }

                        /*
                         * check if substring
                         */
                        else if (strcaseindex(property_name, property) != NULL && define->property.doc != NULL)
                        {
                            found = 1;
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                            PRT_CELL_SET("%s", "DATA");
                            PRT_CELL_SET("NUMERIC: %s.%s.%s", module->name, submodule->name, define->name);

                            sal_snprintf(buffer, DNXC_DATA_UTILS_STRING_MAX, "%d", define->default_data);
                            SHR_IF_ERR_EXIT(diag_dnxc_data_property_dump(unit, &define->property, buffer, prt_ctr));
                        }
                    }
                }       /* Dump numeric */
            }   /* numeric iterator */

        }       /* submodule iterator */
    }   /* module iterator */

    if (found == 0)
    {
        cli_out("WARNING: soc property %s not found\n", property);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Export submodule's features to xml
 */
static shr_error_e
diag_dnxc_data_submodule_features_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (submodule->nof_features > 0)
    {
        dnxc_data_feature_t *features = submodule->features;
        int feature_index, feature_col_id;
        uint8 table_has_content = FALSE;

        PRT_TITLE_SET("%s.%s.features", module->name, submodule->name);

        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &feature_col_id, "Feature");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, feature_col_id, NULL, "State");

        for (feature_index = 0; feature_index < submodule->nof_features; feature_index++)
        {
            int do_export = 0;

            /*
             * Check if data is supported / match flags
             */
            SHR_IF_ERR_EXIT(dnxc_data_utils_dump_verify
                            (unit, _dnxc_data[unit].state, features[feature_index].flags,
                             features[feature_index].labels, flags, NULL, &do_export));
            if (do_export)
            {
                const char *feature_state = (features[feature_index].data == 0 ? "disabled" : "enabled");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", features[feature_index].name);
                PRT_CELL_SET("%s", feature_state);
                table_has_content = TRUE;
            }
        }

        if (TRUE == table_has_content)
        {
            PRT_COMMITX;
        }

    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   export submodule's defines to xml
 */
static shr_error_e
diag_dnxc_data_submodule_defines_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (submodule->nof_defines > 0)
    {
        dnxc_data_define_t *defines = submodule->defines;
        int define_index, define_col_id;
        uint8 table_has_content = FALSE;

        PRT_TITLE_SET("%s.%s.defines", module->name, submodule->name);

        PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &define_col_id, "Define");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, define_col_id, NULL, "Value");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, define_col_id, NULL, "Type");

        for (define_index = 0; define_index < submodule->nof_defines; define_index++)
        {
            int do_export = 0;

            /*
             * Check if data is supported / match flags
             */
            SHR_IF_ERR_EXIT(dnxc_data_utils_dump_verify
                            (unit, _dnxc_data[unit].state, defines[define_index].flags, defines[define_index].labels,
                             flags, NULL, &do_export));
            if (do_export)
            {
                const char *define_type = (defines[define_index].flags & DNXC_DATA_F_NUMERIC) ? "Numeric" : "Define";
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", defines[define_index].name);
                PRT_CELL_SET("%d", defines[define_index].data);
                PRT_CELL_SET("%s", define_type);
                table_has_content = TRUE;
            }
        }

        if (TRUE == table_has_content)
        {
            PRT_COMMITX;
        }

    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Export submodule's specific table to xml
 */
static shr_error_e
diag_dnxc_data_table_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    dnxc_data_table_t * table,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    /*
     * Table column ID holders
     */
    int entry_col_id, key_col_id, result_col_id;
    /*
     * 'for' counters
     */
    int key1_index, key2_index, entry_row, entry_counter = 0;
    /*
     * If each entry in the table requires more than a single line, use a separator to distinguish between them
     * (print only).
     */
    int entry_row_separator = ((table->nof_keys > 1) || (table->nof_values > 1))
        ? PRT_ROW_SEP_UNDERSCORE_BEFORE : PRT_ROW_SEP_NONE;
    /*
     * Indicates if there is actual content in the table to export.
     * Used to prevent exporting empty tables or, if the 'changed' flag is used,
     * not to export tables with no changed entries.
     */
    uint8 table_has_content = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * define title and headline for table
     */
    PRT_TITLE_SET("%s.%s.tables::%s", module->name, submodule->name, table->name);
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &entry_col_id, "entry");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &key_col_id, "key_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, key_col_id, NULL, "key_value");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, entry_col_id, &result_col_id, "result_field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, result_col_id, NULL, "result_value");

    /*
     * Iterate entries and print keys and values
     */
    for (key1_index = 0; (key1_index < table->keys[0].size) || (key1_index == 0); key1_index++)
    {
        for (key2_index = 0; (key2_index < table->keys[1].size) || (key2_index == 0); key2_index++)
        {
            /*
             * Filter changed entries according to dump flags
             */
            int changed = 1;
            if (flags & DNXC_DATA_F_CHANGED)
            {
                SHR_IF_ERR_EXIT(dnxc_data_mgmt_table_entry_changed_get(unit, table, key1_index, key2_index, &changed));
            }
            if (changed)
            {
                /*
                 *  Entry changed - export it.
                 */
                PRT_ROW_ADD(entry_row_separator);
                /*
                 * entry_counter will give a root node to the entry nodes.
                 */
                PRT_CELL_SET("%d", entry_counter);
                /*
                 * Entry keys
                 */
                for (entry_row = 0; (entry_row < table->nof_keys) || (entry_row < table->nof_values); entry_row++)
                {
                    if (entry_row > 0)
                    {
                        /*
                         * 2nd and above lines of the same entry - create a line and skip the entry cell
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(1);
                    }
                    if (entry_row < table->nof_keys)
                    {
                        /*
                         * Key field
                         */
                        PRT_CELL_SET("%s", table->keys[entry_row].name);
                        PRT_CELL_SET("%d", entry_row == 0 ? key1_index : key2_index);
                    }
                    else
                    {
                        PRT_CELL_SKIP(2);
                    }
                    if (entry_row < table->nof_values)
                    {
                        char buffer[DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH];
                        /*
                         * Value field
                         */
                        PRT_CELL_SET("%s", table->values[entry_row].name);
                        /*
                         * Get value as string
                         */
                        SHR_IF_ERR_EXIT(table->entry_get(unit, buffer, key1_index, key2_index, entry_row));
                        PRT_CELL_SET("%s", buffer);
                    }
                    else
                    {
                        PRT_CELL_SKIP(2);
                    }
                }
                /*
                 * Mark that there is a reason to export the table (at least one entry)
                 */
                table_has_content = TRUE;
            }
            /*
             * count even if not exporting
             */
            entry_counter++;
        }
    }
    if (TRUE == table_has_content)
    {
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Iterate over a submodule's tables list and export the valid ones to xml.
 */
static shr_error_e
diag_dnxc_data_submodule_tables_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    int table_index;

    SHR_FUNC_INIT_VARS(unit);

    for (table_index = 0; table_index < submodule->nof_tables; table_index++)
    {
        int do_export;
        dnxc_data_table_t *table = &submodule->tables[table_index];

        /*
         * Check if data is supported / match flags
         */
        SHR_IF_ERR_EXIT(dnxc_data_utils_dump_verify
                        (unit, _dnxc_data[unit].state, table->flags, table->labels, flags, NULL, &do_export));
        if (do_export)
        {
            /*
             * Supported and requires exporting
             */
            SHR_IF_ERR_EXIT(diag_dnxc_data_table_export(unit, module, submodule, table, flags, sand_control));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Export submodule data to xml
 */
static shr_error_e
diag_dnxc_data_submodule_export(
    int unit,
    dnxc_data_module_t * module,
    dnxc_data_submodule_t * submodule,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Defines/Numerics
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_defines_export(unit, module, submodule, flags, sand_control));
    /*
     * Features
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_features_export(unit, module, submodule, flags, sand_control));
    /*
     * Tables
     */
    SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_tables_export(unit, module, submodule, flags, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  data exprot command implementation
 * \see
 *  Man for 'data export'
 */
static shr_error_e
cmd_dnxc_data_export(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_index, submodule_index;
    dnxc_data_module_t *modules = _dnxc_data[unit].modules;
    uint32 dump_flags = 0;
    int changed_only = TRUE, property_only = TRUE;
    char *module_name;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Parse options and set flags accordingly
     */
    SH_SAND_GET_BOOL("property", property_only);
    if (property_only)
    {
        dump_flags |= DNXC_DATA_F_PROPERTY;
    }

    SH_SAND_GET_BOOL("changed", changed_only);
    if (changed_only)
    {
        dump_flags |= DNXC_DATA_F_CHANGED;
    }

    SH_SAND_GET_STR("module", module_name);

    /*
     * Iterate over modules
     */
    for (module_index = 0; module_index < _dnxc_data[unit].nof_modules; module_index++)
    {
        dnxc_data_module_t *module = &modules[module_index];

        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strncasecmp(module->name, module_name, DNXC_DATA_UTILS_STRING_MAX) ||
            !sal_strcasecmp("*", module_name))
        {
            /*
             * iterate over sub modules
             */
            for (submodule_index = 0; submodule_index < module->nof_submodules; submodule_index++)
            {
                dnxc_data_submodule_t *submodule = &module->submodules[submodule_index];
                /*
                 * Export submodule data
                 */
                SHR_IF_ERR_EXIT(diag_dnxc_data_submodule_export(unit, module, submodule, dump_flags, sand_control));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */
/**
 * \brief
 *   Vectors for DNX-Data export command
 */
static sh_sand_option_t dnxc_data_export_options[] = {
    {"changed", SAL_FIELD_TYPE_BOOL, "Export only changed data", "yes"},
    {"property",SAL_FIELD_TYPE_BOOL, "Export only properties", "yes"},
    {"module",SAL_FIELD_TYPE_STR, "Export only the specified module", "*"},
    {"silence", SAL_FIELD_TYPE_BOOL,  NULL,                            "true"},
    {NULL}
};
static sh_sand_man_t dnxc_data_export_man = {
    .brief    = "Export changed data to an xml file (can also print all changed data to the console)",
    .full     = "Prints all the changed data in a format that is friendly to exporting to xml files. To perform the actual export,"
                "the 'file' option should be used with a .xml suffix to the file name.",
    .synopsis = "[file=<filename.xml>]",
    .examples = "changed \n"
                "property=false module=fabric\n"
                "silence \n"

};
/* *INDENT-ON* */

/**
 * \brief DNXC DATA diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_data_cmds[] = {
    /*keyword,   action,                   cmd,  options,                    man                      N/A   N/A   Flags*/
    {"dump",     cmd_dnxc_data_dump,       NULL, dnxc_data_dump_options,     &dnxc_data_dump_man,     NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"export",   cmd_dnxc_data_export,     NULL, dnxc_data_export_options,   &dnxc_data_export_man,   NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"info",     cmd_dnxc_data_info,       NULL, dnxc_data_info_options,     &dnxc_data_info_man,     NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"property_manual", cmd_dnxc_data_man, NULL, dnxc_data_man_options,      &dnxc_data_man_man,      NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"property", cmd_dnxc_data_property,   NULL, dnxc_data_property_options, &dnx_data_property_man,  NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnx},
    {"property", cmd_dnxc_data_property,   NULL, dnxc_data_property_options, &dnxf_data_property_man, NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnxf},
    {NULL}
};

sh_sand_man_t sh_dnxc_data_man = {
    .brief    = "Misc facilities for displaying dnxc data information",
};
/* *INDENT-ON* */

/*
 * }
 */
