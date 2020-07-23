
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_cosq_ingress_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_cosq_ingress_db_t * dnx_cosq_ingress_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_cosq_ingress_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db_fixed_priority_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db_fixed_priority_nif_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db_fixed_priority_mirror_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_nif_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_pbmp_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_cosq_ingress fixed_priority nif") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_cosq_ingress fixed_priority nif\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_cosq_ingress_db/fixed_priority/nif.txt",
            "dnx_cosq_ingress_db[%d]->","((dnx_cosq_ingress_db_t*)sw_state_roots_array[%d][DNX_COSQ_INGRESS_MODULE_ID])->","fixed_priority.nif: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
            "dnx_cosq_ingress_db[%d]->","((dnx_cosq_ingress_db_t*)sw_state_roots_array[%d][DNX_COSQ_INGRESS_MODULE_ID])->","fixed_priority.nif: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.nif,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_cosq_ingress_db_fixed_priority_mirror_dump(int  unit,  int  mirror_idx_0,  int  mirror_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = mirror_idx_0, I0 = mirror_idx_0 + 1;
    int i1 = mirror_idx_1, I1 = mirror_idx_1 + 1, org_i1 = mirror_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_cosq_ingress fixed_priority mirror") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_cosq_ingress fixed_priority mirror\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_cosq_ingress_db/fixed_priority/mirror.txt",
            "dnx_cosq_ingress_db[%d]->","((dnx_cosq_ingress_db_t*)sw_state_roots_array[%d][DNX_COSQ_INGRESS_MODULE_ID])->","fixed_priority.mirror[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror
                , sizeof(*((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("dnx_cosq_ingress_db[]->((dnx_cosq_ingress_db_t*)sw_state_roots_array[][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = ((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[i0]
                    , sizeof(*((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= ((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1)
            {
                LOG_CLI((BSL_META("dnx_cosq_ingress_db[]->((dnx_cosq_ingress_db_t*)sw_state_roots_array[][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_cosq_ingress_db[%d]->","((dnx_cosq_ingress_db_t*)sw_state_roots_array[%d][DNX_COSQ_INGRESS_MODULE_ID])->","fixed_priority.mirror[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_shr_bitdcl(
                    unit,
                    &((dnx_cosq_ingress_db_t*)sw_state_roots_array[unit][DNX_COSQ_INGRESS_MODULE_ID])->fixed_priority.mirror[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_cosq_ingress_db_info[SOC_MAX_NUM_DEVICES][DNX_COSQ_INGRESS_DB_INFO_NOF_ENTRIES];
const char* dnx_cosq_ingress_db_layout_str[DNX_COSQ_INGRESS_DB_INFO_NOF_ENTRIES] = {
    "DNX_COSQ_INGRESS_DB~",
    "DNX_COSQ_INGRESS_DB~FIXED_PRIORITY~",
    "DNX_COSQ_INGRESS_DB~FIXED_PRIORITY~NIF~",
    "DNX_COSQ_INGRESS_DB~FIXED_PRIORITY~MIRROR~",
};
#endif 
#undef BSL_LOG_MODULE
