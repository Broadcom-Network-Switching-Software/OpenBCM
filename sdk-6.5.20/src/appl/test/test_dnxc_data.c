/** \file diag_dnxc_data_c
 * 
 * DEVICE DATA TEST - Testing DNX DATA module 
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
#include <appl/diag/test.h>
#include <appl/diag/diag.h>
/*soc*/
#include <soc/drv.h>
/*sal*/
#include <sal/appl/sal.h>

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)

#include <appl/diag/sand/diag_sand_framework.h>

#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#endif

#if defined(BCM_DNXF_SUPPORT)
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#endif

/*
 * Extern dump command to test
 */
extern shr_error_e diag_dnxc_data_dump(
    int unit,
    uint32 flags,
    char *label,
    char *module,
    char *submodule,
    char *data,
	sh_sand_control_t * sand_control);

#ifdef BCM_DNX_SUPPORT
/**
 * \brief - Test Supported/Unsupported Data types 
 * used to make sure that supporrted data will return the expected values    
 */
int
test_dnx_data_unsupported(
    int unit)
{
    int feature_val;
    uint32 define_val;
    const uint32 *generic_val;
    const dnx_data_module_testing_unsupported_supported_table_t *sup_table_val;
    int res = 0;                /* test result - assume pass */

    /*
     * features
     */
    feature_val = dnx_data_module_testing.unsupported.feature_get
        (unit, dnx_data_module_testing_unsupported_supported_feature);
    if (feature_val != 1)
    {

        /*
         * required data wasn't found throw print an error
         */
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that supported feature will return 1\n")));
        res = 1;
    }

    feature_val = dnx_data_module_testing.unsupported.feature_get
        (unit, dnx_data_module_testing_unsupported_unsupported_feature);
    if (feature_val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that unsupported feature will return 0\n")));
        res = 1;
    }

    /*
     * defines
     */
    define_val = dnx_data_module_testing.unsupported.supported_def_get(unit);
    if (define_val != 0x12345678)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported define will return correct value, got %x\n"),
                   define_val));
        res = 1;
    }

    /*
     * tables
     */
    sup_table_val = dnx_data_module_testing.unsupported.supported_table_get(unit);
    {
        if (sup_table_val == NULL)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - expected that supported table will return correct value\n")));
            res = 1;
        }
        else if (sup_table_val->num != 0x87654321)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - expected that supported table will return correct value, got %x\n"),
                       sup_table_val->num));
            res = 1;
        }
    }
    /*
     * Test generic Access API
     */
    /*
     * features
     */
    generic_val = dnxc_data_utils_generic_data_get(unit, "module_testing", "unsupported", "supported_feature", NULL);
    if (generic_val == NULL || *generic_val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported feature will return 1 (generic API)\n")));
        res = 1;
    }
    generic_val = dnxc_data_utils_generic_data_get(unit, "module_testing", "unsupported", "unsupported_feature", NULL);
    if (generic_val == NULL || *generic_val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that unsupported feature will return 0 (generic API)\n")));
        res = 1;
    }

    return res;
}
/**
 * brief - Test different data types (of memebers in tables)
 */
static int
test_dnx_data_types(
    int unit)
{
    const dnx_data_module_testing_types_all_t *types_table_val;
    int res = 0;                /* test result - assume pass */

    /*
     * Get table struct
     */
    types_table_val = dnx_data_module_testing.types.all_get(unit);
    if (types_table_val == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that supported table will not return NULL\n")));
        res = 1;
    }
    else
    {
        /*
         * check values
         */
        if (types_table_val->intval != -1)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting intval=-1, got %d\n"), types_table_val->intval));
            res = 1;
        }
        if (types_table_val->uint8val != 0x12)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint8val=-1, got %d\n"), types_table_val->uint8val));
            res = 1;
        }
        if (types_table_val->charval != '1')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting charval='c', got %d\n"), types_table_val->charval));
            res = 1;
        }
        if (types_table_val->uint16val != 0x1234)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint16val=0x1234, got %x\n"), types_table_val->uint16val));
            res = 1;
        }
        if (types_table_val->enumval != bcmFabricDeviceTypeFE2)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint16val=bcmFabricDeviceTypeFE2, got %d\n"),
                       types_table_val->enumval));
            res = 1;
        }
        if (types_table_val->arrval[0] != 1 ||
            types_table_val->arrval[1] != 2 || types_table_val->arrval[2] != 3 || types_table_val->arrval[3] != 4)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting arrval={1, 2, 3, 4, 5, 6, 7, 8, 9, 10}\n")));
            res = 1;
        }
        if (!BCM_PBMP_MEMBER(types_table_val->pbmpval, 33))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting port 33 to be in pbmpval\n")));
            res = 1;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
         if (sal_strcasecmp(types_table_val->strval, "dnx data"))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting strval=\"dnx data\", got %s\n"), types_table_val->strval));
            res = 1;
        }
        if (types_table_val->bufferval[1] != 'b')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting bufferval[1]='b', got %d\n"),
                       types_table_val->bufferval[1]));
            res = 1;
        }
    }

    return res;
}

/**
 * \brief - Test different property methods
 */
static int
test_dnx_data_property_methods(
    int unit)
{
    int res = 0;                /* test result - assume pass */
    cmd_result_t rv;
    shr_error_e re;
    uint32 numeric;
    int feature;
    const dnx_data_module_testing_property_methods_enable_t *enable;
    const dnx_data_module_testing_property_methods_port_enable_t *port_enable;
    const dnx_data_module_testing_property_methods_suffix_enable_t *suffix_enable;
    const dnx_data_module_testing_property_methods_range_t *range;
    const dnx_data_module_testing_property_methods_port_range_t *port_range;
    const dnx_data_module_testing_property_methods_suffix_range_t *suffix_range;
    const dnx_data_module_testing_property_methods_direct_map_t *direct_map;
    const dnx_data_module_testing_property_methods_port_direct_map_t *port_direct_map;
    const dnx_data_module_testing_property_methods_suffix_direct_map_t *suffix_direct_map;
    const dnx_data_module_testing_property_methods_custom_t *custom;
    const dnx_data_module_testing_property_methods_pbmp_t *pbmp;
    const dnx_data_module_testing_property_methods_port_pbmp_t *port_pbmp;
    const dnx_data_module_testing_property_methods_suffix_pbmp_t *suffix_pbmp;
    const dnxc_data_table_info_t *table_info;
    /*
     * Add relevant SoC properties
     */
    rv = sh_process_command(unit, "config add dnx_data_feature_enable=1");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_numeric_range=2");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_enable=1");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_port_enable_0=0");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_suffix_enable_link4=0");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_range=5");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_port_range_1=3");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_suffix_range_link2=4");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_direct_map=HIGH");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_port_direct_map_0=LOW");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_suffix_direct_map_link3=NORMAL");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_custom_link2=1");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    /*
     * pbmp
     */
    rv = sh_process_command(unit, "config add dnx_data_pbmp=0x8000");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_port_pbmp_2=0x80000");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnx_data_suffix_pbmp_index3=0x200000");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }

    /*
     * Run Deinit-Init DNX DATA - the SoC properties loaded upon init
     */
    re = sh_process_command(unit, "tr 141");
    if (re != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnx data deinit.\n")));
        return 1;       /* fail */
    }

    /*
     * Get and Compare
     */
    /*
     * feature enable
     */
    feature =
        dnx_data_module_testing.property_methods.feature_get(unit,
                                                             dnx_data_module_testing_property_methods_feature_enable);
    if (feature != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting feature enable to be 1, got %d\n"), feature));
        res = 1;
    }

    /*
     * feature disable
     */
    feature =
        dnx_data_module_testing.property_methods.feature_get(unit,
                                                             dnx_data_module_testing_property_methods_feature_disable);
    if (feature != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting feature disable to be 0, got %d\n"), feature));
        res = 1;
    }

    /*
     * numeric range 
     */
    numeric = dnx_data_module_testing.property_methods.numeric_range_get(unit);
    if (numeric != 2)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting numeric range to be 2, got %d\n"), numeric));
        res = 1;
    }

    /*
     * tables
     */
    /*
     * enable
     */
    enable = dnx_data_module_testing.property_methods.enable_get(unit);
    if (enable->val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting enable to be 1, got %d\n"), enable->val));
        res = 1;
    }

    port_enable = dnx_data_module_testing.property_methods.port_enable_get(unit, 0);
    if (port_enable->val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting  port enable to be 0, got %d\n"), port_enable->val));
        res = 1;
    }

    suffix_enable = dnx_data_module_testing.property_methods.suffix_enable_get(unit, 4);
    if (suffix_enable->val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting suffix enable to be 0, got %d\n"), suffix_enable->val));
        res = 1;
    }

    /*
     * range
     */
    range = dnx_data_module_testing.property_methods.range_get(unit);
    if (range->val != 5)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting range to be 5, got %d\n"), range->val));
        res = 1;
    }

    port_range = dnx_data_module_testing.property_methods.port_range_get(unit, 1);
    if (port_range->val != 3)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting port range to be 3, got %d\n"), port_range->val));
        res = 1;
    }

    suffix_range = dnx_data_module_testing.property_methods.suffix_range_get(unit, 2);
    if (suffix_range->val != 4)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting suffix range to be 4, got %d\n"), suffix_range->val));
        res = 1;
    }

    /*
     * direct map
     */
    direct_map = dnx_data_module_testing.property_methods.direct_map_get(unit);
    if (direct_map->val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting direct_map to be 1, got %d\n"), direct_map->val));
        res = 1;
    }

    port_direct_map = dnx_data_module_testing.property_methods.port_direct_map_get(unit, 0);
    if (port_direct_map->val != 2)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting port direct_map to be 2, got %d\n"), port_direct_map->val));
        res = 1;
    }

    suffix_direct_map = dnx_data_module_testing.property_methods.suffix_direct_map_get(unit, 3);
    if (suffix_direct_map->val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting suffix direct_map to be 0, got %d\n"), suffix_direct_map->val));
        res = 1;
    }

    /*
     * custom
     */
    custom = dnx_data_module_testing.property_methods.custom_get(unit, 2, 1);
    if (custom->val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting custom to be 1, got %d\n"), custom->val));
        res = 1;
    }

    /*
     * check key sizes
     */
    table_info = dnx_data_module_testing.property_methods.custom_info_get(unit);
    if (table_info->key_size[0] != 5 || table_info->key_size[1] != 2)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting different custom key size\n")));
        res = 1;
    }

    /*
     * pbmp
     */
    pbmp = dnx_data_module_testing.property_methods.pbmp_get(unit);
    if (!BCM_PBMP_MEMBER(pbmp->val, 15))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting pbmp port 15 to be set\n")));
        res = 1;
    }
    port_pbmp = dnx_data_module_testing.property_methods.port_pbmp_get(unit, 2);
    if (!BCM_PBMP_MEMBER(port_pbmp->val, 19))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting port_pbmp port 19 to be set\n")));
        res = 1;
    }
    suffix_pbmp = dnx_data_module_testing.property_methods.suffix_pbmp_get(unit, 3);
    if (!BCM_PBMP_MEMBER(suffix_pbmp->val, 21))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting suffix_pbmp port 21 to be set\n")));
        res = 1;
    }

    re = dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE);
    if (re != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnx data init done flag set.\n")));
        return 1;       /* fail */
    }

    return res;
}
/**
 *\brief - main dnx data test
 * Test - unsupported data, dump, all types, property methods
 */
int
test_dnx_data_test(
    int unit,
    args_t * args,
    void *ptr)
{
    int res = 0;                /* test result - assume pass */
    int rv;

    /*
     * Test unsupported data
     */
    cli_out("DNX DATA Test Unsupported - Start\n");
    rv = test_dnx_data_unsupported(unit);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - unsupported data test failed.\n")));
        res = 1;
    }
    cli_out("DNX DATA Test Unsupported - Done\n");

    /*
     * Test dump functions
     */
    cli_out("DNX DATA Test Dump - Start\n");
    rv = sh_process_command(unit,  "data dump module_testing.*");
    if (rv != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dump data test failed.\n")));
        res = 1;
    }
    cli_out("DNX DATA Test Dump - Done\n");

    /*
     * Test different data types
     */
    cli_out("DNX DATA Test Types - Start\n");
    rv = test_dnx_data_types(unit);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - types data test failed.\n")));
        res = 1;
    }
    cli_out("DNX DATA Test Types - Done\n");

    /*
     * Test property methods
     */
    cli_out("DNX DATA Test Property Methods - Start\n");
    rv = test_dnx_data_property_methods(unit);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - property methods test failed.\n")));
        res = 1;
    }
    cli_out("DNX DATA Property Methods - Done\n");

    /*
     * Final Result
     */
    if (res == 0)
    {
        cli_out("DNX DATA Test - Pass\n");
    }
    return res;
}
#endif /* BCM_DNX_SUPPORT */

#ifdef BCM_DNXF_SUPPORT
/**
 * \brief - Test Supported/Unsupported Data types 
 * used to make sure that supporrted data will return the expected values    
 */
int
test_dnxf_data_unsupported(
    int unit)
{
    int feature_val;
    uint32 define_val;
    const uint32 *generic_val;
    const dnxf_data_module_testing_unsupported_supported_table_t *sup_table_val;
    int res = 0;                /* test result - assume pass */

    /*
     * features
     */
    feature_val = dnxf_data_module_testing.unsupported.feature_get
        (unit, dnxf_data_module_testing_unsupported_supported_feature);
    if (feature_val != 1)
    {

        /*
         * required data wasn't found throw print an error
         */
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that supported feature will return 1\n")));
        res = 1;
    }

    feature_val = dnxf_data_module_testing.unsupported.feature_get
        (unit, dnxf_data_module_testing_unsupported_unsupported_feature);
    if (feature_val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that unsupported feature will return 0\n")));
        res = 1;
    }

    /*
     * defines
     */
    define_val = dnxf_data_module_testing.unsupported.supported_def_get(unit);
    if (define_val != 0x12345678)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported define will return correct value, got %x\n"),
                   define_val));
        res = 1;
    }

    /*
     * tables
     */
    sup_table_val = dnxf_data_module_testing.unsupported.supported_table_get(unit);
    {
        if (sup_table_val == NULL)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - expected that supported table will return correct value\n")));
            res = 1;
        }
        else if (sup_table_val->num != 0x87654321)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - expected that supported table will return correct value, got %x\n"),
                       sup_table_val->num));
            res = 1;
        }
    }
    /*
     * Test generic Access API
     */
    /*
     * features
     */
    generic_val = dnxc_data_utils_generic_data_get(unit, "module_testing", "unsupported", "supported_feature", NULL);
    if (generic_val == NULL || *generic_val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that supported feature will return 1 (generic API)\n")));
        res = 1;
    }
    generic_val = dnxc_data_utils_generic_data_get(unit, "module_testing", "unsupported", "unsupported_feature", NULL);
    if (generic_val == NULL || *generic_val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - expected that unsupported feature will return 0 (generic API)\n")));
        res = 1;
    }

    return res;
}
/**
 * brief - Test different data types (of memebers in tables)
 */
