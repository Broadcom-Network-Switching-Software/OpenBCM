
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
#include <soc/dnx/swstate/auto_generated/diagnostic/oam_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern oam_sw_db_info_t * oam_sw_db_info_data[SOC_MAX_NUM_DEVICES];



int
oam_sw_db_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(oam_sw_db_info_oam_group_sw_db_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(oam_sw_db_info_oam_endpoint_sw_db_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(oam_sw_db_info_reflector_dump(unit, filters));
    SHR_IF_ERR_EXIT(oam_sw_db_info_tst_trap_used_cnt_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_group_sw_db_info_oam_group_array_of_linked_lists_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_multihead_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "oam oam_group_sw_db_info oam_group_array_of_linked_lists") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate oam oam_group_sw_db_info oam_group_array_of_linked_lists\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "oam_sw_db_info/oam_group_sw_db_info/oam_group_array_of_linked_lists.txt",
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","oam_group_sw_db_info.oam_group_array_of_linked_lists: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","oam_group_sw_db_info.oam_group_array_of_linked_lists: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_LL_DEFAULT_PRINT(
            unit,
            &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_group_sw_db_info.oam_group_array_of_linked_lists);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_oam_endpoint_sw_db_info_oam_endpoint_array_of_rmep_linked_lists_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_multihead_ll_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "oam oam_endpoint_sw_db_info oam_endpoint_array_of_rmep_linked_lists") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate oam oam_endpoint_sw_db_info oam_endpoint_array_of_rmep_linked_lists\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "oam_sw_db_info/oam_endpoint_sw_db_info/oam_endpoint_array_of_rmep_linked_lists.txt",
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        SW_STATE_LL_DEFAULT_PRINT(
            unit,
            &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->oam_endpoint_sw_db_info.oam_endpoint_array_of_rmep_linked_lists);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(oam_sw_db_info_reflector_encap_id_dump(unit, filters));
    SHR_IF_ERR_EXIT(oam_sw_db_info_reflector_is_allocated_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_encap_id_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "oam reflector encap_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate oam reflector encap_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "oam_sw_db_info/reflector/encap_id.txt",
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","reflector.encap_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","reflector.encap_id: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.encap_id);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_reflector_is_allocated_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "oam reflector is_allocated") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate oam reflector is_allocated\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "oam_sw_db_info/reflector/is_allocated.txt",
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","reflector.is_allocated: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","reflector.is_allocated: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->reflector.is_allocated);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
oam_sw_db_info_tst_trap_used_cnt_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "oam tst_trap_used_cnt") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate oam tst_trap_used_cnt\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "oam_sw_db_info/tst_trap_used_cnt.txt",
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","tst_trap_used_cnt: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "oam_sw_db_info[%d]->","((oam_sw_db_info_t*)sw_state_roots_array[%d][OAM_MODULE_ID])->","tst_trap_used_cnt: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((oam_sw_db_info_t*)sw_state_roots_array[unit][OAM_MODULE_ID])->tst_trap_used_cnt);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t oam_sw_db_info_info[SOC_MAX_NUM_DEVICES][OAM_SW_DB_INFO_INFO_NOF_ENTRIES];
const char* oam_sw_db_info_layout_str[OAM_SW_DB_INFO_INFO_NOF_ENTRIES] = {
    "OAM_SW_DB_INFO~",
    "OAM_SW_DB_INFO~OAM_GROUP_SW_DB_INFO~",
    "OAM_SW_DB_INFO~OAM_GROUP_SW_DB_INFO~OAM_GROUP_ARRAY_OF_LINKED_LISTS~",
    "OAM_SW_DB_INFO~OAM_ENDPOINT_SW_DB_INFO~",
    "OAM_SW_DB_INFO~OAM_ENDPOINT_SW_DB_INFO~OAM_ENDPOINT_ARRAY_OF_RMEP_LINKED_LISTS~",
    "OAM_SW_DB_INFO~REFLECTOR~",
    "OAM_SW_DB_INFO~REFLECTOR~ENCAP_ID~",
    "OAM_SW_DB_INFO~REFLECTOR~IS_ALLOCATED~",
    "OAM_SW_DB_INFO~TST_TRAP_USED_CNT~",
};
#endif 
#undef BSL_LOG_MODULE
