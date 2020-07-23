
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/pm4x10.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_system.h>

        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM12X10_SUPPORT

static int pm12x10_core_seq[] = {0,2,1};
static int pm12x10_invert_core_seq[] = {0,2,1};  /* index by core id, output for loop index */

#define PM_12x10_INFO(pm_info) ((pm_info)->pm_data.pm12x10_db)
#define PM_4X25_INFO(pm_info) (PM_12x10_INFO(pm_info)->pm4x25_info)
#define PM_4X10_INFO(pm_info, i) (PM_12x10_INFO(pm_info)->pm4x10_info[pm12x10_core_seq[i]])
#define PM_4X25_DRIVER(pm_info) PM_DRIVER(PM_4X25_INFO(pm_info))
#define PM_4X10_DRIVER(pm_info, i) PM_DRIVER(PM_4X10_INFO(pm_info, pm12x10_core_seq[i]))


#define TOP_BLK_ID_OFFSET (4)

#define PM12x10_INTERNAL_4x10_CNT (3)

struct pm12x10_s{
    int blk_id;
    pm_info_t pm4x25_info;
    pm_info_t pm4x10_info[PM12x10_INTERNAL_4x10_CNT];
    portmod_pbmp_t phys;
    uint32 external_top_mode;
    int    refclk_source;
    portmod_link_recovery_t link_recovery;
};

STATIC
int
pm12x10_quad0_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXX_CXXPORT_WC_UCMEM_DATA_0m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm12x10_quad0_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXX_CXXPORT_WC_UCMEM_DATA_0m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_quad0_default_bus = {
    "PM12x10 Quad0 Bus",
    pm12x10_quad0_default_bus_read,
    pm12x10_quad0_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int
pm12x10_quad1_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXX_CXXPORT_WC_UCMEM_DATA_1m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm12x10_quad1_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXX_CXXPORT_WC_UCMEM_DATA_1m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_quad1_default_bus = {
    "PM12x10 Quad1 Bus",
    pm12x10_quad1_default_bus_read,
    pm12x10_quad1_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int
pm12x10_quad2_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXX_CXXPORT_WC_UCMEM_DATA_2m, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm12x10_quad2_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXX_CXXPORT_WC_UCMEM_DATA_2m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_quad2_default_bus = {
    "PM12x10 Quad2 Bus",
    pm12x10_quad2_default_bus_read,
    pm12x10_quad2_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC portmod_ucode_buf_t pm12x10_ucode_buf[SOC_MAX_NUM_DEVICES]  = {{NULL, 0}};

STATIC int
pm4x10_quad0_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    int rv;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              portmod_ucode_buf_order_reversed,
                              0,
                              &(pm12x10_ucode_buf[unit]),
                              NULL,
                              CXX_CXXPORT_WC_UCMEM_DATA_0m,
                              CXX_CXXPORT_WC_UCMEM_CTRL_0r);
    _SOC_IF_ERR_EXIT(rv); 

exit:
    SOC_FUNC_RETURN;
}

STATIC int
pm4x10_quad1_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    int rv;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              portmod_ucode_buf_order_reversed,
                              0,
                              &(pm12x10_ucode_buf[unit]),
                              NULL,
                              CXX_CXXPORT_WC_UCMEM_DATA_1m,
                              CXX_CXXPORT_WC_UCMEM_CTRL_1r);
    _SOC_IF_ERR_EXIT(rv); 

exit:
    SOC_FUNC_RETURN;
}

STATIC int
pm4x10_quad2_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    int rv;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              portmod_ucode_buf_order_reversed,
                              0,
                              &(pm12x10_ucode_buf[unit]),
                              NULL,
                              CXX_CXXPORT_WC_UCMEM_DATA_2m,
                              CXX_CXXPORT_WC_UCMEM_CTRL_2r);
    _SOC_IF_ERR_EXIT(rv); 

exit:
    SOC_FUNC_RETURN;
}



int pm12x10_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    *is_supported = FALSE;

    if((interface == SOC_PORT_IF_CAUI) ||  (interface == SOC_PORT_IF_CR10) || (interface == SOC_PORT_IF_SR10)) {
       *is_supported = TRUE;
    }
    return SOC_E_NONE;
}

int pm12x10_pm_destroy(int unit, pm_info_t pm_info)
{        
    if(pm_info->pm_data.pm12x10_db != NULL){
        sal_free(pm_info->pm_data.pm12x10_db);
        pm_info->pm_data.pm12x10_db = NULL;
    }

    /* free pm12x10_ucode_buf */
    if(pm12x10_ucode_buf[unit].ucode_dma_buf != NULL){
        portmod_sys_dma_free(unit, pm12x10_ucode_buf[unit].ucode_dma_buf);
        pm12x10_ucode_buf[unit].ucode_dma_buf = NULL;
    }
    pm12x10_ucode_buf[unit].ucode_alloc_size = 0;

    return SOC_E_NONE;
}


int pm12x10_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, 
                    int wb_buffer_index, pm_info_t pm_info)
{
    pm12x10_t pm12x10_data = NULL;
    int nof_phys = 0, core;
    portmod_bus_update_t bupdate;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_COUNT(pm_add_info->phys, nof_phys);
    if(nof_phys != 12){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("number of phys should be 12")));
    }
    pm12x10_data = sal_alloc(sizeof(*(pm_info->pm_data.pm12x10_db)), "specific_db");
    SOC_NULL_CHECK(pm12x10_data);

    /*PM12x10 Top*/
    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    pm_info->pm_data.pm12x10_db = pm12x10_data;
    pm_info->pm_data.pm12x10_db->blk_id =  pm_add_info->pm_specific_info.pm12x10.blk_id; 
    PORTMOD_PBMP_ASSIGN(pm_info->pm_data.pm12x10_db->phys, pm_add_info->phys);

    for (core = 0; core < PM12x10_INTERNAL_4x10_CNT; core++) {
        pm_info->pm_data.pm12x10_db->pm4x10_info[core] = 
                pm_add_info->pm_specific_info.pm12x10.pm4x10[core];
    }

    pm_info->pm_data.pm12x10_db->link_recovery.enabled = TRUE;
    pm_info->pm_data.pm12x10_db->link_recovery.tick_cnt = 0;
    pm_info->pm_data.pm12x10_db->link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK;
    pm_info->pm_data.pm12x10_db->pm4x25_info = pm_add_info->pm_specific_info.pm12x10.pm4x25;
    pm_info->pm_data.pm12x10_db->refclk_source = pm_add_info->pm_specific_info.pm12x10.refclk_source;
    pm_info->pm_data.pm12x10_db->external_top_mode = PORTMOD_PM12x10_F_EXTERNAL_TOP_MODE_GET(pm_add_info->pm_specific_info.pm12x10.flags);

    if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
        /* Update Quad 0 */
        bupdate.blk_id = pm_info->pm_data.pm12x10_db->blk_id;
        bupdate.default_bus = &pm4x10_quad0_default_bus;
        bupdate.external_fw_loader = pm4x10_quad0_default_fw_loader;
        bupdate.user_acc = NULL; /*use default user_acc */
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_12x10_INFO(pm_info)->pm4x10_info[0], &bupdate));

        /* Update Quad 1 */
        bupdate.default_bus = &pm4x10_quad1_default_bus;
        bupdate.external_fw_loader = pm4x10_quad1_default_fw_loader;
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_12x10_INFO(pm_info)->pm4x10_info[1], &bupdate));

        /* Update Quad 2 */
        bupdate.default_bus = &pm4x10_quad2_default_bus;
        bupdate.external_fw_loader = pm4x10_quad2_default_fw_loader;
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_12x10_INFO(pm_info)->pm4x10_info[2], &bupdate));

    }

exit:
    if(SOC_FUNC_ERROR){
        pm12x10_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN; 
}

int pm12x10_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    /*no need*/
    return SOC_E_NONE;
}


/*int _pm12x10_lane_map_validate(int unit, int port, )*/


int pm12x10_port_attach (int unit, int port, pm_info_t pm_info, 
                         const portmod_port_add_info_t* add_info)
{
    portmod_port_add_info_t  lcl_add_info = *add_info;
    uint32 nof_lanes = 0;
    int i = 0;
    int rv = 0;
    int core_num=0;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_COUNT(add_info->phys, nof_lanes);

    if (nof_lanes == 10) {
        
    } else if(nof_lanes != 12){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("number of lanes should be 10 or 12")));
    }

    lcl_add_info.interface_config.flags |= PHYMOD_INTF_F_SET_CORE_MAP_MODE;

    for(i = 0 ; i < 3 ; i++)
    {
        /* do it before pm is active only - so skip in the second pass of multi stage init */
        if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || (lcl_add_info.flags == 0)) { 
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_pm_bypass_set(unit, PM_4X10_INFO(pm_info,i), 1);
            _SOC_IF_ERR_EXIT(rv);
        }
       
        /* If TSC12 first core is odd number, the mode between 244 and 442 need to be flip. */
         
        rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_core_num_get(unit, port,
                                             PM_4X10_INFO(pm_info,0), &core_num);
        _SOC_IF_ERR_EXIT(rv);

        if(core_num%2) {
            lcl_add_info.interface_config.flags |= PHYMOD_INTF_F_CORE_MAP_MODE_FLIP;
        }
        if (i) {
            lcl_add_info.flags |= PORTMOD_PORT_ADD_F_PORT_ATTACH_EXT_PHY_SKIP;
        }

        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_attach(unit, port, PM_4X10_INFO(pm_info,i), &lcl_add_info);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || (add_info->flags == 0)) {


        for(i = 0 ; i < 3 ; i++)
        {
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_interface_config_set(unit, 
                                          port, PM_4X10_INFO(pm_info,i), 
                                          &add_info->interface_config, 
                                          PORTMOD_INIT_F_ALL_PHYS);
            _SOC_IF_ERR_EXIT(rv);
        }

         if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
            rv = WRITE_CXX_CXXPORT_MODE_REGr(unit, port, 1); 
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    /* CPORT is not connected to any PHY.., so clear the phy information before adding it */
    lcl_add_info = *add_info;
    PORTMOD_PBMP_CLEAR(lcl_add_info.phys);
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_attach(unit, port, PM_4X25_INFO(pm_info), &lcl_add_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_detach(int unit, int port, pm_info_t pm_info)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    for(i = 0 ; i < 3; i++){
        _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, i)->f_portmod_port_detach(unit,
                                   port, PM_4X10_INFO(pm_info, i)));
    }

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_detach(unit, port, PM_4X25_INFO(pm_info));
    _SOC_IF_ERR_EXIT(rv);

    
    if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
        rv = WRITE_CXX_CXXPORT_MODE_REGr(unit, port, 0); 
        _SOC_IF_ERR_EXIT(rv);
    }

    for(i = 0 ; i < 3 ; i++)
    {
        PM_4X10_DRIVER(pm_info, i)->f_portmod_pm_bypass_set(unit, PM_4X10_INFO(pm_info,i), 0);
    }
    
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_replace(unit, port, PM_4X25_INFO(pm_info), new_port);
    _SOC_IF_ERR_EXIT(rv);

    for(i = 0 ; i < 3 ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_replace(unit, port, PM_4X10_INFO(pm_info,i), new_port);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN; 
    
}

