/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT
#include <shared/bsl.h>
#ifdef PORTMOD_SUPPORT

#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <sal/core/sync.h>
#include <soc/phyreg.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif 
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_defs.h>
#endif 

#include <soc/phy/phymod_port_control.h>
#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>


#include <soc/phy/phymod_sim.h>

#define SOC_DCMN_PORT_QMX_FIRST_FSRD_CORE 2

typedef struct dcmn_port_user_access_s {
    int unit; 
    int fsrd_blk_id; 
    int fsrd_internal_quad;
    sal_mutex_t mutex; 
} dcmn_port_user_access_t;




STATIC
int cl45_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    dcmn_port_user_access_t *cl45_user_data;
    uint16 val16 = 0;
    int rv;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl45_user_data = user_acc;

    rv = soc_dcmn_miim_cl45_read(cl45_user_data->unit, core_addr, reg_addr,  &val16);
    (*val) = val16;

    return rv;
}


STATIC
int cl45_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    dcmn_port_user_access_t *cl45_user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl45_user_data = user_acc;

    return soc_dcmn_miim_cl45_write(cl45_user_data->unit, core_addr, reg_addr, val);
}



STATIC
int cl22_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    uint16 val16;
    int rv;
    dcmn_port_user_access_t *cl22_user_data;

    (*val) = 0;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl22_user_data = user_acc;

    rv = soc_dcmn_miim_cl22_read(cl22_user_data->unit, core_addr, reg_addr,&val16);
    (*val) = val16;

    return rv;

}

STATIC
int cl22_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    dcmn_port_user_access_t *cl22_user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    cl22_user_data = user_acc;

    return soc_dcmn_miim_cl22_write(cl22_user_data->unit, core_addr, reg_addr, val);
}



STATIC
int mdio_bus_mutex_take(void* user_acc){
    dcmn_port_user_access_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (dcmn_port_user_access_t *) user_acc;

    return sal_mutex_take(user_data->mutex, sal_mutex_FOREVER); 
}

STATIC
int mdio_bus_mutex_give(void* user_acc){
    dcmn_port_user_access_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = (dcmn_port_user_access_t *) user_acc;

    return sal_mutex_give(user_data->mutex); 
}

STATIC int soc_dcmn_port_bus_write_disabled(void* user_acc, uint32_t* val) {
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    dcmn_port_user_access_t *user_data;
    if(user_acc == NULL){
        return SOC_E_PARAM;
    }

    user_data = (dcmn_port_user_access_t *) user_acc;
    *val = 0;
    if (SOC_IS_RELOADING(user_data->unit)) {
        *val = 1;
    }

#else
    *val = 0;
#endif

    return SOC_E_NONE;
}

phymod_bus_t cl22_bus = {
    "dcmn_cl22_with_mutex",
    cl22_bus_read,
    cl22_bus_write,
    soc_dcmn_port_bus_write_disabled,
    mdio_bus_mutex_take,
    mdio_bus_mutex_give,
    NULL,
    NULL,
    0
};


phymod_bus_t cl45_bus = {
    "dcmn_cl45_with_mutex",
    cl45_bus_read,
    cl45_bus_write,
    soc_dcmn_port_bus_write_disabled,
    mdio_bus_mutex_take,
    mdio_bus_mutex_give,
    NULL,
    NULL,
    0
};


phymod_bus_t cl22_no_mutex_bus = {
    "dcmn_cl22",
    cl22_bus_read,
    cl22_bus_write,
    soc_dcmn_port_bus_write_disabled,
    NULL,
    NULL,
    NULL,
    NULL,
    0
};


phymod_bus_t cl45_no_mutex_bus = {
    "dcmn_cl45",
    cl45_bus_read,
    cl45_bus_write,
    soc_dcmn_port_bus_write_disabled,
    NULL,
    NULL,
    NULL,
    NULL,
    0
};



soc_error_t
soc_dcmn_external_phy_chain_info_get(int unit, int phy, int addresses_array_size, int *addresses, int *phys_in_chain, int *is_clause45)
{
    int clause;
    SOCDNX_INIT_FUNC_DEFS;

    clause = soc_property_port_get(unit, phy, spn_PORT_PHY_CLAUSE, 22);
    if((clause != 22) && (clause != 45)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR("invalid Clause value %d"), clause));
    }
    *phys_in_chain = soc_property_port_get_csv(unit, phy, spn_PORT_PHY_ADDR, addresses_array_size, addresses);

    *is_clause45 = (clause == 45);

exit:
    SOCDNX_FUNC_RETURN; 
}




