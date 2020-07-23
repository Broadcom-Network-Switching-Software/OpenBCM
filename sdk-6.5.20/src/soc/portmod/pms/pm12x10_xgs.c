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
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/pm4x10.h>
#include <soc/portmod/pm12x10.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/pm4x25_shared.h>
#include <soc/portmod/portmod_system.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM12X10_XGS_SUPPORT

static int pm12x10_xgs_core_seq[] = {0,1,2};

#define PM_12x10_INFO(pm_info) ((pm_info)->pm_data.pm12x10_db)
#define PM_4X25_INFO(pm_info) (PM_12x10_INFO(pm_info)->pm4x25_info)
#define PM_4x25_DB(pm_info) (PM_4X25_INFO(pm_info)->pm_data.pm4x25_db)


#define PM_4X10_INFO(pm_info, i) (PM_12x10_INFO(pm_info)->pm4x10_info[pm12x10_xgs_core_seq[i]])
#define PM_4X25_DRIVER(pm_info) PM_DRIVER(PM_4X25_INFO(pm_info))
#define PM_4X10_DRIVER(pm_info, i) PM_DRIVER(PM_4X10_INFO(pm_info, pm12x10_xgs_core_seq[i]))


#define TOP_BLK_ID_OFFSET (4)

#define PM12x10_INTERNAL_4x10_CNT (3)

struct pm12x10_s{
    int blk_id;
    pm_info_t pm4x25_info;
    pm_info_t pm4x10_info[PM12x10_INTERNAL_4x10_CNT];
    portmod_pbmp_t phys;
    uint32 external_top_mode;
    int    refclk_source;
};



STATIC int
pm12x10_xgs_quad0_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXXPORT_WC_UCMEM_DATA0m, user_acc, core_addr, reg_addr, val);
}

STATIC int
pm12x10_xgs_quad0_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXXPORT_WC_UCMEM_DATA0m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_xgs_quad0_default_bus = {
    "PM12x10_xgs Quad0 Bus",
    pm12x10_xgs_quad0_default_bus_read,
    pm12x10_xgs_quad0_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC int
pm12x10_xgs_quad1_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXXPORT_WC_UCMEM_DATA1m, user_acc, core_addr, reg_addr, val);
}

STATIC int
pm12x10_xgs_quad1_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXXPORT_WC_UCMEM_DATA1m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_xgs_quad1_default_bus = {
    "PM12x10_xgs Quad1 Bus",
    pm12x10_xgs_quad1_default_bus_read,
    pm12x10_xgs_quad1_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC int
pm12x10_xgs_quad2_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CXXPORT_WC_UCMEM_DATA2m, user_acc, core_addr, reg_addr, val);
}

STATIC int
pm12x10_xgs_quad2_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CXXPORT_WC_UCMEM_DATA2m, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_xgs_quad2_default_bus = {
    "PM12x10_xgs Quad2 Bus",
    pm12x10_xgs_quad2_default_bus_read,
    pm12x10_xgs_quad2_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC portmod_ucode_buf_t pm12x10_xgs_ucode_buf[SOC_MAX_NUM_DEVICES]  = {{NULL, 0}};

STATIC int
pm4x10_xgs_quad0_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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
                              &(pm12x10_xgs_ucode_buf[unit]),
                              NULL,
                              CXXPORT_WC_UCMEM_DATA0m,
                              CXXPORT_WC_UCMEM_CTRL0r);
    _SOC_IF_ERR_EXIT(rv); 

exit:
    SOC_FUNC_RETURN;
}

STATIC int
pm4x10_xgs_quad1_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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
                              &(pm12x10_xgs_ucode_buf[unit]),
                              NULL,
                              CXXPORT_WC_UCMEM_DATA1m,
                              CXXPORT_WC_UCMEM_CTRL1r);
    _SOC_IF_ERR_EXIT(rv); 

exit:
    SOC_FUNC_RETURN;
}

STATIC int
pm4x10_xgs_quad2_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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
                              &(pm12x10_xgs_ucode_buf[unit]),
                              NULL,
                              CXXPORT_WC_UCMEM_DATA2m,
                              CXXPORT_WC_UCMEM_CTRL2r);
    _SOC_IF_ERR_EXIT(rv); 

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_xgs_port_core_num_get(int unit, int port, pm_info_t pm_info, int* core_num)
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

