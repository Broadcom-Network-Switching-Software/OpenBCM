/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC ARAD SERDES
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/error.h>
#include <soc/phy/phyctrl.h>
#include <soc/dpp/ARAD/arad_serdes.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>

#include <soc/dpp/port_sw_db.h>


soc_error_t 
soc_arad_serdes_polarity_set(int unit, soc_port_t phy_port, int tx_polarity, int rx_polarity)
{
    int internal_lane, array_size;
    uint16 mdio_addr, phy_rd_data;
    uint32 block_addr_tx, block_addr_rx;
    int clause = 22; 
    phyident_core_info_t core_info;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    
    if(SAL_BOOT_PLISIM) {
        SOC_EXIT;
    }

    internal_lane = (phy_port-1) % 4;   

    phyident_core_info_t_init(&core_info);
    SOCDNX_IF_ERR_EXIT(_dpp_phy_addr_multi_get(unit, phy_port, 0, 1, &array_size, &core_info));
    mdio_addr = core_info.mdio_addr;
    
    switch(internal_lane) {
    case 0:
        block_addr_tx = 0x8060;
        block_addr_rx = 0x80b0;
        break; 
    case 1:
        block_addr_tx = 0x8070;
        block_addr_rx = 0x80c0;
        break;
    case 2:
        block_addr_tx = 0x8080;
        block_addr_rx = 0x80d0;
        break;
    case 3:
    default:
        block_addr_tx = 0x8090;
        block_addr_rx = 0x80e0;
        break;
    }

    
    if(SOC_ARAD_SERDES_POLARITY_NO_CHANGE != tx_polarity) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, 0x0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, block_addr_tx));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, mdio_addr, 0x11, &phy_rd_data));
        if(tx_polarity) {
            phy_rd_data |= (1<<5);
        } else {
            phy_rd_data &= ~(1<<5);
        }
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x11, phy_rd_data));                
    }
    
    
    if(SOC_ARAD_SERDES_POLARITY_NO_CHANGE != rx_polarity) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, 0x0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, block_addr_rx));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, mdio_addr, 0x1a, &phy_rd_data));
        phy_rd_data &= ~(0xC);
        if(rx_polarity) {
            phy_rd_data |= 0xC;
        } else {
            phy_rd_data |= 0x8;
        }
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1a, phy_rd_data));    
    }


exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_arad_serdes_lane_map_set(int unit, int wc_idx, int tx_map, int rx_map)
{
    int array_size, n;
    uint16 mdio_addr, phy_rd_data, phy_rx_map, phy_tx_map;
    soc_port_t phy_port;
    uint32 mask, shift;
    int clause = 22; 
    phyident_core_info_t core_info;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    
    if(SAL_BOOT_PLISIM) {
        SOC_EXIT;
    }

    phy_port = (wc_idx * 4)+1;

    phyident_core_info_t_init(&core_info);
    SOCDNX_IF_ERR_EXIT(_dpp_phy_addr_multi_get(unit, phy_port, 0, 1, &array_size, &core_info));
    mdio_addr = core_info.mdio_addr;
    
    phy_rx_map = 0;
    phy_tx_map = 0;
    
    for(n = 0, shift = 0; n < 4; ++n, shift += 2) {
        mask = 0x3 << shift;
        phy_rx_map |= (rx_map >> shift) & mask;
        phy_tx_map |= (tx_map >> shift) & mask;
    }

    
    if(SOC_ARAD_SERDES_LANE_SWAP_NO_CHANGE != tx_map) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, 0x0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0x8160));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, mdio_addr, 0x19, &phy_rd_data));
        phy_rd_data &= 0xFFFFFF00;
        phy_rd_data |= phy_tx_map; 
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x19, phy_rd_data)); 
    }

    
    if(SOC_ARAD_SERDES_LANE_SWAP_NO_CHANGE != rx_map) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, 0x0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0x8160));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, mdio_addr, 0x1b, &phy_rd_data));
        phy_rd_data &= 0xFFFFFF00;         
        phy_rd_data |= phy_rx_map;          
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1b, phy_rd_data)); 
    }


exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_arad_serdes_rx_seq_stop(int unit, soc_port_t port)
{
    soc_port_t phy_port;
    uint32 first_phy_port;
    int array_size;
    uint32 internal_lane;
    uint16 reg_val, mdio_addr, orig_reg_val;
    int clause = 22; 
    soc_pbmp_t phy_ports;
    soc_port_if_t interface_type;
    phyident_core_info_t core_info;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_ports);

    MIIM_LOCK(unit);

    if (IS_SFI_PORT(unit, port)) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        SOC_PBMP_PORT_ADD(phy_ports, phy_port);
    } else {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

        
        if (interface_type == SOC_PORT_IF_CAUI) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port ));
            SOC_PBMP_PORT_ADD(phy_ports, first_phy_port - 1 + 11 );
            SOC_PBMP_PORT_ADD(phy_ports, first_phy_port - 1 + 12 );
        }
    }

    PBMP_ITER(phy_ports, phy_port) {
        
        internal_lane = (IS_SFI_PORT(unit, port) ? SOC_DPP_FABRIC_PORT_TO_LINK(unit, port) : phy_port - 1) % SOC_ARAD_NOF_LINKS_IN_QUAD;

        phyident_core_info_t_init(&core_info);
        SOCDNX_IF_ERR_EXIT(_dpp_phy_addr_multi_get(unit, phy_port, 0, 1, &array_size, &core_info));
        mdio_addr = core_info.mdio_addr;

        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, internal_lane));

        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0x8210));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, mdio_addr, 0x1e, &orig_reg_val));

         reg_val = orig_reg_val | 0x8000;

        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0x8210));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, reg_val));

    }

exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_arad_serdes_rx_seq_start(int unit, soc_port_t port)
{
    soc_port_t phy_port;
    uint32 first_phy_port;
    int array_size;
    uint32 internal_lane;
    uint16 reg_val, mdio_addr, orig_reg_val;
    int clause = 22; 
    soc_pbmp_t phy_ports;
    soc_port_if_t interface_type;
    phyident_core_info_t core_info;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_ports);

    MIIM_LOCK(unit);

    if (IS_SFI_PORT(unit, port)) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        SOC_PBMP_PORT_ADD(phy_ports, phy_port);
    } else {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

        
        if (interface_type == SOC_PORT_IF_CAUI) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port ));
            SOC_PBMP_PORT_ADD(phy_ports, first_phy_port - 1 + 11 );
            SOC_PBMP_PORT_ADD(phy_ports, first_phy_port - 1 + 12 );
        }
    }

    PBMP_ITER(phy_ports, phy_port) {
        
        internal_lane = (IS_SFI_PORT(unit, port) ? SOC_DPP_FABRIC_PORT_TO_LINK(unit, port) : phy_port - 1) % SOC_ARAD_NOF_LINKS_IN_QUAD;

        phyident_core_info_t_init(&core_info);
        SOCDNX_IF_ERR_EXIT(_dpp_phy_addr_multi_get(unit, phy_port, 0, 1, &array_size, &core_info));
        mdio_addr = core_info.mdio_addr;

        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, internal_lane));

        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0x8210));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, mdio_addr, 0x1e, &orig_reg_val));

         reg_val = orig_reg_val & ~(0x8000);
         
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0x8210));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, reg_val));

        

        

        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1f, 0xffd0));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, mdio_addr, 0x1e, 0));
    }


exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME

