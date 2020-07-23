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
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>

#include <soc/portmod/unimac.h>
#include <soc/portmod/pm4x10Q.h>
#include <soc/portmod/pm4x10.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4x10Q_SUPPORT

#define PM_4x10_INFO(pm_info) ((pm_info)->pm_data.pm4x10_db)
#define PM_4x10Q_INFO(pm_info) ((pm_info)->pm_data.pm4x10q_db)
#define PM_4x10Q_PM_4x10_INFO(pm_info) (PM_4x10Q_INFO(pm_info)->pm4x10->pm_data.pm4x10_db)


#define PHY_REG_ADDR_DEVAD(_phy_reg)                        (((_phy_reg) >> 27) & 0x1f)
#define QSGMII_REG_ADDR_LANE(_phy_reg)                      (((_phy_reg) >> 16) & 0x7ff)
#define QSGMII_REG_ADDR_LANE_SET(_phy_reg_new, _phy_reg)    ((_phy_reg_new) |= ((_phy_reg) & 0x7ff0000))
#define QSGMII_REG_ADDR_REG(_phy_reg)                       ((((_phy_reg) & 0x8000) >> 11) | ((_phy_reg) & 0xf))
#define IS_QSGMII_REGISTER(_phy_reg)                        (((_phy_reg) & 0xf800f000) == 0x8000)

#define PM4X10Q_LANES_PER_CORE          (4)
#define PM4X10Q_MAX_PORTS_PER_PM4X10Q   (16)
#define PM4X10Q_MAX_PORTS_PER_LANE      (4)

#define PM4X10Q_EGR_1588_TIMESTAMP_MODE_48BIT (0)
#define PM4X10Q_EGR_1588_TIMESTAMP_MODE_32BIT (1)

extern phymod_bus_t pm4x10_default_bus;

extern int pm4x10_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data);

typedef enum pm4x10q_wb_vars{
    isInitialized,
    isActive,
    ports,
    phyAccPorts,
    numPortsPerLane
}pm4x10q_wb_vars_t;

struct pm4x10q_s{
    portmod_pbmp_t phys;
    int is_initialized;
    uint32 blk_id;
    void* qsgmii_user_acc;
    void* pm4x10_user_acc;
    int auto_cfg[PM4X10Q_MAX_PORTS_PER_PM4X10Q];
    int is_bypass;
    phymod_ref_clk_t ref_clk;
    pm_info_t pm4x10;
    uint32 core_clock_khz;
};

STATIC int
_pm4x10q_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    phymod_core_access_t temp_core;
    portmod_pm4x10q_user_data_t *user_data;

    user_data = (portmod_pm4x10q_user_data_t*)core->access.user_acc;

    temp_core.access = user_data->pm4x10_access;

    SOC_IF_ERROR_RETURN(pm4x10_default_fw_loader(&temp_core, length, data));

    return SOC_E_NONE;
}

STATIC int
_pm4x10q_indacc_wait(int unit, int blk_id,  soc_field_t fwait)
{
    uint32 ctlsts;
    int poll = 1000;

    do {
        SOC_IF_ERROR_RETURN(READ_CHIP_INDACC_CTLSTSr(unit, blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, &ctlsts));
        if (soc_reg_field_get(unit, CHIP_INDACC_CTLSTSr, ctlsts, fwait)) {
            break;
        }
    } while (--poll > 0);

    if (poll <= 0) {
        return SOC_E_TIMEOUT;
    }
    return SOC_E_NONE;
}


STATIC int
_pm4x10q_indacc_gport_get(uint32 phy_reg, int *target_select)
{
    if (QSGMII_REG_ADDR_LANE(phy_reg) <= 15) {
        *target_select = (QSGMII_REG_ADDR_LANE(phy_reg) < 8) ? 0 : 1;
        return SOC_E_NONE;
    }
    
    return SOC_E_INTERNAL;
}


STATIC
int
_pm4x10q_indacc_write(portmod_default_user_access_t *user_data, uint32_t reg_addr, uint32_t val)
{
    int unit  = user_data->unit;
    int gport = 0;
    uint32 ctlsts;

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_gport_get(reg_addr, &gport));
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_WDATAr(unit, user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, val));
    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, gport);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, WR_REQf, 1);
    /*CL22 style*/
    reg_addr = reg_addr & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit, user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, ctlsts));

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_wait(unit, user_data->blk_id , WR_RDYf));
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit,user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK,  0));

    return SOC_E_NONE;
}


STATIC
int
_pm4x10q_indacc_read(portmod_default_user_access_t *user_data, uint32_t reg_addr, uint32_t *val)
{
    int unit  = user_data->unit;
    int gport = 0;
    uint32 ctlsts;

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_gport_get(reg_addr, &gport));
    ctlsts = 0;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, TARGET_SELECTf, gport);
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, RD_REQf, 1);
    /*CL22 style*/
    reg_addr = reg_addr & 0x1f;
    soc_reg_field_set(unit, CHIP_INDACC_CTLSTSr, &ctlsts, ADDRESSf, reg_addr);
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit, user_data->blk_id  | SOC_REG_ADDR_BLOCK_ID_MASK, ctlsts));

    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_wait(unit, user_data->blk_id , RD_RDYf));
    SOC_IF_ERROR_RETURN(READ_CHIP_INDACC_RDATAr(unit, user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK, val));
    SOC_IF_ERROR_RETURN(WRITE_CHIP_INDACC_CTLSTSr(unit,user_data->blk_id  | SOC_REG_ADDR_BLOCK_ID_MASK,  0));

    return SOC_E_NONE;
}