int pm12x10_xgs_pm_destroy(int unit, pm_info_t pm_info)
{
    if(pm_info->pm_data.pm12x10_db != NULL){
        sal_free(pm_info->pm_data.pm12x10_db);
        pm_info->pm_data.pm12x10_db = NULL;
    }

    /* free pm12x10_xgs_ucode_buf */
    if(pm12x10_xgs_ucode_buf[unit].ucode_dma_buf != NULL){
        portmod_sys_dma_free(unit, pm12x10_xgs_ucode_buf[unit].ucode_dma_buf);
        pm12x10_xgs_ucode_buf[unit].ucode_dma_buf = NULL;
    }
    pm12x10_xgs_ucode_buf[unit].ucode_alloc_size = 0;

    return SOC_E_NONE;
}



int pm12x10_xgs_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, 
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
    pm_info->type = portmodDispatchTypePm12x10_xgs;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    pm_info->pm_data.pm12x10_db = pm12x10_data;
    pm_info->pm_data.pm12x10_db->blk_id =  pm_add_info->pm_specific_info.pm12x10.blk_id; 
    PORTMOD_PBMP_ASSIGN(pm_info->pm_data.pm12x10_db->phys, pm_add_info->phys);

    for (core = 0; core < PM12x10_INTERNAL_4x10_CNT; core++) {
        pm_info->pm_data.pm12x10_db->pm4x10_info[core] = 
                pm_add_info->pm_specific_info.pm12x10.pm4x10[core];
    }

    pm_info->pm_data.pm12x10_db->pm4x25_info = pm_add_info->pm_specific_info.pm12x10.pm4x25;
    pm_info->pm_data.pm12x10_db->refclk_source = pm_add_info->pm_specific_info.pm12x10.refclk_source;
    pm_info->pm_data.pm12x10_db->external_top_mode = PORTMOD_PM12x10_F_EXTERNAL_TOP_MODE_GET(pm_add_info->pm_specific_info.pm12x10.flags);

    if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
        /* Update Quad 0 */
        bupdate.blk_id = pm_info->pm_data.pm12x10_db->blk_id;
        bupdate.default_bus = &pm4x10_xgs_quad0_default_bus;
        bupdate.external_fw_loader = pm4x10_xgs_quad0_default_fw_loader;
        bupdate.user_acc = NULL; /*use default user_acc */
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_12x10_INFO(pm_info)->pm4x10_info[0], &bupdate));

        /* Update Quad 1 */
        bupdate.default_bus = &pm4x10_xgs_quad1_default_bus;
        bupdate.external_fw_loader = pm4x10_xgs_quad1_default_fw_loader;
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_12x10_INFO(pm_info)->pm4x10_info[1], &bupdate));

        /* Update Quad 2 */
        bupdate.default_bus = &pm4x10_xgs_quad2_default_bus;
        bupdate.external_fw_loader = pm4x10_xgs_quad2_default_fw_loader;
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_12x10_INFO(pm_info)->pm4x10_info[2], &bupdate));

    }

exit:
    if(SOC_FUNC_ERROR){
        pm12x10_xgs_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN; 
}

int pm12x10_xgs_port_attach (int unit, int port, pm_info_t pm_info, 
                         const portmod_port_add_info_t* add_info)
{
    portmod_port_add_info_t  lcl_add_info = *add_info;
    uint32 nof_lanes = 0;
    int i = 0;
    int rv = 0;
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
            rv = WRITE_CXXPORT_MODE_REGr(unit, port, 1); 
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    /* CAUI10 port is not connected to any PHY.., so clear the phy information before adding it */
    lcl_add_info = *add_info;
    SOC_PBMP_CLEAR(lcl_add_info.phys);
    for(i = 0 ; i < PM4X25_LANES_PER_CORE; i++){
        PM_4x25_DB(pm_info)->nof_phys[i] = 0;
    }

    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_attach(unit, port, PM_4X25_INFO(pm_info), &lcl_add_info);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN; 
}

int pm12x10_xgs_port_detach(int unit, int port, pm_info_t pm_info)
{
    int rv = 0;
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    /* Detach MAC first since detaching PHY and resetting the TSC's first
     * will render CLMAC/CLPORT registers inaccessible
     */
    rv = PM_4X25_DRIVER(pm_info)->f_portmod_port_detach(unit, port, PM_4X25_INFO(pm_info));
    _SOC_IF_ERR_EXIT(rv);

    for(i = 0 ; i < 3; i++){
        _SOC_IF_ERR_EXIT(PM_4X10_DRIVER(pm_info, i)->f_portmod_port_detach(unit,
                                   port, PM_4X10_INFO(pm_info, i)));
    }

    
    if(!pm_info->pm_data.pm12x10_db->external_top_mode) {
        rv = WRITE_CXXPORT_MODE_REGr(unit, port, 0); 
        _SOC_IF_ERR_EXIT(rv);
    }

    for(i = 0 ; i < 3 ; i++)
    {
        PM_4X10_DRIVER(pm_info, i)->f_portmod_pm_bypass_set(unit, PM_4X10_INFO(pm_info,i), 0);
    }
    
exit:
    SOC_FUNC_RETURN; 
}


