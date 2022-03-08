
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/swstate/dnx_sw_state_example_temp_mngr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

static int
dnx_algo_template_input_valid_values_init(
    int unit)
{
    int rv;
    template_mngr_example_data_t profile_data;
    dnx_algo_template_create_data_t data, tmp_data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    profile_data.element_8 = DEFAULT_PROFILE_DATA_8;
    profile_data.element_16 = DEFAULT_PROFILE_DATA_16;
    profile_data.element_32 = DEFAULT_PROFILE_DATA_32;
    profile_data.element_int = DEFAULT_PROFILE_DATA_INT;

    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.first_profile = FIRST_PROFILE;
    data.nof_profiles = NOF_PROFILES;
    data.max_references = MAX_REFERENCES;
    data.default_profile = DEFAULT_PROFILE;
    data.data_size = sizeof(template_mngr_example_data_t);
    data.default_data = &profile_data;
    data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC;

    tmp_data = data;

    sal_strncpy(data.name, TEMPLATE_TEST_WITH_DEFAULT_CHECK_VALUES, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default_check_values.create(unit, &data, NULL));

    tmp_data.flags = FLAGS_NONE;
    sal_strncpy(tmp_data.name, TEMPLATE_TEST_WITHOUT_DEFAULT_CHECK_VALUES, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default_check_values.create(unit, &tmp_data, NULL));

    tmp_data.flags = 0;
    sal_strncpy(tmp_data.name, TEMPLATE_TEST_PER_CORE_CHECK_VALUES, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core_check_values.alloc(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core_check_values.create(unit, core_id, &tmp_data, NULL));
    }

    tmp_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;

    rv = algo_temp_mngr_db.template_with_default_check_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "template already exists but creation was successful\n");
    }

    tmp_data.first_profile = NEGATIVE_PROFILE;

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
    tmp_data.first_profile = FIRST_PROFILE;

    tmp_data.nof_profiles = NEGATIVE_NOF_PROFILES;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative nof_profiles but creation was successful\n");
    }
    tmp_data.nof_profiles = NOF_PROFILES;

    tmp_data.max_references = NEGATIVE_MAX_REFERENCES;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative max_references but creation was successful\n");
    }
    tmp_data.max_references = MAX_REFERENCES;

    tmp_data.default_profile = NEGATIVE_DEFAULT_PROFILE;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative default_profile but creation was successful\n");
    }

    tmp_data.default_profile = NOF_PROFILES;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "default_profile = nof_profiles but creation was successful\n");
    }
    tmp_data.default_profile = DEFAULT_PROFILE;

    tmp_data.data_size = NEGATIVE_DATA_SIZE;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative data_size but creation was successful\n");
    }
    tmp_data.data_size = sizeof(uint8);
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_algo_template_test_without_default_init(
    int unit)
{
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.flags = FLAGS_NONE;
    data.nof_profiles = NOF_PROFILES;
    data.max_references = MAX_REFERENCES;
    data.data_size = sizeof(template_mngr_example_data_t);
    data.first_profile = FIRST_PROFILE;

    sal_strncpy(data.name, TEMPLATE_TEST_WITHOUT_DEFAULT, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_algo_template_test_with_default_init(
    int unit)
{
    dnx_algo_template_create_data_t data_default;
    template_mngr_example_data_t profile_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&profile_data, 0, sizeof(profile_data));
    profile_data.element_8 = DEFAULT_PROFILE_DATA_8;
    profile_data.element_16 = DEFAULT_PROFILE_DATA_16;
    profile_data.element_32 = DEFAULT_PROFILE_DATA_32;
    profile_data.element_int = DEFAULT_PROFILE_DATA_INT;

    sal_memset(&data_default, 0, sizeof(dnx_algo_template_create_data_t));
    data_default.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data_default.first_profile = FIRST_PROFILE;
    data_default.nof_profiles = NOF_PROFILES;
    data_default.max_references = MAX_REFERENCES;
    data_default.default_profile = DEFAULT_PROFILE;
    data_default.data_size = sizeof(template_mngr_example_data_t);
    data_default.default_data = &profile_data;

    sal_strncpy(data_default.name, TEMPLATE_TEST_WITH_DEFAULT, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default.create(unit, &data_default, NULL));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_algo_template_test_per_core_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.flags = FLAGS_NONE;
    data.nof_profiles = NOF_PROFILES;
    data.max_references = MAX_REFERENCES;
    data.data_size = sizeof(template_mngr_example_data_t);
    data.first_profile = FIRST_PROFILE;

    sal_strncpy(data.name, TEMPLATE_TEST_PER_CORE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.alloc(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.create(unit, core_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_algo_template_test_with_same_data_prifiles_init(
    int unit)
{
    dnx_algo_template_create_data_t data_default;
    template_mngr_example_data_t profile_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&profile_data, 0, sizeof(profile_data));
    profile_data.element_8 = DEFAULT_PROFILE_DATA_8;
    profile_data.element_16 = DEFAULT_PROFILE_DATA_16;
    profile_data.element_32 = DEFAULT_PROFILE_DATA_32;
    profile_data.element_int = DEFAULT_PROFILE_DATA_INT;

    sal_memset(&data_default, 0, sizeof(dnx_algo_template_create_data_t));
    data_default.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE | DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    data_default.first_profile = FIRST_PROFILE;
    data_default.nof_profiles = NOF_PROFILES;
    data_default.max_references = MAX_REFERENCES;
    data_default.default_profile = DEFAULT_PROFILE;
    data_default.data_size = sizeof(template_mngr_example_data_t);
    data_default.default_data = &profile_data;
    data_default.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SAME_DATA;

    sal_strncpy(data_default.name, TEMPLATE_TEST_WITH_SAME_DATA, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_same_data.create(unit, &data_default, NULL));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_sw_state_example_temp_mngr_init(
    int unit)
{

    bsl_severity_t original_severity_sw_state = BSL_INFO, original_temp_mngr = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, original_temp_mngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, bslSeverityFatal);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_template_input_valid_values_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_template_test_without_default_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_template_test_with_default_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_template_test_per_core_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_template_test_with_same_data_prifiles_init(unit));

exit:

    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_temp_mngr);

    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

#else
typedef int make_iso_compilers_happy;

#endif
