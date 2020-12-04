/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fabric.c
 * Purpose:     fabric CLI commands
 */

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/dcmn/fabric.h>

#include <bcm/error.h>
#include <bcm/fabric.h>
#include <bcm/debug.h>

#include <bcm_int/dpp/utils.h>

#include <soc/dpp/mbcm.h>

STATIC cmd_result_t _cmd_dpp_tdm_edit(int unit, args_t *args);

#define CMD_TDM_EDIT_USAGE \
    "    edit [show] [port=<port>]\n"     \
    "        - Show tdm editing info with given port\n"               

char cmd_dpp_tdm_usage[] =
    "\n"
    CMD_TDM_EDIT_USAGE  "\n"
    ;

static cmd_t _cmd_dpp_tdm_list[] = {
    {"EDIT",   _cmd_dpp_tdm_edit,  "\n" CMD_TDM_EDIT_USAGE,  NULL}
};

STATIC int
_tdm_editing_print(int unit, bcm_gport_t gport, bcm_fabric_tdm_editing_t *editing)
{
    char type_str[20];
    char flags_str[60] = "";
    int print_uni = 0, print_mul = 0, print_userfield = 0;	
    int i = 0;

    switch (editing->type){
      case bcmFabricTdmEditingAppend:
          sal_strncpy(type_str, "Append", 20); 
          print_userfield = 1;
          break;
      case bcmFabricTdmEditingRemove:
          sal_strncpy(type_str, "Remove", 20); 
          break;
      case bcmFabricTdmEditingNoChange:
          sal_strncpy(type_str, "Nochange", 20); 
          break;
      case bcmFabricTdmEditingCustomExternal:
          sal_strncpy(type_str, "CustomExternal", 20); 
          break;  
      default:
          return BCM_E_NONE;
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) {
        sal_sprintf(flags_str, "%s", "Ingress");
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS) {
        sal_sprintf(flags_str, "%s", "Egress");
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {
        sal_sprintf(flags_str + sal_strlen(flags_str), "%s", ",Unicast");
        print_uni = 1;
    }
	
    if (editing->flags & BCM_FABRIC_TDM_EDITING_MULTICAST) {
        sal_sprintf(flags_str + sal_strlen(flags_str), "%s", ",Multicast");
        print_mul = 1;
    }

    if (editing->flags & BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC) {
        sal_sprintf(flags_str + sal_strlen(flags_str), "%s", ",Asymmetric_CRC");
    }

    cli_out("Port:%d flags:%s type:%s add_crc:%s", gport, flags_str, type_str, (editing->add_packet_crc ? "Y":"N")); 
    if (print_uni)
        cli_out(" Dest_Port:0x%8.8x", editing->destination_port);
    if (print_mul)
        cli_out(" MC:%d", editing->multicast_id);
    if (print_userfield) {
        cli_out(" User_Field_count:%d", editing->user_field_count);
        cli_out(" User_Field:0x");
        for (i = 0; i < BCM_FABRIC_TDM_USER_FIELD_MAX_SIZE; i++)
            cli_out("%2.2x", editing->user_field[i]);	
    }
    cli_out("\n");
    return BCM_E_NONE;
}

STATIC cmd_result_t
_cmd_dpp_tdm_edit(int unit, args_t *args)
{
    cmd_result_t       ret_code;
    int                rv = BCM_E_NONE;
    parse_table_t      pt;
    char	      *subcmd;
    bcm_port_t     port = 0;
    bcm_fabric_tdm_editing_t editing;
    int            one_entry_only = 0;    
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }
    
    if (sal_strcasecmp(subcmd, "show") == 0) {
        if (ARG_CNT(args) > 0) {			
            /* Parse option */
            parse_table_init(0, &pt);
            parse_table_add(&pt, "Port", PQ_DFL | PQ_INT, 0, (void *)&port, 0);
            if (!parseEndOk(args, &pt, &ret_code)) {
                return ret_code;
            }
            one_entry_only = 1;
        }
		
        if (one_entry_only) {
            /* Get ingress */
            bcm_fabric_tdm_editing_t_init(&editing);			
            editing.flags = BCM_FABRIC_TDM_EDITING_INGRESS;

            rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
            if (BCM_FAILURE(rv)) {
                cli_out("%s: Error reading fabric tdm editing: %s\n",
                        ARG_CMD(args), bcm_errmsg(rv));
                return (CMD_FAIL);
            }
			
            _tdm_editing_print(unit, port, &editing);
			
            /* Get egress */
            bcm_fabric_tdm_editing_t_init(&editing);			
            editing.flags = BCM_FABRIC_TDM_EDITING_EGRESS;

            rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
            if (BCM_FAILURE(rv)) {
                cli_out("%s: Error reading fabric tdm editing: %s\n",
                        ARG_CMD(args), bcm_errmsg(rv));
                return (CMD_FAIL);
            }
			
            _tdm_editing_print(unit, port, &editing);
        } 
        else {
            BCM_PBMP_ITER(PBMP_TDM_ALL(unit), port) {
                /* Get ingress */
                bcm_fabric_tdm_editing_t_init(&editing);			
                editing.flags = BCM_FABRIC_TDM_EDITING_INGRESS;

                rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
                if (BCM_FAILURE(rv)) {
                    cli_out("%s: Error reading fabric tdm editing: %s\n",
                            ARG_CMD(args), bcm_errmsg(rv));
                    return (CMD_FAIL);
                }
				
                _tdm_editing_print(unit, port, &editing);

                /* Get egress */
                bcm_fabric_tdm_editing_t_init(&editing);			
                editing.flags = BCM_FABRIC_TDM_EDITING_EGRESS;

                rv = bcm_fabric_tdm_editing_get(unit, port, &editing);
                if (BCM_FAILURE(rv)) {
                    cli_out("%s: Error reading fabric tdm editing: %s\n",
                            ARG_CMD(args), bcm_errmsg(rv));
                    return (CMD_FAIL);
                }
				
                _tdm_editing_print(unit, port, &editing);				
            }
        }

    } 

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}


cmd_result_t
cmd_dpp_tdm(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
        _cmd_dpp_tdm_list, COUNTOF(_cmd_dpp_tdm_list));
}

cmd_result_t
diag_dpp_fabric_thresholds_types_parse(int unit, char *th_name, diag_dnx_fabric_link_th_info_t *thresholds_info)
{
    int strnlen_th_name;
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_th_name = sal_strnlen(th_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if (!sal_strncasecmp(th_name, "RCI", strnlen_th_name)) {
        thresholds_info->th_types_rx_nof = 3;
        thresholds_info->th_types_rx[0] = bcmFabricLinkRxRciLvl1FC;
        thresholds_info->th_types_rx[1] = bcmFabricLinkRxRciLvl2FC;
        thresholds_info->th_types_rx[2] = bcmFabricLinkRxRciLvl3FC;
    } else if (!sal_strncasecmp(th_name, "LLFC", strnlen_th_name)) {
        thresholds_info->th_types_rx_nof = 1;
        thresholds_info->th_types_rx[0] = bcmFabricLinkRxFifoLLFC;
    } else {
        cli_out("Threshold type is not supported (%s).\n", th_name);
        return CMD_USAGE;
    }
    return CMD_OK;
}
  
  
cmd_result_t
diag_dpp_fabric_thresholds_threshold_to_str(int unit, bcm_fabric_link_threshold_type_t type, char **type_name)
{
    switch (type)
    {
        /*LLFC*/
        case bcmFabricLinkRxFifoLLFC:
            *type_name = "LLFC";
            break;                                            
        /*RCI*/
        case bcmFabricLinkRxRciLvl1FC:
            *type_name = "RCI1";
            break;
        case bcmFabricLinkRxRciLvl2FC:
            *type_name = "RCI2";
            break;
        case bcmFabricLinkRxRciLvl3FC:
            *type_name = "RCI3";
            break;
        default:
            *type_name = "unknown";
            break;
    }
    return CMD_OK;
}
  
cmd_result_t
diag_dpp_fabric_queues_print(int unit)
{
    int rv, i;
    soc_dpp_fabric_queues_info_t queues_info;

    sal_memset(&queues_info, 0, sizeof(queues_info));

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_queues_info_get,(unit, &queues_info));
    if (BCM_FAILURE(rv)){
        return CMD_FAIL;
    }

    /* Print all valid queues diagnostics */

    /********************************** print DTQ *********************************/
    if (queues_info.soc_dpp_fabric_dtq_data_max_occ_val.is_valid && queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_val.is_valid) {

        cli_out("\n\nData read from DRAM to IPT Queues:\n\n");
        cli_out("=========\n\n");

        if (queues_info.soc_dpp_fabric_dtq_data_max_occ_val.info) {
            cli_out("Data cells: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dtq_data_max_occ_val.info, queues_info.soc_dpp_fabric_dtq_data_max_occ_ctx.info);
        }

        if (queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_val.info) {
            cli_out("Control cells: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_val.info, queues_info.soc_dpp_fabric_dtq_ctrl_max_occ_ctx.info);
        }
    }

    if (queues_info.soc_dpp_fabric_dtq_info[0].is_valid) { /*if this diagnostic is valid then queue 0 is valid (for all cases: single FAP/MESH/CLOS with different amount of pipes)*/
        cli_out("\n\nFabric DTQ Data: even queues are SRAM, odd are DRAM:\n\n");
        cli_out("=========\n\n");

        for(i = 0; i < (sizeof(queues_info.soc_dpp_fabric_dtq_info)/sizeof(*(queues_info.soc_dpp_fabric_dtq_info))); ++i) {
            if(queues_info.soc_dpp_fabric_dtq_info[i].is_valid && queues_info.soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_max_occ_val) { /*if max is 0- other fields will be 0 too*/
                cli_out("\nDTQ %d:\n", i);
                cli_out("Amount of words: %u\n", queues_info.soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_occ_val);
                cli_out("Minimum amount of words: %u\n", queues_info.soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_min_occ_val);
                cli_out("Maximum amount of words: %u\n", queues_info.soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_max_occ_val);
            }
        }
    }

    if (queues_info.soc_dpp_local_dtq_info[0].is_valid) {
        cli_out("\n\nLocal DTQ Data: even queues are SRAM, odd are DRAM:\n\n");
        cli_out("=========\n\n");

        for (i = 0; i < (sizeof(queues_info.soc_dpp_local_dtq_info)/sizeof(*(queues_info.soc_dpp_local_dtq_info))); ++i) {
            if(queues_info.soc_dpp_local_dtq_info[i].soc_dpp_dtq_max_occ_val) {
                cli_out("\nDTQ %d:\n", i);
                cli_out("Amount of words: %u\n", queues_info.soc_dpp_local_dtq_info[i].soc_dpp_dtq_occ_val);
                cli_out("Minimum amount of words: %u\n", queues_info.soc_dpp_local_dtq_info[i].soc_dpp_dtq_min_occ_val);
                cli_out("Maximum amount of words: %u\n", queues_info.soc_dpp_local_dtq_info[i].soc_dpp_dtq_max_occ_val);
            }
        }
    }

    /********************************** print DBLF/DQCF *********************************/
    if (queues_info.soc_dpp_fabric_dblf0_max_occ_val.is_valid && queues_info.soc_dpp_fabric_dblf1_max_occ_val.is_valid) {
        cli_out("\n\nDRAM buffer pointers Queues:\n\n");
        cli_out("=========\n\n");

        if (queues_info.soc_dpp_fabric_dblf0_max_occ_val.info) {
            cli_out("DBLF 0: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dblf0_max_occ_val.info, queues_info.soc_dpp_fabric_dblf0_max_occ_ctx.info);
        }
        if (queues_info.soc_dpp_fabric_dblf1_max_occ_val.info) {
            cli_out("DBLF 1: Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dblf1_max_occ_val.info, queues_info.soc_dpp_fabric_dblf1_max_occ_ctx.info);
        }
    }

    if (queues_info.soc_dpp_fabric_sram_dqcf_max_occ_val.is_valid && queues_info.soc_dpp_fabric_dram_dblf_max_occ_val.is_valid) {

        cli_out("\n\nSRAM Dequeue Command FIFOs:\n\n");
        cli_out("=========\n\n");
        if (queues_info.soc_dpp_fabric_sram_dqcf_max_occ_val.info) {
            cli_out("Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_sram_dqcf_max_occ_val.info, queues_info.soc_dpp_fabric_sram_dqcf_max_occ_ctx.info);
        }

        cli_out("\n\nDRAM buffer pointers Queues:\n\n");
        cli_out("=========\n\n");
        if (queues_info.soc_dpp_fabric_dram_dblf_max_occ_val.info) {
            cli_out("Max occupancy %u in context %u\n", queues_info.soc_dpp_fabric_dram_dblf_max_occ_val.info, queues_info.soc_dpp_fabric_dram_dblf_max_occ_ctx.info);
        }
    }

    return CMD_OK;
}