int pm12x10_xgs_port_led_chain_config(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(WRITE_CXXPORT_LED_CHAIN_CONFIGr (unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

int pm12x10_xgs_pm4x10_enable(int unit, int port, uint32 phy, uint32 enable) 
{
    pm_info_t pm_info;
    int found = 0, quad, i, phy_i;
    int core_num;
    uint32 soft_reset, power_save;
    soc_port_if_t if_type;
    SOC_INIT_FUNC_DEFS;

    /* get PM info */
    _SOC_IF_ERR_EXIT(portmod_pm_info_type_get(unit, port, portmodDispatchTypePm12x10_xgs, &pm_info)); 

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
        
        _SOC_IF_ERR_EXIT(pm12x10_xgs_port_core_num_get(unit, port, pm_info, &core_num));

        if (!((((core_num >= 4) && (core_num <= 6))) || ((core_num >= 13) && (core_num <= 15)))) {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("invalid core for current PM")));
        }

        /* Enable block */
        _SOC_IF_ERR_EXIT(READ_CXXPORT_SOFT_RESETr(unit, port, &soft_reset));
        _SOC_IF_ERR_EXIT(READ_CXXPORT_POWER_SAVEr(unit, port, &power_save));

        switch(quad) {
            case 0:
                soc_reg_field_set(unit, CXXPORT_SOFT_RESETr, &soft_reset, CORE0_RSTf, enable ? 0 : 1);
                soc_reg_field_set(unit, CXXPORT_POWER_SAVEr, &power_save, CORE0_PWRDWNf, enable ? 0 : 1);
                break;
            case 1:
                soc_reg_field_set(unit, CXXPORT_SOFT_RESETr, &soft_reset, CORE1_RSTf, enable ? 0 : 1);
                soc_reg_field_set(unit, CXXPORT_POWER_SAVEr, &power_save, CORE1_PWRDWNf, enable ? 0 : 1);
                break;
            case 2:
                soc_reg_field_set(unit, CXXPORT_SOFT_RESETr, &soft_reset, CORE2_RSTf, enable ? 0 : 1);
                soc_reg_field_set(unit, CXXPORT_POWER_SAVEr, &power_save, CORE2_PWRDWNf, enable ? 0 : 1);
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Invalid quad")));
        }

        /* PM4x25 reset */
        if(if_type != SOC_PORT_IF_CAUI) {
            soc_reg_field_set(unit, CXXPORT_SOFT_RESETr, &soft_reset, CORE3_RSTf, 1); /* always in reset */
            soc_reg_field_set(unit, CXXPORT_POWER_SAVEr, &power_save, CORE3_PWRDWNf, 1); /* always power down */
        } else {
            soc_reg_field_set(unit, CXXPORT_SOFT_RESETr, &soft_reset, CORE3_RSTf, enable ? 0 : 1);
            soc_reg_field_set(unit, CXXPORT_POWER_SAVEr, &power_save, CORE3_PWRDWNf, enable ? 0 : 1);
        }

        if(enable) {
            _SOC_IF_ERR_EXIT(WRITE_CXXPORT_POWER_SAVEr(unit, port, power_save));
            _SOC_IF_ERR_EXIT(WRITE_CXXPORT_SOFT_RESETr(unit, port, soft_reset));
        } else {
            _SOC_IF_ERR_EXIT(WRITE_CXXPORT_SOFT_RESETr(unit, port, soft_reset));
            _SOC_IF_ERR_EXIT(WRITE_CXXPORT_POWER_SAVEr(unit, port, power_save));
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int pm12x10_xgs_pm4x10_tsc_reset(int unit, int port, uint32 phy, uint32 in_reset) 
{
    pm_info_t pm_info;
    int found = 0, quad, i, phy_i;
    int core_num;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* get PM info */
    _SOC_IF_ERR_EXIT(portmod_pm_info_type_get(unit, port, portmodDispatchTypePm12x10_xgs, &pm_info)); 

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
        

        _SOC_IF_ERR_EXIT(pm12x10_xgs_port_core_num_get(unit, port, pm_info, &core_num));

        if (!((((core_num >= 4) && (core_num <= 6))) || ((core_num >= 13) && (core_num <= 15)))) {
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("invalid core for current PM")));
        }

        /* Enable block */
        switch(quad) {
            case 0:
                _SOC_IF_ERR_EXIT(READ_CXXPORT_XGXS0_CTRL0_REGr(unit, port, &reg_val));
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL0_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL0_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL0_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL0_REGr, &reg_val, REFOUT_ENf, 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL0_REGr, &reg_val, REFIN_ENf, 1);
                _SOC_IF_ERR_EXIT(WRITE_CXXPORT_XGXS0_CTRL0_REGr(unit, port, reg_val));
                break;
            case 1:
                _SOC_IF_ERR_EXIT(READ_CXXPORT_XGXS0_CTRL1_REGr(unit, port, &reg_val));
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL1_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL1_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL1_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL1_REGr, &reg_val, REFOUT_ENf, 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL1_REGr, &reg_val, REFIN_ENf, 1);
                _SOC_IF_ERR_EXIT(WRITE_CXXPORT_XGXS0_CTRL1_REGr(unit, port, reg_val));
                break;
            case 2:
                _SOC_IF_ERR_EXIT(READ_CXXPORT_XGXS0_CTRL2_REGr(unit, port, &reg_val));
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL2_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL2_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL2_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL2_REGr, &reg_val, REFOUT_ENf, 0);
                soc_reg_field_set (unit, CXXPORT_XGXS0_CTRL2_REGr, &reg_val, REFIN_ENf, 1);
                _SOC_IF_ERR_EXIT(WRITE_CXXPORT_XGXS0_CTRL2_REGr(unit, port, reg_val));
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("Invalid quad")));
        }
    }

