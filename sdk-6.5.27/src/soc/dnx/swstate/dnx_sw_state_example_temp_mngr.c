
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_CTEST)

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/example_temp_mngr_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>
#include <soc/dnx/swstate/dnx_sw_state_example_temp_mngr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

static int
soc_prop_data_get(
    int unit,
    template_mngr_example_data_t * profile_data,
    dnx_algo_template_create_data_t * data_default)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(profile_data, 0, sizeof(template_mngr_example_data_t));
    sal_memset(data_default, 0, sizeof(dnx_algo_template_create_data_t));

    profile_data->element_8 = dnx_data_infra_tests.temp_mngr_example.element_8_get(unit);
    profile_data->element_16 = dnx_data_infra_tests.temp_mngr_example.element_16_get(unit);
    profile_data->element_32 = dnx_data_infra_tests.temp_mngr_example.element_32_get(unit);
    profile_data->element_int = dnx_data_infra_tests.temp_mngr_example.element_int_get(unit);

    sal_strncpy(data_default->name, "template ", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    data_default->flags = dnx_data_infra_tests.temp_mngr_example.create_flags_get(unit);
    data_default->first_profile = dnx_data_infra_tests.temp_mngr_example.first_profile_get(unit);
    data_default->nof_profiles = dnx_data_infra_tests.temp_mngr_example.nof_profiles_get(unit);
    data_default->max_references = dnx_data_infra_tests.temp_mngr_example.max_references_get(unit);
    data_default->data_size = sizeof(template_mngr_example_data_t);

    if (_SHR_IS_FLAG_SET(data_default->flags, DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE))
    {
        data_default->default_data = profile_data;
        data_default->default_profile = dnx_data_infra_tests.temp_mngr_example.default_profile_get(unit);
    }
    if (dnx_data_infra_tests.temp_mngr_example.extra_arg_get(unit))
    {
        data_default->advanced_algorithm = dnx_data_infra_tests.temp_mngr_example.advanced_algorithm_get(unit);
    }

    SHR_FUNC_EXIT;
}

static int
dnx_algo_template_input_bad_values_init(
    int unit)
{
    int rv;
    template_mngr_example_data_t profile_data;
    dnx_algo_template_create_data_t data, tmp_data;
    bsl_severity_t original_severity_sw_state = BSL_INFO, original_temp_mngr = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, original_temp_mngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, bslSeverityFatal);

    soc_prop_data_get(unit, &profile_data, &data);
    tmp_data = data;
    tmp_data.flags |= DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;

    tmp_data.first_profile = -1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative first_profile but creation was successful\n");
    }

    tmp_data.first_profile = tmp_data.nof_profiles - 1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "first_profile > default_profile but creation was successful\n");
    }
    tmp_data.first_profile = 0;

    tmp_data.nof_profiles = -1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative nof_profiles but creation was successful\n");
    }
    tmp_data.nof_profiles = -1;

    tmp_data.max_references = -1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative max_references but creation was successful\n");
    }
    tmp_data.max_references = 32;

    tmp_data.default_profile = -1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative default_profile but creation was successful\n");
    }

    tmp_data.default_profile = 16;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "default_profile = nof_profiles but creation was successful\n");
    }
    tmp_data.default_profile = 0;

    tmp_data.data_size = -1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative data_size but creation was successful\n");
    }
    tmp_data.data_size = sizeof(uint8);
exit:

    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, original_temp_mngr);
    SHR_FUNC_EXIT;
}

int
dnx_sw_state_example_temp_mngr_soc_prop_init(
    int unit)
{
    dnx_algo_template_create_data_t data_default;
    template_mngr_example_data_t profile_data;

    SHR_FUNC_INIT_VARS(unit);
    soc_prop_data_get(unit, &profile_data, &data_default);
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template.create(unit, &data_default, NULL));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_sw_state_example_temp_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init algo_temp_mngr_db module.\n")));
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_template_input_bad_values_init(unit));
    SHR_IF_ERR_EXIT(dnx_sw_state_example_temp_mngr_soc_prop_init(unit));

exit:
    SHR_FUNC_EXIT;
}
#undef _ERR_MSG_MODULE_NAME

#else
typedef int make_iso_compilers_happy;

#endif
