
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/auto_generated/diagnostic/wb_engine_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern sw_state_wb_engine_t * sw_state_wb_engine_data[SOC_MAX_NUM_DEVICES];



int
sw_state_wb_engine_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(sw_state_wb_engine_buffer_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_dump(int  unit,  int  engine_id,  int  buffer_id,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(sw_state_wb_engine_buffer_instance_dump(unit, engine_id,buffer_id, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
sw_state_wb_engine_buffer_instance_dump(int  unit,  int  engine_id,  int  buffer_id,  int  instance_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = engine_id, I0 = engine_id + 1;
    int i1 = buffer_id, I1 = buffer_id + 1, org_i1 = buffer_id;
    int i2 = instance_idx_0, I2 = instance_idx_0 + 1, org_i2 = instance_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "DNX_SW_STATE_BUFF") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "wb_engine buffer instance") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate wb_engine buffer instance\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "sw_state_wb_engine/buffer/instance.txt",
            "sw_state_wb_engine[%d]->","((sw_state_wb_engine_t*)sw_state_roots_array[%d][WB_ENGINE_MODULE_ID])->","buffer[][].instance[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID]));

        if (i0 == -1) {
            I0 = SOC_WB_ENGINE_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer
                , sizeof(*((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= SOC_WB_ENGINE_NOF)
        {
            LOG_CLI((BSL_META("sw_state_wb_engine[]->((sw_state_wb_engine_t*)sw_state_roots_array[][WB_ENGINE_MODULE_ID])->buffer[][].instance[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(SOC_WB_ENGINE_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    WB_ENGINE_MODULE_ID,
                    ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0]
                    , sizeof(*((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, WB_ENGINE_MODULE_ID, ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0]))
            {
                LOG_CLI((BSL_META("sw_state_wb_engine[]->((sw_state_wb_engine_t*)sw_state_roots_array[][WB_ENGINE_MODULE_ID])->buffer[][].instance[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, WB_ENGINE_MODULE_ID, ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        WB_ENGINE_MODULE_ID,
                        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance
                        , sizeof(*((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, WB_ENGINE_MODULE_ID, ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance))
                {
                    LOG_CLI((BSL_META("sw_state_wb_engine[]->((sw_state_wb_engine_t*)sw_state_roots_array[][WB_ENGINE_MODULE_ID])->buffer[][].instance[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, WB_ENGINE_MODULE_ID, ((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "sw_state_wb_engine[%d]->","((sw_state_wb_engine_t*)sw_state_roots_array[%d][WB_ENGINE_MODULE_ID])->","buffer[%s%d][%s%d].instance[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint8(
                        unit,
                        &((sw_state_wb_engine_t*)sw_state_roots_array[unit][WB_ENGINE_MODULE_ID])->buffer[i0][i1].instance[i2]);

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





dnx_sw_state_diagnostic_info_t sw_state_wb_engine_info[SOC_MAX_NUM_DEVICES][SW_STATE_WB_ENGINE_INFO_NOF_ENTRIES];
const char* sw_state_wb_engine_layout_str[SW_STATE_WB_ENGINE_INFO_NOF_ENTRIES] = {
    "SW_STATE_WB_ENGINE~",
    "SW_STATE_WB_ENGINE~BUFFER~",
    "SW_STATE_WB_ENGINE~BUFFER~INSTANCE~",
};
#endif 
#endif  
#undef BSL_LOG_MODULE