STATIC int
_pm4x10q_sbus_qsgmii_write(portmod_default_user_access_t *user_data, uint32_t phy_reg, uint32_t val)
{
    uint32  reg_addr, reg_data;

    /* The "phy_reg" in Sbus MDIO access is expected in 32 bits PHY address 
     *  format with AER information included. Since this interface allows  
     *  MDIO access in Claue22 only, the AER process must be applied.
     */

    /* AER process : AER block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = 0xffd0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* read data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, val));
    return SOC_E_NONE;
}


STATIC int
_pm4x10q_sbus_qsgmii_read(portmod_default_user_access_t *user_data, uint32_t phy_reg, uint32_t *val)
{
    uint32  reg_addr, reg_data;

    /* The "phy_reg" in Sbus MDIO access is expected in 32 bits PHY address 
     *  format with AER information included. Since this interface allows  
     *  MDIO access in Claue22 only, the AER process must be applied.
     */

    /* AER process : AER block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = 0xffd0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* AER process : lane control */
    reg_addr = 0x1e;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    /* lane control to indicated lane 0~7 for each QSGMIMI core */
    reg_data = QSGMII_REG_ADDR_LANE(phy_reg) & 0x7;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* target register block selection */
    reg_addr = 0x1f;
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    reg_data = phy_reg & 0xfff0;
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_write(user_data, reg_addr, reg_data));

    /* read data */
    reg_addr = QSGMII_REG_ADDR_REG(phy_reg);
    QSGMII_REG_ADDR_LANE_SET(reg_addr, phy_reg);
    SOC_IF_ERROR_RETURN(_pm4x10q_indacc_read(user_data, reg_addr, val));
    return SOC_E_NONE;
}

STATIC int
_pm4x10q_is_pcs_reg(uint32_t reg_addr, int *is_pcs)
{
    *is_pcs = (PHY_REG_ADDR_DEVAD(reg_addr) == 0) && ((IS_QSGMII_REGISTER(reg_addr)) || (QSGMII_REG_ADDR_REG(reg_addr) < 0x10));
    return SOC_E_NONE;
}

STATIC int
_pm4x10q_sbus_pm4x10_sbus_set(void *user_acc, phymod_bus_t* pm4x10_bus)
{
    portmod_pm4x10q_user_data_t *user_data = user_acc;

    user_data->pm4x10_access.bus = pm4x10_bus;
    return SOC_E_NONE;
}

STATIC int
_pm4x10q_sbus_reg_write(void *user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    portmod_pm4x10q_user_data_t *user_data = user_acc;
    int is_pcs = 0;

    if (user_data->pm4x10_access.bus == NULL) {
        SOC_IF_ERROR_RETURN(_pm4x10q_sbus_pm4x10_sbus_set(user_acc, &pm4x10_default_bus));
    }

    SOC_IF_ERROR_RETURN(_pm4x10q_is_pcs_reg(reg_addr, &is_pcs));
    if(is_pcs){
        SOC_IF_ERROR_RETURN(portmod_common_mutex_take((void*)&user_data->qsgmiie_user_data));
        SOC_IF_ERROR_RETURN(_pm4x10q_sbus_qsgmii_write(&user_data->qsgmiie_user_data, reg_addr, val));
        SOC_IF_ERROR_RETURN(portmod_common_mutex_give((void*)&user_data->qsgmiie_user_data));
        return SOC_E_NONE;
    } 
    /* coverity[var_deref_op:FALSE] */
    return user_data->pm4x10_access.bus->write(user_data->pm4x10_access.user_acc, user_data->pm4x10_access.addr, reg_addr, val);
}


STATIC int
_pm4x10q_sbus_reg_read(void *user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    portmod_pm4x10q_user_data_t *user_data = user_acc;
    int is_pcs = 0;

    if (user_data->pm4x10_access.bus == NULL) {
        SOC_IF_ERROR_RETURN(_pm4x10q_sbus_pm4x10_sbus_set(user_acc, &pm4x10_default_bus));
    }

    SOC_IF_ERROR_RETURN(_pm4x10q_is_pcs_reg(reg_addr, &is_pcs)); 
    if(is_pcs){
        SOC_IF_ERROR_RETURN(portmod_common_mutex_take((void*)&user_data->qsgmiie_user_data));
        SOC_IF_ERROR_RETURN(_pm4x10q_sbus_qsgmii_read(&user_data->qsgmiie_user_data, reg_addr, val));
        SOC_IF_ERROR_RETURN(portmod_common_mutex_give((void*)&user_data->qsgmiie_user_data));
        return SOC_E_NONE;
    } 
    /* coverity[var_deref_op:FALSE] */
    return user_data->pm4x10_access.bus->read(user_data->pm4x10_access.user_acc, user_data->pm4x10_access.addr, reg_addr, val);
}

STATIC int
    _pm4x10q_sbus_mutex_take(void *user_acc)
{
    portmod_pm4x10q_user_data_t *user_data;

    user_data = (portmod_pm4x10q_user_data_t*)user_acc;

    return portmod_common_mutex_take(&(user_data->qsgmiie_user_data));
}

STATIC int
    _pm4x10q_sbus_mutex_give(void *user_acc)
{
    portmod_pm4x10q_user_data_t *user_data;

    user_data = (portmod_pm4x10q_user_data_t*)user_acc;

    return portmod_common_mutex_give(&(user_data->qsgmiie_user_data));
}


