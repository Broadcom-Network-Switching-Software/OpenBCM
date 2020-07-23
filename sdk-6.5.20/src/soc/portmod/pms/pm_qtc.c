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
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_system.h>

#include <soc/portmod/unimac.h> 
#include <soc/portmod/pm_qtc.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM_QTC_SUPPORT

/* 10G mode at 10.3125G and 1G mode at 1.25G */
#define PM_QTC_LANES_PER_CORE (16)
#define MAX_PORTS_PER_PM_QTC (16)

#define PM_QTC_INFO(pm_info) ((pm_info)->pm_data.pm_qtc_db)

#define PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc) \
    do { \
        uint32 is_bypass; \
        phy_acc = 0; \
        SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass); \
        phy_acc = (is_bypass && PM_QTC_INFO(pm_info)->first_phy != -1) ? (PM_QTC_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK) : port; \
    } while (0)

#define PM_QTC_PORT_NUM_TO_PHY_ACCESS(_ac,_nphy,_p) \
     do	{ \
	   int i; \
           for (i = 0; i < (_nphy); i++) { \
               PORTMOD_USER_ACC_LPORT_SET(&((_ac)[i].access), (_p)); \
	   } \
     } while(0)
		
extern int pm_qtc_phy_control_set(phymod_phy_access_t *phy_access, int chain_len, soc_phy_control_t type, phymod_tx_t*, uint32, uint32 v);
/*extern int pm_qtc_phy_control_get(int u, soc_port_t p, pm_info_t pm, soc_phy_control_t type, phymod_tx_t*, uint32, uint32 *v);*/
extern int pm_qtc_phy_control_get(phymod_phy_access_t *phy_access, int chain_len, soc_phy_control_t type, phymod_tx_t*, uint32, uint32 *v);

/* Warm Boot Variables to indicate the boot state */
typedef enum pm_qtc_fabric_wb_vars{
    isInitialized,
    isActive,
    isBypassed,
    ports,
    threePortsMode,
    phy_type
}pm_qtc_wb_vars_t;


typedef struct pm_qtc_port_s {
    portmod_port_interface_config_t    interface_config;
    portmod_port_init_config_t         port_init_config;
    phymod_phy_init_config_t           phy_init_config;
    phymod_phy_inf_config_t            phy_interface_config;
} pm_qtc_port_t;

struct pm_qtc_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t core_access[1 + MAX_PHYN];
    uint32 first_phy_lane[1 + MAX_PHYN]; /* need one less, keep in line with core_access index. */
    int nof_phys; /* internal + External Phys */
    pm_qtc_port_t  port_config[PM_QTC_LANES_PER_CORE];
    uint8 default_fw_loader_is_used;
    uint8 default_bus_is_used;
    portmod_phy_external_reset_f  portmod_phy_external_reset;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    int core_num;
    int core_num_int;  /* 0, 1, 2 for TSC12 * - leaving it here, in case we add gphy here */
};

#define PM_QTC_QUAD_MODE_IF(interface) \
    (interface == SOC_PORT_IF_CAUI  || \
     interface == SOC_PORT_IF_XLAUI || \
     interface == SOC_PORT_IF_DNX_XAUI)

#define PM_QTC_DUAL_MODE_IF(interface) \
    (interface == SOC_PORT_IF_RXAUI)

#define PM_QTC_EGR_1588_TIMESTAMP_MODE_48BIT (0)
#define PM_QTC_EGR_1588_TIMESTAMP_MODE_32BIT (1)
STATIC
int pm_qtc_port_soft_reset(int unit, int port, pm_info_t pm_info, int enable);

int pm_qtc_pm_interface_type_is_supported(int unit, soc_port_if_t interface, 
                                          int* is_supported)
{
    SOC_INIT_FUNC_DEFS;
    switch(interface){
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_QSGMII:
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_XFI:
            *is_supported = TRUE;
            break;
        default:
            *is_supported = FALSE;
            break;
    }
    SOC_FUNC_RETURN;
}

STATIC
int _pm_qtc_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM_QTC; i++) {
       rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD, 
                     pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in the variables like
 * isInitialized, isActive, isBypassed, ports.. etc.,
 * All of these variables need to be added to warmboot
 * any variables added to save the state of warmboot should be 
 * included here.  
 */
STATIC
int pm_qtc_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv=0;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm_qtc", NULL, NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_ports_mode", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threePortsMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phy_type] = wb_var_id;


    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
    if(!SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], -1);
        _SOC_IF_ERR_EXIT(rv);
    }


exit:
   SOC_FUNC_RETURN


}

STATIC portmod_ucode_buf_t pm_qtc_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

int pm_qtc_pm_destroy(int unit, pm_info_t pm_info)
{
    if(pm_info->pm_data.pm_qtc_db != NULL) {
        sal_free(pm_info->pm_data.pm_qtc_db);
        pm_info->pm_data.pm_qtc_db = NULL;
    }

    /* free pm_qtc_ucode_buf */
    if(pm_qtc_ucode_buf[unit].ucode_dma_buf != NULL){
        portmod_sys_dma_free(unit, pm_qtc_ucode_buf[unit].ucode_dma_buf);
        pm_qtc_ucode_buf[unit].ucode_dma_buf = NULL;
    }
    pm_qtc_ucode_buf[unit].ucode_alloc_size = 0;

    return SOC_E_NONE;
}

STATIC int
pm_qtc_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{

    int rv;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    portmod_ucode_buf_order_t load_order;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;
    /* user_data->blk_id = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK; */

    if (PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_GET(user_data)) {
        load_order = portmod_ucode_buf_order_reversed;
    } else {
        load_order = portmod_ucode_buf_order_straight;
    }

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              load_order,
                              0,
                              &(pm_qtc_ucode_buf[unit]),
                              NULL,
                              GPORT_WC_UCMEM_DATAm,
                              GPORT_WC_UCMEM_CTRLr );
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

STATIC
int
pm_qtc_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(GPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm_qtc_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(GPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

int pm_qtc_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{                    
    int phy_acc;      
    uint32 reg_val, old_val;

    SOC_INIT_FUNC_DEFS;

    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
            
    _SOC_IF_ERR_EXIT(READ_COMMAND_CONFIGr(unit, phy_acc, &reg_val));
    old_val = reg_val;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_RESETf, 1);

    _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, old_val));

exit:
    SOC_FUNC_RETURN;        
}

phymod_bus_t pm_qtc_default_bus = {
    "PM_QTC Bus",
    pm_qtc_default_bus_read,
    pm_qtc_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

int pm_qtc_pm_init(int unit,
                   const portmod_pm_create_info_internal_t* pm_add_info,
                   int wb_buffer_index, pm_info_t pm_info)
{

    const portmod_pm_qtc_create_info_t *info;
    pm_qtc_t pm_qtc_data = NULL;
    int i, pm_is_active, rv ;
    SOC_INIT_FUNC_DEFS;

    info = &pm_add_info->pm_specific_info.pm_qtc;
    pm_qtc_data = sal_alloc(sizeof(struct pm_qtc_s), "pm_qtc_specific_db");
    SOC_NULL_CHECK(pm_qtc_data);

    pm_info->type = portmodDispatchTypePm_qtc;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm_qtc_db = pm_qtc_data;
    PORTMOD_PBMP_ASSIGN(pm_qtc_data->phys, pm_add_info->phys);
    pm_qtc_data->core_access[0].type = phymodDispatchTypeInvalid;
    pm_qtc_data->default_bus_is_used = 0;
    pm_qtc_data->default_fw_loader_is_used = 0;
    pm_qtc_data->portmod_phy_external_reset = pm_add_info->pm_specific_info.pm_qtc.portmod_phy_external_reset;
    pm_qtc_data->portmod_mac_soft_reset = pm_add_info->pm_specific_info.pm_qtc.portmod_mac_soft_reset; 
    pm_qtc_data->first_phy = -1;

    pm_qtc_data->nof_phys = 0;
    /* YTU why MAX_PHYN= 3? should it be just one core */
    for(i = 0 ; i < MAX_PHYN + 1; i++){ /* only one phy  */
        phymod_core_access_t_init(&pm_qtc_data->core_access[i]);
        pm_qtc_data->first_phy_lane[i] = 0;
    } 

    if(info->ref_clk != phymodRefClk156Mhz && info->ref_clk != phymodRefClk125Mhz) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal ref clock")));
    }
    if(info->lane_map.num_of_lanes != PM_QTC_LANES_PER_CORE){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                     (_SOC_MSG("lanes map length should be 4")));
    }
    _SOC_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(
                                         info->fw_load_method));

    sal_memcpy(&pm_qtc_data->polarity, &info->polarity,
                sizeof(phymod_polarity_t));
    sal_memcpy(&(pm_qtc_data->core_access[0].access), &info->access,
                sizeof(phymod_access_t));
    sal_memcpy(&pm_qtc_data->lane_map, &info->lane_map,
                sizeof(pm_qtc_data->lane_map));
    pm_qtc_data->ref_clk = info->ref_clk;
    pm_qtc_data->fw_load_method = info->fw_load_method;
    pm_qtc_data->external_fw_loader = info->external_fw_loader;

    if(info->access.bus == NULL) {
        /* if null - use default */
        pm_qtc_data->core_access[0].access.bus = &pm_qtc_default_bus;
        pm_qtc_data->default_bus_is_used = 1;
    }

    if(pm_qtc_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm_qtc_data->external_fw_loader = pm_qtc_default_fw_loader;
        pm_qtc_data->default_fw_loader_is_used = 1;
    }

    pm_qtc_data->nof_phys = 1;

    pm_qtc_data->core_num     = info->core_num;
    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm_qtc_wb_buffer_init(unit, wb_buffer_index,  pm_info));


    if(SOC_WARM_BOOT(unit)){
        for( i = 0 ; i < MAX_PORTS_PER_PM_QTC; i++) {
           rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[phy_type], &pm_qtc_data->core_access[i].type, i);
           _SOC_IF_ERR_EXIT(rv);

        }
    }

    if(!SOC_WARM_BOOT(unit)){

        pm_is_active = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

/*
         rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[isBypassed], 0);
         _SOC_IF_ERR_EXIT(rv);
         
         rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                       pm_info->wb_vars_ids[threePortsMode], 0);
         _SOC_IF_ERR_EXIT(rv);
         
         rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                           pm_info->wb_vars_ids[threePortsMode], 0);
         _SOC_IF_ERR_EXIT(rv);
*/


    }

