
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/reflector_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern reflector_sw_db_info_t * reflector_sw_db_info_data[SOC_MAX_NUM_DEVICES];



int
reflector_sw_db_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(reflector_sw_db_info_uc_dump(unit, filters));
    SHR_IF_ERR_EXIT(reflector_sw_db_info_sbfd_reflector_cnt_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(reflector_sw_db_info_uc_encap_id_dump(unit, filters));
    SHR_IF_ERR_EXIT(reflector_sw_db_info_uc_is_allocated_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_encap_id_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "reflector uc encap_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate reflector uc encap_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "reflector_sw_db_info/uc/encap_id.txt",
            "reflector_sw_db_info[%d]->","((reflector_sw_db_info_t*)sw_state_roots_array[%d][REFLECTOR_MODULE_ID])->","uc.encap_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "reflector_sw_db_info[%d]->","((reflector_sw_db_info_t*)sw_state_roots_array[%d][REFLECTOR_MODULE_ID])->","uc.encap_id: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.encap_id);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_uc_is_allocated_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "reflector uc is_allocated") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate reflector uc is_allocated\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "reflector_sw_db_info/uc/is_allocated.txt",
            "reflector_sw_db_info[%d]->","((reflector_sw_db_info_t*)sw_state_roots_array[%d][REFLECTOR_MODULE_ID])->","uc.is_allocated: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "reflector_sw_db_info[%d]->","((reflector_sw_db_info_t*)sw_state_roots_array[%d][REFLECTOR_MODULE_ID])->","uc.is_allocated: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->uc.is_allocated);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
reflector_sw_db_info_sbfd_reflector_cnt_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "reflector sbfd_reflector_cnt") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate reflector sbfd_reflector_cnt\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "reflector_sw_db_info/sbfd_reflector_cnt.txt",
            "reflector_sw_db_info[%d]->","((reflector_sw_db_info_t*)sw_state_roots_array[%d][REFLECTOR_MODULE_ID])->","sbfd_reflector_cnt: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "reflector_sw_db_info[%d]->","((reflector_sw_db_info_t*)sw_state_roots_array[%d][REFLECTOR_MODULE_ID])->","sbfd_reflector_cnt: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((reflector_sw_db_info_t*)sw_state_roots_array[unit][REFLECTOR_MODULE_ID])->sbfd_reflector_cnt);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t reflector_sw_db_info_info[SOC_MAX_NUM_DEVICES][REFLECTOR_SW_DB_INFO_INFO_NOF_ENTRIES];
const char* reflector_sw_db_info_layout_str[REFLECTOR_SW_DB_INFO_INFO_NOF_ENTRIES] = {
    "REFLECTOR_SW_DB_INFO~",
    "REFLECTOR_SW_DB_INFO~UC~",
    "REFLECTOR_SW_DB_INFO~UC~ENCAP_ID~",
    "REFLECTOR_SW_DB_INFO~UC~IS_ALLOCATED~",
    "REFLECTOR_SW_DB_INFO~SBFD_REFLECTOR_CNT~",
};
#endif 
#undef BSL_LOG_MODULE