static int
test_dnxf_data_types(
    int unit)
{
    const dnxf_data_module_testing_types_all_t *types_table_val;
    int res = 0;                /* test result - assume pass */

    /*
     * Get table struct
     */
    types_table_val = dnxf_data_module_testing.types.all_get(unit);
    if (types_table_val == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - expected that supported table will not return NULL\n")));
        res = 1;
    }
    else
    {
        /*
         * check values
         */
        if (types_table_val->intval != -1)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting intval=-1, got %d\n"), types_table_val->intval));
            res = 1;
        }
        if (types_table_val->uint8val != 0x12)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint8val=-1, got %d\n"), types_table_val->uint8val));
            res = 1;
        }
        if (types_table_val->charval != '1')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting charval='c', got %d\n"), types_table_val->charval));
            res = 1;
        }
        if (types_table_val->uint16val != 0x1234)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint16val=0x1234, got %x\n"), types_table_val->uint16val));
            res = 1;
        }
        if (types_table_val->enumval != bcmFabricDeviceTypeFE2)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting uint16val=bcmFabricDeviceTypeFE2, got %d\n"),
                       types_table_val->enumval));
            res = 1;
        }
        if (types_table_val->arrval[0] != 1 ||
            types_table_val->arrval[1] != 2 || types_table_val->arrval[2] != 3 || types_table_val->arrval[3] != 4)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting arrval={1, 2, 3, 4, 5, 6, 7, 8, 9, 10}\n")));
            res = 1;
        }
        if (!BCM_PBMP_MEMBER(types_table_val->pbmpval, 33))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting port 33 to be in pbmpval\n")));
            res = 1;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (sal_strcasecmp(types_table_val->strval, "dnxf data"))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting strval=\"dnxf data\", got %s\n"), types_table_val->strval));
            res = 1;
        }
        if (types_table_val->bufferval[1] != 'b')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error - Expecting bufferval[1]='b', got %d\n"),
                       types_table_val->bufferval[1]));
            res = 1;
        }
    }

    return res;
}

/**
 * \brief - Test different property methods
 */
static int
test_dnxf_data_property_methods(
    int unit)
{
    int res = 0;                /* test result - assume pass */
    cmd_result_t rv;
    shr_error_e re;
    uint32 numeric;
    int feature;
    const dnxf_data_module_testing_property_methods_enable_t *enable;
    const dnxf_data_module_testing_property_methods_port_enable_t *port_enable;
    const dnxf_data_module_testing_property_methods_suffix_enable_t *suffix_enable;
    const dnxf_data_module_testing_property_methods_range_t *range;
    const dnxf_data_module_testing_property_methods_port_range_t *port_range;
    const dnxf_data_module_testing_property_methods_suffix_range_t *suffix_range;
    const dnxf_data_module_testing_property_methods_direct_map_t *direct_map;
    const dnxf_data_module_testing_property_methods_port_direct_map_t *port_direct_map;
    const dnxf_data_module_testing_property_methods_suffix_direct_map_t *suffix_direct_map;
    const dnxf_data_module_testing_property_methods_custom_t *custom;

    /*
     * Add relevant SoC properties
     */
    rv = sh_process_command(unit, "config add dnxf_data_feature_enable=1");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_numeric_range=2");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_enable=1");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_port_enable_3=0");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_suffix_enable_link4=0");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_range=5");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_port_range_1=3");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_suffix_range_link2=4");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_direct_map=HIGH");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_port_direct_map_2=LOW");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_suffix_direct_map_link3=NORMAL");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }
    rv = sh_process_command(unit, "config add dnxf_data_custom_link2=1");
    if (rv != CMD_OK)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - config add command failed.\n")));
        return 1;       /* fail */
    }

    /*
     * Run Deinit-Init DNXF DATA - the SoC properties loaded upon init
     */
    re = dnxf_data_deinit(unit);
    if (re != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxf data deinit.\n")));
        return 1;       /* fail */
    }

    re = dnxf_data_init(unit);
    if (re != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxf data init.\n")));
        return 1;       /* fail */
    }

    /*
     * Get and Compare
     */
    /*
     * feature enable
     */
    feature =
        dnxf_data_module_testing.property_methods.feature_get(unit,
                                                              dnxf_data_module_testing_property_methods_feature_enable);
    if (feature != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting feature enable to be 1, got %d\n"), feature));
        res = 1;
    }

    /*
     * numeric range 
     */
    numeric = dnxf_data_module_testing.property_methods.numeric_range_get(unit);
    if (numeric != 2)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting numeric range to be 2, got %d\n"), numeric));
        res = 1;
    }

    /*
     * tables
     */
    /*
     * enable
     */
    enable = dnxf_data_module_testing.property_methods.enable_get(unit);
    if (enable->val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting enable to be 1, got %d\n"), enable->val));
        res = 1;
    }

    port_enable = dnxf_data_module_testing.property_methods.port_enable_get(unit, 3);
    if (port_enable->val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting  port enable to be 0, got %d\n"), port_enable->val));
        res = 1;
    }

    suffix_enable = dnxf_data_module_testing.property_methods.suffix_enable_get(unit, 4);
    if (suffix_enable->val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting suffix enable to be 0, got %d\n"), suffix_enable->val));
        res = 1;
    }

    /*
     * range
     */
    range = dnxf_data_module_testing.property_methods.range_get(unit);
    if (range->val != 5)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting range to be 5, got %d\n"), range->val));
        res = 1;
    }

    port_range = dnxf_data_module_testing.property_methods.port_range_get(unit, 1);
    if (port_range->val != 3)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting port range to be 3, got %d\n"), port_range->val));
        res = 1;
    }

    suffix_range = dnxf_data_module_testing.property_methods.suffix_range_get(unit, 2);
    if (suffix_range->val != 4)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting suffix range to be 4, got %d\n"), suffix_range->val));
        res = 1;
    }

    /*
     * direct map
     */
    direct_map = dnxf_data_module_testing.property_methods.direct_map_get(unit);
    if (direct_map->val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting direct_map to be 1, got %d\n"), direct_map->val));
        res = 1;
    }

    port_direct_map = dnxf_data_module_testing.property_methods.port_direct_map_get(unit, 2);
    if (port_direct_map->val != 2)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting port direct_map to be 2, got %d\n"), port_direct_map->val));
        res = 1;
    }

    suffix_direct_map = dnxf_data_module_testing.property_methods.suffix_direct_map_get(unit, 3);
    if (suffix_direct_map->val != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error - Expecting suffix direct_map to be 0, got %d\n"), suffix_direct_map->val));
        res = 1;
    }

    /*
     * custom
     */
    custom = dnxf_data_module_testing.property_methods.custom_get(unit, 2, 1);
    if (custom->val != 1)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - Expecting custom to be 1, got %d\n"), custom->val));
        res = 1;
    }

    re = dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE);
    if (re != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxf data init done flag set.\n")));
        return 1;       /* fail */
    }

    return res;
}
/**
 *\brief - main dnxf data test
 * Test - unsupported data, dump, all types, property methods
 */