STATIC soc_error_t
soc_dcmn_mdio_phy_access_get(int unit, dcmn_port_user_access_t *acc_data, int is_clause45, uint16 addr, phymod_access_t *access, int *is_sim)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(acc_data);
    SOCDNX_NULL_CHECK(access);
    phymod_access_t_init(access);

    PHYMOD_ACC_USER_ACC(access) = acc_data;
    PHYMOD_ACC_ADDR(access) = addr;
    if(!is_clause45){
        if(acc_data->mutex != NULL){
            PHYMOD_ACC_BUS(access) = &cl22_bus;
        }else{
            PHYMOD_ACC_BUS(access) = &cl22_no_mutex_bus;
        }
    } else{
        if(acc_data->mutex != NULL){
            PHYMOD_ACC_BUS(access) = &cl45_bus;    
        }else{
            PHYMOD_ACC_BUS(access) = &cl45_no_mutex_bus;
        }
         
        PHYMOD_ACC_F_CLAUSE45_SET(access);
    }

    PHYMOD_ACC_DEVAD(access) = 0 | PHYMOD_ACC_DEVAD_FORCE_MASK; 

    SOCDNX_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeFalcon, access, 0, is_sim));

exit:
    SOCDNX_FUNC_RETURN;  
}





soc_error_t
soc_dcmn_external_phy_core_access_get(int unit, uint32 addr, phymod_access_t *access)
{
    int cl = 0, is_sim = 0, is_cl45 = 0;
    dcmn_port_user_access_t *local_user_access = NULL;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(local_user_access, dcmn_port_user_access_t, 1, "dnx_fabric_ext_phy_specific_db");
    sal_memset(local_user_access, 0, sizeof(dcmn_port_user_access_t));
    local_user_access->unit = unit;
    
    cl = soc_property_suffix_num_get(unit, -1, spn_PORT_PHY_CLAUSE  , "fabric", 45);
    if(45 == cl){
        is_cl45 = TRUE;
    } 
    else if(22 == cl){
        is_cl45 = FALSE;
    } 
    else{
        SOCDNX_FREE(local_user_access);
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid clause %d"), cl));
    }

    soc_dcmn_mdio_phy_access_get(unit, local_user_access, is_cl45, addr, access, &is_sim);

exit:
    SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_to_phymod_ref_clk(int unit, int  ref_clk, phymod_ref_clk_t *phymod_ref_clk){
    SOCDNX_INIT_FUNC_DEFS;

    *phymod_ref_clk = phymodRefClkCount;
    switch(ref_clk){
        case soc_dcmn_init_serdes_ref_clock_disable: break;
        case soc_dcmn_init_serdes_ref_clock_125:
        case 125:
            *phymod_ref_clk = phymodRefClk125Mhz;
            break;
        case soc_dcmn_init_serdes_ref_clock_156_25:
        case 156:
            *phymod_ref_clk = phymodRefClk156Mhz;
            break;
        default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid ref clk %d"), ref_clk));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
soc_dcmn_to_portmod_lb(int unit, soc_port_t port, soc_dcmn_loopback_mode_t dmnn_lb_mode, portmod_loopback_mode_t *portmod_lb_mode){
    SOCDNX_INIT_FUNC_DEFS;

    *portmod_lb_mode = portmodLoopbackCount;
    switch(dmnn_lb_mode){
    case soc_dcmn_loopback_mode_none:
        *portmod_lb_mode = portmodLoopbackCount;
        break;
    case soc_dcmn_loopback_mode_mac_async_fifo:
        *portmod_lb_mode = portmodLoopbackMacAsyncFifo;
        break;
    case soc_dcmn_loopback_mode_mac_outer:
        *portmod_lb_mode = portmodLoopbackMacOuter;
        break;
    case soc_dcmn_loopback_mode_mac_pcs:
        *portmod_lb_mode = portmodLoopbackMacPCS;
        break;
    case soc_dcmn_loopback_mode_phy_gloop:
        *portmod_lb_mode = portmodLoopbackPhyGloopPMD;
        break;
    case soc_dcmn_loopback_mode_phy_rloop:
        *portmod_lb_mode = portmodLoopbackPhyRloopPMD;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lb type %d"), dmnn_lb_mode));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



#define UC_TABLE_ENTRY_SIZE (4)



int
soc_dcmn_fabric_broadcast_firmware_loader(int unit,  uint32_t length, const uint8_t* data)
{
    soc_reg_above_64_val_t wr_data;
    int i=0;
    int word_index = 0;
#ifdef BCM_DPP_SUPPORT
    int blk_ins;
#endif

    if (!SOC_IS_FLAIR(unit)) {
        if (!SOC_IS_QAX(unit)) {
            PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, 0x7)); 
        } else {
            PHYMOD_IF_ERR_RETURN(WRITE_CLPORT_WC_UCMEM_CTRLr(unit, REG_PORT_ANY, 0x1));
        }
        for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
            SOC_REG_ABOVE_64_CLEAR(wr_data);
            if(i + UC_TABLE_ENTRY_SIZE < length){
                sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
            }else{ 
                sal_memcpy((uint8 *)wr_data, data + i, length - i);
            }
            
            for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
                wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
            }  
            
            if (!SOC_IS_QAX(unit)) {
                PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_FSRD_WL_EXT_MEMm(unit, MEM_BLOCK_ALL, 0, wr_data)); 
            } else {
                PHYMOD_IF_ERR_RETURN(WRITE_CLPORT_WC_UCMEM_DATAm(unit, MEM_BLOCK_ALL, 0, wr_data));
            }
        }
        if (!SOC_IS_QAX(unit)) {
            PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, 0x0));
        } else {
            PHYMOD_IF_ERR_RETURN(WRITE_CLPORT_WC_UCMEM_CTRLr(unit, REG_PORT_ANY, 0x0));
        }
    }