exit:
    SOC_FUNC_RETURN;
}


/* set interrupt value. */
int pm12x10_xgs_port_interrupt_enable_get (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 *val)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXXPORT_INTR_ENABLEr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeCore0Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, CORE0_INTf);
             break;
        case portmodIntrTypeCore1Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, CORE1_INTf);
             break;
        case portmodIntrTypeCore2Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, CORE2_INTf);
             break;
        case portmodIntrTypeCore3Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, CORE3_INTf);
             break;
        case portmodIntrTypeTsc0Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, TSC_0_INTf);
             break;
        case portmodIntrTypeTsc1Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, TSC_0_INTf);
             break;
        case portmodIntrTypeTsc2Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_ENABLEr, reg_val, TSC_0_INTf);
             break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

/* set interrupt value. */ 
int pm12x10_xgs_port_interrupt_enable_set (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 val)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXXPORT_INTR_ENABLEr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeCore0Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, CORE0_INTf, val);
             break;
        case portmodIntrTypeCore1Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, CORE1_INTf, val);
             break;
        case portmodIntrTypeCore2Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, CORE2_INTf, val);
             break;
        case portmodIntrTypeCore3Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, CORE3_INTf, val);
             break;
        case portmodIntrTypeTsc0Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, TSC_0_INTf, val);
             break;
        case portmodIntrTypeTsc1Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, TSC_0_INTf, val);
             break;
        case portmodIntrTypeTsc2Intr:
             soc_reg_field_set(unit, CXXPORT_INTR_ENABLEr, &reg_val, TSC_0_INTf, val);
             break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
    _SOC_IF_ERR_EXIT(WRITE_CXXPORT_INTR_ENABLEr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm12x10_xgs_port_interrupt_get (int unit, int port, pm_info_t pm_info,
                               int intr_type, uint32* val)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXXPORT_INTR_STATUSr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeCore0Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE0_INTf);
             break;
        case portmodIntrTypeCore1Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE1_INTf);
             break;
        case portmodIntrTypeCore2Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE2_INTf);
             break;
        case portmodIntrTypeCore3Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE3_INTf);
             break;
        case portmodIntrTypeTsc0Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, TSC_0_INTf);
             break;
        case portmodIntrTypeTsc1Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, TSC_1_INTf);
             break;
        case portmodIntrTypeTsc2Intr:
             *val = soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, TSC_2_INTf);
             break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm12x10_xgs_port_interrupts_get (int unit, int port, pm_info_t pm_info,
                                int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 reg_val, cnt = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CXXPORT_INTR_STATUSr(unit, port, &reg_val));

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE0_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore0Intr;
    }

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE1_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore1Intr;
    }

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE2_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore2Intr;
    }

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, CORE3_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeCore3Intr;
    }

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, TSC_0_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsc0Intr;
    }

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, TSC_1_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsc1Intr;
    }

    if (soc_reg_field_get(unit, CXXPORT_INTR_STATUSr, reg_val, TSC_2_INTf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsc2Intr;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}



#endif /* PORTMOD_PM12X10_XGS_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
