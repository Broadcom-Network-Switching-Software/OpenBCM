/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <appl/diag/shell.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/port.h>

#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/eyescan.h>
#include <soc/phy/phyctrl.h>
#include <phymod/phymod_diag.h>


/*FIXME: remove this!*/
#define MAX_PORTS (500)
#define START_PORT (1)
#define MAX_PMS (66)
#define SUB_PHYS_NUM (4)
#define MAX_PHYS_PER_PORT (24)


typedef cmd_result_t (*portmod_diag_action_func_ptr_t)(int unit, args_t* a);
typedef void (*portmod_diag_usage_func_ptr_t)(int unit);
typedef int (*portmod_diag_id_supported_func_ptr_t)(int unit, int diag_id);

/*!
 * @struct portmod_port_diag_status_s
 * @brief This structure return port diagnostics information from the PMM 
 */ 
typedef struct portmod_port_diag_status_s {
    int enable; /**< indicate port is enabled or disabled */
    int link; /**< indicate link status */
    int loopback; /**< indicate loopback status */
    int lf; /**< indicate local fault status */
    int rf; /**< indicate remote fault status */
    portmod_port_interface_config_t config;
    portmod_port_pcs_t encoding;
    phymod_autoneg_status_t an_status; /**< auto-negotiation status */
    portmod_llfc_control_t llfc_control; /**< llfc control status */
    portmod_pfc_control_t pfc_control; /**< pfc control status */
    portmod_pfc_config_t pfc_config; /**< pfc config status */
    portmod_port_ability_t local_ability; /**< local_ability */
    portmod_port_ability_t adv_ability; /**< adv_ability */
    portmod_port_ability_t remote_ability; /**< remote_ability */
    portmod_pbmp_t phys; /**< The PHYs that assemble the port */
    int sub_phy; /**< qsgmii only; Sub PHY index */
} portmod_port_diag_status_t;


/*!
 * @struct portmod_port_diag_fc_s
 * @brief This structure return port diagnostics flow control status from the PMM 
 */ 
typedef struct portmod_port_diag_fc_s {
    portmod_llfc_control_t llfc_ctrl; /**< llfc control status */
    portmod_pfc_control_t pfc_ctrl; /**< pfc control status */
    portmod_pfc_config_t pfc_cfg; /**< pfc config status */
} portmod_port_diag_fc_t;


/*!
 * @struct portmod_port_diag_autoneg_s
 * @brief This structure return port diagnostics auto-negotiation status from the PMM 
 */ 
typedef struct portmod_port_diag_autoneg_s {
    portmod_port_ability_t local_ability; /**< local_ability */
    portmod_port_ability_t adv_ability; /**< adv_ability */
    portmod_port_ability_t remote_ability; /**< remote_ability */
} portmod_port_diag_autoneg_t;


/*!
 * @struct portmod_port_diag_prbs_s
 * @brief This structure for prbs diagnostics including config/enable/status 
 */ 
typedef struct portmod_port_diag_prbs_s {
    int enable; /**< prbs enable */
    int flags; /**< prbs direction */
    int mode; /**< phy or mac */
    char *cmd; /**< set/get/clear */
    int interval; /**< sleep interval */
    phymod_prbs_t prbs_config; /**< prbs config */
    phymod_prbs_status_t prbs_status; /**< prbs status */
} portmod_port_diag_prbs_t;


typedef struct portmod_diag_table_s {
    char* cmd_name; /*Command name*/
    portmod_diag_action_func_ptr_t action_func; /*Callback to command fuction*/
    char* cmd_short_desc; /*shortly describe the command*/
    portmod_diag_usage_func_ptr_t usage_func; /*Callback to command usage function*/
    int diag_id; /*diag id - used in order to make sure this command is supported by the device*/
} portmod_diag_table_t;



char portmod_diag_usage[] = "portmod <options>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
     "  usage                      -display detailed portmod diagnostic usage\n"
     "  info port[=port-id]        -display port detailed or summary SW information\n"
     "  info pm[=pm_id]]           -display pm detailed or summary SW information\n"
     "  status port[=port-id]      -display port detailed or summary HW status\n"
     "  fc port=<port-id>          -display PFC/LLFC information of the specific port\n"
     "  autoneg port=<port-id>     -display auto-negotiation ability of the specific port\n"
     "  core port=<port-id>        -display core information which the specific port belongs to\n"
     "  phy port=<port-id>         -display phy/lane information the specific port includes\n"
     "  prbs set port=<port-id> [mode=<phy|mac> polynomial=<value> Loopback=<true|false> Invert=<value>]\n"
     "                             -set PRBS generator on the port-id specific port\n"
     "  prbs get port=<port-id>    -get internal checker on the port-id specific port\n"
     "  prbs clear port=<port-id>  -clear internal PRBS configuration on the port-id specific port\n"
     "  eyescan port=<port-id> [vertical_max=<value> vertical_min=<value> vertical_step=<value>\n"
     "          horizontal_max=<value> horizontal_min=<value> horizontal_step=<value> \n"
     "          sample_time=<value> ber=<value> lane=<value> mode=<fast/lowBER>]\n"
     "                             -run eyescan test\n"
     "  regdump port=<port-id      -dump all registers of the sepecific port\n"
#endif
     ;
#ifdef PORTMOD_DIAG

STATIC cmd_result_t
enum_value_to_string(enum_mapping_t *mapping,  int val, char **str)
{
    char *key;
    int i = 0;

    key = mapping[0].key;
    while (key != NULL) {
        if (val == mapping[i].enum_value) {
            *str = mapping[i].key;
            return CMD_OK;
        }
        i++;
        key = mapping[i].key;
    }
    return CMD_FAIL;
}


STATIC cmd_result_t
portmod_port_diag_status_get(int unit, int port, portmod_port_diag_status_t *diag_status){
    int ret;
    portmod_port_interface_config_t config;
    portmod_port_diag_info_t diag_info;
    uint32 properties = 0;

    if(NULL == diag_status){
        cli_out("diag_status is null pointer");
        return CMD_FAIL;
    }
    sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));
    sal_memset(&config, 0, sizeof(portmod_port_interface_config_t));
    ret = portmod_port_interface_config_get(unit, port, &config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY);
    if(ret != SOC_E_NONE){
        cli_out("ERROR: Getting port interface config failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    }
    ret = portmod_port_encoding_get(unit, port, &properties, &diag_status->encoding);

    if(ret != SOC_E_NONE){
        cli_out("ERROR: Getting port encoding failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    }
    
    ret = portmod_port_link_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &diag_status->link);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port link failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }  
    ret = portmod_port_autoneg_status_get(unit, port, &diag_status->an_status);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port auto-negociation status failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }
    ret = portmod_port_loopback_get(unit, port, portmodLoopbackPhyRloopPMD, &diag_status->loopback);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port loopback status failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }  
    ret = portmod_port_enable_get(unit, port, 0, &diag_status->enable);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port enable status failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }  
    ret = portmod_port_local_fault_status_get(unit, port, &diag_status->lf);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port local fault status failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }  
    ret = portmod_port_remote_fault_status_get(unit, port, &diag_status->rf);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port remote fault status failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }  

    /*get port-phy mapping*/
    ret = portmod_port_diag_info_get(unit, port, &diag_info);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port info failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }  
    sal_memcpy(&diag_status->phys, &diag_info.phys, sizeof(portmod_pbmp_t));
    
    return CMD_OK;
}

STATIC cmd_result_t
portmod_port_diag_fc_get(int unit, int port, portmod_port_diag_fc_t* diag_fc){
    int ret;

    if(NULL == diag_fc){
        cli_out("diag_fc is null pointer");  
        return CMD_FAIL;
    }

    /* get flow control status */
    ret = portmod_port_llfc_control_get(unit, port, &diag_fc->llfc_ctrl);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port llfc control failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 
    ret = portmod_port_pfc_control_get(unit, port, &diag_fc->pfc_ctrl);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port pfc control failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 
    ret = portmod_port_pfc_config_get(unit, port, &diag_fc->pfc_cfg);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting port pfc config: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 

    return CMD_OK;

}

