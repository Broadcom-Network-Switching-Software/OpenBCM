
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ecgm_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_ecgm_sw_state_t * dnx_ecgm_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_ecgm_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ecgm_db_interface_caching_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_port_caching_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ecgm_db_interface_caching_port_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_interface_caching_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_interface_caching_profile_id_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_interface_caching_valid_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_port_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_port_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm interface_caching port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm interface_caching port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/interface_caching/port.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.port: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_ecgm_interface_profile_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm interface_caching info") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm interface_caching info\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/interface_caching/info.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.info: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.info: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_profile_id_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm interface_caching profile_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm interface_caching profile_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/interface_caching/profile_id.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.profile_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.profile_id: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_interface_caching_valid_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm interface_caching valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm interface_caching valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/interface_caching/valid.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","interface_caching.valid: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ecgm_db_port_caching_port_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_port_caching_info_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_port_caching_profile_id_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_ecgm_db_port_caching_valid_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_port_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_port_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm port_caching port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm port_caching port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/port_caching/port.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.port: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_ecgm_port_profile_info_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm port_caching info") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm port_caching info\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/port_caching/info.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.info: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.info: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_algo_ecgm_port_profile_info_print(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_profile_id_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm port_caching profile_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm port_caching profile_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/port_caching/profile_id.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.profile_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.profile_id: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ecgm_db_port_caching_valid_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ecgm port_caching valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ecgm port_caching valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ecgm_db/port_caching/valid.txt",
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_ecgm_db[%d]->","((dnx_ecgm_sw_state_t*)sw_state_roots_array[%d][DNX_ECGM_MODULE_ID])->","port_caching.valid: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_ecgm_db_info[SOC_MAX_NUM_DEVICES][DNX_ECGM_DB_INFO_NOF_ENTRIES];
const char* dnx_ecgm_db_layout_str[DNX_ECGM_DB_INFO_NOF_ENTRIES] = {
    "DNX_ECGM_DB~",
    "DNX_ECGM_DB~INTERFACE_CACHING~",
    "DNX_ECGM_DB~INTERFACE_CACHING~PORT~",
    "DNX_ECGM_DB~INTERFACE_CACHING~INFO~",
    "DNX_ECGM_DB~INTERFACE_CACHING~PROFILE_ID~",
    "DNX_ECGM_DB~INTERFACE_CACHING~VALID~",
    "DNX_ECGM_DB~PORT_CACHING~",
    "DNX_ECGM_DB~PORT_CACHING~PORT~",
    "DNX_ECGM_DB~PORT_CACHING~INFO~",
    "DNX_ECGM_DB~PORT_CACHING~PROFILE_ID~",
    "DNX_ECGM_DB~PORT_CACHING~VALID~",
};
#endif 
#undef BSL_LOG_MODULE