#ifdef BCM_DPP_SUPPORT 
   else {
        for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
            PHYMOD_IF_ERR_RETURN(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, blk_ins, 0x7));

            for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
                SOC_REG_ABOVE_64_CLEAR(wr_data);
                if(i + UC_TABLE_ENTRY_SIZE < length){
                    sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
                }else{ 
                    sal_memcpy((uint8 *)wr_data, data + i, length - i);
                }
                
                for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
                    wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
                }  
                      
                PHYMOD_IF_ERR_RETURN(WRITE_FSRD_FSRD_WL_EXT_MEMm(unit, blk_ins, 0, wr_data)); 
            }

            PHYMOD_IF_ERR_RETURN(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, blk_ins, 0x0));
        }
    }
#endif

    return SOC_E_NONE;
}

STATIC int
soc_dcmn_fabric_firmware_loader_callback(const phymod_core_access_t* core,  uint32_t length, const uint8_t* data)
{
    dcmn_port_user_access_t *user_data;
    soc_reg_above_64_val_t wr_data;
    int i=0, unit, reg_access_blk_id;
    int word_index = 0;
    uint32 quad;

    user_data = core->access.user_acc;
    unit = user_data->unit;
    reg_access_blk_id = user_data->fsrd_blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;
    quad = (1 << user_data->fsrd_internal_quad);

    PHYMOD_IF_ERR_RETURN(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, reg_access_blk_id, quad)); 
    for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if(i + UC_TABLE_ENTRY_SIZE < length){
            sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
        }else{ 
            sal_memcpy((uint8 *)wr_data, data + i, length - i);
        }
        
        for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
            wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
        }  
        
        PHYMOD_IF_ERR_RETURN(WRITE_FSRD_FSRD_WL_EXT_MEMm(user_data->unit, user_data->fsrd_blk_id  , 0, wr_data));
    }
    PHYMOD_IF_ERR_RETURN(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(user_data->unit, reg_access_blk_id, 0x0));

    return PHYMOD_E_NONE;
}



soc_error_t
soc_dcmn_fabric_pms_add(int unit, int cores_num , int first_port, int use_mutex, int quads_in_fsrd, core_address_get_f address_get_func, void **alloced_buffer)
{
    int i = 0, is_sim;
    int fmac_block_id, fsrd_block_id;
    int core_port_index = 0;
    int phy = first_port, port = first_port;
    soc_error_t rv;
    uint16 addr = 0;
    portmod_pm_create_info_t pm_info;
    soc_dcmn_init_serdes_ref_clock_t ref_clk = phymodRefClkCount;
    dcmn_port_user_access_t *user_data = NULL;
    uint32 tx_lane_map, rx_polarity, tx_polarity;
    int cl, is_cl45, serdes_core, mac_core, j;
    portmod_pm_identifier_t dnx_fabric_o_nif_pm;
    SOCDNX_INIT_FUNC_DEFS;


    user_data = sal_alloc(sizeof(dcmn_port_user_access_t)*cores_num , "user_data");
    SOCDNX_NULL_CHECK(user_data);
    for(i = 0 ; i < cores_num ; i++){
        user_data[i].unit = unit;
        if(use_mutex){
            user_data[i].mutex = sal_mutex_create("core mutex");
        }else {
            user_data[i].mutex = NULL;
        }
    }

    
    cl = soc_property_suffix_num_get(unit, -1, spn_PORT_PHY_CLAUSE  , "fabric", 45);
    if (cl == 45)
    {
        is_cl45 = TRUE;
    } else if (cl == 22) {
        is_cl45 = FALSE;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid clause %d"), cl));
    }





    for(i = 0; i < cores_num ; i++){

        SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

        
        pm_info.pm_specific_info.dnx_fabric.lane_map.num_of_lanes = 4;
        for( j = 0 ; j < 4 ; j++){
            pm_info.pm_specific_info.dnx_fabric.lane_map.lane_map_rx[j] = j;
        }

        pm_info.pm_specific_info.dnx_fabric.core_index = i;
        #ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
            if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
                pm_info.type = portmodDispatchTypeDnx_fabric_o_nif;
                pm_info.pm_specific_info.dnx_fabric.first_phy_offset = 1;
                pm_info.pm_specific_info.dnx_fabric.is_over_nif = 1;
            } else
        #endif 
            {
                pm_info.type = portmodDispatchTypeDnx_fabric;
                pm_info.pm_specific_info.dnx_fabric.first_phy_offset = 0;
                pm_info.pm_specific_info.dnx_fabric.is_over_nif = 0;
            }
        pm_info.pm_specific_info.dnx_fabric.fw_load_method = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", phymodFirmwareLoadMethodExternal);
        pm_info.pm_specific_info.dnx_fabric.fw_load_method &= 0xff;
        if( pm_info.pm_specific_info.dnx_fabric.fw_load_method == phymodFirmwareLoadMethodExternal){
            pm_info.pm_specific_info.dnx_fabric.external_fw_loader =  soc_dcmn_fabric_firmware_loader_callback;
        }


        port = first_port + i*4;

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy = SOC_INFO(unit).port_l2p_mapping[port];
        } else{
            phy = port;
        }

        if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
            portmod_pm_identifier_t_init(unit, &dnx_fabric_o_nif_pm);
#ifdef PORTMOD_PM4X25_SUPPORT
            dnx_fabric_o_nif_pm.type = portmodDispatchTypePm4x25;
#endif
            dnx_fabric_o_nif_pm.phy = phy;
            pm_info.pm_specific_info.dnx_fabric.fabric_o_nif_pm = dnx_fabric_o_nif_pm;
        }
        
        serdes_core = i;
        mac_core = i;
        if (SOC_IS_QMX(unit))
        {
            serdes_core += SOC_DCMN_PORT_QMX_FIRST_FSRD_CORE;
        }
            
        
        fmac_block_id = FMAC_BLOCK(unit, mac_core);
        fsrd_block_id = FSRD_BLOCK(unit, (serdes_core/quads_in_fsrd));
        user_data[i].fsrd_blk_id = fsrd_block_id; 
        user_data[i].fsrd_internal_quad = (serdes_core % quads_in_fsrd);

        pm_info.pm_specific_info.dnx_fabric.fmac_schan_id = SOC_BLOCK_INFO(unit, fmac_block_id).schan;
        pm_info.pm_specific_info.dnx_fabric.fsrd_schan_id = SOC_BLOCK_INFO(unit, fsrd_block_id).schan;
        pm_info.pm_specific_info.dnx_fabric.fsrd_internal_quad = user_data[i].fsrd_internal_quad;
        SOCDNX_IF_ERR_EXIT(address_get_func(unit, serdes_core, &addr));
        rv = soc_dcmn_mdio_phy_access_get(unit, &user_data[i], is_cl45, addr, &pm_info.pm_specific_info.dnx_fabric.access, &is_sim);
        SOCDNX_IF_ERR_EXIT(rv);

        if(is_sim) {
            pm_info.pm_specific_info.dnx_fabric.fw_load_method = phymodFirmwareLoadMethodNone;
        }

        ref_clk = SOC_INFO(unit).port_refclk_int[port];
        rv = soc_to_phymod_ref_clk(unit, ref_clk, &pm_info.pm_specific_info.dnx_fabric.ref_clk);
        SOCDNX_IF_ERR_EXIT(rv);
        if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
            pm_info.pm_specific_info.dnx_fabric.com_clk = phymodRefClk156Mhz;
        } else {
            pm_info.pm_specific_info.dnx_fabric.com_clk = pm_info.pm_specific_info.dnx_fabric.ref_clk;
        }

        
        if (!SOC_WARM_BOOT(unit))
        {
            if (SOC_IS_FE3200(unit) && soc_property_suffix_num_only_suffix_str_get(unit, i, spn_PHY_TX_LANE_MAP, "quad"))
            {
                tx_lane_map = soc_property_suffix_num_get(unit, i, spn_PHY_TX_LANE_MAP, "quad", SOC_DCMN_PORT_NO_LANE_SWAP); 
            }
            else
            {
                tx_lane_map = soc_property_suffix_num_get(unit, i, spn_PHY_TX_LANE_MAP, "fabric_quad", SOC_DCMN_PORT_NO_LANE_SWAP); 
            }
            for(core_port_index = 0 ;  core_port_index < 4 ; core_port_index++) {
                pm_info.pm_specific_info.dnx_fabric.lane_map.lane_map_tx[core_port_index] = ((tx_lane_map >> (core_port_index * 4)) & 0xf);
            }    
        } else {
            pm_info.pm_specific_info.dnx_fabric.lane_map.lane_map_tx[core_port_index] = 0xFFFFFFFF; 
        }
        
        
        PORTMOD_PBMP_CLEAR(pm_info.phys);
        for(core_port_index = 0 ;  core_port_index < 4 ; core_port_index++){
            if (!SOC_WARM_BOOT(unit))
            {
                rx_polarity = soc_property_port_get(unit, port, spn_PHY_RX_POLARITY_FLIP, 0);
                tx_polarity = soc_property_port_get(unit, port, spn_PHY_TX_POLARITY_FLIP, 0);
                pm_info.pm_specific_info.dnx_fabric.polarity.rx_polarity |= ((rx_polarity & 0x1) << core_port_index);
                pm_info.pm_specific_info.dnx_fabric.polarity.tx_polarity |= ((tx_polarity & 0x1) << core_port_index);

            } 

            PORTMOD_PBMP_PORT_ADD(pm_info.phys, phy);
            phy++;
            port++;
        }
        SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));
    }
    *alloced_buffer = user_data;