phymod_bus_t pm4x10q_default_bus = {
    "PM4X10Q PCS Bus",
    _pm4x10q_sbus_reg_read,
    _pm4x10q_sbus_reg_write,
    NULL,
    _pm4x10q_sbus_mutex_take,
    _pm4x10q_sbus_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY| PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int pm4x10q_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv, i, j, invalid_port = -1;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x10Q", NULL, NULL, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, PM4X10Q_MAX_PORTS_PER_LANE, PM4X10Q_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_acc_ports", wb_buffer_index, sizeof(int), NULL, PM4X10Q_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phyAccPorts] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "num_ports_per_lane", wb_buffer_index, sizeof(int), NULL, PM4X10Q_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[numPortsPerLane] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
    if(!SOC_WARM_BOOT(unit)){
        for (i = 0; i < PM4X10Q_MAX_PORTS_PER_LANE; ++i) {
            for (j = 0; j < PM4X10Q_LANES_PER_CORE; ++j) {
                rv = SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &invalid_port, i, j); 
                _SOC_IF_ERR_EXIT(rv);
            }
        }

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], -1);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], 0);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN;
}


STATIC 
int _pm4x10q_sub_phy_get(int unit, soc_port_t port, pm_info_t pm_info, int *phy_index, int* sub_phy_index)
{
    uint32 phy_num_ports;
    int i, j, found = 0, rv, temp_port;
    SOC_INIT_FUNC_DEFS;

    *phy_index = -1;
    *sub_phy_index = -1;

    for (i = 0; i < PM4X10Q_LANES_PER_CORE && !found; ++i) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, i);
        _SOC_IF_ERR_EXIT(rv);
        for (j = 0; j < phy_num_ports; ++j) {
            rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &temp_port, i, j);
            _SOC_IF_ERR_EXIT(rv);
            if (temp_port == port) {
                *phy_index = i;
                *sub_phy_index = j;
                found = 1;
                break;
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    SOC_INIT_FUNC_DEFS;
    
    switch(interface){
    case SOC_PORT_IF_QSGMII:
        *is_supported = TRUE;
        break;
    default:
        *is_supported = FALSE;
    }

    SOC_FUNC_RETURN; 
}


int pm4x10Q_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x10q_create_info_internal_t *info = &pm_add_info->pm_specific_info.pm4x10q;
    pm4x10q_t pm4x10q_data = NULL;
    int rv;
    uint32 pm_is_active, pm_is_initialized;
    SOC_INIT_FUNC_DEFS;

    pm_info->unit = unit;
    pm_info->type = portmodDispatchTypePm4x10Q;
    pm_info->wb_buffer_id = wb_buffer_index;

    pm4x10q_data = sal_alloc(sizeof(struct pm4x10q_s), "pm4x10q_specific_db");
    SOC_NULL_CHECK(pm4x10q_data);
    pm_info->pm_data.pm4x10q_db = pm4x10q_data;

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x10q_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    PORTMOD_PBMP_ASSIGN(pm4x10q_data->phys, pm_add_info->phys);
    pm4x10q_data->pm4x10 = info->pm4x10;
    pm4x10q_data->is_initialized = 0;

    pm4x10q_data->is_bypass = 0;
    pm4x10q_data->blk_id = pm_add_info->pm_specific_info.pm4x10q.blk_id;
    pm4x10q_data->qsgmii_user_acc = info->qsgmii_user_acc;
    pm4x10q_data->pm4x10_user_acc = info->pm4x10_user_acc;
    pm4x10q_data->ref_clk = pm_add_info->pm_specific_info.pm4x10.ref_clk;
    pm4x10q_data->core_clock_khz = pm_add_info->pm_specific_info.pm4x10q.core_clock_khz;

    if(!SOC_WARM_BOOT(unit)){
        pm_is_active = 0;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_initialized = 0;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &pm_is_initialized);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    if(SOC_FUNC_ERROR){
        pm4x10Q_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}


int pm4x10Q_pm_destroy(int unit, pm_info_t pm_info)
{
        
    SOC_INIT_FUNC_DEFS;
    
    if(pm_info->pm_data.pm4x10q_db != NULL) {
        SOC_IF_ERROR_RETURN(pm4x10Q_pm_destroy(unit, pm_info->pm_data.pm4x10q_db->pm4x10));
        sal_free(pm_info->pm_data.pm4x10q_db);
        pm_info->pm_data.pm4x10q_db = NULL;
    }

    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
}


int pm4x10Q_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int phy, index, phy_num_ports, reg_port, rv, pm_is_active;
    uint32 reg_val, ilkn_rst = 0, phy_acc_port;
    portmod_bus_update_t pm_update;
    int flags;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_ITER(add_info->phys, phy){
        break;
    }
    index = (phy - 1) % 4;

    reg_port = PM_4x10Q_INFO(pm_info)->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    if (!PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, index);
        _SOC_IF_ERR_EXIT(rv);
        if (phy_acc_port == -1) {
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &port, index);
            _SOC_IF_ERR_EXIT(rv);
            phy_acc_port = port;
        }
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, index);
        _SOC_IF_ERR_EXIT(rv);
        if (phy_num_ports < 0) {
            phy_num_ports = 0;
        }
        rv = SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &port, index, phy_num_ports);
        _SOC_IF_ERR_EXIT(rv);
        phy_num_ports += 1;
        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, index);
        _SOC_IF_ERR_EXIT(rv);

        pm_update.default_bus = &pm4x10q_default_bus;
        pm_update.user_acc = PM_4x10Q_INFO(pm_info)->qsgmii_user_acc;
        pm_update.blk_id = -1; /*dont change the default blk_id*/
        pm_update.external_fw_loader = _pm4x10q_fw_loader;

        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_4x10Q_INFO(pm_info)->pm4x10, &pm_update));

        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
        if (pm_is_active == 0) {
            /* special init process for PMQ(PM4x10Q in QSGMII mode) - to release PMQ's QSGMII reset state after QSGMII-PCS and UniMAC init. */
            SOC_IF_ERROR_RETURN(READ_CHIP_CONFIGr(unit, reg_port, &reg_val));
            soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, QMODEf, 1);
            if ((PM_4x10Q_INFO(pm_info)->core_clock_khz > 325000) || (PM_4x10Q_INFO(pm_info)->core_clock_khz == 0)) {
                soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, IP_TDMf, 0x3);
            } else {
                /* According to PORTMACRO-210, when clk=300M or less, ip_tdm should be 2 or less */
                soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, IP_TDMf, 0x1);
            }
            SOC_IF_ERROR_RETURN(WRITE_CHIP_CONFIGr(unit, reg_port, reg_val));
            /* reset ILKN FIFO */
            _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, FLUSHf, 0x1);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, ILKN_BYPASS_RSTNf, 0);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_QSGMII_PCSf, 0x1);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT1f, 0x1);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT0f, 0x1);
            _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

            _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, FLUSHf, 0x0);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_QSGMII_PCSf, 0);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT1f, 0);
            soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT0f, 0);
            _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

            pm_is_active = 1;
            rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
            _SOC_IF_ERR_EXIT(rv);
        }

        if (add_info->flags & PORTMOD_PORT_ADD_F_EGR_1588_TIMESTAMP_MODE_48BIT) {
            SOC_IF_ERROR_RETURN(READ_GPORT_MODE_REGr(unit, port, &reg_val));
            soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EGR_1588_TIMESTAMPING_MODEf, PM4X10Q_EGR_1588_TIMESTAMP_MODE_48BIT);
            SOC_IF_ERROR_RETURN(WRITE_GPORT_MODE_REGr(unit, port, reg_val));
        }

        /* Initialize mask for purging packet data received from the MAC */
        _SOC_IF_ERR_EXIT(WRITE_GPORT_RSV_MASKr(unit, port, 0x78));
        _SOC_IF_ERR_EXIT(WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, 0x78));

        _SOC_IF_ERR_EXIT(READ_GPORT_CONFIGr(unit, port, &reg_val));
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, CLR_CNTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, port, reg_val));

        /* Reset the clear-count bit after 64 clocks */
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, CLR_CNTf, 0);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_GPORT_CONFIGr(unit, port, &reg_val));
        soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, GPORT_ENf, 1);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_MODES_SELf, 0x1);
        soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_FWDf, 0);
        soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_OWRTf, 0);
        _SOC_IF_ERR_EXIT(WRITE_GPORT_MODE_REGr(unit, port, reg_val));

         /* init MAC */
        flags = 0;
        if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
            flags |= UNIMAC_INIT_F_RX_STRIP_CRC;
        }
        _SOC_IF_ERR_EXIT(unimac_init(unit, port, flags));

        if (phy_acc_port == port) { /* only done once per phy lane */
            if (PM_4x10Q_INFO(pm_info)->is_bypass == 0) {
                _SOC_IF_ERR_EXIT(pm4x10_pm_bypass_set(unit, PM_4x10Q_INFO(pm_info)->pm4x10, 1));
                PM_4x10Q_INFO(pm_info)->is_bypass = 1;
            }
            _SOC_IF_ERR_EXIT(pm4x10_port_attach(unit, port, PM_4x10Q_INFO(pm_info)->pm4x10, add_info));
        }
    }else {
        SOC_IF_ERROR_RETURN(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
        ilkn_rst = soc_reg_field_get(unit, CHIP_SWRSTr, reg_val, ILKN_BYPASS_RSTNf);
        /* coverity[negative_shift:FALSE] */
        ilkn_rst |= 0x1 << index;
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, ILKN_BYPASS_RSTNf, ilkn_rst);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));
    }
exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable, phy_index, sub_phy, total_ports_left, i, reg_port, invalid_port = -1, temp_port, rv, pm_is_active;
    uint32 reg_val, phy_num_ports, phy_acc_port, ilkn_rst = 0;
    portmod_bus_update_t pm_update;
    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(pm4x10Q_port_enable_get(unit, port, pm_info, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    _SOC_IF_ERR_EXIT(_pm4x10q_sub_phy_get(unit, port, pm_info, &phy_index, &sub_phy));

    if (phy_index == -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("port %d was not found in internal DB"), port));
    }

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, phy_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, phy_index);
    _SOC_IF_ERR_EXIT(rv);
    phy_num_ports -= 1;

    reg_port = PM_4x10Q_INFO(pm_info)->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    if (phy_num_ports == 0) {

        _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
        ilkn_rst = soc_reg_field_get(unit, CHIP_SWRSTr, reg_val, ILKN_BYPASS_RSTNf);
        /* coverity[negative_shift:FALSE] */
        ilkn_rst &= (~(0x1 << phy_index));
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, ILKN_BYPASS_RSTNf, ilkn_rst);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

        _SOC_IF_ERR_EXIT(pm4x10_port_detach(unit, phy_acc_port, PM_4x10Q_INFO(pm_info)->pm4x10));

        PM_4x10Q_INFO(pm_info)->is_bypass = 0;        

        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &invalid_port, phy_index);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* deinit MAC */
    _SOC_IF_ERR_EXIT(unimac_deinit(unit, port));

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, &reg_val));
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_MODES_SELf, 0);
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_FWDf, 0);
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_OWRTf, 0);
    _SOC_IF_ERR_EXIT(WRITE_GPORT_MODE_REGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_GPORT_CONFIGr(unit, port, &reg_val));
    soc_reg_field_set(unit, GPORT_CONFIGr, &reg_val, GPORT_ENf, 0);
    _SOC_IF_ERR_EXIT(WRITE_GPORT_CONFIGr(unit, port, reg_val));

    /* Initialize mask for purging packet data received from the MAC */
   _SOC_IF_ERR_EXIT(WRITE_GPORT_RSV_MASKr(unit, port, 0x70)); 
    _SOC_IF_ERR_EXIT(WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, 0x70));

    /* Disable - clean db */
    total_ports_left = 0;
    for (i = 0; i < PM4X10Q_LANES_PER_CORE; ++i) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, i);
        _SOC_IF_ERR_EXIT(rv);
        total_ports_left += phy_num_ports;
    }
    if (total_ports_left == 1) { /*last one*/

        pm_is_active = 0;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        /* reset ILKN FIFO */

        _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, ILKN_BYPASS_RSTNf, 0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, FLUSHf, 0x1);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_QSGMII_PCSf, 0x0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT1f, 0x0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT0f, 0x0);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

        _SOC_IF_ERR_EXIT(READ_CHIP_CONFIGr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, QMODEf, 0);
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, IP_TDMf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_CONFIGr(unit, reg_port, reg_val));

        PM_4x10Q_INFO(pm_info)->is_initialized = 0;

        /*restore bus and fw_loader*/
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_4x10Q_INFO(pm_info)->pm4x10, NULL));

        /*restore user access*/
        pm_update.default_bus = NULL;
        pm_update.user_acc = PM_4x10Q_INFO(pm_info)->pm4x10_user_acc;
        pm_update.blk_id = -1; /*dont change the default blk_id*/
        pm_update.external_fw_loader = NULL;
        _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_4x10Q_INFO(pm_info)->pm4x10, &pm_update));

        _SOC_IF_ERR_EXIT(pm4x10_pm_bypass_set(unit, PM_4x10Q_INFO(pm_info)->pm4x10, 0));
    }

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EGR_1588_TIMESTAMPING_MODEf, 0x1);
    _SOC_IF_ERR_EXIT(WRITE_GPORT_MODE_REGr(unit, port, reg_val));

    

    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, phy_index);
    _SOC_IF_ERR_EXIT(rv);
    phy_num_ports -= 1;

    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[numPortsPerLane], &phy_num_ports, phy_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &invalid_port, phy_index, sub_phy);
    _SOC_IF_ERR_EXIT(rv);
    while (sub_phy < phy_num_ports) {
        rv = SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &temp_port, phy_index, sub_phy + 1);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &temp_port, phy_index, sub_phy);
        _SOC_IF_ERR_EXIT(rv);
        ++sub_phy;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

        
    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int     actual_flags = flags, nof_phys = 0;
    phymod_phy_power_t           phy_power;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 phychain_flag;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    SOC_INIT_FUNC_DEFS;

    /* If no Rx/Tx flags - set both */
    if ((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))) {
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* If no Mac/Phy flags - set both */
    if ((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))) {
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    /* if MAC is set and either RX or TX set is invalid combination */
    if( (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) ) {
        if((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }

    /* phychain_flag is meant to be used if we want only part of the phychin to be affected*/
    phychain_flag = PORTMOD_PORT_ENABLE_INTERNAL_PHY_ONLY_GET(flags) ? 
        PORTMOD_INIT_F_INTERNAL_SERDES_ONLY : 
        PORTMOD_INIT_F_EXTERNAL_MOST_ONLY ;

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    phy_power.rx = phymodPowerNoChange;
    phy_power.tx = phymodPowerNoChange;

    if(PORTMOD_PORT_ENABLE_RX_GET(actual_flags)){
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
        rx_control = (enable) ? phymodRxSquelchOff : phymodRxSquelchOn;
    }
    if(PORTMOD_PORT_ENABLE_TX_GET(actual_flags)){
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
        tx_control = (enable) ? phymodTxSquelchOff : phymodTxSquelchOn;
    }

    if (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)){
        _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, 0, enable));

    } 
    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        if (enable) {
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, phychain_flag, &phy_power));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_tx_lane_control_set(unit, port, phy_access, nof_phys, phychain_flag, tx_control));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_rx_lane_control_set(unit, port, phy_access, nof_phys, phychain_flag, rx_control));
        } else {
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_tx_lane_control_set(unit, port, phy_access, nof_phys, phychain_flag, tx_control));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_rx_lane_control_set(unit, port, phy_access, nof_phys, phychain_flag, rx_control));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, phychain_flag, &phy_power));
        }
    }

exit:
    SOC_FUNC_RETURN; 
}