exit:
    if(SOC_FUNC_ERROR){
        pm_qtc_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

int
pm_qtc_default_bus_update(int unit, pm_info_t pm_info, const portmod_bus_update_t* update) 
{
    return SOC_E_NONE;
}

STATIC
int _pm_qtc_tsc_reset(int unit, pm_info_t pm_info, int port, int in_reset)
{
    uint32 reg_val;
    int    phy_acc;
    portmod_default_user_access_t *user_data;
    phymod_core_access_t* core;

    SOC_INIT_FUNC_DEFS;

    core = (PM_QTC_INFO(pm_info)->core_access);
    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    /* PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc); */
    phy_acc = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;



 if(in_reset) {
       _SOC_IF_ERR_EXIT(READ_GPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
       /*
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 1);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, PLLBYPf, 0);
        */
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 1);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, REFIN_ENf, 0);
        /* soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, REFOUT_ENf, 0); */
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 0);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_PLLf, 0);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_REFCLKf, 0);
        /* soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, TSCCLK_ENf, 1); */
        _SOC_IF_ERR_EXIT(WRITE_GPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));
 } else {
        _SOC_IF_ERR_EXIT(READ_GPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 0);
        /* soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, PLLBYPf, 0); */
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 0);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, REFIN_ENf, 1);
        /* soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, REFOUT_ENf, 1); */
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 1);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_PLLf, 1);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_REFCLKf, 1);
        soc_reg_field_set(unit, GPORT_XGXS0_CTRL_REGr, &reg_val, TSCCLK_ENf, 1);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));
 }

 exit:
   
    SOC_FUNC_RETURN;
}

#if 0
static int _xlport_mode_get(int unit, int phy_acc, int bindex, 
                      portmod_core_port_mode_t *core_mode, int *cur_lanes)
{
  
    return SOC_E_NONE;
 
}

static int _xlport_mode_set(int unit, int phy_acc, portmod_core_port_mode_t cur_mode)
{
   
     
    return SOC_E_NONE;
}

static int _gport_lag_failover_status_toggle(int unit, pm_info_t pm_info, int port)
{
  
    uint32_t rval;
    int    phy_acc;
    portmod_default_user_access_t *user_data;
    phymod_core_access_t* core;

    SOC_INIT_FUNC_DEFS;

    core = (PM_QTC_INFO(pm_info)->core_access);
    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    /* PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc); */
    phy_acc = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_PGW_GE_LAG_FAILOVER_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, PGW_GE_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_PGW_GE_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));
    soc_reg_field_set(unit, PGW_GE_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_PGW_GE_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));

exit:

    SOC_FUNC_RETURN;

}

#endif

int pm_qtc_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return SOC_E_NONE;
    
}

int pm_qtc_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
  
    return SOC_E_NONE;
}

int pm_qtc_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    return SOC_E_NONE;
   
}

int pm_qtc_port_encap_set (int unit, int port, pm_info_t pm_info, 
                                int flags, portmod_encap_t encap)
{
    return (unimac_encap_set(unit, port, encap));
   
}

int pm_qtc_port_encap_get (int unit, int port, pm_info_t pm_info, 
                            int *flags, portmod_encap_t *encap)
{
    return (unimac_encap_get(unit, port, encap));
    
}

int pm_qtc_port_config_port_type_set (int unit, int port, pm_info_t pm_info, int type)
{
    return SOC_E_NONE;
   
}

int pm_qtc_port_config_port_type_get (int unit, int port, pm_info_t pm_info, int *type)
{
    return SOC_E_NONE;
   
}

int pm_qtc_port_led_chain_config (int unit, int port, pm_info_t pm_info, 
                          int value)
{
    return SOC_E_NONE;
   
}

typedef enum unimac_port_mode_e{
    XLMAC_4_LANES_SEPARATE  = 0,
    XLMAC_3_TRI_0_1_2_2     = 1,
    XLMAC_3_TRI_0_0_2_3     = 2,
    XLMAC_2_LANES_DUAL      = 3,
    XLMAC_4_LANES_TOGETHER  = 4
} unimac_port_mode_t;