int pm12x10_port_enable_set (int unit, int port, pm_info_t pm_info, 
                             int flags, int enable)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    if(enable) {
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_enable_set(unit, port, 
                                          PM_4X25_INFO(pm_info), flags, 1);
        _SOC_IF_ERR_EXIT(rv);
        for(i = 0 ; i < 3; i++) {
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_enable_set(unit, 
                                   port, PM_4X10_INFO(pm_info, i), flags, 2);
            _SOC_IF_ERR_EXIT(rv);
        }
    } else {
        int flags_temp = flags;

        if (flags) {  /* make sure MAC is also disabled (in case it was enabled) */
            PORTMOD_PORT_ENABLE_MAC_SET(flags_temp);
        }

        for(i = 0 ; i < 3; i++){
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_enable_set(unit, 
                                   port, PM_4X10_INFO(pm_info, i), flags_temp, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_enable_set(unit, port, 
                                          PM_4X25_INFO(pm_info), flags, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_enable_get (int unit, int port, pm_info_t pm_info, 
                             int flags, int* enable)
{
    int rv = 0;    
    SOC_INIT_FUNC_DEFS;

    *enable = 0;
    if (PORTMOD_PORT_ENABLE_PHY_GET(flags)) {
        rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_enable_get(unit, 
                         port, PM_4X10_INFO(pm_info, 2), flags, enable);
    } else {
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_enable_get(unit, 
                            port, PM_4X25_INFO(pm_info), flags, enable);
    }
    _SOC_IF_ERR_EXIT(rv);
       
exit:
    SOC_FUNC_RETURN;    
}

STATIC
int _pm12x10_internal_if_cfg_set (int unit, int port, pm_info_t pm_info, 
                               const portmod_port_interface_config_t* config,
                               int phy_init_flags)
{
    int i =0, rv=0;

    for(i = 0 ; !rv && (i < 3) ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_interface_config_set(
                         unit, port, PM_4X10_INFO(pm_info, i), config, 
                         phy_init_flags);
    }
    return (rv);
}


/**
 *      if_cfg->flags |= PHYMOD_INTF_F_SET_SPD_DISABLE;
 *      if_cfg->flags ^= PHYMOD_INTF_F_SET_SPD_DISABLE;
 *
 *      if_cfg->flags |= PHYMOD_INTF_F_SET_CORE_MAP_MODE;
 *      if_cfg->flags ^= PHYMOD_INTF_F_SET_CORE_MAP_MODE;
 *      if(core_num%2) {
 *          my_config->flags |= PHYMOD_INTF_F_CORE_MAP_MODE_FLIP;
 *      }
 *
 *      if_cfg->flags |= PHYMOD_INTF_F_SET_SPD_NO_TRIGGER ;
 *      if_cfg->flags ^= PHYMOD_INTF_F_SET_SPD_NO_TRIGGER ;
 *
 *      if_cfg->flags |= PHYMOD_INTF_F_SET_SPD_TRIGGER;
 *      if_cfg->flags ^= PHYMOD_INTF_F_SET_SPD_TRIGGER;
 */
int pm12x10_port_interface_config_set (int unit, int port, pm_info_t pm_info, 
                               const portmod_port_interface_config_t* config,
                               int phy_init_flags)
{
    int rv = 0;
    portmod_port_interface_config_t* my_config = (portmod_port_interface_config_t*)config;
    int core_num;
    SOC_INIT_FUNC_DEFS;

    if (config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        return (_pm12x10_internal_if_cfg_set (unit, port, pm_info, my_config, phy_init_flags));
    }

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_interface_config_set (unit, 
                                  port, PM_4X25_INFO(pm_info), config, phy_init_flags);
    _SOC_IF_ERR_EXIT(rv);


    /* speed disable */
    my_config->flags |= PHYMOD_INTF_F_SET_SPD_DISABLE;
    rv = _pm12x10_internal_if_cfg_set (unit, port, pm_info, my_config, 
                                       PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
    _SOC_IF_ERR_EXIT(rv);

    /* core map mode set */
    my_config->flags ^= PHYMOD_INTF_F_SET_SPD_DISABLE;
    my_config->flags |= PHYMOD_INTF_F_SET_CORE_MAP_MODE;

    rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_core_num_get(unit, port,
                                     PM_4X10_INFO(pm_info,0), &core_num);
    _SOC_IF_ERR_EXIT(rv);

    if(core_num%2) {
        my_config->flags |= PHYMOD_INTF_F_CORE_MAP_MODE_FLIP;
    }

    rv = _pm12x10_internal_if_cfg_set (unit, port, pm_info, my_config, 
                                       PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
    _SOC_IF_ERR_EXIT(rv);

    if (my_config->flags & PHYMOD_INTF_F_CORE_MAP_MODE_FLIP)
        my_config->flags ^= PHYMOD_INTF_F_CORE_MAP_MODE_FLIP;

    /* Speed set - no trigger */
    my_config->flags ^= PHYMOD_INTF_F_SET_CORE_MAP_MODE;
    my_config->flags |= PHYMOD_INTF_F_SET_SPD_NO_TRIGGER ;

    rv = _pm12x10_internal_if_cfg_set (unit, port, pm_info, my_config, 
                                       PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
    _SOC_IF_ERR_EXIT(rv);

    /* Speed trigger */
    my_config->flags ^= PHYMOD_INTF_F_SET_SPD_NO_TRIGGER ;
    my_config->flags |= PHYMOD_INTF_F_SET_SPD_TRIGGER;

    rv = _pm12x10_internal_if_cfg_set (unit, port, pm_info, my_config, phy_init_flags);
    _SOC_IF_ERR_EXIT(rv);

    my_config->flags ^= PHYMOD_INTF_F_SET_SPD_TRIGGER;

exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_interface_config_get (int unit, int port, pm_info_t pm_info, 
                                       portmod_port_interface_config_t* config, int phy_init_flags)
{
    int rv = 0, flags;
    SOC_INIT_FUNC_DEFS;

    /* Get Phy information from  pm4x10 (Phy) */
    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_interface_config_get(
                            unit, port, PM_4X10_INFO(pm_info, 2), config, phy_init_flags);
    _SOC_IF_ERR_EXIT(rv);

    /* Get mac information from pm4x25 (CLMAC) */
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_encap_get( unit, port,
                            PM_4X25_INFO(pm_info), &flags, &config->encap_mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_default_interface_get(int unit, int port, pm_info_t pm_info,
                                       const portmod_port_interface_config_t* config,
                                       soc_port_if_t* interface)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_default_interface_get
                          (unit, port, PM_4X10_INFO(pm_info, 2), config, interface));

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_loopback_set(int unit, int port, pm_info_t pm_info, 
                  portmod_loopback_mode_t loopback_type, int enable)
{
   int rv = 0, i;
   portmod_port_interface_config_t config;
   SOC_INIT_FUNC_DEFS;

   switch(loopback_type) {
    case portmodLoopbackMacOuter:
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_loopback_set (unit, port, 
                           PM_4X25_INFO(pm_info), loopback_type, enable);
        _SOC_IF_ERR_EXIT(rv);
        break;

    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPCS: /*slide*/
         for (i = 0; i < 3; i++) {
              rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_loopback_set(
                         unit, port, PM_4X10_INFO(pm_info, i), loopback_type, enable);
              _SOC_IF_ERR_EXIT(rv);
         }
        /* add the speed trigger */
        /* Get Phy information from  pm4x10 (Phy) */
        rv = portmod_port_interface_config_t_init(unit, &config);
        _SOC_IF_ERR_EXIT(rv);

        rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_interface_config_get(
                                unit, port, PM_4X10_INFO(pm_info, 2), &config, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
        _SOC_IF_ERR_EXIT(rv);

        /* Set SPD_ID, intf and lanes on all pm4x10's without triggering speed change */
        config.flags = PHYMOD_INTF_F_SET_SPD_NO_TRIGGER ;
        rv = _pm12x10_internal_if_cfg_set (unit, port, pm_info, &config, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
        _SOC_IF_ERR_EXIT(rv);

        /*  Now trigger the speed change (after all pm4x10's are configured correctly) */
        config.flags = PHYMOD_INTF_F_SET_SPD_TRIGGER;
        rv = _pm12x10_internal_if_cfg_set (unit, port, pm_info, &config, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
        _SOC_IF_ERR_EXIT(rv);

         break;
    case portmodLoopbackPhyRloopPCS:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Rloop PCS is not supported.")));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, 
              (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }

    if (enable)
        _SOC_IF_ERR_EXIT(PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, PM_4X25_INFO(pm_info)));

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_loopback_get(int unit, int port, pm_info_t pm_info, 
                  portmod_loopback_mode_t loopback_type, int* enable)
{
   int rv = 0;
   SOC_INIT_FUNC_DEFS;

   switch(loopback_type) {
    case portmodLoopbackMacOuter:
        rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_loopback_get(unit, port, 
                                PM_4X25_INFO(pm_info), loopback_type, enable);
        _SOC_IF_ERR_EXIT(rv);
        break;

    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPCS: /*slide*/
        rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_loopback_get(unit, port,
                                PM_4X10_INFO(pm_info, 2), loopback_type, enable);
        _SOC_IF_ERR_EXIT(rv);
        break;
    case portmodLoopbackPhyRloopPCS:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Rloop PCS is not supported.")));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
            _SOC_MSG("unsupported loopback type %d"), loopback_type));
    }   

exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_ability_local_get (int unit, int port, pm_info_t pm_info, int phy_init_flags,
                                    portmod_port_ability_t* ability)
{    
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
/* CHECK IF WE SHOULD USE 4X25 or 4X10
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_ability_local_get(unit, port, 
                                 PM_4X25_INFO(pm_info), ability); */

    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_ability_local_get (
             unit, port, PM_4X10_INFO(pm_info,2), phy_init_flags, ability);
    _SOC_IF_ERR_EXIT(rv);

exit:    
    SOC_FUNC_RETURN;  
}

int pm12x10_port_autoneg_set(int unit, int port, pm_info_t pm_info, int phy_init_flags,
                              const phymod_autoneg_control_t* an)
{
    int rv = 0;
    int i = 0, port_fallback_lane, core_seq;

    SOC_INIT_FUNC_DEFS;
    if (an->enable) {
        ((phymod_autoneg_control_t*)an)->num_lane_adv = 10 ;
        ((phymod_autoneg_control_t*)an)->flags |= PHYMOD_AN_F_SET_PRIOR_ENABLE;
        for ( i = 0 ; i < 3 ; i++)
        {
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_autoneg_set(
                           unit, port, PM_4X10_INFO(pm_info, i), phy_init_flags, an);
            _SOC_IF_ERR_EXIT(rv);
        }
    ((phymod_autoneg_control_t*)an)->flags &= ~PHYMOD_AN_F_SET_PRIOR_ENABLE;
    }

    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_fallback_lane_get(
                           unit, port, PM_4X10_INFO(pm_info, 2), &port_fallback_lane);
    _SOC_IF_ERR_EXIT(rv);

    if(port_fallback_lane>2||port_fallback_lane<0) { 
      _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("port_fallback_lane runaway")));
    }
    core_seq = pm12x10_invert_core_seq[port_fallback_lane] ;

    rv = PM_4X10_DRIVER(pm_info, core_seq)->f_portmod_port_autoneg_set(
                           unit, port, PM_4X10_INFO(pm_info, core_seq), phy_init_flags, an);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_autoneg_get (int unit, int port, pm_info_t pm_info, int phy_init_flags,
                              phymod_autoneg_control_t* an)
{
    int port_fallback_lane, core_seq;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_fallback_lane_get(
                           unit, port, PM_4X10_INFO(pm_info, 2), &port_fallback_lane));
    
    if(port_fallback_lane>2||port_fallback_lane<0) { 
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("port_fallback_lane runaway")));
    }
    core_seq = pm12x10_invert_core_seq[port_fallback_lane] ;

    _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, core_seq)->f_portmod_port_autoneg_get(
                unit, port, PM_4X10_INFO(pm_info, core_seq), phy_init_flags, an));

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_autoneg_status_get (int unit, int port, pm_info_t pm_info, 
                              phymod_autoneg_status_t* an)
{
    return (PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_autoneg_status_get(
                unit, port, PM_4X10_INFO(pm_info, 2), an));
}

int pm12x10_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{     
    int all_link= 1;
    int tmp_link;
    int i;
    int link_recovery_enable = 0;
    portmod_port_interface_config_t  config;
    portmod_access_get_params_t params;
    phymod_phy_access_t         phy_access;
    int tmp_nof_phys = 0;
    int is_most_ext_i = 0;
    int rv;
    int phy_init_flags = 0;
    uint32 signal_detect = 0;
    uint32 all_signal_detect = 1;

    SOC_INIT_FUNC_DEFS;


    /* first check if 12 lane port */
    _SOC_IF_ERR_EXIT(
        pm12x10_port_interface_config_get(unit, port, pm_info, &config, phy_init_flags));

    if (config.speed >= 120000) {
        link_recovery_enable = 1;
    }

/* for now not enable the 100G Sr10 link recovery */
#if 0
    /* next need to check if 100G SR10, if so, the link recovery needs to be enabled */
    if (config.speed == 100000) {
        /*first get the master core phy access */
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_phy_lane_access_get(
                                       unit, port, PM_4X10_INFO(pm_info, 2),
                                       &params, 1 , &phy_access, &tmp_nof_phys, &is_most_ext_i);

        _SOC_IF_ERR_EXIT(phymod_phy_firmware_lane_config_get(&phy_access, &firmware_lane_config));
        if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
            link_recovery_enable = 1;
        }
    }
#endif

    for ( i = 0 ; i < 3 ; i++)
    {
        _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, i)->f_portmod_port_link_get( unit, port, 
                                         PM_4X10_INFO(pm_info, i), flags, &tmp_link));
        /* accumulate link status of all 3 cores. */
        all_link &= tmp_link; 
    }

    /* next we need check if link recovery needs to be enabled */
    if ((link_recovery_enable) && (PM_12x10_INFO(pm_info)->link_recovery.enabled)) {

        switch (PM_12x10_INFO(pm_info)->link_recovery.state) {

        case PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK:
            /* link is good */
            if (all_link) {
                PM_12x10_INFO(pm_info)->link_recovery.tick_cnt = 0;
                PM_12x10_INFO(pm_info)->link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_LINK_GOOD;
            } else if ((PM_12x10_INFO(pm_info)->link_recovery.tick_cnt) > PORTMOD_LINK_RECOVERY_LINK_WAIT_CNT_LIMIT) {
                /* no link after all the try, reset the rx patha again */
                PM_12x10_INFO(pm_info)->link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_RESET_RX;
                PM_12x10_INFO(pm_info)->link_recovery.tick_cnt = 0;
            } else {
                /* no link yet, keep wait for link */
                PM_12x10_INFO(pm_info)->link_recovery.tick_cnt += 1;
            }
            break;

        case PORTMOD_LINK_RECOVERY_STATE_RESET_RX:
            /*first check link status */
            if (all_link) {
                PM_12x10_INFO(pm_info)->link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_LINK_GOOD;
                PM_12x10_INFO(pm_info)->link_recovery.tick_cnt = 0;
            } else {
                /* first reset the rx path */
                all_signal_detect = 1;
                signal_detect = 0;
                for ( i = 0 ; i < 3 ; i++) {
                    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
                    params.phyn = 0;
                    rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                                   unit, port, PM_4X10_INFO(pm_info, i),
                                                   &params, 1 , &phy_access, &tmp_nof_phys, &is_most_ext_i);
                    _SOC_IF_ERR_EXIT(rv);
                    /* check if there is signal detect */
                    _SOC_IF_ERR_EXIT(phymod_phy_rx_signal_detect_get(&phy_access, &signal_detect));
                    all_signal_detect &= signal_detect;
                    signal_detect = 0;
                }
                /* only toggle rx reset path only if there is signal on the line */
                if (all_signal_detect) {
                         /* first put the rx into reset */
                    for ( i = 0 ; i < 3 ; i++) {
                        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
                        params.phyn = 0;
                        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                                       unit, port, PM_4X10_INFO(pm_info, i),
                                                       &params, 1 , &phy_access, &tmp_nof_phys, &is_most_ext_i);
                        _SOC_IF_ERR_EXIT(rv);

                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, phymodRxDisable));
                    }
                    /* add some delay */
                    sal_usleep(5000);
                    for ( i = 0 ; i < 3 ; i++) {
                        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
                        params.phyn = 0;
                        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                                       unit, port, PM_4X10_INFO(pm_info, i),
                                                       &params, 1 , &phy_access, &tmp_nof_phys, &is_most_ext_i);
                        _SOC_IF_ERR_EXIT(rv);

                         /* toggle CDR lock */
                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, phymodRxSquelchOn));
                    }
                    /* add some delay */
                    sal_usleep(5000);
                    for ( i = 0 ; i < 3 ; i++) {
                        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
                        params.phyn = 0;
                        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                                       unit, port, PM_4X10_INFO(pm_info, i),
                                                       &params, 1 , &phy_access, &tmp_nof_phys, &is_most_ext_i);
                        _SOC_IF_ERR_EXIT(rv);

                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, phymodRxSquelchOff));
                    }
                    sal_usleep(10000);
                    /* release the rx reset */
                    for ( i = 0 ; i < 3 ; i++) {
                        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
                        params.phyn = 0;
                        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                                       unit, port, PM_4X10_INFO(pm_info, i),
                                                       &params, 1 , &phy_access, &tmp_nof_phys, &is_most_ext_i);
                        _SOC_IF_ERR_EXIT(rv);

                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, phymodRxReset));
                    }
                    sal_usleep(10000);

                }
                PM_12x10_INFO(pm_info)->link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK;
                PM_12x10_INFO(pm_info)->link_recovery.tick_cnt = 0;
            }
            break;

        case PORTMOD_LINK_RECOVERY_STATE_LINK_GOOD:
            /* no link, need to reset the rx path */
            if (!all_link) {
                PM_12x10_INFO(pm_info)->link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_RESET_RX;
                PM_12x10_INFO(pm_info)->link_recovery.tick_cnt = 0;
            }
            break;
        }
    }

    *link = all_link;
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    return SOC_E_NONE;
}

