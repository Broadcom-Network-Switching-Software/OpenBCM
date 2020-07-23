/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNX FABRIC DIAG
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_APPL_FABRIC


#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/fabric.h>

#include <appl/diag/shell.h>

#include <appl/diag/dcmn/fabric.h>
#include <appl/diag/dcmn/diag.h>

#include <soc/dcmn/error.h>

#ifdef BCM_DFE_SUPPORT

#include <bcm_int/dfe/dfe_fifo_type.h>

#include <soc/dfe/cmn/dfe_diag.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_drv.h>


STATIC int
diag_dfe_fabric_counters_seperator_print(int unit, int nof_pipes)
{
	int i;

	SOCDNX_INIT_FUNC_DEFS;

	cli_out("#--------------------------");
	for (i = 0 ; i < nof_pipes * 45  ; i++ )
	{

		cli_out("-");
	}

	cli_out("#\n");

	SOCDNX_FUNC_RETURN;
}

STATIC int
diag_dfe_fabric_counters_header_print(int unit, char header[40], int nof_pipes)
{
	int i;

	SOCDNX_INIT_FUNC_DEFS;

	cli_out("# %-25s|", header);
	for (i = 0 ; i < nof_pipes * 45 - 1; i++ )
	{
		cli_out(" ");
	} 

	cli_out("#\n");
	SOCDNX_FUNC_RETURN;
}

