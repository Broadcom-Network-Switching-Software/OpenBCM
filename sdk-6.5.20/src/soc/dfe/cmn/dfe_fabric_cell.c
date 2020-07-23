
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE FABRIC CELL
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_fabric_cell.h>
#include <soc/dfe/cmn/dfe_fabric_source_routed_cell.h>
#include <soc/dfe/cmn/mbcm.h>
#include <bcm_int/control.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dcmn/dcmn_cells_buffer.h>
#include <soc/dcmn/dcmn_captured_buffer.h>


void clear_data_out_strcute (soc_dcmn_captured_control_cell_t* data_out)
{
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
soc_dfe_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(filter_type_arr);
    SOCDNX_NULL_CHECK(filter_type_val);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cell_filter_set,(unit, flags, array_size, filter_type_arr, filter_type_val));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_cell_filter_count_get(int unit, int *count)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(count);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cell_filter_count_get,(unit, count));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;

}

int 
soc_dfe_cell_filter_clear(int unit)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_cell_filter_clear);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_cell_filter_receive(int unit, dcmn_captured_cell_t* data_out)
{

    int rv, is_empty;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(data_out);
    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    
    rv = dcmn_captured_buffer_is_empty(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, &is_empty);
    SOCDNX_IF_ERR_EXIT(rv);

    
    
    while (is_empty) 
    {
        rv = soc_dfe_fabric_cell_load(unit);
        if (rv == SOC_E_EMPTY)
        {
            break;
        }
        SOCDNX_IF_ERR_EXIT(rv);

        rv = dcmn_captured_buffer_is_empty(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, &is_empty);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    if (is_empty)
    {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
    }

    
    rv = dcmn_captured_buffer_get(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, data_out);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


int soc_dfe_control_cell_filter_set(int unit, uint32 flags, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val) 
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (array_size != 0) {
        SOCDNX_NULL_CHECK(control_cell_filter_type_arr);
        SOCDNX_NULL_CHECK(filter_type_val);
    }

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_control_cell_filter_set,(unit, flags, cell_type, array_size, control_cell_filter_type_arr, filter_type_val));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


int 
soc_dfe_control_cell_filter_clear(int unit)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_control_cell_filter_clear);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}



int 
soc_dfe_control_cell_filter_receive(int unit, soc_dcmn_captured_control_cell_t* data_out)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(data_out);

    
    clear_data_out_strcute(data_out);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_control_cell_filter_receive,(unit, data_out));
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;

}



soc_error_t 
soc_dfe_fabric_cell_load(int unit)
{
    int rv;
    int is_two_parts = 0;
    soc_dcmn_fabric_cell_entry_t entry;
    soc_dcmn_fabric_cell_entry_t entry_2;
    soc_dcmn_fabric_cell_info_t cell_info;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&cell_info, 0x00, sizeof(soc_dcmn_fabric_cell_info_t));
    
    
    rv = soc_dfe_fabric_cell_entry_get(unit, entry);
    if (rv == SOC_E_EMPTY)
    {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
    }
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_cell_is_cell_in_two_parts, (unit, entry, &is_two_parts)));
    if (is_two_parts)
    {
        
        rv =  soc_dfe_fabric_cell_entry_get(unit, entry_2);
        SOCDNX_IF_ERR_EXIT(rv);  
    }
    
    rv = soc_dfe_fabric_cell_parse(unit, entry, entry_2, &cell_info, is_two_parts);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if(cell_info.cell_type == soc_dcmn_fabric_cell_type_sr_cell) 
    {
        rv = dcmn_cells_buffer_add(unit, &SOC_DFE_CONTROL(unit)->sr_cells_buffer, &cell_info.cell.sr_cell);
        SOCDNX_IF_ERR_EXIT(rv);
    } 
    else 
    {
        rv = dcmn_captured_buffer_add(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, &cell_info.cell.captured_cell);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t
soc_dfe_fabric_cell_entry_get(int unit, soc_dcmn_fabric_cell_entry_t entry)
{
    int rv;
    int channel;
    int found_cell = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable != SOC_DFE_PROPERTY_UNAVAIL && SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable)
    {
        for (channel = SOC_DFE_DEFS_GET(unit, fifo_dma_fabric_cell_first_channel);
              channel < SOC_DFE_DEFS_GET(unit, fifo_dma_nof_fabric_cell_channels);
              channel++)
        {
            rv = soc_dfe_fifo_dma_channel_entry_get(unit, channel, SOC_DCMN_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32 * sizeof(uint32), 
                                                    SOC_DFE_DEFS_GET(unit, fifo_dma_fabric_cell_nof_entries_per_cell), (uint8 *) entry);
            if (rv == SOC_E_EMPTY)
            {
                
                continue;
            }
            SOCDNX_IF_ERR_EXIT(rv);

            found_cell = 1;
            break;
        }
    } else {
        
        rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_cell_get,(unit, entry));
        if (rv == SOC_E_EMPTY)
        {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
        }
        SOCDNX_IF_ERR_EXIT(rv);
        found_cell = 1;
    }
    if (!found_cell)
    {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_dfe_fabric_cell_parse(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_entry_t entry_2, soc_dcmn_fabric_cell_info_t *cell_info, int is_two_parts)
{
    int rv;
    uint32 nof_lines;
    soc_dcmn_fabric_cell_parse_table_t parse_table[SOC_DCMN_FABRIC_CELL_PARSE_TABLE_MAX_NOF_LINES];
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_cell_type_get, (unit, entry, &cell_info->cell_type));
    SOCDNX_IF_ERR_EXIT(rv);

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_cell_parse_table_get, (unit, cell_info->cell_type, SOC_DCMN_FABRIC_CELL_PARSE_TABLE_MAX_NOF_LINES, parse_table, &nof_lines, is_two_parts));
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dcmn_fabric_cell_parser(unit, entry, entry_2, parse_table, nof_lines, cell_info, is_two_parts);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN; 
}


#undef _ERR_MSG_MODULE_NAME