int pm_qtc_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                    const portmod_access_get_params_t* params,
                                    int max_phys,
                                    phymod_phy_access_t* phy_access,
                                    int* nof_phys,
                                    int* is_most_ext)
{
    int phyn = 0, rv;
    int port_index, lane_mask;
    SOC_INIT_FUNC_DEFS;

    if(params->phyn > PM_QTC_INFO(pm_info)->nof_phys){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
               PM_QTC_INFO(pm_info)->nof_phys - 1, params->phyn));
    }

    phyn = params->phyn;

    if(phyn < 0) {
        phyn = PM_QTC_INFO(pm_info)->nof_phys - 1;
    }

    sal_memcpy (&phy_access[0], &(PM_QTC_INFO(pm_info)->core_access[phyn]),
                sizeof(PM_QTC_INFO(pm_info)->core_access[phyn]));

    if(SOC_WARM_BOOT(unit)) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &(phy_access[0].type), phyn);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get(unit, port, pm_info, &port_index,
                                          &(phy_access[0].access.lane_mask)));

    if (params->lane != -1) {
        lane_mask = port_index + params->lane;
        phy_access[0].access.lane_mask &= (0x1 << lane_mask);
    }

    phy_access[0].access.lane_mask <<= PM_QTC_INFO(pm_info)->first_phy_lane[phyn];

    *nof_phys = 1;

    if (is_most_ext) {
        if (phyn == PM_QTC_INFO(pm_info)->nof_phys-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm_qtc_pm_enable (int unit, int port, pm_info_t pm_info, int port_index, 
                        const portmod_port_add_info_t* add_info, int enable)
{
  
    uint32 reg_val;
    int    rv;
    int    phy_acc, temp, phy;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    phymod_core_access_t* core_access;
    uint32 nof_phys;
    const portmod_port_interface_config_t* config;
    portmod_pbmp_t port_phys_in_pm;
    SOC_INIT_FUNC_DEFS;

    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    config = &(add_info->interface_config);

    if(enable) {


       /* Configure GPort */
       /*  _SOC_IF_ERR_EXIT(READ_GPORT_RSV_MASKr(unit, phy_acc, &reg_val)); */
        reg_val = 0x78;
        _SOC_IF_ERR_EXIT(WRITE_GPORT_RSV_MASKr(unit, phy_acc, reg_val));

        reg_val = 0x3478;
        _SOC_IF_ERR_EXIT(WRITE_GPORT_STAT_UPDATE_MASKr(unit, phy_acc, reg_val));

       /* TBD : PGW_GE WRITE : GP0_TDM_MODEf */

        _SOC_IF_ERR_EXIT(READ_GPORT_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, CLR_CNTf, 0);
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, GPORT_ENf, 1);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, phy_acc, reg_val));

        _SOC_IF_ERR_EXIT(READ_GPORT_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, CLR_CNTf, 1);
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, GPORT_ENf, 1);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, phy_acc, reg_val));

        _SOC_IF_ERR_EXIT(READ_GPORT_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, CLR_CNTf, 0);
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, GPORT_ENf, 1);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, phy_acc, reg_val));

        /* Get Serdes OOR */
        _SOC_IF_ERR_EXIT(_pm_qtc_tsc_reset(unit, pm_info, port, 1));
        _SOC_IF_ERR_EXIT(_pm_qtc_tsc_reset(unit, pm_info, port, 0));

        nof_phys = PM_QTC_INFO(pm_info)->nof_phys;
        core_access = (PM_QTC_INFO(pm_info)->core_access);


        /* pass the port number in case legacy phy driver is used  */
        PM_QTC_PORT_NUM_TO_PHY_ACCESS(core_access,nof_phys,port);

        _SOC_IF_ERR_EXIT(portmod_port_phychain_core_probe(unit, port, core_access,
                                                              nof_phys));

        /* Get the first phy related to this port */
        PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
        PORTMOD_PBMP_AND(port_phys_in_pm, PM_QTC_INFO(pm_info)->phys);

        temp = 0;
        PORTMOD_PBMP_ITER(PM_QTC_INFO(pm_info)->phys, phy) {
            if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                             pm_info->wb_vars_ids[phy_type], &core_access->type, temp);
            _SOC_IF_ERR_EXIT(rv);
            }
            temp++;
        }

        _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

        /* set internal setting. */
        core_conf.firmware_load_method = PM_QTC_INFO(pm_info)->fw_load_method;
        core_conf.firmware_loader = PM_QTC_INFO(pm_info)->external_fw_loader;
        core_conf.lane_map = PM_QTC_INFO(pm_info)->lane_map;

        _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                   config->interface, &core_conf.interface.interface_type));
        core_conf.interface.data_rate = config->speed;
        core_conf.interface.interface_modes = config->interface_modes;
        core_conf.interface.ref_clock = PM_QTC_INFO(pm_info)->ref_clk;
        core_conf.interface.pll_divider_req = 0x2;  /* FIX THIS - IN PORTCTRL.c */

        _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
        core_status.pmd_active = 0;

        if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
            PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
        }

        _SOC_IF_ERR_EXIT(portmod_port_phychain_core_init(core_access,
                                                         nof_phys,
                                                         &core_conf,
                                                         &core_status));
    } else { /* disable */

        /* Put Serdes in reset*/
         _SOC_IF_ERR_EXIT(_pm_qtc_tsc_reset(unit, pm_info, port, 1));

        /* put MAC in reset */
        _SOC_IF_ERR_EXIT(unimac_soft_reset_set(unit, port, 1));
    }

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _pm_qtc_pm_port_init(int unit, int port, int phy_acc, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable)
{
    uint32 reg_val;
  /*   soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f}; */
    SOC_INIT_FUNC_DEFS;

   
    /* Configure UniMAC */
    if(enable) {
        _SOC_IF_ERR_EXIT(READ_FRM_LENGTHr(unit, phy_acc, &reg_val));
        reg_val = 0x2800;
        _SOC_IF_ERR_EXIT(WRITE_FRM_LENGTHr(unit, phy_acc, reg_val));

        _SOC_IF_ERR_EXIT(READ_COMMAND_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, CNTL_FRM_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, CRC_FWDf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, ENA_EXT_CONFIGf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, EN_INTERNAL_TX_CRSf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, ETH_SPEEDf, 0x2); /* SET THE SPEED TBD */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, FCS_CORRUPT_URUN_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, FD_TX_URUN_FIX_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, HD_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, IGNORE_TX_PAUSEf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, LINE_LOOPBACKf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, LOOP_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, MAC_LOOP_CONf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, NO_LGTH_CHECKf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, OOB_EFC_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, OVERFLOW_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAD_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAUSE_FWDf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAUSE_IGNOREf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PROMIS_ENf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RUNT_FILTER_DISf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ERR_DISCf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_LOW_LATENCY_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_OVERRIDE_RXf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_OVERRIDE_TXf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_RESETf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ADDR_INSf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ENAf, 0);
        _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, reg_val));
      
        _SOC_IF_ERR_EXIT(READ_COMMAND_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, CNTL_FRM_ENAf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, CRC_FWDf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, ENA_EXT_CONFIGf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, EN_INTERNAL_TX_CRSf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, ETH_SPEEDf, 0x2); /* SET THE SPEED TBD */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, FCS_CORRUPT_URUN_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, FD_TX_URUN_FIX_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, HD_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, IGNORE_TX_PAUSEf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, LINE_LOOPBACKf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, LOOP_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, MAC_LOOP_CONf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, NO_LGTH_CHECKf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, OOB_EFC_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, OVERFLOW_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAD_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAUSE_FWDf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAUSE_IGNOREf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PROMIS_ENf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RUNT_FILTER_DISf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ENAf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ERR_DISCf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_LOW_LATENCY_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_OVERRIDE_RXf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_OVERRIDE_TXf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_RESETf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ADDR_INSf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ENAf, 1);
        _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, reg_val));
      
        reg_val = 0x8;
        _SOC_IF_ERR_EXIT(WRITE_TX_IPG_LENGTHr(unit, phy_acc, reg_val));

    }
    else {
        _SOC_IF_ERR_EXIT(READ_COMMAND_CONFIGr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, CNTL_FRM_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, CRC_FWDf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, ENA_EXT_CONFIGf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, EN_INTERNAL_TX_CRSf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, ETH_SPEEDf, 0x2); /* SET THE SPEED TBD */
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, FCS_CORRUPT_URUN_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, FD_TX_URUN_FIX_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, HD_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, IGNORE_TX_PAUSEf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, LINE_LOOPBACKf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, LOOP_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, MAC_LOOP_CONf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, NO_LGTH_CHECKf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, OOB_EFC_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, OVERFLOW_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAD_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAUSE_FWDf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PAUSE_IGNOREf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, PROMIS_ENf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RUNT_FILTER_DISf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ENAf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_ERR_DISCf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, RX_LOW_LATENCY_ENf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_OVERRIDE_RXf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_OVERRIDE_TXf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_RESETf, 1);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ADDR_INSf, 0);
        soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, TX_ENAf, 0);
        _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, reg_val));
 
    }

exit:
    SOC_FUNC_RETURN;

       
}

STATIC
int portmod_interface_pm_qtc_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config) ;
STATIC
int portmod_interface_pm_qtc_xphy_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config) ;