cmd_result_t
diag_dfe_fabric_counters_print(int unit){
    
	int i;
    int nof_pipes;
    int rv;
    soc_dfe_counters_info_t fe_counters_info;


    soc_dfe_counters_info_init(&fe_counters_info);
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_counters_get_info,(unit,&fe_counters_info));
    if (BCM_FAILURE(rv))
    {
        return CMD_FAIL;
    }
    nof_pipes=fe_counters_info.nof_pipes;

    
    /* printing proccess*/

    /* Heading printing */
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    
    cli_out("#\t\t\t   ");

    for (i = 0 ; i < nof_pipes ; i++)
    {
        cli_out("|                   Pipe %1d                   ", i);
    }
    cli_out("#\n");
    
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);

    /* DCH printing*/
                                                                                           
    diag_dfe_fabric_counters_header_print(unit, "DCH:", nof_pipes);

    cli_out("#     %-20s", "Total Incoming Cells");

    for (i = 0; i < nof_pipes; i++ )
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_counters_info[i].dch_total_in.value, fe_counters_info.dch_counters_info[i].dch_total_in.overflow? "(Overflow)":"");
    }

    cli_out(" #\n");
   

    cli_out("#     %-20s", "Total Outgoing Cells");

    for (i = 0; i < nof_pipes; i++ )
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_counters_info[i].dch_total_out.value, fe_counters_info.dch_counters_info[i].dch_total_out.overflow? "(Overflow)":"");
    }

    cli_out(" #\n");
    

    cli_out("#     %-20s", "Fifo Discard");

    for (i = 0; i < nof_pipes; i++ )
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_counters_info[i].dch_fifo_discard.value, fe_counters_info.dch_counters_info[i].dch_fifo_discard.overflow? "(Overflow)":"");
    }

    cli_out(" #\n");


    cli_out("#     %-20s", "Reorder Discard");

    for (i = 0; i < nof_pipes; i++ )
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_counters_info[i].reorder_discard.value, fe_counters_info.dch_counters_info[i].reorder_discard.overflow? "(Overflow)":"");
    }

    cli_out(" #\n");


    cli_out("#     %-20s", "Unreach Discard");

    for (i = 0; i < nof_pipes; i++ )
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_counters_info[i].sum_dch_unreach_discard.value, fe_counters_info.dch_counters_info[i].sum_dch_unreach_discard.overflow? "(Overflow)":"");
    }

    cli_out(" #\n");

  
    cli_out("#     %-20s", "Max Cells in Fifos");

    for (i = 0; i < nof_pipes; i++ )
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_counters_info[i].dch_max_q_size, "");
    }

    cli_out(" #\n");		
          
                       
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);

    /* DCM printing*/
                                                                                                
    diag_dfe_fabric_counters_header_print(unit, "DCM:", nof_pipes);

    if (SOC_IS_FE1600(unit))
    {
        diag_dfe_fabric_counters_header_print(unit, "--DCMA--:", nof_pipes);

        cli_out("#     %-20s", "Total Incoming Cells");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcma_tot, "");
        }
        cli_out(" #\n");


        cli_out("#     %-20s", "Dropped Cells");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcma_drp, "");
        }
        cli_out(" #\n");


        cli_out("#     %-20s", "Max Cells in Fifos");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcma_max, "");
        }
        cli_out(" #\n");


        diag_dfe_fabric_counters_header_print(unit, "--DCMB--:", nof_pipes);

        cli_out("#     %-20s", "Total Incoming Cells");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcmb_tot, "");
        }
        cli_out(" #\n");


        cli_out("#     %-20s", "Dropped Cells");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcmb_drp, "");
        }
        cli_out(" #\n");

        cli_out("#     %-20s", "Max Cells in Fifos");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcmb_max, "");
        }
        cli_out(" #\n");

    }
    else
    {
        cli_out("#     %-20s", "Total Incoming Cells");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcm_tot.value, fe_counters_info.dcm_counters_info[i].dcm_tot.overflow? "(Overflow)":"");
        }
        cli_out(" #\n");

        cli_out("#     %-20s", "Dropped Cells");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcm_drop.value, fe_counters_info.dcm_counters_info[i].dcm_drop.overflow? "(Overflow)":"");
        }
        cli_out(" #\n");

        cli_out("#     %-20s", "Max Cells in Fifos");
        for (i = 0 ; i < nof_pipes; i++)
        {
            cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_counters_info[i].dcm_max, "");
        }
        cli_out(" #\n");

    }

    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    
    /* DCL printing*/
    diag_dfe_fabric_counters_header_print(unit, "DCL:", nof_pipes);

    cli_out("#     %-20s", "Total Incoming Cells");
    for (i = 0 ; i < nof_pipes; i++)
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcl_counters_info[i].dcl_tot_in.value, fe_counters_info.dcl_counters_info[i].dcl_tot_in.overflow? "(Overflow)":"");
    }
    cli_out(" #\n");

    cli_out("#     %-20s", "Total Outgoing Cells");
    for (i = 0 ; i < nof_pipes; i++)
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcl_counters_info[i].dcl_tot_out.value, fe_counters_info.dcl_counters_info[i].dcl_tot_out.overflow? "(Overflow)":"");
    }
    cli_out(" #\n");	
    
    cli_out("#     %-20s", "Dropped Cells");
    for (i = 0 ; i < nof_pipes; i++)
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcl_counters_info[i].dcl_dropped.value, fe_counters_info.dcl_counters_info[i].dcl_dropped.overflow? "(Overflow)":"");
    }
    cli_out(" #\n");	
    
    cli_out("#     %-20s", "Max Cells in Fifos");
    for (i = 0 ; i < nof_pipes; i++)
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcl_counters_info[i].dcl_max, "");
    }
    cli_out(" #\n");				     


    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);

    /*Flow Control printing*/ 

    diag_dfe_fabric_counters_header_print(unit, "Flow Control:", nof_pipes);
   
    cli_out("#     %-20s", "DCH to DCM FC");
    for (i = 0 ; i < nof_pipes; i++)
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dch_dcm_fc[i].value, fe_counters_info.dch_dcm_fc[i].overflow? "(Overflow)":"");
    }
    cli_out(" #\n");

    cli_out("#     %-20s", "DCM to DCL FC");
    for (i = 0 ; i < nof_pipes; i++)
    {
        cli_out(" |                    %-10u %-10s  ", fe_counters_info.dcm_dcl_fc[i].value, fe_counters_info.dcm_dcl_fc[i].overflow? "(Overflow)":"");
    }
    cli_out(" #\n");


    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);

    /* Control Cells printing*/

    diag_dfe_fabric_counters_header_print(unit, "Control Cells(All Pipes):", nof_pipes);

    cli_out("#     %-20s", "Total Control Cells");
    cli_out(" |                    %-10u %-10s  ", fe_counters_info.ccs_counters_info.ccs_total_control.value, fe_counters_info.ccs_counters_info.ccs_total_control.overflow? "(Overflow)":"");
    for (i=1 ; i < nof_pipes; i ++)
    {
        cli_out(" |                    %-21s  ", "X");
    }
    cli_out(" #\n");

    cli_out("#     %-20s", "Credits");
    cli_out(" |                    %-10u %-10s  ", fe_counters_info.ccs_counters_info.ccs_credits.value, fe_counters_info.ccs_counters_info.ccs_credits.overflow? "(Overflow)":"");
    for (i=1 ; i < nof_pipes; i ++)
    {
        cli_out(" |                    %-21s  ", "X");
    }
    cli_out(" #\n");

    cli_out("#     %-20s", "Flow Status");
    cli_out(" |                    %-10u %-10s  ", fe_counters_info.ccs_counters_info.ccs_flow_stat.value, fe_counters_info.ccs_counters_info.ccs_flow_stat.overflow? "(Overflow)":"");
    for (i=1 ; i < nof_pipes; i ++)
    {
        cli_out(" |                    %-21s  ", "X");
    }
    cli_out(" #\n");

    cli_out("#     %-20s", "Reachability");
    cli_out(" |                    %-10u %-10s  ", fe_counters_info.ccs_counters_info.ccs_reachability.value, fe_counters_info.ccs_counters_info.ccs_reachability.overflow? "(Overflow)":"");
    for (i=1 ; i < nof_pipes; i ++)
    {
        cli_out(" |                    %-21s  ", "X");
    }
    cli_out(" #\n");

    cli_out("#     %-20s", "Unreachable");
    cli_out(" |                    %-10u %-10s  ", fe_counters_info.ccs_counters_info.ccs_unreach_dest.value, fe_counters_info.ccs_counters_info.ccs_unreach_dest.overflow? "(Overflow)":"");
    for (i=1 ; i < nof_pipes; i ++)
    {
        cli_out(" |                    %-21s  ", "X");
    }
    cli_out(" #\n");

        
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);
    diag_dfe_fabric_counters_seperator_print(unit, nof_pipes);

    return CMD_OK;

}