int pm4x10Q_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    int phy_enable = 1, mac_enable = 1, mac_rx_enable, mac_tx_enable;
    int actual_flags = flags;
    uint32 phychain_flag;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    /* phychain_flag is meant to be used if we want only part of the phychin to be affected*/
    phychain_flag = PORTMOD_PORT_ENABLE_INTERNAL_PHY_ONLY_GET(flags) ? 
        PORTMOD_INIT_F_INTERNAL_SERDES_ONLY : 
        PORTMOD_INIT_F_EXTERNAL_MOST_ONLY ;

    if (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) {
       mac_enable = 0;

       if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
           _SOC_IF_ERR_EXIT(unimac_enable_get(unit, port, UNIMAC_ENABLE_SET_FLAGS_RX_EN, &mac_rx_enable));
           mac_enable |= (mac_rx_enable) ? 1 : 0;
       }
       if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
           _SOC_IF_ERR_EXIT(unimac_enable_get(unit, port, UNIMAC_ENABLE_SET_FLAGS_TX_EN, &mac_tx_enable));
           mac_enable |= (mac_tx_enable) ? 1 : 0;
       }

    }

    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_tx_lane_control_get(unit, port, phy_access, nof_phys, phychain_flag, &tx_control));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_rx_lane_control_get(unit, port, phy_access, nof_phys, phychain_flag, &rx_control));

        phy_enable = 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            phy_enable |= (rx_control == phymodRxSquelchOn) ? 0 : 1;
        } 
        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            phy_enable |= (tx_control == phymodTxSquelchOn) ? 0 : 1;
        }
    } 

    *enable = (mac_enable && phy_enable);

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_inf_config_t     phy_interface_config;
    soc_port_if_t        interface = SOC_PORT_IF_NULL;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    params.lane = -1;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(pm4x10Q_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,  config->speed,
                    interface, &phy_interface_config.interface_type));

    phy_interface_config.data_rate       = config->speed;
    phy_interface_config.interface_modes = config->interface_modes;
    phy_interface_config.ref_clock       = PM_4x10Q_INFO(pm_info)->ref_clk;
    _SOC_IF_ERR_EXIT(phymod_phy_interface_config_set(&phy_access, 0, &phy_interface_config));

    _SOC_IF_ERR_EXIT(unimac_speed_set(unit, port, phy_interface_config.data_rate));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    int nof_phys;
    phymod_phy_inf_config_t     phy_interface_config;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));
    _SOC_IF_ERR_EXIT(phymod_phy_interface_config_get(phy_access, 0, PM_4x10Q_INFO(pm_info)->ref_clk, &phy_interface_config));

    config->speed = phy_interface_config.data_rate;

    _SOC_IF_ERR_EXIT(portmod_intf_from_phymod_intf(unit, phy_interface_config.interface_type, &config->interface));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    *interface = SOC_PORT_IF_NULL;
    return SOC_E_NONE;
}

int pm4x10Q_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys; 
    phymod_loopback_mode_t phymod_lb_type;
    SOC_INIT_FUNC_DEFS;
    
    switch (loopback_type) {
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(unimac_loopback_set(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:
        _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit, 
                                                loopback_type, &phymod_lb_type));
            
        _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        _SOC_IF_ERR_EXIT(portmod_port_phychain_loopback_set(unit, port, phy_access, 
                                                            nof_phys, 
                                                            phymod_lb_type, 
                                                            enable));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    int rv, nof_phys;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32_t tmp_enable=0;
    phymod_loopback_mode_t phymod_lb_type;
    SOC_INIT_FUNC_DEFS;
    
    switch (loopback_type) {
    case portmodLoopbackMacOuter:
       _SOC_IF_ERR_EXIT(unimac_loopback_get(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:
        _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit, 
                                                loopback_type, &phymod_lb_type));
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        rv = portmod_port_phychain_loopback_get(unit, port, phy_access,
                                            nof_phys,
                                            phymod_lb_type,
                                            &tmp_enable);
        if (rv == PHYMOD_E_UNAVAIL) {
            rv = PHYMOD_E_NONE;
            tmp_enable = 0;
        }

        _SOC_IF_ERR_EXIT(rv);
        *enable = tmp_enable;

        break;
    default:
        (*enable) = 0; 
    }

exit:
    SOC_FUNC_RETURN;    
}


