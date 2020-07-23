
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_apptype_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_apptype_sw_t * dnx_field_apptype_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_apptype_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_predef_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_predef_info_dump(int  unit,  int  predef_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_predef_info_nof_context_ids_dump(unit, predef_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_predef_info_context_ids_dump(unit, predef_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_predef_info_valid_for_kbp_dump(unit, predef_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_predef_info_nof_context_ids_dump(int  unit,  int  predef_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = predef_info_idx_0, I0 = predef_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype predef_info nof_context_ids") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype predef_info nof_context_ids\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/predef_info/nof_context_ids.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","predef_info[].nof_context_ids: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = bcmFieldAppTypeCount;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= bcmFieldAppTypeCount)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[].nof_context_ids: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(bcmFieldAppTypeCount == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","predef_info[%s%d].nof_context_ids: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[i0].nof_context_ids);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_predef_info_context_ids_dump(int  unit,  int  predef_info_idx_0,  int  context_ids_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = predef_info_idx_0, I0 = predef_info_idx_0 + 1;
    int i1 = context_ids_idx_0, I1 = context_ids_idx_0 + 1, org_i1 = context_ids_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype predef_info context_ids") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype predef_info context_ids\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/predef_info/context_ids.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","predef_info[].context_ids[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = bcmFieldAppTypeCount;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= bcmFieldAppTypeCount)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[].context_ids[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(bcmFieldAppTypeCount == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[i0].context_ids
                    , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[i0].context_ids), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
            {
                LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[].context_ids[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","predef_info[%s%d].context_ids[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[i0].context_ids[i1]);

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
dnx_field_apptype_sw_predef_info_valid_for_kbp_dump(int  unit,  int  predef_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = predef_info_idx_0, I0 = predef_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype predef_info valid_for_kbp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype predef_info valid_for_kbp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/predef_info/valid_for_kbp.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","predef_info[].valid_for_kbp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = bcmFieldAppTypeCount;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= bcmFieldAppTypeCount)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[].valid_for_kbp: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(bcmFieldAppTypeCount == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","predef_info[%s%d].valid_for_kbp: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->predef_info[i0].valid_for_kbp);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_opcode_id_dump(unit, user_def_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_acl_context_dump(unit, user_def_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_base_apptype_dump(unit, user_def_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_flags_dump(unit, user_def_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_name_dump(unit, user_def_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw_user_def_info_profile_id_dump(unit, user_def_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_opcode_id_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_def_info_idx_0, I0 = user_def_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype user_def_info opcode_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype user_def_info opcode_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/user_def_info/opcode_id.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[].opcode_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[].opcode_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[%s%d].opcode_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].opcode_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_acl_context_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_def_info_idx_0, I0 = user_def_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype user_def_info acl_context") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype user_def_info acl_context\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/user_def_info/acl_context.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[].acl_context: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[].acl_context: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[%s%d].acl_context: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].acl_context);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_base_apptype_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_def_info_idx_0, I0 = user_def_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype user_def_info base_apptype") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype user_def_info base_apptype\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/user_def_info/base_apptype.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[].base_apptype: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[].base_apptype: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[%s%d].base_apptype: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].base_apptype);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_flags_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_def_info_idx_0, I0 = user_def_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_apptype_flags_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype user_def_info flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype user_def_info flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/user_def_info/flags.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].flags,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[%s%d].flags: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].flags,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_name_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_def_info_idx_0, I0 = user_def_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_string_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype user_def_info name") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype user_def_info name\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/user_def_info/name.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[].name: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[].name: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[%s%d].name: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_STRING_STRING_PRINT(
                unit,
                DNX_FIELD_APPTYPE_MODULE_ID,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].name);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_apptype_sw_user_def_info_profile_id_dump(int  unit,  int  user_def_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = user_def_info_idx_0, I0 = user_def_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_apptype user_def_info profile_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_apptype user_def_info profile_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_apptype_sw/user_def_info/profile_id.txt",
            "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[].profile_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info
                , sizeof(*((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_apptype_sw[]->((dnx_field_apptype_sw_t*)sw_state_roots_array[][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[].profile_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_apptype_sw[%d]->","((dnx_field_apptype_sw_t*)sw_state_roots_array[%d][DNX_FIELD_APPTYPE_MODULE_ID])->","user_def_info[%s%d].profile_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_apptype_sw_t*)sw_state_roots_array[unit][DNX_FIELD_APPTYPE_MODULE_ID])->user_def_info[i0].profile_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_apptype_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_APPTYPE_SW_INFO_NOF_ENTRIES];
const char* dnx_field_apptype_sw_layout_str[DNX_FIELD_APPTYPE_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_APPTYPE_SW~",
    "DNX_FIELD_APPTYPE_SW~PREDEF_INFO~",
    "DNX_FIELD_APPTYPE_SW~PREDEF_INFO~NOF_CONTEXT_IDS~",
    "DNX_FIELD_APPTYPE_SW~PREDEF_INFO~CONTEXT_IDS~",
    "DNX_FIELD_APPTYPE_SW~PREDEF_INFO~VALID_FOR_KBP~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~OPCODE_ID~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~ACL_CONTEXT~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~BASE_APPTYPE~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~FLAGS~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~NAME~",
    "DNX_FIELD_APPTYPE_SW~USER_DEF_INFO~PROFILE_ID~",
};
#endif 
#undef BSL_LOG_MODULE
