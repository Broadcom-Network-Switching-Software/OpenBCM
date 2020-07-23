/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 DEFS H
 */
#include <shared/bsl.h>
#include <soc/dfe/cmn/dfe_diag.h>
#include <soc/dfe/fe1600/fe1600_diag.h>
#include <soc/dfe/cmn/mbcm.h>
#include <sal/appl/io.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/cmn/dfe_defs.h>



#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG

void soc_dfe_counters_info_init(soc_dfe_counters_info_t* fe_counters_info)
{

    int i=0;
    
    for (i=0;i<SOC_DFE_MAX_NOF_PIPES;i++)
    {
        fe_counters_info->dch_counters_info[i].dch_total_in.value=0;
        fe_counters_info->dch_counters_info[i].dch_total_in.overflow=0;
        fe_counters_info->dch_counters_info[i].dch_total_out.value=0;
        fe_counters_info->dch_counters_info[i].dch_total_out.overflow=0;
        fe_counters_info->dch_counters_info[i].dch_fifo_discard.value=0;
        fe_counters_info->dch_counters_info[i].dch_fifo_discard.overflow=0;
        fe_counters_info->dch_counters_info[i].reorder_discard.value=0;
        fe_counters_info->dch_counters_info[i].reorder_discard.overflow=0;
        fe_counters_info->dch_counters_info[i].sum_dch_unreach_discard.value=0;
        fe_counters_info->dch_counters_info[i].sum_dch_unreach_discard.overflow=0;
        fe_counters_info->dch_counters_info[i].dch_max_q_size=0;
        
        fe_counters_info->dcl_counters_info[i].dcl_tot_in.value=0; 
        fe_counters_info->dcl_counters_info[i].dcl_tot_in.overflow=0;
        fe_counters_info->dcl_counters_info[i].dcl_tot_out.value=0;
        fe_counters_info->dcl_counters_info[i].dcl_tot_out.overflow=0;
        fe_counters_info->dcl_counters_info[i].dcl_dropped.value=0;   
        fe_counters_info->dcl_counters_info[i].dcl_dropped.overflow=0;
        fe_counters_info->dcl_counters_info[i].dcl_max=0;

        fe_counters_info->dcm_counters_info[i].dcma_tot=0;
        fe_counters_info->dcm_counters_info[i].dcma_drp=0;
        fe_counters_info->dcm_counters_info[i].dcma_max=0;
        fe_counters_info->dcm_counters_info[i].dcmb_tot=0;
        fe_counters_info->dcm_counters_info[i].dcmb_drp=0;
        fe_counters_info->dcm_counters_info[i].dcmb_max=0;

		fe_counters_info->dcm_counters_info[i].dcm_tot.value=0;
		fe_counters_info->dcm_counters_info[i].dcm_tot.overflow=0;
		fe_counters_info->dcm_counters_info[i].dcm_drop.value=0;
		fe_counters_info->dcm_counters_info[i].dcm_drop.overflow=0;
		fe_counters_info->dcm_counters_info[i].dcm_max=0;

		fe_counters_info->dch_dcm_fc[i].value=0;
		fe_counters_info->dch_dcm_fc[i].overflow=0;
		fe_counters_info->dcm_dcl_fc[i].value=0;
		fe_counters_info->dcm_dcl_fc[i].overflow=0;
    }

    
    fe_counters_info->ccs_counters_info.ccs_total_control.value=0;
    fe_counters_info->ccs_counters_info.ccs_total_control.overflow=0;
    fe_counters_info->ccs_counters_info.ccs_flow_stat.value=0;
    fe_counters_info->ccs_counters_info.ccs_flow_stat.overflow=0;
    fe_counters_info->ccs_counters_info.ccs_credits.value=0;
    fe_counters_info->ccs_counters_info.ccs_credits.overflow=0;
    fe_counters_info->ccs_counters_info.ccs_reachability.value=0;
    fe_counters_info->ccs_counters_info.ccs_reachability.overflow=0;
    fe_counters_info->ccs_counters_info.ccs_unreach_dest.value=0;
    fe_counters_info->ccs_counters_info.ccs_unreach_dest.overflow=0;



    fe_counters_info->nof_pipes=0;
    
}


