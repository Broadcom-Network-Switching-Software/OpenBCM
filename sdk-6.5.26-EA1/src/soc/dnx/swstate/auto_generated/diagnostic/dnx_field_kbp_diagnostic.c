
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_kbp_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_kbp_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_kbp_sw_t * dnx_field_kbp_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_kbp_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_dump(int  unit,  int  opcode_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_master_key_info_dump(unit, opcode_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_is_valid_dump(unit, opcode_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_dump(int  unit,  int  opcode_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_dump(unit, opcode_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_dump(unit, opcode_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_dump(int  unit,  int  opcode_info_idx_0,  int  segment_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_dump(unit, opcode_info_idx_0, segment_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_dump(unit, opcode_info_idx_0, segment_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_dump(int  unit,  int  opcode_info_idx_0,  int  segment_info_idx_0,  int  qual_idx_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = opcode_info_idx_0, I0 = opcode_info_idx_0 + 1;
    int i1 = segment_info_idx_0, I1 = segment_info_idx_0 + 1, org_i1 = segment_info_idx_0;
    int i2 = qual_idx_idx_0, I2 = qual_idx_idx_0 + 1, org_i2 = qual_idx_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_kbp opcode_info master_key_info segment_info qual_idx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_kbp opcode_info master_key_info segment_info qual_idx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_kbp_sw/opcode_info/master_key_info/segment_info/qual_idx.txt",
            "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[].master_key_info.segment_info[].qual_idx[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__QUAL_IDX,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info
                , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__QUAL_IDX, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info))
        {
            LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].master_key_info.segment_info[].qual_idx[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__QUAL_IDX, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info
                    , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].master_key_info.segment_info[].qual_idx[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info[i1].qual_idx
                        , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info[i1].qual_idx), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT)
                {
                    LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].master_key_info.segment_info[].qual_idx[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[%s%d].master_key_info.segment_info[%s%d].qual_idx[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint8(
                        unit,
                        &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info[i1].qual_idx[i2]);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_dump(int  unit,  int  opcode_info_idx_0,  int  segment_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = opcode_info_idx_0, I0 = opcode_info_idx_0 + 1;
    int i1 = segment_info_idx_0, I1 = segment_info_idx_0 + 1, org_i1 = segment_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_group_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_kbp opcode_info master_key_info segment_info fg_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_kbp opcode_info master_key_info segment_info fg_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_kbp_sw/opcode_info/master_key_info/segment_info/fg_id.txt",
            "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[].master_key_info.segment_info[].fg_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__FG_ID,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info
                , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__FG_ID, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info))
        {
            LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].master_key_info.segment_info[].fg_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__SEGMENT_INFO__FG_ID, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info
                    , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].master_key_info.segment_info[].fg_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_FIELD_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info[i1].fg_id,
                    "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[%s%d].master_key_info.segment_info[%s%d].fg_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.segment_info[i1].fg_id,
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
dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_dump(int  unit,  int  opcode_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = opcode_info_idx_0, I0 = opcode_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_kbp opcode_info master_key_info nof_fwd_segments") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_kbp opcode_info master_key_info nof_fwd_segments\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_kbp_sw/opcode_info/master_key_info/nof_fwd_segments.txt",
            "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[].master_key_info.nof_fwd_segments: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__NOF_FWD_SEGMENTS,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info
                , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__NOF_FWD_SEGMENTS, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info))
        {
            LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].master_key_info.nof_fwd_segments: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__MASTER_KEY_INFO__NOF_FWD_SEGMENTS, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[%s%d].master_key_info.nof_fwd_segments: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].master_key_info.nof_fwd_segments);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_kbp_sw_opcode_info_is_valid_dump(int  unit,  int  opcode_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = opcode_info_idx_0, I0 = opcode_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_kbp opcode_info is_valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_kbp opcode_info is_valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_kbp_sw/opcode_info/is_valid.txt",
            "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[].is_valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__IS_VALID,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info
                , sizeof(*((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__IS_VALID, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info))
        {
            LOG_CLI((BSL_META("dnx_field_kbp_sw[]->((dnx_field_kbp_sw_t*)sw_state_roots_array[][DNX_FIELD_KBP_MODULE_ID])->opcode_info[].is_valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_KBP_SW__OPCODE_INFO__IS_VALID, ((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_kbp_sw[%d]->","((dnx_field_kbp_sw_t*)sw_state_roots_array[%d][DNX_FIELD_KBP_MODULE_ID])->","opcode_info[%s%d].is_valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_kbp_sw_t*)sw_state_roots_array[unit][DNX_FIELD_KBP_MODULE_ID])->opcode_info[i0].is_valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_kbp_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_KBP_SW_INFO_NOF_ENTRIES];
const char* dnx_field_kbp_sw_layout_str[DNX_FIELD_KBP_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_KBP_SW~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~MASTER_KEY_INFO~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~MASTER_KEY_INFO~SEGMENT_INFO~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~MASTER_KEY_INFO~SEGMENT_INFO~QUAL_IDX~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~MASTER_KEY_INFO~SEGMENT_INFO~FG_ID~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~MASTER_KEY_INFO~NOF_FWD_SEGMENTS~",
    "DNX_FIELD_KBP_SW~OPCODE_INFO~IS_VALID~",
};
#endif 
#endif  
#endif  
#undef BSL_LOG_MODULE