int pm12x10_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    return SOC_E_NONE;
}

int pm12x10_port_prbs_config_set (int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, 
                                  int flags, const phymod_prbs_t* config)
{
    int i;

    SOC_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        for(i = 0 ; i < 3 ; i++)
        {
            _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, i)->f_portmod_port_prbs_config_set(unit, port,PM_4X10_INFO(pm_info,i), mode, flags, config));
        }
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_config_get(int unit, int port, pm_info_t pm_info, 
                            portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_prbs_config_get(unit, port, PM_4X10_INFO(pm_info,0),mode, flags, config));
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_enable_set (int unit, int port, pm_info_t pm_info, 
                                  portmod_prbs_mode_t mode, int flags, int enable)
{
    int i;

    SOC_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        for(i = 0 ; i < 3 ; i++)
        {
            _SOC_IF_ERR_EXIT
                (PM_4X10_DRIVER(pm_info, i)->f_portmod_port_prbs_enable_set(unit, port,
                                                                            PM_4X10_INFO(pm_info,i), 
                                                                            mode, flags, enable));
        }
    }
    else{    
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    } 
exit:
    SOC_FUNC_RETURN;  
}

int pm12x10_port_prbs_enable_get (int unit, int port, pm_info_t pm_info, 
                                  portmod_prbs_mode_t mode, int flags, int* enable)
{

    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT
            (PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_prbs_enable_get(unit, port, 
                                                                        PM_4X10_INFO(pm_info,0),
                                                                        mode, flags, enable));
    }
    else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    }
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_prbs_status_get (int unit, int port, pm_info_t pm_info, 
                                  portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{

    SOC_INIT_FUNC_DEFS;
    if(mode == 0){ /*phy*/
        _SOC_IF_ERR_EXIT
            (PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_prbs_status_get(unit, port, 
                                                                        PM_4X10_INFO(pm_info,0),
                                                                        mode, flags, status));
    }
    else{
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported for PM12x10")));
    }
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_firmware_mode_set (int unit, int port, pm_info_t pm_info, 
                                    phymod_firmware_mode_t fw_mode)
{      
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_phy_firmware_mode_set(unit, port, fw_mode));

exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_firmware_mode_get (int unit, int port, pm_info_t pm_info, 
                                    phymod_firmware_mode_t* fw_mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_common_phy_firmware_mode_get(unit, port, fw_mode));

exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_runt_threshold_set(int unit, int port, pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_runt_threshold_set(unit, port, 
                                                  PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);    
       
exit:
    SOC_FUNC_RETURN; 
    
}

int pm12x10_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_runt_threshold_get(unit, port, 
                                                  PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       

exit:
    SOC_FUNC_RETURN;    
}


int pm12x10_port_max_packet_size_set (int unit, int port, 
                                      pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_max_packet_size_set(unit, 
                                       port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_max_packet_size_get (int unit, int port, 
                                      pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_max_packet_size_get(unit, 
                                       port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       

exit:
    SOC_FUNC_RETURN;    
}


int pm12x10_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pad_size_set(unit, port, 
                                            PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);    
       
exit:
    SOC_FUNC_RETURN;  
}


int pm12x10_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pad_size_get(unit, port, 
                                            PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       

exit:
    SOC_FUNC_RETURN;  
}


int pm12x10_port_local_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_local_fault_status_get(unit, port, 
                                                     PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_local_fault_control_set(unit, port, 
                                                     PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_local_fault_control_get(unit, port, 
                                                     PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_remote_fault_status_get(unit, port, 
                                                      PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);       

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_remote_fault_control_set(unit, port, 
                                                      PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_remote_fault_control_get(unit, port, 
                                                      PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_local_fault_status_clear(unit, port,
                                                      PM_4X25_INFO(pm_info));
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_remote_fault_status_clear(unit, port,
                                                      PM_4X25_INFO(pm_info));
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;

}

int pm12x10_port_pause_control_set (int unit, int port, pm_info_t pm_info, 
                                    const portmod_pause_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pause_control_set(unit, port, 
                                               PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);    
       
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_pause_control_get (int unit, int port, pm_info_t pm_info, 
                                    portmod_pause_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pause_control_get(unit, port, 
                                               PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);  
         
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_pfc_control_set (int unit, int port, pm_info_t pm_info, 
                                  const portmod_pfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pfc_control_set(unit, port, 
                                            PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);  
         
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_pfc_control_get (int unit, int port, pm_info_t pm_info, 
                                  portmod_pfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pfc_control_get(unit, port, 
                                             PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);  
         
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_port_llfc_control_set(int unit, int port, pm_info_t pm_info, 
                                  const portmod_llfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_llfc_control_set(unit, port, 
                                              PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);   
        
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_llfc_control_get(int unit, int port, pm_info_t pm_info, 
                                  portmod_llfc_control_t* control)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_llfc_control_get(unit, port, 
                                             PM_4X25_INFO(pm_info), control);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;  
}


int pm12x10_port_core_access_get(int unit, int port, pm_info_t pm_info, 
                                 int phyn, int max_cores, 
                                 phymod_core_access_t* core_access_arr, 
                                 int* nof_cores,
                                 int* is_most_ext)
{
    int i = 0;
    int rv = 0;
    int tmp_nof_cores;
    int is_most_ext_i;
    SOC_INIT_FUNC_DEFS;

    if(max_cores < 3){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
               (_SOC_MSG("3 core structures are required, %d given"), 
                 max_cores));
    }

    if (is_most_ext) {
        *is_most_ext = 0;
    }

    for( i = 0 ; i < 3 ; i++){
        rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_core_access_get(unit, 
                                      port, PM_4X10_INFO(pm_info, i), phyn, 
                                      1, &core_access_arr[i], &tmp_nof_cores, &is_most_ext_i);
        _SOC_IF_ERR_EXIT(rv);

        if (is_most_ext) {
            *is_most_ext = (*is_most_ext | is_most_ext_i ? 1 : 0);
        }
    }

    *nof_cores = 3;
    

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, 
                                     const portmod_access_get_params_t* params, 
                                     int max_phys, 
                                     phymod_phy_access_t* access, 
                                     int* nof_phys, int* is_most_ext)
{
    int i = 0;
    int rv = 0;
    int tmp_nof_phys;
    int is_most_ext_i;
    int lane_mask, consolidated_mask, first_set_bit;
    int prev_xphy_id, xphy_id;
    SOC_INIT_FUNC_DEFS;

    prev_xphy_id = PORTMOD_XPHY_ID_INVALID;
    xphy_id = PORTMOD_XPHY_ID_INVALID; 
    lane_mask = 0;
    consolidated_mask = 0;

    if(max_phys < 3){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
             (_SOC_MSG("3 phy access structures are required, %d given"), 
              max_phys));
    }

    if (is_most_ext) {
        *is_most_ext = 0;
    }

    *nof_phys = 0;
    for( i = 0 ; i < 3 ; i++){

        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_lane_mask_get ( 
                                         unit, port, /* PM_4X10_INFO(pm_info, i),  */
                                         PM_12x10_INFO(pm_info)->pm4x10_info[i],
                                         params, &lane_mask, &xphy_id );
        _SOC_IF_ERR_EXIT(rv);

        if (xphy_id != PORTMOD_XPHY_ID_INVALID) { 
            if (!consolidated_mask) { 
                consolidated_mask = lane_mask;
            }
            if ( prev_xphy_id == xphy_id ) {
                consolidated_mask |= lane_mask;
            }
            prev_xphy_id = xphy_id;
        } 


        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                       unit, port, /* PM_4X10_INFO(pm_info, i),  */
                                       PM_12x10_INFO(pm_info)->pm4x10_info[i],
                                       params, 1 , &access[*nof_phys], &tmp_nof_phys, &is_most_ext_i);

        _SOC_IF_ERR_EXIT(rv);

        if (is_most_ext) {
            *is_most_ext = (*is_most_ext | is_most_ext_i ? 1 : 0);
        }
        *nof_phys += tmp_nof_phys;
    }

    if ((params->lane != -1) && consolidated_mask) {
        for(first_set_bit=0; first_set_bit<12; first_set_bit++){
            if( (consolidated_mask>>first_set_bit) & 0x1) {
                break;
            }
        }
        if ((!((1 << (params->lane + first_set_bit)) & consolidated_mask))) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
                    (_SOC_MSG("Requested lane %d doesn't exist in lane_mask=0x%x\n")
                     ,params->lane, consolidated_mask ));
        }
        for(i = 0 ; i < *nof_phys ; i++){
            access[i].access.lane_mask = ((1 << (params->lane + first_set_bit)) & consolidated_mask); 
        }
    }
exit:
    SOC_FUNC_RETURN; 
    
}


/*!
 * pm12x10_port_duplex_set
 *
 * @brief duplex set.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        -
 */
int pm12x10_port_duplex_set(int unit, int port, pm_info_t pm_info, int enable)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_duplex_set(unit, port, 
                                             PM_4X25_INFO(pm_info), enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
}


/*!
 * pm12x10_port_duplex_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        -
 */
int pm12x10_port_duplex_get(int unit, int port, pm_info_t pm_info, int* duplex)
{
    *duplex=1;
    return (0);
}

int pm12x10_port_phy_reg_read (int unit, int port, pm_info_t pm_info, 
                           int lane, int flags, int reg_addr, uint32* value)
{
    int core_index, rv = 0;
    int core_id = 2;
    portmod_access_get_params_t params;
    phymod_phy_access_t         phy_access;
    int is_most_ext;
    int tmp_nof_phys;
    int mask = 0;

    SOC_INIT_FUNC_DEFS;
   
    if (lane >= 0) {  /* lane is specified */
        for (core_index = 0 ; core_index < 3 ; core_index++){  /* walk through 3 cores */
           _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
           if (flags & SOC_PHY_INTERNAL) {
               params.phyn = 0;
           }
           params.lane = lane;
           rv = PM_4X10_DRIVER(pm_info, core_index)->f_portmod_port_phy_lane_access_get(
                                          unit, port, PM_4X10_INFO(pm_info, core_index), 
                                          &params, 1, &phy_access, &tmp_nof_phys, &is_most_ext);
           _SOC_IF_ERR_EXIT(rv);
           if (tmp_nof_phys >= 1) {  /* found the right core */
               mask = phy_access.access.lane_mask;
               lane = 0;
               while (mask >0) {
                   if (mask & 1) {
                       break;
                   }
                   lane ++;
                   mask >>= 1 ;
               }
               rv = PM_4X10_DRIVER(pm_info, core_index)->f_portmod_port_phy_reg_read(unit, 
                      port, PM_4X10_INFO(pm_info, core_index), lane, flags, reg_addr, value);
               break;
           }
        }
    } else {
        rv = PM_4X10_DRIVER(pm_info, core_id)->f_portmod_port_phy_reg_read(unit, 
                      port, PM_4X10_INFO(pm_info, core_id), lane, flags, reg_addr, value);
    }
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_phy_reg_write (int unit, int port, pm_info_t pm_info, 
                           int lane, int flags, int reg_addr, uint32 value)
{
    int core_index = 0, rv = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t         phy_access;
    int is_most_ext;
    int tmp_nof_phys;
    int mask = 0;

    SOC_INIT_FUNC_DEFS;

    if (lane >= 0) {  /* lane is specified */
        for (core_index = 0 ; core_index < 3 ; core_index++){  /* walk through 3 cores */
            _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
            if (flags & SOC_PHY_INTERNAL) {
                params.phyn = 0;
            }
            params.lane = lane;
            rv = PM_4X10_DRIVER(pm_info, core_index)->f_portmod_port_phy_lane_access_get(
                                           unit, port, PM_4X10_INFO(pm_info, core_index), 
                                           &params, 1, &phy_access, &tmp_nof_phys, &is_most_ext);
            _SOC_IF_ERR_EXIT(rv);
            if (tmp_nof_phys >= 1) {  /* found the right core */
                mask = phy_access.access.lane_mask;
                lane = 0;
                while (mask >0) {
                    if (mask & 1) {
                        break;
                    }
                    lane ++; 
                    mask >>= 1 ;
                }
                rv = PM_4X10_DRIVER(pm_info, core_index)->f_portmod_port_phy_reg_write(unit, 
                                               port, PM_4X10_INFO(pm_info, core_index), lane, flags, reg_addr, value);
                _SOC_IF_ERR_EXIT(rv);
                break;
            }
        }
    } else {
        for (core_index = 0 ; core_index < 3 ; core_index++){
            rv = PM_4X10_DRIVER(pm_info, core_index)->f_portmod_port_phy_reg_write(unit, 
                          port, PM_4X10_INFO(pm_info, core_index), lane, flags, reg_addr, value);
            _SOC_IF_ERR_EXIT(rv);
        }
    }
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_reset_set (int unit, int port, pm_info_t pm_info, 
                           int mode, int opcode, int value)
{
    int rv = 0, i = 0;
    SOC_INIT_FUNC_DEFS;

    for( i = 0 ; i < 3 ; i++){
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_reset_set (unit, port, 
                               PM_4X10_INFO(pm_info, i), mode, opcode, value);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_reset_get (int unit, int port, pm_info_t pm_info, 
                           int mode, int opcode, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_reset_get (unit, port, 
                           PM_4X10_INFO(pm_info, 2), mode, opcode, value);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
}

/*Port remote Adv get*/
int pm12x10_port_adv_remote_get (int unit, int port, pm_info_t pm_info, 
                                int* value)
{
    return (0);
}

/*!
 * pm12x10_port_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int pm12x10_port_frame_spacing_stretch_set (int unit, int port,
                                           pm_info_t pm_info,int spacing)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
    
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_frame_spacing_stretch_set(unit,
                                       port, PM_4X25_INFO(pm_info), spacing);
    _SOC_IF_ERR_EXIT(rv);       
    
exit:
    SOC_FUNC_RETURN;

}

/*!
 * pm12x10_port_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int pm12x10_port_frame_spacing_stretch_get (int unit, int port,
                                            pm_info_t pm_info,
                                            const int *spacing)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
    
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_frame_spacing_stretch_get(unit,
                                       port, PM_4X25_INFO(pm_info), spacing);
    _SOC_IF_ERR_EXIT(rv);       
    
exit:
    SOC_FUNC_RETURN;
}


/*! 
 * pm12x10_port_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int pm12x10_port_diag_fifo_status_get (int unit, int port,pm_info_t pm_info, 
                                 const portmod_fifo_status_t* diag_info)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_diag_fifo_status_get(unit,
                                       port, PM_4X25_INFO(pm_info), diag_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

/*! 
 * pm12x10_port_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm12x10_port_pfc_config_set (int unit, int port,pm_info_t pm_info, 
                           const portmod_pfc_config_t* pfc_cfg)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pfc_config_set(unit,
                                       port, PM_4X25_INFO(pm_info), pfc_cfg);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}


/*! 
 * pm12x10_port_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm12x10_port_pfc_config_get (int unit, int port,pm_info_t pm_info, 
                                 portmod_pfc_config_t* pfc_cfg)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_pfc_config_get(unit,
                                       port, PM_4X25_INFO(pm_info), pfc_cfg);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

/*! 
 * pm12x10_port_eee_set
 *
 * @brief set EEE control and timers 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int pm12x10_port_eee_set(int unit, int port, pm_info_t pm_info,
                          const portmod_eee_t* eee)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_eee_set(unit,
                                       port, PM_4X25_INFO(pm_info), eee);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

/*! 
 * pm12x10_port_eee_get
 *
 * @brief get EEE control and timers 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int pm12x10_port_eee_get (int unit, int port, pm_info_t pm_info,
                          portmod_eee_t* eee)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_eee_get(unit,
                                       port, PM_4X25_INFO(pm_info), eee);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

/*! 
 * pm12x10_port_vlan_tag_set
 *
 * @brief vlan tag set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm12x10_port_vlan_tag_set (int unit, int port, pm_info_t pm_info,
                               const portmod_vlan_tag_t* vlan_tag)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_vlan_tag_set(unit,
                                       port, PM_4X25_INFO(pm_info), vlan_tag);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}


/*! 
 * pm12x10_port_vlan_tag_get
 *
 * @brief vlan tag get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm12x10_port_vlan_tag_get(int unit, int port, pm_info_t pm_info,
                              portmod_vlan_tag_t* vlan_tag)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_vlan_tag_get(unit,
                                       port, PM_4X25_INFO(pm_info), vlan_tag);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

int pm12x10_port_rx_control_set (int unit, int port, pm_info_t pm_info, 
                                 const portmod_rx_control_t* rx_ctrl)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_rx_control_set(unit,
                                       port, PM_4X25_INFO(pm_info), rx_ctrl);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_rx_control_get (int unit, int port, pm_info_t pm_info,
                                 portmod_rx_control_t* rx_ctrl)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_rx_control_get(unit,
                                       port, PM_4X25_INFO(pm_info), rx_ctrl);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


/*!
 * pm12x10_port_tx_mac_sa_set
 *
 * @brief set/get Mac Addr.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  mac_addr        -
 */
int pm12x10_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_mac_sa_set(unit,
                                       port, PM_4X25_INFO(pm_info), mac_addr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm12x10_port_tx_mac_sa_get
 *
 * @brief set/get Mac Addr.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  mac_addr        -
 */
int pm12x10_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_mac_sa_get (unit,
                                       port, PM_4X25_INFO(pm_info), mac_addr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_tx_average_ipg_set (int unit, int port,
                                    pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_average_ipg_set(unit,
                                       port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_tx_average_ipg_get (int unit, int port,
                                    pm_info_t pm_info, int* value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_average_ipg_get (unit,
                                       port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_drv_name_get (int unit, int port, pm_info_t pm_info,
                               char* buf, int len)
{
    strncpy(buf, "PM12X10 Driver", len);
    return (SOC_E_NONE);
}

int pm12x10_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_modid_set(unit,
                                       port, PM_4X25_INFO(pm_info), value);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_led_chain_config(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_LED_CHAIN_CONFIGr (unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_clear_rx_lss_status_set (int unit, soc_port_t port, pm_info_t pm_info,
                                          int lcl_fault, int rmt_fault)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_clear_rx_lss_status_set(unit,
                                       port, PM_4X25_INFO(pm_info), lcl_fault, rmt_fault);
    _SOC_IF_ERR_EXIT(rv);


exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_clear_rx_lss_status_get (int unit, soc_port_t port, pm_info_t pm_info,
                                          int *lcl_fault, int *rmt_fault)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_clear_rx_lss_status_get(unit,
                                       port, PM_4X25_INFO(pm_info), lcl_fault, rmt_fault);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit,
                                       port, PM_4X25_INFO(pm_info));
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_lag_failover_loopback_get (int unit, soc_port_t port, pm_info_t pm_info, int *val)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_failover_loopback_get(unit,
                                       port, PM_4X25_INFO(pm_info), val);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_lag_failover_loopback_set (int unit, soc_port_t port, pm_info_t pm_info, int val)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_failover_loopback_set(unit,
                                       port, PM_4X25_INFO(pm_info), val);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_mode_set (int unit, soc_port_t port, pm_info_t pm_info,
                           const portmod_port_mode_info_t *mode)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_mode_set(unit,
                 port, PM_4X25_INFO(pm_info), mode);

    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_mode_get (int unit, soc_port_t port, pm_info_t pm_info,
                                          portmod_port_mode_info_t *mode)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;
/*
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_mode_get(unit,
                 port, PM_4X25_INFO(pm_info), mode);
*/
    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_mode_get (
             unit, port, PM_4X10_INFO(pm_info,2), mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_ability_remote_get (int unit, soc_port_t port, pm_info_t pm_info, int phy_init_flags,
                                          portmod_port_ability_t *ability)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_ability_remote_get (
             unit, port, PM_4X10_INFO(pm_info,2), phy_init_flags, ability);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_ability_advert_get (int unit, soc_port_t port, pm_info_t pm_info, int phy_init_flags,
                                          portmod_port_ability_t *ability)
{
    int rv = 0;
    int port_fallback_lane, core_seq;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_fallback_lane_get(
                           unit, port, PM_4X10_INFO(pm_info, 2), &port_fallback_lane);
    _SOC_IF_ERR_EXIT(rv);

    if(port_fallback_lane>2||port_fallback_lane<0) { 
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("port_fallback_lane runaway")));
    }
    core_seq = pm12x10_invert_core_seq[port_fallback_lane] ;

    rv = PM_4X10_DRIVER(pm_info, core_seq)->f_portmod_port_ability_advert_get (
             unit, port, PM_4X10_INFO(pm_info,2), phy_init_flags, ability);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_ability_advert_set (int unit, soc_port_t port, pm_info_t pm_info, int phy_init_flags,
                                          portmod_port_ability_t *ability)
{
    int rv = 0, i=0;
    SOC_INIT_FUNC_DEFS;

    for(i = 0 ; i < 3 ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_ability_advert_set (
             unit, port, PM_4X10_INFO(pm_info,i), phy_init_flags, ability);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_encap_get (int unit, soc_port_t port, pm_info_t pm_info, 
                            int *flags, portmod_encap_t *encap)
        
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_encap_get(unit,
                 port, PM_4X25_INFO(pm_info), flags, encap);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_encap_set (int unit, soc_port_t port, pm_info_t pm_info, 
                            int flags, portmod_encap_t encap)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_encap_set(unit,
                 port, PM_4X25_INFO(pm_info), flags, encap);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_higig_mode_set (int unit, soc_port_t port, pm_info_t pm_info, int mode)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_higig_mode_set(unit,
                 port, PM_4X25_INFO(pm_info), mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_higig_mode_get (int unit, soc_port_t port, pm_info_t pm_info, int *mode)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_higig_mode_get(unit,
                 port, PM_4X25_INFO(pm_info), mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_higig2_mode_get (int unit, soc_port_t port, pm_info_t pm_info, int *mode)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_higig2_mode_get(unit,
                 port, PM_4X25_INFO(pm_info), mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_higig2_mode_set (int unit, soc_port_t port, pm_info_t pm_info, int mode)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_higig2_mode_set(unit,
                 port, PM_4X25_INFO(pm_info), mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_port_config_port_type_get (int unit, soc_port_t port, pm_info_t pm_info, int *type)

{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_config_port_type_get(unit,
                 port, PM_4X25_INFO(pm_info), type);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_config_port_type_set (int unit, soc_port_t port, pm_info_t pm_info, int type)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_config_port_type_set(unit,
                 port, PM_4X25_INFO(pm_info), type);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int
pm12x10_port_trunk_hwfailover_config_set (int unit, soc_port_t port, 
                                          pm_info_t pm_info, int hw_count)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_trunk_hwfailover_config_set(unit,
                 port, PM_4X25_INFO(pm_info), hw_count));
}


int
pm12x10_port_trunk_hwfailover_config_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                  int *enable)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_trunk_hwfailover_config_get(unit,
                 port, PM_4X25_INFO(pm_info), enable));
}

int
pm12x10_port_trunk_hwfailover_status_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                  int *loopback)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_trunk_hwfailover_status_get(unit,
                 port, PM_4X25_INFO(pm_info), loopback));
}

int pm12x10_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg)
{

    phymod_phy_access_t phy_access_raw[1+MAX_PHYN];
    phymod_phy_access_t *phy_access;
    phymod_tx_t          ln_txparam;
    int nof_phys=0;
    int i=0;
    uint32 lane_map;
    uint32 saved_lane_mask[3];
    portmod_access_get_params_t params;
    uint32 sum=0, value=0 ; 


    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
    SOC_IF_ERROR_RETURN(phymod_tx_t_init(&ln_txparam));

    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { 
        /* most external  PHY_DIAG_DEV_DFLT and PHY_DIAG_DEV_EXT */  
        params.phyn = -1 ;  
    }

    if(PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS ){
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* line side is default */
        params.sys_side = PORTMOD_SIDE_LINE;
    }

    params.apply_lane_mask = 1;

    SOC_IF_ERROR_RETURN(pm12x10_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 3, phy_access_raw, &nof_phys, NULL));

    /* if the phys  are identical, consolidate lane mask and run  once. */
    if ( nof_phys == 3 ){
        for(i=0; i < nof_phys; i++) {
            saved_lane_mask[i] = phy_access_raw[i].access.lane_mask;
            phy_access_raw[i].access.lane_mask = 0; /* need this to do compare. */
        }

        if( !sal_memcmp( &phy_access_raw[0],&phy_access_raw[1], sizeof(phymod_phy_access_t)) &&
            !sal_memcmp( &phy_access_raw[0],&phy_access_raw[2], sizeof(phymod_phy_access_t))){
            phy_access_raw[0].access.lane_mask = ( saved_lane_mask[0]| saved_lane_mask[1]| saved_lane_mask[2]);
            phymod_access_t_init(&phy_access_raw[1].access);
            phymod_access_t_init(&phy_access_raw[2].access);
            /* cli_out("Consolidating entries.\n"); */
            nof_phys = 1;
        } else {
            /* restore lane mask */
            for(i=0; i < nof_phys; i++) {
                phy_access_raw[i].access.lane_mask = saved_lane_mask[i];
            }
        }
    }

    for(i=0; i < nof_phys; i++) {

        phy_access  = &phy_access_raw[i];
        lane_map    = phy_access->access.lane_mask;

        switch(op_cmd) {
            case PHY_DIAG_CTRL_DSC:
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit, 
                                     "pm4x10_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                         unit, port, PHY_DIAG_CTRL_DSC));

                SOC_IF_ERROR_RETURN
                    (portmod_port_phychain_pmd_info_dump(unit, phy_access, 1,
                                                 (void*)arg));
                break;

            case PHY_DIAG_CTRL_PCS:
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                         unit, port, PHY_DIAG_CTRL_PCS));

                SOC_IF_ERROR_RETURN
                    (portmod_port_phychain_pcs_info_dump(unit, phy_access, 1,
                                                         (void*)arg));
                break;

            case PHY_DIAG_CTRL_LINKMON_MODE:
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_MODE  0x%x\n"),
                         unit, port, PHY_DIAG_CTRL_LINKMON_MODE));

                if(phy_access[0].access.lane_mask){
                    SOC_IF_ERROR_RETURN
                        (portmod_pm_phy_link_mon_enable_set(phy_access, 1, PTR_TO_INT(arg)));
                }
                break;

            case PHY_DIAG_CTRL_LINKMON_STATUS:
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_STATUS 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_LINKMON_STATUS));

                if(phy_access[0].access.lane_mask){
                    SOC_IF_ERROR_RETURN
                        (portmod_pm_phy_link_mon_status_get(phy_access, 1));
                }
                break;


            default:
                if(op_type == PHY_DIAG_CTRL_SET) {
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                     "pm4x10_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                             unit, port, PHY_DIAG_CTRL_SET));
                    if (!SAL_BOOT_SIMULATION) {
                        if( !(phy_access->access.lane_mask == 0)){
                            SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, 1, op_cmd, &ln_txparam, lane_map, PTR_TO_INT(arg)));
                        }
                    }
                } else if(op_type == PHY_DIAG_CTRL_GET) {
                    LOG_INFO(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                     "pm4x10_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                              unit, port, PHY_DIAG_CTRL_GET));
                    if (!SAL_BOOT_SIMULATION) {
                        if( !(phy_access->access.lane_mask == 0)){
                            SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,1,op_cmd, &ln_txparam, lane_map, (uint32 *)arg));
                        } else {
                            *(uint32 *)arg = 0;
                        }
                    }
                } else {
                    return (SOC_E_UNAVAIL);
                }
                break ;
        }
        if(op_type == (int)PHY_DIAG_CTRL_GET) {
            if(op_cmd == (int)SOC_PHY_CONTROL_PRBS_RX_STATUS) {
                /* prbs error count/flag accumulation */
                /* SDK-69842 only the center (last core) 4-lane prbs reported */
                value = *(uint32 *)arg ;
                if((value==(uint32)(-1))||(value==(uint32)(-2))) {
                    if(sum==(uint32)(-1)) {
                        *(uint32 *)arg = sum ;
                    } else {
                        *(uint32 *)arg = value ;
                        sum            = value ;
                    }
                } else {
                    if(sum!=(uint32)(-1)&&(sum!=(uint32)(-2))) {
                        sum = sum+ value ;
                    }
                    *(uint32 *)arg = sum ;
                }
            }
        }
    } /* for */

    return PHYMOD_E_NONE;
}

int
pm12x10_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    return (PM_4X10_DRIVER(pm_info,2)->f_portmod_port_ref_clk_get(unit,
                 port, PM_4X10_INFO(pm_info,2), ref_clk));
}

int pm12x10_port_lag_remove_failover_lpbk_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_remove_failover_lpbk_get(unit,
                 port, PM_4X25_INFO(pm_info), val));
}

int pm12x10_port_lag_remove_failover_lpbk_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_remove_failover_lpbk_set(unit,
                 port, PM_4X25_INFO(pm_info), val));
}

int pm12x10_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_lag_failover_disable(unit,
                 port, PM_4X25_INFO(pm_info)));
}