exit:
    if(_rv != SOC_E_NONE){
        if(user_data != NULL){
            for(i = 0 ; i < cores_num ; i++){
                if(user_data[i].mutex != NULL){
                    sal_mutex_destroy(user_data[i].mutex);
                }
            }
            sal_free(user_data);
        }
    }
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_fabric_pcs_parse(int unit, char *pcs_str, int pcs_def, int *pcs)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (pcs_str == NULL)
    {
        *pcs = pcs_def;
    } else if (!sal_strcmp(pcs_str, "KR_FEC") || !sal_strcmp(pcs_str, "2")) {
        *pcs = PORTMOD_PCS_64B66B_FEC;
    } else if (!sal_strcmp(pcs_str, "64_66") || !sal_strcmp(pcs_str, "4")) {
        *pcs = PORTMOD_PCS_64B66B;
    } else if (!sal_strcmp(pcs_str, "RS_FEC")) {
        *pcs = PORTMOD_PCS_64B66B_RS_FEC;
    } else if (!sal_strcmp(pcs_str, "LL_RS_FEC")) {
        *pcs = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG_STR("invalid pcs %s"), pcs_str));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t
soc_dcmn_fabric_port_probe(int unit, int port, dcmn_port_init_stage_t init_stage, int fw_verify, dcmn_port_fabric_init_config_t* port_config)
{
    portmod_port_add_info_t info;
    uint32 encoding_properties = 0;
    int phy = 0;
    uint32 value, value_def;
    uint32 pre_curr, main_curr, post_curr;
    uint32 pre_curr_def, main_curr_def, post_curr_def;
    uint32 is_fiber_perf;
    int enable;
    
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {

        SOCDNX_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &info));

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy = SOC_INFO(unit).port_l2p_mapping[port];
        } else{
            phy = port;
        }
        PORTMOD_PBMP_PORT_ADD(info.phys, phy);
        info.interface_config.interface = SOC_PORT_IF_SFI;
        info.interface_config.speed = port_config->speed;
        info.interface_config.interface_modes = 0;
        info.interface_config.flags = 0;
        info.link_training_en = port_config->cl72;
        if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit))
        {
            info.is_fabric_o_nif = 1;
            if (info.link_training_en) {
                info.init_config.fs_cl72 = 1; 
            }
        }

        if(init_stage == dcmn_port_init_until_fw_load) {
            PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_SET(&info);
            PORTMOD_PORT_ADD_F_INIT_PASS1_SET(&info);
        }

        if(init_stage == dcmn_port_init_resume_after_fw_load) {
            PORTMOD_PORT_ADD_F_INIT_PASS2_SET(&info);
        }

        if(fw_verify) {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(&info);
        } else {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&info);
        }

        SOCDNX_IF_ERR_EXIT(portmod_port_add(unit, port, &info));

        if(init_stage == dcmn_port_init_until_fw_load) {
            SOC_EXIT;
        }

        if (port_config->pcs == PORTMOD_PCS_64B66B_FEC)
        {
            PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(encoding_properties);
        } else if (port_config->pcs == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC || port_config->pcs == PORTMOD_PCS_64B66B_RS_FEC)
        {
            PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(encoding_properties);
        }

        SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, encoding_properties, port_config->pcs));

        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, -1, -1, 0, SOC_PHY_CONTROL_DRIVER_CURRENT, &value_def));
        value = soc_property_port_get(unit, port, spn_SERDES_DRIVER_CURRENT, value_def);
        if (value != value_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, -1, -1, 0, SOC_PHY_CONTROL_DRIVER_CURRENT, value));
        }

        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, -1, -1, 0, SOC_PHY_CONTROL_TX_FIR_PRE, &pre_curr_def));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, -1, -1, 0, SOC_PHY_CONTROL_TX_FIR_MAIN, &main_curr_def));
        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, -1, -1, 0, SOC_PHY_CONTROL_TX_FIR_POST, &post_curr_def));
        value_def = pre_curr_def << SOC_DCMN_PORT_SERDES_PRE_CURRENT_FIRST;
        value_def = value_def | (main_curr_def << SOC_DCMN_PORT_SERDES_MAIN_CURRENT_FIRST);
        value_def = value_def | (post_curr_def << SOC_DCMN_PORT_SERDES_POST_CURRENT_FIRST);

        value = soc_property_port_get(unit, port, spn_SERDES_PREEMPHASIS, value_def);

        pre_curr = (value & SOC_DCMN_PORT_SERDES_PRE_CURRENT_MASK) >> SOC_DCMN_PORT_SERDES_PRE_CURRENT_FIRST;
        if (pre_curr != pre_curr_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, -1, -1, 0, SOC_PHY_CONTROL_TX_FIR_PRE, pre_curr));
        }

        main_curr = (value & SOC_DCMN_PORT_SERDES_MAIN_CURRENT_MASK) >> SOC_DCMN_PORT_SERDES_MAIN_CURRENT_FIRST;
        if (main_curr != main_curr_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, -1, -1, 0, SOC_PHY_CONTROL_TX_FIR_MAIN, main_curr));
        }

        post_curr = (value & SOC_DCMN_PORT_SERDES_POST_CURRENT_MASK) >> SOC_DCMN_PORT_SERDES_POST_CURRENT_FIRST;
        if (post_curr != post_curr_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, -1, -1, 0, SOC_PHY_CONTROL_TX_FIR_POST, post_curr));
        }

        is_fiber_perf = soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF, 0);
        if(is_fiber_perf) {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, -1, -1, 0, SOC_PHY_CONTROL_MEDIUM_TYPE, SOC_PORT_MEDIUM_FIBER));
        }

        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_get(unit, port, -1, -1, 0, SOC_PHY_CONTROL_FIRMWARE_MODE, &value_def));
        value = soc_property_port_get(unit, port, spn_SERDES_FIRMWARE_MODE, value_def);
        if (value != value_def)
        {
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_enable_get(unit, port , &enable));
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_enable_set(unit, port, 0)); 
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_control_set(unit, port, -1, -1, 0, SOC_PHY_CONTROL_FIRMWARE_MODE, value));
            SOCDNX_IF_ERR_EXIT(soc_dcmn_port_enable_set(unit, port, enable)); 
            
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_dcmn_port_cl72_set(int unit, soc_port_t port, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(portmod_port_cl72_set(unit, port, (enable ? 1 : 0))); 

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_cl72_get(int unit, soc_port_t port, int *enable)
{
    uint32 local_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_cl72_get(unit, port, &local_enable)); 
    *enable = (local_enable ? 1 : 0);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_phy_control_set(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value)
{
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int is_lane_control, rv;
    phymod_ref_clk_t ref_clk;
    int is_legacy_ext_phy = 0;
    uint32_t port_dynamic_state = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    switch (type)
    {
        
        case SOC_PHY_CONTROL_LANE_SWAP:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }

    if(IS_IL_PORT(unit, port) || IS_SFI_PORT(unit, port)) { 
        is_legacy_ext_phy = 0;
    } else if( params.phyn != 0) { 
        SOCDNX_IF_ERR_EXIT(portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy) { 
        if(type != SOC_PHY_CONTROL_AUTONEG_MODE){
            SOCDNX_IF_ERR_EXIT(soc_port_control_set_wrapper(unit, ref_clk, is_lane_control, phys, phys_returned, type, value));
        }
        else{
            port_dynamic_state |= 0x2;
            port_dynamic_state |= value << 16;
            portmod_port_update_dynamic_state(unit, port, port_dynamic_state);
        }
    } else {
        SOCDNX_IF_ERR_EXIT(portmod_port_ext_phy_control_set(unit, port, phyn, lane, is_sys_side, type, value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_dcmn_port_phy_control_get(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value)
{
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params; 
    int is_lane_control, rv;
    phymod_ref_clk_t ref_clk;
    int is_legacy_ext_phy = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    switch (type)
    {
        
        case SOC_PHY_CONTROL_LANE_SWAP:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }

    if(IS_IL_PORT(unit, port) || IS_SFI_PORT(unit, port)) { 
        is_legacy_ext_phy = 0;
    } else if( params.phyn != 0) { 
        SOCDNX_IF_ERR_EXIT(portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy) {
        SOCDNX_IF_ERR_EXIT(soc_port_control_get_wrapper(unit, ref_clk, is_lane_control, phys, phys_returned, type, value));
    } else {
        SOCDNX_IF_ERR_EXIT(portmod_port_ext_phy_control_get(unit, port, phyn, lane, is_sys_side, type, value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_dcmn_port_config_get(int unit, soc_port_t port, dcmn_port_fabric_init_config_t* port_config)
{
    int speed;
    char *pcs_str;
    SOCDNX_INIT_FUNC_DEFS;

    
    pcs_str = soc_property_port_get_str(unit, port, spn_BACKPLANE_SERDES_ENCODING);
    SOCDNX_IF_ERR_EXIT(soc_dcmn_fabric_pcs_parse(unit, pcs_str, port_config->pcs, &(port_config->pcs)));
    speed = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, port_config->speed);
    if((speed != -1) && (speed != 0))
    {
        port_config->speed = speed;
    }
    port_config->cl72 = soc_property_port_get(unit, port, spn_PORT_INIT_CL72, port_config->cl72) ? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_phy_reg_get(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32  phy_id;
    uint32 phy_reg;
    uint16 phy_rd_data;
    uint32 reg_flag;
    int    rv;
    int nof_phys_structs = 0;
    int clause;
    phymod_phy_access_t phy_access[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if(flags & SOC_PHY_INTERNAL){
        params.phyn = 0;
    }
    clause = (flags & SOC_PHY_CLAUSE45 ? 45 : 22);
    if ((flags & SOC_PHY_NOMAP) == 0){
        rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access, &nof_phys_structs, NULL);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported flags for fe3200")));
    } else {
        reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
        if (reg_flag & SOC_PHY_REG_INDIRECT) {
            if (flags & BCM_PORT_PHY_NOMAP) {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("indirect register access is not supported if SOC_PHY_NOMAP flag is set")));
            }
            phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
            rv = phymod_phy_reg_read(phy_access, phy_reg_addr, phy_data);
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            if (flags & SOC_PHY_NOMAP) {
                phy_id = port;
                phy_reg = phy_reg_addr;
                rv = soc_dcmn_miim_read(unit, clause, phy_id, phy_reg, &phy_rd_data);
                SOCDNX_IF_ERR_EXIT(rv);
                *phy_data = phy_rd_data;
            } else{
                PHYMOD_LOCK_TAKE(phy_access);
                rv = PHYMOD_BUS_READ(&phy_access[0].access, phy_reg_addr, phy_data);
                PHYMOD_LOCK_GIVE(phy_access);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_phy_reg_set(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    uint16 phy_wr_data;
    uint32  phy_id;
    uint32 phy_reg;
    uint32 reg_flag;
    int    rv;
    int nof_phys_structs = 0;
    int clause;
    phymod_phy_access_t phy_access[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if(flags & SOC_PHY_INTERNAL){
        params.phyn = 0;
    }
    clause = (flags & SOC_PHY_CLAUSE45 ? 45 : 22);
    if ((flags & SOC_PHY_NOMAP) == 0){
        rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access, &nof_phys_structs, NULL);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported flags for fe3200")));
    } else {
        reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
        if (reg_flag & SOC_PHY_REG_INDIRECT) {
            if (flags & BCM_PORT_PHY_NOMAP) {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("indirect register access is not supported if SOC_PHY_NOMAP flag is set")));
            }
            phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
            rv = phymod_phy_reg_write(phy_access, phy_reg_addr, phy_data);
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            if (flags & BCM_PORT_PHY_NOMAP) {
                phy_id = port;
                phy_reg = phy_reg_addr;
                phy_wr_data = phy_data & 0xffff;
                rv = soc_dcmn_miim_write(unit, clause, phy_id, phy_reg, phy_wr_data);
                SOCDNX_IF_ERR_EXIT(rv);
            } else{
                PHYMOD_LOCK_TAKE(phy_access);
                rv = PHYMOD_BUS_WRITE(&phy_access[0].access, phy_reg_addr, phy_data);
                PHYMOD_LOCK_GIVE(phy_access);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_phy_reg_modify(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask)
{
    uint32 phy_rd_data;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_reg_get(unit, port, flags, phy_reg_addr, &phy_rd_data));
    phy_data |= (phy_rd_data & ~phy_mask);
    SOCDNX_IF_ERR_EXIT(soc_dcmn_port_phy_reg_set(unit, port, flags, phy_reg_addr, phy_data));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_dcmn_port_control_pcs_set(int unit, soc_port_t port, soc_dcmn_port_pcs_t pcs)
{
    uint32 properties = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (pcs == soc_dcmn_port_pcs_64_66_fec)
    {
        PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(properties); 
    } else if (pcs == soc_dcmn_port_pcs_64_66_rs_fec|| pcs == soc_dcmn_port_pcs_64_66_ll_rs_fec)
    {
        PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(properties);
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, (_shr_port_pcs_t) pcs));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_control_pcs_get(int unit, soc_port_t port, soc_dcmn_port_pcs_t* pcs)
{
    uint32 properties = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, (portmod_port_pcs_t *) pcs));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_control_low_latency_llfc_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));

    if ((encoding != PORTMOD_PCS_64B66B_FEC)
        && (encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_RS_FEC))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Low latency LLFC contorl is supported only by KR_FEC, RS_FEC or LL_RS_FEC")));
    }
    if(value){
        properties |= PORTMOD_ENCODING_LOW_LATENCY_LLFC;
    } else{
        properties &= ~PORTMOD_ENCODING_LOW_LATENCY_LLFC;
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_control_low_latency_llfc_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if ((encoding != PORTMOD_PCS_64B66B_FEC)
        && (encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_RS_FEC))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Low latency LLFC contorl is supported only by KR_FEC, RS_FEC or LL_RS_FEC")));
    }
    *value = PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_control_fec_error_detect_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC && encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_RS_FEC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Error detect contorl is supported only by KR_FEC, RS_FEC or LL_RS_FEC")));
    }

    if(value){
        properties |= PORTMOD_ENCODING_FEC_ERROR_DETECT;
    } else{
        properties &= ~PORTMOD_ENCODING_FEC_ERROR_DETECT;
    }

    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_control_fec_error_detect_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC && encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_RS_FEC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Error detect is supported only by KR_FEC, RS_FEC or LL_RS_FEC")));
    }

    *value = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_control_power_set(int unit, soc_port_t port, uint32 flags, soc_dcmn_port_power_t power)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, soc_dcmn_port_power_on == power ? 1 : 0));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_control_power_get(int unit, soc_port_t port, soc_dcmn_port_power_t* power)
{
    int enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, &enable));
    *power =  enable ? soc_dcmn_port_power_on : soc_dcmn_port_power_off;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable)
{
    
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_RX;
    SOCDNX_INIT_FUNC_DEFS;

    if(flags & SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE){
        portmod_enable_flags |= PORTMOD_PORT_ENABLE_MAC ;
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, portmod_enable_flags, enable));

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
soc_dcmn_port_control_tx_enable_set(int unit, soc_port_t port, int enable)
{
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_TX;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, portmod_enable_flags, enable));
exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
soc_dcmn_port_control_rx_enable_get(int unit, soc_port_t port, int* enable)
{
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_RX | PORTMOD_PORT_ENABLE_MAC;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, portmod_enable_flags, enable));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_control_tx_enable_get(int unit, soc_port_t port, int* enable)
{
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_TX | PORTMOD_PORT_ENABLE_MAC;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, portmod_enable_flags, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_prbs_tx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, portmod_mode, flags, value));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dcmn_port_prbs_rx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, portmod_mode, flags, value));

