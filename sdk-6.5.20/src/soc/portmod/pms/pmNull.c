/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PMNULL_SUPPORT

typedef struct pm_null_saved_data_s
{
    int higig_mode[SOC_MAX_NUM_PORTS];
    int higig2_mode[SOC_MAX_NUM_PORTS];
} pm_null_saved_data_t;

static pm_null_saved_data_t *saved_data[SOC_MAX_NUM_DEVICES];


int pmNull_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
        
    SOC_INIT_FUNC_DEFS;

    *is_supported =  (interface == SOC_PORT_IF_NULL)? 1 : 0;
    
    SOC_FUNC_RETURN; 
    
}

/*
 * Functions:
 *    _pm_null_port_ability_get
 *
 * Purpose:
 *    Some dummy routines for null and no
 *      connection PHY drivers.
 */

static int
_pm_null_port_ability_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{

    int max_speed = SOC_INFO(unit).port_speed_max[port];

    if ( max_speed == 0) {
        if (IS_GE_PORT(unit,port)) {
             max_speed = 1000;
        } else if (IS_FE_PORT(unit,port)) {
             max_speed = 100;
        }
    }

    switch (max_speed) {
    case 127000:
        ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
        /* fall through */
    case 120000:
        ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
        /* fall through */
    case 106000:
        ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
        /* fall through */
    case 100000:
        ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
        /* fall through */
    case 53000:
        ability->speed_full_duplex |= SOC_PA_SPEED_53GB;
        /* fall through */
    case 50000:
        ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
        /* fall through */
    case 42000:
        ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
        /* fall through */
    case 40000:
        ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
        /* fall through */
    case 32000:
        ability->speed_full_duplex |= SOC_PA_SPEED_32GB;
        /* fall through */
    case 30000:
        ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
        /* fall through */
    case 27000:
        ability->speed_full_duplex |= SOC_PA_SPEED_27GB;
        /* fall through */
    case 25000:
        ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
        /* fall through */
    case 24000:
        ability->speed_full_duplex |= SOC_PA_SPEED_24GB;
        /* fall through */
    case 21000:
        ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
        /* fall through */
    case 20000:
        ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
        /* fall through */
    case 16000:
        ability->speed_full_duplex |= SOC_PA_SPEED_16GB;
        /* fall through */
    case 15000:
        ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
        /* fall through */
    case 13000:
        ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
        /* fall through */
    case 12500:
        ability->speed_full_duplex |= SOC_PA_SPEED_12P5GB;
        /* fall through */
    case 12000:
        ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
        /* fall through */
    case 11000:
        ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
        /* fall through */
    case 10000:
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        /* fall through */
    case 6000:
        ability->speed_full_duplex |= SOC_PA_SPEED_6000MB;
        /* fall through */
    case 5000:
        ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
        /* fall through */
    case 3000:
        ability->speed_full_duplex |= SOC_PA_SPEED_3000MB;
        /* fall through */
    case 2500:
        ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        /* fall through */
    case 1000:
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        /* fall through */
    case 100:
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        /* fall through */
    case 10:
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        /* fall through */
    default:
        break;
    }

    ability->speed_half_duplex = 0x0;

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) ||
        IS_CE_PORT(unit, port)) {
        ability->interface = SOC_PA_INTF_XGMII;
    } else if (IS_GE_PORT(unit, port)) {
        ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_GMII;
    } else {
        ability->interface = SOC_PA_INTF_MII;
    }

    ability->loopback = SOC_PA_LB_NONE | SOC_PA_LB_MAC | SOC_PA_LB_PHY;

    return SOC_E_NONE;
}

int pmNull_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    pm_info->type         = pm_add_info->type;
    pm_info->unit         = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* initialize saved_data */
    if (unit < SOC_MAX_NUM_DEVICES) {
        if (NULL == saved_data[unit]) {
            saved_data[unit] = sal_alloc(sizeof(*saved_data[unit]),
                                         "pmNull saved data");
            if (NULL == saved_data[unit]) {
                return SOC_E_MEMORY;
            }
            sal_memset(saved_data[unit], 0, sizeof(*saved_data[unit]));
        }

    }

    SOC_FUNC_RETURN; 
}


int pmNull_pm_destroy(int unit, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */
    sal_free(saved_data[unit]);
    saved_data[unit] = NULL;
        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_detach(int unit, int port, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}


int pmNull_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}

int pmNull_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
   
    *enable = 0; 

        
    SOC_FUNC_RETURN; 
    
}

int pmNull_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN; 
}

int pmNull_port_higig2_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    SOC_INIT_FUNC_DEFS;

    saved_data[unit]->higig2_mode[port] = mode; 
    SOC_FUNC_RETURN; 
}