int pm4x10Q_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
    int rv;
    int phy_index, sub_phy;
    soc_port_t phy_acc_port;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10q_sub_phy_get(unit, port, pm_info, &phy_index, &sub_phy));

    /* call pm4x10 below only for phys*/
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, phy_index);
    _SOC_IF_ERR_EXIT(rv);
    
    rv = pm4x10_port_core_access_get(unit, phy_acc_port, PM_4x10Q_INFO(pm_info)->pm4x10, phyn, max_cores, core_access_arr, nof_cores, is_most_ext);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{
    int rv;
    int phy_index, sub_phy;
    soc_port_t phy_acc_port;
    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(_pm4x10q_sub_phy_get(unit, port, pm_info, &phy_index, &sub_phy));

    /* call pm4x10 below only for phys*/
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, phy_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = pm4x10_port_phy_lane_access_get(unit, phy_acc_port, PM_4x10Q_INFO(pm_info)->pm4x10, params, max_phys, access, nof_phys, is_most_ext);
    _SOC_IF_ERR_EXIT(rv);

    access[0].access.lane_mask = 1 << ((phy_index * 4) + sub_phy);

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_tx_average_ipg_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_tx_average_ipg_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_tx_average_ipg_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_tx_average_ipg_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_encap_set(int unit, int port, pm_info_t pm_info, int flags, portmod_encap_t encap)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_encap_set(unit, port, encap));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_encap_get(int unit, int port, pm_info_t pm_info, int *flags, portmod_encap_t *encap)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_encap_get(unit, port, encap));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_eee_set(unit, port, eee));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_eee_get(int unit, int port, pm_info_t pm_info, portmod_eee_t* eee)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_eee_get(unit, port, eee));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_speed_get(unit, port, speed));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_pfc_config_set(int unit, int port,pm_info_t pm_info, const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_pfc_config_set(unit, port, pfc_cfg));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_pfc_config_get(int unit, int port,pm_info_t pm_info, portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_pfc_config_get(unit, port, pfc_cfg));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_pause_control_set(int unit, int port, pm_info_t pm_info, const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_pause_control_set(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_pause_control_get(int unit, int port, pm_info_t pm_info, portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_pause_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_duplex_set(unit, port, enable)); 

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_duplex_get(unit, port, enable)); 

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_GPORT_CNTMAXSIZEr(unit, port, (uint32_t *)val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(WRITE_GPORT_CNTMAXSIZEr(unit, port, (uint32_t)val));

exit:
    SOC_FUNC_RETURN;
}


int
pm4x10Q_default_bus_update(int unit, pm_info_t pm_info, const portmod_bus_update_t* update)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_default_bus_update(unit, PM_4x10Q_INFO(pm_info)->pm4x10, update));
exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_local_fault_control_set(int unit, int port, pm_info_t pm_info, const portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_local_fault_control_get(int unit, int port, pm_info_t pm_info, portmod_local_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pm4x10Q_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_port_local_fault_status_clear(unit, port, PM_4x10Q_INFO(pm_info)->pm4x10));
exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info, const portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info, portmod_remote_fault_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pm4x10Q_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_port_remote_fault_status_clear(unit, port, PM_4x10Q_INFO(pm_info)->pm4x10));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_rx_control_set(int unit, int port, pm_info_t pm_info,const portmod_rx_control_t* rx_ctrl)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_pass_control_frame_set(unit, port, rx_ctrl->pass_control_frames));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pm4x10Q_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pm4x10Q_port_diag_fifo_status_get (int unit, int port,pm_info_t pm_info, const portmod_fifo_status_t* diag_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_diag_fifo_status_get(unit, port, diag_info));
exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_drv_name_get(int unit, int port, pm_info_t pm_info, char* buf, int len)
{
    strncpy(buf, "PM4X10Q Driver", len);
    return (SOC_E_NONE);
}

int pm4x10Q_port_max_packet_size_set (int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_max_size_set(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_max_packet_size_get (int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_max_size_get(unit, port, value));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10Q_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_enable_get(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10Q_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,const portmod_vlan_tag_t* vlan_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_vlan_tag_set(unit, port, vlan_tag->outer_vlan_tag, vlan_tag->inner_vlan_tag));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10Q_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_rx_vlan_tag_get(unit, port, (int*)&vlan_tag->outer_vlan_tag, (int*)&vlan_tag->inner_vlan_tag));

exit:
    SOC_FUNC_RETURN;

}

int pm4x10Q_port_pfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(unimac_pfc_control_set(unit, port, control));        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(unimac_pfc_control_get(unit, port, control));     
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_llfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_llfc_control_get(int unit, int port, pm_info_t pm_info, portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pm4x10Q_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    int rv;
    int phy_index, sub_phy;
    soc_port_t phy_acc_port;

    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(_pm4x10q_sub_phy_get(unit, port, pm_info, &phy_index, &sub_phy));

    /* call pm4x10 below only for phys*/
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, phy_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT(portmod_port_phychain_link_status_get(unit, port, phy_access, nof_phys, flags, (uint32_t*) link));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, const phymod_autoneg_control_t* an)
{
    int rv;
    int phy_index, sub_phy;
    soc_port_t phy_acc_port;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10q_sub_phy_get(unit, port, pm_info, &phy_index, &sub_phy));

    /* call pm4x10 below only for phys*/
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, phy_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = pm4x10_port_autoneg_set(unit, phy_acc_port, PM_4x10Q_INFO(pm_info)->pm4x10, phy_flags, an);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an)
{
    int rv;
    int phy_index, sub_phy;
    soc_port_t phy_acc_port;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10q_sub_phy_get(unit, port, pm_info, &phy_index, &sub_phy));

    /* call pm4x10 below only for phys*/
    rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyAccPorts], &phy_acc_port, phy_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = pm4x10_port_autoneg_get(unit, phy_acc_port, PM_4x10Q_INFO(pm_info)->pm4x10, phy_flags, an);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (unimac_mac_sa_set(unit, port, mac_addr));
}


int pm4x10Q_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (unimac_mac_sa_get(unit, port, mac_addr));
}

int pm4x10Q_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (unimac_mac_sa_set(unit, port, mac_addr));
}


int pm4x10Q_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (unimac_mac_sa_get(unit, port, mac_addr));
}

int pm4x10Q_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{        
    SOC_INIT_FUNC_DEFS;
    
    /*Unimac tx padding is always enabled*/
    SOC_FUNC_RETURN; 
    
}

int pm4x10Q_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /*Unimac tx padding is always enabled and min frame size is 64*/
    *value = 64;
        
exit:
    SOC_FUNC_RETURN;     
}

int pm4x10Q_port_update(int unit, int port, pm_info_t pm_info,
                        const portmod_port_update_control_t* update_control)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pm4x10Q_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int *nof_lanes)
{
    SOC_INIT_FUNC_DEFS;

    /*QSGMII ports always occupy only one lane*/
    *nof_lanes = 1;

    SOC_FUNC_RETURN;
}