STATIC cmd_result_t
portmod_port_diag_autoneg_get(int unit, int port, portmod_port_diag_autoneg_t* diag_autoneg){
    int ret;

    if(NULL == diag_autoneg){
        cli_out("diag_autoneg is null pointer");
        return CMD_FAIL;
    }
    
    /* get flow control status */
    ret = portmod_port_ability_local_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &diag_autoneg->local_ability);
    if(ret != SOC_E_NONE){
        cli_out("ERROR: Getting port local ability failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    }
    ret = portmod_port_ability_advert_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &diag_autoneg->adv_ability);
    if(ret != SOC_E_NONE){
        cli_out("ERROR: Getting port advertisement ability failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    }
    ret = portmod_port_ability_remote_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &diag_autoneg->remote_ability);
    if(ret != SOC_E_NONE){
        cli_out("ERROR: Getting port recieved ability failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    }

    return CMD_OK;
}

STATIC cmd_result_t
portmod_port_diag_core_info_get(int unit, int port, phymod_core_diagnostics_t* diag_core, phymod_core_firmware_info_t* diag_firmware){
    int ret, nof_cores;
    phymod_core_access_t core_access;
    portmod_access_get_params_t access_param;

    if(NULL == diag_core){
        cli_out("diag_core is null pointer");  
        return CMD_FAIL;
    }
    if(NULL == diag_firmware){
        cli_out("diag_firmware is null pointer");  
        return CMD_FAIL;
    }

    /* get phy polority and nof_phys */
    ret = portmod_access_get_params_t_init(unit, &access_param);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Access parameters init failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 
    ret = portmod_port_main_core_access_get(unit, port, -1, &core_access, &nof_cores);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Core access get failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 
    ret = phymod_core_diagnostics_get(&core_access, diag_core);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting core diag failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }   
    ret = phymod_core_firmware_info_get(&core_access, diag_firmware);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Getting core firmware info failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 

    return CMD_OK;
}

STATIC cmd_result_t
portmod_port_diag_phy_info_get(int unit, int port, int phyn, int lane, int port_loc, phymod_phy_diagnostics_t* diag_phy){
    int ret, nof_phys;
    phymod_phy_access_t phy_access;
    phymod_phy_access_t phy_access12[3];
    portmod_access_get_params_t access_param;

    if(NULL == diag_phy){
        cli_out("diag_phy is null pointer");  
        return CMD_FAIL;
    }

    /* get phy polority and nof_phys */
    ret = portmod_access_get_params_t_init(unit, &access_param);
    access_param.phyn = phyn;
    access_param.lane = lane;
    access_param.sys_side = port_loc;

    if(ret != SOC_E_NONE){
        cli_out("ERROR: Access parameters init failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    } 
    if (IS_C_PORT(unit, port)) {
        ret = portmod_port_phy_lane_access_get(unit, port, &access_param,
                                               3, &phy_access12[0], 
                                               &nof_phys, NULL);
    } else {
        ret = portmod_port_phy_lane_access_get(unit, port, &access_param,
                                               1, &phy_access, &nof_phys, NULL);
    }
    if(ret != SOC_E_NONE){
        cli_out("ERROR: Core access get failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    }
    if (IS_C_PORT(unit, port)) {
        if(nof_phys > 0){
            if(phy_access12[0].access.lane_mask){
                ret = phymod_phy_diagnostics_get(&phy_access12[0], diag_phy);
            }
        } else {
            cli_out("ERROR: Getting phy diagnostics info failed. No matching phy found.\n");
            return CMD_FAIL;
        }
    } else {
        ret = phymod_phy_diagnostics_get(&phy_access, diag_phy);
    }
    if(ret != SOC_E_NONE){
        cli_out("ERROR: Getting phy diagnostics info failed: %s\n", soc_errmsg(ret));
        return CMD_FAIL;
    } 

    return CMD_OK;
}

STATIC cmd_result_t
portmod_port_diag_prbs_run(int unit, int port, portmod_port_diag_prbs_t* diag_prbs){
    int ret;

    if(NULL == diag_prbs){
        cli_out("diag_prbs is null pointer");  
        return CMD_FAIL;
    }

    if (sal_strcasecmp(diag_prbs->cmd, "set") == 0){
        /* Set prbs configuration and enable prbs*/
        ret = portmod_port_prbs_config_set(unit, port, diag_prbs->mode, diag_prbs->flags, &diag_prbs->prbs_config);
        if(ret != SOC_E_NONE){     
            cli_out("ERROR: port prbs config set failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
        ret = portmod_port_prbs_enable_set(unit, port, diag_prbs->mode, diag_prbs->flags, diag_prbs->enable);
        if(ret != SOC_E_NONE){     
            cli_out("ERROR: port prbs enable set failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
    }
    else if (sal_strcasecmp(diag_prbs->cmd, "clear") == 0){
        /*disable prbs*/
        ret = portmod_port_prbs_enable_set(unit, port, diag_prbs->mode, diag_prbs->flags, diag_prbs->enable);
        if(ret != SOC_E_NONE){     
            cli_out("ERROR: port prbs enable set failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
    }
    else if (sal_strcasecmp(diag_prbs->cmd, "get") == 0){ 
        sal_sleep(diag_prbs->interval);
        /*check prbs results*/
        ret = portmod_port_prbs_status_get(unit, port, diag_prbs->mode, diag_prbs->flags, &diag_prbs->prbs_status);
        if(ret != SOC_E_NONE){     
            cli_out("ERROR: port prbs status get failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
portmod_port_diag_phy_regdump(int unit, int port){
    int ret, nof_phys;
    phymod_phy_access_t phy_access;
    phymod_phy_access_t phy_access12[3];
    portmod_access_get_params_t access_param;

    /* get phy polority and nof_phys */
    ret = portmod_access_get_params_t_init(unit, &access_param);
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: Access parameters init failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 
    if (IS_C_PORT(unit, port)) {
        ret = portmod_port_phy_lane_access_get(unit, port, &access_param,
                                               3, &phy_access12[0], &nof_phys, NULL);
    } else {
        ret = portmod_port_phy_lane_access_get(unit, port, &access_param,
                                               1, &phy_access, &nof_phys, NULL);
    }
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: phy access get failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 
    if (IS_C_PORT(unit, port)) {
        ret = phymod_phy_status_dump(&phy_access12[0]);
    } else {
        ret = phymod_phy_status_dump(&phy_access);
    }
    if(ret != SOC_E_NONE){     
        cli_out("ERROR: phy status dump %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    } 

    return CMD_OK;
}


STATIC cmd_result_t
portmod_info_pm_summary_diag(int unit){
    cmd_result_t ret = CMD_OK;
    int phy, port, pm;
    int range_start, range_end;
    char *pm_type = "";
    char *unknow_string = "unknown";
    int is_first_range;
    int is_virtual = FALSE;
    portmod_pm_diag_info_t diag_info;

   
    cli_out(" PM id |    type     |    phys    | logic ports \n");
    cli_out("------------------------------------------------\n");
    for( pm = 0; pm <= MAX_PMS; pm++){
        ret = portmod_pm_diag_info_get(unit, pm, &diag_info);
        if (BCM_FAILURE(ret)){
            continue;
        }
        if(diag_info.type == portmodDispatchTypeCount){
            /*invalid port*/
            continue;
        }
        ret = enum_value_to_string(portmod_dispatch_type_t_mapping, diag_info.type, &pm_type);
        if(CMD_OK != ret){
            pm_type = unknow_string;
        }
        cli_out("   %02d  | %-10s  | ",  pm, pm_type);
        is_virtual = TRUE;
        /*add phys_str*/

        PORTMOD_PBMP_ITER(diag_info.phys, phy){
            is_virtual = FALSE;
            break;
        }
        if(is_virtual){
            cli_out("\n");
            continue;
        } 
        
        range_start = phy;
        range_end = phy;
        is_first_range = TRUE;
        PORTMOD_PBMP_ITER(diag_info.phys, phy){
            if(phy == range_start){ /*first one*/
                continue;
            }
            if(phy != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                cli_out("%03d - %03d", range_start, range_end);
                range_start = phy;
                range_end = phy;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        cli_out("%03d - %03d  |", range_start, range_end);


        /*display ports which belongs to this PM*/
        SOC_PBMP_ITER(diag_info.ports, port){
            break;
        }
        if (port >= MAX_PORTS){
            cli_out("no port attached\n");
            continue;
        }

        range_start = port;
        range_end = port;
        is_first_range = TRUE;
        SOC_PBMP_ITER(diag_info.ports, port){
            if(port == range_start){ 
                continue;
            }
            if(port != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                if (range_start == range_end){
                    cli_out("%03d", range_start);
                } 
                else {
                    cli_out("%03d - %03d", range_start, range_end);
                }
                range_start = port;
                range_end = port;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        if (range_start == range_end){
            cli_out("%03d\n", range_start);
        }else {
            cli_out("%03d - %03d\n", range_start, range_end);
        }

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
        if(diag_info.type == portmodDispatchTypePm4x10_qtc){
            /*Pm4x10_qtc uses two pm ids*/
            pm ++;
        }
#endif
    }
    return CMD_OK;
}


STATIC cmd_result_t
portmod_info_pm_detail_diag(int unit, int pm){
    cmd_result_t ret = CMD_OK;
    int i, phy, port;
    int range_start, range_end;
    char *pm_type = "";
    char *ref_clk = "";
    char *unknow_string = "unknown";
    int is_first_range;
    portmod_pm_diag_info_t diag_info;

    sal_memset(&diag_info, 0, sizeof(portmod_pm_diag_info_t));
    ret = portmod_pm_diag_info_get(unit, pm, &diag_info);
    if (BCM_FAILURE(ret)){
        return CMD_FAIL;
    }
    if(diag_info.type == portmodDispatchTypeCount){
        /*invalid port*/
        return CMD_USAGE;
    }
    
    ret = enum_value_to_string(portmod_dispatch_type_t_mapping, diag_info.type, &pm_type);
    if(CMD_OK != ret){
        pm_type = unknow_string;
    }
    cli_out("PM ID: %02d\n",  pm);
    cli_out("PM Type: %-10s\n", pm_type);

    /*add phys_str*/
    cli_out("Phys: ");
    /*is_virtual = TRUE;*/
    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        break;
    }
    
    range_start = phy;
    range_end = phy;
    is_first_range = TRUE;
    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        if(phy == range_start){ /*first one*/
            continue;
        }
        if(phy != range_end + 1){
            if(!is_first_range){
                cli_out(",");
            }
            cli_out("%03d - %03d", range_start, range_end);
            range_start = phy;
            range_end = phy;
            is_first_range = FALSE;
        }
        else{
            range_end++;
        }
    }
    if(!is_first_range){
        cli_out(",");
    }
    cli_out("%03d - %03d\n", range_start, range_end);

    /*display ports attached to this PM*/
    cli_out("Logical Ports Attached: ");
    SOC_PBMP_ITER(diag_info.ports, port){
        break;
    }
    if (port >= MAX_PORTS){
        cli_out("no\n");
    }
    else{
        range_start = port;
        range_end = port;
        is_first_range = TRUE;
        SOC_PBMP_ITER(diag_info.ports, port){
            if(port == range_start){ 
                continue;
            }
            if(port != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                if (range_start == range_end){
                    cli_out("%03d", range_start);
                }
                else {
                    cli_out("%03d - %03d", range_start, range_end);
                }
                range_start = port;
                range_end = port;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        if (range_start == range_end){
            cli_out("%03d\n", range_start);
        }
        else {
            cli_out("%03d - %03d\n", range_start, range_end);
        }        
    }
    /*display PM reference clock*/
    ret = enum_value_to_string(phymod_ref_clk_t_mapping, diag_info.core_info.ref_clk, &ref_clk);
    if(CMD_OK != ret){
        ref_clk = unknow_string;
    }
    /*display PM lane map*/
    for (i = 0; i < diag_info.core_info.lane_map.num_of_lanes; i++){
        if (0 == i){
            cli_out("Lane Map RX: ");
        }
        cli_out("%d", diag_info.core_info.lane_map.lane_map_rx[i]);
        if (i < diag_info.core_info.lane_map.num_of_lanes-1){
            cli_out("-");
        }
        else{
            cli_out("\n");
        }
    }
    for (i = 0; i < diag_info.core_info.lane_map.num_of_lanes; i++){
        if (0 == i){
            cli_out("Lane Map TX: ");
        }
        cli_out("%d", diag_info.core_info.lane_map.lane_map_tx[i]);
        if (i < diag_info.core_info.lane_map.num_of_lanes-1){
            cli_out("-");
        }
        else{
            cli_out("\n");
        }
    }

    return CMD_OK;
}



STATIC cmd_result_t
portmod_info_port_summary_diag(int unit){
    int port, rv;
    int phy;
    int range_start, range_end;
    int is_first_range;
    int is_virtual = FALSE;
    char *interface_types_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
    portmod_port_diag_info_t diag_info;
    portmod_dispatch_type_t port_type;
    char *mac[2] = {"XLMAC", "CLMAC"};
    int mac_type = 0;

    sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));
    cli_out("port | alias | PM id | port type | phys | mac           \n");
    cli_out("--------------------------------------------------\n");
    BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port){
        rv = portmod_port_diag_info_get(unit, port, &diag_info);
        if (BCM_FAILURE(rv)){
            continue;
        }
        rv = portmod_port_pm_type_get(unit, port, &port, &port_type);
        if (BCM_FAILURE(rv)) {
            continue;
        }

        switch(port_type) {
#ifdef PORTMOD_PM4X25_SUPPORT
            case portmodDispatchTypePm4x25:
                mac_type = 1;
                break;
#endif
#ifdef PORTMOD_PM4X25TD_SUPPORT
            case portmodDispatchTypePm4x25td:
                mac_type = 1;
                break;
#endif
#ifdef PORTMOD_PM12X10_SUPPORT
            case portmodDispatchTypePm12x10:
                mac_type = 1;
                break;
#endif
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
            case portmodDispatchTypePm12x10_xgs:
                mac_type = 1;
                break;
#endif

#ifdef PORTMOD_PM4X10_SUPPORT
            case portmodDispatchTypePm4x10:
                mac_type = 0;
                break;

#endif
#ifdef PORTMOD_PM4X10TD_SUPPORT
            case portmodDispatchTypePm4x10td:
                mac_type = 0;
                break;
#endif
#ifdef PORTMOD_PM4x10Q_SUPPORT
            case portmodDispatchTypePm4x10Q:
                mac_type = 0;
                break;
#endif
            default :
                mac_type = 0;
                break;
        }

        if(diag_info.pm_id < 0){
            /*invalid port*/
            continue;
        }
        if(diag_info.original_port == port){
            cli_out(" %03d |       |   %02d  | %-9s | ", port, diag_info.pm_id, interface_types_names[diag_info.interface]);
        }
        else{
            cli_out(" %03d | %03d |   %02d  | %-9s | ", port, diag_info.original_port, diag_info.pm_id, interface_types_names[diag_info.interface]);
        }
        is_virtual = TRUE;
        /*add phys_str*/

        PORTMOD_PBMP_ITER(diag_info.phys, phy){
            is_virtual = FALSE;
            break;
        }
        if(is_virtual){
            cli_out("virtual\n");
            continue;
        } 
        if(diag_info.interface == SOC_PORT_IF_QSGMII){
            /* coverity[uninit_use_in_call] */
            cli_out("%03d.%d", phy, diag_info.sub_phy);
            continue;
        }
        
        range_start = phy;
        range_end = phy;
        is_first_range = TRUE;
        PORTMOD_PBMP_ITER(diag_info.phys, phy){
            if(phy == range_start){ /*first one*/
                continue;
            }
            if(phy != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                if(range_start == range_end){
                    cli_out("%03d", range_start);
                } else {
                    cli_out("%03d - %03d", range_start, range_end);
                }
                range_start = phy;
                range_end = phy;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        if(range_start == range_end){
            cli_out("%03d", range_start);
        } else {
            cli_out("%03d - %03d", range_start, range_end);
        }
        
        cli_out("  | %s\n", mac[mac_type]);
    }
    return CMD_OK;
}



STATIC cmd_result_t
portmod_info_port_detail_diag(int unit, int port){
    int rv, i;
    int phy, is_most_ext=0, phyn=0;
    int range_start, range_end;
    int is_first_range;
    int is_virtual = FALSE;
    char *mode_str = "";
    char *unknow_str = "unknow";
    int nof_cores; /**< core number */
    phymod_core_access_t core_access[1 + MAX_PHYN];
    portmod_port_diag_info_t diag_info;
    portmod_dispatch_type_t port_type;
    char *mac[2] = {"XLMAC", "CLMAC"};
    int mac_type = 0;
   
    rv = portmod_port_diag_info_get(unit, port, &diag_info);
    if (BCM_FAILURE(rv)){
        return CMD_FAIL;
    }
    if(diag_info.pm_id < 0){
        /*invalid port*/
        return CMD_FAIL;
    }
    cli_out("Port: %03d\n", port);
    cli_out("Parent PM: %03d\n", diag_info.pm_id);
    cli_out("Phys: ");

    is_virtual = TRUE;
    /*add phys_str*/
    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        is_virtual = FALSE;
        break;
    }
    if(is_virtual){
        cli_out("virtual\n");
        return CMD_FAIL;;
    }
    if(diag_info.interface == SOC_PORT_IF_QSGMII){
        /* coverity[uninit_use_in_call] */
        cli_out("%03d.%d\n", phy, diag_info.sub_phy);
        return CMD_FAIL;;
    }
    
    range_start = phy;
    range_end = phy;
    is_first_range = TRUE;
    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        if(phy == range_start){ /*first one*/
            continue;
        }
        if(phy != range_end + 1){
            if(!is_first_range){
                cli_out(",");
            }
            if(range_start == range_end){
                cli_out("%03d", range_start);
            } else {
                cli_out("%03d - %03d", range_start, range_end);
            }
            range_start = phy;
            range_end = phy;
            is_first_range = FALSE;
        }
        else{
            range_end++;
        }
    }
    if(!is_first_range){
        cli_out(",");
    }
    if(range_start == range_end){
        cli_out("%03d\n", range_start);
    } else {
        cli_out("%03d - %03d\n", range_start, range_end);
    }

    rv = portmod_port_pm_type_get(unit, port, &port, &port_type);
    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    }

    switch(port_type) {
#ifdef PORTMOD_PM4X25_SUPPORT
            case portmodDispatchTypePm4x25:
                mac_type = 1;
                break;
#endif
#ifdef PORTMOD_PM4X25TD_SUPPORT
            case portmodDispatchTypePm4x25td:
                mac_type = 1;
                break;
#endif
#ifdef PORTMOD_PM12X10_SUPPORT
            case portmodDispatchTypePm12x10:
                mac_type = 1;
                break;
#endif
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
            case portmodDispatchTypePm12x10_xgs:
                mac_type = 1;
                break;
#endif

#ifdef PORTMOD_PM4X10_SUPPORT
            case portmodDispatchTypePm4x10:
                mac_type = 0;
                break;

#endif
#ifdef PORTMOD_PM4X10TD_SUPPORT
            case portmodDispatchTypePm4x10td:
                mac_type = 0;
                break;
#endif
#ifdef PORTMOD_PM4x10Q_SUPPORT
            case portmodDispatchTypePm4x10Q:
                mac_type = 0;
                break;
#endif
            default :
                mac_type = 0;
                break;
        }

    /* display mac driver type*/
    cli_out("Mac: %s\n", mac[mac_type]);

    /* display polarity*/
    cli_out("Polarity: TX:%d, RX:%d\n", diag_info.polarity.tx_polarity, diag_info.polarity.rx_polarity);


    /*display PM phy address*/
    while (!is_most_ext) {
        rv = portmod_port_core_access_get(unit, port, phyn, MAX_PHYN, core_access, &nof_cores, &is_most_ext);
        if (BCM_FAILURE(rv)){
            return CMD_FAIL;
        }

        for (i=0 ; i<nof_cores ; i++) {
            if (phyn == 0) {
                cli_out("Internal PHY Address: %d\n", core_access[i].access.addr);
            } else {
                cli_out("External PHY Address: %d\n", core_access[i].access.addr);
            }
        }

        phyn++;
    }

    if (phyn <=1 ) {
        cli_out("External PHY Address: Not Attached\n");
    }

    /* display core port mode */
    rv = enum_value_to_string(portmod_core_port_mode_t_mapping, diag_info.core_mode.cur_mode, &mode_str);
    if(CMD_OK != rv) {
        mode_str = unknow_str;
    }
    cli_out("Core Mode: %s\n", mode_str);
        
    return CMD_OK;
}



STATIC cmd_result_t
portmod_status_port_summary_diag(int unit){
    int port;
    int phy;
    int range_start, range_end;
    int is_first_range;
    int is_virtual = TRUE;
    cmd_result_t ret = CMD_OK;

    char *interface_types_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
    char *port_encoding_names[] = SOC_PORT_ENCODING_NAMES_INITIALIZER;
    portmod_port_diag_status_t diag_status;

    sal_memset(&diag_status, 0, sizeof(portmod_port_diag_status_t));
    cli_out("Port | Intf type | Speed | Encoding      | LINK | AN       | LT  | LB  | EN | Fault | Phys  |\n");
    cli_out("---------------------------------------------------------------------------------------------\n");
    BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port){
        ret = portmod_port_diag_status_get(unit, port, &diag_status);
        if (BCM_FAILURE(ret)){
            continue;
        }
        cli_out(" %03d | %-9s | %03dG  | %-13s | %-4s | %-8s | %-3s | %-3s | %-2s | %-5s |", \
            port, interface_types_names[diag_status.config.interface], diag_status.config.speed/1000, \
            port_encoding_names[diag_status.encoding], (0 == diag_status.link)?"DOWN":"UP", \
            (0 == diag_status.an_status.enabled)?"OFF":((0 == diag_status.an_status.locked)?"UNLOCKED":"LOCKED"), \
            "OFF", (0 == diag_status.loopback)?"OFF":"ON", (0 == diag_status.enable)?"N":"Y", \
            ((0 == diag_status.lf&&0 == diag_status.rf)?"NONE":((0 != diag_status.lf&&0 == diag_status.rf)?"LF": \
            ((0 == diag_status.lf&&0 != diag_status.rf)?"RF":"LF,RF"))));
        /*display port-phy mapping*/
        PORTMOD_PBMP_ITER(diag_status.phys, phy){
            is_virtual = FALSE;
            break;
        }
        if(is_virtual){
            cli_out("virtual\n");
            return CMD_FAIL;;
        }
        if(diag_status.config.interface == SOC_PORT_IF_QSGMII){
            /* coverity[uninit_use_in_call] */
            cli_out("%03d.%d\n", phy, diag_status.sub_phy);
            return CMD_FAIL;;
        }
        
        range_start = phy;
        range_end = phy;
        is_first_range = TRUE;
        PORTMOD_PBMP_ITER(diag_status.phys, phy){
            if(phy == range_start){ /*first one*/
                continue;
            }
            if(phy != range_end + 1){
                if(!is_first_range){
                    cli_out(",");
                }
                if(range_start == range_end){
                    cli_out("%03d", range_start);
                } else {
                    cli_out("%03d-%03d", range_start, range_end);
                }
                range_start = phy;
                range_end = phy;
                is_first_range = FALSE;
            }
            else{
                range_end++;
            }
        }
        if(!is_first_range){
            cli_out(",");
        }
        if(range_start == range_end){
            cli_out("%03d\n", range_start);
        } else {
            cli_out("%03d-%03d\n", range_start, range_end);
        }
        
    }
    
    return CMD_OK;
}



STATIC cmd_result_t
portmod_status_port_detail_diag(int unit, int port){
    int rv;
    int phy;
    int range_start, range_end;
    int is_first_range;
    int is_virtual = FALSE;
    portmod_port_diag_info_t diag_info;
  
   
    rv = portmod_port_diag_info_get(unit, port, &diag_info);
    if (BCM_FAILURE(rv)){
        return CMD_FAIL;
    }
    if(diag_info.pm_id < 0){
        /*invalid port*/
        return CMD_FAIL;
    }
    cli_out("Port: %03d\n", port);
    cli_out("Parent PM: %03d\n", diag_info.pm_id);
    cli_out("Phys:");

    is_virtual = TRUE;
    /*add phys_str*/
    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        is_virtual = FALSE;
        break;
    }
    if(is_virtual){
        cli_out("virtual\n");
        return CMD_FAIL;;
    }
    if(diag_info.interface == SOC_PORT_IF_QSGMII){
        /* coverity[uninit_use_in_call] */
        cli_out("%03d.%d\n", phy, diag_info.sub_phy);
        return CMD_FAIL;;
    }
    
    range_start = phy;
    range_end = phy;
    is_first_range = TRUE;
    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        if(phy == range_start){ /*first one*/
            continue;
        }
        if(phy != range_end + 1){
            if(!is_first_range){
                cli_out(",");
            }
            if(range_start == range_end){
                cli_out("%03d", range_start);
            } else {
                cli_out("%03d - %03d", range_start, range_end);
            }
            range_start = phy;
            range_end = phy;
            is_first_range = FALSE;
        }
        else{
            range_end++;
        }
    }
    if(!is_first_range){
        cli_out(",");
    }
    if(range_start == range_end){
        cli_out("%03d\n", range_start);
    } else {
        cli_out("%03d - %03d\n", range_start, range_end);
    }
        
    return CMD_OK;
}


STATIC cmd_result_t
portmod_port_ability_display(portmod_port_ability_t *port_ability){
    int i, string_len;
    char *port_ability_speed_string[] = SOC_PA_SPEED_STRING;
    char *port_ability_pause_string[] = SOC_PA_PAUSE_STRING;
    char *port_ability_intf_string[] = SOC_PA_INTF_STRING;
    char *port_ability_medium_string[] = SOC_PA_MEDIUM_STRING;
    char *port_ability_lb_string[] = SOC_PA_LB_STRING;
    char *port_ability_flags_string[] = _SHR_PA_FLAGS_STRING;
    char *port_ability_eee_string[] = SOC_PA_EEE_STRING;
    char *port_ability_encap_string[] = SOC_PA_ENCAP_STRING;

    if(port_ability == NULL) {
        return CMD_FAIL;
    }
    string_len = sizeof(port_ability_speed_string)/sizeof(char *);
    cli_out("Half Duplex Speed: |");
    for (i = 0;i < string_len;i++){
        if (port_ability->speed_half_duplex&(1<<i)){
            cli_out("%s|", port_ability_speed_string[i]);
        }
    }
    cli_out("\n");

    string_len = sizeof(port_ability_speed_string)/sizeof(char *);
    cli_out("Full Duplex Speed: |");
    for (i = 0;i < string_len;i++){
        if (port_ability->speed_full_duplex&(1<<i)){
            cli_out("%s|", port_ability_speed_string[i]);
        }        
    }
    cli_out("\n");
    
    cli_out("Pause:             |");
    string_len = sizeof(port_ability_pause_string)/sizeof(char *);
    port_ability->pause = 3;
    for (i = 0;i < string_len;i++){
        if (port_ability->pause&(1<<i)){
            cli_out("%s|", port_ability_pause_string[i]);
        }        
    }
    cli_out("\n");

    string_len = sizeof(port_ability_intf_string)/sizeof(char *);
    cli_out("Interface:         |");
    port_ability->interface = 15;
    for (i = 0;i < string_len;i++){
        if (port_ability->interface&(1<<i)){
            cli_out("%s|", port_ability_intf_string[i]);
        }        
    }
    cli_out("\n");
    
    string_len = sizeof(port_ability_medium_string)/sizeof(char *);
    cli_out("Medium:            |");
    port_ability->medium= 15;
    for (i = 0;i < string_len;i++){
        if (port_ability->medium&(1<<i)){
            cli_out("%s|", port_ability_medium_string[i]);
        }        
    }
    cli_out("\n");

    string_len = sizeof(port_ability_lb_string)/sizeof(char *);
    cli_out("Loopback:          |");
    port_ability->loopback= 15;
    for (i = 0;i < string_len;i++){
        if (port_ability->loopback&(1<<i)){
            cli_out("%s|", port_ability_lb_string[i]);
        }        
    }
    cli_out("\n");

    string_len = sizeof(port_ability_flags_string)/sizeof(char *);
    cli_out("Flags:             |");
    port_ability->flags= 15;
    for (i = 0;i < string_len;i++){
        if (port_ability->flags&(1<<i)){
            cli_out("%s|", port_ability_flags_string[i]);
        }        
    }
    cli_out("\n");

    string_len = sizeof(port_ability_eee_string)/sizeof(char *);
    cli_out("EEE:               |");
    port_ability->eee= 15;
    for (i = 0;i < string_len;i++){
        if (port_ability->eee&(1<<i)){
            cli_out("%s|", port_ability_eee_string[i]);
        }        
    }
    cli_out("\n");

    string_len = sizeof(port_ability_encap_string)/sizeof(char *);
    cli_out("ENCAP:             |");
    port_ability->encap= 15;
    for (i = 0;i < string_len;i++){
        if (port_ability->encap&(1<<i)){
            cli_out("%s|", port_ability_encap_string[i]);
        }        
    }
    cli_out("\n");
    
    
    return CMD_OK;
}


STATIC cmd_result_t
portmod_info_diag(int unit, args_t *a){
    char *c;
	int pm, port;
	parse_table_t pt;
    cmd_result_t ret = CMD_OK;

	if (NULL == (c = ARG_CUR(a))){
        ret = CMD_USAGE;
    }
    else if(sal_strcasecmp(c, "pm") == 0){
        ret = portmod_info_pm_summary_diag(unit);
    }
    else if(sal_strcasecmp(c, "port") == 0){
        ret = portmod_info_port_summary_diag(unit);
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, 0);
        parse_table_add(&pt, "pm", PQ_INT, (void *)MAX_PMS, &pm, 0);
        if (parse_arg_eq(a, &pt) < 0){
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        if((MAX_PORTS == port) && (MAX_PMS == pm)) {
            return CMD_USAGE;
        }
        if(MAX_PORTS > port){
            /* coverity[overrun-local] */
            if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
                cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
                return CMD_FAIL;       
            }
            ret = portmod_info_port_detail_diag(unit, port);
        }
        else if(MAX_PMS > pm){
            ret = portmod_info_pm_detail_diag(unit, pm);
        }
        else{
            ret = CMD_USAGE;
        }
    }
    ARG_NEXT(a);
    
    return ret;
}


STATIC void 
portmod_info_usage(int unit){
    cli_out("command:\n");
    cli_out("  info pm=<pm-id>      -display the the detailed information of the pm-id specific PM.\n");
    cli_out("  info pm              -display the summary information of all PMs.\n");
    cli_out("  info port=<port-id>  -display the the detailed information of the port-id specific port.\n");
    cli_out("  info port            -display the summary information of all ports.\n");
    cli_out("example:\n");
    cli_out("  portmod info pm=0\n");
    cli_out("  portmod info pm\n");
    cli_out("  portmod info port=1\n");
    cli_out("  portmod info port\n");
}


STATIC cmd_result_t
portmod_status_diag(int unit, args_t *a){
    char *c;
	int port;
	parse_table_t pt;
    cmd_result_t ret = CMD_OK;

	if (NULL == (c = ARG_CUR(a))){
        ret = CMD_USAGE;
    } 
    else if(sal_strcasecmp(c, "port") == 0){
        ret = portmod_status_port_summary_diag(unit);
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, 0);
        if (parse_arg_eq(a, &pt) < 0){
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }
        ret = portmod_status_port_detail_diag(unit, port);
    }
    ARG_NEXT(a);
    
    return ret;
}


STATIC void 
portmod_status_usage(int unit){
    cli_out("command:\n");
    cli_out("  status port=<port-id>  -display the the detailed status of the port-id specific port.\n");
    cli_out("  status port            -display the summary status of all ports.\n");
    cli_out("example:\n");
    cli_out("  portmod status port=1\n");
    cli_out("  portmod status port\n");
}


STATIC cmd_result_t
portmod_fc_diag(int unit, args_t *a){
    int port;
	parse_table_t pt;
    portmod_port_diag_fc_t diag_fc;
    int ret = CMD_OK;

    sal_memset(&diag_fc, 0, sizeof(portmod_port_diag_fc_t));
    if (!ARG_CNT(a)) {  
        ret = CMD_USAGE;
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, 0);
        if (parse_arg_eq(a, &pt) < 0){
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }
        ret = portmod_port_diag_fc_get(unit, port, &diag_fc);
        if(ret != SOC_E_NONE){      
            cli_out("ERROR: Getting flow control status failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
        if(MAX_PORTS > port){        
            cli_out("PFC config type: %d\n", diag_fc.pfc_cfg.type);
            cli_out("PFC config opcode: %d\n", diag_fc.pfc_cfg.opcode);
            cli_out("PFC config classes: %d\n", diag_fc.pfc_cfg.classes);
            cli_out("PFC config da_oui: %d\n", diag_fc.pfc_cfg.da_oui);
            cli_out("PFC config da_nonoui: %d\n", diag_fc.pfc_cfg.da_nonoui);
            cli_out("PFC config rxpass: %d\n\n", diag_fc.pfc_cfg.rxpass);

            cli_out("PFC control rx_enable: %d\n", diag_fc.pfc_ctrl.rx_enable);
            cli_out("PFC control tx_enable: %d\n", diag_fc.pfc_ctrl.tx_enable);
            cli_out("PFC control stats_en: %d\n", diag_fc.pfc_ctrl.stats_en);
            cli_out("PFC control force_xon: %d\n", diag_fc.pfc_ctrl.force_xon);
            cli_out("PFC control refresh_timer: %d\n", diag_fc.pfc_ctrl.refresh_timer);
            cli_out("PFC control xoff_timer: %d\n\n", diag_fc.pfc_ctrl.xoff_timer);

            cli_out("LLFC control rx_enable: %d\n", diag_fc.llfc_ctrl.rx_enable);
            cli_out("LLFC control tx_enable: %d\n", diag_fc.llfc_ctrl.tx_enable);
            cli_out("LLFC control crc_ignore: %d\n", diag_fc.llfc_ctrl.crc_ignore);
            cli_out("LLFC control in_ipg_only: %d\n", diag_fc.llfc_ctrl.in_ipg_only);
        
        }
        else{
            ret = CMD_USAGE;
        }
    }
    return ret;
}

STATIC void 
portmod_fc_usage(int unit){
    cli_out("command:\n");
    cli_out("  fc port=<port-id>\n");
    cli_out("example:\n");
    cli_out("  portmod fc port=1\n");
}

STATIC cmd_result_t
portmod_autoneg_diag(int unit, args_t *a){
	int port;
	parse_table_t    pt;
    portmod_port_diag_autoneg_t diag_autoneg;
    int ret = CMD_OK;

    sal_memset(&diag_autoneg, 0, sizeof(portmod_port_diag_autoneg_t));
    if (!ARG_CNT(a)) {  
        ret = CMD_USAGE;
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, 0);
        if (parse_arg_eq(a, &pt) < 0){
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }
        ret = portmod_port_diag_autoneg_get(unit, port, &diag_autoneg);
        if(ret != SOC_E_NONE){      
            cli_out("ERROR: Getting auto-negotiation status failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
        if(MAX_PORTS > port){        
            cli_out("**********************************************************\n");
            cli_out("****Auto-negociation local port supported ability*********\n");
            cli_out("**********************************************************\n");
            portmod_port_ability_display(&diag_autoneg.local_ability);
            cli_out("**********************************************************\n");
            cli_out("****Auto-negociation local port advertised ability********\n");
            cli_out("**********************************************************\n");
            portmod_port_ability_display(&diag_autoneg.adv_ability);
            cli_out("**********************************************************\n");
            cli_out("****Auto-negociation received ability from partner********\n");
            cli_out("**********************************************************\n");
            portmod_port_ability_display(&diag_autoneg.remote_ability);
        }
        else{
            ret = CMD_USAGE;
        }
    }
    return ret;
}


STATIC void 
portmod_autoneg_usage(int unit){
    cli_out("command:\n");
    cli_out("  autoneg port=<port-id>\n");
    cli_out("example:\n");
    cli_out("  portmod autoneg port=1\n");
}


STATIC cmd_result_t
portmod_core_diag(int unit, args_t *a){
    parse_table_t pt;
    int port;
    phymod_core_diagnostics_t core_diag;
    phymod_core_firmware_info_t core_firmware;
    int ret = SOC_E_NONE;

    sal_memset(&core_diag, 0, sizeof(phymod_core_diagnostics_t));
    sal_memset(&core_firmware, 0, sizeof(phymod_core_firmware_info_t));
    if (!ARG_CNT(a)) {  
        ret = CMD_USAGE;
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("ERROR: invalid option: %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }    
        /* Now free allocated strings */
        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }
        ret = portmod_port_diag_core_info_get(unit, port, &core_diag, &core_firmware);
        if(ret != SOC_E_NONE){      
            cli_out("ERROR: Getting core status failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }
        cli_out("Port%2d(%s): Core temperature is %d\n", port, BCM_PORT_NAME(unit, port), \
            core_diag.temperature);
        cli_out("Port%2d(%s): Core pll range is %d\n", port, BCM_PORT_NAME(unit, port), \
            core_diag.pll_range);
        cli_out("Port%2d(%s): Core firmware version is %d\n", port, BCM_PORT_NAME(unit, port), \
            core_firmware.fw_version);
        cli_out("Port%2d(%s): Core firmware crc is %d\n", port, BCM_PORT_NAME(unit, port), \
            core_firmware.fw_crc);
    }
    
    return CMD_OK;
}


STATIC void 
portmod_core_usage(int unit){
    cli_out("command:\n");
    cli_out("  core port=<port-id>\n");
    cli_out("example:\n");
    cli_out("  portmod core port=1\n");
}


STATIC cmd_result_t
portmod_phy_diag(int unit, args_t *a){
    parse_table_t pt;
    int port, phyn, lane;
    char *osr_str = "";
    char *pmd_str = "";
    char *unknow_str = "unknow";
    char *if_str;
    int port_loc = 0;
    phymod_phy_diagnostics_t phy_diag;
    int ret = CMD_OK;

    sal_memset(&phy_diag, 0, sizeof(phymod_phy_diagnostics_t));
    if (!ARG_CNT(a)) {  
        ret = CMD_USAGE;
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, NULL);
        parse_table_add(&pt, "phychain", PQ_INT,(void *)-1, &phyn, NULL);
        parse_table_add(&pt, "lane", PQ_INT,(void *)0, &lane, NULL);
        parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("ERROR: invalid option: %s\n", ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }   

        port_loc = PORTMOD_SIDE_LINE ;
        if (if_str) {
            if (sal_strcasecmp(if_str, "sys") == 0) {
                port_loc = PORTMOD_SIDE_SYSTEM ;
            } else if (sal_strcasecmp(if_str, "line") == 0) {
                port_loc = PORTMOD_SIDE_LINE ;
            } else if (if_str[0] != 0) {
                cli_out("InterFace must be sys or line.\n");
                return CMD_FAIL;
            }
        }
 
        /* Now free allocated strings */
        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }
        
        ret = portmod_port_diag_phy_info_get(unit, port, phyn, lane, port_loc,  &phy_diag);
        if(ret != SOC_E_NONE){      
            cli_out("ERROR: Getting phy status failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;                                    
        }     
        ret = enum_value_to_string(phymod_osr_mode_t_mapping, phy_diag.osr_mode, &osr_str);
        if(CMD_OK != ret){
            osr_str = unknow_str;
        }
        ret = enum_value_to_string(phymod_pmd_mode_t_mapping, phy_diag.pmd_mode, &pmd_str);
        if(CMD_OK != ret){
            pmd_str = unknow_str;
        }
        cli_out("PHY signal_detect: %d\n", phy_diag.signal_detect);
        cli_out("PHY vga_bias_reduced: %d\n", phy_diag.vga_bias_reduced);
        cli_out("PHY postc_metric: %d\n", phy_diag.postc_metric);
        cli_out("PHY osr_mode: %d\n", phy_diag.signal_detect);
        cli_out("PHY pmd_mode: %d\n", phy_diag.signal_detect);
        cli_out("PHY rx_lock: %d\n", phy_diag.rx_lock);
        cli_out("PHY rx_ppm: %d\n", phy_diag.rx_ppm);
        cli_out("PHY tx_ppm: %d\n", phy_diag.tx_ppm);
        cli_out("PHY k90_offset %d\n", phy_diag.clk90_offset);
        cli_out("PHY clkp1_offset: %d\n", phy_diag.clkp1_offset);
        cli_out("PHY p1_lvl: %d\n", phy_diag.p1_lvl);
        cli_out("PHY m1_lvl: %d\n", phy_diag.m1_lvl);
        cli_out("PHY dfe1_dcd: %d\n", phy_diag.dfe1_dcd);
        cli_out("PHY dfe2_dcd: %d\n", phy_diag.dfe2_dcd);
        cli_out("PHY slicer_target: %d\n", phy_diag.slicer_target);

        cli_out("PHY slicer_offset.offset_pe %d\n", phy_diag.slicer_offset.offset_pe);
        cli_out("PHY slicer_offset.offset_ze %d\n", phy_diag.slicer_offset.offset_ze);
        cli_out("PHY slicer_offset.offset_me %d\n", phy_diag.slicer_offset.offset_me);
        cli_out("PHY slicer_offset.offset_po %d\n", phy_diag.slicer_offset.offset_po);
        cli_out("PHY slicer_offset.offset_zo %d\n", phy_diag.slicer_offset.offset_zo);
        cli_out("PHY slicer_offset.offset_mo %d\n", phy_diag.slicer_offset.offset_mo);

        cli_out("PHY eyescan.heye_left: %d\n", phy_diag.eyescan.heye_left);
        cli_out("PHY eyescan.heye_right: %d\n", phy_diag.eyescan.heye_right);
        cli_out("PHY eyescan.veye_upper: %d\n", phy_diag.eyescan.veye_upper);
        cli_out("PHY eyescan.veye_lower: %d\n", phy_diag.eyescan.veye_lower);

        cli_out("PHY state_machine_status: %d\n", phy_diag.state_machine_status);
        cli_out("PHY link_time : %d\n", phy_diag.link_time);
        cli_out("PHY pf_main : %d\n", phy_diag.pf_main);
        cli_out("PHY pf_hiz : %d\n", phy_diag.pf_hiz);
        cli_out("PHY pf_bst: %d\n", phy_diag.pf_bst);
        cli_out("PHY pf_low: %d\n", phy_diag.pf_low);
        cli_out("PHY pf2_ctrl: %d\n", phy_diag.pf2_ctrl);
        cli_out("PHY vga: %d\n", phy_diag.vga);
        cli_out("PHY dc_offset: %d\n", phy_diag.dc_offset);
        cli_out("PHY p1_lvl_ctrl: %d\n", phy_diag.p1_lvl_ctrl);
        cli_out("PHY dfe1: %d\n", phy_diag.dfe1);
        cli_out("PHY dfe2: %d\n", phy_diag.dfe2);
        cli_out("PHY dfe3: %d\n", phy_diag.dfe3);
        cli_out("PHY dfe4: %d\n", phy_diag.dfe4);
        cli_out("PHY dfe5: %d\n", phy_diag.dfe5);
        cli_out("PHY dfe6: %d\n", phy_diag.dfe6);
        cli_out("PHY txfir_pre: %d\n", phy_diag.txfir_pre);
        cli_out("PHY txfir_main: %d\n", phy_diag.txfir_main);
        cli_out("PHY txfir_post1: %d\n", phy_diag.txfir_post1);
        cli_out("PHY txfir_post2: %d\n", phy_diag.txfir_post2);
        cli_out("PHY txfir_post3: %d\n", phy_diag.txfir_post3);
        cli_out("PHY tx_amp_ctrl: %d\n", phy_diag.tx_amp_ctrl);
        cli_out("PHY br_pd_en: %d\n", phy_diag.br_pd_en);

        }
    
    return ret;
}


STATIC void 
portmod_phy_usage(int unit){
    cli_out("command:\n");
    cli_out("  phy port=<port-id> [phychain=<phyn> lane=<lane number> if=<sys|line>]\n");
    cli_out("example:\n");
    cli_out("  portmod phy port=1\n");
}


STATIC cmd_result_t
portmod_prbs_diag(int unit, args_t *a){
    int ret = CMD_OK;
    int port;
    parse_table_t pt;    
    portmod_port_diag_prbs_t diag_prbs;
    int poly = 0, invert = 0, flags = 0, interval=10;
    int enable, mode = 0;
    char *c, *mode_str;
    char *prbs_polys[] = {"X7_X6_1", "X15_X14_1", "X23_X18_1", "X31_X28_1", "X9_X5_1", "X11_X9_1", "X58_X31_1",
                         "0", "1", "2", "3", "4", "5", "6"};    
    enum { PORTMOD_PRBS_SI_MODE, PORTMOD_PRBS_HC_MODE };

    sal_memset(&diag_prbs, 0, sizeof(portmod_port_diag_prbs_t));
    if (NULL == (c = ARG_GET(a))) {
        return CMD_USAGE;
    }
    else if (sal_strcasecmp(c, "set") == 0){
        diag_prbs.cmd = "set";
        enable = 1;
    } 
    else if (sal_strcasecmp(c, "get") == 0){
        diag_prbs.cmd = "get";
        enable = 0;
    } 
    else if (sal_strcasecmp(c, "clear") == 0){
        diag_prbs.cmd = "clear";
        enable = 0;
    } 
    else{
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, NULL);
    parse_table_add(&pt, "mode", PQ_STRING, 0, &mode_str, NULL);
    if (sal_strcasecmp(diag_prbs.cmd, "set") == 0) {
        parse_table_add(&pt, "polynomial", PQ_DFL|PQ_MULTI,
                        (void *)(0), &poly, &prbs_polys);
        parse_table_add(&pt, "invert", PQ_DFL|PQ_BOOL,
                (void *)(0), &invert, NULL);
    } else if (sal_strcasecmp(diag_prbs.cmd, "get") == 0) {
        parse_table_add(&pt, "interval", PQ_DFL|PQ_INT,
                (void *)(0), &interval, NULL);
    }
    if (parse_arg_eq(a, &pt) < 0){
        cli_out("ERROR: invalid option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    if (mode_str) {
        if ((sal_strcasecmp(mode_str, "si") == 0) || (sal_strcasecmp(mode_str, "mac") == 0)) {
            mode = 1;
        } else if ((sal_strcasecmp(mode_str, "hc") == 0) || (sal_strcasecmp(mode_str, "phy") == 0)) {
            mode = 0;
        }
    }
    /* Now free allocated strings */
    parse_arg_eq_done(&pt);    
    if(MAX_PORTS == port) {
        return CMD_USAGE;
    }
    if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
        cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
        return CMD_FAIL;       
    }
    diag_prbs.enable = enable;
    diag_prbs.flags = flags;
    diag_prbs.mode = mode;
    diag_prbs.prbs_config.poly = poly%7;
    diag_prbs.prbs_config.invert = invert; 
    diag_prbs.interval = interval;
    ret = portmod_port_diag_prbs_run(unit, port, &diag_prbs);
    if(ret != SOC_E_NONE){      
        cli_out("ERROR: run prbs diagnostic failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }     
    if (sal_strcasecmp(diag_prbs.cmd, "get") == 0){ 
        /*display prbs results*/
        cli_out("Port%2d(%s): PRBS is %scurrently locked!\n", port, BCM_PORT_NAME(unit, port), \
            (0 == diag_prbs.prbs_status.prbs_lock)?"not ":"");
        cli_out("Port%2d(%s): PRBS is %sunlocked since last call!\n", port, BCM_PORT_NAME(unit, port), \
            (0 == diag_prbs.prbs_status.prbs_lock_loss)?"not ":"");
        cli_out("Port%2d(%s): PRBS has %d errors count!\n", port, BCM_PORT_NAME(unit, port), \
            diag_prbs.prbs_status.error_count);
    }
    
    return ret;
}


STATIC void 
portmod_prbs_usage(int unit){
    cli_out("command:\n");
    cli_out("  set port=<port-id> [mode=<phy|mac> polynomial=<value> Loopback=<true|false> Invert=<value>]\n");
    cli_out("  get port=<port-id> [Interval=<value>]\n");
    cli_out("  clear port=<port-id>\n");
    cli_out("example:\n");
    cli_out("  portmod prbs set port=1 mode=phy\n");
}


STATIC cmd_result_t
portmod_eyescan_diag(int unit, args_t *a){
    int i, nof_phys, port;
    int port_ids[PHYMOD_CONFIG_MAX_CORES_PER_PORT] ;
    int line_rates[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
    phymod_phy_access_t phy_access[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
    portmod_access_get_params_t access_param;
    portmod_port_interface_config_t interface_config;
    char *mode_str = NULL;
    phymod_eyescan_mode_t mode = phymodEyescanModeCount;
    phymod_phy_eyescan_options_t eyescan_options;
    parse_table_t pt;
    int ret = CMD_OK;
    uint32  sys_lane_mask=0xFFFFFFFF;
    char *if_str= NULL;
    int phy_unit=-1;   
 
    /*default values*/
  /* coverity[check_return] */
    portmod_access_get_params_t_init(unit, &access_param);
    access_param.phyn = -1;
    access_param.lane = 0;

    /*default values */
    eyescan_options.timeout_in_milliseconds = 1000;
    eyescan_options.horz_max = 31; 
    eyescan_options.horz_min = -31;
    eyescan_options.hstep = 1;
    eyescan_options.vert_max = 31;
    eyescan_options.vert_min = -31;
    eyescan_options.vstep = 1;
    eyescan_options.mode = 0; /*ber mode*/

    if (!ARG_CNT(a)) {  
        ret = CMD_USAGE;
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, NULL);
        parse_table_add(&pt, "ber", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.mode), NULL);
        parse_table_add(&pt, "vertical_max", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.vert_max), NULL);
        parse_table_add(&pt, "vertical_min", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.vert_min), NULL);
        parse_table_add(&pt, "vertical_step", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.vstep), NULL);
        parse_table_add(&pt, "horizontal_max", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.horz_max), NULL);
        parse_table_add(&pt, "horizontal_min", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.horz_min), NULL);
        parse_table_add(&pt, "horizontal_step", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.hstep), NULL);
        parse_table_add(&pt, "sample_time", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.timeout_in_milliseconds), NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0), &access_param.lane, NULL);
        parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0), &phy_unit, NULL);
        parse_table_add(&pt, "sys_lane_mask", PQ_DFL | PQ_INT, (void *)(0), &sys_lane_mask, NULL);
        parse_table_add(&pt, "mode", PQ_STRING, 0, &mode_str, NULL);
        parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("ERROR: could not parse parameters\n");
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
    
        if (mode_str) {
            if (0 == sal_strcasecmp(mode_str, "fast")){
                mode = phymodEyescanModeFast;
            } 
            else if (0 == sal_strcasecmp(mode_str, "lowBER")){
                mode = phymodEyescanModeLowBER;
            }
            else{
                cli_out("Mode must be fast or lowBER.\n");
                return CMD_FAIL;
            }
        }

        if(phy_unit == 0){
            access_param.phyn = 0; /* internal */
        } else { /* otherwise most ext. */
            access_param.phyn = -1; /* internal */
        }

        access_param.sys_side = PORTMOD_SIDE_LINE; 
        if (if_str) {
            if (sal_strcasecmp(if_str, "sys") == 0) {
                access_param.sys_side = PORTMOD_SIDE_SYSTEM; 
            } else if (sal_strcasecmp(if_str, "line") == 0) {
                access_param.sys_side = PORTMOD_SIDE_LINE; 
            } else if (if_str[0] != 0) {
                cli_out("InterFace must be sys or line.\n");
                return CMD_FAIL;
            }
        } 

        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }

        ret = portmod_port_phy_lane_access_get(unit, port, &access_param, PHYMOD_CONFIG_MAX_CORES_PER_PORT, 
                                               phy_access, &nof_phys, NULL);
        if(ret != SOC_E_NONE){    
            cli_out("ERROR: get phy access failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;        
        }

        if(nof_phys == 0){
            cli_out("ERROR: Invalid lane# \n");
            return CMD_FAIL;
        }

        ret = portmod_port_interface_config_get(unit, port, &interface_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY);
        if(ret != SOC_E_NONE){
            cli_out("ERROR: get interface config failed: %s\n", soc_errmsg(ret));
            return CMD_FAIL;
        }
        for (i = 0;i < PHYMOD_CONFIG_MAX_CORES_PER_PORT;i++){
            line_rates[i] = interface_config.speed;
            port_ids[i]   = port;
        }

        /* if the phys/lane are identical, run eyescan on only once. */
        if ( nof_phys == 3 ){
            if( !sal_memcmp( &phy_access[0],&phy_access[1], sizeof(phymod_phy_access_t)) &&
                !sal_memcmp( &phy_access[0],&phy_access[2], sizeof(phymod_phy_access_t))){
            
                cli_out("Removing repeated entries.\n");     
                 nof_phys = 1;
            }
        }

        /* Check for System Side request. */
        if( sys_lane_mask != 0xFFFFFFFF) {
            cli_out("sys_lane_mask no longer support here, use if=sys  lane=<lane#> \n");
            return CMD_FAIL;        
        }

        ret = phymod_diag_eyescan_run(phy_access, unit, port_ids, line_rates, nof_phys, mode, &eyescan_options);
        if(ret != SOC_E_NONE){    
            cli_out("ERROR: run eyescan diagnostic failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;        
        }
    }
    
    return ret;
}

int portmod_eyescan_diag_dispatch(int unit, soc_port_t port,  args_t *a)
{

    int i, nof_phys;
    int line_rates[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
    int port_ids[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
    phymod_phy_access_t phy_access[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
    portmod_access_get_params_t access_param;
    portmod_port_interface_config_t interface_config;
    char *mode_str = NULL;
    parse_table_t pt;
    int ret = CMD_OK;
    uint32  sys_lane_mask=0xFFFFFFFF;
    char *if_str= NULL;
    phymod_eyescan_mode_t mode = 0;
    phymod_phy_eyescan_options_t eyescan_options;
    int phy_unit=-1;

    /*default values*/
    /* coverity[check_return] */
    portmod_access_get_params_t_init(unit, &access_param);
    access_param.phyn = phy_unit;
    access_param.lane = 0;
    mode = phymodEyescanModeFast;

    /*default values */
    eyescan_options.timeout_in_milliseconds = 1000;
    eyescan_options.horz_max = 31; 
    eyescan_options.horz_min = -31;
    eyescan_options.hstep = 1;
    eyescan_options.vert_max = 31;
    eyescan_options.vert_min = -31;
    eyescan_options.vstep = 1;
    eyescan_options.mode = 0; /*ber mode*/

    if (!ARG_CNT(a)) {  
        ret = CMD_USAGE;
    }
    else{
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "ber", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.mode), NULL);
        parse_table_add(&pt, "vertical_max", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.vert_max), NULL);
        parse_table_add(&pt, "vertical_min", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.vert_min), NULL);
        parse_table_add(&pt, "vertical_step", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.vstep), NULL);
        parse_table_add(&pt, "horizontal_max", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.horz_max), NULL);
        parse_table_add(&pt, "horizontal_min", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.horz_min), NULL);
        parse_table_add(&pt, "horizontal_step", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.hstep), NULL);
        parse_table_add(&pt, "sample_time", PQ_DFL | PQ_INT, (void *)(0),
                        &(eyescan_options.timeout_in_milliseconds), NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0), &access_param.lane, NULL);
        parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0), &phy_unit, NULL);
        parse_table_add(&pt, "sys_lane_mask", PQ_DFL | PQ_INT, (void *)(0), &sys_lane_mask, NULL);
        parse_table_add(&pt, "mode", PQ_STRING, 0, &mode_str, NULL);
        parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("ERROR: could not parse parameters\n");
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
    
        if (mode_str) {
            if (0 == sal_strcasecmp(mode_str, "fast")){
                mode = phymodEyescanModeFast;
            } 
            else if (0 == sal_strcasecmp(mode_str, "lowBER")){
                mode = phymodEyescanModeLowBER;
            }
            else{
                mode = phymodEyescanModeCount;
            }
        }
        if(phy_unit == 0){
            access_param.phyn = 0; /* internal */
        } else { /* otherwise most ext. */
            access_param.phyn = -1; /* internal */
        }

        access_param.sys_side = PORTMOD_SIDE_LINE; 
        if (if_str) {
            if (sal_strcasecmp(if_str, "sys") == 0) {
                access_param.sys_side = PORTMOD_SIDE_SYSTEM; 
            } else if (sal_strcasecmp(if_str, "line") == 0) {
                access_param.sys_side = PORTMOD_SIDE_LINE; 
            } else if (if_str[0] != 0) {
                cli_out("InterFace must be sys or line.\n");
                return CMD_FAIL;
            }
        } 

        parse_arg_eq_done(&pt);
        if(MAX_PORTS == port) {
            return CMD_USAGE;
        }
        if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
            cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
            return CMD_FAIL;       
        }

        ret = portmod_port_phy_lane_access_get(unit, port, &access_param, PHYMOD_CONFIG_MAX_CORES_PER_PORT, 
                                               phy_access, &nof_phys, NULL);
        if(ret != SOC_E_NONE){    
            cli_out("ERROR: get phy access failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;        
        }

        if(nof_phys == 0){
            cli_out("ERROR: Invalid lane# \n");
            return CMD_FAIL;
        }

        ret = portmod_port_interface_config_get(unit, port, &interface_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY);
        if(ret != SOC_E_NONE){    
            cli_out("ERROR: get interface config failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;        
        }        
        for (i = 0;i < PHYMOD_CONFIG_MAX_CORES_PER_PORT;i++){
            line_rates[i] = interface_config.speed;
            port_ids[i]   = port;
        }

        /* if the phys/lane are identical, run eyescan on only once. */
        if ( nof_phys == 3 ){
            if( !sal_memcmp( &phy_access[0],&phy_access[1], sizeof(phymod_phy_access_t)) &&
                !sal_memcmp( &phy_access[0],&phy_access[2], sizeof(phymod_phy_access_t))){
            
                cli_out("Removing repeated entries.\n");     
                 nof_phys = 1;
            }
        }

        /* Check for System Side request. */
        if( sys_lane_mask != 0xFFFFFFFF) {
            cli_out("sys_lane_mask no longer support here, use if=sys  lane=<lane#> \n");
            return CMD_FAIL;        
        }

        ret = phymod_diag_eyescan_run(phy_access, unit, port_ids, line_rates, nof_phys, mode, &eyescan_options);
        if(ret != SOC_E_NONE){    
            cli_out("ERROR: run eyescan diagnostic failed: %s\n", soc_errmsg(ret));     
            return CMD_FAIL;        
        }
    }
    
    return ret;
}

STATIC void 
portmod_eyescan_usage(int unit){
    cli_out("command:\n");
    cli_out("  eyescan port=<port-id> [vertical_max=<value> vertical_min=<value> vertical_step=<value>\n");
    cli_out("          horizontal_max=<value> horizontal_min=<value> horizontal_step=<value>\n");
    cli_out("          sample_time=<value> ber=<value> lane=<value> mode=<fast/lowBER>]\n");
    cli_out("          if=<sys/line> \n");
    cli_out("example:\n");
    cli_out("  portmod eyescan port=1 mode=fast\n");
}


STATIC cmd_result_t
portmod_regdump_diag(int unit, args_t *a){
    parse_table_t pt;
    int port;
    int ret = CMD_OK;
    char *c;

    if (NULL == (c = ARG_CUR(a))) {
        cli_out("ERROR: please input port ID: port=<id>\n");
        return CMD_FAIL;
    }
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "port", PQ_INT,(void *)MAX_PORTS, &port, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("ERROR: invalid option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);
    if(MAX_PORTS == port) {
        return CMD_USAGE;
    }
    if(!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
        cli_out("ERROR: Port %d is not valid for unit %d\n", port, unit);     
        return CMD_FAIL;       
    }
    ret = portmod_port_diag_phy_regdump(unit, port);
    if(SOC_E_NONE != ret){      
        cli_out("ERROR: Getting phy status failed: %s\n", soc_errmsg(ret));     
        return CMD_FAIL;                                    
    }     

    return ret;
}


STATIC void 
portmod_regdump_usage(int unit){
    cli_out("command:\n");
    cli_out("  regdump port=<port-id>\n");
    cli_out("example:\n");
    cli_out("  portmod regdump port=1\n");
}

#endif


STATIC cmd_result_t 
portmod_diag_pack_usage(int unit, args_t *a);

/*portmod diag pack*/ 
const portmod_diag_table_t portmod_diag_pack[] = {
    /*CMD_NAME,    CMD_ACTION,                     CMD_SHORT_DESC,              CMD_USAGE,                      CMD_ID*/
    {"usage",     portmod_diag_pack_usage, "display usage",         NULL,                     0},
    {"info",      portmod_info_diag,       "show SW state",         portmod_info_usage,       0},
    {"status",    portmod_status_diag,     "show HW status",        portmod_status_usage,     0},
    {"fc",        portmod_fc_diag,         "show FC info",          portmod_fc_usage,         0},
    {"autoneg",   portmod_autoneg_diag,    "show autoneg info",     portmod_autoneg_usage,    0},
    {"core",      portmod_core_diag,       "show core info",        portmod_core_usage,       0},
    {"phy",       portmod_phy_diag,        "show lane info",        portmod_phy_usage,        0},
    {"prbs",      portmod_prbs_diag,       "run prbs test",         portmod_prbs_usage,       0},
    {"eyescan",   portmod_eyescan_diag,    "run eyescan",           portmod_eyescan_usage,    0},
    {"regdump",   portmod_regdump_diag,    "dump all registers",    portmod_regdump_usage,    0},
    {NULL,        NULL,                    "NULL",                  NULL,                     0}
};


cmd_result_t
cmd_portmod_diag(int unit, args_t *a){
    char *c;
    cmd_result_t ret = CMD_OK;
    int i, diag_count;
    const portmod_diag_table_t *diag_pack;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
    if (NULL == (c = ARG_GET(a))){
        return CMD_USAGE;
    }

    diag_pack = portmod_diag_pack;
    diag_count = sizeof(portmod_diag_pack)/sizeof(portmod_diag_table_t);
    for (i = 0;i < diag_count;i++){
        if(diag_pack[i].cmd_name && !sal_strcasecmp(c, diag_pack[i].cmd_name)){
            if (NULL != diag_pack[i].action_func){
                ret = diag_pack[i].action_func(unit, a);
            }
            if ((CMD_USAGE == ret)&&(NULL != diag_pack[i].usage_func)){
                diag_pack[i].usage_func(unit);
                return CMD_FAIL;
            }
            return ret;
        }
    }

    return CMD_USAGE;    
}


STATIC cmd_result_t 
portmod_diag_pack_usage(int unit, args_t *a){
    int i, diag_count;
    const portmod_diag_table_t *diag_pack;
        

    cli_out("PortMod Diagnostic Pack Usage:\n");
    cli_out("------------------------------\n");

    diag_pack = portmod_diag_pack;
    diag_count = sizeof(portmod_diag_pack)/sizeof(portmod_diag_table_t);
    for (i = 0;i < diag_count;i++){
        if (NULL != diag_pack[i].usage_func){
            diag_pack[i].usage_func(unit);
        }
    }
    
    return CMD_OK;
}