/*
 *  clear pm_info's phy_interface_config, and set few members of phy_interface_config
 *  initialize phy_init_config tx params and set registers 
*/
int _pm_qtc_port_interface_config_init_set(int unit, int port, 
                        pm_info_t pm_info, 
                        const portmod_port_interface_config_t* config)
{
    uint32 pm_is_bypassed = 0; 
    int    i, nof_phys = 0, rv = 0; /* phy_acc; */
    phymod_phy_access_t phy_access[1 + MAX_PHYN];
    phymod_phy_access_t phy_access_lane;

    phymod_phy_inf_config_t     *phy_interface_config;
    portmod_access_get_params_t params;
    phymod_tx_t                 phymod_tx;
    portmod_port_init_config_t *init_config;
    phymod_phy_init_config_t   *phy_init_config;
    int                         port_index;
    uint32_t                    bitmap;

    SOC_INIT_FUNC_DEFS;

    /* PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc); */

    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    init_config          = &PM_QTC_INFO(pm_info)->port_config[port_index].port_init_config;
    phy_interface_config = &PM_QTC_INFO(pm_info)->port_config[port_index].phy_interface_config;
    phy_init_config      = &PM_QTC_INFO(pm_info)->port_config[port_index].phy_init_config;

    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isBypassed], 
                                &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
   
    if(!pm_is_bypassed){
        /* set port speed */
        rv = unimac_speed_set(unit, port, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        /* set encapsulation */
        rv = unimac_encap_set (unit, port, SOC_ENCAP_IEEE);
        _SOC_IF_ERR_EXIT(rv);
    }


    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm_qtc_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access_lane, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access_lane, phymodMediaTypeChipToChip, &phymod_tx));
    
    /* overlap preemphasis and amp from config */
    if(init_config->tx_params_user_flag & PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG) {
        phymod_tx.pre  = init_config->tx_params.pre ;
        phymod_tx.main = init_config->tx_params.main ;
        phymod_tx.post = init_config->tx_params.post ;
    }
    if(init_config->tx_params_user_flag & PORTMOD_USER_SET_TX_AMP_BY_CONFIG) {
        phymod_tx.amp  = init_config->tx_params.amp ;
    }
    for(i=0 ; i<PM_QTC_LANES_PER_CORE/4 ; i++) {
        phy_init_config->tx[i].pre  = phymod_tx.pre  ;
        phy_init_config->tx[i].main = phymod_tx.main ;
        phy_init_config->tx[i].post = phymod_tx.post ;
        phy_init_config->tx[i].post2= phymod_tx.post2;
        phy_init_config->tx[i].post3= phymod_tx.post3;
        phy_init_config->tx[i].amp  = phymod_tx.amp  ;
    }
    
    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
 
    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (phy_interface_config));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,  config->speed,
                     config->interface, &phy_interface_config->interface_type));

    phy_interface_config->data_rate       = config->speed;
    phy_interface_config->interface_modes = config->interface_modes;
    phy_interface_config->ref_clock       = PM_QTC_INFO(pm_info)->ref_clk;

    if(nof_phys > 1)
    {   /* external phy. */
        portmod_interface_pm_qtc_xphy_to_phymod_interface(unit, port,
                                        config, init_config, phy_interface_config);
    }
    else
    {   /* internal phy. */
        portmod_interface_pm_qtc_to_phymod_interface(unit, port,
                                        config, init_config, phy_interface_config);
    }
 
    SOC_IF_ERROR_RETURN(portmod_port_phychain_tx_set(phy_access,
                                                         nof_phys, phy_init_config->tx));
exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_attach (int unit, int port, pm_info_t pm_info, 
                        const portmod_port_add_info_t* add_info)
{
    int        port_index = -1, rv = 0, phy, i, phys_count = 0, nof_phys, my_i, port_i;
    uint32     pm_is_active = 0, bitmap;
    /* uint32 pm_is_bypassed = 0; */
    portmod_pbmp_t port_phys_in_pm;
    int  first_phy = -1, phy_acc;

    phymod_phy_init_config_t    *phy_init_config;
    phymod_phy_inf_config_t     *phy_interface_config;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_access_t phy_access_lane;
    portmod_port_ability_t port_ability;
    portmod_access_get_params_t params;


    SOC_INIT_FUNC_DEFS;

    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* pm_is_bypassed = 0; */

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_QTC_INFO(pm_info)->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);

    i = 0;
    PORTMOD_PBMP_ITER(PM_QTC_INFO(pm_info)->phys, phy) {
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                             pm_info->wb_vars_ids[ports], &port, i);
            _SOC_IF_ERR_EXIT(rv);
            first_phy = (first_phy == -1) ? phy : first_phy;
            port_index = (port_index == -1 ? i : port_index);
        }
        i++;
    }

    PM_QTC_INFO(pm_info)->first_phy = first_phy;
    if(port_index<0 || port_index>=PM_QTC_LANES_PER_CORE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("can't attach port %d. can't find relevant phy"), port));
    }

    /* port_index is 0, 1, 2, or 3 */
    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* init data for interface_config and port_init_config */
    sal_memcpy(&PM_QTC_INFO(pm_info)->port_config[port_index].interface_config,
                    &add_info->interface_config,
                    sizeof(portmod_port_interface_config_t));

    sal_memcpy(&PM_QTC_INFO(pm_info)->port_config[port_index].port_init_config,
                   &add_info->init_config,
                   sizeof(portmod_port_init_config_t));

    /* if not active - initalize PM */
    if(!pm_is_active){

        /*init the PM*/
        rv = _pm_qtc_pm_enable (unit, port, pm_info, port_index, add_info, 1);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_active = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                 pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* initalize port */
    rv = _pm_qtc_pm_port_init(unit, port, phy_acc, port_index, add_info, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phys */
    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
     phy_init_config      = &PM_QTC_INFO(pm_info)->port_config[port_index].phy_init_config;
     phy_interface_config = &PM_QTC_INFO(pm_info)->port_config[port_index].phy_interface_config;

    /* clean up polarity and tx of phy_init_config */
    _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(phy_init_config));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    _SOC_IF_ERR_EXIT(pm_qtc_port_phy_lane_access_get(unit, port, pm_info,
                                &params, 1, &phy_access_lane, &nof_phys, NULL));

    my_i = 0;
    for(i=0 ; i<PM_QTC_LANES_PER_CORE ; i++) {

        rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[ports], &port_i, i);
        _SOC_IF_ERR_EXIT(rv);

        if(port_i != port) {
            continue;
        }

        if(SHR_BITGET(&PM_QTC_INFO(pm_info)->polarity.tx_polarity,i)) {
            SHR_BITSET(&phy_init_config->polarity.tx_polarity, my_i);
        }

        if(SHR_BITGET(&PM_QTC_INFO(pm_info)->polarity.rx_polarity,i)) {
            SHR_BITSET(&phy_init_config->polarity.rx_polarity, my_i);
        }

        /*
        _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access_lane, phymodMediaTypeChipToChip, &phy_init_config>tx[my_i]));
        */
        my_i++;
    }

    phy_init_config->op_mode = add_info->phy_op_mode;

    _SOC_IF_ERR_EXIT(_pm_qtc_port_interface_config_init_set(unit, port,
                          pm_info, &add_info->interface_config));

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                           phy_access ,(1+MAX_PHYN),
                                                           &nof_phys));

    _SOC_IF_ERR_EXIT(portmod_port_phychain_phy_init(phy_access, nof_phys,
                                                            phy_init_config));

    _SOC_IF_ERR_EXIT(portmod_port_phychain_interface_config_set(unit, port, phy_access, nof_phys,
                                   PM_QTC_INFO(pm_info)->port_config[port_index].interface_config.flags ,
                                   phy_interface_config,
                                   PM_QTC_INFO(pm_info)->ref_clk,
                                   TRUE )); /* Apply to ALL PHY with same interface config */


    /* set the default advert ability */
    _SOC_IF_ERR_EXIT
        (pm_qtc_port_ability_local_get(unit, port, pm_info, &port_ability));
    _SOC_IF_ERR_EXIT
        (pm_qtc_port_ability_advert_set(unit, port, pm_info, &port_ability));

    /* De-Assert SOFT_RESET */
  _SOC_IF_ERR_EXIT(pm_qtc_port_soft_reset(unit, port, pm_info, 0));
exit:
    SOC_FUNC_RETURN;
}


int pm_qtc_port_detach(int unit, int port, pm_info_t pm_info)
{
    int     enable, tmp_port, i=0, rv=0, port_index = -1;
    int     invalid_port = -1, is_last_one = TRUE, phy_acc;
    uint32  inactive = 0;

    SOC_INIT_FUNC_DEFS;

    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(pm_qtc_port_enable_get(unit, port, pm_info, 0, &enable));
    if(enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
                  (_SOC_MSG("can't detach active port %d"), port));
    }

    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM_QTC; i++) {
       rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, 
                    pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           port_index = (port_index == -1 ? i : port_index);
           rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, 
                    pm_info->wb_vars_ids[ports], &invalid_port, i);
           _SOC_IF_ERR_EXIT(rv);
       } else if (tmp_port != -1) {
           is_last_one = FALSE;
       }
    }

    if(port_index == -1) { 
        _SOC_EXIT_WITH_ERR(SOC_E_PORT,(_SOC_MSG("Port %d wasn't found"),port));
    }   

    rv = _pm_qtc_pm_port_init(unit, port, phy_acc, port_index, NULL, 0);
    _SOC_IF_ERR_EXIT(rv);

    /*deinit PM in case of last one*/
    if (is_last_one) {
       rv = _pm_qtc_pm_enable(unit, port, pm_info, port_index, NULL, 0);
       _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
                             pm_info->wb_vars_ids[isActive], &inactive);
       _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}


int pm_qtc_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    return SOC_E_NONE;

}

int pm_qtc_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    return SOC_E_NONE;
   
}

