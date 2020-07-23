
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ipq_diagnostic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_ipq_db_t * dnx_ipq_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_ipq_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ipq_db_base_queues_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_ipq_db_base_queue_is_asymm_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_dump(int  unit,  int  base_queues_idx_0,  int  base_queues_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_ipq_db_base_queues_num_cos_dump(unit, base_queues_idx_0,base_queues_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_ipq_db_base_queues_sys_port_ref_counter_dump(unit, base_queues_idx_0,base_queues_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_num_cos_dump(int  unit,  int  base_queues_idx_0,  int  base_queues_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = base_queues_idx_0, I0 = base_queues_idx_0 + 1;
    int i1 = base_queues_idx_1, I1 = base_queues_idx_1 + 1, org_i1 = base_queues_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ipq base_queues num_cos") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ipq base_queues num_cos\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ipq_db/base_queues/num_cos.txt",
            "dnx_ipq_db[%d]->","((dnx_ipq_db_t*)sw_state_roots_array[%d][DNX_IPQ_MODULE_ID])->","base_queues[][].num_cos: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_IPQ_MODULE_ID,
                ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues
                , sizeof(*((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues))
        {
            LOG_CLI((BSL_META("dnx_ipq_db[]->((dnx_ipq_db_t*)sw_state_roots_array[][DNX_IPQ_MODULE_ID])->base_queues[][].num_cos: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_IPQ_MODULE_ID,
                    ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]
                    , sizeof(*((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]))
            {
                LOG_CLI((BSL_META("dnx_ipq_db[]->((dnx_ipq_db_t*)sw_state_roots_array[][DNX_IPQ_MODULE_ID])->base_queues[][].num_cos: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ipq_db[%d]->","((dnx_ipq_db_t*)sw_state_roots_array[%d][DNX_IPQ_MODULE_ID])->","base_queues[%s%d][%s%d].num_cos: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0][i1].num_cos);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_sys_port_ref_counter_dump(int  unit,  int  base_queues_idx_0,  int  base_queues_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = base_queues_idx_0, I0 = base_queues_idx_0 + 1;
    int i1 = base_queues_idx_1, I1 = base_queues_idx_1 + 1, org_i1 = base_queues_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ipq base_queues sys_port_ref_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ipq base_queues sys_port_ref_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ipq_db/base_queues/sys_port_ref_counter.txt",
            "dnx_ipq_db[%d]->","((dnx_ipq_db_t*)sw_state_roots_array[%d][DNX_IPQ_MODULE_ID])->","base_queues[][].sys_port_ref_counter: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_IPQ_MODULE_ID,
                ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues
                , sizeof(*((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues))
        {
            LOG_CLI((BSL_META("dnx_ipq_db[]->((dnx_ipq_db_t*)sw_state_roots_array[][DNX_IPQ_MODULE_ID])->base_queues[][].sys_port_ref_counter: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_IPQ_MODULE_ID,
                    ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]
                    , sizeof(*((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]))
            {
                LOG_CLI((BSL_META("dnx_ipq_db[]->((dnx_ipq_db_t*)sw_state_roots_array[][DNX_IPQ_MODULE_ID])->base_queues[][].sys_port_ref_counter: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_ipq_db[%d]->","((dnx_ipq_db_t*)sw_state_roots_array[%d][DNX_IPQ_MODULE_ID])->","base_queues[%s%d][%s%d].sys_port_ref_counter: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[i0][i1].sys_port_ref_counter);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_dump(int  unit,  int  base_queue_is_asymm_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = base_queue_is_asymm_idx_0, I0 = base_queue_is_asymm_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_ipq base_queue_is_asymm") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_ipq base_queue_is_asymm\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_ipq_db/base_queue_is_asymm.txt",
            "dnx_ipq_db[%d]->","((dnx_ipq_db_t*)sw_state_roots_array[%d][DNX_IPQ_MODULE_ID])->","base_queue_is_asymm[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_IPQ_MODULE_ID,
                ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm
                , sizeof(*((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm))
        {
            LOG_CLI((BSL_META("dnx_ipq_db[]->((dnx_ipq_db_t*)sw_state_roots_array[][DNX_IPQ_MODULE_ID])->base_queue_is_asymm[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_IPQ_MODULE_ID, ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_ipq_db[%d]->","((dnx_ipq_db_t*)sw_state_roots_array[%d][DNX_IPQ_MODULE_ID])->","base_queue_is_asymm[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_ipq_db_info[SOC_MAX_NUM_DEVICES][DNX_IPQ_DB_INFO_NOF_ENTRIES];
const char* dnx_ipq_db_layout_str[DNX_IPQ_DB_INFO_NOF_ENTRIES] = {
    "DNX_IPQ_DB~",
    "DNX_IPQ_DB~BASE_QUEUES~",
    "DNX_IPQ_DB~BASE_QUEUES~NUM_COS~",
    "DNX_IPQ_DB~BASE_QUEUES~SYS_PORT_REF_COUNTER~",
    "DNX_IPQ_DB~BASE_QUEUE_IS_ASYMM~",
};
#endif 
#undef BSL_LOG_MODULE