cmd_result_t
diag_dfe_fabric_queues_print(int unit)
{

    uint32 nof_pipes;
    int    i,j,k, rv;
    soc_dfe_queues_info_t fe_queues_info;

    soc_dfe_queues_info_init(unit, &fe_queues_info);

    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_queues_get_info,(unit, &fe_queues_info));
    if (BCM_FAILURE(rv))
    {
        return CMD_FAIL;
    }

    nof_pipes= fe_queues_info.nof_pipes;

    /* DCH print */

    cli_out("\n\nDCH Queues:\n\n");
    cli_out("=========\n\n");

    for (i=0;i<nof_pipes;i++)
    {
        for (j=0;j<SOC_DFE_DEFS_GET(unit, nof_instances_dch);j++)
        {
            if (fe_queues_info.dch_queues_info[i].nof_dch[j].value_a)
            {
                cli_out("DCH%d Pipe %d -A: [%u,%u]\n",j, i, fe_queues_info.dch_queues_info[i].nof_dch[j].nof_link_a,fe_queues_info.dch_queues_info[i].nof_dch[j].value_a);
            }
            if (fe_queues_info.dch_queues_info[i].nof_dch[j].value_b)
            {
                cli_out("DCH%d Pipe %d -B: [%u,%u]\n",j, i, fe_queues_info.dch_queues_info[i].nof_dch[j].nof_link_b,fe_queues_info.dch_queues_info[i].nof_dch[j].value_b);
            }
            if (fe_queues_info.dch_queues_info[i].nof_dch[j].value_c)
            {
                cli_out("DCH%d Pipe %d- C: [%u,%u]\n",j, i, fe_queues_info.dch_queues_info[i].nof_dch[j].nof_link_c,fe_queues_info.dch_queues_info[i].nof_dch[j].value_c);
            }
        }
    }
    
    /* DCMA-DCMB\ DCM Printing */

    if (SOC_IS_FE1600(unit))
    {
        cli_out("\nDCMA Queues:\n\n");
        cli_out("=========\n\n");

        for (i=0;i<nof_pipes;i++)
        {
            for (k=0;k<SOC_DFE_DEFS_GET(unit, nof_instances_dcma);k++)
            {
                for (j=0;j<SOC_DFE_NOF_FIFOS_PER_DCM;j++)
                {
                    if (fe_queues_info.dcm_queues_info[i].nof_dcma[k].nof_fifo[j])
                    {
                        cli_out("DCMA%d Pipe %d- Fifo %c: %u\n", k, i, j+'a', fe_queues_info.dcm_queues_info[i].nof_dcma[k].nof_fifo[j]);
                    }
                }
            }
            
        }

        cli_out("\nDCMB Queues:\n\n");
        cli_out("=========\n\n");

        for (i=0;i<nof_pipes;i++) /*pipe*/
        {
            for (k=0;k<SOC_DFE_DEFS_GET(unit, nof_instances_dcmb);k++) 
            {
                for (j=0;j<SOC_DFE_NOF_FIFOS_PER_DCM;j++)
                {
                    if (fe_queues_info.dcm_queues_info[i].nof_dcmb[k].nof_fifo[j])
                    {

                        cli_out("DCMB%d Pipe %d- Fifo %c: %u\n", k, i, j+'a', fe_queues_info.dcm_queues_info[i].nof_dcmb[k].nof_fifo[j]);
                    }
                }
            }
            
        }
    }
    else /* not FE1600 */
    {
        cli_out("\nDCM Queues:\n\n");
        cli_out("=========\n\n");

        for (i = 0; i < nof_pipes ; i++)
        {
            for (k = 0; k < SOC_DFE_DEFS_GET(unit, nof_instances_dcm); k++)
            {
                for (j=0; j < SOC_DFE_NOF_FIFOS_PER_DCM; j++)
                {
                    if (fe_queues_info.dcm_queues_info[i].nof_dcm[k].nof_fifo[j])
                    {
                        cli_out("DCM%d Pipe %d- Fifo %c: %u\n", k, i, j+'a', fe_queues_info.dcm_queues_info[i].nof_dcm[k].nof_fifo[j]);
                    }
                }
            }
        }
    } 

    /* DCL print */

    cli_out("\nDCL Queues:\n\n");
    cli_out("=========\n\n");

    for (i=0; i<nof_pipes; i++)
    {
        for (j=0; j<SOC_DFE_DEFS_GET(unit, nof_instances_dcl); j++)
        {
            if (fe_queues_info.dcl_queues_info[i].nof_dcl[j].value)
            {
                cli_out("DCL%d Pipe %d : [%u,%u]\n", j, i, fe_queues_info.dcl_queues_info[i].nof_dcl[j].nof_link, fe_queues_info.dcl_queues_info[i].nof_dcl[j].value);
            }
        }
    }

    return CMD_OK;
}
  
    
void
diag_dfe_fabric_cell_snake_test_result_print(
    int unit, 
    soc_fabric_cell_snake_test_results_t* results)
{
    int i,
        rv;
    
    cli_out("Fabric snake test results\n");
    cli_out("-------------------------\n");

    /*print test status*/
    cli_out("Result: %s\n", results->test_failed ? "fail" : "pass");

    /*print failure stages*/
    if (results->failure_stage_flags)
    {
        cli_out("Fail stages (0x%x):", results->failure_stage_flags);
        for (i=0; soc_dfe_diag_flag_str_fabric_cell_snake_test_stages[i].name != NULL; i++)
        {
            if (soc_dfe_diag_flag_str_fabric_cell_snake_test_stages[i].flag & results->failure_stage_flags)
            {
                cli_out("%s ",  soc_dfe_diag_flag_str_fabric_cell_snake_test_stages[i].name);
            }
        }
        cli_out("\n");
    }

    /*print interrupts*/
    if (!SHR_BITNULL_RANGE(results->interrupts_status, 0, 32*SOC_REG_ABOVE_64_MAX_SIZE_U32))
    {
        const soc_dfe_diag_flag_str_t *interrupts_names;

        cli_out("Interrupts (0x%0x, 0x%0x, 0x%0x): ", results->interrupts_status[2], results->interrupts_status[1], results->interrupts_status[0]);

        rv = soc_dfe_diag_fabric_cell_snake_test_interrupts_name_get(unit, &interrupts_names);
        if (interrupts_names == NULL || rv != BCM_E_NONE)
        {
            return;
        }
        for (i=0; interrupts_names[i].name != NULL; i++)
        {
            if (SHR_BITGET(results->interrupts_status, interrupts_names[i].flag))
            {
                cli_out("%s ",  interrupts_names[i].name);
            }
        }
        cli_out("\n");
    }


    /*print LFSRs per pipe*/
    for (i = 0; i<SOC_DFE_MAX_NOF_PIPES; i++)
    {
        if (results->lfsr_per_pipe[i] != 0)
        {
            cli_out("LFSR_PIPE_%d=0x%x\n", i, results->lfsr_per_pipe[i]);
        }
    }
    cli_out("-------------------------\n\n");

}