int pm_qtc_port_enable_set (int unit, int port, pm_info_t pm_info, 
                            int flags, int enable)
{
 
    uint32  is_bypassed = 0;
    int     rv = 0; 
    int     actual_flags = flags, /*rst_flags, */  nof_phys = 0 ; 

    phymod_phy_power_t           phy_power;
    phymod_phy_access_t phy_access[1+MAX_PHYN]; 

    SOC_INIT_FUNC_DEFS;

    /* If no Rx/Tx flags - set both */
    if ((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && 
        (!PORTMOD_PORT_ENABLE_RX_GET(flags)))
    {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* If no Mac/Phy flags - set both */
    if ((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && 
        (!PORTMOD_PORT_ENABLE_MAC_GET(flags)))
    {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, 
                               pm_info->wb_vars_ids[isBypassed], &is_bypassed);
     _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    phy_power.rx = phymodPowerNoChange;
    phy_power.tx = phymodPowerNoChange;

    if(PORTMOD_PORT_ENABLE_RX_GET(actual_flags)){
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
    }
    if(PORTMOD_PORT_ENABLE_TX_GET(actual_flags)){
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
    }

    if(enable) {
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)){
      	    if(PM_QTC_INFO(pm_info)->portmod_mac_soft_reset) {
		 /* rst_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS; */
	    }
	    rv = unimac_enable_set(unit, port, 0, 1);
            _SOC_IF_ERR_EXIT(rv);
      	    if(PM_QTC_INFO(pm_info)->portmod_mac_soft_reset) {
		        _SOC_IF_ERR_EXIT(pm_qtc_port_soft_reset(unit, port, pm_info, 0));
	        }
        }

        if(PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))
        {
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));
    
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, &phy_power));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_tx_lane_control_set(unit, port, phy_access, nof_phys,
                                                           phymodTxSquelchOff));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_rx_lane_control_set(unit, port, phy_access, nof_phys,
                                                          phymodRxSquelchOff));
        }
    } else {
        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                                  phy_access ,(1+MAX_PHYN),
                                                   &nof_phys));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_tx_lane_control_set(unit, port, phy_access, nof_phys,
                                                           phymodTxSquelchOn));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_rx_lane_control_set(unit, port, phy_access, nof_phys,
                                                           phymodRxSquelchOn));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, &phy_power));
        } 

        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)){
            rv = unimac_enable_set(unit, port, 0, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;

    return SOC_E_NONE;

}

int pm_qtc_port_enable_get (int unit, int port, pm_info_t pm_info, int flags, 
                            int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    int phy_enable = 0;
    int mac_enable = 0, mac_rx_enable, mac_tx_enable;
    
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    if (PORTMOD_PORT_ENABLE_PHY_GET(flags)||(0 == flags)) {

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));

            _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_tx_lane_control_get(phy_access, nof_phys,
                                                               &tx_control));
            _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_rx_lane_control_get(phy_access, nof_phys,
                                                               &rx_control));
        phy_enable = ((tx_control == phymodTxSquelchOn) &&
                     (rx_control == phymodRxSquelchOn)) ? 0 : 1;
        if(0 != flags) {
            *enable = phy_enable;
        }
    } 

    if (PORTMOD_PORT_ENABLE_MAC_GET(flags)||(0 == flags)) {
        if (PORTMOD_PORT_ENABLE_RX_GET(flags)||(0 == flags)) {
            _SOC_IF_ERR_EXIT(unimac_enable_get(unit, port, UNIMAC_ENABLE_SET_FLAGS_RX_EN, &mac_rx_enable));
            mac_enable |= (mac_rx_enable) ? 1 : 0;
        }
        if (PORTMOD_PORT_ENABLE_TX_GET(flags)||(0 == flags)) {
            _SOC_IF_ERR_EXIT(unimac_enable_get(unit, port, UNIMAC_ENABLE_SET_FLAGS_TX_EN, &mac_tx_enable));
            mac_enable |= (mac_tx_enable) ? 1 : 0;
        }
        if(0 != flags) {
            *enable = mac_enable;
        }
    }

    if(0 == flags){
        *enable = (mac_enable || phy_enable) ;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_interface_pm_qtc_to_phymod_interface
 *
 * @brief  passing and process data structure from in to out 
 *          
 * @param[in]   interface_config
 * @param[im]   init_config
 *
 * @param[out]  phy_interface_config
 */
STATIC
int portmod_interface_pm_qtc_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config)
{
    int port_num_lanes;
    int fiber_pref;
        
    port_num_lanes         = interface_config->port_num_lanes;
    fiber_pref             = PHYMOD_INTF_MODES_FIBER_GET(interface_config);

    /*for qsgmii mode */
    if (port_num_lanes == 1) {
            phy_interface_config->interface_type = phymodInterfaceQSGMII;
    } else {
        switch (interface_config->speed) {
            case 10:
                phy_interface_config->interface_type = phymodInterfaceSGMII;
                break;
            case 100:
                phy_interface_config->interface_type = phymodInterfaceSGMII;
                break;
            case 1000:
                if (fiber_pref) {
                    phy_interface_config->interface_type = phymodInterface1000X;
                } else {
                    phy_interface_config->interface_type = phymodInterfaceSGMII;
                }
                break;
            case 2500:
                phy_interface_config->interface_type = phymodInterface1000X;
                break;
            default:
                return SOC_E_PARAM;
        }
    }
    return SOC_E_PARAM;
}

/*!
 * portmod_interface_pm_qtc_xphy_to_phymod_interface
 *
 * @brief  passing and process data structure from in to out 
 *          
 * @param[in]   interface_config
 * @param[im]   init_config
 *
 * @param[out]  phy_interface_config
 */
STATIC
int portmod_interface_pm_qtc_xphy_to_phymod_interface(int unit,
                                                  int port,
                                                  const portmod_port_interface_config_t *interface_config,
                                                  const portmod_port_init_config_t *init_config,
                                                  phymod_phy_inf_config_t *phy_interface_config)
{
    return SOC_E_NONE;
   
        
      
 
}
 
int pm_qtc_port_interface_config_set(int unit, int port,
                        pm_info_t pm_info,
                        const portmod_port_interface_config_t* config,
                        int phy_init_flags)
{
    uint32 pm_is_bypassed = 0; 
    int    nof_phys = 0, rv = 0; /*  phy_acc; */
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    phymod_phy_access_t         phy_access;
    phymod_phy_inf_config_t     phy_interface_config;
    portmod_access_get_params_t params;
    portmod_port_init_config_t *init_config;
    phymod_phy_init_config_t   *phy_init_config;
    int                         port_index;
    uint32_t                    bitmap;
    portmod_port_interface_config_t *interface_config ;
    portmod_port_interface_config_t  config_temp ;
    SOC_INIT_FUNC_DEFS;

    /* PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc); */

    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    init_config          = &PM_QTC_INFO(pm_info)->port_config[port_index].port_init_config;
    phy_init_config      = &PM_QTC_INFO(pm_info)->port_config[port_index].phy_init_config;
    interface_config     = &PM_QTC_INFO(pm_info)->port_config[port_index].interface_config;

    interface_config->port_refclk_int       = PM_QTC_INFO(pm_info)->ref_clk;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    params.lane = -1;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(pm_qtc_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));

    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD, 
                                pm_info->wb_vars_ids[isBypassed], 
                                &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if(config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        /* only flag saved by interface_config->flags */
        interface_config->flags     = PHYMOD_INTF_F_INTF_PARAM_SET_ONLY;
        interface_config->interface = config->interface ;

        SOC_FUNC_RETURN;
    }

    if (!(config->flags & PHYMOD_INTF_F_SET_SPD_TRIGGER) && (!pm_is_bypassed)){

        /* set port speed */
        /*flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS; */
        rv = unimac_speed_set(unit, port, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        /* set encapsulation */
        /*flags = XLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS; */
        rv = unimac_encap_set (unit, port, /*flags, */ config->encap_mode);
 	    _SOC_IF_ERR_EXIT(rv);
 	
        /* De-Assert SOFT_RESET */
	 	rv = pm_qtc_port_soft_reset(unit, port, pm_info, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,  config->speed,
                     config->interface, &phy_interface_config.interface_type));

    phy_interface_config.data_rate       = config->speed;
    phy_interface_config.interface_modes = config->interface_modes;
    phy_interface_config.ref_clock       = PM_QTC_INFO(pm_info)->ref_clk;

    /* copy const *config to config_temp cause config.interface may come from storage */
    _SOC_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &config_temp));
    sal_memcpy(&config_temp, config, sizeof(portmod_port_interface_config_t));

    if(interface_config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        /* overwrite config->interface form storage */
        config_temp.interface                = interface_config->interface ;
        interface_config->flags              = 0 ;
    }

    portmod_interface_pm_qtc_to_phymod_interface(unit, port, 
                                        &config_temp, init_config, &phy_interface_config);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access_arr ,(1+MAX_PHYN),
                                           &nof_phys));

    if( nof_phys > 1 ){
        /* external phy interface translation. */
        portmod_interface_pm_qtc_xphy_to_phymod_interface(unit, port, 
                                        &config_temp, init_config, &phy_interface_config);

    } else {
        portmod_interface_pm_qtc_to_phymod_interface(unit, port, 
                                        &config_temp, init_config, &phy_interface_config);
    }

    if (!(config->flags & PHYMOD_INTF_F_SET_SPD_TRIGGER)) {
            _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_set(phy_access_arr, nof_phys, phy_init_config->tx));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_set(unit, port, phy_access_arr, nof_phys,
                                                    config_temp.flags,
                                                    &phy_interface_config,
                                                    PM_QTC_INFO(pm_info)->ref_clk,
                                                    phy_init_flags));
exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_interface_config_get (int unit, int port, pm_info_t pm_info, 
                                      portmod_port_interface_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t      phy_interface_config;
    portmod_port_interface_config_t *interface_config ;
    int                          port_index;
    uint32_t                     bitmap;
    int nof_phys = 0, rv = 0,  ref_clock ;
    
    SOC_INIT_FUNC_DEFS;
    
    ref_clock = PM_QTC_INFO(pm_info)->ref_clk;

    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    interface_config     = &PM_QTC_INFO(pm_info)->port_config[port_index].interface_config;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_interface_config));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    phy_interface_config.data_rate =  interface_config->max_speed;
    phy_interface_config.interface_type =  phymodInterfaceXGMII;
    phy_interface_config.ref_clock =  ref_clock;

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                    0, ref_clock,
                                                    &phy_interface_config));

    config->speed           = phy_interface_config.data_rate;
    config->interface_modes = phy_interface_config.interface_modes;
    config->flags           = 0;
    config->interface       = interface_config->interface;
    rv = portmod_intf_from_phymod_intf (unit, 
             phy_interface_config.interface_type, &(config->interface));
    _SOC_IF_ERR_EXIT(rv);

    rv = unimac_encap_get(unit, port, &config->encap_mode); 
    _SOC_IF_ERR_EXIT(rv);
                          
    config->port_num_lanes  = interface_config->port_num_lanes;
    config->max_speed       = interface_config->max_speed;
    config->pll_divider_req = interface_config->pll_divider_req;
    config->interface_modes = interface_config->interface_modes;

exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    *interface = SOC_PORT_IF_NULL;
    return SOC_E_NONE;
}

int pm_qtc_port_loopback_get(int unit, int port, pm_info_t pm_info, 
                 portmod_loopback_mode_t loopback_type, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    uint32_t tmp_enable=0;
    phymod_loopback_mode_t phymod_lb_type;

    SOC_INIT_FUNC_DEFS;

    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(unimac_loopback_get(unit, port, enable));
            break;

        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
             if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
                (*enable) = 0; /* No phy --> no phy loopback*/
             } else {
                _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));
                _SOC_IF_ERR_EXIT
                    (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                      phy_access ,(1+MAX_PHYN),
                                                      &nof_phys));

                _SOC_IF_ERR_EXIT(portmod_port_phychain_loopback_get(unit, port,
                                                                    phy_access,
                                                                    nof_phys,
                                                                    phymod_lb_type,
                                                                    &tmp_enable));
                *enable = tmp_enable;

             }
             break;
        default:
            (*enable) = 0; /* not supported --> no loopback */
            break;
    }

exit:
    SOC_FUNC_RETURN;

}

int pm_qtc_port_loopback_set (int unit, int port, pm_info_t pm_info, 
                  portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    phymod_loopback_mode_t phymod_lb_type;

    SOC_INIT_FUNC_DEFS;

/*
    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
*/

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(unimac_loopback_set(unit, port, enable));
        break;

    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPCS: /*slide*/
        if(PM_QTC_INFO(pm_info)->nof_phys != 0) {
            if (enable) {
                _SOC_IF_ERR_EXIT(unimac_loopback_set(unit, port, 0));
            }
            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));

            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT(portmod_port_phychain_loopback_set(unit, port,
                                                                phy_access,
                                                                nof_phys,
                                                                phymod_lb_type,
                                                                enable));
         }
         break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                  _SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }
#if 0
    if (enable)
        _SOC_IF_ERR_EXIT(_gport_lag_failover_status_toggle(unit, pm_info, port));
#endif
exit:
    SOC_FUNC_RETURN;

}

int pm_qtc_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm_qtc_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_enable_get(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm_qtc_port_ability_local_get (int unit, int port, pm_info_t pm_info, 
                                   portmod_port_ability_t* ability)
{
    portmod_port_interface_config_t *interface_config;
    int                 fiber_pref, port_index;
    /* int                 max_speed = 1000; */
    int                 num_of_lanes = 0 ;
    uint32              bitmap, tmpbit ;
  

    if (_pm_qtc_port_index_get (unit, port, pm_info, &port_index, &bitmap))
        return (SOC_E_PARAM);
    
    interface_config = &(PM_QTC_INFO(pm_info)->port_config[port_index].interface_config);

    fiber_pref        = PHYMOD_INTF_MODES_FIBER_GET(interface_config);
    /* max_speed         = interface_config->max_speed; */

    tmpbit = bitmap ;
    while(tmpbit) {
        num_of_lanes += ((tmpbit&1)?1:0) ;
        tmpbit >>= 1 ;
    }

    sal_memset(ability, 0, sizeof(*ability));


    ability->loopback  = SOC_PA_LB_PHY | SOC_PA_LB_MAC;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    
    if (num_of_lanes==1) {
        ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB |
                                           SOC_PA_SPEED_1000MB |
                                           SOC_PA_SPEED_2500MB;

        /* 
        if (fiber_pref) {
            ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB |
                                           SOC_PA_SPEED_10GB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
        } else {
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
        }
        */

        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;

    } else {
        
        ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
        if (fiber_pref)   {
            ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
        } else {
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                       SOC_PA_SPEED_100MB;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->flags     = SOC_PA_AUTONEG;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "%-22s:unit=%d p=%d sp=%08x bitmap=%x\n"),
              __func__, unit, port, ability->speed_full_duplex, bitmap));

    return (SOC_E_NONE);
}

static int _pm_qtc_nof_lanes_get(int unit, int port, pm_info_t pm_info)
{
    int port_index, rv;
    uint32_t bitmap, bcnt = 0;

    rv = _pm_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap);
    if (rv) return (0); /* bit count is zero will flag error */

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }

    return (bcnt);

}

int pm_qtc_port_autoneg_set (int unit, int port, pm_info_t pm_info, 
                             const phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32_t bitmap;
    phymod_autoneg_control_t  *pAn = (phymod_autoneg_control_t*)an;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (pAn->num_lane_adv == 0) {
            pAn->num_lane_adv = _pm_qtc_nof_lanes_get(unit, port, pm_info);
        }

        pAn->an_mode = PM_QTC_INFO(pm_info)->port_config[port_index].port_init_config.an_mode;

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = phymod_AN_MODE_SGMII;
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_autoneg_set(unit, port, phy_access, nof_phys, an));
    }

exit:
    SOC_FUNC_RETURN;

 
}

int pm_qtc_port_autoneg_get (int unit, int port, pm_info_t pm_info, 
                             phymod_autoneg_control_t* an)
{      
    return SOC_E_NONE;
 
}

int pm_qtc_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{   
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    
    SOC_INIT_FUNC_DEFS; 

    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Autoneg isn't supported")));
    } else {
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_status_get(phy_access, nof_phys, an_status));
    }

exit:
    SOC_FUNC_RETURN;
    
}

int pm_qtc_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_link_status_get(phy_access, nof_phys, flags, (uint32_t*) link));
exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    return SOC_E_NONE;
}