int
test_dnxf_data_test(
    int unit,
    args_t * args,
    void *ptr)
{
    int res = 0;                /* test result - assume pass */
    int rv;

    /*
     * Test unsupported data
     */
    cli_out("DNXF DATA Test Unsupported - Start\n");
    rv = test_dnxf_data_unsupported(unit);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - unsupported data test failed.\n")));
        res = 1;
    }
    cli_out("DNXF DATA Test Unsupported - Done\n");

    /*
     * Test dump functions
     */
    cli_out("DNXF DATA Test Dump - Start\n");
    rv = sh_process_command(unit,  "data dump module_testing.*");
    if (rv != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dump data test failed.\n")));
        res = 1;
    }
    cli_out("DNXF DATA Test Dump - Done\n");

    /*
     * Test different data types
     */
    cli_out("DNXF DATA Test Types - Start\n");
    rv = test_dnxf_data_types(unit);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - types data test failed.\n")));
        res = 1;
    }
    cli_out("DNXF DATA Test Types - Done\n");

    /*
     * Test property methods
     */
    cli_out("DNXF DATA Test Property Methods - Start\n");
    rv = test_dnxf_data_property_methods(unit);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - property methods test failed.\n")));
        res = 1;
    }
    cli_out("DNXF DATA Property Methods - Done\n");

    /*
     * Final Result
     */
    if (res == 0)
    {
        cli_out("DNXF DATA Test - Pass\n");
    }
    return res;
}
#endif /* BCM_DNXF_SUPPORT */

int
test_dnxc_data_test(
    int unit,
    args_t * args,
    void *ptr)
{
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        return test_dnx_data_test(unit, args, ptr);
    }
#endif /* BCM_DNX_SUPPORT */

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        return test_dnxf_data_test(unit, args, ptr);
    }
#endif /* BCM_DNXF_SUPPORT */

    /*
     * Wrong unit - throw failure
     */
    return 1;
}
#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */
