
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_cache_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_cach_shadow_t * dnx_field_tcam_cache_shadow_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_cache_shadow_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow_tcam_bank_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow_payload_table_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_cache_shadow_tcam_bank_dump(int  unit,  int  tcam_bank_idx_0,  int  tcam_bank_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow_tcam_bank_entry_key_dump(unit, tcam_bank_idx_0,tcam_bank_idx_1, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_cache_shadow_tcam_bank_entry_key_dump(int  unit,  int  tcam_bank_idx_0,  int  tcam_bank_idx_1,  int  entry_key_idx_0,  int  entry_key_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = tcam_bank_idx_0, I0 = tcam_bank_idx_0 + 1;
    int i1 = tcam_bank_idx_1, I1 = tcam_bank_idx_1 + 1, org_i1 = tcam_bank_idx_1;
    int i2 = entry_key_idx_0, I2 = entry_key_idx_0 + 1, org_i2 = entry_key_idx_0;
    int i3 = entry_key_idx_1, I3 = entry_key_idx_1 + 1, org_i3 = entry_key_idx_1;
    char *s0 = "", *s1 = "", *s2 = "", *s3 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_cache tcam_bank entry_key") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_cache tcam_bank entry_key\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_cache_shadow/tcam_bank/entry_key.txt",
            "dnx_field_tcam_cache_shadow[%d]->","((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_CACHE_MODULE_ID])->","tcam_bank[][].entry_key[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_CACHE_MODULE_ID,
                ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank
                , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[][].entry_key[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_CACHE_MODULE_ID,
                    ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0]
                    , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0]))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[][].entry_key[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_CACHE_MODULE_ID,
                        ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key
                        , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[][].entry_key[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    
                    
                        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                            unit,
                            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2]);

                    if (i3 == -1) {
                        I3 = dnx_sw_state_get_nof_elements(unit,
                            DNX_FIELD_TCAM_CACHE_MODULE_ID,
                            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2]);
                        i3 = dnx_sw_state_dump_check_all_the_same(unit,
                            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2]
                            , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2]), I3, &s3) ? I3 - 1 : 0;
                    }

                    if(i3 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2]))
                    {
                        LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[][].entry_key[][]: ")));
                        LOG_CLI((BSL_META("Invalid index: %d \n"),i3));
                        SHR_EXIT();
                    }

                    if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2]) == 0)
                    {
                        SHR_EXIT();
                    }

                    for(; i3 < I3; i3++) {
                        dnx_sw_state_dump_update_current_idx(unit, i3);
                        DNX_SW_STATE_PRINT_MONITOR(
                            unit,
                            "dnx_field_tcam_cache_shadow[%d]->","((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_CACHE_MODULE_ID])->","tcam_bank[%s%d][%s%d].entry_key[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        DNX_SW_STATE_PRINT_FILE(
                            unit,
                            "[%s%d][%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        dnx_sw_state_print_uint8(
                            unit,
                            &((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->tcam_bank[i0][i1].entry_key[i2][i3]);

                    }
                    i3 = org_i3;
                    dnx_sw_state_dump_end_of_stride(unit);
                }
                i2 = org_i2;
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_cache_shadow_payload_table_dump(int  unit,  int  payload_table_idx_0,  int  payload_table_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow_payload_table_entry_payload_dump(unit, payload_table_idx_0,payload_table_idx_1, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_cache_shadow_payload_table_entry_payload_dump(int  unit,  int  payload_table_idx_0,  int  payload_table_idx_1,  int  entry_payload_idx_0,  int  entry_payload_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = payload_table_idx_0, I0 = payload_table_idx_0 + 1;
    int i1 = payload_table_idx_1, I1 = payload_table_idx_1 + 1, org_i1 = payload_table_idx_1;
    int i2 = entry_payload_idx_0, I2 = entry_payload_idx_0 + 1, org_i2 = entry_payload_idx_0;
    int i3 = entry_payload_idx_1, I3 = entry_payload_idx_1 + 1, org_i3 = entry_payload_idx_1;
    char *s0 = "", *s1 = "", *s2 = "", *s3 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_cache payload_table entry_payload") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_cache payload_table entry_payload\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_cache_shadow/payload_table/entry_payload.txt",
            "dnx_field_tcam_cache_shadow[%d]->","((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_CACHE_MODULE_ID])->","payload_table[][].entry_payload[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_CACHE_MODULE_ID,
                ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table
                , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[][].entry_payload[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_CACHE_MODULE_ID,
                    ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0]
                    , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0]))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[][].entry_payload[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_CACHE_MODULE_ID,
                        ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload
                        , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[][].entry_payload[][]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    
                    
                        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                            unit,
                            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2]);

                    if (i3 == -1) {
                        I3 = dnx_sw_state_get_nof_elements(unit,
                            DNX_FIELD_TCAM_CACHE_MODULE_ID,
                            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2]);
                        i3 = dnx_sw_state_dump_check_all_the_same(unit,
                            ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2]
                            , sizeof(*((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2]), I3, &s3) ? I3 - 1 : 0;
                    }

                    if(i3 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2]))
                    {
                        LOG_CLI((BSL_META("dnx_field_tcam_cache_shadow[]->((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[][].entry_payload[][]: ")));
                        LOG_CLI((BSL_META("Invalid index: %d \n"),i3));
                        SHR_EXIT();
                    }

                    if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_CACHE_MODULE_ID, ((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2]) == 0)
                    {
                        SHR_EXIT();
                    }

                    for(; i3 < I3; i3++) {
                        dnx_sw_state_dump_update_current_idx(unit, i3);
                        DNX_SW_STATE_PRINT_MONITOR(
                            unit,
                            "dnx_field_tcam_cache_shadow[%d]->","((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_CACHE_MODULE_ID])->","payload_table[%s%d][%s%d].entry_payload[%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        DNX_SW_STATE_PRINT_FILE(
                            unit,
                            "[%s%d][%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2, s3, i3);

                        dnx_sw_state_print_uint8(
                            unit,
                            &((dnx_field_tcam_cach_shadow_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_CACHE_MODULE_ID])->payload_table[i0][i1].entry_payload[i2][i3]);

                    }
                    i3 = org_i3;
                    dnx_sw_state_dump_end_of_stride(unit);
                }
                i2 = org_i2;
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_cache_shadow_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_CACHE_SHADOW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_cache_shadow_layout_str[DNX_FIELD_TCAM_CACHE_SHADOW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_CACHE_SHADOW~",
    "DNX_FIELD_TCAM_CACHE_SHADOW~TCAM_BANK~",
    "DNX_FIELD_TCAM_CACHE_SHADOW~TCAM_BANK~ENTRY_KEY~",
    "DNX_FIELD_TCAM_CACHE_SHADOW~PAYLOAD_TABLE~",
    "DNX_FIELD_TCAM_CACHE_SHADOW~PAYLOAD_TABLE~ENTRY_PAYLOAD~",
};
#endif 
#undef BSL_LOG_MODULE
