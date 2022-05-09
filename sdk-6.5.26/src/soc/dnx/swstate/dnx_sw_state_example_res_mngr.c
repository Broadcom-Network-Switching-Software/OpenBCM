
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
#include <bcm_int/dnx/algo/swstate/auto_generated/access/example_res_mngr_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

int
dnx_sw_state_example_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t create_data;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init algo_res_mngr_db module.\n")));
    SHR_IF_ERR_EXIT(algo_res_mngr_db.init(unit));

    sal_memset(&create_data, 0, sizeof(dnx_algo_res_create_data_t));
    sal_strncpy(create_data.name, "tested resource", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    create_data.flags = dnx_data_infra_tests.res_mngr_example.create_flags_get(unit);
    create_data.first_element = dnx_data_infra_tests.res_mngr_example.first_element_get(unit);
    create_data.nof_elements = dnx_data_infra_tests.res_mngr_example.nof_elements_get(unit);

    if (dnx_data_infra_tests.res_mngr_example.extra_arg_get(unit))
    {
        sal_memset(&extra_create_info, 0, sizeof(resource_tag_bitmap_extra_arguments_create_info_t));
        extra_create_info.grains_size[0] = dnx_data_infra_tests.res_mngr_example.grain_size_1_get(unit);
        extra_create_info.grains_size[1] = dnx_data_infra_tests.res_mngr_example.grain_size_2_get(unit);
        extra_create_info.max_tags_value[0] = dnx_data_infra_tests.res_mngr_example.max_tag_value_1_get(unit);
        extra_create_info.max_tags_value[1] = dnx_data_infra_tests.res_mngr_example.max_tag_value_2_get(unit);
        extra_create_info.nof_tags_levels = dnx_data_infra_tests.res_mngr_example.nof_tags_levels_get(unit);
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource.create(unit, &create_data, &extra_create_info));
    }
    else
        SHR_IF_ERR_EXIT(algo_res_mngr_db.resource.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

#else
typedef int make_iso_compilers_happy;

#endif
