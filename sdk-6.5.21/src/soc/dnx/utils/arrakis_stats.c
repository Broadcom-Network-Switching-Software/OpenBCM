/*
 * $Id: arrakis_stats.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * Description: See arrakis_stats.h
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <soc/dnx/utils/arrakis_stats.h>
#include <soc/dnx/utils/arrakis_stats_internal.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/core/libc.h>
#include <soc/dnxc/dnxc_regression_utils.h>

#define STATS_SYNTAX_XML_PATH                   "../../dnx/arrakis_stats/stats_syntax.xml"
#define STATS_SYNTAX_XML_TOP_ELEMENT_NAME       "stat-families"

#define STATS_DECIMAL_PRECISION                 3

#define STATS_DATE                          "Date"

typedef struct
{
    char dimensions[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN];
    char dimension_parsing_name[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN];
    int dimensions_nof;
} stat_family_xml_t;

shr_error_e
arrakis_stat_family_init(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *stat_family_name)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_family, _SHR_E_PARAM, "stat_family pointer is NULL");
    SHR_NULL_CHECK(stat_family_name, _SHR_E_PARAM, "stat_family_name pointer is NULL");
    if (sal_strlen(stat_family_name) > ARRAKIS_STATS_FAMILY_NAME_MAX_LEN - 1)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "stat_family_name length exceeds ARRAKIS_STATS_FAMILY_NAME_MAX_LEN");
    }

    sal_strncpy(stat_family->name, stat_family_name, ARRAKIS_STATS_FAMILY_NAME_MAX_LEN - 1);

    stat_family->dimensions_nof = 0;

    for (i = 0; i < ARRAKIS_STATS_DIMENSIONS_MAX_NOF; i++)
    {
        stat_family->samples[i][0] = '\0';
        stat_family->samples_nof[i] = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_fill_arguments_check(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_family, _SHR_E_PARAM, "stat_family pointer is NULL");
    SHR_NULL_CHECK(dimension, _SHR_E_PARAM, "dimension pointer is NULL");
    if (sal_strlen(dimension) > ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN - 1)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "dimension exceeds ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_multiple_fill_arguments_check(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    void *samples)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_family, _SHR_E_PARAM, "stat_family pointer is NULL");
    SHR_NULL_CHECK(dimensions, _SHR_E_PARAM, "dimensions pointer is NULL");
    SHR_NULL_CHECK(samples, _SHR_E_PARAM, "samples pointer is NULL");

    for (i = 0; i < dimensions_nof; i++)
    {
        SHR_NULL_CHECK(dimensions[i], _SHR_E_PARAM, "A dimension pointer in dimensions pointer is NULL");
        if (sal_strlen(dimensions[i]) > ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "dimension exceeds ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_stat_family_sample_add(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    char *sample)
{
    int dimension_idx = -1;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < stat_family->dimensions_nof; i++)
    {
        if (sal_strcmp(dimension, stat_family->dimensions[i]) == 0)
        {
            dimension_idx = i;
            break;
        }
    }

    if (dimension_idx == -1)
    {
        if (stat_family->dimensions_nof == ARRAKIS_STATS_DIMENSIONS_MAX_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Number of dimensions exceeds ARRAKIS_STATS_DIMENSIONS_MAX_NOF");
        }
        dimension_idx = stat_family->dimensions_nof;
        sal_strncpy(stat_family->dimensions[dimension_idx], dimension, ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN - 1);
        stat_family->dimensions_nof++;
    }

    if (sal_strlen(stat_family->samples[dimension_idx]) + sal_strlen(sample) > ARRAKIS_STATS_SAMPLES_STR_MAX_LEN - 1)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Samples string length exceeds ARRAKIS_STATS_SAMPLES_STR_MAX_LEN");
    }
    sal_strncat(stat_family->samples[dimension_idx], sample,
                ARRAKIS_STATS_SAMPLES_STR_MAX_LEN - 1 - sal_strlen(stat_family->samples[dimension_idx]));
    stat_family->samples_nof[dimension_idx]++;

exit:
    SHR_FUNC_EXIT;
}

static void
stats_trailing_zeroes_remove(
    char *num_str)
{
    int i;

    for (i = sal_strlen(num_str) - 1; i > 0; i--)
    {
        if (num_str[i] == '0')
        {
            num_str[i] = '\0';
        }
        else
        {
            break;
        }
    }
}

shr_error_e
arrakis_stat_family_float_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    float sample)
{
    char sample_str[STATS_SAMPLE_STR_MAX_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_fill_arguments_check(unit, stat_family, dimension));

    sal_snprintf(sample_str, STATS_SAMPLE_STR_MAX_LEN, "%s%.3f", STATS_DELIMITER, sample);
    stats_trailing_zeroes_remove(sample_str);
    SHR_IF_ERR_EXIT(stats_stat_family_sample_add(unit, stat_family, dimension, sample_str));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
arrakis_stat_family_int_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    int sample)
{
    char sample_str[STATS_SAMPLE_STR_MAX_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_fill_arguments_check(unit, stat_family, dimension));

    sal_snprintf(sample_str, STATS_SAMPLE_STR_MAX_LEN, "%s%d", STATS_DELIMITER, sample);
    SHR_IF_ERR_EXIT(stats_stat_family_sample_add(unit, stat_family, dimension, sample_str));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
arrakis_stat_family_uint32_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    uint32 sample)
{
    char sample_str[STATS_SAMPLE_STR_MAX_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_fill_arguments_check(unit, stat_family, dimension));

    sal_snprintf(sample_str, STATS_SAMPLE_STR_MAX_LEN, "%s%u", STATS_DELIMITER, sample);
    SHR_IF_ERR_EXIT(stats_stat_family_sample_add(unit, stat_family, dimension, sample_str));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
arrakis_stat_family_float_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    float *samples)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_multiple_fill_arguments_check(unit, stat_family, dimensions, dimensions_nof, samples));

    for (i = 0; i < dimensions_nof; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_float_fill(unit, stat_family, dimensions[i], samples[i]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
arrakis_stat_family_int_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    int *samples)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_multiple_fill_arguments_check(unit, stat_family, dimensions, dimensions_nof, samples));

    for (i = 0; i < dimensions_nof; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_int_fill(unit, stat_family, dimensions[i], samples[i]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
arrakis_stat_family_uint32_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    uint32 *samples)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_multiple_fill_arguments_check(unit, stat_family, dimensions, dimensions_nof, samples));

    for (i = 0; i < dimensions_nof; i++)
    {
        SHR_IF_ERR_EXIT(arrakis_stat_family_uint32_fill(unit, stat_family, dimensions[i], samples[i]));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_stats_syntax_xml_parse(
    int unit,
    char *stat_family_name,
    stat_family_xml_t * stat_family_xml)
{
    char stat_family_name_itr[ARRAKIS_STATS_FAMILY_NAME_MAX_LEN];
    char dimension[ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN];
    char dimension_parsing_name[ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN];
    xml_node *stats_syntax_root;
    xml_node *stat_family_node;
    xml_node *elements_node;
    xml_node *element_node;

    SHR_FUNC_INIT_VARS(unit);

    stat_family_xml->dimensions_nof = 0;

    stats_syntax_root = dbx_file_get_xml_top(unit, STATS_SYNTAX_XML_PATH, STATS_SYNTAX_XML_TOP_ELEMENT_NAME, 0);
    if (stats_syntax_root == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbx_file_get_xml_top() failed getting stats_syntax.xml");
    }

    RHDATA_ITERATOR(stat_family_node, stats_syntax_root, "stat-family")
    {
        RHDATA_GET_STR_DEF_NULL(stat_family_node, "Name", stat_family_name_itr);
        if (sal_strcmp(stat_family_name, stat_family_name_itr) == 0)
        {
            elements_node = dbx_xml_child_get_first(stat_family_node, "elements");
            if (elements_node == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stat Family in stats_syntax.xml missing element tags");
            }
            RHDATA_ITERATOR(element_node, elements_node, "element")
            {
                RHDATA_GET_STR_DEF_NULL(element_node, "Name", dimension);
                RHDATA_GET_STR_DEF_NULL(element_node, "Parsing_Name", dimension_parsing_name);
                sal_strncpy(stat_family_xml->dimensions[stat_family_xml->dimensions_nof], dimension,
                            ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN - 1);
                sal_strncpy(stat_family_xml->dimension_parsing_name[stat_family_xml->dimensions_nof],
                            dimension_parsing_name, ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN - 1);
                stat_family_xml->dimensions_nof++;
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "stat_family_name do not exists in stats_syntax.xml");

exit:
    SHR_FUNC_EXIT;
}

static int
stats_is_stat_family_with_date_dimension(
    int unit,
    stat_family_xml_t * stat_family_xml)
{
    int idx;

    for (idx = 0; idx < stat_family_xml->dimensions_nof; idx++)
    {
        if (sal_strcmp(stat_family_xml->dimensions[idx], STATS_DATE) == 0)
        {
            return 1;
        }
    }

    return 0;
}

static shr_error_e
stats_nof_dimensions_validate(
    int unit,
    arrakis_stat_family_t * stat_family,
    stat_family_xml_t * stat_family_xml)
{
    int expected_nof_dimension = stat_family_xml->dimensions_nof;

    SHR_FUNC_INIT_VARS(unit);

    if (stats_is_stat_family_with_date_dimension(unit, stat_family_xml))
    {
        expected_nof_dimension -= 1;
    }

    if (stat_family->dimensions_nof != expected_nof_dimension)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of dimensions doesn't match stat_family definition in stats_syntax.xml");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_nof_samples_validate(
    int unit,
    arrakis_stat_family_t * stat_family,
    stat_family_xml_t * stat_family_xml)
{
    int is_stat_family_with_date_dimension = stats_is_stat_family_with_date_dimension(unit, stat_family_xml);
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (stat_family->dimensions_nof == 0)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "stat_family has no data to commit");
    }

    if (is_stat_family_with_date_dimension && stat_family->samples_nof[0] != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stat Family with Date dimension should have only one sample per dimension");
    }

    for (i = 1; i < stat_family->dimensions_nof; i++)
    {
        if (is_stat_family_with_date_dimension && stat_family->samples_nof[i] != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stat Family with Date dimension should have only one sample per dimension");
        }

        if (stat_family->samples_nof[i] != stat_family->samples_nof[i - 1])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Dimensions do not have the same number of samples");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_dimensions_parsing_name_fill_and_validate_dimension_names(
    int unit,
    arrakis_stat_family_t * stat_family,
    stat_family_xml_t * stat_family_xml)
{
    int xml_idx;
    int struct_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (struct_idx = 0; struct_idx < stat_family->dimensions_nof; struct_idx++)
    {
        for (xml_idx = 0; xml_idx < stat_family_xml->dimensions_nof; xml_idx++)
        {
            if (sal_strcmp(stat_family->dimensions[struct_idx], stat_family_xml->dimensions[xml_idx]) == 0)
            {
                sal_strncpy(stat_family->dimensions_parsing_name[struct_idx],
                            stat_family_xml->dimension_parsing_name[xml_idx],
                            ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN - 1);
                break;
            }
        }

        if (xml_idx == stat_family_xml->dimensions_nof)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Dimensions specified do not match Stat Family description in stats_syntax.xml");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
stats_stat_family_validate_and_fill_dimensions_parsing_name(
    int unit,
    arrakis_stat_family_t * stat_family,
    stat_family_xml_t * stat_family_xml)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(stats_nof_samples_validate(unit, stat_family, stat_family_xml));

    SHR_IF_ERR_EXIT(stats_nof_dimensions_validate(unit, stat_family, stat_family_xml));

    SHR_IF_ERR_EXIT(stats_dimensions_parsing_name_fill_and_validate_dimension_names
                    (unit, stat_family, stat_family_xml));

exit:
    SHR_FUNC_EXIT;
}

static void
stats_data_commit(
    int unit,
    arrakis_stat_family_t * stat_family)
{
    char dimension_str[ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN];
    int dim_idx;

    for (dim_idx = 0; dim_idx < stat_family->dimensions_nof; dim_idx++)
    {
        if (stat_family->dimensions_parsing_name[dim_idx][0] != '\0')
        {
            sal_strncpy(dimension_str, stat_family->dimensions_parsing_name[dim_idx],
                        ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN - 1);
        }
        else
        {
            sal_strncpy(dimension_str, stat_family->dimensions[dim_idx],
                        ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN - 1);
        }

        sal_strncat(dimension_str, stat_family->samples[dim_idx],
                    ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN - 1 - sal_strlen(dimension_str));

        dnxc_regression_utils_print(stat_family->name, dimension_str);
    }
}

shr_error_e
arrakis_stat_family_commit(
    int unit,
    arrakis_stat_family_t * stat_family)
{
    stat_family_xml_t stat_family_xml;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_family, _SHR_E_PARAM, "stat_family pointer is NULL");

    SHR_IF_ERR_EXIT(stats_stats_syntax_xml_parse(unit, stat_family->name, &stat_family_xml));

    SHR_IF_ERR_EXIT(stats_stat_family_validate_and_fill_dimensions_parsing_name(unit, stat_family, &stat_family_xml));

    stats_data_commit(unit, stat_family);

exit:
    SHR_FUNC_EXIT;
}