exit:
    SOCDNX_FUNC_RETURN;
    
}

soc_error_t 
soc_dcmn_port_prbs_tx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int *value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, portmod_mode, flags, value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_prbs_rx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int *value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, portmod_mode, flags, value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_prbs_rx_status_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value)
{
    phymod_prbs_status_t status;
    int flags = PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    SOCDNX_IF_ERR_EXIT(phymod_prbs_status_t_init(&status));
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_status_get(unit, port, portmod_mode, flags, &status));
    if(status.prbs_lock_loss) {
        *value = -2;
    } else if(!status.prbs_lock) {
        *value = -1;
    } else {
        *value = status.error_count;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_prbs_tx_invert_data_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int *invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    *invert = config.invert ? 1: 0;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dcmn_port_prbs_tx_invert_data_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    config.invert = invert ? 1: 0;
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_dcmn_port_prbs_polynomial_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int flags = 0;
    phymod_prbs_poly_t poly;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    SOCDNX_IF_ERR_EXIT(soc_prbs_poly_to_phymod(value, &poly));
    config.poly = poly;
    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_prbs_polynomial_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int *value)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SOCDNX_INIT_FUNC_DEFS;

    portmod_mode = (mode == soc_dcmn_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    SOCDNX_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    SOCDNX_IF_ERR_EXIT(phymod_prbs_poly_to_soc(config.poly, (uint32*) value));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback)
{
    soc_dcmn_loopback_mode_t current_lb = soc_dcmn_loopback_mode_none;
    portmod_loopback_mode_t portmod_lb_mode = portmodLoopbackCount;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dcmn_port_loopback_get(unit, port, &current_lb));
    if(current_lb == loopback){
        
        SOC_EXIT;
    }
    if(current_lb != soc_dcmn_loopback_mode_none){
        
        SOCDNX_IF_ERR_EXIT(soc_dcmn_to_portmod_lb(unit, port, current_lb, &portmod_lb_mode));
        rv = portmod_port_loopback_set(unit, port, portmod_lb_mode, 0);
        if ((rv != SOC_E_NONE) && (portmod_lb_mode == portmodLoopbackPhyGloopPMD)) {
            rv = portmod_port_loopback_set(unit, port, portmodLoopbackPhyGloopPCS, 0);
        }
        SOCDNX_IF_ERR_EXIT(rv);
    }
    if(loopback != soc_dcmn_loopback_mode_none){
        
        SOCDNX_IF_ERR_EXIT(soc_dcmn_to_portmod_lb(unit, port, loopback, &portmod_lb_mode));
        rv = portmod_port_loopback_set(unit, port, portmod_lb_mode, 1);
        if ((rv != SOC_E_NONE) && (portmod_lb_mode == portmodLoopbackPhyGloopPMD)) {
            rv = portmod_port_loopback_set(unit, port, portmodLoopbackPhyGloopPCS, 1);
        }
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback)
{
    portmod_loopback_mode_t supported_lb_modes[] = { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS,
        portmodLoopbackPhyGloopPMD,portmodLoopbackPhyGloopPCS,portmodLoopbackPhyRloopPMD};
    soc_dcmn_loopback_mode_t mapped_moeds[] = {soc_dcmn_loopback_mode_mac_async_fifo, soc_dcmn_loopback_mode_mac_outer, soc_dcmn_loopback_mode_mac_pcs,
        soc_dcmn_loopback_mode_phy_gloop, soc_dcmn_loopback_mode_phy_gloop, soc_dcmn_loopback_mode_phy_rloop};
    int i = 0, rv;
    int enable = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *loopback = soc_dcmn_loopback_mode_none;
    for(i = 0 ; i < COUNTOF(supported_lb_modes); i++){

        if(IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port)) {
            if(supported_lb_modes[i] == portmodLoopbackPhyGloopPCS) {
                continue;
            }
        }
        rv = portmod_port_loopback_get(unit, port, supported_lb_modes[i], &enable);
        if(rv != SOC_E_NONE) {
            
            continue; 
        }
        SOCDNX_IF_ERR_EXIT(rv);
        if(enable){
            *loopback = mapped_moeds[i];
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_enable_set(int unit, soc_port_t port, int enable)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable? 1 : 0));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_enable_get(int unit, soc_port_t port, int *enable)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_dcmn_port_rx_locked_get(int unit, soc_port_t port, uint32 *rx_locked)
{
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;
    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SOCDNX_IF_ERR_EXIT(phymod_phy_rx_pmd_locked_get(phys, rx_locked));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dcmn_port_extract_cig_from_llfc_enable_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Extract Congrstion Ind from LLFC cells contorl is supported only by KR_FEC")));
    }
    if(value){
        properties |= PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    } else{
        properties &= ~PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_dcmn_port_extract_cig_from_llfc_enable_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Extract Congrstion Ind from LLFC cells contorl is supported only by KR_FEC")));
    }
    *value = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties);
exit:
    SOCDNX_FUNC_RETURN;
}


#endif 

#undef _ERR_MSG_MODULE_NAME