int pm12x10_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_cntmaxsize_get(unit,
                 port, PM_4X25_INFO(pm_info), val));
}

int pm12x10_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_cntmaxsize_set(unit,
                 port, PM_4X25_INFO(pm_info), val));
}


int pm12x10_port_mac_ctrl_set(int unit, int port, 
                   pm_info_t pm_info, uint64 ctrl)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_mac_ctrl_set(unit,
                 port, PM_4X25_INFO(pm_info), ctrl));
}

int pm12x10_port_drain_cell_get(int unit, int port, 
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_drain_cell_get(unit,
                 port, PM_4X25_INFO(pm_info), drain_cells));
}

int pm12x10_port_drain_cell_stop (int unit, int port, 
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_drain_cell_stop(unit,
                 port, PM_4X25_INFO(pm_info), drain_cells));
}

int pm12x10_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_drain_cell_start(unit,
                 port, PM_4X25_INFO(pm_info)));
}

int pm12x10_port_txfifo_cell_cnt_get(int unit, int port, 
                       pm_info_t pm_info, uint32* fval)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_txfifo_cell_cnt_get(unit,
                 port, PM_4X25_INFO(pm_info), fval));
}

int pm12x10_port_egress_queue_drain_get(int unit, int port, 
                   pm_info_t pm_info, uint64 *ctrl, int *rx)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_egress_queue_drain_get(unit,
                 port, PM_4X25_INFO(pm_info), ctrl, rx));
}

int pm12x10_port_drain_cells_rx_enable (int unit, int port, 
                   pm_info_t pm_info, int rx_en)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_drain_cells_rx_enable (unit,
                 port, PM_4X25_INFO(pm_info), rx_en));
}

int pm12x10_port_egress_queue_drain_rx_en(int unit, int port, 
                   pm_info_t pm_info, int rx_en)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_egress_queue_drain_rx_en (unit,
                 port, PM_4X25_INFO(pm_info), rx_en));
}

int pm12x10_port_mac_reset_set(int unit, int port, 
                   pm_info_t pm_info, int val)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_mac_reset_set (unit,
                 port, PM_4X25_INFO(pm_info), val));
}

int pm12x10_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_mac_reset_get (unit,
                 port, PM_4X25_INFO(pm_info), val));
}

int pm12x10_port_soft_reset_toggle (int unit, int port, pm_info_t pm_info, int idx)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_soft_reset_toggle (unit,
                 port, PM_4X25_INFO(pm_info), idx));
}

int pm12x10_port_mac_reset_check(int unit, int port, pm_info_t pm_info, int enable, int* reset)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_mac_reset_check(unit,
                 port, PM_4X25_INFO(pm_info), enable, reset));
}

int pm12x10_pm4x10_enable(int unit, int port, uint32 phy, uint32 enable) 
{
    pm_info_t pm_info;
    int found = 0, quad, i, phy_i;
    uint32 soft_reset, power_save;
    soc_port_if_t if_type;
    SOC_INIT_FUNC_DEFS;

    /* get PM info */
    _SOC_IF_ERR_EXIT(portmod_pm_info_type_get(unit, port, portmodDispatchTypePm12x10, &pm_info)); 

    if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
        /* get inteface type */
        _SOC_IF_ERR_EXIT(portmod_port_interface_type_get(unit, port, &if_type));

        /* Find quad */
        i=0;
        quad=-1;
        PORTMOD_PBMP_ITER(PM_12x10_INFO(pm_info)->phys, phy_i) {
            if(i%4 == 0) {
                quad++;
            }
            if(phy_i == phy) {
                found = 1;
                break;
            }
            i++;
        }

        if(!found) {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Phy invalid for current PM")));
        }
        
        /* Enable block */
        _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_SOFT_RESETr(unit, port, &soft_reset));
        _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_POWER_SAVEr(unit, port, &power_save));

        switch(quad) {
            case 0:
                soc_reg_field_set(unit, CXX_CXXPORT_SOFT_RESETr, &soft_reset, CORE0_RSTf, enable ? 0 : 1);
                soc_reg_field_set(unit, CXX_CXXPORT_POWER_SAVEr, &power_save, CORE0_PWRDWNf, enable ? 0 : 1);
                break;
            case 1:
                soc_reg_field_set(unit, CXX_CXXPORT_SOFT_RESETr, &soft_reset, CORE1_RSTf, enable ? 0 : 1);
                soc_reg_field_set(unit, CXX_CXXPORT_POWER_SAVEr, &power_save, CORE1_PWRDWNf, enable ? 0 : 1);
                break;
            case 2:
                soc_reg_field_set(unit, CXX_CXXPORT_SOFT_RESETr, &soft_reset, CORE2_RSTf, enable ? 0 : 1);
                soc_reg_field_set(unit, CXX_CXXPORT_POWER_SAVEr, &power_save, CORE2_PWRDWNf, enable ? 0 : 1);
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Invalid quad")));
        }

        /* PM4x25 reset */
        if(if_type != SOC_PORT_IF_CAUI) {
            soc_reg_field_set(unit, CXX_CXXPORT_SOFT_RESETr, &soft_reset, CORE3_RSTf, 1); /* always in reset */
            soc_reg_field_set(unit, CXX_CXXPORT_POWER_SAVEr, &power_save, CORE3_PWRDWNf, 1); /* always power down */
        } else {
            soc_reg_field_set(unit, CXX_CXXPORT_SOFT_RESETr, &soft_reset, CORE3_RSTf, enable ? 0 : 1);
            soc_reg_field_set(unit, CXX_CXXPORT_POWER_SAVEr, &power_save, CORE3_PWRDWNf, enable ? 0 : 1);
        }

        if(enable) {
            _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_POWER_SAVEr(unit, port, power_save));
            _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_SOFT_RESETr(unit, port, soft_reset));
        } else {
            _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_SOFT_RESETr(unit, port, soft_reset));
            _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_POWER_SAVEr(unit, port, power_save));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_pm4x10_tsc_reset(int unit, int port, uint32 phy, uint32 in_reset) 
{
    pm_info_t pm_info;
    int found = 0, quad, i, phy_i;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* get PM info */
    _SOC_IF_ERR_EXIT(portmod_pm_info_type_get(unit, port, portmodDispatchTypePm12x10, &pm_info)); 

    if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
        /* Find quad */
        i=0;
        quad=-1;
        PORTMOD_PBMP_ITER(PM_12x10_INFO(pm_info)->phys, phy_i) {
            if(i%4 == 0) {
                quad++;
            }
            if(phy_i == phy) {
                found = 1;
                break;
            }
            i++;
        }

        if(!found) {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Phy invalid for current PM")));
        }
        
        /* Enable block */
        switch(quad) {
            case 0:
                _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_XGXS_0_CTRL_0_REGr(unit, port, &reg_val));
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_0_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_0_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_0_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_0_REGr, &reg_val, REFOUT_ENf, 
                                         (pm_info->pm_data.pm12x10_db->refclk_source == 0)? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_0_REGr, &reg_val, REFIN_ENf, 
                                         (pm_info->pm_data.pm12x10_db->refclk_source == 0)? 0 : 1);
                _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_XGXS_0_CTRL_0_REGr(unit, port, reg_val));
                break;
            case 1:
                _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_XGXS_0_CTRL_1_REGr(unit, port, &reg_val));
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_1_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_1_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_1_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_1_REGr, &reg_val, REFOUT_ENf, 
                                         (pm_info->pm_data.pm12x10_db->refclk_source == 1)? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_1_REGr, &reg_val, REFIN_ENf, 
                                         (pm_info->pm_data.pm12x10_db->refclk_source == 1)? 0 : 1);
                _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_XGXS_0_CTRL_1_REGr(unit, port, reg_val));
                break;
            case 2:
                _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_XGXS_0_CTRL_2_REGr(unit, port, &reg_val));
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_2_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_2_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_2_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_2_REGr, &reg_val, REFOUT_ENf, 
                                         (pm_info->pm_data.pm12x10_db->refclk_source == 2)? 1 : 0);
                soc_reg_field_set (unit, CXX_CXXPORT_XGXS_0_CTRL_2_REGr, &reg_val, REFIN_ENf, 
                                         (pm_info->pm_data.pm12x10_db->refclk_source == 2)? 0 : 1);
                _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_XGXS_0_CTRL_2_REGr(unit, port, reg_val));
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Invalid quad")));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_core_num_get(int unit, int port, pm_info_t pm_info, int* core_num)
{
    int rv = 0, i;
    SOC_INIT_FUNC_DEFS;

    for(i = 0 ; i < 3 ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_core_num_get(unit, port, 
                                             PM_4X10_INFO(pm_info,i), core_num);
        _SOC_IF_ERR_EXIT(rv);
    }
    
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_ext_phy_attach_to_pm(int unit, pm_info_t pm_info, const phymod_core_access_t* ext_phy_access, uint32 first_phy_lane)
{
    /* empty happy function. */
    return SOC_E_NONE;
}


int pm12x10_ext_phy_detach_from_pm(int unit, pm_info_t pm_info, phymod_core_access_t* ext_phy_access)
{
    /* empty happy function. */
    return SOC_E_NONE;
}

int pm12x10_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    /* empty happy function. */
    return SOC_E_NONE;
}


int pm12x10_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    /* empty happy function. */
    return SOC_E_NONE;
}

/*!
 * pm12x10_port_speed_get
 *
 * @brief port speed get 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  speed         -
 */
int pm12x10_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    return (PM_4X10_DRIVER(pm_info,2)->f_portmod_port_speed_get(unit,
                            port, PM_4X10_INFO(pm_info,2), speed));
}

/*Port discard set*/
int pm12x10_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_discard_set(unit,
                            port, PM_4X25_INFO(pm_info), discard));
}

/*Port soft reset set set*/
int pm12x10_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx,
                                int val, int flags)
{
    int rv = 0, i;
    SOC_INIT_FUNC_DEFS;

    for(i = 0 ; i < 3 ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_soft_reset_set(unit, port,
                                             PM_4X10_INFO(pm_info,i), idx, val, flags);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN; 
}

/*Port tx_en=0 and softreset mac*/
int pm12x10_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_down(unit,
                            port, PM_4X25_INFO(pm_info)));
}

int pm12x10_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_rx_mac_enable_set(unit,
                            port, PM_4X25_INFO(pm_info), enable));
}

int pm12x10_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_rx_mac_enable_get(unit,
                            port, PM_4X25_INFO(pm_info), enable));
}

int pm12x10_port_tx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_mac_enable_set(unit,
                            port, PM_4X25_INFO(pm_info), enable));
}

