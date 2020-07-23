
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF FABRIC CELL
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell.h>
#include <soc/dnxf/cmn/dnxf_fabric_source_routed_cell.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <bcm_int/control.h>
#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <soc/dnxc/dnxc_captured_buffer.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>


static void clear_data_out_strcute (soc_dnxc_captured_control_cell_t* data_out)
{
    sal_memset(data_out, 0 ,sizeof(soc_dnxc_captured_control_cell_t));

    data_out->dest_device = -1;
    data_out->source_device = -1;
    data_out->dest_port = -1;
    data_out->src_queue_num = -1;
    data_out->dest_queue_num = -1;
    data_out->sub_flow_id = -1;
    data_out->flow_id = -1;
    data_out->reachability_bitmap = -1;
    data_out->base_index = -1;
    data_out->source_link_number = -1;
}

int 
soc_dnxf_cell_filter_init(int unit) 
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_filter_init,(unit));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int 
soc_dnxf_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dnxc_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(filter_type_arr, _SHR_E_PARAM, "filter_type_arr");
    SHR_NULL_CHECK(filter_type_val, _SHR_E_PARAM, "filter_type_val");

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_filter_set,(unit, flags, array_size, filter_type_arr, filter_type_val));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int 
soc_dnxf_cell_filter_count_get(int unit, int *count)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(count, _SHR_E_PARAM, "count");

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_filter_count_get,(unit, count));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int 
soc_dnxf_cell_filter_clear(int unit)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_cell_filter_clear);
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int 
soc_dnxf_cell_filter_receive(int unit, dnxc_captured_cell_t* data_out)
{
    int rv, is_empty;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");
    DNXF_UNIT_LOCK_TAKE(unit);

    
    rv = dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty);
    SHR_IF_ERR_EXIT(rv);

    
    
    while (is_empty) 
    {
        rv = soc_dnxf_fabric_cell_load(unit);
        if (rv == _SHR_E_EMPTY)
        {
            break;
        }
        SHR_IF_ERR_EXIT(rv);

        rv = dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty);
        SHR_IF_ERR_EXIT(rv);
    }
    if (is_empty)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

    
    rv = dnxc_captured_buffer_get(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, data_out);
    SHR_IF_ERR_EXIT(rv);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}


int soc_dnxf_control_cell_filter_set(int unit, uint32 flags, soc_dnxc_control_cell_types_t cell_type, uint32 array_size, soc_dnxc_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val) 
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    if (array_size != 0) {
        SHR_NULL_CHECK(control_cell_filter_type_arr, _SHR_E_PARAM, "control_cell_filter_type_arr");
        SHR_NULL_CHECK(filter_type_val, _SHR_E_PARAM, "filter_type_val");
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_control_cell_filter_set,(unit, flags, cell_type, array_size, control_cell_filter_type_arr, filter_type_val));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}


int 
soc_dnxf_control_cell_filter_clear(int unit)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_control_cell_filter_clear);
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}



int 
soc_dnxf_control_cell_filter_receive(int unit, soc_dnxc_captured_control_cell_t* data_out)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");

    
    clear_data_out_strcute(data_out);

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_control_cell_filter_receive,(unit, data_out));
    SHR_IF_ERR_EXIT(rc);


exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}


static shr_error_e
soc_dnxf_fabric_cell_parse(int unit, soc_dnxc_fabric_cell_entry_t entry, soc_dnxc_fabric_cell_info_t *cell_info)
{
    int rv;
    soc_dnxc_fabric_cell_parse_table_t parse_table[soc_dnxc_fabric_cell_field_nof];
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_cell_parse_table_get, (unit, entry, &cell_info->cell_type, parse_table));
    SHR_IF_ERR_EXIT(rv);

    rv = soc_dnxc_fabric_cell_parser(unit, entry, parse_table, cell_info);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxf_fabric_cell_load(int unit)
{
    int rv;
    soc_dnxc_fabric_cell_entry_t entry;
    soc_dnxc_fabric_cell_info_t cell_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cell_info, 0x00, sizeof(soc_dnxc_fabric_cell_info_t));
    
    
    rv = soc_dnxf_fabric_cell_entry_get(unit, entry);
    if (rv == _SHR_E_EMPTY)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    
    rv = soc_dnxf_fabric_cell_parse(unit, entry, &cell_info);
    SHR_IF_ERR_EXIT(rv);

    
    if(cell_info.cell_type == soc_dnxc_fabric_cell_type_sr_cell) 
    {
        rv = dnxc_cells_buffer_add(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer, &cell_info.cell.sr_cell);
        SHR_IF_ERR_EXIT(rv);
    } 
    else 
    {
        rv = dnxc_captured_buffer_add(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &cell_info.cell.captured_cell);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT; 
}

shr_error_e
soc_dnxf_fabric_cell_entry_get(int unit, soc_dnxc_fabric_cell_entry_t entry)
{
    int rv;
    int channel;
    int found_cell = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit))
    {
        for (channel = 0; channel < dnxf_data_fabric.cell.fifo_dma_nof_channels_get(unit); channel++)
        {
            rv = soc_dnxf_fifo_dma_channel_entry_get(unit, channel, SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32 * sizeof(uint32), 
                                                    dnxf_data_fabric.cell.fifo_dma_fabric_cell_nof_entries_per_cell_get(unit), (uint8 *) entry);
            if (rv == _SHR_E_EMPTY)
            {
                
                continue;
            }
            SHR_IF_ERR_EXIT(rv);

            found_cell = 1;
            break;
        }
    } else {
        
        rv = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_cell_get,(unit, entry));
        if (rv == _SHR_E_EMPTY)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(rv);
        found_cell = 1;
    }
    if (!found_cell)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