cmd_result_t
diag_dfe_fabric_thresholds_supported_get(int unit, bcm_port_t port, bcm_fabric_link_threshold_type_t type, int *supported)
{
    soc_dfe_fabric_link_device_mode_t link_mode;
    int rv;

    *supported = 1;
    /*rx device mode filter*/
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, port, 1, &link_mode));
    if (BCM_FAILURE(rv))
    {
        return CMD_FAIL;
    }

    if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
    {
        switch (type)
        {
            case bcmFabricLinkRxRciLvl1FC:
            case bcmFabricLinkRxRciLvl2FC:
            case bcmFabricLinkRxRciLvl3FC:
                *supported = 0;
                break;
            default:
                break;
        }
    }
    if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe3)
    {
        switch (type)
        {
            case bcmFabricLinkRxGciLvl1FC:
            case bcmFabricLinkRxGciLvl2FC:
            case bcmFabricLinkRxGciLvl3FC:
                *supported = 0;
                break;
            default:
                break;
        }
    }

    /*tx device mode filter*/
    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, port, 0, &link_mode));
    if (BCM_FAILURE(rv))
    {
        return CMD_FAIL;
    }
    if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe3)
    {
        switch (type)
        {
            case bcmFabricLinkFE1TxBypassLLFC:
                *supported = 0;
                break;
            default:
                break;
        }
    }
       
    return CMD_OK; 
}
cmd_result_t
diag_dfe_fabric_thresholds_types_parse(int unit, char *th_name, diag_dnx_fabric_link_th_info_t *thresholds_info)
{
    int strnlen_th_name;
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_th_name = sal_strnlen(th_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(th_name, "RCI", strnlen_th_name))
    {
        if (!SOC_IS_FE1600(unit))
        {
            /*RX*/
            if (SOC_DFE_IS_FE13(unit))
            {
                thresholds_info->th_types_rx_nof = 3;
                thresholds_info->th_types_rx[0] = bcmFabricLinkRxRciLvl1FC;
                thresholds_info->th_types_rx[1] = bcmFabricLinkRxRciLvl2FC;
                thresholds_info->th_types_rx[2] = bcmFabricLinkRxRciLvl3FC;
            }

            /*TX*/
            thresholds_info->th_types_tx_nof = 3;
            thresholds_info->th_types_tx[0] = bcmFabricLinkTxRciLvl1FC;
            thresholds_info->th_types_tx[1] = bcmFabricLinkTxRciLvl2FC;
            thresholds_info->th_types_tx[2] = bcmFabricLinkTxRciLvl3FC;

            /*Middle*/
            thresholds_info->th_types_middle_nof = 3;
            thresholds_info->th_types_middle[0] = bcmFabricLinkMidRciLvl1FC;
            thresholds_info->th_types_middle[1] = bcmFabricLinkMidRciLvl2FC;
            thresholds_info->th_types_middle[2] = bcmFabricLinkMidRciLvl3FC;
        } else { /*FE1600*/ 
            /*TX*/
            thresholds_info->th_types_tx_nof = 1;
            thresholds_info->th_types_tx[0] = bcmFabricLinkRciFC;
        }
    } else if (!sal_strncasecmp(th_name, "GCI", strnlen_th_name)) {
        /*RX*/
        thresholds_info->th_types_rx_nof = 3;
        thresholds_info->th_types_rx[0] = bcmFabricLinkRxGciLvl1FC;
        thresholds_info->th_types_rx[1] = bcmFabricLinkRxGciLvl2FC;
        thresholds_info->th_types_rx[2] = bcmFabricLinkRxGciLvl3FC;

        /*TX*/
        thresholds_info->th_types_tx_nof = 3;
        thresholds_info->th_types_tx[0] = bcmFabricLinkTxGciLvl1FC;
        thresholds_info->th_types_tx[1] = bcmFabricLinkTxGciLvl2FC;
        thresholds_info->th_types_tx[2] = bcmFabricLinkTxGciLvl3FC;

        /*Middle*/
        if (!SOC_IS_FE1600(unit))
        {
            thresholds_info->th_types_middle_nof = 3;
            thresholds_info->th_types_middle[0] = bcmFabricLinkMidGciLvl1FC;
            thresholds_info->th_types_middle[1] = bcmFabricLinkMidGciLvl2FC;
            thresholds_info->th_types_middle[2] = bcmFabricLinkMidGciLvl3FC;
        }
    } else if (!sal_strncasecmp(th_name, "DROP", strnlen_th_name)) {
        /*RX*/
        if (SOC_IS_FE1600(unit))
        {
            thresholds_info->th_types_rx_nof = 4;
            thresholds_info->th_types_rx[0] = bcmFabricLinkRxPrio0Drop;
            thresholds_info->th_types_rx[1] = bcmFabricLinkRxPrio1Drop;
            thresholds_info->th_types_rx[2] = bcmFabricLinkRxPrio2Drop;
            thresholds_info->th_types_rx[3] = bcmFabricLinkRxPrio3Drop;
        }

        /*TX*/
        thresholds_info->th_types_tx_nof = 4;
        thresholds_info->th_types_tx[0] = bcmFabricLinkTxPrio0Drop;
        thresholds_info->th_types_tx[1] = bcmFabricLinkTxPrio1Drop;
        thresholds_info->th_types_tx[2] = bcmFabricLinkTxPrio2Drop;
        thresholds_info->th_types_tx[3] = bcmFabricLinkTxPrio3Drop;

        /*Middle*/
        if (!SOC_IS_FE1600(unit))
        {
            thresholds_info->th_types_middle_nof = 4;
            thresholds_info->th_types_middle[0] = bcmFabricLinkMidPrio0Drop;
            thresholds_info->th_types_middle[1] = bcmFabricLinkMidPrio1Drop;
            thresholds_info->th_types_middle[2] = bcmFabricLinkMidPrio2Drop;
            thresholds_info->th_types_middle[3] = bcmFabricLinkMidPrio3Drop;
        }
    } else if (!sal_strncasecmp(th_name, "LLFC", strnlen_th_name)) {
        /*RX*/
        thresholds_info->th_types_rx_nof = 1;
        thresholds_info->th_types_rx[0] = bcmFabricLinkRxFifoLLFC;

        /*TX*/
        if (SOC_DFE_IS_FE13(unit))
        {
            thresholds_info->th_types_tx_nof = 1;
            thresholds_info->th_types_tx[0] = bcmFabricLinkFE1TxBypassLLFC;
        }

    } else if (!sal_strncasecmp(th_name, "ALMOST_FULL", strnlen_th_name)) {
        if (!SOC_IS_FE1600(unit))
        {
            /*TX*/
            thresholds_info->th_types_tx_nof = 1;
            thresholds_info->th_types_tx[0] = bcmFabricLinkTxAlmostFull;

            /*Middle*/
            thresholds_info->th_types_middle_nof = 1;
            thresholds_info->th_types_middle[0] = bcmFabricLinkMidAlmostFull;
        }
    } else if (!sal_strncasecmp(th_name, "SIZE", strnlen_th_name)) {
        if (!SOC_IS_FE1600(unit))
        {
            /*RX*/
            thresholds_info->th_types_rx_nof = 1;
            thresholds_info->th_types_rx[0] = bcmFabricLinkRxFifoSize;
            /*TX*/
            thresholds_info->th_types_tx_nof = 1;
            thresholds_info->th_types_tx[0] = bcmFabricLinkTxFifoSize;

            /*Middle*/
            thresholds_info->th_types_middle_nof = 1;
            thresholds_info->th_types_middle[0] = bcmFabricLinkMidFifoSize;
        }
    } else if (!sal_strncasecmp(th_name, "FULL", strnlen_th_name)) {
        if (!SOC_IS_FE1600(unit))
        {
            /*RX*/
            thresholds_info->th_types_rx_nof = 1;
            thresholds_info->th_types_rx[0] = bcmFabricLinkRxFull;

            /*Middle*/
            thresholds_info->th_types_middle_nof = 1;
            thresholds_info->th_types_middle[0] = bcmFabricLinkMidFull;
        }
    } else {
        cli_out("Threshold type is not supported (%s).\n", th_name);
        return CMD_USAGE;
    }

    return CMD_OK;
}