int pm12x10_port_tx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    return (PM_4X25_DRIVER(pm_info)->f_portmod_port_tx_mac_enable_get(unit,
                            port, PM_4X25_INFO(pm_info), enable));
}

/* 
 * Flex port PGW reconfigure sequence for 100G.
 *
 * 1. XLPORT_SOFT_RESET to 1 for all 12 lanes
 *    - pm4x10_portmod_port_soft_reset_set
 *      (XLPORT_SOFT_RESET should be controlled by physical port basis.
 *       flags : PORTMOD_PORT_REG_ACCESS_DIRECT)
 *
 * 2. XLPORT/XLMAC down sequence
 *    - pm4x10_port_pgw_reconfig(flag : PORTMOD_PORT_PGW_MAC_RESET)
 *
 * 3. CPORT/CLMAC down sequence
 *    - pm4x25td_port_pgw_reconfig(flag : PORTMOD_PORT_PGW_MAC_RESET)
 *
 * 4. XLPORT up sequence1
 *    - pm4x10_port_pgw_reconfig(flag : PORTMOD_PORT_PGW_POWER_SAVE)
 *
 * 5. CPORT/CLMAC up sequence
 *    - pm4x25td_port_pgw_reconfig(flag : PORTMOD_PORT_PGW_MAC_UNRESET)
 *
 * 6. XLPORT up sequence2
 *    - pm4x10_port_pgw_reconfig(flag : PORTMOD_PORT_PGW_MAC_UNRESET)
 *
 * 7. XLPORT_SOFT_RESET to 0 for all 12 lanes
 *    - pm4x10_portmod_port_soft_reset_set
 *      (XLPORT_SOFT_RESET should be controlled by physical port basis.
 *       flags : PORTMOD_PORT_REG_ACCESS_DIRECT)
 */
int pm12x10_port_pgw_reconfig (int unit, int port, pm_info_t pm_info,
                               const portmod_port_mode_info_t *pmode,
                               int first_port, int flags)
{
    int rv = 0, i;
    int phy_idx, lane_idx;
    int phy_port;
    SOC_INIT_FUNC_DEFS;

    /* phy_port is first physical port of 3 TSC Cores used by 100G */
    for (phy_idx = 0; phy_idx < 3 ; ++phy_idx ) {
        for (lane_idx = 0; lane_idx < 4; ++lane_idx) {
            phy_port = first_port + 4*phy_idx+ lane_idx;
            rv = PM_4X10_DRIVER(pm_info, phy_idx)->f_portmod_port_soft_reset_set
                     (unit, phy_port, PM_4X10_INFO(pm_info, phy_idx), lane_idx, 1,
                      PORTMOD_PORT_REG_ACCESS_DIRECT);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    for(i = 0 ; i < 3 ; i++) {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_pgw_reconfig(unit, port,
                                             PM_4X10_INFO(pm_info,i), pmode, first_port,
                                             PORTMOD_PORT_PGW_MAC_RESET);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT((PM_4X25_DRIVER(pm_info)->f_portmod_port_pgw_reconfig(unit,
                            port, PM_4X25_INFO(pm_info), pmode, first_port,
                            PORTMOD_PORT_PGW_MAC_RESET)));

    for(i = 0 ; i < 3 ; i++) {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_pgw_reconfig(unit, port,
                                             PM_4X10_INFO(pm_info,i), pmode, first_port,
                                             PORTMOD_PORT_PGW_POWER_SAVE);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT((PM_4X25_DRIVER(pm_info)->f_portmod_port_pgw_reconfig(unit,
                            port, PM_4X25_INFO(pm_info), pmode, first_port,
                            PORTMOD_PORT_PGW_MAC_UNRESET)));

    /* phy_port is first physical port of 3 TSC Cores used by 100G */
    for (phy_idx = 0; phy_idx < 3 ; ++phy_idx ) {
        for (lane_idx = 0; lane_idx < 4; ++lane_idx) {
            phy_port = first_port + 4*phy_idx+ lane_idx;
            rv = PM_4X10_DRIVER(pm_info, phy_idx)->f_portmod_port_soft_reset_set
                     (unit, phy_port, PM_4X10_INFO(pm_info, phy_idx), lane_idx, 0,
                      PORTMOD_PORT_REG_ACCESS_DIRECT);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    for(i = 0 ; i < 3 ; i++) {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_pgw_reconfig(unit, port,
                                             PM_4X10_INFO(pm_info,i), pmode, first_port,
                                             PORTMOD_PORT_PGW_MAC_UNRESET);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_notify(int unit, int port, pm_info_t pm_info,  int link) {
    int i = 0;
    int rv = 0;
    int tmp_nof_phys;
    int is_most_ext_i;
    int is_change_speed = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t access[MAX_PHYN + 1];
    portmod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    for( i = 0 ; i < 3 ; i++){
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_phy_lane_access_get(
                                       unit, port, PM_4X10_INFO(pm_info, i), 
                                       &params, 1, &access[i], &tmp_nof_phys, &is_most_ext_i);
        _SOC_IF_ERR_EXIT(rv);

        if (tmp_nof_phys > 1) {
            is_change_speed = 1;
        }

    }

    if (link && is_change_speed) {
        /* get the external phy interface  config */
        _SOC_IF_ERR_EXIT(portmod_port_interface_config_get(unit,
                                                           port,
                                                           &interface_config,
                                                           PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
        /* Set the interface config for internal phy only */
        _SOC_IF_ERR_EXIT(portmod_port_interface_config_set(unit,
                                                           port,
                                                           &interface_config, 
                                                           PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
    } else {
        /*TBD*/
    }

    exit:
    SOC_FUNC_RETURN; 
}

/* set interrupt value. */
int pm12x10_port_interrupt_enable_get (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 *val)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_INTR_ENABLEr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeCore0Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, CORE0_INTf);
             break;
        case portmodIntrTypeCore1Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, CORE1_INTf);
             break;
        case portmodIntrTypeCore2Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, CORE2_INTf);
             break;
        case portmodIntrTypeCore3Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, CORE3_INTf);
             break;
        case portmodIntrTypeTsc0Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, TSC_0_INTf);
             break;
        case portmodIntrTypeTsc1Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, TSC_0_INTf);
             break;
        case portmodIntrTypeTsc2Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_ENABLEr, reg_val, TSC_0_INTf);
             break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

/* set interrupt value. */ 
int pm12x10_port_interrupt_enable_set (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 val)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_INTR_ENABLEr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeCore0Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, CORE0_INTf, val);
             break;
        case portmodIntrTypeCore1Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, CORE1_INTf, val);
             break;
        case portmodIntrTypeCore2Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, CORE2_INTf, val);
             break;
        case portmodIntrTypeCore3Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, CORE3_INTf, val);
             break;
        case portmodIntrTypeTsc0Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, TSC_0_INTf, val);
             break;
        case portmodIntrTypeTsc1Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, TSC_0_INTf, val);
             break;
        case portmodIntrTypeTsc2Intr:
             soc_reg_field_set(unit, CXX_CXXPORT_INTR_ENABLEr, &reg_val, TSC_0_INTf, val);
             break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
    _SOC_IF_ERR_EXIT(WRITE_CXX_CXXPORT_INTR_ENABLEr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm12x10_port_interrupt_get (int unit, int port, pm_info_t pm_info,
                               int intr_type, uint32* val)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_INTR_STATUSr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeCore0Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE0_INTf);
             break;
        case portmodIntrTypeCore1Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE1_INTf);
             break;
        case portmodIntrTypeCore2Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE2_INTf);
             break;
        case portmodIntrTypeCore3Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE3_INTf);
             break;
        case portmodIntrTypeTsc0Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, TSC_0_INTf);
             break;
        case portmodIntrTypeTsc1Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, TSC_1_INTf);
             break;
        case portmodIntrTypeTsc2Intr:
             *val = soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, TSC_2_INTf);
             break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_interrupts_get (int unit, int port, pm_info_t pm_info,
                                int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 reg_val, cnt = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXX_CXXPORT_INTR_STATUSr(unit, port, &reg_val));

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE0_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore0Intr;
    }

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE1_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore1Intr;
    }

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE2_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore2Intr;
    }

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, CORE3_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore3Intr;
    }

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, TSC_0_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsc0Intr;
    }

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, TSC_1_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsc1Intr;
    }

    if (soc_reg_field_get(unit, CXX_CXXPORT_INTR_STATUSr, reg_val, TSC_2_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsc2Intr;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}


/*!
 * pm12x10_port_eee_clock_set
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm12x10_port_eee_clock_set(int unit, int port, pm_info_t pm_info,
                              const portmod_eee_clock_t* eee_clk)
{
    return(PM_4X25_DRIVER(pm_info)->f_portmod_port_eee_clock_set (unit,
                            port, PM_4X25_INFO(pm_info), eee_clk));
}


/*!
 * pm12x10_port_eee_clock_get
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  eee             -
 */
int pm12x10_port_eee_clock_get (int unit, int port, pm_info_t pm_info,
                               portmod_eee_clock_t* eee_clk)
{
    return(PM_4X25_DRIVER(pm_info)->f_portmod_port_eee_clock_get (unit,
                            port, PM_4X25_INFO(pm_info), eee_clk));
}

/*!
 * pm12x10_port_check_legacy_phy
 *
 * @brief  portmod check if external phy is legacy
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  legacy_phy      - check if phy is legacy
 */
int pm12x10_port_check_legacy_phy(int unit, int port, pm_info_t pm_info, int* legacy_phy)
{
    return(PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_check_legacy_phy(unit, 
                                 port, PM_4X10_INFO(pm_info, 2), legacy_phy));
}


int pm12x10_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    int i, rv=0;

    for (i = 0; !rv && (i < 3); i++) { 
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_failover_mode_set(unit, 
                                 port, PM_4X10_INFO(pm_info, i), failover);
    }
    return (rv);
}

int pm12x10_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    int i, rv=0;

    for (i = 0; !rv && (i < 3); i++) { 
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_failover_mode_get(unit, 
                                 port, PM_4X10_INFO(pm_info, i), failover);
    }
    return (rv);
}

int pm12x10_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
    int i, rv=0;

    for (i = 0; !rv && (i < 3); i++) { 
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_mac_rsv_mask_set(unit, 
                                 port, PM_4X10_INFO(pm_info, i), rsv_mask);
    }
    return (rv);
}

int pm12x10_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    int i, rv=0;

    for (i = 0; !rv && (i < 3); i++) { 
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_mib_reset_toggle(unit, 
                                 port, PM_4X10_INFO(pm_info, i), port_index);
    }
    return (rv);
}

int pm12x10_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                         const portmod_port_interface_config_t *f_intf_config,
                         const portmod_port_init_config_t      *f_init_config,
                         const int phy_op_mode)

{
    int i, rv=0;

    for (i = 0; !rv && (i < 3); i++) { 
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_warmboot_db_restore(unit, 
                    port, PM_4X10_INFO(pm_info, i), f_intf_config, f_init_config, phy_op_mode);
    }
    return (rv);
}

int pm12x10_port_flow_control_set (int unit, int port, pm_info_t pm_info,
                                  int merge_mode_en, int parallel_fc_en)
{
    int i, rv=0;

    for (i = 0; !rv && (i < 3); i++) { 
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_flow_control_set(unit, 
                                 port, PM_4X10_INFO(pm_info, i), merge_mode_en, parallel_fc_en);
    }
    if (SOC_FAILURE(rv)) return (rv);

    rv = (PM_4X25_DRIVER(pm_info)->f_portmod_port_flow_control_set (unit,
                            port, PM_4X25_INFO(pm_info), merge_mode_en, parallel_fc_en));
    return (rv);
}

int pm12x10_port_update_dynamic_state(int unit, int port, pm_info_t pm_info, uint32_t port_dynamic_state) {
    int rv = 0, i;
    SOC_INIT_FUNC_DEFS;

    for(i = 0 ; i < 3 ; i++)
    {
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_update_dynamic_state(unit, port,
                                             PM_4X10_INFO(pm_info,i), port_dynamic_state);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info, phymod_operation_mode_t* val)
{
    int rv = 0;
    int port_fallback_lane;

    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_fallback_lane_get(
                           unit, port, PM_4X10_INFO(pm_info, 2), &port_fallback_lane);

   if (SOC_FAILURE(rv)) return (rv); 

   return (PM_4X10_DRIVER(pm_info, port_fallback_lane)->f_portmod_port_phy_op_mode_get(
                                unit, port, PM_4X10_INFO(pm_info, port_fallback_lane), val));

}

int pm12x10_port_medium_config_set(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    int rv, i;
    SOC_INIT_FUNC_DEFS;

    for(i = 0 ; i < 3 ; i++){
        rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_medium_config_set(unit, port, 
                                            PM_4X10_INFO(pm_info, i), medium, config);
        if (SOC_FAILURE(rv)) return (rv);
    }

    SOC_FUNC_RETURN;
}

int pm12x10_port_medium_config_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_medium_config_get(unit, port, 
                                        PM_4X10_INFO(pm_info, 0), medium, config);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm12x10_port_medium_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t* medium)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_medium_get(unit, port, 
                                        PM_4X10_INFO(pm_info, 0), medium);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm12x10_port_multi_get(int unit, int port, pm_info_t pm_info, portmod_multi_get_t* multi_get)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_multi_get(unit, port, 
                                        PM_4X10_INFO(pm_info, 0), multi_get);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm12x10_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{

    return (SOC_E_NONE);
}

int pm12x10_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    int i= 0,phy_port = 0,phy_pos = 0,core_num = 0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    SOC_PBMP_ITER(pm_info->pm_data.pm12x10_db->phys, i) {
        if (i == phy_port) {
             core_num = (phy_pos % PHYMOD_MAX_LANES_PER_PORT);
             break;
        }
         phy_pos++;
    }
    if ((core_num<3) && (PM_4X10_DRIVER(pm_info, core_num)->f_portmod_port_lane_map_get)) {
        _rv = PM_4X10_DRIVER(pm_info, core_num)->f_portmod_port_lane_map_get(unit, port,
                                    PM_4X10_INFO(pm_info, core_num), flags, lane_map);
    } else {
        _rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_lane_map_get(unit, port,
                                    PM_4X25_INFO(pm_info), flags, lane_map);
    }
    SOC_FUNC_RETURN;
}

int pm12x10_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    return SOC_E_NONE;
}

int pm12x10_port_pll_div_get(int unit, int port, pm_info_t pm_info,
                             const portmod_port_resources_t* port_resource,
                             uint32_t* pll_div)
{
    return SOC_E_NONE;
}

int pm12x10_port_lane_count_get (int unit, int port, pm_info_t pm_info, int line_side, int* num_lanes)
{

    int rv, i;
    int local_num_lanes=1;
    SOC_INIT_FUNC_DEFS;

    *num_lanes = 0;
    for(i = 0 ; i < 3 ; i++){
        if(PM_4X10_DRIVER(pm_info, i)->f_portmod_port_lane_count_get != NULL){
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_lane_count_get(unit, port,
                                        PM_4X10_INFO(pm_info, i), line_side, &local_num_lanes);
            if (SOC_FAILURE(rv)) return (rv);
            *num_lanes += local_num_lanes ;
        }
    }
    SOC_FUNC_RETURN;
}

int
pm12x10_port_timestamp_adjust_set(int unit, int port, pm_info_t pm_info,
                          soc_port_timestamp_adjust_t *ts_adjust)

{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_timestamp_adjust_set(unit,
                                       port, PM_4X25_INFO(pm_info), ts_adjust);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

int pm12x10_port_timestamp_adjust_get(int unit, int port, pm_info_t pm_info,
                          soc_port_timestamp_adjust_t *ts_adjust)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_timestamp_adjust_get(unit,
                                       port, PM_4X25_INFO(pm_info), ts_adjust);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

int pm12x10_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    portmod_port_interface_config_t config;
    int i =0, rv=0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_interface_config_t_init(unit, &config));
    rv = PM_4X10_DRIVER(pm_info, 2)->f_portmod_port_interface_config_get(
                        unit, port, PM_4X10_INFO(pm_info, 2), &config, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY);
    _SOC_IF_ERR_EXIT(rv);
    /* speed disable */
    config.flags |= PHYMOD_INTF_F_SET_SPD_DISABLE;
    _SOC_IF_ERR_EXIT (_pm12x10_internal_if_cfg_set (unit, port, pm_info, &config,
                                       PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));

    for (i = 0 ; !rv && (i < 3) ; i++) {
        if (PM_4X10_DRIVER(pm_info, i)->f_portmod_port_cl72_set != NULL) {
            rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_cl72_set(
                             unit, port, PM_4X10_INFO(pm_info, i), enable);
        }
    }
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    int rv=0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X10_DRIVER(pm_info, 0)->f_portmod_port_cl72_get(unit, port,
                                       PM_4X10_INFO(pm_info, 0), enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                           portmod_egr_1588_timestamp_config_t timestamp_config)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_egr_1588_timestamp_config_set(unit, port,
                                            PM_4X25_INFO(pm_info), timestamp_config);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                          portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_egr_1588_timestamp_config_get(unit, port,
                                            PM_4X25_INFO(pm_info), timestamp_config);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_fec_error_inject_set(int unit, int port, pm_info_t pm_info,
                                      uint16 error_control_map,
                                      portmod_fec_error_mask_t bit_error_mask)
{
    int rv = 0, i;
    uint16 error_control = 0;
    SOC_INIT_FUNC_DEFS;

    for (i = 0; i < 3; i++) {
         error_control = (error_control_map >> (0x4 * i)) & 0xf;
         rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_fec_error_inject_set(
                    unit, port, PM_4X10_INFO(pm_info, i), error_control, bit_error_mask);
         _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_port_fec_error_inject_get(int unit, int port, pm_info_t pm_info,
                                      uint16* error_control_map,
                                      portmod_fec_error_mask_t* bit_error_mask)
{
    int rv = 0, i;
    uint16 error_control;
    portmod_fec_error_mask_t fec_error_mask;
    SOC_INIT_FUNC_DEFS;

    *error_control_map = 0;
    for (i = 0; i < 3; i++) {
         rv = PM_4X10_DRIVER(pm_info, i)->f_portmod_port_fec_error_inject_get(
                    unit, port, PM_4X10_INFO(pm_info, i), &error_control, &fec_error_mask);
         _SOC_IF_ERR_EXIT(rv);
         *error_control_map |= (error_control & 0xf) << (0x4 * i);
    }
    sal_memcpy(bit_error_mask, &fec_error_mask, sizeof(portmod_fec_error_mask_t));

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM12X10_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
