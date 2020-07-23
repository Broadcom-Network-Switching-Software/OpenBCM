
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_field_action_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_algo_field_action_sw_t * dnx_algo_field_action_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_algo_field_action_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_fes_state_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_context_state_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_dump(int  unit,  int  fes_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_fes_state_field_group_dump(unit, fes_state_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_dump(unit, fes_state_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_dump(unit, fes_state_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_fes_state_field_group_dump(int  unit,  int  fes_state_idx_0,  int  field_group_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = field_group_idx_0, I1 = field_group_idx_0 + 1, org_i1 = field_group_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf2 fes_state field_group") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf2 fes_state field_group\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf2/fes_state/field_group.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.fes_state[].field_group[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[].field_group[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].field_group
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].field_group), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[].field_group[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].field_group[i1],
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.fes_state[%s%d].field_group[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].field_group[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_algo_field_action_sw_ipmf2_fes_state_place_in_fg_dump(int  unit,  int  fes_state_idx_0,  int  place_in_fg_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = place_in_fg_idx_0, I1 = place_in_fg_idx_0 + 1, org_i1 = place_in_fg_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf2 fes_state place_in_fg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf2 fes_state place_in_fg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf2/fes_state/place_in_fg.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.fes_state[].place_in_fg[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[].place_in_fg[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].place_in_fg
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].place_in_fg), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[].place_in_fg[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.fes_state[%s%d].place_in_fg[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].place_in_fg[i1]);

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
dnx_algo_field_action_sw_ipmf2_fes_state_mask_is_alloc_dump(int  unit,  int  fes_state_idx_0,  int  mask_is_alloc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = mask_is_alloc_idx_0, I1 = mask_is_alloc_idx_0 + 1, org_i1 = mask_is_alloc_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf2 fes_state mask_is_alloc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf2 fes_state mask_is_alloc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf2/fes_state/mask_is_alloc.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.fes_state[].mask_is_alloc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[].mask_is_alloc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].mask_is_alloc
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].mask_is_alloc), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[].mask_is_alloc[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.fes_state[%s%d].mask_is_alloc[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.fes_state[i0].mask_is_alloc[i1]);

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
dnx_algo_field_action_sw_ipmf2_context_state_dump(int  unit,  int  context_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf2_context_state_priority_dump(unit, context_state_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf2_context_state_priority_dump(int  unit,  int  context_state_idx_0,  int  priority_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_state_idx_0, I0 = context_state_idx_0 + 1;
    int i1 = priority_idx_0, I1 = priority_idx_0 + 1, org_i1 = priority_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_priority_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf2 context_state priority") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf2 context_state priority\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf2/context_state/priority.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.context_state[].priority[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[].priority[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[i0].priority
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[i0].priority), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[].priority[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[i0].priority[i1],
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf2.context_state[%s%d].priority[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf2.context_state[i0].priority[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_algo_field_action_sw_ipmf3_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_fes_state_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_context_state_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_dump(int  unit,  int  fes_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_fes_state_field_group_dump(unit, fes_state_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_dump(unit, fes_state_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_dump(unit, fes_state_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_fes_state_field_group_dump(int  unit,  int  fes_state_idx_0,  int  field_group_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = field_group_idx_0, I1 = field_group_idx_0 + 1, org_i1 = field_group_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf3 fes_state field_group") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf3 fes_state field_group\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf3/fes_state/field_group.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.fes_state[].field_group[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[].field_group[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].field_group
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].field_group), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[].field_group[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].field_group[i1],
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.fes_state[%s%d].field_group[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].field_group[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_algo_field_action_sw_ipmf3_fes_state_place_in_fg_dump(int  unit,  int  fes_state_idx_0,  int  place_in_fg_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = place_in_fg_idx_0, I1 = place_in_fg_idx_0 + 1, org_i1 = place_in_fg_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf3 fes_state place_in_fg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf3 fes_state place_in_fg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf3/fes_state/place_in_fg.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.fes_state[].place_in_fg[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[].place_in_fg[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].place_in_fg
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].place_in_fg), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[].place_in_fg[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.fes_state[%s%d].place_in_fg[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].place_in_fg[i1]);

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
dnx_algo_field_action_sw_ipmf3_fes_state_mask_is_alloc_dump(int  unit,  int  fes_state_idx_0,  int  mask_is_alloc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = mask_is_alloc_idx_0, I1 = mask_is_alloc_idx_0 + 1, org_i1 = mask_is_alloc_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf3 fes_state mask_is_alloc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf3 fes_state mask_is_alloc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf3/fes_state/mask_is_alloc.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.fes_state[].mask_is_alloc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[].mask_is_alloc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].mask_is_alloc
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].mask_is_alloc), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[].mask_is_alloc[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.fes_state[%s%d].mask_is_alloc[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.fes_state[i0].mask_is_alloc[i1]);

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
dnx_algo_field_action_sw_ipmf3_context_state_dump(int  unit,  int  context_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_ipmf3_context_state_priority_dump(unit, context_state_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_ipmf3_context_state_priority_dump(int  unit,  int  context_state_idx_0,  int  priority_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_state_idx_0, I0 = context_state_idx_0 + 1;
    int i1 = priority_idx_0, I1 = priority_idx_0 + 1, org_i1 = priority_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_priority_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action ipmf3 context_state priority") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action ipmf3 context_state priority\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/ipmf3/context_state/priority.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.context_state[].priority[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[].priority[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[i0].priority
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[i0].priority), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[].priority[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[i0].priority[i1],
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","ipmf3.context_state[%s%d].priority[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->ipmf3.context_state[i0].priority[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_algo_field_action_sw_epmf_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_fes_state_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_context_state_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_dump(int  unit,  int  fes_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_fes_state_field_group_dump(unit, fes_state_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_dump(unit, fes_state_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_dump(unit, fes_state_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_fes_state_field_group_dump(int  unit,  int  fes_state_idx_0,  int  field_group_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = field_group_idx_0, I1 = field_group_idx_0 + 1, org_i1 = field_group_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action epmf fes_state field_group") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action epmf fes_state field_group\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/epmf/fes_state/field_group.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.fes_state[].field_group[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[].field_group[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].field_group
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].field_group), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[].field_group[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].field_group[i1],
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.fes_state[%s%d].field_group[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].field_group[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_algo_field_action_sw_epmf_fes_state_place_in_fg_dump(int  unit,  int  fes_state_idx_0,  int  place_in_fg_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = place_in_fg_idx_0, I1 = place_in_fg_idx_0 + 1, org_i1 = place_in_fg_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action epmf fes_state place_in_fg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action epmf fes_state place_in_fg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/epmf/fes_state/place_in_fg.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.fes_state[].place_in_fg[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[].place_in_fg[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].place_in_fg
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].place_in_fg), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[].place_in_fg[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.fes_state[%s%d].place_in_fg[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].place_in_fg[i1]);

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
dnx_algo_field_action_sw_epmf_fes_state_mask_is_alloc_dump(int  unit,  int  fes_state_idx_0,  int  mask_is_alloc_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = fes_state_idx_0, I0 = fes_state_idx_0 + 1;
    int i1 = mask_is_alloc_idx_0, I1 = mask_is_alloc_idx_0 + 1, org_i1 = mask_is_alloc_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action epmf fes_state mask_is_alloc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action epmf fes_state mask_is_alloc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/epmf/fes_state/mask_is_alloc.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.fes_state[].mask_is_alloc[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[].mask_is_alloc[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].mask_is_alloc
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].mask_is_alloc), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[].mask_is_alloc[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.fes_state[%s%d].mask_is_alloc[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.fes_state[i0].mask_is_alloc[i1]);

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
dnx_algo_field_action_sw_epmf_context_state_dump(int  unit,  int  context_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw_epmf_context_state_priority_dump(unit, context_state_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_field_action_sw_epmf_context_state_priority_dump(int  unit,  int  context_state_idx_0,  int  priority_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_state_idx_0, I0 = context_state_idx_0 + 1;
    int i1 = priority_idx_0, I1 = priority_idx_0 + 1, org_i1 = priority_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_action_priority_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_field_action epmf context_state priority") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_field_action epmf context_state priority\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_field_action_sw/epmf/context_state/priority.txt",
            "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.context_state[].priority[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state
                , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS)
        {
            LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[].priority[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[i0].priority
                    , sizeof(*((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[i0].priority), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT)
            {
                LOG_CLI((BSL_META("dnx_algo_field_action_sw[]->((dnx_algo_field_action_sw_t*)sw_state_roots_array[][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[].priority[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[i0].priority[i1],
                    "dnx_algo_field_action_sw[%d]->","((dnx_algo_field_action_sw_t*)sw_state_roots_array[%d][DNX_ALGO_FIELD_ACTION_MODULE_ID])->","epmf.context_state[%s%d].priority[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_algo_field_action_sw_t*)sw_state_roots_array[unit][DNX_ALGO_FIELD_ACTION_MODULE_ID])->epmf.context_state[i0].priority[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_algo_field_action_sw_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_FIELD_ACTION_SW_INFO_NOF_ENTRIES];
const char* dnx_algo_field_action_sw_layout_str[DNX_ALGO_FIELD_ACTION_SW_INFO_NOF_ENTRIES] = {
    "DNX_ALGO_FIELD_ACTION_SW~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~FES_STATE~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~FES_STATE~FIELD_GROUP~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~FES_STATE~PLACE_IN_FG~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~FES_STATE~MASK_IS_ALLOC~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~CONTEXT_STATE~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF2~CONTEXT_STATE~PRIORITY~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~FES_STATE~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~FES_STATE~FIELD_GROUP~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~FES_STATE~PLACE_IN_FG~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~FES_STATE~MASK_IS_ALLOC~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~CONTEXT_STATE~",
    "DNX_ALGO_FIELD_ACTION_SW~IPMF3~CONTEXT_STATE~PRIORITY~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~FES_STATE~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~FES_STATE~FIELD_GROUP~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~FES_STATE~PLACE_IN_FG~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~FES_STATE~MASK_IS_ALLOC~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~CONTEXT_STATE~",
    "DNX_ALGO_FIELD_ACTION_SW~EPMF~CONTEXT_STATE~PRIORITY~",
};
#endif 
#undef BSL_LOG_MODULE