int pm_qtc_port_prbs_config_set(int unit, int port, pm_info_t pm_info, 
                     portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PMQTC")));
    }

    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                            (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_set(phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_prbs_config_get (int unit, int port, pm_info_t pm_info, 
                                 portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC PRBS is not supported for PMQTC")));
    }

    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                       (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_get(phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}


int pm_qtc_port_prbs_enable_set (int unit, int port, pm_info_t pm_info, 
                                 portmod_prbs_mode_t mode, int flags, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;
    
    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (
              _SOC_MSG("MAC PRBS is not supported for PMQTC")));
    }
    
    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
              _SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_set(phy_access, nof_phys, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_prbs_enable_get (int unit, int port, pm_info_t pm_info, 
                                 portmod_prbs_mode_t mode, int flags, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 is_enabled;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PMQTC")));
    }

    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                    (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_get(phy_access, nof_phys, flags, &is_enabled));

    (*enable) = (is_enabled ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}


int pm_qtc_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                    portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
                (_SOC_MSG("MAC PRBS is not supported for PMQTC")));
    }

    if(PM_QTC_INFO(pm_info)->nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, 
                    (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_status_get(phy_access, nof_phys, flags, status));

exit:
    SOC_FUNC_RETURN;

}


int pm_qtc_port_firmware_mode_set (int unit, int port, pm_info_t pm_info,
                                   phymod_firmware_mode_t fw_mode)
{
        
    return SOC_E_NONE;
    
}

int pm_qtc_port_firmware_mode_get (int unit, int port, pm_info_t pm_info, 
                                   phymod_firmware_mode_t* fw_mode)
{
        
    
    return SOC_E_NONE;
}


int pm_qtc_port_runt_threshold_set (int unit, int port, 
                                    pm_info_t pm_info, int value)
{
        
    return SOC_E_NONE;
    
}

int pm_qtc_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    return SOC_E_NONE;
    
}


int pm_qtc_port_max_packet_size_set (int unit, int port, 
                                     pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_max_size_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;

}

int pm_qtc_port_max_packet_size_get (int unit, int port, 
                                     pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(unimac_rx_max_size_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;

}


int pm_qtc_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    return SOC_E_NONE;
        
}

int pm_qtc_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    return SOC_E_NONE;
        
    
}


int pm_qtc_port_tx_drop_on_local_fault_set (int unit, int port, 
                                            pm_info_t pm_info, int enable)
{
    return SOC_E_NONE;
}

int pm_qtc_port_tx_drop_on_local_fault_get (int unit, int port, 
                                            pm_info_t pm_info, int* enable)
{
    return SOC_E_NONE;
}


int pm_qtc_port_tx_drop_on_remote_fault_set(int unit, int port,
                                            pm_info_t pm_info, int enable)
{
    return SOC_E_NONE;
}

int pm_qtc_port_tx_drop_on_remote_fault_get (int unit, int port, 
                                             pm_info_t pm_info, int* enable)
{
    return SOC_E_NONE;
}

int pm_qtc_port_local_fault_enable_set (int unit, int port, 
                                        pm_info_t pm_info, int enable)
{
    return SOC_E_NONE;
}

int pm_qtc_port_local_fault_enable_get (int unit, int port, 
                                        pm_info_t pm_info, int* enable)
{
    return SOC_E_NONE;
}

int pm_qtc_port_local_fault_status_get(int unit, int port,
                                       pm_info_t pm_info, int* value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_local_fault_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_local_fault_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_remote_fault_status_get(int unit, int port,
                                        pm_info_t pm_info, int* value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_remote_fault_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_remote_fault_control_t* control)
{
    return SOC_E_NONE;
}


int pm_qtc_port_pause_control_set(int unit, int port, pm_info_t pm_info, 
                                  const portmod_pause_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_pause_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_llfc_control_set(int unit, int port, 
           pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_llfc_control_get(int unit, int port,
            pm_info_t pm_info, portmod_llfc_control_t* control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_core_access_get (int unit, int port, 
                                 pm_info_t pm_info, int phyn, int max_cores, 
                                 phymod_core_access_t* core_access_arr, 
                                 int* nof_cores,
                                 int* is_most_ext)
{
    return SOC_E_NONE;
}

int pm_qtc_ext_phy_attach_to_pm(int unit, pm_info_t pm_info, const phymod_core_access_t *ext_phy_access, uint32 first_phy_lane)
{
    return SOC_E_NONE;
}

int pm_qtc_ext_phy_detach_from_pm(int unit, pm_info_t pm_info, phymod_core_access_t *ext_phy_access)
{
    return SOC_E_NONE;
}

/*!
 * pm_qtc_port_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm_qtc_port_frame_spacing_stretch_set (int unit, int port, 
                                           pm_info_t pm_info,int spacing)
{
    return SOC_E_NONE;
}

/*! 
 * pm_qtc_port_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm_qtc_port_frame_spacing_stretch_get (int unit, int port,
                                            pm_info_t pm_info, 
                                            const int *spacing)
{
    return SOC_E_NONE;
}

/*! 
 * pm_qtc_port_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int pm_qtc_port_diag_fifo_status_get (int unit, int port,pm_info_t pm_info, 
                                 const portmod_fifo_status_t* diag_info)
{
    return SOC_E_NONE;
}

/*! 
 * pm_qtc_port_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm_qtc_port_pfc_config_set (int unit, int port,pm_info_t pm_info,
                           const portmod_pfc_config_t* pfc_cfg)
{
    /* return(unimac_pfc_config_set(unit, port, pfc_cfg)); */
    return SOC_E_NONE;
}


/*! 
 * pm_qtc_port_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm_qtc_port_pfc_config_get (int unit, int port,pm_info_t pm_info, 
                           const portmod_pfc_config_t* pfc_cfg)
{
    /* return (unimac_pfc_config_get (unit, port, pfc_cfg)); */
    return SOC_E_NONE;
}

int pm_qtc_port_pfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_control_t* control)
{
        
    return SOC_E_NONE;
    
}

int pm_qtc_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
        
    
    return SOC_E_NONE;
}

/*!
 * pm_qtc_port_eee_set
 *
 * @brief set EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm_qtc_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    return SOC_E_NONE;
}

/*!
 * pm_qtc_port_eee_get
 *
 * @brief get EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm_qtc_port_eee_get(int unit, int port, pm_info_t pm_info, portmod_eee_t* eee)
{
    return SOC_E_NONE;
}

/*! 
 * pm_qtc_port_vlan_tag_set
 *
 * @brief vlan tag set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm_qtc_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,const portmod_vlan_tag_t* vlan_tag)
{
    return SOC_E_NONE;
}


/*! 
 * pm_qtc_port_vlan_tag_get
 *
 * @brief vlan tag get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm_qtc_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return SOC_E_NONE;
}

/*! 
 * pm_qtc_port_duplex_set
 *
 * @brief duplex set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        - 
 */
int pm_qtc_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return SOC_E_NONE;
}


/*! 
 * pm_qtc_port_duplex_get
 *
 * @brief duplex get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm_qtc_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    return SOC_E_NONE;
}

/*!
 * pm_qtc_port_speed_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  speed         -
 */
int pm_qtc_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    return SOC_E_NONE;
}

int pm_qtc_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32* value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32 value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_reset_set (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int direction)
{
    return SOC_E_NONE;
}

int pm_qtc_port_reset_get (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int* direction)
{
    return SOC_E_NONE;
}

/*Port remote Adv get*/
int pm_qtc_port_adv_remote_get (int unit, int port, pm_info_t pm_info, 
                                int* ability_mask)
{
    return (0);
}