int pmNull_port_higig2_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    SOC_INIT_FUNC_DEFS;

    *mode = saved_data[unit]->higig2_mode[port]; 
    SOC_FUNC_RETURN; 
}

int pmNull_port_higig_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    SOC_INIT_FUNC_DEFS;

    saved_data[unit]->higig_mode[port] = mode; 
    SOC_FUNC_RETURN; 
}

int pmNull_port_higig_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    SOC_INIT_FUNC_DEFS;

    *mode = saved_data[unit]->higig_mode[port]; 
    SOC_FUNC_RETURN; 
}

int pmNull_port_encap_set (int unit, int port, pm_info_t pm_info, 
                                int flags, portmod_encap_t encap)
{
    return SOC_E_NONE;
}

int pmNull_port_encap_get (int unit, int port, pm_info_t pm_info, 
                            int *flags, portmod_encap_t *encap)
{
    return SOC_E_NONE;
}

int pmNull_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}

int pmNull_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
}

int pmNull_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    *interface = SOC_PORT_IF_NULL;
    return SOC_E_NONE;
}

int pmNull_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
    
}

int pmNull_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        

    SOC_FUNC_RETURN; 
    
}


/* Port RX MAC ENABLE set/get*/
int pmNull_port_rx_mac_enable_set(int unit, int port,
                                  pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}


int pmNull_port_rx_mac_enable_get(int unit, int port,
                                  pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    *enable = 0;

    SOC_FUNC_RETURN;
}


int pmNull_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */
    *nof_cores = 0;

    if (NULL != is_most_ext) *is_most_ext = 1;

    SOC_FUNC_RETURN; 
    
}


int pmNull_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */
    /* This code changes are required to handle phy_acess_get from portmod_common.c call during flex port */
    *nof_phys = 0;

    if (NULL != is_most_ext) *is_most_ext = 1;

    SOC_FUNC_RETURN; 
}

int pmNull_port_link_get(int unit, int port, pm_info_t pm_info, int flags,  int* link)
{
    SOC_INIT_FUNC_DEFS;
    
    *link = 0; 

    SOC_FUNC_RETURN; 
}

int pmNull_port_mac_reset_check(int unit, int port, pm_info_t pm_info,
                                int enable, int* reset)
{
    SOC_INIT_FUNC_DEFS;

    *reset = 0;

    SOC_FUNC_RETURN;
}

/*!
 * pmNull_port_duplex_set
 *
 * @brief duplex set.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        -
 */
int pmNull_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return (0);
}


/*!
 * pmNull_port_duplex_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        -
 */
int pmNull_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    *enable=1;
    return (0);
}

int pmNull_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{
    an_status->enabled = 0;
    return (0);
}

int pmNull_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    return (0);
}

int pmNull_port_vlan_tag_set(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (0);
}

int pmNull_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (0);
}

int pmNull_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    return (0);
}

int pmNull_port_eee_get(int unit, int port, pm_info_t pm_info, portmod_eee_t* eee)
{
    return (0);
}

int pmNull_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    return (SOC_E_NONE);
}

int pmNull_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (SOC_E_NONE);
}

int pmNull_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (SOC_E_NONE);
}

int pmNull_port_autoneg_get (int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                             phymod_autoneg_control_t* an)
{
    an->enable = 0;
    return (SOC_E_NONE);
}

int pmNull_port_autoneg_set (int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                             const phymod_autoneg_control_t* an)
{
    return (SOC_E_NONE);
}

int pmNull_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability)
{
    return (SOC_E_NONE);
}

int pmNull_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability)
{
    return (SOC_E_NONE);
}

int pmNull_port_pause_control_set(int unit, int port, pm_info_t pm_info, 
                                  const portmod_pause_control_t* control)
{
    return (SOC_E_NONE);
}

int pmNull_port_pause_control_get(int unit, int port, pm_info_t pm_info, 
                                  portmod_pause_control_t* control)
{
    return (SOC_E_NONE);
}

int pmNull_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (SOC_E_NONE);
}


int pmNull_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (SOC_E_NONE);
}

int pmNull_port_ability_local_get (int unit, int port, pm_info_t pm_info, int phy_flags, 
                                   portmod_port_ability_t* ability)
{
    return(_pm_null_port_ability_get(unit, port, ability));
}

int pmNull_port_max_packet_size_set (int unit, int port,
                                     pm_info_t pm_info, int value)
{
    return (SOC_E_NONE);
}

int pmNull_port_max_packet_size_get (int unit, int port,
                                     pm_info_t pm_info, int* value)
{
    *value = 0x3fe8; 
    return (SOC_E_NONE);
}

int pmNull_port_local_fault_status_get(int unit, int port,
                                       pm_info_t pm_info, int* value)
{
    *value=0;
    return (SOC_E_NONE);
}

int pmNull_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_local_fault_control_t* control)
{
    return (SOC_E_NONE);
}

int pmNull_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_local_fault_control_t* control)
{
    control->enable = 0;
    control->drop_tx_on_fault = 0;
    return (SOC_E_NONE);
}

int pmNull_port_remote_fault_status_get(int unit, int port,
                                        pm_info_t pm_info, int* value)
{
    *value=0;
    return (SOC_E_NONE);
}

int pmNull_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_remote_fault_control_t* control)
{
    return (SOC_E_NONE);
}

int pmNull_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_remote_fault_control_t* control)
{
    control->enable = 0;
    control->drop_tx_on_fault = 0;
    return (SOC_E_NONE);
}

int pmNull_port_check_legacy_phy (int unit, int port, pm_info_t pm_info, int *legacy_phy)
{
    *legacy_phy = 0;
    return (SOC_E_NONE);
}

int pmNull_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    return (SOC_E_NONE);
}

int pmNull_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    return (SOC_E_NONE);
}

int pmNull_port_tx_average_ipg_set (int unit, int port, pm_info_t pm_info, int value)
{
    return (SOC_E_NONE);
}

int pmNull_port_tx_average_ipg_get (int unit, int port, pm_info_t pm_info, int* value)
{
    return (SOC_E_NONE);
}

int pmNull_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    return (SOC_E_NONE);
}

int pmNull_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    return (SOC_E_NONE);
}

int pmNull_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    return (SOC_E_NONE);
}

int pmNull_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                         const portmod_port_interface_config_t *f_intf_config,
                         const portmod_port_init_config_t      *f_init_config,
                         const phymod_operation_mode_t phy_op_mode)
{
    return (SOC_E_NONE);
}

int pmNull_port_medium_config_set(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    return (SOC_E_NONE);
}

int pmNull_port_medium_config_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    return (SOC_E_NONE);
}

int pmNull_port_medium_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t* medium)
{
    return (SOC_E_NONE);
}

int pmNull_port_drain_cell_get(int unit, int port,
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return SOC_E_NONE;
}

int pmNull_port_drain_cell_stop (int unit, int port,
           pm_info_t pm_info, const portmod_drain_cells_t *drain_cells)
{
    return (SOC_E_NONE);
}

int pmNull_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    return SOC_E_NONE;
}

int pmNull_port_egress_queue_drain_get(int unit, int port,
                   pm_info_t pm_info, uint64 *ctrl, int *rx)
{
    return SOC_E_NONE;
}

int pmNull_port_mac_reset_set(int unit, int port,
                   pm_info_t pm_info, int val)
{
    return SOC_E_NONE;
}

int pmNull_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    *val = 0;
    return SOC_E_NONE;
}

int pmNull_port_txfifo_cell_cnt_get(int unit, int port,
                       pm_info_t pm_info, uint32* fval)
{
     *fval =0;
      return SOC_E_NONE;
}

int pmNull_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    return SOC_E_NONE;
}

int pmNull_port_drain_cells_rx_enable (int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return SOC_E_NONE;
}

int pmNull_port_egress_queue_drain_rx_en(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return SOC_E_NONE;
}

int pmNull_port_mac_ctrl_set(int unit, int port,
                   pm_info_t pm_info, uint64 ctrl)
{
    return SOC_E_NONE;
}

int pmNull_port_flow_control_set (int unit, int port, pm_info_t pm_info,
                                  int merge_mode_en, int parallel_fc_en)
{
    return SOC_E_NONE;
}

int pmNull_port_mode_set(int unit, soc_port_t port,
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    return SOC_E_NONE;
}

int pmNull_port_mode_get(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    return SOC_E_NONE;
}

int pmNull_port_speed_config_set(int unit, int port, pm_info_t pm_info,
                                 const portmod_speed_config_t* speed_config)
{
    return SOC_E_NONE;
}

int pmNull_port_speed_config_get(int unit, int port, pm_info_t pm_info,
                                 portmod_speed_config_t* speed_config)
{
    return SOC_E_NONE;
}

int pmNull_port_speed_ability_local_get(int unit, int port, pm_info_t pm_info,
                                        int max_num_abilities,
                                        portmod_port_speed_ability_t* abilities,
                                        int* num_abilities)
{
    return SOC_E_NONE;
}

int pmNull_port_update(int unit, int port, pm_info_t pm_info,
                       const portmod_port_update_control_t* update_control)
{
    return SOC_E_NONE;
}

#endif /* PORTMOD_PMNULL_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