int pm4x10Q_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_tx_t          ln_txparam[PM4X10Q_LANES_PER_CORE];
    int nof_phys, i;
    uint32 lane_map;
    portmod_access_get_params_t params;

    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

    for (i = 0; i < PM4X10Q_LANES_PER_CORE; i++) {
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(&ln_txparam[i]));
    }

    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { 
        /* most external PHY_DIAG_DEV_DFLT and PHY_DIAG_DEV_EXT */  
        params.phyn = -1 ;  
    }

    if(PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS ){
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* line side is default */
        params.sys_side = PORTMOD_SIDE_LINE;
    }

    params.apply_lane_mask = 1;

    SOC_IF_ERROR_RETURN(pm4x10Q_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, phy_access, &nof_phys, NULL));

    lane_map = phy_access[0].access.lane_mask;

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_DSC));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pmd_info_dump(unit, phy_access, nof_phys,
                                                 (void*)arg));
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
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys, op_cmd, ln_txparam, lane_map, PTR_TO_INT(arg)));
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
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,op_cmd, ln_txparam, lane_map, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                return (SOC_E_UNAVAIL);
            }
            break ;
    }
    return (SOC_E_NONE);
}

int pm4x10Q_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{   
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    
    SOC_INIT_FUNC_DEFS; 

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
    if(nof_phys == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Autoneg isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_status_get(unit, port, phy_access, nof_phys, an_status));

exit:
    SOC_FUNC_RETURN;
    
}

int pm4x10Q_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_phy_link_up_event(unit, port, phy_access, nof_phys));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10Q_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_phy_link_down_event(unit, port, phy_access, nof_phys));
exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_ability_local_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    SOC_INIT_FUNC_DEFS;

    ability->speed_full_duplex = SOC_PA_SPEED_2500MB | SOC_PA_SPEED_1000MB | SOC_PA_SPEED_100MB | SOC_PA_SPEED_10MB;
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->medium    = SOC_PA_MEDIUM_FIBER | SOC_PA_MEDIUM_COPPER;
    ability->interface = SOC_PA_INTF_QSGMII;
    /* QSGMII doesn't support PHY loopback per subport */
    ability->loopback  = SOC_PA_LB_MAC;
    ability->fec       = SOC_PA_FEC_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE;
    ability->channel   = SOC_PA_CHANNEL_LONG;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    /* Autoneg not supported for QSGMII ports */
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    /* Autoneg not supported for QSGMII ports */
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_mode_set(int unit, int port, pm_info_t pm_info, const portmod_port_mode_info_t* mode)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_mode_get(int unit, int port, pm_info_t pm_info, portmod_port_mode_info_t* mode)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_mac_reset_check(int unit, int port, pm_info_t pm_info, int enable, int* reset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_soft_reset_get(unit, port, reset));

    *reset = (enable == *reset) ? (enable ? 0 : 1) : 1;

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_check_legacy_phy(int unit, int port, pm_info_t pm_info, int* legacy_phy)
{
    SOC_INIT_FUNC_DEFS;

    *legacy_phy = 0;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_core_num_get(int unit, int port, pm_info_t pm_info, int* core_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_port_core_num_get(unit, port, PM_4x10Q_INFO(pm_info)->pm4x10, core_num));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_ref_clk_get(int unit, int port, pm_info_t pm_info, int* ref_clk)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_port_ref_clk_get(unit, port, PM_4x10Q_INFO(pm_info)->pm4x10, ref_clk));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx, int val, int flags)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_soft_reset_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_drain_cells_rx_enable(int unit, int port, pm_info_t pm_info, int enable)
{
    int flags;
    SOC_INIT_FUNC_DEFS;

    flags = UNIMAC_ENABLE_SET_FLAGS_RX_EN | UNIMAC_ENABLE_SET_FLAGS_TX_EN;

    _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    int flags;
    SOC_INIT_FUNC_DEFS;

    flags = UNIMAC_ENABLE_SET_FLAGS_TX_EN;

    _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, flags, 0));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_txfifo_cell_cnt_get(int unit, int port, pm_info_t pm_info, uint32* cnt)
{
    SOC_INIT_FUNC_DEFS;

    *cnt = 0;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_drain_cell_get(int unit, int port, pm_info_t pm_info, portmod_drain_cells_t* drain_cells)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(unimac_drain_cell_get(unit, port, drain_cells));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_drain_cell_stop(int unit, int port, pm_info_t pm_info, const portmod_drain_cells_t* drain_cells)
{
    
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_port_lag_failover_status_toggle(int unit, int port, pm_info_t pm_info)
{
    /* Lag failover not supported */
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int
pm4x10Q_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                           portmod_egr_1588_timestamp_config_t timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    SOC_INIT_FUNC_DEFS;

    if (timestamp_config.timestamp_mode == portmodTimestampMode32bit) {
        timestamp_mode = PM4X10Q_EGR_1588_TIMESTAMP_MODE_32BIT;
    } else if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
        timestamp_mode = PM4X10Q_EGR_1588_TIMESTAMP_MODE_48BIT;
    } else {
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, &reg_val));

    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val,
                      EGR_1588_TIMESTAMPING_MODEf, timestamp_mode);
    _SOC_IF_ERR_EXIT(WRITE_GPORT_MODE_REGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10Q_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                         portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, &reg_val));
    timestamp_mode = soc_reg_field_get(unit, GPORT_MODE_REGr, reg_val,
                                       EGR_1588_TIMESTAMPING_MODEf);

    if (timestamp_mode == PM4X10Q_EGR_1588_TIMESTAMP_MODE_32BIT) {
        timestamp_config->timestamp_mode = portmodTimestampMode32bit;
    } else {
        timestamp_config->timestamp_mode = portmodTimestampMode48bit;
    }

    timestamp_config->cmic_48_overr_en = 0;

exit:
    SOC_FUNC_RETURN;
}
#endif /* PORTMOD_PM4x10Q_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