cmd_result_t
diag_dfe_fabric_thresholds_threshold_to_str(int unit, bcm_fabric_link_threshold_type_t type, char **type_name)
{
    switch (type)
    {
        /*LLFC*/
        case bcmFabricLinkRxFifoLLFC:
            *type_name = "LLFC";
            break;
       case bcmFabricLinkFE1TxBypassLLFC:
            *type_name = "FE1_LLFC";
            break;
                                               
        /*RCI*/    
        case bcmFabricLinkRciFC:
            *type_name = "RCI";
            break;
        case bcmFabricLinkTxRciLvl1FC:
        case bcmFabricLinkRxRciLvl1FC:
        case bcmFabricLinkMidRciLvl1FC:
            *type_name = "RCI1";
            break;
        case bcmFabricLinkTxRciLvl2FC:
        case bcmFabricLinkRxRciLvl2FC:
        case bcmFabricLinkMidRciLvl2FC:
            *type_name = "RCI2";
            break;
        case bcmFabricLinkTxRciLvl3FC:
        case bcmFabricLinkRxRciLvl3FC:
        case bcmFabricLinkMidRciLvl3FC:
            *type_name = "RCI3";
            break;

        /*GCI*/
        case bcmFabricLinkTxGciLvl1FC:
        case bcmFabricLinkRxGciLvl1FC:
        case bcmFabricLinkMidGciLvl1FC:
            *type_name = "GCI1";
            break;
        case bcmFabricLinkTxGciLvl2FC:
        case bcmFabricLinkRxGciLvl2FC:
        case bcmFabricLinkMidGciLvl2FC:
            *type_name = "GCI2";
            break;
        case bcmFabricLinkTxGciLvl3FC:
        case bcmFabricLinkRxGciLvl3FC:
        case bcmFabricLinkMidGciLvl3FC:
            *type_name = "GCI3";
            break;

        /*DROP*/
        case bcmFabricLinkTxPrio0Drop:
        case bcmFabricLinkRxPrio0Drop:
        case bcmFabricLinkMidPrio0Drop:
            *type_name = "DROP0";
            break;
        case bcmFabricLinkTxPrio1Drop:
        case bcmFabricLinkRxPrio1Drop:
        case bcmFabricLinkMidPrio1Drop:
            *type_name = "DROP1";
            break;
        case bcmFabricLinkTxPrio2Drop:
        case bcmFabricLinkRxPrio2Drop:
        case bcmFabricLinkMidPrio2Drop:
            *type_name = "DROP2";
            break;
        case bcmFabricLinkTxPrio3Drop:
        case bcmFabricLinkRxPrio3Drop:
        case bcmFabricLinkMidPrio3Drop:
            *type_name = "DROP3";
            break;
        case bcmFabricLinkRxMcLowPrioDrop:
            *type_name = "DROPMC";
            break;

        /*SIZE*/
        case bcmFabricLinkRxFifoSize:
        case bcmFabricLinkTxFifoSize:
        case bcmFabricLinkMidFifoSize:
            *type_name = "SIZE";
            break;

        /*ALMOST_FULL*/
        case bcmFabricLinkMidAlmostFull:
        case bcmFabricLinkTxAlmostFull:
            *type_name = "ALMOST_FULL";
            break;

        /*FULL*/
        case bcmFabricLinkRxFull:
            *type_name = "FULL";
            break;

        default:
            *type_name = "unknown";
            break;
    }

    return CMD_OK;
}


#endif /*BCM_DFE_SUPPORT*/

#undef _ERR_MSG_MODULE_NAME