void soc_dfe_queues_info_init(int unit, soc_dfe_queues_info_t* fe_queues_info)
{
    int i;
    int j;

    for (i=0;i<SOC_DFE_MAX_NOF_PIPES;i++)
    {
        for (j=0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dch);j++)
        {
            fe_queues_info->dch_queues_info[i].nof_dch[j].nof_link_a=0;
            fe_queues_info->dch_queues_info[i].nof_dch[j].nof_link_b=0;
			fe_queues_info->dch_queues_info[i].nof_dch[j].nof_link_c=0;
            fe_queues_info->dch_queues_info[i].nof_dch[j].value_a=0;
            fe_queues_info->dch_queues_info[i].nof_dch[j].value_b=0;
			fe_queues_info->dch_queues_info[i].nof_dch[j].value_c=0;

        }

		for (j=0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dcm); j++)
		{
            fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[0]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[1]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[2]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcm[j].nof_fifo[3]=0;
		}

		for (j=0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dcma); j++)
		{
			fe_queues_info->dcm_queues_info[i].nof_dcma[j].nof_fifo[0]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcma[j].nof_fifo[1]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcma[j].nof_fifo[2]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcma[j].nof_fifo[3]=0;
		}

		for (j=0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dcmb); j++)
		{
			fe_queues_info->dcm_queues_info[i].nof_dcmb[j].nof_fifo[0]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcmb[j].nof_fifo[1]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcmb[j].nof_fifo[2]=0;
			fe_queues_info->dcm_queues_info[i].nof_dcmb[j].nof_fifo[3]=0;
		}

		for (j=0; j < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); j++)
		{
			fe_queues_info->dcl_queues_info[i].nof_dcl[j].nof_link=0;
			fe_queues_info->dcl_queues_info[i].nof_dcl[j].value=0;
		}

    }
    fe_queues_info->nof_pipes=0;

}


const soc_dfe_diag_flag_str_t soc_dfe_diag_flag_str_fabric_cell_snake_test_stages[] =  {
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG,                  "REGISTER_CONFIG"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET,                 "GET_OUT_OF_RESET"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION,             "DATA_CELL_GENERATION"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION,          "CONTROL_CELL_GENERATION"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND,   "DATA_CELL_FILTER_WRITE_COMMAND"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND,"CONTROL_CELL_FILTER_WRITE_COMMAND"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND,    "DATA_CELL_FILTER_READ_COMMAND"},
    {SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND, "CONTROL_CELL_FILTER_READ_COMMAND"},
    {0x0,                                                               NULL},
};



soc_error_t
soc_dfe_diag_fabric_cell_snake_test_interrupts_name_get(int unit, const soc_dfe_diag_flag_str_t **intr_names)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_diag_fabric_cell_snake_test_interrupts_name_get, (unit, intr_names));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_dfe_diag_cell_pipe_counter_get(int unit, int pipe, uint64 *counter)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_diag_cell_pipe_counter_get, (unit, pipe, counter));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_diag_fabric_traffic_profile_get(int unit, int source_id, int dest_id, soc_dfe_diag_fabric_traffic_profile_t *traffic_profile)
{
    soc_dcmn_filter_type_t filter_type_arr[5];
    uint32 filter_val[5];
    int priority, count;
    uint32 filter_count = 0;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    filter_type_arr[0] = soc_dcmn_filter_type_priority;
    filter_type_arr[1] = soc_dcmn_filter_type_traffic_cast;
    
    filter_count = 2;
    if (source_id != -1)
    {
        filter_type_arr[filter_count] = soc_dcmn_filter_type_source_id;
        filter_val[filter_count] = source_id;
        filter_count++;
    }
    if (dest_id != -1)
    {
        filter_type_arr[filter_count] = soc_dcmn_filter_type_dest_id;
        filter_val[filter_count] = dest_id;
        filter_count++;
    }

    
    filter_val[1] = 0; 
    for (priority = 0; priority < soc_dfe_fabric_priority_nof; priority++)
    {
        filter_val[0] = priority;
        rv = soc_dfe_cell_filter_set(unit, SOC_CELL_FILTER_FLAG_DONT_TRAP, filter_count, filter_type_arr, filter_val);
        SOCDNX_IF_ERR_EXIT(rv);
        sal_usleep(30000);

        
        rv = soc_dfe_cell_filter_count_get(unit, &count);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(30000);

        rv = soc_dfe_cell_filter_count_get(unit, &count);
        SOCDNX_IF_ERR_EXIT(rv);

        traffic_profile->unicast[priority] = count ? 1 : 0;
    }

    
    filter_val[1] = 2; 
    for (priority = 0; priority < soc_dfe_fabric_priority_nof; priority++)
    {
        filter_val[0] = priority;
        rv = soc_dfe_cell_filter_set(unit, SOC_CELL_FILTER_FLAG_DONT_TRAP, filter_count, filter_type_arr, filter_val);
        SOCDNX_IF_ERR_EXIT(rv);
        sal_usleep(30000);

        
        rv = soc_dfe_cell_filter_count_get(unit, &count);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(30000);

        rv = soc_dfe_cell_filter_count_get(unit, &count);
        SOCDNX_IF_ERR_EXIT(rv);

        traffic_profile->multicast[priority] = count ? 1 : 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME

