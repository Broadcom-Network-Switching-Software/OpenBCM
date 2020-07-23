/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC PORTS MANAGER
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#define LINKS_IN_QUAD 4
#include <shared/bsl.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dcmn/error.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/portmode.h>
#include <soc/drv.h>
#include <soc/phy/phyctrl.h>
#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <soc/ll.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/ARAD/NIF/common_drv.h>
#include <soc/dpp/ARAD/NIF/ilkn_drv.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#include <bcm/link.h>

extern CONST mac_driver_t soc_xmac_driver;
extern CONST mac_driver_t soc_cmac_driver;
extern CONST mac_driver_t soc_ilkn_driver;
extern CONST mac_driver_t soc_null_driver;

extern int soc_xgxs_in_reset(int unit, soc_port_t port);
extern int soc_wc_xgxs_in_reset(int unit, soc_port_t port, int reg_idx);


STATIC soc_error_t
soc_dpp_mac_probe(int unit, soc_port_t port, CONST mac_driver_t** md)
{
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    switch(interface) {
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XLAUI: 
            *md = &soc_xmac_driver;
            break;
        case SOC_PORT_IF_CAUI:
            *md = &soc_cmac_driver;
            break;
        case SOC_PORT_IF_ILKN:
            *md = &soc_ilkn_driver;
            break;
        case SOC_PORT_IF_CPU:
        case SOC_PORT_IF_TM_INTERNAL_PKT:
        case SOC_PORT_IF_RCY:
        case SOC_PORT_IF_OLP:
        case SOC_PORT_IF_ERP:
        case SOC_PORT_IF_OAMP:
        case SOC_PORT_IF_SAT:
        case SOC_PORT_IF_IPSEC:
            *md = &soc_null_driver;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface isn't supported")));
            break;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC soc_error_t
soc_pm_serdes_ports_in_quad_get(int unit, uint32 quad, soc_pbmp_t *ports_in_quad) 
{
    uint32 phy_port, pquad, valid;
    soc_port_t port;
    soc_pbmp_t phy_ports;
    soc_port_if_t interface_type;

    SOCDNX_INIT_FUNC_DEFS;

    
    if(SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("This functionality not supported by jericho")));
    }

    SOC_PBMP_CLEAR(*ports_in_quad);
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &valid));
        if(!valid) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (SOC_PORT_IF_CPU != interface_type) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
            SOC_PBMP_ITER(phy_ports, phy_port) {
                pquad = (phy_port - 1) / LANES_IN_QUAD;
                if (quad == pquad) {
                    SOC_PBMP_PORT_ADD(*ports_in_quad, port);
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_pm_last_initialized_in_serdes_quads_get(int unit, soc_port_t port, uint32 *last_in_use) 
{
    soc_port_t port_i;
    soc_pbmp_t quads_in_use, quads_in_use_i, valid_ports;
    uint32 is_valid, first_phy_port, first_phy_port_i, initialized_i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    *last_in_use = 1;

    SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &valid_ports));
    SOC_PBMP_ITER(valid_ports, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &first_phy_port_i));

        
        if (first_phy_port_i == first_phy_port) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port_i, &initialized_i));
        if (!initialized_i) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port_i, &quads_in_use_i));
        SOC_PBMP_AND(quads_in_use_i, quads_in_use);
        if (SOC_PBMP_NOT_NULL(quads_in_use_i)) {
            
            *last_in_use = 0;
            break;
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN
}

soc_error_t
soc_pm_serdes_quads_in_use_get(int unit, soc_port_t port, soc_pbmp_t *quads_in_use) 
{
    uint32 phy_port, quad, is_valid;
    soc_port_if_t interface_type;
    soc_pbmp_t phy_ports;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("This functionality not supported by jericho")));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOC_PBMP_CLEAR(*quads_in_use);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (SOC_PORT_IF_CPU == interface_type) {
        SOC_PBMP_CLEAR(*quads_in_use);
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
        SOC_PBMP_ITER(phy_ports, phy_port) {
            if (phy_port == 0) {
                continue;
            }
            quad = (phy_port - 1) / LANES_IN_QUAD;
            SOC_PBMP_PORT_ADD(*quads_in_use, quad);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_pm_mac_ability_get(int unit, soc_port_t port, soc_port_ability_t *mac_ability)
{
    CONST mac_driver_t *md;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_ABILITY_LOCAL_GET(md, unit, port, mac_ability));

exit:
    SOCDNX_FUNC_RETURN; 
}


 
soc_error_t
soc_pm_tx_remote_fault_enable_set(int unit, soc_port_t port, int enable)
{
    CONST mac_driver_t *md;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    if(interface != SOC_PORT_IF_CAUI){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is not CAUI port"),port));
    }
    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_TX_ENABLE, enable));

exit:
    SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_pm_tx_remote_fault_enable_get(int unit, soc_port_t port, int *enable)
{
    CONST mac_driver_t *md;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    if(interface != SOC_PORT_IF_CAUI){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is not CAUI port"),port));
    }
    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_TX_ENABLE, enable));

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_pm_init(int unit, soc_pbmp_t all_phy_pbmp, uint32 first_direct_port)
{
    int blk, blktype; 
    soc_port_t phy_port, logical_port;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_INFO(unit).physical_port_offset = first_direct_port;
    SOC_PBMP_ITER(all_phy_pbmp, phy_port) {
        logical_port = first_direct_port + phy_port - 1;

        
        SOC_INFO(unit).port_l2p_mapping[logical_port] = phy_port;
        
        
        blk = SOC_DRIVER(unit)->port_info[phy_port].blk;
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        SOC_INFO(unit).port_type[logical_port] = blktype;

        
        SOC_INFO(unit).block_port[blk] = logical_port;
        
        
        SOC_PBMP_PORT_ADD(SOC_INFO(unit).physical_pbm, phy_port);
    }

    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_mac_reset_set(int unit, soc_port_t port, uint32 is_in_reset)
{
    soc_port_if_t interface;
    soc_pbmp_t quads_in_use;
    uint32 quad, internal_quad, reg_val32, phy_port, in_block_index;
    uint32 phy_port_mode, core_port_mode, offset;
    soc_port_t direct_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    
    if(SOC_PORT_IF_TM_INTERNAL_PKT == interface) {
        
    } else if(SOC_PORT_IF_ILKN == interface) {

        SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        
        SOC_PBMP_ITER(quads_in_use, quad){
                phy_port = quad*LANES_IN_QUAD + 1;
                direct_port = phy_port + SOC_INFO(unit).physical_port_offset;
                
                internal_quad = SOC_DRIVER(unit)->port_info[phy_port].bindex / LINKS_IN_QUAD;
                SOCDNX_IF_ERR_EXIT(READ_PORT_MAC_CONTROLr(unit, direct_port, &reg_val32));
#if defined(INCLUDE_KBP) && defined(BCM_88660_A0)
                if(SOC_IS_ARADPLUS(unit) && (offset == 1) && (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)){
                    
                }
                else
#endif
                {
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, CMAC_RESETf, 1);
                }

                switch(internal_quad) {
                    case 0:
                        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_0_RESETf, 1);
                        break;
                    case 1:
                        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_1_RESETf, 1);
                        break;
                    case 2:
#if defined(INCLUDE_KBP) && defined(BCM_88660_A0)
                        if(SOC_IS_ARADPLUS(unit) && (offset == 1) && (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)){
                            
                        }
                        else
#endif
                        {
                            soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_2_RESETf, 1);
                        }
                        break;
                    default:
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d reset invalid quad"),port));
                }
                SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, direct_port, reg_val32));
        }

    } else if(SOC_PORT_IF_CAUI == interface){

        
        SOCDNX_IF_ERR_EXIT(READ_PORT_MODE_REGr(unit, port, &reg_val32));
        soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, MAC_MODEf, MAC_MODE_AGGR);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MODE_REGr(unit, port, reg_val32));

        
        SOCDNX_IF_ERR_EXIT(READ_PORT_MAC_CONTROLr(unit, port, &reg_val32));
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, CMAC_RESETf, is_in_reset);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_0_RESETf, 1);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_1_RESETf, 1);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_2_RESETf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, port, reg_val32));

    } else { 
        if(SOC_BLK_CLP == SOC_PORT_TYPE(unit,port) || SOC_BLK_XLP == SOC_PORT_TYPE(unit,port)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_in_block_index_get(unit, port, &in_block_index));
            internal_quad = in_block_index/LANES_IN_QUAD;;

            
            SOCDNX_IF_ERR_EXIT(soc_common_drv_core_port_mode_get(unit, port, &core_port_mode));
            SOCDNX_IF_ERR_EXIT(soc_common_drv_phy_port_mode_get(unit, port, &phy_port_mode));

            SOCDNX_IF_ERR_EXIT(READ_PORT_MODE_REGr(unit, port, &reg_val32));
            soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, MAC_MODEf, MAC_MODE_INDEP);
            switch (internal_quad) {
                case 0:
                    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, XPORT_0_PHY_PORT_MODEf, phy_port_mode);
                    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, XPORT_0_CORE_PORT_MODEf, core_port_mode);
                    break;
                case 1:
                    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, XPORT_1_PHY_PORT_MODEf, phy_port_mode);
                    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, XPORT_1_CORE_PORT_MODEf, core_port_mode);
                    break;
                case 2:
                    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, XPORT_2_PHY_PORT_MODEf, phy_port_mode);
                    soc_reg_field_set(unit, PORT_MODE_REGr, &reg_val32, XPORT_2_CORE_PORT_MODEf, core_port_mode);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Wrong internal quad id %d"), internal_quad));
                    break;
            }
            SOCDNX_IF_ERR_EXIT(WRITE_PORT_MODE_REGr(unit, port, reg_val32));

            sal_usleep(50);

            SOCDNX_IF_ERR_EXIT(READ_PORT_MAC_CONTROLr(unit, port, &reg_val32));
            soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, CMAC_RESETf, 1);

            switch(internal_quad) {
                case 0:
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_0_RESETf, is_in_reset);
                    break;
                case 1:
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_1_RESETf, is_in_reset);
                    break;
                case 2:
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_2_RESETf, is_in_reset);
                    break;
                
                
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d reset invalid quad"),port));
            }

            SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, port, reg_val32));
        }

    }

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC soc_error_t
soc_pm_mac_loopback_fifo_reset_set(int unit, soc_port_t port, uint32 is_in_reset)
{
    soc_port_if_t interface;
    uint32 internal_quad, reg_val32, in_block_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(SOC_PORT_IF_ILKN != interface && SOC_PORT_IF_CAUI != interface) {

        if(SOC_BLK_CLP == SOC_PORT_TYPE(unit,port) || SOC_BLK_XLP == SOC_PORT_TYPE(unit,port)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_in_block_index_get(unit, port, &in_block_index));
            internal_quad = in_block_index/LANES_IN_QUAD;;

            SOCDNX_IF_ERR_EXIT(READ_PORT_MAC_CONTROLr(unit, port, &reg_val32));

            switch(internal_quad) {
                case 0:
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_0_LINE_LPBK_RESETf, is_in_reset);
                    break;
                case 1:
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_1_LINE_LPBK_RESETf, is_in_reset);
                    break;
                case 2:
                    soc_reg_field_set(unit, PORT_MAC_CONTROLr, &reg_val32, XMAC_2_LINE_LPBK_RESETf, is_in_reset);
                    break;
                
                
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d reset invalid quad"),port));
            }

            SOCDNX_IF_ERR_EXIT(WRITE_PORT_MAC_CONTROLr(unit, port, reg_val32));
        }

    }

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_port_remove(int unit, soc_port_t port)
{
    uint32 is_valid, is_initialized;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
    if(is_initialized) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_DISABLED, (_BSL_SOCDNX_MSG("Can't remove initialized port %d"),port));
    }


    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_remove(unit, port));
    
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_ports_init(int unit, soc_pbmp_t pbmp, soc_pbmp_t* okay_ports)
{
    soc_port_t port, phy_port, direct_port, master_port;
    uint32 is_valid, is_initialized, quad;
    soc_pbmp_t quads_in_use, quads_oor;
    soc_port_if_t interface;
    CONST mac_driver_t* md;
    uint32 internal_quad, base_quad, is_master;
    int locked;
    soc_pbmp_t masters_pbmp;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    locked = 0;
    
    SOC_PBMP_ITER(pbmp, port){
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
        if(!is_valid) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Can't init invalid port %d"), port));
        }

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
        if(is_initialized && !SOC_WARM_BOOT(unit) ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port is already initialized %d"), port));
        }
    }
    
    SOC_PBMP_ITER(pbmp, port) {
        
        SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_get(unit, &quads_oor));

        switch(SOC_INFO(unit).port_type[port]) {
        case SOC_BLK_XLP:
            SOC_PBMP_ITER(quads_in_use, quad){
                if(!SOC_PBMP_MEMBER(quads_oor, quad)) {
                    if (!SOC_WARM_BOOT(unit)){
                        SOCDNX_IF_ERR_EXIT(soc_xgxs_reset(unit, port));
                        }
                    SOC_PBMP_PORT_ADD(quads_oor, quad);
                    }
                }
            break;
        case SOC_BLK_CLP:
            SOC_PBMP_ITER(quads_in_use, quad){
                if(!SOC_PBMP_MEMBER(quads_oor, quad)) {
                    phy_port = quad*LANES_IN_QUAD + 1;
                    
                    direct_port = phy_port + SOC_INFO(unit).physical_port_offset;
                    internal_quad = SOC_DRIVER(unit)->port_info[phy_port].bindex / LINKS_IN_QUAD;
                    if (!SOC_WARM_BOOT(unit)){
                        SOCDNX_IF_ERR_EXIT(soc_wc_xgxs_reset(unit, direct_port, internal_quad));
                        }
                    SOC_PBMP_PORT_ADD(quads_oor, quad);
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
                    if(SOC_PORT_IF_ILKN != interface){
                        
                        base_quad = quad - internal_quad;
                        if(!SOC_PBMP_MEMBER(quads_oor, base_quad)) {
                            if (!SOC_WARM_BOOT(unit)){
                                SOCDNX_IF_ERR_EXIT(soc_wc_xgxs_reset(unit, direct_port, 0));
                                }
                            SOC_PBMP_PORT_ADD(quads_oor, base_quad);
                            }
                        }
                    }
                }

            break;
        default:
            break;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_set(unit,quads_oor));
        
        SOCDNX_IF_ERR_EXIT(soc_phy_port_init(unit, port));       
    }

    
    SOC_PBMP_CLEAR(masters_pbmp);
    SOC_PBMP_ITER(pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
        if(is_master) {
            SOC_PBMP_PORT_ADD(masters_pbmp, port);
        }
    }

    MIIM_LOCK(unit);
    locked = 1;
    
    
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_pbm_probe_init(unit,masters_pbmp,okay_ports));

    if (!SOC_WARM_BOOT(unit)) {
        
        SOC_PBMP_ITER(*okay_ports, port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
            if (interface_type == SOC_PORT_IF_RXAUI || interface_type == SOC_PORT_IF_XAUI) {
                
                SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_FIRMWARE_MODE, 1)); 
            }
        }
    }
        
    
    if (!SOC_WARM_BOOT(unit)) {
        
        SOC_PBMP_ITER(*okay_ports, port) {
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, port, 0));
        }
    }

    SOC_PBMP_ITER(pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
        INT_PHY_SW_STATE(unit, port) = INT_PHY_SW_STATE(unit, master_port);
        EXT_PHY_SW_STATE(unit, port) = EXT_PHY_SW_STATE(unit, master_port);

        sal_memcpy(&SOC_PHY_INFO(unit, port),&SOC_PHY_INFO(unit, master_port),sizeof(soc_phy_info_t));

    }

    locked = 0;
    MIIM_UNLOCK(unit);
     
    if (!SOC_WARM_BOOT(unit)) {
        
        SOC_PBMP_ITER(*okay_ports, port) {
    
            
            SOCDNX_IF_ERR_EXIT(soc_pm_mac_reset_set(unit, port, 0 ));
        }
        
        sal_usleep(1000);
      
        SOC_PBMP_ITER(*okay_ports, port) {
    
            SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
            SOCDNX_IF_ERR_EXIT(MAC_INIT(md, unit, port));
    
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 1));
    
            
            SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, 0));
        }
    }

exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_port_deinit(int unit, soc_port_t port)
{
    uint32 quad, last_in_use=0;
    uint32 internal_quad, first_phy, fld_val[1];
    uint32 reg32_val, nof_lanes, no_frag, is_master;
    soc_reg_above_64_val_t reg_above_64;
    soc_pbmp_t quads_in_use, quads_oor;
    CONST mac_driver_t* md;
    soc_port_t phy_port, direct_port;
    soc_port_if_t interface_type;
    int locked, clp = 0; 
    SOCDNX_INIT_FUNC_DEFS;
    
    locked = 0;
    
    MIIM_LOCK(unit);
    locked = 1;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        
    nof_lanes = 0;
    while (((interface_type == SOC_PORT_IF_CAUI) || (interface_type == SOC_PORT_IF_ILKN))) {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
        
        if (nof_lanes == 0) {
            clp = (first_phy > 12 ? 1 : 0);
        }
        reg32_val = 0;
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_HWf, 0);
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_MDIOREGSf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MLD_CTRL_REGr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, clp)) , reg32_val));
        sal_sleep(1);
        reg32_val = 0;
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_HWf, 1);
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_MDIOREGSf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_MLD_CTRL_REGr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, clp)) , reg32_val));
        
        if ((interface_type == SOC_PORT_IF_ILKN) && (nof_lanes == 0)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));
            
            if (nof_lanes > 12) {
                
                clp = 1 - clp;
            }
            else {
                break;
            }
        } else {
            break;
        }
    }
    if (interface_type == SOC_PORT_IF_ILKN) {
        int field_len;

        
        field_len = soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, NIF_NO_FRAGf);
        if ((first_phy-1) >= field_len) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Port %d is invalid phy %d"),port, first_phy));
        }
        
        no_frag = 1;
        SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, REG_PORT_ANY, reg_above_64));
        *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64, NIF_NO_FRAGf);
        SHR_BITCLR(fld_val, (first_phy-1));
        *fld_val |= (no_frag<<(first_phy-1));
        soc_reg_above_64_field32_set(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64, NIF_NO_FRAGf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, REG_PORT_ANY, reg_above_64));
    }
    
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, port, 0));
    SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, 0));
    

    
    SOCDNX_IF_ERR_EXIT(soc_pm_last_initialized_in_serdes_quads_get(unit, port, &last_in_use));
    if(last_in_use) {
        SOCDNX_IF_ERR_EXIT(soc_pm_mac_reset_set(unit, port, 1 ));
        
    }

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
    if(is_master) {
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_detach(unit, port));
    }

    locked = 0;
    MIIM_UNLOCK(unit);
    
    
    if(last_in_use) {
        SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_get(unit, &quads_oor));

        switch(SOC_INFO(unit).port_type[port]) {
            case SOC_BLK_XLP:
                SOC_PBMP_ITER(quads_in_use, quad){
                    if(SOC_PBMP_MEMBER(quads_oor, quad)) {
                        SOCDNX_IF_ERR_EXIT(soc_xgxs_in_reset(unit, port));
                        SOC_PBMP_PORT_REMOVE(quads_oor, quad);
                    }
                }
                break;
            case SOC_BLK_CLP:
                SOC_PBMP_ITER(quads_in_use, quad){
                    if(SOC_PBMP_MEMBER(quads_oor, quad)) {
                        phy_port = quad*LANES_IN_QUAD + 1;
                        direct_port = phy_port + SOC_INFO(unit).physical_port_offset;
                        
                        internal_quad = SOC_DRIVER(unit)->port_info[phy_port].bindex / LINKS_IN_QUAD;
                        SOCDNX_IF_ERR_EXIT(soc_wc_xgxs_in_reset(unit, direct_port, internal_quad));
                        SOC_PBMP_PORT_REMOVE(quads_oor, quad);
                    }
                }
                break;
            default:
                break;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_set(unit,quads_oor));   
    }
    
exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_speed_set(int unit, soc_port_t port, int speed)
{
    uint32 is_valid, is_initialized, enabled;
    CONST mac_driver_t* md;
    soc_port_if_t interface_type;
    int locked = 0, is_virtual = 0, an_enable, an_done;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, speed));

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    switch (interface_type) {
    case SOC_PORT_IF_CPU:
    case SOC_PORT_IF_TM_INTERNAL_PKT:
    case SOC_PORT_IF_RCY:
    case SOC_PORT_IF_ERP:
    case SOC_PORT_IF_OLP:
    case SOC_PORT_IF_OAMP:
    case SOC_PORT_IF_SAT:
    case SOC_PORT_IF_IPSEC:
        is_virtual = 1;
        break;
    default: 
        is_virtual = 0;
    }
    
    if(is_initialized && !is_virtual) {
        
        SOCDNX_IF_ERR_EXIT(soc_pm_enable_get(unit, port,&enabled));
        if(enabled) {
            soc_pm_enable_set(unit,port,0);
        }

        if (SOC_DPP_ARAD_IS_HG_SPEED_ONLY(speed) && (!IS_HG_PORT(unit, port))){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d couln't set to higig speed only"),port));
        }

        
        MIIM_LOCK(unit);
        locked = 1;
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_auto_negotiate_get(unit, port, &an_enable, &an_done)); 
        if(an_enable) {
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_auto_negotiate_set(unit, port, FALSE)); 
        }
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_speed_set(unit, port, speed));
        locked = 0;
        MIIM_UNLOCK(unit);

        SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
        SOCDNX_IF_ERR_EXIT(MAC_SPEED_SET(md, unit, port, speed));

        
        if(enabled) {
            soc_pm_enable_set(unit,port,1);
        }
    }

exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_speed_get(int unit, soc_port_t port, int* speed)
{
    uint32 is_valid, is_initialized, is_virtual;
    soc_port_if_t interface_type;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    switch (interface_type) {
    case SOC_PORT_IF_CPU:
    case SOC_PORT_IF_TM_INTERNAL_PKT:
    case SOC_PORT_IF_RCY:
    case SOC_PORT_IF_ERP:
    case SOC_PORT_IF_OLP:
    case SOC_PORT_IF_OAMP:
    case SOC_PORT_IF_SAT:
    case SOC_PORT_IF_IPSEC:
        is_virtual = 1;
        break;
    default: 
        is_virtual = 0;
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));

    if(is_initialized && !is_virtual) {
        MIIM_LOCK(unit);
        rv = soc_phyctrl_speed_get(unit, port, speed);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, speed));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_pm_higig_set(int unit, soc_port_t port, uint32 higig)
{
    uint32 is_valid, is_initialized;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
    if(is_initialized) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_DISABLED, (_BSL_SOCDNX_MSG("Port %d can't set higig when port is initialized"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_set(unit, port, higig ? 1 : 0));

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_pm_enable_set(int unit, soc_port_t port, uint32 enable)
{
    CONST mac_driver_t* md;
    uint32 is_initialized;
    soc_port_if_t intf;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
    if(!is_initialized) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_DISABLED, (_BSL_SOCDNX_MSG("Can't enable uninitialized port %d"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &intf));

    if(enable) {
        if(SOC_PORT_IF_ILKN == intf) {
            SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, TRUE));
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, port, TRUE));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, port, TRUE));
            SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, TRUE));
        }
    } else {
        if(SOC_PORT_IF_ILKN == intf) {
            SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, FALSE));
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, port, FALSE));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, port, FALSE));
            SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, FALSE));
        }
    }

exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;

}




soc_error_t 
soc_pm_mac_enable_set(int unit, soc_port_t port, uint32 enable)
{
    CONST mac_driver_t* md;
    uint32 is_initialized;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
    if(!is_initialized) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_DISABLED, (_BSL_SOCDNX_MSG("Can't enable uninitialized port %d"),port));
    }
     SOCDNX_IF_ERR_EXIT(MAC_ENABLE_SET(md, unit, port, enable));
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_pm_mac_speed_set(int unit, soc_port_t port, uint32 speed)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_SPEED_SET(md, unit, port, speed));
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_pm_ilkn_dynamic_num_of_lanes_set(int unit, soc_port_t port, int lanes_number)
{
    soc_port_if_t intf;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &intf));
    if(intf != SOC_PORT_IF_ILKN){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("soc_pm_ilkn_dynamic_num_of_lanes_set can be called fust for ILKN interface")));
    }
    SOCDNX_IF_ERR_EXIT(soc_ilkn_drv_num_of_lanes_set(unit, port, lanes_number));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_ilkn_dynamic_num_of_lanes_get(int unit, soc_port_t port, uint32 *lanes_number)
{
    soc_port_if_t intf;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &intf));
    if(intf != SOC_PORT_IF_ILKN){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("soc_pm_ilkn_dynamic_num_of_lanes_get can be called fust for ILKN interface")));
    }
    SOCDNX_IF_ERR_EXIT(soc_ilkn_drv_num_of_lanes_get(unit, port, lanes_number));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_ilkn_id_get(int unit,soc_port_t port, uint32* ilkn_id)
{
    uint32 is_valid, offset;
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    if(SOC_PORT_IF_ILKN != interface) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Port %d isn't ilkn"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    *ilkn_id = offset;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_default_speed_get(int unit, soc_port_t port, int* speed)
{
    uint32 is_valid;
    soc_port_if_t interface;
    uint32 nof_lanes;
#ifdef BCM_LB_SUPPORT
    soc_pbmp_t modem_pbmp;
    int pcnt = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable)
    {
        SOCDNX_IF_ERR_EXIT(qax_lb_modem_ports_on_same_interface_get(unit, port, &modem_pbmp));
        SOC_PBMP_COUNT(modem_pbmp, pcnt);
        if ((pcnt != 0) && (interface != SOC_PORT_IF_LBG)) {
            *speed = 10000;
            SOC_EXIT;
        }
    }
#endif

    switch(interface) {
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_DNX_XAUI:
            *speed = 10000;
            break;
        case SOC_PORT_IF_SGMII:
            *speed = 1000;
            break;
        case SOC_PORT_IF_XLAUI: 
        case SOC_PORT_IF_XLAUI2: 
            *speed = 40000;
            break;
        case SOC_PORT_IF_CAUI:
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));
            *speed = nof_lanes == 12 ? 120000 : 100000;
            break;
        case SOC_PORT_IF_ILKN:
            *speed = 12500;
            break;
        case SOC_PORT_IF_TM_INTERNAL_PKT:
            *speed = 120000;
            break;
        case SOC_PORT_IF_SAT:
            *speed = 40000;
            break;
        case SOC_PORT_IF_IPSEC:
            *speed = 20000;
            break;
        case SOC_PORT_IF_CPU:
        case SOC_PORT_IF_RCY:
        case SOC_PORT_IF_ERP:
        case SOC_PORT_IF_OLP:
        case SOC_PORT_IF_OAMP:
        case SOC_PORT_IF_QSGMII:
            *speed = 1000;
            break;
        case SOC_PORT_IF_LBG:
            *speed = 10000;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface isn't supported")));
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_enable_get(int unit, soc_port_t port, uint32* enable)
{
    uint32 is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    if (SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        *enable = TRUE;
    } else {
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_get(unit, port, (int*)enable));
    }

exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_mac_loopback_set(int unit, soc_port_t port, uint32 lb)
{
    CONST mac_driver_t* md;
    soc_pbmp_t quads_in_use, ports_in_quad;
    uint32 quad, loopback, q_first_port_in_lb;
    soc_port_t p;
    SOCDNX_INIT_FUNC_DEFS;

    
    q_first_port_in_lb = 1;
    SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use));
    SOC_PBMP_ITER(quads_in_use, quad) {
        SOCDNX_IF_ERR_EXIT(soc_pm_serdes_ports_in_quad_get(unit, quad, &ports_in_quad));
        SOC_PBMP_ITER(ports_in_quad, p) {
            SOCDNX_IF_ERR_EXIT(soc_pm_mac_loopback_get(unit, p, &loopback));
            if(loopback) {
                q_first_port_in_lb = 0;
            }
        }
    }

    
    if(q_first_port_in_lb) {
        SOCDNX_IF_ERR_EXIT(soc_pm_mac_loopback_fifo_reset_set(unit, port, 1));
        sal_usleep(100);
        SOCDNX_IF_ERR_EXIT(soc_pm_mac_loopback_fifo_reset_set(unit, port, 0));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_LOOPBACK_SET(md, unit, port, lb));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_mac_loopback_get(int unit, soc_port_t port, uint32* lb)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_LOOPBACK_GET(md, unit, port, (int*)lb));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_link_status_get(int unit, soc_port_t port, int *is_link_up, int *is_latch_down)
{
    int val, rv;
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (SOC_PORT_IF_ILKN == interface_type) {
        SOCDNX_IF_ERR_EXIT(soc_ilkn_link_status_get(unit, port, is_link_up, is_latch_down));
    } else {
        MIIM_LOCK(unit);
        rv = soc_phyctrl_link_get(unit, port, &val);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
        (*is_link_up) = (val ? 1 : 0);

        SOCDNX_IF_ERR_EXIT(soc_common_link_status_get(unit, port, is_latch_down));
        if(*is_latch_down) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, *is_latch_down));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_pm_link_status_clear(int unit, soc_port_t port)
{
    soc_port_if_t interface_type;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_latch_down_set(unit, port, 0));
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    if (SOC_PORT_IF_ILKN == interface_type) {
        SOCDNX_IF_ERR_EXIT(soc_ilkn_link_status_clear(unit, port));
    }   
    else {
         SOCDNX_IF_ERR_EXIT(soc_common_link_status_clear(unit, port));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_pm_port_clp_reset(int unit, soc_port_t port, int full_reset)
{
    uint32 is_valid, is_initialized;
    uint32 phy_port, reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    int clp_id;
    soc_pbmp_t quads_oor;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_initialized_get(unit, port, &is_initialized));
    if(is_initialized) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_DISABLED, (_BSL_SOCDNX_MSG("Can't remove initialized port %d"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    clp_id = (1 == phy_port) ? 0 : 1;
    
    if (full_reset) {
        
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, (clp_id==0 ? CLP_0_RESETf : CLP_1_RESETf), 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, (clp_id==0 ? CLP_0_RESETf : CLP_1_RESETf), 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_get(unit, &quads_oor));
        if (clp_id == 0) {
            SOC_PBMP_PORT_REMOVE(quads_oor, 0);
            SOC_PBMP_PORT_REMOVE(quads_oor, 1);
            SOC_PBMP_PORT_REMOVE(quads_oor, 2);
        } else {
            SOC_PBMP_PORT_REMOVE(quads_oor, 4);
            SOC_PBMP_PORT_REMOVE(quads_oor, 5);
            SOC_PBMP_PORT_REMOVE(quads_oor, 6);
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_set(unit, quads_oor));
    }

    
    
    SOCDNX_IF_ERR_EXIT(READ_PORT_MLD_CTRL_REGr(unit, port, &reg32_val));
    soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_HWf, 1);
    soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_MDIOREGSf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_MLD_CTRL_REGr(unit, port, reg32_val));

    if (full_reset) {
        
        SOCDNX_IF_ERR_EXIT(WRITE_REG_2029700r(unit, port, 0x6));
        
        
        SOCDNX_IF_ERR_EXIT(READ_PORT_ECC_CONTROLr(unit, port, &reg32_val));
        
        soc_reg_field_set(unit, PORT_ECC_CONTROLr, &reg32_val, CDC_RXFIFO_MEM_ENf, 0x1);
        
        soc_reg_field_set(unit, PORT_ECC_CONTROLr, &reg32_val, CDC_TXFIFO_MEM_ENf, 0x1);
        
        soc_reg_field_set(unit, PORT_ECC_CONTROLr, &reg32_val, MIB_RSC_MEM_ENf, 0x1);
        
        soc_reg_field_set(unit, PORT_ECC_CONTROLr, &reg32_val, MIB_TSC_MEM_ENf, 0x1);
        
        soc_reg_field_set(unit, PORT_ECC_CONTROLr, &reg32_val, TXFIFO_MEM_ENf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_ECC_CONTROLr(unit, port, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
soc_pm_port_recover(int unit, soc_pbmp_t *okay_nif_ports)
{
    soc_pbmp_t pbmp, phy_ports;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp);

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

    
    SOC_PBMP_CLEAR(pbmp);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &pbmp));
    SOCDNX_IF_ERR_EXIT(soc_pm_ports_init(unit, pbmp, okay_nif_ports));
    PBMP_ITER(pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_pm_init(unit, phy_ports, SOC_DPP_FIRST_DIRECT_PORT(unit)));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t
soc_pm_frame_max_set(int unit, soc_port_t port, int size)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_FRAME_MAX_SET(md, unit, port, size));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_frame_max_get(int unit, soc_port_t port, int* size)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_FRAME_MAX_GET(md, unit, port, size));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_local_fault_get(int unit, soc_port_t port, int* local_fault)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_FAULT_LOCAL_STATUS, local_fault));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_remote_fault_get(int unit, soc_port_t port, int* remote_fault)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_STATUS, remote_fault));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_local_fault_clear(int unit, soc_port_t port)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_FAULT_LOCAL_STATUS, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_remote_fault_clear(int unit, soc_port_t port)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_STATUS, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_pad_size_set(int unit, soc_port_t port, int value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_PAD_SIZE, value));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_set(unit, port, value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_pad_size_get(int unit, soc_port_t port, int *value)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_runt_pad_get(unit, port, (uint32 *)(value)));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_pm_pfc_refresh_set(int unit, soc_port_t port, int value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_PFC_REFRESH_TIME, value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_pfc_refresh_get(int unit, soc_port_t port, int *value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_PFC_REFRESH_TIME, value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_PAUSE_ADDR_SET(md, unit, port, mac_sa));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_PAUSE_ADDR_GET(md, unit, port, mac_sa));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_PAUSE_SET(md, unit, port, pause_tx, pause_rx));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_PAUSE_GET(md, unit, port, pause_tx, pause_rx));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_pm_pfc_set(int unit, soc_port_t port, int is_rx , int pfc)
{
    CONST mac_driver_t* md;
    soc_mac_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    control = is_rx ? SOC_MAC_CONTROL_PFC_RX_ENABLE : SOC_MAC_CONTROL_PFC_TX_ENABLE;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, control, pfc));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_pfc_get(int unit, soc_port_t port, int is_rx , int *pfc)
{
    CONST mac_driver_t* md;
    soc_mac_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    control = is_rx ? SOC_MAC_CONTROL_PFC_RX_ENABLE : SOC_MAC_CONTROL_PFC_TX_ENABLE;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, control, pfc));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_llfc_set(int unit, soc_port_t port, int is_rx , int llfc)
{
    CONST mac_driver_t* md;
    soc_mac_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    control = is_rx ? SOC_MAC_CONTROL_LLFC_RX_ENABLE : SOC_MAC_CONTROL_LLFC_TX_ENABLE;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, control, llfc));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_pm_llfc_get(int unit, soc_port_t port, int is_rx , int *llfc)
{
    CONST mac_driver_t* md;
    soc_mac_control_t control;
    SOCDNX_INIT_FUNC_DEFS;

    control = is_rx ? SOC_MAC_CONTROL_LLFC_RX_ENABLE : SOC_MAC_CONTROL_LLFC_TX_ENABLE;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, control, llfc));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_enable_get(int unit, soc_port_t port, uint32* value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_EEE_ENABLE, (int *)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_enable_set(int unit, soc_port_t port, uint32 value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_EEE_ENABLE, (int)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_EEE_TX_IDLE_TIME, (int *)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_EEE_TX_IDLE_TIME, (int)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_EEE_TX_WAKE_TIME, (int *)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_EEE_TX_WAKE_TIME, (int)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_link_active_duration_get(int unit, soc_port_t port, uint32 *value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_EEE_LINK_ACTIVE_DURATION, (int *)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_eee_link_active_duration_set(int unit, soc_port_t port, uint32 value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_EEE_LINK_ACTIVE_DURATION, (int)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_fault_remote_enable_set(int unit, soc_port_t port, uint32 value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE, (int)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_fault_remote_enable_get(int unit, soc_port_t port, uint32 *value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE, (int *)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_fault_local_enable_set(int unit, soc_port_t port, uint32 value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_SET(md, unit, port, SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, (int)value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_pm_fault_local_enable_get(int unit, soc_port_t port, uint32 *value)
{
    CONST mac_driver_t* md;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_mac_probe(unit, port, &md));
    SOCDNX_IF_ERR_EXIT(MAC_CONTROL_GET(md, unit, port, SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE, (int *)value));

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