/*get port auto negotiation local ability*/
int pm_qtc_port_ability_advert_get(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;
    phymod_autoneg_ability_t    an_ability;
    portmod_port_ability_t port_ability;
    int phy_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_get(unit, port, phy_access, nof_phys,
                                                   &an_ability, &port_ability));

    phy_index = nof_phys -1;
    if (phy_index && PORTMOD_IS_LEGACY_PHY_GET(&(phy_access[phy_index].access))) {
        sal_memcpy(ability, &port_ability, sizeof(portmod_port_ability_t));
    } else {
        portmod_common_phy_to_port_ability(&an_ability, ability);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_port_ability_advert_set(int unit, int port, pm_info_t pm_info, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0, port_index;
    uint32 bitmap;
    portmod_port_ability_t      port_ability;
    phymod_autoneg_ability_t    an_ability;
    portmod_access_get_params_t params;
    pm_qtc_port_t        *pInfo = NULL; 

    int port_num_lanes, line_interface, cx4_10g;
    int an_cl72, an_fec, hg_mode;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm_qtc_port_index_get (unit, port, pm_info, &port_index, 
                                             &bitmap));
    pInfo = &(PM_QTC_INFO(pm_info)->port_config[port_index]);


    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT
        (pm_qtc_port_ability_local_get(unit, port, pm_info, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.pause             &= ability->pause;
    port_ability.interface         &= ability->interface;
    port_ability.medium            &= ability->medium;
    port_ability.eee               &= ability->eee;
    port_ability.loopback          &= ability->loopback;
    port_ability.flags             &= ability->flags;
    port_ability.speed_half_duplex &= ability->speed_half_duplex;
    port_ability.speed_full_duplex &= ability->speed_full_duplex;

    port_num_lanes = pInfo->interface_config.port_num_lanes;
    line_interface = pInfo->interface_config.interface;
    cx4_10g        = pInfo->port_init_config.cx4_10g;
    an_cl72        = pInfo->port_init_config.an_cl72;
    an_fec         = pInfo->port_init_config.an_fec;
    hg_mode        = PHYMOD_INTF_MODES_HIGIG_GET(&pInfo->interface_config); 

    portmod_common_port_to_phy_ability(&port_ability, &an_ability,
                                       port_num_lanes, line_interface, cx4_10g,
                                       an_cl72, an_fec, hg_mode);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_set(unit, port, phy_access, nof_phys,
                                                   &an_ability, &port_ability));

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x orig(HD=0x%08x, FD=0x%08x) \n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n"),
                     port_ability.speed_half_duplex,
                     port_ability.speed_full_duplex,
                     port_ability.pause,
                     ability->speed_half_duplex,
                     ability->speed_full_duplex,
                     port_ability.interface,
                     port_ability.medium, port_ability.loopback,
                     port_ability.flags));
exit:
    SOC_FUNC_RETURN;
}

/*Port ability remote Adv get*/
int pm_qtc_port_ability_remote_get(int unit, int port, pm_info_t pm_info, 
                                       portmod_port_ability_t* ability)
{
    return SOC_E_NONE;
}

int pm_qtc_port_rx_control_set (int unit, int port, pm_info_t pm_info,
                                 const portmod_rx_control_t* rx_ctrl)
{
    return SOC_E_NONE;
}

int pm_qtc_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return SOC_E_NONE;
}


int pm_qtc_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return SOC_E_NONE;
}

int pm_qtc_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return SOC_E_NONE;
}


int pm_qtc_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return SOC_E_NONE;
}

int pm_qtc_port_tx_average_ipg_set (int unit, int port, 
                                    pm_info_t pm_info, int value)
{
    return SOC_E_NONE;
}


int pm_qtc_port_tx_average_ipg_get (int unit, int port, 
                                    pm_info_t pm_info, int* value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_update (int unit, int port, pm_info_t pm_info,
                        const portmod_port_update_control_t* update_control)
{
    return SOC_E_NONE;
}

int pm_qtc_port_drv_name_get (int unit, int port, pm_info_t pm_info, 
                              char* buf, int len)
{
    return SOC_E_NONE;
}

int pm_qtc_port_clear_rx_lss_status_set (int unit, soc_port_t port, 
                           pm_info_t pm_info, int lcl_fault, int rmt_fault)
{
    return SOC_E_NONE;
}

int pm_qtc_port_clear_rx_lss_status_get (int unit, soc_port_t port, 
                           pm_info_t pm_info, int *lcl_fault, int *rmt_fault)
{
    return SOC_E_NONE;
}

int pm_qtc_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    return SOC_E_NONE;
}

int pm_qtc_port_lag_failover_loopback_set (int unit, soc_port_t port, 
                                        pm_info_t pm_info, int value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_lag_failover_loopback_get (int unit, soc_port_t port, 
                                        pm_info_t pm_info, int *value)
{
    return SOC_E_NONE;
}

int pm_qtc_port_mode_set(int unit, soc_port_t port,
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    return SOC_E_NONE;
}

int pm_qtc_port_mode_get(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    return SOC_E_NONE;
}

int
pm_qtc_port_trunk_hwfailover_config_set(int unit, soc_port_t port, pm_info_t pm_info, int hw_count)
{
    return SOC_E_NONE;
}

int
pm_qtc_port_trunk_hwfailover_config_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *enable)
{
    return SOC_E_NONE;
}


int
pm_qtc_port_trunk_hwfailover_status_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *loopback)
{
    return SOC_E_NONE;
}

int pm_qtc_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    return SOC_E_NONE;
}

int
pm_qtc_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    return SOC_E_NONE;
}

int pm_qtc_port_lag_remove_failover_lpbk_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return SOC_E_NONE;
}

int pm_qtc_port_lag_remove_failover_lpbk_set(int unit, int port, pm_info_t pm_info, int val)
{
    return SOC_E_NONE;
}

int pm_qtc_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    return SOC_E_NONE;
}

int pm_qtc_port_mac_ctrl_set(int unit, int port, 
                   pm_info_t pm_info, uint64 ctrl)
{
    return SOC_E_NONE;
}

int pm_qtc_port_drain_cell_get(int unit, int port, 
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return SOC_E_NONE;
}

int pm_qtc_port_drain_cell_stop (int unit, int port, 
           pm_info_t pm_info, const portmod_drain_cells_t *drain_cells)
{
    return (SOC_E_NONE);
}

int pm_qtc_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    return SOC_E_NONE;
}

int pm_qtc_port_txfifo_cell_cnt_get(int unit, int port,
                       pm_info_t pm_info, uint32* fval)
{
/*          return(unimac_txfifo_cell_cnt_get(unit, port, fval));  */
         *fval =0;
          return SOC_E_NONE; 
}

int pm_qtc_port_egress_queue_drain_get(int unit, int port,
                   pm_info_t pm_info, uint64 *ctrl, int *rx)
{
    return SOC_E_NONE;
}

int pm_qtc_port_drain_cells_rx_enable (int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return SOC_E_NONE;
}

int pm_qtc_port_egress_queue_drain_rx_en(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return SOC_E_NONE;
}

int pm_qtc_port_mac_reset_set(int unit, int port,
                   pm_info_t pm_info, int val)
{
    return SOC_E_NONE;
}

int pm_qtc_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return SOC_E_NONE;
}

int pm_qtc_port_mac_reset_check(int unit, int port, pm_info_t pm_info,
                                int enable, int* reset)
{
    return SOC_E_NONE;
}

int pm_qtc_port_core_num_get(int unit, int port, pm_info_t pm_info,
                                int* core_num)
{
    return SOC_E_NONE;
}

int pm_qtc_port_e2ecc_hdr_set (int unit, int port, pm_info_t pm_info, 
                               const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return SOC_E_NONE;
}


int pm_qtc_port_e2ecc_hdr_get (int unit, int port, pm_info_t pm_info, 
                               portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (SOC_E_NONE);
}

int pm_qtc_port_e2e_enable_set (int unit, int port, pm_info_t pm_info, int enable) 
{
    return (SOC_E_NONE);
}


int pm_qtc_port_e2e_enable_get (int unit, int port, pm_info_t pm_info, int *enable)
{
    return (SOC_E_NONE);
}

int pm_qtc_port_fallback_lane_get(int unit, int port, pm_info_t pm_info, int* fallback_lane)
{
    return (SOC_E_NONE);
}

STATIC
int pm_qtc_port_soft_reset(int unit, int port, pm_info_t pm_info, int in_out)
{
    return (SOC_E_NONE);
}


/*Port discard set*/
int pm_qtc_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    return (SOC_E_NONE);
    /* return(unimac_discard_set(unit, port, discard)); */
}

int pm_qtc_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                          portmod_egr_1588_timestamp_config_t timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    if (timestamp_config.timestamp_mode == portmodTimestampMode32bit) {
        timestamp_mode = PM_QTC_EGR_1588_TIMESTAMP_MODE_32BIT;
    } else if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
        timestamp_mode = PM_QTC_EGR_1588_TIMESTAMP_MODE_48BIT;
    } else {
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val,
                      EGR_1588_TIMESTAMPING_MODEf, timestamp_mode);
    _SOC_IF_ERR_EXIT(WRITE_GPORT_MODE_REGr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm_qtc_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                          portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM_QTC_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, phy_acc, &reg_val));
    timestamp_mode = soc_reg_field_get(unit, GPORT_MODE_REGr, reg_val,
                                       EGR_1588_TIMESTAMPING_MODEf);

    if (timestamp_mode == PM_QTC_EGR_1588_TIMESTAMP_MODE_32BIT) {
        timestamp_config->timestamp_mode = portmodTimestampMode32bit;
    } else {
        timestamp_config->timestamp_mode = portmodTimestampMode48bit;
    }

    timestamp_config->cmic_48_overr_en = 0;

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM_QTC_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
