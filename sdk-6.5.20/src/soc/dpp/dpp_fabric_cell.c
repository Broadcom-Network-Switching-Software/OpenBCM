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

#include <soc/dpp/dpp_fabric_cell.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_fabric_cell.h>

static
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
soc_dpp_cell_filter_set(int unit, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{

    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The device doesn't support FDR")));
    }

    SOCDNX_NULL_CHECK(filter_type_arr);
    SOCDNX_NULL_CHECK(filter_type_val);

    rc = soc_arad_cell_filter_set(unit, array_size, filter_type_arr, filter_type_val);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int 
soc_dpp_cell_filter_clear(int unit)
{

    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The device doesn't support FDR")));
    }

     rc = soc_arad_cell_filter_clear(unit);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dpp_cell_filter_receive(int unit, dcmn_captured_cell_t* data_out)
{

    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The device doesn't support FDR")));
    }

    SOCDNX_NULL_CHECK(data_out);

    rc = soc_arad_cell_filter_receive(unit, data_out);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}


int soc_dpp_control_cell_filter_set(int unit, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val) 
{

    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The device doesn't support FDR")));
    }

    SOCDNX_NULL_CHECK(control_cell_filter_type_arr);
    SOCDNX_NULL_CHECK(filter_type_val);

    rc = soc_arad_control_cell_filter_set(unit, cell_type, array_size, control_cell_filter_type_arr, filter_type_val);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int 
soc_dpp_control_cell_filter_clear(int unit)
{
 
   int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The device doesn't support FDR")));
    }

    rc = soc_arad_control_cell_filter_clear(unit);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}



int 
soc_dpp_control_cell_filter_receive(int unit, soc_dcmn_captured_control_cell_t* data_out)
{

    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The device doesn't support FDR")));
    }

    SOCDNX_NULL_CHECK(data_out);

    
    clear_data_out_strcute(data_out);

    rc = soc_arad_control_cell_filter_receive(unit, data_out);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME
