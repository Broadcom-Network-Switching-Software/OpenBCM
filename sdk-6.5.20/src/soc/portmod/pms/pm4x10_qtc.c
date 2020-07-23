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
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>

#include <soc/portmod/unimac.h>
#include <soc/portmod/pm4x10_qtc.h>
#include <soc/portmod/pm4x10.h>
#include <soc/portmod/pm_qtc.h>

#include <soc/portmod/portmod_system.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10_QTC_SUPPORT

#define PM_4x10_QTC_INFO(pm_info) ((pm_info)->pm_data.pm4x10_qtc_db)

#define PM4X10_QTC_LANES_PER_CORE          (4)
#define MAX_PORTS_PER_PM4X10_QTC   (16)
#define PM4X10_QTC_MAX_PORTS_PER_LANE      (4)

/* WB defines */
#define PM4x10_QTC_WB_BUFFER_VERSION        (2)

#define PM4x10_QTC_IS_INITIALIZED_SET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &is_initialized)
#define PM4x10_QTC_IS_INITIALIZED_GET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], is_initialized)

#define PM4x10_QTC_IS_ACTIVE_SET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &is_active)
#define PM4x10_QTC_IS_ACTIVE_GET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], is_active)

#define PM4x10_QTC_QTC_MODE_SET(unit, pm_info, qtc_mode) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[qtcMode], &qtc_mode)
#define PM4x10_QTC_QTC_MODE_GET(unit, pm_info, qtc_mode) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[qtcMode], qtc_mode)

#define PM4x10_QTC_ACTIVE_PORT_NUM_SET(unit, pm_info, active_port_num) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[activePortNum], &active_port_num)
#define PM4x10_QTC_ACTIVE_PORT_NUM_GET(unit, pm_info, active_port_num) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[activePortNum], active_port_num)

#define PM4x10_QTC_PORTS_SET(unit, pm_info, port, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], \
                        &port, port_index)
#define PM4x10_QTC_PORTS_GET(unit, pm_info, port, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], \
                        port, port_index)

#define PM4x10_QTC_PHY_TYPE_SET(unit, pm_info, type) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyType], &type)
#define PM4x10_QTC_PHY_TYPE_GET(unit, pm_info, type) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phyType], type)

#define PM4x10_QTC_SPEED_SET(unit, pm_info, speed, port_index)     \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,         \
                        pm_info->wb_vars_ids[dataRate], &speed, port_index)
#define PM4x10_QTC_SPEED_GET(unit, pm_info, speed, port_index)     \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,         \
                        pm_info->wb_vars_ids[dataRate], speed, port_index)

#define PM4x10_QTC_MAX_SPEED_SET(unit, pm_info, max_speed, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,             \
                        pm_info->wb_vars_ids[maxSpeed], &max_speed, port_index)
#define PM4x10_QTC_MAX_SPEED_GET(unit, pm_info, max_speed, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,             \
                        pm_info->wb_vars_ids[maxSpeed], max_speed, port_index)

#define PM4x10_QTC_INTERFACE_TYPE_SET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                        pm_info->wb_vars_ids[interfaceType],  &interface_type, port_index)
#define PM4x10_QTC_INTERFACE_TYPE_GET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                        pm_info->wb_vars_ids[interfaceType],  interface_type, port_index)

#define PM4x10_QTC_AN_MODE_SET(unit, pm_info, an_mode, port_index)               \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                        pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM4x10_QTC_AN_MODE_GET(unit, pm_info, an_mode, port_index)               \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                        pm_info->wb_vars_ids[anMode], an_mode, port_index)

#define PM4x10_QTC_PORT_LANE_CONNECTION_DB_SET(unit, pm_info, lane_connection_db, xphy_idx, port_index) \
    SOC_WB_ENGINE_SET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[laneConnectionDb], \
                        (uint8 *)lane_connection_db, port_index, xphy_idx)
#define PM4x10_QTC_PORT_LANE_CONNECTION_DB_GET(unit, pm_info, lane_connection_db, xphy_idx, port_index) \
    SOC_WB_ENGINE_GET_DBL_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[laneConnectionDb], \
                        (uint8 *)lane_connection_db, port_index, xphy_idx)

#define PM4x10_QTC_PORT_NUM_PHYS_SET(unit, pm_info, nof_phys, port_index)   \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[nofPhys], nof_phys, port_index)
#define PM4x10_QTC_PORT_NUM_PHYS_GET(unit, pm_info, nof_phys, port_index)   \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[nofPhys], nof_phys, port_index)

#define PM4x10_QTC_PORT_DYNAMIC_STATE_SET(unit, pm_info, dynamic_state, port_index)     \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                        pm_info->wb_vars_ids[portDynamicState], &dynamic_state, port_index)
#define PM4x10_QTC_PORT_DYNAMIC_STATE_GET(unit, pm_info, dynamic_state, port_index)     \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,              \
                        pm_info->wb_vars_ids[portDynamicState], dynamic_state, port_index)

#define PM4x10_QTC_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, \
                                            port_index)                          \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                              pm_info->wb_vars_ids[isInterfaceRestore],          \
                              &is_interface_restore, port_index)
#define PM4x10_QTC_IS_INTERFACE_RESTORE_GET(unit, pm_info, is_interface_restore, \
                                            port_index)                          \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                       \
                              pm_info->wb_vars_ids[isInterfaceRestore],          \
                              is_interface_restore, port_index)

typedef enum pm4x10_qtc_wb_vars{
    isInitialized,
    isActive,
    qtcMode,
    activePortNum, /* Active port number of the PM4x10_QTC core*/
    phyType,
    ports,
    dataRate, /* port speed - phy_intf_config.data_rate */
    maxSpeed, /* Max Port Speed Supported */
    interfaceType, /* interface of type soc_port_if_t */
    anMode,
    laneConnectionDb, /* represents the physical topology of the  serdes and ext phy lanes */
    nofPhys, /* number of phys for the port */
    portDynamicState,
    isInterfaceRestore /*indicate whether interface should be restored from DB*/
}pm4x10_qtc_wb_vars_t;

struct pm4x10_qtc_s{
    pm_info_t pm4x10;
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access;
    int nof_phys[MAX_PORTS_PER_PM4X10_QTC]; /* internal + External Phys for each lane*/
    portmod_xphy_lane_connection_t lane_conn[MAX_PHYN][MAX_PORTS_PER_PM4X10_QTC];
    uint8 default_fw_loader_is_used;
    uint8 default_bus_is_used;
    portmod_phy_external_reset_f  portmod_phy_external_reset;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    int core_num;
    uint32 core_clock_khz;
    int warmboot_skip_db_restore;
};

#define PM4x10_INFO(pm_info)    PM_4x10_QTC_INFO(pm_info)->pm4x10

#define PM4x10_QTC_LOCAL_VARIABLE_DEFS  \
     portmod_qtc_mode_t qtc_mode = portmodQtcModeInvalid

#define PM4x10_QTC_QMOD_GET(pm_info) \
    do { \
        _SOC_IF_ERR_EXIT(PM4x10_QTC_QTC_MODE_GET(unit, pm_info, &qtc_mode)); \
    } while (0)

#define IS_QMOD  ((qtc_mode == portmodQtcModeQsgmii) || \
                  (qtc_mode == portmodQtcModeUsxgmii))

#define PM4x10_QTC_CORE_ACTIVE_GET(core_is_active)  \
    do { \
        core_is_active = 0; \
        _SOC_IF_ERR_EXIT(PM4x10_QTC_ACTIVE_PORT_NUM_GET(unit, pm_info, &active_port_num)); \
        if (active_port_num) { \
            core_is_active = 1; \
        } \
    } while (0)

#define PORTMOD_PM4x10_QTC_IS_GEN1(pm_version)             ((pm_version.type == PM_TYPE_PM4X10Q) && (pm_version.gen == PM_GEN1))
#define PORTMOD_PM4x10_QTC_IS_GEN1_GEN2(pm_version)        ((pm_version.type == PM_TYPE_PM4X10Q) && ((pm_version.gen == PM_GEN1) || (pm_version.gen == PM_GEN2)))
#define PORTMOD_PM4x10_QTC_IS_GEN2_16nm(pm_version)        ((pm_version.type == PM_TYPE_PM4X10Q) && (pm_version.gen == PM_GEN2) && (pm_version.tech_process == PM_TECH_16NM))
#define PORTMOD_PM4x10_QTC_IS_GEN2_28nm(pm_version)        ((pm_version.type == PM_TYPE_PM4X10Q) && (pm_version.gen == PM_GEN2) && (pm_version.tech_process == PM_TECH_28NM))
#define PORTMOD_PM4x10_QTC_REVISION_IS_A0(pm_version)      ((pm_version.revision_letter == PM_REVISION_A) && (pm_version.revision_number == PM_REVISION_0))

#define UNIMAC_MEM_INIT_DONE_MASK 0xffff
#define GP_GMIB_MEM_INIT_DONE_MASK 0xf0000

#define PM4X10_QTC_EGR_1588_TIMESTAMP_MODE_48BIT (0)
#define PM4X10_QTC_EGR_1588_TIMESTAMP_MODE_32BIT (1)

int _pm4x10_qtc_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, const phymod_autoneg_control_t* an);
int _pm4x10_qtc_port_ability_local_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability);
int _pm4x10_qtc_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability);

#if 0
STATIC
int _pm4x10_qtc_pm_version_get(int unit, int port, pm_info_t pm_info, pm_version_t *version)
{
    uint32 reg_val, pm_type;
    int reg_port;
    portmod_default_user_access_t *user_data;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)PM_4x10_QTC_INFO(pm_info)->int_core_access.access.user_acc;
    reg_port = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;
    version->type            = PM_TYPE_INVALID;
    version->gen             = PM_GEN_INVALID;
    version->tech_process    = PM_TECH_INVALID;
    version->revision_number = PM_REVISION_NUM_INVALID;
    version->revision_letter = PM_REVISION_LETTER_INVALID;

    /*
     * PM version ID register is not defined in PM GEN1.
     */
    if (SOC_REG_IS_VALID(unit, PMQ_PM_VERSION_IDr)) {
        _SOC_IF_ERR_EXIT(READ_PMQ_PM_VERSION_IDr(unit, reg_port, &reg_val));

        pm_type = soc_reg_field_get(unit, PMQ_PM_VERSION_IDr, reg_val, PM_TYPEf);
        if (pm_type == 3) {
            version->type = PM_TYPE_PM4X10Q;
            version->gen  = PM_GEN2;
        }
        /* The hardware value of techology/revision starts from 0 */
        version->tech_process    = soc_reg_field_get(unit, PMQ_PM_VERSION_IDr, reg_val, TECH_PROCESSf) +1;
        version->revision_number = soc_reg_field_get(unit, PMQ_PM_VERSION_IDr, reg_val, REV_NUMBERf) + 1;
        version->revision_letter = soc_reg_field_get(unit, PMQ_PM_VERSION_IDr, reg_val, REV_LETTERf) + 1;
    } else {
        version->type = PM_TYPE_PM4X10Q;
        version->gen  = PM_GEN1;
    }

exit:
    SOC_FUNC_RETURN;
}
#endif

int pm4x10_qtc_pm_interface_type_is_supported(int unit, soc_port_if_t interface,
                                          int* is_supported)
{
    SOC_INIT_FUNC_DEFS;
    switch(interface){
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_QSGMII:
        case SOC_PORT_IF_GMII:
            *is_supported = TRUE;
            break;
        default:
            *is_supported = FALSE;
            break;
    }
    if (*is_supported == FALSE) {
        _SOC_IF_ERR_EXIT(pm4x10_pm_interface_type_is_supported(unit, interface, is_supported));
    }

exit:
    SOC_FUNC_RETURN;
}

int portmod_pm4x10_qtc_wb_upgrade_func(int unit, void *arg, int recovered_version, int new_version)
{
    pm_info_t pm_info;
    SOC_INIT_FUNC_DEFS;

    pm_info = arg;
    PM_4x10_QTC_INFO(pm_info)->warmboot_skip_db_restore  = TRUE;

    SOC_FUNC_RETURN;
}

STATIC
int pm4x10_qtc_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    uint32 is_initialized, is_active;
    int qtc_mode, active_port_num, lane, phyn, phy_type;
    portmod_xphy_lane_connection_t lane_conn;
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x10_qtc",
                        portmod_pm4x10_qtc_wb_upgrade_func, pm_info,
                        PM4x10_QTC_WB_BUFFER_VERSION, 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized",
                        wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active",
                        wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "qtc_mode",
                        wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[qtcMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "active_port_num",
                        wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[activePortNum] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type",
                        wb_buffer_index, sizeof(int), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phyType] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports",
                        wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X10_QTC,
                        VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "data_rate",
                        wb_buffer_index, sizeof(int), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[dataRate] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "max_speed",
                        wb_buffer_index, sizeof(uint32), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[maxSpeed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_type",
                        wb_buffer_index, sizeof(uint32), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceType] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode",
                        wb_buffer_index, sizeof(uint32), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane_connection_db",
                        wb_buffer_index, sizeof(portmod_xphy_lane_connection_t), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, MAX_PHYN, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[laneConnectionDb] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "nof_phys",
                        wb_buffer_index, sizeof(int), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[nofPhys] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_dynamic_state",
                        wb_buffer_index, sizeof(uint32_t), NULL,
                        MAX_PORTS_PER_PM4X10_QTC, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portDynamicState] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "is_interface_restore", wb_buffer_index,
                          sizeof(uint32), NULL, MAX_PORTS_PER_PM4X10_QTC,
                          VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInterfaceRestore] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

    if(!SOC_WARM_BOOT(unit)){
        is_initialized = 0;
        _SOC_IF_ERR_EXIT(PM4x10_QTC_IS_INITIALIZED_SET(unit, pm_info, is_initialized));

        is_active = 0;
        _SOC_IF_ERR_EXIT(PM4x10_QTC_IS_ACTIVE_SET(unit, pm_info, is_active));

        qtc_mode = 0;
        _SOC_IF_ERR_EXIT(PM4x10_QTC_QTC_MODE_SET(unit, pm_info, qtc_mode));

        active_port_num = 0;
        _SOC_IF_ERR_EXIT(PM4x10_QTC_ACTIVE_PORT_NUM_SET(unit, pm_info, active_port_num));

        phy_type = 0;
        _SOC_IF_ERR_EXIT(PM4x10_QTC_PHY_TYPE_SET(unit, pm_info, phy_type));

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[ports], -1);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[dataRate], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[maxSpeed], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[interfaceType], _SHR_PORT_IF_COUNT);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[anMode], phymod_AN_MODE_Count);
        _SOC_IF_ERR_EXIT(rv);
        for(phyn = 0 ; phyn < MAX_PHYN ; phyn++){
            for(lane = 0 ; lane < PM4X10_QTC_LANES_PER_CORE ; lane++) {
                 portmod_xphy_lane_connection_t_init(unit, &lane_conn);
                _SOC_IF_ERR_EXIT(PM4x10_QTC_PORT_LANE_CONNECTION_DB_SET(unit, pm_info,
                                    &lane_conn, phyn, lane));
            }
        }
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[nofPhys], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                    pm_info->wb_vars_ids[portDynamicState], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                 pm_info->wb_vars_ids[isInterfaceRestore], 0);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC portmod_ucode_buf_t pm4x10_qtc_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};
STATIC portmod_ucode_buf_t pm4x10_qtc_ucode_buf_2nd[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};


STATIC int
pm4x10_qtc_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
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
                              &(pm4x10_qtc_ucode_buf[unit]),
                              &(pm4x10_qtc_ucode_buf_2nd[unit]),
                              PMQPORT_WC_UCMEM_DATAm,
                              PMQPORT_WC_UCMEM_CTRLr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

STATIC
int
pm4x10_qtc_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(PMQPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x10_qtc_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(PMQPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

phymod_bus_t pm4x10_qtc_default_bus = {
    "PM4x10_QTC Bus",
    pm4x10_qtc_default_bus_read,
    pm4x10_qtc_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

STATIC
int _pm4x10_qtc_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
    int i, tmp_port = 0;
    SOC_INIT_FUNC_DEFS;

    *first_index = -1;
    *bitmap = 0;

    for ( i = 0 ; i < MAX_PORTS_PER_PM4X10_QTC; i++) {
        _SOC_IF_ERR_EXIT(PM4x10_QTC_PORTS_GET(unit, pm_info, &tmp_port, i));

        if (tmp_port == port) {
            *first_index = (*first_index == -1) ? i : *first_index;
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

STATIC
int _pm4x10_qtc_pm_qtc_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    pm4x10_qtc_t pm4x10_qtc_data = NULL;
    const portmod_pm_qtc_create_info_t *pm_qtc_info = &pm_add_info->pm_specific_info.pm4x10_qtc.pm_qtc_info;
    int i, j, nof_phys = 0, lane, phyn, phy;
    portmod_xphy_lane_connection_t *db_lane_conn;
    portmod_qtc_mode_t qtc_mode = portmodQtcModeInvalid;
    SOC_INIT_FUNC_DEFS;

    pm4x10_qtc_data = sal_alloc(sizeof(struct pm4x10_qtc_s), "pm4x10_qtc_specific_db");
    SOC_NULL_CHECK(pm4x10_qtc_data);
    sal_memset(pm4x10_qtc_data, 0, sizeof(struct pm4x10_qtc_s));
    pm_info->pm_data.pm4x10_qtc_db = pm4x10_qtc_data;

    PORTMOD_PBMP_ASSIGN(pm4x10_qtc_data->phys, pm_add_info->phys);
    pm4x10_qtc_data->default_bus_is_used = 0;
    pm4x10_qtc_data->default_fw_loader_is_used = 0;
    pm4x10_qtc_data->portmod_phy_external_reset = pm_add_info->pm_specific_info.pm_qtc.portmod_phy_external_reset;
    pm4x10_qtc_data->portmod_mac_soft_reset = pm_add_info->pm_specific_info.pm_qtc.portmod_mac_soft_reset;
    for (i = 0 ; i < MAX_PORTS_PER_PM4X10_QTC; i++) {
        pm4x10_qtc_data->nof_phys[i] = 0;
    }
    PORTMOD_PBMP_ITER(pm_add_info->phys, phy) {
        pm4x10_qtc_data->first_phy = phy;
        break;
    }

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm4x10_qtc_data->int_core_access);

    /* initialize lane connections */
    for (i = 0; i < MAX_PHYN; i++) {
        for (j = 0; j < MAX_PORTS_PER_PM4X10_QTC; j++) {
            portmod_xphy_lane_connection_t_init(unit, &pm4x10_qtc_data->lane_conn[i][j]);
        }
    }

    _SOC_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(
                                         pm_qtc_info->fw_load_method));

    sal_memcpy(&pm4x10_qtc_data->polarity, &pm_qtc_info->polarity,
                sizeof(phymod_polarity_t));
    sal_memcpy(&(pm4x10_qtc_data->int_core_access.access), &pm_qtc_info->access,
                sizeof(phymod_access_t));
    sal_memcpy(&pm4x10_qtc_data->lane_map, &pm_qtc_info->lane_map,
                sizeof(pm4x10_qtc_data->lane_map));
    pm4x10_qtc_data->ref_clk = pm_qtc_info->ref_clk;
    pm4x10_qtc_data->fw_load_method = pm_qtc_info->fw_load_method;
    pm4x10_qtc_data->external_fw_loader = pm_qtc_info->external_fw_loader;
    pm4x10_qtc_data->core_clock_khz = pm_qtc_info->core_clock_khz;
    pm4x10_qtc_data->warmboot_skip_db_restore = TRUE;

    if(pm_qtc_info->access.bus == NULL) {
        /* if null - use default */
        pm4x10_qtc_data->int_core_access.access.bus = &pm4x10_qtc_default_bus;
        pm4x10_qtc_data->default_bus_is_used = 1;
    }

    if(pm4x10_qtc_data->external_fw_loader == NULL) {
        /* if null - use default */
        pm4x10_qtc_data->external_fw_loader = pm4x10_qtc_default_fw_loader;
        pm4x10_qtc_data->default_fw_loader_is_used = 1;
    }

    for (i = 0; i < MAX_PORTS_PER_PM4X10_QTC; i++) {
        pm4x10_qtc_data->nof_phys[i] = 1;
    }

    pm4x10_qtc_data->core_num     = pm_qtc_info->core_num;

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x10_qtc_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if (SOC_WARM_BOOT(unit)) {
        _SOC_IF_ERR_EXIT(PM4x10_QTC_PHY_TYPE_GET(unit, pm_info,
                            &pm4x10_qtc_data->int_core_access.type));

        /* restore nof_phys from wb */
        for (i = 0; i < MAX_PORTS_PER_PM4X10_QTC; i++) {
            _SOC_IF_ERR_EXIT(PM4x10_QTC_PORT_NUM_PHYS_GET(unit, pm_info, &nof_phys, i));
            pm4x10_qtc_data->nof_phys[i] = nof_phys;
        }

        for (phyn = 0; phyn < MAX_PHYN; phyn++) {
            for (lane = 0; lane < MAX_PORTS_PER_PM4X10_QTC; lane++) {
                db_lane_conn = &(PM_4x10_QTC_INFO(pm_info)->lane_conn[phyn][lane]);
                _SOC_IF_ERR_EXIT(PM4x10_QTC_PORT_LANE_CONNECTION_DB_GET(unit, pm_info,
                                 db_lane_conn, phyn, lane));
            }
        }

        /* restore qtc mode */
        _SOC_IF_ERR_EXIT(PM4x10_QTC_QTC_MODE_GET(unit, pm_info, &qtc_mode));
        if (qtc_mode == portmodQtcModeQsgmii) {
            PM_4x10_QTC_INFO(pm_info)->int_core_access.access.flags |= PHYMOD_ACC_F_QMODE;
        } else if (qtc_mode == portmodQtcModeUsxgmii) {
            PM_4x10_QTC_INFO(pm_info)->int_core_access.access.flags |= PHYMOD_ACC_F_USXMODE;
        }

    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pm4x10_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x10_qtc_create_info_t *info = &pm_add_info->pm_specific_info.pm4x10_qtc;
    int rv;
    portmod_pm_create_info_internal_t pm4x10_add_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_pm_create_info_internal_t_init(unit, &pm4x10_add_info));

    pm_info->pm_data.pm4x10_qtc_db->pm4x10= sal_alloc(sizeof(struct pm_info_s), "port_macros");
    SOC_NULL_CHECK(pm_info->pm_data.pm4x10_qtc_db->pm4x10);
    sal_memset(pm_info->pm_data.pm4x10_qtc_db->pm4x10, 0, sizeof(struct pm_info_s));

    /*pm4x10 init*/
    PORTMOD_PBMP_ASSIGN(pm4x10_add_info.phys, pm_add_info->phys);
    pm4x10_add_info.type = portmodDispatchTypePm4x10;
    sal_memcpy(&pm4x10_add_info.pm_specific_info.pm4x10, &info->pm4x10_info,
                        sizeof(portmod_pm4x10_create_info_t));
    rv = __portmod__dispatch__[pm4x10_add_info.type]->f_portmod_pm_init(unit,
                                    &pm4x10_add_info, wb_buffer_index, pm_info->pm_data.pm4x10_qtc_db->pm4x10);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    int pm4x10_wb_buffer_id;
    SOC_INIT_FUNC_DEFS;

    pm_info->unit = unit;
    pm_info->type = portmodDispatchTypePm4x10_qtc;
    pm_info->wb_buffer_id = wb_buffer_index;

    /*pm_qtc init*/
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm_qtc_pm_init(unit, pm_add_info,
                                wb_buffer_index, pm_info));

    /*pm4x10 init*/
    pm4x10_wb_buffer_id = wb_buffer_index + 1;
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm4x10_pm_init(unit, pm_add_info,
                                pm4x10_wb_buffer_id, pm_info));

exit:
    if(SOC_FUNC_ERROR){
        pm4x10_qtc_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if(pm_info->pm_data.pm4x10_qtc_db != NULL) {
        if (pm_info->pm_data.pm4x10_qtc_db->pm4x10 != NULL) {
            SOC_IF_ERROR_RETURN(pm4x10_pm_destroy(unit, pm_info->pm_data.pm4x10_qtc_db->pm4x10));
            sal_free(pm_info->pm_data.pm4x10_qtc_db->pm4x10);
            pm_info->pm_data.pm4x10_qtc_db->pm4x10 = NULL;
        }
        sal_free(pm_info->pm_data.pm4x10_qtc_db);
        pm_info->pm_data.pm4x10_qtc_db = NULL;
    }

    /* free pm4x10_ucode_buf */
    if(pm4x10_qtc_ucode_buf[unit].ucode_dma_buf != NULL){
        portmod_sys_dma_free(unit, pm4x10_qtc_ucode_buf[unit].ucode_dma_buf);
        pm4x10_qtc_ucode_buf[unit].ucode_dma_buf = NULL;
    }
    pm4x10_qtc_ucode_buf[unit].ucode_alloc_size = 0;

    if (pm4x10_qtc_ucode_buf_2nd[unit].ucode_dma_buf != NULL) {
        portmod_sys_dma_free(unit, pm4x10_qtc_ucode_buf_2nd[unit].ucode_dma_buf);
        pm4x10_qtc_ucode_buf_2nd[unit].ucode_dma_buf = NULL;
    }
    pm4x10_qtc_ucode_buf_2nd[unit].ucode_alloc_size = 0;

    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_ext_phy_interface_check(int unit, int port, phymod_phy_access_t *phy_acc,
                               const portmod_port_interface_config_t *interface_config,
                               soc_port_if_t interface, int *is_valid)
{
    int rv = 0;
    int num_lanes = 0, data_rate = 0;
    uint32_t interface_mode = 0;
    phymod_interface_t  phymod_intf;
    int is_legacy_phy = 0, xphy_id;
    SOC_INIT_FUNC_DEFS;

    num_lanes      = interface_config->port_num_lanes;
    interface_mode = interface_config->interface_modes;
    data_rate      = interface_config->speed;

    xphy_id = phy_acc->access.addr;
    _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));

    if (is_legacy_phy) {
        /* if its a legacy PHY let the PHY driver decide the valid interface types */
        *is_valid = 1;
        rv = SOC_E_NONE;
        SOC_EXIT;
    }

    /* convert portmod interface to phymod interface */
    rv = portmod_intf_to_phymod_intf(unit, data_rate, interface, &phymod_intf);
    _SOC_IF_ERR_EXIT(rv);

    rv = portmod_port_line_interface_is_valid(unit, phy_acc, port, data_rate, phymod_intf,
                                            num_lanes, interface_mode, is_valid);
    _SOC_IF_ERR_EXIT(rv);

    if (!*is_valid) {
        LOG_ERROR(BSL_LS_BCM_PORT,
              (BSL_META_UP(unit, port,
               "ERROR: u=%d p=%d interface type %d not supported by external PHY for this speed %d \n"),
               unit, port, interface, data_rate));
        rv = SOC_E_PARAM;
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_interface_check(int unit, int port, pm_info_t pm_info,
                            const portmod_port_interface_config_t *interface_config,
                            soc_port_if_t interface, int *is_valid)
{
    int num_lanes;

    *is_valid = 0;
    num_lanes = interface_config->port_num_lanes;

    switch (interface_config->speed) {
        case 0:
            /* When AN=1 the input speed to portmod may be 0. At
             * this moment the speed is still undecided so there
             * is no reason to invalidate the interface type.
             */
            *is_valid = 1;
            break;
        case 10:
            if (num_lanes == 1) {
                if (SOC_PORT_IF_SGMII == interface) {
                    *is_valid = 1;
                }
            }
            break;
        case 100:
            if (num_lanes == 1) {
                if (SOC_PORT_IF_SGMII == interface) {
                    *is_valid = 1;
                }
            }
            break;
        case 1000:
            if (num_lanes == 1) {
                if ((SOC_PORT_IF_SGMII == interface) ||
                    (SOC_PORT_IF_QSGMII == interface)) {
                    *is_valid = 1;
                }
            }
            break;
        case 2500:
            if (num_lanes == 1) {
                if (SOC_PORT_IF_SGMII == interface) {
                    *is_valid = 1;
                }
            }
            break;
        default:
            break;
    }

    return SOC_E_NONE;
}

int _pm4x10_qtc_port_interface_check(int unit, int port, pm_info_t pm_info,
                                const portmod_port_interface_config_t* config,
                                int is_ext_phy, soc_port_if_t interface,
                                int* is_valid)
{
    int nof_phys = 0;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    if (is_ext_phy && nof_phys < 2) {
        return SOC_E_PARAM;
    }

    if (is_ext_phy) {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_ext_phy_interface_check(unit, port, &phy_access[nof_phys-1], config,
                                                         interface, is_valid));
    } else {
         _SOC_IF_ERR_EXIT(_pm4x10_qtc_interface_check(unit, port, pm_info, config, interface, is_valid));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_default_interface_get(int unit, int port,
                               const portmod_port_interface_config_t *interface_config,
                               soc_port_if_t *interface)
{
    *interface = SOC_PORT_IF_SGMII;

    return SOC_E_NONE;
}

int pm4x10_qtc_port_tsc_reset_set(int unit, int port, pm_info_t pm_info, int in_reset)
{
    uint32 reg_val;
    int reg_port;
    portmod_default_user_access_t *user_data;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)PM_4x10_QTC_INFO(pm_info)->int_core_access.access.user_acc;
    reg_port = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    /* Bring Internal Phy OOR */
    _SOC_IF_ERR_EXIT(READ_PMQ_XGXS0_CTRL_REGr(unit, reg_port, &reg_val));
    soc_reg_field_set(unit, PMQ_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
    soc_reg_field_set(unit, PMQ_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
    soc_reg_field_set(unit, PMQ_XGXS0_CTRL_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_PMQ_XGXS0_CTRL_REGr(unit, reg_port, reg_val));

    /* Based on the feedback from SJ pmd support team, ~10-15usecs would be sufficient
     * for the PLL/AFE to settle down out of IDDQ reset.
     */
    sal_usleep(10);

exit:
    SOC_FUNC_RETURN;
}

static phymod_dispatch_type_t pm4x10_qtc_serdes_list[] =
{
#ifdef PHYMOD_QTCE_SUPPORT
    phymodDispatchTypeQtce,
#endif
#ifdef PHYMOD_QTCE16_SUPPORT
    phymodDispatchTypeQtce16,
#endif
    phymodDispatchTypeInvalid
};

STATIC
int _pm4x10_qtc_port_ecc_init(int unit, int port, pm_info_t pm_info)
{
    int reg_port;
    uint32 reg_val;
    portmod_default_user_access_t *user_data;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)PM_4x10_QTC_INFO(pm_info)->int_core_access.access.user_acc;
    reg_port = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    /* Init port ecc */
    _SOC_IF_ERR_EXIT(READ_PMQ_ECC_INIT_CTRLr(unit, reg_port, &reg_val));
    soc_reg_field_set(unit, PMQ_ECC_INIT_CTRLr, &reg_val, GMIB0_MEM_INITf, 0x1);
    soc_reg_field_set(unit, PMQ_ECC_INIT_CTRLr, &reg_val, GMIB1_MEM_INITf, 0x1);
    soc_reg_field_set(unit, PMQ_ECC_INIT_CTRLr, &reg_val, GP0_MEM_INITf, 0x1);
    soc_reg_field_set(unit, PMQ_ECC_INIT_CTRLr, &reg_val, GP1_MEM_INITf, 0x1);
    _SOC_IF_ERR_EXIT(WRITE_PMQ_ECC_INIT_CTRLr(unit, reg_port, reg_val));

    /* Wait for GP/GMIB mem to finish init */
    soc_timeout_init(&to, 10, 0);
    while (!soc_timeout_check(&to)) {
        _SOC_IF_ERR_EXIT(READ_PMQ_ECC_INIT_STSr(unit, reg_port, &reg_val));
        if ((reg_val & GP_GMIB_MEM_INIT_DONE_MASK) == GP_GMIB_MEM_INIT_DONE_MASK){
            break;
        }
    }

    /*Mask the ECC status for each of the UNIMACs*/
    _SOC_IF_ERR_EXIT(READ_PMQ_ECCr(unit, reg_port, &reg_val));
    soc_reg_field_set(unit, PMQ_ECCr, &reg_val, PORTNUM_ECCSTS_MASKf, 0xffff);
    _SOC_IF_ERR_EXIT(WRITE_PMQ_ECCr(unit, reg_port, reg_val));

exit:
     SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pm_core_probe(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int     rv = SOC_E_NONE;
    int     temp, phy;
    portmod_pbmp_t port_phys_in_pm;
    int probe =0;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i, is_legacy_phy;
    portmod_xphy_lane_connection_t lane_conn;
    phymod_core_access_t core_access;
    int core_probed;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&lane_conn,0,sizeof(portmod_xphy_lane_connection_t));

    /* probe phys (both internal and external cores. */

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x10_QTC_INFO(pm_info)->phys);

    /* if internal core is not probe, probe it. Currently checking to type to see
       phymodDispatchTypeInvalid, later need to move to WB.  */
    _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm4x10_qtc_serdes_list, &PM_4x10_QTC_INFO(pm_info)->int_core_access, &probe));

    /* save probed phy type to wb. */
    if(probe) {
        rv = PM4x10_QTC_PHY_TYPE_SET(unit, pm_info, PM_4x10_QTC_INFO(pm_info)->int_core_access.type);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* probe ext core related to this logical port. */
    temp = 0;
    PORTMOD_PBMP_ITER(PM_4x10_QTC_INFO(pm_info)->phys, phy) {
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if( PM_4x10_QTC_INFO(pm_info)->nof_phys[temp] >= 2 ){
                nof_ext_phys = PM_4x10_QTC_INFO(pm_info)->nof_phys[temp]-1;
                for(i=0 ; i<nof_ext_phys ; i++) {
                    xphy_id = PM_4x10_QTC_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        _SOC_IF_ERR_EXIT(portmod_common_ext_phy_probe(unit, port, &core_access, &probe));
                        if ((probe == 0) &&
                            (core_access.type == phymodDispatchTypeInvalid)){
                            pm4x10_qtc_xphy_lane_detach_from_pm(unit, pm_info, phy, i+1, &lane_conn);
                            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                "Warning: EXT PHY p=%d xphy_id=%d can't be probed\n"), port, xphy_id));
                        } else {
                            core_probed = 1;
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_probed_set(unit, xphy_id, core_probed));
                            /* Check if after the probe the ext phy is legacy, so we dont overrun the current value */
                            _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_access_set(unit, xphy_id, &core_access, is_legacy_phy));
                        }
                    }
                }
            }
        }
        temp++;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pm_serdes_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int rv;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    const portmod_port_interface_config_t* config;
    uint32 core_is_initialized;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    uint32 init_flags = 0;
    SOC_INIT_FUNC_DEFS;

    config = &(add_info->interface_config);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    core_conf.firmware_load_method = PM_4x10_QTC_INFO(pm_info)->fw_load_method;
    core_conf.firmware_loader = PM_4x10_QTC_INFO(pm_info)->external_fw_loader;
    core_conf.lane_map = PM_4x10_QTC_INFO(pm_info)->lane_map;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                interface, &core_conf.interface.interface_type));
    core_conf.interface.data_rate = config->speed;
    core_conf.interface.interface_modes = config->interface_modes;
    core_conf.interface.ref_clock = PM_4x10_QTC_INFO(pm_info)->ref_clk;

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
    core_status.pmd_active = 0;

    init_flags = PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) | PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info);

    if (PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }

    core_conf.op_datapath = add_info->phy_op_datapath;

    rv = PM4x10_QTC_IS_INITIALIZED_GET(unit, pm_info, &core_is_initialized);
    _SOC_IF_ERR_EXIT(rv);

    if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized)) {
         /* firmware load will happen after pass 1 */
         if (!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized) ||
             PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                 _SOC_IF_ERR_EXIT(phymod_core_init(&PM_4x10_QTC_INFO(pm_info)->int_core_access,
                                                   &core_conf,
                                                   &core_status));

                if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
                    PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
                    rv = PM4x10_QTC_IS_INITIALIZED_SET(unit, pm_info, core_is_initialized);
                    _SOC_IF_ERR_EXIT(rv);
                }
                if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_flags == 0) {
                    PORTMOD_CORE_INIT_FLAG_INITIALZIED_SET(core_is_initialized);
                    rv = PM4x10_QTC_IS_INITIALIZED_SET(unit, pm_info, core_is_initialized);
                    _SOC_IF_ERR_EXIT(rv);
                }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pm_ext_phy_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int temp, phy;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    const portmod_port_interface_config_t* config;
    portmod_pbmp_t port_phys_in_pm;
    phymod_firmware_load_method_t fw_load_method;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    uint32 primary_core_num;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    uint32 xphy_id;
    int is_legacy_phy = 0;
    uint32 nof_ext_phys = 0;
    int force_fw_load, is_initialized = 0;
    int init_all = 0, lane, i;

    SOC_INIT_FUNC_DEFS;

    config = &(add_info->interface_config);

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x10_QTC_INFO(pm_info)->phys);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                interface, &core_conf.interface.interface_type));
    core_conf.interface.data_rate = config->speed;
    core_conf.interface.interface_modes = config->interface_modes;
    core_conf.interface.ref_clock = PM_4x10_QTC_INFO(pm_info)->ref_clk;

    core_conf.op_datapath = add_info->phy_op_datapath;

    /* deal with ext phys now ... */
    if(PORTMOD_PORT_ADD_F_PORT_ATTACH_EXT_PHY_SKIP_GET(add_info)) {
        return(SOC_E_NONE);
    }

    core_conf.firmware_load_method = (PM_4x10_QTC_INFO(pm_info)->fw_load_method ==
                                            phymodFirmwareLoadMethodNone) ?
                                            phymodFirmwareLoadMethodNone :
                                            phymodFirmwareLoadMethodInternal;
    core_conf.firmware_loader = NULL;

    /* config "no swap" for external phys.set later using APIs*/
    for (lane=0 ; lane < PM4X10_QTC_LANES_PER_CORE; lane++) {
        core_conf.lane_map.lane_map_rx[lane] = lane;
        core_conf.lane_map.lane_map_tx[lane] = lane;
    }

    temp = 0;
    PORTMOD_PBMP_ITER(PM_4x10_QTC_INFO(pm_info)->phys, phy) {
        if (PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if (PM_4x10_QTC_INFO(pm_info)->nof_phys[temp] >= 2 ) {
                nof_ext_phys = PM_4x10_QTC_INFO(pm_info)->nof_phys[temp]-1;
                for (i=0 ; i<nof_ext_phys; i++) {
                    xphy_id = PM_4x10_QTC_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        _SOC_IF_ERR_EXIT(portmod_xphy_fw_load_method_get(unit, xphy_id, &fw_load_method));
                        _SOC_IF_ERR_EXIT(portmod_xphy_force_fw_load_get(unit, xphy_id, &force_fw_load));
                        _SOC_IF_ERR_EXIT(portmod_xphy_lane_map_get(unit, xphy_id, &lane_map));
                        _SOC_IF_ERR_EXIT(portmod_xphy_primary_core_num_get(unit, xphy_id, &primary_core_num));

                        _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
                        core_status.pmd_active = 0;

                        /* for external phys, FW load method is default to Internal
                        unless it is force to NONE. */
                        core_conf.firmware_load_method = fw_load_method;
                        core_conf.firmware_loader = NULL;

                        if (force_fw_load == phymodFirmwareLoadForce) {
                            PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&core_conf);
                        } else if (force_fw_load == phymodFirmwareLoadAuto) {
                            PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&core_conf);
                        }

                        if ((PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS1)) {
                            continue;
                        }
                        if ((PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)|| init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS2)) {
                            continue;
                        }
                        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
                        if (is_legacy_phy) {
                            /* Dont do a core init for legacy Phys.*/
                        } else {
                            _SOC_IF_ERR_EXIT(phymod_core_init(&core_access,
                                    &core_conf,
                                    &core_status));
                        }

                        if (init_all) {
                            is_initialized = (PHYMOD_CORE_INIT_F_EXECUTE_PASS2 | PHYMOD_CORE_INIT_F_EXECUTE_PASS1);
                        } else if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                            is_initialized |= PHYMOD_CORE_INIT_F_EXECUTE_PASS2;
                        } else {
                            is_initialized = PHYMOD_CORE_INIT_F_EXECUTE_PASS1;
                        }

                        _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_set(unit, xphy_id, is_initialized));
                    }
                }
            }
        }
        temp++;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pm_core_init(int unit, pm_info_t pm_info,
                             const portmod_port_add_info_t* add_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm_serdes_core_init(unit, pm_info, add_info));
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm_ext_phy_core_init(unit, pm_info, add_info));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pm_port_init(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int reg_port, port_index;
    uint32 bitmap, reg_val, field_val;
    portmod_default_user_access_t *user_data;
    soc_timeout_t to;
    int flags;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)PM_4x10_QTC_INFO(pm_info)->int_core_access.access.user_acc;
    reg_port = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    /* Init unimac */
    flags = 0;
    if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
        flags |= UNIMAC_INIT_F_RX_STRIP_CRC;
    }
    _SOC_IF_ERR_EXIT(unimac_init(unit, port, flags));

    /* Init port ecc */
    _SOC_IF_ERR_EXIT(READ_PMQ_ECC_INIT_CTRLr(unit, reg_port, &reg_val));
    field_val = soc_reg_field_get(unit, PMQ_ECC_INIT_CTRLr, reg_val, UNIMAC_MEM_INITf);
    field_val |= 0x1 << port_index;
    soc_reg_field_set(unit, PMQ_ECC_INIT_CTRLr, &reg_val, UNIMAC_MEM_INITf, field_val);
    _SOC_IF_ERR_EXIT(WRITE_PMQ_ECC_INIT_CTRLr(unit, reg_port, reg_val));

    /* Wait for UNIMAC mem to finish init */
    soc_timeout_init(&to, 10, 0);
    while (!soc_timeout_check(&to)) {
        _SOC_IF_ERR_EXIT(READ_PMQ_ECC_INIT_STSr(unit, reg_port, &reg_val));
        if ((reg_val & UNIMAC_MEM_INIT_DONE_MASK) == (0x1 << port_index)){
            break;
        }
    }

    /*Mask the ECC status for each of the UNIMACs*/
    _SOC_IF_ERR_EXIT(READ_PMQ_ECCr(unit, reg_port, &reg_val));
    field_val = soc_reg_field_get(unit, PMQ_ECCr, reg_val, PORTNUM_ECCSTS_MASKf);
    field_val &= ~(0x1 << port_index);
    soc_reg_field_set(unit, PMQ_ECCr, &reg_val, PORTNUM_ECCSTS_MASKf, field_val);
    _SOC_IF_ERR_EXIT(WRITE_PMQ_ECCr(unit, reg_port, reg_val));

exit:
     SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_pmq_gport_init(int unit, int port, pm_info_t pm_info,
                        const portmod_port_add_info_t* add_info)
{
    int reg_port, pm_is_active;
    uint32 reg_val;
    portmod_default_user_access_t *user_data;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)PM_4x10_QTC_INFO(pm_info)->int_core_access.access.user_acc;
    reg_port = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    _SOC_IF_ERR_EXIT(PM4x10_QTC_IS_ACTIVE_GET(unit, pm_info, &pm_is_active));
    if (pm_is_active == 0) {
        /* special init process for PMQ(PM4x10Q in QSGMII mode) - to release PMQ's QSGMII reset state after QSGMII-PCS and UniMAC init. */
        SOC_IF_ERROR_RETURN(READ_CHIP_CONFIGr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, QMODEf, 1);
        if ((PM_4x10_QTC_INFO(pm_info)->core_clock_khz > 325000) || (PM_4x10_QTC_INFO(pm_info)->core_clock_khz == 0)) {
            soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, IP_TDMf, 0x3);
        } else {
            /* According to PORTMACRO-210, when clk=300M or less, ip_tdm should be 2 or less */
            soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, IP_TDMf, 0x1);
        }
        if (SOC_REG_FIELD_VALID(unit, CHIP_CONFIGr, PCS_USXGMII_MODE_ENf)) {
            if (add_info->init_config.qtc_mode == portmodQtcModeUsxgmii) {
                soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, PCS_USXGMII_MODE_ENf, 0x1);
            } else {
                soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, PCS_USXGMII_MODE_ENf, 0);
            }
        }
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, PAUSE_PFC_SELf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_CHIP_CONFIGr(unit, reg_port, reg_val));

        /* Get Serdes OOR */
        _SOC_IF_ERR_EXIT(pm4x10_qtc_port_tsc_reset_set(unit, port, pm_info, 1));
        _SOC_IF_ERR_EXIT(pm4x10_qtc_port_tsc_reset_set(unit, port, pm_info, 0));

        _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_QSGMII_PCSf, 0x1);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT1f, 0x1);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT0f, 0x1);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

        _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_QSGMII_PCSf, 0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT1f, 0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT0f, 0);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

        /* Init port ecc */
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_ecc_init(unit, port, pm_info));

        pm_is_active = 1;
        _SOC_IF_ERR_EXIT(PM4x10_QTC_IS_ACTIVE_SET(unit, pm_info, pm_is_active));
    }

    if (add_info->flags & PORTMOD_PORT_ADD_F_EGR_1588_TIMESTAMP_MODE_48BIT) {
        SOC_IF_ERROR_RETURN(READ_GPORT_MODE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EGR_1588_TIMESTAMPING_MODEf, 0);
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
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_MODES_SELf, 0x0);
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_FWDf, 0);
    soc_reg_field_set(unit, GPORT_MODE_REGr, &reg_val, EP_TO_GP_CRC_OWRTf, 0);
    _SOC_IF_ERR_EXIT(WRITE_GPORT_MODE_REGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_attach_core_probe (int unit, int port, pm_info_t pm_info,
                        const portmod_port_add_info_t* add_info)
{
    int port_index = -1, rv = 0, phy, i;
    portmod_pbmp_t port_phys_in_pm;
    int prev_phy = -1;
    SOC_INIT_FUNC_DEFS;

    /* Initialize PMQ and gport*/
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pmq_gport_init(unit, port, pm_info, add_info));

    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x10_QTC_INFO(pm_info)->phys);
    i = 0;
    PORTMOD_PBMP_ITER(PM_4x10_QTC_INFO(pm_info)->phys, phy) {
        if (prev_phy != -1) {
            i += phy - prev_phy;
        }

        if (PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            _SOC_IF_ERR_EXIT(PM4x10_QTC_PORTS_SET(unit, pm_info, port, i));
            port_index = (port_index == -1) ? i : port_index;
        }
        prev_phy = phy;
    }

    rv = PM4x10_QTC_SPEED_SET(unit, pm_info, add_info->interface_config.speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x10_QTC_MAX_SPEED_SET(unit, pm_info,add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x10_QTC_INTERFACE_TYPE_SET(unit, pm_info, add_info->interface_config.interface, port_index);
     _SOC_IF_ERR_EXIT(rv);

    rv = PM4x10_QTC_AN_MODE_SET(unit, pm_info, add_info->init_config.an_mode,  port_index);
    _SOC_IF_ERR_EXIT(rv);

    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_4x10_QTC_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_4x10_QTC_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_4x10_QTC_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[0];
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_4x10_QTC_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }

    /* QTC core probe */
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm_core_probe(unit, port, pm_info, add_info));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_attach_resume_fw_load (int unit, int port, pm_info_t pm_info,
                        const portmod_port_add_info_t* add_info)
{
    int port_i, my_i;
    int i, nof_phys, usr_cfg_idx;
    uint32 bitmap, port_dynamic_state;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int port_index = -1;
    phymod_phy_init_config_t init_config;
    phymod_interface_t          phymod_serdes_interface = phymodInterfaceCount;
    phymod_autoneg_control_t an;
    portmod_port_ability_t port_ability;

    SOC_INIT_FUNC_DEFS;

    phymod_autoneg_control_t_init(&an);

    /* Initialze phys */
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    if (PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] > 0) {
        _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
        _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                               phy_access ,(1+MAX_PHYN),
                                                               &nof_phys));
        my_i = 0, usr_cfg_idx = 0;
        for (i = 0; i < MAX_PORTS_PER_PM4X10_QTC; i++) {
            _SOC_IF_ERR_EXIT(PM4x10_QTC_PORTS_GET(unit, pm_info, &port_i, i));

            if(port_i != port) {
                continue;
            }

            if(SHR_BITGET(&(PM_4x10_QTC_INFO(pm_info)->polarity.tx_polarity), i)) {
                SHR_BITSET(&init_config.polarity.tx_polarity, my_i);
            }

            if(SHR_BITGET(&(PM_4x10_QTC_INFO(pm_info)->polarity.rx_polarity), i)) {
                SHR_BITSET(&init_config.polarity.rx_polarity, my_i);
            }

            _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access[0], phymodMediaTypeChipToChip, &init_config.tx[my_i]));

            if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
                init_config.tx[my_i].pre = add_info->init_config.tx_params[usr_cfg_idx].pre;
                init_config.tx[my_i].main = add_info->init_config.tx_params[usr_cfg_idx].main;
                init_config.tx[my_i].post = add_info->init_config.tx_params[usr_cfg_idx].post;
            }

            if (PORTMOD_USER_SET_TX_AMP_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
                init_config.tx[my_i].amp = add_info->init_config.tx_params[usr_cfg_idx].amp;
            }

            if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])
                || PORTMOD_USER_SET_TX_AMP_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
                port_dynamic_state = 0;
                PORTMOD_PORT_DEFAULT_TX_PARAMS_UPDATED_SET(port_dynamic_state);

                _SOC_IF_ERR_EXIT(PM4x10_QTC_PORT_DYNAMIC_STATE_SET(unit, pm_info, port_dynamic_state, port_index));
            }

            usr_cfg_idx++;
            my_i++;
        }

        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,
                                           add_info->interface_config.speed,
                                           add_info->interface_config.interface,
                                           &init_config.interface.interface_type));
        init_config.interface.data_rate = add_info->interface_config.speed;
        init_config.interface.pll_divider_req =
                        add_info->interface_config.pll_divider_req;
        init_config.interface.ref_clock = PM_4x10_QTC_INFO(pm_info)->ref_clk;

        _SOC_IF_ERR_EXIT(portmod_port_phychain_phy_init(unit, phy_access, nof_phys,
                                                        &init_config));
    }

    /* PHY interface set  */
    if (add_info->interface_config.serdes_interface != SOC_PORT_IF_NULL) {
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,
                                add_info->interface_config.speed,
                                add_info->interface_config.serdes_interface,
                                &phymod_serdes_interface));
    }

    /* Apply to SerDes with same interface config */
    _SOC_IF_ERR_EXIT(
            portmod_port_phychain_interface_config_set(unit, port, phy_access, nof_phys,
                                          add_info->interface_config.flags ,
                                          &init_config.interface,
                                          phymod_serdes_interface,
                                          PM_4x10_QTC_INFO(pm_info)->ref_clk,
                                          PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
    /* set the default advert ability */
    _SOC_IF_ERR_EXIT
        (_pm4x10_qtc_port_ability_local_get(unit, port, pm_info, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &port_ability));
    _SOC_IF_ERR_EXIT
        (_pm4x10_qtc_port_ability_advert_set(unit, port, pm_info, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &port_ability));

     /* Enable SerDes AN */
     an.an_mode = phymod_AN_MODE_SGMII;
     an.enable = 1;
     _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_autoneg_set(unit, port, pm_info, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &an));

    /* Initialize unimac and port*/
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm_port_init(unit, port, pm_info, add_info));

    /* Disable Internal SerDes LPI bypass */
    _SOC_IF_ERR_EXIT(phymod_phy_eee_set(&phy_access[0], FALSE));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int  init_all = 0;
    SOC_INIT_FUNC_DEFS;

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || (init_all)) {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_attach_core_probe(unit, port, pm_info, add_info));
    }

    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
        !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (SOC_E_NONE);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || (init_all)) {
        /* Core initialization */
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_pm_core_init(unit, pm_info, add_info));
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN(_pm4x10_qtc_port_attach_resume_fw_load (unit, port, pm_info, add_info));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int rv, active_port_num, current_qtc_mode = 0;
    portmod_qtc_mode_t qtc_mode = add_info->init_config.qtc_mode;
    int init_all = 0, core_is_active = 0;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_CORE_ACTIVE_GET(core_is_active);
    if (!core_is_active) {
        _SOC_IF_ERR_EXIT(PM4x10_QTC_QTC_MODE_SET(unit, pm_info, qtc_mode));
    } else {
        _SOC_IF_ERR_EXIT(PM4x10_QTC_QTC_MODE_GET(unit, pm_info, &current_qtc_mode));
        if (current_qtc_mode != qtc_mode) {
            /*PM4x10_QTC doesn't support mixed mode*/
            _SOC_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }

    if (qtc_mode == portmodQtcModeQsgmii) {
        PM_4x10_QTC_INFO(pm_info)->int_core_access.access.flags = PHYMOD_ACC_F_QMODE;
    } else if (qtc_mode == portmodQtcModeUsxgmii) {
        PM_4x10_QTC_INFO(pm_info)->int_core_access.access.flags = PHYMOD_ACC_F_USXMODE;
    }

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;
    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_attach(unit, port, PM4x10_INFO(pm_info), add_info));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_attach(unit, port, pm_info, add_info));
    }

    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || (init_all)) {
        active_port_num++;
        rv = PM4x10_QTC_ACTIVE_PORT_NUM_SET(unit, pm_info, active_port_num);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable, i, reg_port, invalid_port = -1;
    uint32 reg_val;
    portmod_default_user_access_t *user_data;
    int tmp_port = 0, is_last_one = TRUE;
    uint32 inactive = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x10_qtc_port_enable_get(unit, port, pm_info, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    /*remove from array and check if it was the last one*/
    for (i = 0; i < MAX_PORTS_PER_PM4X10_QTC; i++) {
        _SOC_IF_ERR_EXIT(PM4x10_QTC_PORTS_GET(unit, pm_info, &tmp_port, i));
       if (tmp_port == port) {
           _SOC_IF_ERR_EXIT(PM4x10_QTC_PORTS_SET(unit, pm_info, invalid_port, i));
       } else if (tmp_port != -1) {
           is_last_one = FALSE;
       }
    }

    user_data = (portmod_default_user_access_t*)PM_4x10_QTC_INFO(pm_info)->int_core_access.access.user_acc;
    reg_port = user_data->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;

    /* deinit MAC */
    _SOC_IF_ERR_EXIT(unimac_deinit(unit, port));

    if (is_last_one) { /*last one*/
        /* Put Serdes in reset*/
        _SOC_IF_ERR_EXIT(pm4x10_qtc_port_tsc_reset_set(unit, port, pm_info, 1));

        _SOC_IF_ERR_EXIT(PM4x10_QTC_IS_ACTIVE_SET(unit, pm_info, inactive));

         _SOC_IF_ERR_EXIT(PM4x10_QTC_IS_INITIALIZED_SET(unit, pm_info, inactive));

        _SOC_IF_ERR_EXIT(READ_CHIP_SWRSTr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_QSGMII_PCSf, 0x0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT1f, 0x0);
        soc_reg_field_set(unit, CHIP_SWRSTr, &reg_val, SOFT_RESET_GPORT0f, 0x0);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_SWRSTr(unit, reg_port, reg_val));

        _SOC_IF_ERR_EXIT(READ_CHIP_CONFIGr(unit, reg_port, &reg_val));
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, QMODEf, 0);
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, IP_TDMf, 0);
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, PCS_USXGMII_MODE_ENf, 0);
        soc_reg_field_set(unit, CHIP_CONFIGr, &reg_val, PAUSE_PFC_SELf, 0x1);
        _SOC_IF_ERR_EXIT(WRITE_CHIP_CONFIGr(unit, reg_port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_detach(int unit, int port, pm_info_t pm_info)
{
    int rv, active_port_num;
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_detach(unit, port, PM4x10_INFO(pm_info)));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_detach(unit, port, pm_info));
    }

    rv = PM4x10_QTC_ACTIVE_PORT_NUM_GET(unit, pm_info, &active_port_num);
    _SOC_IF_ERR_EXIT(rv);
    active_port_num--;
    if (active_port_num <= 0) {
        active_port_num = 0;
    }
    rv = PM4x10_QTC_ACTIVE_PORT_NUM_SET(unit, pm_info, active_port_num);
    _SOC_IF_ERR_EXIT(rv);

    if (active_port_num == 0) {
        qtc_mode = portmodQtcModeInvalid;
        rv = PM4x10_QTC_QTC_MODE_SET(unit, pm_info, qtc_mode);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_pm_core_info_get(unit,
                        PM4x10_INFO(pm_info), phyn, core_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_replace(unit, port, PM4x10_INFO(pm_info), new_port));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int     actual_flags = flags, nof_phys = 0;
    phymod_phy_power_t           phy_power;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 phychain_flag;
    int  unimac_flags = 0;
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
    if ((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))) {
        if ((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))) {
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

    if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
        unimac_flags |= UNIMAC_ENABLE_SET_FLAGS_RX_EN;
    }
    if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
        unimac_flags |= UNIMAC_ENABLE_SET_FLAGS_TX_EN;
    }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        if (enable) {
            if (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, 0, enable));
            }
            if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
                if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                    _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_lane_control_set(
                                unit, port, phy_access, nof_phys, phychain_flag, phymodTxSquelchOff));
                }
                if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                    _SOC_IF_ERR_EXIT(portmod_port_phychain_rx_lane_control_set(
                                unit, port, phy_access, nof_phys, phychain_flag, phymodRxSquelchOff));
                }
            }
        } else {
            if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
                if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                    _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_lane_control_set(
                                unit, port, phy_access, nof_phys, phychain_flag, phymodTxSquelchOn));
                }
                if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                    _SOC_IF_ERR_EXIT(portmod_port_phychain_rx_lane_control_set(
                                unit, port, phy_access, nof_phys, phychain_flag, phymodRxSquelchOn));
                }
            }
            if (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, unimac_flags, enable));
            }
        }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_enable_set(unit, port, PM4x10_INFO(pm_info), flags, enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_enable_set(unit, port, pm_info, flags, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
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

int pm4x10_qtc_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_enable_get(unit, port, PM4x10_INFO(pm_info), flags, enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_enable_get(unit, port, pm_info, flags, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    int nof_phys;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t     phy_interface_config;
    soc_port_if_t        interface = SOC_PORT_IF_NULL;
    int port_index;
    uint32 bitmap;
    phymod_interface_t   phymod_serdes_interface = phymodInterfaceCount;
    portmod_port_interface_config_t config_temp;
    uint32 is_interface_restore = 0;
    int is_valid = 0, is_ext_phy = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &config_temp));
    sal_memcpy(&config_temp, config, sizeof(portmod_port_interface_config_t));

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                phy_access ,(1+MAX_PHYN), &nof_phys));

    if (config->flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP) {
        int mac_speed = 0, mac_only = 0;
        phymod_phy_inf_config_t config_tmp;

        _SOC_IF_ERR_EXIT(phymod_phy_interface_config_get(&phy_access[0], 0,0,  &config_tmp));

        if (config->speed == config_tmp.data_rate) {
            _SOC_IF_ERR_EXIT(unimac_speed_get(unit, port, &mac_speed));
            if (config->speed == mac_speed) {
                SOC_FUNC_RETURN;
            }

            mac_only = 1;
        }

        _SOC_IF_ERR_EXIT(unimac_speed_set(unit, port, config->speed));

        if (mac_only) {
            /* line side speed and SerDes speed are equal, no need update SerDes configuration */
            SOC_FUNC_RETURN;
        }
    }


    if (config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        _SOC_IF_ERR_EXIT(
            PM4x10_QTC_INTERFACE_TYPE_SET(unit, pm_info, config->interface,
                                       port_index));
        is_interface_restore = 1;
        _SOC_IF_ERR_EXIT(
            PM4x10_QTC_IS_INTERFACE_RESTORE_SET(unit, pm_info,
                                             is_interface_restore, port_index));
        SOC_FUNC_RETURN;
    }

    _SOC_IF_ERR_EXIT(
        PM4x10_QTC_IS_INTERFACE_RESTORE_GET(unit, pm_info, &is_interface_restore,
                                             port_index));
    /*
     * if the interface is already set with PARAM_SET_ONLY
     * h/w interface needs to be updated during speed set.
     * So load the interface information and clear the flag;
     */
    if (is_interface_restore) {
        _SOC_IF_ERR_EXIT(
            PM4x10_QTC_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index));
        config_temp.interface = interface;

        is_interface_restore = 0;
        _SOC_IF_ERR_EXIT(
            PM4x10_QTC_IS_INTERFACE_RESTORE_SET(unit, pm_info,
                                       is_interface_restore, port_index));
    }

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(&config_temp, &interface));
    if (nof_phys > 1) {
        is_ext_phy = TRUE;
    }
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_interface_check(unit, port, pm_info, config, is_ext_phy, interface, &is_valid));
    if (!is_valid) {
        LOG_ERROR(BSL_LS_BCM_PORT,
              (BSL_META_UP(unit, port,
               "ERROR: u=%d p=%d interface type %d not supported by internal SERDES for this speed %d \n"),
               unit, port, interface, config->speed));
        _SOC_IF_ERR_EXIT(SOC_E_PARAM);
    }

    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                     interface, &phy_interface_config.interface_type));

    phy_interface_config.data_rate       = config->speed;
    phy_interface_config.interface_modes = config->interface_modes;
    phy_interface_config.ref_clock       = PM_4x10_QTC_INFO(pm_info)->ref_clk;
    if (config->serdes_interface != SOC_PORT_IF_NULL) {
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,
                         config->speed,
                         config->serdes_interface,
                         &phymod_serdes_interface));
    }

    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_interface_config_set(unit, port, phy_access, nof_phys,
                                                    config->flags,
                                                    &phy_interface_config,
                                                    phymod_serdes_interface,
                                                    PM_4x10_QTC_INFO(pm_info)->ref_clk,
                                                    phy_init_flags));

    _SOC_IF_ERR_EXIT(unimac_speed_set(unit, port, phy_interface_config.data_rate));

    _SOC_IF_ERR_EXIT(PM4x10_QTC_SPEED_SET(unit, pm_info, config->speed, port_index));
    _SOC_IF_ERR_EXIT(PM4x10_QTC_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interface_config_set(unit, port, PM4x10_INFO(pm_info), config, phy_init_flags));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_interface_config_set(unit, port, pm_info, config, phy_init_flags));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    int nof_phys;
    phymod_phy_inf_config_t     phy_interface_config;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int port_index, max_speed;
    uint32 bitmap;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM4x10_QTC_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index));

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));
    phy_interface_config.data_rate = max_speed;
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                    0, PM_4x10_QTC_INFO(pm_info)->ref_clk,
                                                    &phy_interface_config, phy_init_flags));

    config->speed = phy_interface_config.data_rate;

    _SOC_IF_ERR_EXIT(portmod_intf_from_phymod_intf(unit, phy_interface_config.interface_type, &config->interface));

    config->port_num_lanes = SOC_INFO(unit).port_num_lanes[port];

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interface_config_get(unit, port, PM4x10_INFO(pm_info), config, phy_init_flags));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_interface_config_get(unit, port, pm_info, config, phy_init_flags));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_default_interface_get(int unit, int port, pm_info_t pm_info,
                                      const portmod_port_interface_config_t* config,
                                      soc_port_if_t* interface)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    int is_legacy_phy;
    int xphy_id;
    portmod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (nof_phys==1) {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_default_interface_get(unit, port, config, interface));
    } else {
        xphy_id = phy_access[nof_phys-1].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));

        if (!is_legacy_phy) {
            _SOC_IF_ERR_EXIT(portmod_port_phymod_xphy_default_interface_get(unit,
                                                &(phy_access[nof_phys-1]), config, interface));
        } else {
            interface_config = *config;
            _SOC_IF_ERR_EXIT(portmod_common_default_interface_get(&interface_config));
            *interface = interface_config.interface;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_default_interface_get(unit, port, PM4x10_INFO(pm_info), config, interface));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_default_interface_get(unit, port,
                         pm_info, config, interface));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
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

int pm4x10_qtc_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_loopback_set(unit, port, PM4x10_INFO(pm_info), loopback_type, enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_loopback_set(unit, port, pm_info, loopback_type, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
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

int pm4x10_qtc_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_loopback_get(unit, port, PM4x10_INFO(pm_info), loopback_type, enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_loopback_get(unit, port, pm_info, loopback_type, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_core_access_get (int unit, int port,
                                 pm_info_t pm_info, int phyn, int max_cores,
                                 phymod_core_access_t* core_access_arr,
                                 int* nof_cores,
                                 int* is_most_ext)
{
    int port_index;
    uint32 bitmap;
    int index;
    int done=0;
    int i,j, is_legacy_phy;
    uint32 xphy_id;
    phymod_core_access_t core_access;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    if(phyn > PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
                       PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] - 1, phyn));
    }

    if(phyn < 0)
    {
        phyn = PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if( phyn == 0 ){
        sal_memcpy(&core_access_arr[0], &(PM_4x10_QTC_INFO(pm_info)->int_core_access),
                     sizeof(phymod_core_access_t));
        *nof_cores = 1;
    } else {
        index = 0;
        done = 0;
        for(i=0 ; (i< MAX_PORTS_PER_PM4X10_QTC); i++) {
            if(bitmap & (0x1U << i)) {
                xphy_id = PM_4x10_QTC_INFO(pm_info)->lane_conn[phyn-1][i].xphy_id;
                if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                    if(index == 0){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        sal_memcpy(&core_access_arr[index], &core_access,
                                   sizeof(phymod_core_access_t));
                        index++;
                    } else {
                        for( j = 0 ; (j < index) &&(!done) ; j++) {
                             if((!sal_memcmp (&core_access_arr[j], &core_access,
                                               sizeof(phymod_core_access_t)))) { /* found a match */
                                done = 1;
                            }
                        }
                        if((!done) && (index < max_cores)){
                            sal_memcpy(&core_access_arr[index], &core_access,
                                       sizeof(phymod_core_access_t));
                            index++;
                        }
                    }
                }
            }
        }
        *nof_cores = index;
    }
    if (is_most_ext) {
        if (phyn == PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_core_access_get(unit, port,
                        PM4x10_INFO(pm_info), phyn, max_cores, core_access_arr, nof_cores, is_most_ext));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_core_access_get(unit, port, pm_info,
                        phyn, max_cores, core_access_arr, nof_cores, is_most_ext));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                    const portmod_access_get_params_t* params,
                                    int max_phys,
                                    phymod_phy_access_t* access,
                                    int* nof_phys,
                                    int* is_most_ext)
{
    int rv;
    int phyn = 0;
    uint32 bitmap, lane_mask[MAX_NUM_CORES];
    int port_index;
    uint32  iphy_lane_mask = 0;
    int serdes_lane=0;
    int xphy_lane_mask=0;
    uint32 xphy_id = 0;
    phymod_core_access_t core_access;
    phymod_port_loc_t port_loc[MAX_NUM_CORES];
    int i, is_legacy_phy;
    uint32 xphy_idx;
    int num_of_phys;
    int done;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    SHR_BITSET(&iphy_lane_mask, port_index);

    if(params->phyn >= PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
           (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
           PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] - 1, params->phyn));
    }

    for( i = 0 ; i < max_phys; i++) {
        lane_mask[i] = 0;
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&access[i]));
    }

    phyn = params->phyn;

    /* if phyn is -1, it is looking for outer most phy.
    assumption is that all lane has same phy depth. */
    if(phyn < 0) {
        phyn = PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    if( phyn == 0 ) {
        /* internal core */
        sal_memcpy (&access[0], &(PM_4x10_QTC_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        access[0].access.lane_mask = 1 << port_index;
        if (params->lane != -1) {
            serdes_lane = port_index + params->lane;
            access[0].access.lane_mask &= (0x1 << serdes_lane );
            if (access[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        }
        *nof_phys = 1;
        access[0].port_loc = phymodPortLocLine; /* only line is availabe for internal. */

        /* if it is warm boot, get probed information from wb db instead of re-probing. */
        if(SOC_WARM_BOOT(unit)) {
            rv = PM4x10_QTC_PHY_TYPE_GET(unit, pm_info, &(access[0].type));
            _SOC_IF_ERR_EXIT(rv);
        }
    } else {
        /* external phy */
        num_of_phys = 0;

        /* xphy idx is 1 less than phyn since internal core does not count */
        xphy_idx = phyn-1;

        if (params->lane != -1) { /* specific lane */
            serdes_lane = port_index + params->lane;
            xphy_id = PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
            _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
            sal_memcpy (&access[num_of_phys], &core_access,
                        sizeof(phymod_phy_access_t));
            xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                          PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
            if (xphy_lane_mask != -1) {
                access[0].access.lane_mask |= xphy_lane_mask;
            }
            access[0].port_loc = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                       phymodPortLocSys : phymodPortLocLine;
            *nof_phys = 1;
        } else { /* all lanes */
            /* go thru all the lanes related to this port. */
            serdes_lane = 0;

            for (serdes_lane = 0; serdes_lane < MAX_PORTS_PER_PM4X10_QTC; serdes_lane++) {
                if (iphy_lane_mask & (1U<<serdes_lane)) {
                    xphy_id = PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
                    if(xphy_id != PORTMOD_XPHY_ID_INVALID) {
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        if(num_of_phys == 0 ) {/* first one */
                            sal_memcpy (&access[num_of_phys], &core_access,
                                        sizeof(phymod_phy_access_t));
                            xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                        PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                        PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                            if( xphy_lane_mask != -1) {
                                lane_mask[num_of_phys] |= xphy_lane_mask;
                            }
                            port_loc[num_of_phys] = (params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                                      phymodPortLocSys : phymodPortLocLine;
                            num_of_phys++;
                        } else {
                            /* if not the first one, see if this one sharing core with other lanes. */
                            done = 0;
                            for( i = 0 ; (i < num_of_phys) &&(!done) ; i++) {
                                 if((!sal_memcmp (&access[i], &core_access,
                                                   sizeof(phymod_phy_access_t)))) { /* found a match */
                                    /* update lane */
                                    xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                                PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                                PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                                    if( xphy_lane_mask != -1) {
                                        lane_mask[i] |= xphy_lane_mask;
                                    }
                                    done = 1;
                                }
                            }
                            if((!done) && (num_of_phys < max_phys)){
                                sal_memcpy (&access[num_of_phys], &core_access,
                                sizeof(phymod_phy_access_t));
                                xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                            PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                            PM_4x10_QTC_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                                if( xphy_lane_mask != -1) {
                                    lane_mask[num_of_phys] |= xphy_lane_mask;
                                }
                                port_loc[num_of_phys] = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                                                    phymodPortLocSys : phymodPortLocLine ;
                                num_of_phys++;
                            }
                        }
                    }
                }
            }
            *nof_phys = num_of_phys;
            for( i = 0 ; (i < *nof_phys) ; i++) {
                access[i].access.lane_mask = lane_mask[i];
                access[i].port_loc = port_loc[i];
            }
        }
    }

    if (is_most_ext) {
        if (phyn == PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }


exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_lane_access_get(unit, port,
                        PM4x10_INFO(pm_info), params, max_phys, access, nof_phys, is_most_ext));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_phy_lane_access_get(unit, port, pm_info,
                        params, max_phys, access, nof_phys, is_most_ext));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, const phymod_autoneg_control_t* an)
{
    int rv;
    uint32 bitmap, port_dynamic_state;
    int port_index;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    uint32 an_done;
    phymod_autoneg_control_t  *pAn = (phymod_autoneg_control_t*)an;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    if(PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (pAn->num_lane_adv == 0) {
            pAn->num_lane_adv = 1;
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            rv = PM4x10_QTC_AN_MODE_GET(unit, pm_info, &pAn->an_mode, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = phymod_AN_MODE_SGMII;
        }

        rv = PM4x10_QTC_PORT_DYNAMIC_STATE_GET(unit, pm_info,
                                       &port_dynamic_state, port_index);
        _SOC_IF_ERR_EXIT(rv);

        if (PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(port_dynamic_state)) {
            rv = PM4x10_QTC_AN_MODE_GET(unit, pm_info, &pAn->an_mode, port_index);
            _SOC_IF_ERR_EXIT(rv);

            PORTMOD_PORT_AUTONEG_MODE_UPDATED_CLR(port_dynamic_state);
            rv = PM4x10_QTC_PORT_DYNAMIC_STATE_SET(unit, pm_info,
                                               port_dynamic_state, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        /* In case of warm boot, get autoneg informaton from hardware */
        if (SOC_WARM_BOOT(unit)) {
            if (pAn->an_mode >= phymod_AN_MODE_Count) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_autoneg_get(unit, port, phy_access, nof_phys, phy_flags, pAn, &an_done));
            }
        }

        rv = portmod_port_phychain_autoneg_set(unit, port, phy_access, nof_phys, phy_flags, an);
        /* Because the returned value PHYMOD_E_UNAVAIL is -12 from tscf phymod driver */
        if (rv == PHYMOD_E_UNAVAIL) {
            rv = SOC_E_UNAVAIL;
        }
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, const phymod_autoneg_control_t* an)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_autoneg_set(unit, port,
                        PM4x10_INFO(pm_info), phy_flags, an));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_autoneg_set(unit, port, pm_info, phy_flags, an));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an)
{
    int rv;
    int port_index;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    uint32 bitmap, an_done;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    if (PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (an->num_lane_adv == 0) {
            an->num_lane_adv = 1;
        }
        rv = PM4x10_QTC_AN_MODE_GET(unit, pm_info, &an->an_mode, port_index);
        _SOC_IF_ERR_EXIT(rv);

        if (an->an_mode == phymod_AN_MODE_NONE) {
            an->an_mode = phymod_AN_MODE_SGMII;
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_get(unit, port, phy_access, nof_phys, phy_flags, an, &an_done));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_autoneg_get(unit, port,
                        PM4x10_INFO(pm_info), phy_flags, an));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_autoneg_get(unit, port, pm_info, phy_flags, an));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_ability_local_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    int port_index, rv;
    int max_speed;
    uint32 bitmap;
    portmod_port_ability_t legacy_phy_ability;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int    nof_phys, xphy_id, is_legacy_phy;
    SOC_INIT_FUNC_DEFS;

    sal_memset(ability, 0, sizeof(portmod_port_ability_t));
    sal_memset(&legacy_phy_ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                            phy_access, (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    rv = PM4x10_QTC_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->medium    = SOC_PA_MEDIUM_COPPER;
    ability->interface = SOC_PA_INTF_SGMII;
    /* QMODE doesn't support PHY loopback per subport */
    ability->loopback  = SOC_PA_LB_MAC;
    ability->fec       = SOC_PA_FEC_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE;

    switch(max_speed) {  /* must include all the supported speeds */
        /* coverity[unterminated_case] */
        case 2500:
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        /* coverity[unterminated_case] */
        case 1000:
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        /* coverity[unterminated_case] */
        case 100:
            ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
            ability->speed_half_duplex |= SOC_PA_SPEED_100MB;
        case 10:
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
            ability->speed_half_duplex |= SOC_PA_SPEED_10MB;
        default:
            break;
    }

    /* If legacy external phy presents, retrieve the local ability from the
     * external phy as it has more variants and the ability might vary for each.
     * However, if this function is called by the legacy ext PHY driver through
     * the PORTMOD callback function portmod_ext_to_int_cmd_get(), the returned
     * data should be of the internal SERDES. portmod_ext_to_int_cmd_get() uses
     * PORTMOD_USER_ACC_CMD_FOR_PHY_SET() to hint its callees that the target
     * of data retrieving is the internal SERDES.
     */
    if (nof_phys > 1 && !(phy_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY)) {
        /* check if the legacy phy present */
        xphy_id = phy_access[nof_phys-1].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
        if(is_legacy_phy) {
            /* call portmod phy chain function to retrieve the ability */
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_local_ability_get(unit, port, phy_access,
                                                         nof_phys, &legacy_phy_ability));
            /* PHY driver returns all the speeds supported by PHY
             * Advertise only the speeds supported by both PortMacro and the PHY
             */
            legacy_phy_ability.speed_full_duplex &= ability->speed_full_duplex;
            /* PHY driver may not return all supported encap modes
             * Advertise encap modes supported by both PortMacro and the PHY
             */
            legacy_phy_ability.encap |= ability->encap;
            *ability = legacy_phy_ability;
        } else {
            /* We might need to consider calling portmod phy chain for
             * non legacy phys as well*/
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_ability_local_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_ability_local_get(unit, port,
                        PM4x10_INFO(pm_info), phy_flags, ability));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_ability_local_get(unit, port, pm_info, phy_flags, ability));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0, port_index, rv;
    portmod_port_ability_t      port_ability;
    phymod_autoneg_ability_t    an_ability;
    portmod_access_get_params_t params;
    int port_num_lanes = 1, line_interface, cx4_10g = 0;
    int an_cl72 = 0, an_fec = 0, hg_mode = 0;
    uint32 bitmap;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT
        (pm4x10_qtc_port_ability_local_get(unit, port, pm_info, phy_flags, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.pause             &= ability->pause;
    port_ability.interface         &= ability->interface;
    port_ability.medium            &= ability->medium;
    port_ability.eee               &= ability->eee;
    port_ability.loopback          &= ability->loopback;
    port_ability.flags             &= ability->flags;
    port_ability.speed_half_duplex &= ability->speed_half_duplex;
    port_ability.speed_full_duplex &= ability->speed_full_duplex;
    port_ability.fec               &= ability->fec;
    port_ability.channel           &= ability->channel;

    rv = PM4x10_QTC_INTERFACE_TYPE_GET(unit, pm_info, &line_interface, port_index);
    _SOC_IF_ERR_EXIT(rv);

    portmod_common_port_to_phy_ability(&port_ability, &an_ability,
                                       port_num_lanes, line_interface, cx4_10g,
                                       an_cl72, an_fec, hg_mode);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_set(unit, port, phy_access, nof_phys, phy_flags,
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

int pm4x10_qtc_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_ability_advert_set(unit, port,
                        PM4x10_INFO(pm_info), phy_flags, ability));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_ability_advert_set(unit, port, pm_info, phy_flags, ability));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;
    phymod_autoneg_ability_t    an_ability;
    portmod_port_ability_t port_ability;
    int phy_index, xphy_id, is_legacy_phy = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_get(unit, port, phy_access, nof_phys, phy_flags,
                                                   &an_ability, &port_ability));

    phy_index = nof_phys -1;
    if (phy_index) {
        xphy_id = phy_access[phy_index].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
        if (is_legacy_phy) {
            sal_memcpy(ability, &port_ability, sizeof(portmod_port_ability_t));
        }
    }
    if (!is_legacy_phy) {
        portmod_common_phy_to_port_ability(&an_ability, ability);
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_ability_advert_get(unit, port,
                        PM4x10_INFO(pm_info), phy_flags, ability));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_ability_advert_get(unit, port, pm_info, phy_flags, ability));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interface_check(int unit, int port, pm_info_t pm_info,
                                const portmod_port_interface_config_t* config,
                                int is_ext_phy, soc_port_if_t interface,
                                int* is_valid)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interface_check(unit, port,
                        PM4x10_INFO(pm_info), config, is_ext_phy, interface, is_valid));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_interface_check(unit, port, pm_info, config,
                        is_ext_phy, interface, is_valid));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_update_dynamic_state(int unit, int port, pm_info_t pm_info, uint32_t port_dynamic_state)
{
    int rv = 0;
    int port_index;
    phymod_an_mode_type_t an_mode;
    uint32 bitmap, an_updated = FALSE, tmp_state;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get(unit, port, pm_info, &port_index, &bitmap));

    if (PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(port_dynamic_state)) {
        rv = PM4x10_QTC_PORT_DYNAMIC_STATE_GET(unit, pm_info, &tmp_state,
                                           port_index);
        _SOC_IF_ERR_EXIT(rv);
        /*we need to change the data base An_mode based on the new value */
        switch (port_dynamic_state >> 16 & 0xffff) {
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37:
            an_mode = phymod_AN_MODE_CL37;
            an_updated = TRUE;
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM:
            an_mode = phymod_AN_MODE_CL37BAM;
            an_updated = TRUE;
            break;
        default:
            break;
        }
    } else if (PORTMOD_PORT_IS_DEFAULT_TX_PARAMS_UPDATED(port_dynamic_state)) {
        rv = PM4x10_QTC_PORT_DYNAMIC_STATE_SET(unit, pm_info,
                                      port_dynamic_state, port_index);
        _SOC_IF_ERR_EXIT(rv);
    }
    if (an_updated == TRUE) {
        rv = PM4x10_QTC_AN_MODE_SET(unit, pm_info, an_mode, port_index);
        _SOC_IF_ERR_EXIT(rv);
        PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(tmp_state);
        rv = PM4x10_QTC_PORT_DYNAMIC_STATE_SET(unit, pm_info, tmp_state,
                                           port_index);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_update_dynamic_state(int unit, int port, pm_info_t pm_info, uint32_t port_dynamic_state)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_update_dynamic_state(unit, port,
                        PM4x10_INFO(pm_info), port_dynamic_state));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_update_dynamic_state(unit, port,
                        pm_info, port_dynamic_state));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_tx_average_ipg_set(int unit, int port, pm_info_t pm_info, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_average_ipg_set(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        _SOC_IF_ERR_EXIT(unimac_tx_average_ipg_set(unit, port, value));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_tx_average_ipg_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_average_ipg_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        _SOC_IF_ERR_EXIT(unimac_tx_average_ipg_get(unit, port, value));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_encap_set(int unit, int port, pm_info_t pm_info, int flags, portmod_encap_t encap)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_encap_set(unit, port,
                        PM4x10_INFO(pm_info), flags, encap));
    } else {
        _SOC_IF_ERR_EXIT(unimac_encap_set(unit, port, encap));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_encap_get(int unit, int port, pm_info_t pm_info, int *flags, portmod_encap_t *encap)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_encap_get(unit, port,
                        PM4x10_INFO(pm_info), flags, encap));
    } else {
        _SOC_IF_ERR_EXIT(unimac_encap_get(unit, port, encap));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_eee_set(unit, port,
                        PM4x10_INFO(pm_info), eee));
    } else {
        _SOC_IF_ERR_EXIT(unimac_eee_set(unit, port, eee));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_eee_get(int unit, int port, pm_info_t pm_info, portmod_eee_t* eee)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_eee_get(unit, port,
                        PM4x10_INFO(pm_info), eee));
    } else {
        _SOC_IF_ERR_EXIT(unimac_eee_get(unit, port, eee));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_speed_get(unit, port,
                        PM4x10_INFO(pm_info), speed));
    } else {
        _SOC_IF_ERR_EXIT(unimac_speed_get(unit, port, speed));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pfc_config_set(int unit, int port,pm_info_t pm_info, const portmod_pfc_config_t* pfc_cfg)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pfc_config_set(unit, port,
                        PM4x10_INFO(pm_info), pfc_cfg));
    } else {
        if (pfc_cfg->classes != 8) {
            return SOC_E_PARAM;
        }
        _SOC_IF_ERR_EXIT(unimac_pfc_config_set(unit, port, pfc_cfg));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pfc_config_get(int unit, int port,pm_info_t pm_info, portmod_pfc_config_t* pfc_cfg)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pfc_config_get(unit, port,
                        PM4x10_INFO(pm_info), pfc_cfg));
    } else {
        pfc_cfg->classes = 8;
        _SOC_IF_ERR_EXIT(unimac_pfc_config_get(unit, port, pfc_cfg));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pause_control_set(int unit, int port, pm_info_t pm_info, const portmod_pause_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pause_control_set(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        _SOC_IF_ERR_EXIT(unimac_pause_control_set(unit, port, control));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pause_control_get(int unit, int port, pm_info_t pm_info, portmod_pause_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pause_control_get(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        _SOC_IF_ERR_EXIT(unimac_pause_control_get(unit, port, control));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    int rv = 0;
    int nof_phys;
    phymod_duplex_mode_t duplex = phymodDuplexFull;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    rv = portmod_port_phychain_duplex_get(unit, port, phy_access, nof_phys, &duplex);
    if (rv == PHYMOD_E_NONE) {
        *enable = (int)duplex;
    } else {
        _SOC_IF_ERR_EXIT(unimac_duplex_get(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_duplex_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(unimac_duplex_set(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_duplex_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_duplex_get(unit, port, pm_info, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_cntmaxsize_get(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        _SOC_IF_ERR_EXIT(READ_GPORT_CNTMAXSIZEr(unit, port, (uint32_t *)val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_cntmaxsize_set(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        _SOC_IF_ERR_EXIT(WRITE_GPORT_CNTMAXSIZEr(unit, port, (uint32_t)val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_local_fault_control_set(int unit, int port, pm_info_t pm_info, const portmod_local_fault_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_local_fault_control_set(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
       return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_local_fault_control_get(int unit, int port, pm_info_t pm_info, portmod_local_fault_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_local_fault_control_get(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_local_fault_status_clear(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info, const portmod_remote_fault_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_remote_fault_control_set(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
       return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info, portmod_remote_fault_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_remote_fault_control_get(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_remote_fault_status_clear(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
       return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int rv = 0;
    int nof_phys = 0;
    uint32 xphy_id;
    int lane_mask;
    uint32 active_lanes;
    SOC_INIT_FUNC_DEFS;

    /* Validate parameters */
    _SOC_IF_ERR_EXIT(portmod_xphy_lane_connection_t_validate(unit,lane_connection));

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    PORTMOD_PBMP_ITER(PM_4x10_QTC_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    if (!SOC_WARM_BOOT(unit)) {
        rv = PM4x10_QTC_PORT_LANE_CONNECTION_DB_SET(unit, pm_info, lane_connection, phyn-1, lane_index);
        _SOC_IF_ERR_EXIT(rv);
        candidate_lane_conn = &(PM_4x10_QTC_INFO(pm_info)->lane_conn[phyn-1][lane_index]);
        sal_memcpy(candidate_lane_conn, lane_connection, sizeof(portmod_xphy_lane_connection_t));

        xphy_id = candidate_lane_conn->xphy_id;
        lane_mask = candidate_lane_conn->ss_lane_mask;

        /*
         * Keeping track of the xphy lane usage.  If the xphy lane is connected
         * it will set the corresponding bit to mark as lane is active.
         */

        if ((lane_mask != -1) && (xphy_id != PORTMOD_XPHY_ID_INVALID)) {
            _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_get(unit, xphy_id, &active_lanes));
            active_lanes |= lane_mask;
            _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_set(unit,xphy_id, active_lanes));
        }

        rv = PM4x10_QTC_PORT_NUM_PHYS_GET(unit, pm_info, &nof_phys, lane_index);
        _SOC_IF_ERR_EXIT(rv);
        if (nof_phys <= phyn) {
            nof_phys = phyn + 1;
            rv = PM4x10_QTC_PORT_NUM_PHYS_SET(unit, pm_info, &nof_phys, lane_index);
            _SOC_IF_ERR_EXIT(rv);
            PM_4x10_QTC_INFO(pm_info)->nof_phys[lane_index] = phyn+1;
        }
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x10_qtc_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    int phy_id_offset;
    SOC_INIT_FUNC_DEFS;

    phy_id_offset = iphy - PM_4x10_QTC_INFO(pm_info)->first_phy;
    /* In Eth mode, there are 4 EXT PHY ids connected with the same core. */
    if (phy_id_offset < PM4X10_QTC_LANES_PER_CORE) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_xphy_lane_attach_to_pm(unit,
                        PM4x10_INFO(pm_info), iphy, phyn, lane_connection));
    }
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_xphy_lane_attach_to_pm(unit, pm_info, iphy,
                    phyn, lane_connection));

exit:
   SOC_FUNC_RETURN;
}

int _pm4x10_qtc_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int rv = 0;
    int nof_phys = 0;
    uint32 active_lanes;
    uint32 xphy_id;
    int lane_mask;
    SOC_INIT_FUNC_DEFS;

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    PORTMOD_PBMP_ITER(PM_4x10_QTC_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    if (!SOC_WARM_BOOT(unit)) {
        candidate_lane_conn = &(PM_4x10_QTC_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

        xphy_id = candidate_lane_conn->xphy_id;
        lane_mask = candidate_lane_conn->ss_lane_mask;

        if ((lane_mask != -1) && (xphy_id != PORTMOD_XPHY_ID_INVALID)) {

            /*
            * Keeping track of the xphy lane usage.  If the xphy lane is disconnected
            * it will clear the corresponding bit to mark as lane is not active.
            * When none of the lane are active, it clear intialization status,
            * phy_type and probe status.  When it probe again, it will re-initialize
            * this xphy again.
            */
            _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_get(unit, xphy_id, &active_lanes));
            active_lanes &= ~lane_mask;
            _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_set(unit, xphy_id, active_lanes));

            if (active_lanes == 0) { /* none of the lanes are active */
                _SOC_IF_ERR_EXIT(portmod_xphy_core_probed_set(unit, xphy_id, 0));
                _SOC_IF_ERR_EXIT(portmod_xphy_phy_type_set(unit,xphy_id,phymodDispatchTypeInvalid));
                _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_set(unit, xphy_id,0));
            }
        }

        portmod_xphy_lane_connection_t_init(unit, candidate_lane_conn);
        rv = PM4x10_QTC_PORT_LANE_CONNECTION_DB_SET(unit, pm_info, candidate_lane_conn, phyn-1, lane_index);
        _SOC_IF_ERR_EXIT(rv);

        if (phyn > 0) {
            nof_phys = phyn;
            rv = PM4x10_QTC_PORT_NUM_PHYS_SET(unit, pm_info, &nof_phys, lane_index);
            _SOC_IF_ERR_EXIT(rv);
            PM_4x10_QTC_INFO(pm_info)->nof_phys[lane_index] = nof_phys;
        }
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x10_qtc_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    int phy_id_offset;
    SOC_INIT_FUNC_DEFS;

    phy_id_offset = iphy - PM_4x10_QTC_INFO(pm_info)->first_phy;

    if (phy_id_offset < PM4X10_QTC_LANES_PER_CORE) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_xphy_lane_detach_from_pm(unit,
                        PM4x10_INFO(pm_info), iphy, phyn, lane_connection));
    }
    _SOC_IF_ERR_EXIT(_pm4x10_qtc_xphy_lane_detach_from_pm(unit, pm_info, iphy,
                    phyn, lane_connection));

exit:
   SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_diag_fifo_status_get(int unit, int port,pm_info_t pm_info, const portmod_fifo_status_t* diag_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_diag_fifo_status_get(unit, port,
                        PM4x10_INFO(pm_info), diag_info));
    } else {
        _SOC_IF_ERR_EXIT(unimac_diag_fifo_status_get(unit, port, diag_info));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_drv_name_get(int unit, int port, pm_info_t pm_info, char* buf, int len)
{
    strncpy(buf, "PM4X10_QTC Driver", len);
    return (SOC_E_NONE);
}

int pm4x10_qtc_port_max_packet_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_max_packet_size_set(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        _SOC_IF_ERR_EXIT(unimac_rx_max_size_set(unit, port, value));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_max_packet_size_get (int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_max_packet_size_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        _SOC_IF_ERR_EXIT(unimac_rx_max_size_get(unit, port, value));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_rx_mac_enable_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(unimac_rx_enable_set(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_qtc_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_rx_mac_enable_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(unimac_rx_enable_get(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;

}

int pm4x10_qtc_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,const portmod_vlan_tag_t* vlan_tag)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_vlan_tag_set(unit, port,
                        PM4x10_INFO(pm_info), vlan_tag));
    } else {
        _SOC_IF_ERR_EXIT(unimac_rx_vlan_tag_set(unit, port, vlan_tag->outer_vlan_tag, vlan_tag->inner_vlan_tag));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_vlan_tag_get(unit, port,
                        PM4x10_INFO(pm_info), vlan_tag));
    } else {
        _SOC_IF_ERR_EXIT(unimac_rx_vlan_tag_get(unit, port, (int*)&vlan_tag->outer_vlan_tag, (int*)&vlan_tag->inner_vlan_tag));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pfc_control_set(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        _SOC_IF_ERR_EXIT(unimac_pfc_control_set(unit, port, control));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pfc_control_get(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        _SOC_IF_ERR_EXIT(unimac_pfc_control_get(unit, port, control));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_llfc_control_set(int unit, int port, pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_llfc_control_set(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_llfc_control_get(int unit, int port, pm_info_t pm_info, portmod_llfc_control_t* control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_llfc_control_get(unit, port,
                        PM4x10_INFO(pm_info), control));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT(portmod_port_phychain_link_status_get(unit, port, phy_access, nof_phys, flags, (uint32_t*) link));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_link_get(unit, port,
                        PM4x10_INFO(pm_info), flags, link));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_link_get(unit, port, pm_info, flags, link));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_mac_sa_set(unit, port,
                        PM4x10_INFO(pm_info), mac_addr));
    } else {
        _SOC_IF_ERR_EXIT(unimac_mac_sa_set(unit, port, mac_addr));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_mac_sa_get(unit, port,
                        PM4x10_INFO(pm_info), mac_addr));
    } else {
        _SOC_IF_ERR_EXIT(unimac_mac_sa_get(unit, port, mac_addr));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_rx_mac_sa_set(unit, port,
                        PM4x10_INFO(pm_info), mac_addr));
    } else {
        _SOC_IF_ERR_EXIT(unimac_mac_sa_set(unit, port, mac_addr));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_rx_mac_sa_get(unit, port,
                        PM4x10_INFO(pm_info), mac_addr));
    } else {
        _SOC_IF_ERR_EXIT(unimac_mac_sa_get(unit, port, mac_addr));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pad_size_set(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        /*Unimac tx padding is always enabled*/
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pad_size_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        /*Unimac tx padding is always enabled and min frame size is 64*/
        *value = 64;
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    int link;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    uint32_t serdes_link_status=0;
    phymod_phy_rx_lane_control_t rx_control;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if(update_control->link_status == -1) {
        _SOC_IF_ERR_EXIT(pm4x10_qtc_port_link_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &link));
    } else {
        link = update_control->link_status;
    }

    /* Following code applies to the phy 54290 */
    if (PORTMOD_PORT_UPDATE_F_UPDATE_SERDES_LINK_GET(update_control)){
        if (nof_phys > 1){
            _SOC_IF_ERR_EXIT
                (phymod_phy_link_status_get(&phy_access[0],&serdes_link_status));

            if(link) {
                _SOC_IF_ERR_EXIT
                    (phymod_phy_rx_lane_control_get(&phy_access[0],&rx_control));
                if(rx_control == phymodRxSquelchOn){
                    phymod_phy_rx_lane_control_set(&phy_access[0],
                                                   phymodRxSquelchOff);
                }
            } else {
                /* if external phy and link is down, internal serdes link is up,
                   force internal SerDes link to be down. */
                if(serdes_link_status){
                    phymod_phy_rx_lane_control_set(&phy_access[0],
                                                   phymodRxSquelchOn);
                    phymod_phy_rx_lane_control_set(&phy_access[0],
                                                   phymodRxSquelchOff);
                }
            }
        }
        /*
         * PORTMOD_PORT_UPDATE_F_UPDATE_SERDES_LINK is set for XGS devices. Since XGS devices
         * already update their MAC registers elsewhere, there is an early return below to
         * avoid redundant works.
         */
        return (SOC_E_NONE);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_update(int unit, int port, pm_info_t pm_info,
                        const portmod_port_update_control_t* update_control)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_update(unit, port,
                        PM4x10_INFO(pm_info), update_control));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_update(unit, port, pm_info, update_control));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int *nof_lanes)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_nof_lanes_get(unit, port,
                        PM4x10_INFO(pm_info), nof_lanes));
    } else {
        /*QSGMII ports always occupy only one lane*/
        *nof_lanes = 1;
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_tx_t          ln_txparam[PM4X10_QTC_LANES_PER_CORE];
    int nof_phys, i;
    uint32 lane_map;
    portmod_access_get_params_t params;

    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

    for (i = 0; i < PM4X10_QTC_LANES_PER_CORE; i++) {
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(&ln_txparam[i]));
    }

    if (PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT) {
        params.phyn = 0;
    } else {
        /* most external PHY_DIAG_DEV_DFLT and PHY_DIAG_DEV_EXT */
        params.phyn = -1;
    }

    if (PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS) {
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* line side is default */
        params.sys_side = PORTMOD_SIDE_LINE;
    }

    params.apply_lane_mask = 1;

    SOC_IF_ERROR_RETURN(pm4x10_qtc_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, phy_access, &nof_phys, NULL));

    lane_map = phy_access[0].access.lane_mask;

    switch (op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_qtc_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_DSC));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pmd_info_dump(unit, phy_access, nof_phys,
                                                 (void*)arg));
            break;

        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "pm4x10_qtc_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_PCS));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pcs_info_dump(unit, phy_access, nof_phys,
                                                    (void*)arg));
            break;

        default:
            if(op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "pm4x10_qtc_port_diag_ctrl: "
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
                                     "pm4x10_qtc_port_diag_ctrl: "
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

int pm4x10_qtc_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_diag_ctrl(unit, port,
                        PM4x10_INFO(pm_info), inst, op_type, op_cmd, arg));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_diag_ctrl(unit, port, pm_info, inst,
                        op_type, op_cmd, arg));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_autoneg_status_get(int unit, int port, pm_info_t pm_info,
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

int pm4x10_qtc_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_autoneg_status_get(unit, port,
                        PM4x10_INFO(pm_info), an_status));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_autoneg_status_get(unit, port, pm_info, an_status));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
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

int pm4x10_qtc_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_link_up_event(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_phy_link_up_event(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
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

int pm4x10_qtc_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_link_down_event(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_phy_link_down_event(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mode_set(int unit, int port, pm_info_t pm_info, const portmod_port_mode_info_t* mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mode_set(unit, port,
                        PM4x10_INFO(pm_info), mode));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_mode_get(int unit, int port, pm_info_t pm_info, portmod_port_mode_info_t* mode)
{
    int port_index;
    uint32 bitmap;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        _pm4x10_qtc_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    mode->port_index = port_index;
    mode->cur_mode = portmodPortModeQuad;
    mode->lanes = 1;

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mode_get(int unit, int port, pm_info_t pm_info, portmod_port_mode_info_t* mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mode_get(unit, port,
                        PM4x10_INFO(pm_info), mode));
    } else {
        _SOC_IF_ERR_EXIT(
            _pm4x10_qtc_port_mode_get(unit, port, pm_info, mode));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_reset_check(int unit, int port, pm_info_t pm_info, int enable, int* reset)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_reset_check(unit, port,
                        PM4x10_INFO(pm_info), enable, reset));
    } else {
        _SOC_IF_ERR_EXIT(unimac_reset_check(unit, port, enable, reset));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pcs_fsm_check(int unit, int port, pm_info_t pm_info, int* status)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    int nof_phys = 0;
    phymod_phy_access_t phy_access[1+MAX_PHYN];

    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (IS_QMOD) {
        _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));
        _SOC_IF_ERR_EXIT(phymod_phy_pcs_tx_fsm_check(&phy_access[0], (uint32 *)status));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_check_legacy_phy(int unit, int port, pm_info_t pm_info, int *legacy_phy)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys, xphy_id, is_legacy_phy;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    *legacy_phy = 0;

    if (PM_4x10_QTC_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
     while (nof_phys > 1) {
         nof_phys--;
         xphy_id = phy_access[nof_phys].access.addr;
         _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
         if (is_legacy_phy) {
             *legacy_phy = 1;
             break;
         }
     }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_check_legacy_phy(int unit, int port, pm_info_t pm_info, int* legacy_phy)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_check_legacy_phy(unit, port,
                        PM4x10_INFO(pm_info), legacy_phy));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_check_legacy_phy(unit, port, pm_info, legacy_phy));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_discard_set(unit, port,
                        PM4x10_INFO(pm_info), discard));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_core_num_get(int unit, int port, pm_info_t pm_info, int* core_num)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_core_num_get(unit, port,
                        PM4x10_INFO(pm_info), core_num));
    } else {
        *core_num = PM_4x10_QTC_INFO(pm_info)->core_num;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_ref_clk_get(int unit, int port, pm_info_t pm_info, int* ref_clk)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_ref_clk_get(unit, port,
                        PM4x10_INFO(pm_info), ref_clk));
    } else {
        *ref_clk = PM_4x10_QTC_INFO(pm_info)->ref_clk;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx, int val, int flags)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_soft_reset_set(unit, port,
                        PM4x10_INFO(pm_info), idx, val, flags));
    } else {
        _SOC_IF_ERR_EXIT(unimac_soft_reset_set(unit, port, val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_drain_cells_rx_enable(int unit, int port, pm_info_t pm_info, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_drain_cells_rx_enable(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        int flags = UNIMAC_ENABLE_SET_FLAGS_RX_EN | UNIMAC_ENABLE_SET_FLAGS_TX_EN;

        _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, flags, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_down(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        int flags = UNIMAC_ENABLE_SET_FLAGS_TX_EN;

        _SOC_IF_ERR_EXIT(unimac_enable_set(unit, port, flags, 0));
    }

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10_qtc_port_txfifo_cell_cnt_get(int unit, int port, pm_info_t pm_info, uint32* cnt)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_txfifo_cell_cnt_get(unit, port,
                        PM4x10_INFO(pm_info), cnt));
    } else {
        *cnt = 0;
    }

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10_qtc_port_drain_cell_get(int unit, int port, pm_info_t pm_info, portmod_drain_cells_t* drain_cells)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_drain_cell_get(unit, port,
                        PM4x10_INFO(pm_info), drain_cells));
    } else {
        _SOC_IF_ERR_EXIT(unimac_drain_cell_get(unit, port, drain_cells));
    }

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10_qtc_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_drain_cell_start(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int
pm4x10_qtc_port_drain_cell_stop(int unit, int port, pm_info_t pm_info, const portmod_drain_cells_t* drain_cells)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_drain_cell_stop(unit, port,
                        PM4x10_INFO(pm_info), drain_cells));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lag_failover_status_toggle(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lag_failover_status_toggle(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_flow_control_set (int unit, int port, pm_info_t pm_info,
                                  int merge_mode_en, int parallel_fc_en)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_flow_control_set(unit, port,
                        PM4x10_INFO(pm_info), merge_mode_en, parallel_fc_en));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_higig2_mode_set(int unit, int port, pm_info_t pm_info, int mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_higig2_mode_set(unit, port,
                        PM4x10_INFO(pm_info), mode));
    } else {
        if (mode != PORTMOD_ENCAP_IEEE_MODE) {
            return (SOC_E_UNAVAIL);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_higig_mode_set(int unit, int port, pm_info_t pm_info, int mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_higig_mode_set(unit, port,
                        PM4x10_INFO(pm_info), mode));
    } else {
        if (mode != PORTMOD_ENCAP_IEEE_MODE) {
            return (SOC_E_UNAVAIL);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_higig2_mode_get(int unit, int port, pm_info_t pm_info, int *mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_higig2_mode_get(unit, port,
                        PM4x10_INFO(pm_info), mode));
    } else {
        *(mode) = PORTMOD_ENCAP_IEEE_MODE;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_higig_mode_get(int unit, int port, pm_info_t pm_info, int *mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_higig_mode_get(unit, port,
                        PM4x10_INFO(pm_info), mode));
    } else {
        *(mode) = PORTMOD_ENCAP_IEEE_MODE;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_multi_get(int unit, int port, pm_info_t pm_info, portmod_multi_get_t* multi_get)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_multi_get(unit, port,
                        PM4x10_INFO(pm_info), multi_get));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_rsv_mask_set(unit, port,
                        PM4x10_INFO(pm_info), rsv_mask));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mib_reset_toggle(unit, port,
                        PM4x10_INFO(pm_info), port_index));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_medium_config_set(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_medium_config_set(unit, port, medium, config));

exit:
    SOC_FUNC_RETURN;
}


int pm4x10_qtc_port_medium_config_set(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_medium_config_set(unit, port,
                        PM4x10_INFO(pm_info), medium, config));
    } else {
        _SOC_IF_ERR_EXIT(
            _pm4x10_qtc_port_medium_config_set(unit, port, pm_info, medium, config));

    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_medium_config_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_medium_config_get(unit, port, medium, config));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_medium_config_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_medium_config_get(unit, port,
                        PM4x10_INFO(pm_info), medium, config));
    } else {
        _SOC_IF_ERR_EXIT(
            _pm4x10_qtc_port_medium_config_get(unit, port, pm_info, medium, config));

    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_medium_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t* medium)
{
    SOC_INIT_FUNC_DEFS;

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_medium_get(unit, port, medium));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_medium_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t* medium)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_medium_get(unit, port,
                        PM4x10_INFO(pm_info), medium));
    } else {
        _SOC_IF_ERR_EXIT(
            _pm4x10_qtc_port_medium_get(unit, port, pm_info, medium));

    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_pm_bypass_set(int unit, pm_info_t pm_info, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_pm_bypass_set(unit,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_pm_phys_get(unit,
                        PM4x10_INFO(pm_info), phys));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_cl72_set(unit, port, phy_access, nof_phys, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_cl72_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_cl72_set(unit, port, pm_info, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_cl72_get(unit, phy_access, nof_phys, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_cl72_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_cl72_get(unit, port, pm_info, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_cl72_status_get(int unit, int port, pm_info_t pm_info, phymod_cl72_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_cl72_status_get(unit, phy_access, nof_phys, status));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_cl72_status_get(int unit, int port, pm_info_t pm_info, phymod_cl72_status_t* status)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_cl72_status_get(unit, port,
                        PM4x10_INFO(pm_info), status));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_cl72_status_get(unit, port, pm_info, status));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_tx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_mac_enable_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(unimac_tx_enable_set(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_tx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_tx_mac_enable_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        _SOC_IF_ERR_EXIT(unimac_tx_enable_get(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_link_latch_down_get(int unit, int port, pm_info_t pm_info, uint32 flags, int* link)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_link_latch_down_get(unit, port,
                        PM4x10_INFO(pm_info), flags, link));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_prbs_config_set(unit, port,
                        PM4x10_INFO(pm_info), mode, flags, config));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_prbs_config_get(unit, port,
                        PM4x10_INFO(pm_info), mode, flags, config));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_prbs_enable_set(unit, port,
                        PM4x10_INFO(pm_info), mode, flags, enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_prbs_enable_get(unit, port,
                        PM4x10_INFO(pm_info), mode, flags, enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_prbs_status_get(unit, port,
                        PM4x10_INFO(pm_info), mode, flags, status));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_firmware_mode_set(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t fw_mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_firmware_mode_set(unit, port,
                        PM4x10_INFO(pm_info), fw_mode));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_firmware_mode_get(unit, port,
                        PM4x10_INFO(pm_info), fw_mode));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_runt_threshold_set(int unit, int port, pm_info_t pm_info, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_runt_threshold_set(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_runt_threshold_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_local_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_local_fault_status_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_remote_fault_status_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_remote_fault_status_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32* value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_read(unit, port, phy_access, nof_phys, lane, flags, reg_addr,value));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32 value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_write(unit, port, phy_access, nof_phys, lane, flags, reg_addr,value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_reg_read(unit, port,
                        PM4x10_INFO(pm_info), lane, flags, reg_addr, value));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_phy_reg_read(unit, port,
                             pm_info, lane, flags, reg_addr, value));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32 value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_reg_write(unit, port,
                        PM4x10_INFO(pm_info), lane, flags, reg_addr, value));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_phy_reg_write(unit, port,
                             pm_info, lane, flags, reg_addr, value));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_reset_set(int unit, int port, pm_info_t pm_info, int mode, int opcode, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_reset_set(unit, port,
                        PM4x10_INFO(pm_info), mode, opcode, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_reset_get(int unit, int port, pm_info_t pm_info, int mode, int opcode, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_reset_get(unit, port,
                        PM4x10_INFO(pm_info), mode, opcode, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_adv_remote_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_adv_remote_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_ability_remote_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                       portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                         nof_phys;
    phymod_autoneg_ability_t    an_ability;
    int phy_index, xphy_id, is_legacy_phy = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                          (1+MAX_PHYN), &nof_phys));

    phy_index = nof_phys -1;
    if (phy_index) {
        xphy_id = phy_access[phy_index].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
    }
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_remote_ability_get(unit, port,
                    phy_access, nof_phys, phy_flags, &an_ability, ability));
    if (!is_legacy_phy) {
        portmod_common_phy_to_port_ability(&an_ability, ability);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_ability_remote_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_ability_remote_get(unit, port,
                        PM4x10_INFO(pm_info), phy_flags, ability));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_ability_remote_get(unit, port, pm_info,
                        phy_flags, ability));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_frame_spacing_stretch_set(int unit, int port, pm_info_t pm_info, int spacing)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_frame_spacing_stretch_set(unit, port,
                        PM4x10_INFO(pm_info), spacing));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_frame_spacing_stretch_get(int unit, int port, pm_info_t pm_info, const int* spacing)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_frame_spacing_stretch_get(unit, port,
                        PM4x10_INFO(pm_info), spacing));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_rx_control_set(int unit, int port, pm_info_t pm_info, const portmod_rx_control_t* rx_ctrl)
{
    portmod_pfc_config_t pfc_cfg;
    SOC_INIT_FUNC_DEFS;

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_CONTROL_FRAME) {
        _SOC_IF_ERR_EXIT
            (unimac_pass_control_frame_set(unit, port, rx_ctrl->pass_control_frames));
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PFC_FRAME) {
        _SOC_IF_ERR_EXIT(unimac_pfc_config_get(unit, port, &pfc_cfg));
        pfc_cfg.rxpass = rx_ctrl->pass_pfc_frames;
        _SOC_IF_ERR_EXIT(unimac_pfc_config_set(unit, port, &pfc_cfg));
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PAUSE_FRAME) {
        _SOC_IF_ERR_EXIT
            (unimac_pass_pause_frame_set(unit, port, rx_ctrl->pass_pause_frames));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_rx_control_set(int unit, int port, pm_info_t pm_info, const portmod_rx_control_t* rx_ctrl)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_rx_control_set(unit, port,
                        PM4x10_INFO(pm_info), rx_ctrl));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_rx_control_set(unit, port, pm_info, rx_ctrl));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_rx_control_get(int unit, int port, pm_info_t pm_info, portmod_rx_control_t* rx_ctrl)
{
    portmod_pfc_config_t pfc_cfg;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (unimac_pass_control_frame_get(unit, port, (int*)&rx_ctrl->pass_control_frames));
    _SOC_IF_ERR_EXIT(unimac_pfc_config_get(unit, port, &pfc_cfg));
    rx_ctrl->pass_pfc_frames = pfc_cfg.rxpass;
    _SOC_IF_ERR_EXIT
        (unimac_pass_pause_frame_get(unit, port, &rx_ctrl->pass_pause_frames));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_rx_control_get(int unit, int port, pm_info_t pm_info, portmod_rx_control_t* rx_ctrl)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_rx_control_get(unit, port,
                        PM4x10_INFO(pm_info), rx_ctrl));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_rx_control_get(unit, port, pm_info, rx_ctrl));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_eee_clock_set(int unit, int port, pm_info_t pm_info, const portmod_eee_clock_t* eee_clk)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_eee_clock_set(unit, port,
                        PM4x10_INFO(pm_info), eee_clk));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_eee_clock_get(int unit, int port, pm_info_t pm_info, portmod_eee_clock_t* eee_clk)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_eee_clock_get(unit, port,
                        PM4x10_INFO(pm_info), eee_clk));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_modid_set(int unit, int port, pm_info_t pm_info, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_modid_set(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_clear_rx_lss_status_set(int unit, int port, pm_info_t pm_info, int lcl_fault, int rmt_fault)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_clear_rx_lss_status_set(unit, port,
                        PM4x10_INFO(pm_info), lcl_fault, rmt_fault));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_clear_rx_lss_status_get(int unit, int port, pm_info_t pm_info, int* lcl_fault, int* rmt_fault)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_clear_rx_lss_status_get(unit, port,
                        PM4x10_INFO(pm_info), lcl_fault, rmt_fault));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lag_failover_loopback_set(int unit, int port, pm_info_t pm_info, int value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lag_failover_loopback_set(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lag_failover_loopback_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lag_failover_loopback_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_config_port_type_set(int unit, int port, pm_info_t pm_info, int type)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_config_port_type_set(unit, port,
                        PM4x10_INFO(pm_info), type));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_config_port_type_get(int unit, int port, pm_info_t pm_info, int* type)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_config_port_type_get(unit, port,
                        PM4x10_INFO(pm_info), type));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_trunk_hwfailover_config_set(int unit, int port, pm_info_t pm_info, int hw_count)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_trunk_hwfailover_config_set(unit, port,
                        PM4x10_INFO(pm_info), hw_count));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_trunk_hwfailover_config_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_trunk_hwfailover_config_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_trunk_hwfailover_status_get(int unit, int port, pm_info_t pm_info, int* loopback)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_trunk_hwfailover_status_get(unit, port,
                        PM4x10_INFO(pm_info), loopback));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lag_failover_disable(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lag_remove_failover_lpbk_set(int unit, int port, pm_info_t pm_info, int val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lag_remove_failover_lpbk_set(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lag_remove_failover_lpbk_get(int unit, int port, pm_info_t pm_info, int* val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lag_remove_failover_lpbk_get(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_egress_queue_drain_rx_en(int unit, int port, pm_info_t pm_info, int rx_en)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_egress_queue_drain_rx_en(unit, port,
                        PM4x10_INFO(pm_info), rx_en));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_ctrl_set(int unit, int port, pm_info_t pm_info, uint64 ctrl)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_ctrl_set(unit, port,
                        PM4x10_INFO(pm_info), ctrl));
    } else {
       uint32 mac_ctrl;

       COMPILER_64_TO_32_LO(mac_ctrl, ctrl);

       _SOC_IF_ERR_EXIT(unimac_mac_ctrl_set(unit, port, mac_ctrl));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_egress_queue_drain_get(int unit, int port, pm_info_t pm_info, uint64* ctrl, int* rxen)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_egress_queue_drain_get(unit, port,
                        PM4x10_INFO(pm_info), ctrl, rxen));
    } else {
        uint32 mac_ctrl;

        _SOC_IF_ERR_EXIT(unimac_mac_ctrl_get(unit, port, &mac_ctrl, rxen));
        COMPILER_64_SET(*ctrl, 0, mac_ctrl);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_reset_set(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        _SOC_IF_ERR_EXIT(unimac_soft_reset_set(unit, port, val));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_reset_get(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        _SOC_IF_ERR_EXIT(unimac_soft_reset_get(unit, port, val));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    int phy_acc = port;
    uint32 reg_val, old_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_COMMAND_CONFIGr(unit, phy_acc, &reg_val));
    old_val = reg_val;

    soc_reg_field_set(unit, COMMAND_CONFIGr, &reg_val, SW_RESETf, 1);

    _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_COMMAND_CONFIGr(unit, phy_acc, old_val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_reset_set(unit, port,
                        PM4x10_INFO(pm_info), idx));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_soft_reset_toggle(unit, port, pm_info, idx));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_e2ecc_hdr_set(int unit, int port, pm_info_t pm_info, const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_e2ecc_hdr_set(unit, port,
                        PM4x10_INFO(pm_info), e2ecc_hdr));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_e2ecc_hdr_get(int unit, int port, pm_info_t pm_info, portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_e2ecc_hdr_get(unit, port,
                        PM4x10_INFO(pm_info), e2ecc_hdr));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_e2e_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_e2e_enable_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_e2e_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_e2e_enable_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_fallback_lane_get(int unit, int port, pm_info_t pm_info, int* fallback_lane)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_fallback_lane_get(unit, port,
                        PM4x10_INFO(pm_info), fallback_lane));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pgw_reconfig(int unit, int port, pm_info_t pm_info, const portmod_port_mode_info_t* pmode, int phy_port, int flags)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pgw_reconfig(unit, port,
                        PM4x10_INFO(pm_info), pmode, phy_port, flags));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_notify(int unit, int port, pm_info_t pm_info, int link)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_notify(unit, port,
                        PM4x10_INFO(pm_info), link));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_control_phy_timesync_set(int unit, int port, pm_info_t pm_info, portmod_port_control_phy_timesync_t config, uint64 value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_control_phy_timesync_set(unit, port,
                        PM4x10_INFO(pm_info), config, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_control_phy_timesync_get(int unit, int port, pm_info_t pm_info, portmod_port_control_phy_timesync_t config, uint64* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_control_phy_timesync_get(unit, port,
                        PM4x10_INFO(pm_info), config, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_config_set(int unit, int port, pm_info_t pm_info, const portmod_phy_timesync_config_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_config_set(unit, port,
                        PM4x10_INFO(pm_info), config));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_config_get(int unit, int port, pm_info_t pm_info, portmod_phy_timesync_config_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_config_get(unit, port,
                        PM4x10_INFO(pm_info), config));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_enable_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_enable_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_enable_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_enable_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_nco_addend_set(int unit, int port, pm_info_t pm_info, uint32 freq_step)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_nco_addend_set(unit, port,
                        PM4x10_INFO(pm_info), freq_step));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_nco_addend_get(int unit, int port, pm_info_t pm_info, uint32* freq_step)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_nco_addend_get(unit, port,
                        PM4x10_INFO(pm_info), freq_step));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_framesync_mode_set(int unit, int port, pm_info_t pm_info, const portmod_timesync_framesync_t* framesync)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_framesync_mode_set(unit, port,
                        PM4x10_INFO(pm_info), framesync));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_framesync_mode_get(int unit, int port, pm_info_t pm_info, portmod_timesync_framesync_t* framesync)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_framesync_mode_get(unit, port,
                        PM4x10_INFO(pm_info), framesync));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_local_time_set(int unit, int port, pm_info_t pm_info, uint64 local_time)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_local_time_set(unit, port,
                        PM4x10_INFO(pm_info), local_time));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_local_time_get(int unit, int port, pm_info_t pm_info, uint64* local_time)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_local_time_get(unit, port,
                        PM4x10_INFO(pm_info), local_time));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_load_ctrl_set(int unit, int port, pm_info_t pm_info, uint32 load_once, uint32 load_always)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_load_ctrl_set(unit, port,
                        PM4x10_INFO(pm_info), load_once, load_always));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_load_ctrl_get(int unit, int port, pm_info_t pm_info, uint32* load_once, uint32* load_always)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_load_ctrl_get(unit, port,
                        PM4x10_INFO(pm_info), load_once, load_always));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_tx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_tx_timestamp_offset_set(unit, port,
                        PM4x10_INFO(pm_info), ts_offset));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_tx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_tx_timestamp_offset_get(unit, port,
                        PM4x10_INFO(pm_info), ts_offset));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_rx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_rx_timestamp_offset_set(unit, port,
                        PM4x10_INFO(pm_info), ts_offset));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_rx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_rx_timestamp_offset_get(unit, port,
                        PM4x10_INFO(pm_info), ts_offset));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_intr_enable_set(int unit, int port, pm_info_t pm_info, uint32 intr_enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_intr_enable_set(unit, port,
                        PM4x10_INFO(pm_info), intr_enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_intr_enable_get(int unit, int port, pm_info_t pm_info, uint32* intr_enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_intr_enable_get(unit, port,
                        PM4x10_INFO(pm_info), intr_enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_intr_status_get(int unit, int port, pm_info_t pm_info, uint32* intr_status)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_intr_status_get(unit, port,
                        PM4x10_INFO(pm_info), intr_status));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_intr_status_clear(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_intr_status_clear(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_timesync_do_sync(int unit, int port, pm_info_t pm_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_timesync_do_sync(unit, port,
                        PM4x10_INFO(pm_info)));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_capture_timestamp_get(int unit, int port, pm_info_t pm_info, uint64* cap_ts)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_capture_timestamp_get(unit, port,
                        PM4x10_INFO(pm_info), cap_ts));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_heartbeat_timestamp_get(int unit, int port, pm_info_t pm_info, uint64* hb_ts)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_heartbeat_timestamp_get(unit, port,
                        PM4x10_INFO(pm_info), hb_ts));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_edc_config_set(int unit, int port, pm_info_t pm_info, const portmod_edc_config_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_edc_config_set(unit, port,
                        PM4x10_INFO(pm_info), config));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_edc_config_get(int unit, int port, pm_info_t pm_info, portmod_edc_config_t* config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_edc_config_get(unit, port,
                        PM4x10_INFO(pm_info), config));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interrupt_enable_set(int unit, int port, pm_info_t pm_info, int intr_type, uint32 val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interrupt_enable_set(unit, port,
                        PM4x10_INFO(pm_info), intr_type, val));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interrupt_enable_get(int unit, int port, pm_info_t pm_info, int intr_type, uint32* val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interrupt_enable_get(unit, port,
                        PM4x10_INFO(pm_info), intr_type, val));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interrupt_get(int unit, int port, pm_info_t pm_info, int intr_type, uint32* val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interrupt_get(unit, port,
                        PM4x10_INFO(pm_info), intr_type, val));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_interrupts_get(int unit, int port, pm_info_t pm_info, int arr_max_size, uint32* intr_arr, uint32* size)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_interrupts_get(unit, port,
                        PM4x10_INFO(pm_info), arr_max_size, intr_arr, size));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_failover_mode_set(unit, port,
                        PM4x10_INFO(pm_info), failover));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_failover_mode_get(unit, port,
                        PM4x10_INFO(pm_info), failover));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x10_qtc_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info)
{
    if (PM_4x10_QTC_INFO(pm_info)->warmboot_skip_db_restore) {
       return (SOC_E_NONE);
    } else {
       return (SOC_E_UNAVAIL);
    }

}

int pm4x10_qtc_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* intf_config, const portmod_port_init_config_t* init_config, phymod_operation_mode_t phy_op_mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_warmboot_db_restore(unit, port,
                        PM4x10_INFO(pm_info), intf_config, init_config, phy_op_mode));
    } else {
        _SOC_IF_ERR_EXIT(
            _pm4x10_qtc_port_warmboot_db_restore(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info, phymod_operation_mode_t* val)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_phy_op_mode_get(unit, port,
                        PM4x10_INFO(pm_info), val));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lane_map_set(unit, port,
                        PM4x10_INFO(pm_info), flags, lane_map));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lane_map_get(unit, port,
                        PM4x10_INFO(pm_info), flags, lane_map));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_polarity_set(unit, port,
                        PM4x10_INFO(pm_info), polarity));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_polarity_get(unit, port,
                        PM4x10_INFO(pm_info), polarity));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_pm_is_in_pm12x10(int unit, pm_info_t pm_info, int* in_pm12x10)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_pm_is_in_pm12x10(unit,
                        PM4x10_INFO(pm_info), in_pm12x10));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_pll_div_get(int unit, int port, pm_info_t pm_info, const portmod_port_resources_t* port_resource, uint32_t* pll_div)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_pll_div_get(unit, port,
                        PM4x10_INFO(pm_info), port_resource, pll_div));
    } else {
        *pll_div = (qtc_mode == portmodQtcModeUsxgmii) ? phymod_TSCE_PLL_DIV66 : phymod_TSCE_PLL_DIV64;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_master_set(int unit, int port, pm_info_t pm_info, int master_mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_master_set(unit, port,
                        PM4x10_INFO(pm_info), master_mode));
    } else {
        _SOC_IF_ERR_EXIT(portmod_port_phychain_master_set(unit, port, pm_info, master_mode));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_master_get(int unit, int port, pm_info_t pm_info, int* master_mode)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_master_get(unit, port,
                        PM4x10_INFO(pm_info), master_mode));
    } else {
        _SOC_IF_ERR_EXIT(portmod_port_phychain_master_get(unit, port, pm_info, master_mode));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_preemption_control_set(int unit, int port, pm_info_t pm_info, portmod_preemption_control_t type, uint32 value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_preemption_control_set(unit, port,
                        PM4x10_INFO(pm_info), type, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_preemption_control_get(int unit, int port, pm_info_t pm_info, portmod_preemption_control_t type, uint32* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_preemption_control_get(unit, port,
                        PM4x10_INFO(pm_info), type, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_preemption_tx_status_get(int unit, int port, pm_info_t pm_info, uint32* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_preemption_tx_status_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_preemption_verify_status_get(int unit, int port, pm_info_t pm_info, portmod_preemption_verify_status_type_t type, uint32* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_preemption_verify_status_get(unit, port,
                        PM4x10_INFO(pm_info), type, value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_preemption_rx_timeout_set(int unit, int port, pm_info_t pm_info, uint32 enable, uint32 timeout_cnt)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_preemption_rx_timeout_set(unit, port,
                        PM4x10_INFO(pm_info), enable, timeout_cnt));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_preemption_rx_timeout_get(int unit, int port, pm_info_t pm_info, uint32* enable, uint32* timeout_cnt)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_preemption_rx_timeout_get(unit, port,
                        PM4x10_INFO(pm_info), enable, timeout_cnt));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lane_count_get(int unit, int port, pm_info_t pm_info, int line_side, int* num_lanes)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lane_count_get(unit, port,
                        PM4x10_INFO(pm_info), line_side, num_lanes));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_timesync_tx_info_get(unit, port,
                        PM4x10_INFO(pm_info), tx_info));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_lane_mask_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int* lane_mask, int* xphy_id)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_lane_mask_get(unit, port,
                        PM4x10_INFO(pm_info), params, lane_mask, xphy_id));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_timestamp_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_timestamp_enable_set(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
       return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_mac_timestamp_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_mac_timestamp_enable_get(unit, port,
                        PM4x10_INFO(pm_info), enable));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_txpi_override_set(int unit, int port, pm_info_t pm_info, const portmod_txpi_override_t* override)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_txpi_override_set(unit, port,
                        PM4x10_INFO(pm_info), override));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_txpi_override_get(int unit, int port, pm_info_t pm_info, portmod_txpi_override_t* override)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_txpi_override_get(unit, port,
                        PM4x10_INFO(pm_info), override));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_txpi_sdm_data_get(int unit, int port, pm_info_t pm_info, int* value)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_txpi_sdm_data_get(unit, port,
                        PM4x10_INFO(pm_info), value));
    } else {
        return (SOC_E_UNAVAIL);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
    phy_synce_cfg.stg0_mode = cfg->stg0_mode;
    phy_synce_cfg.stg1_mode = cfg->stg1_mode;
    phy_synce_cfg.sdm_val = cfg->sdm_val;

    _SOC_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_set(&phy_access[0], phy_synce_cfg));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                   portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);

    _SOC_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_get(&phy_access[0], &phy_synce_cfg));

    cfg->stg0_mode = phy_synce_cfg.stg0_mode;
    cfg->stg1_mode = phy_synce_cfg.stg1_mode;
    cfg->sdm_val = phy_synce_cfg.sdm_val;

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_port_synce_clk_ctrl_t* cfg)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_synce_clk_ctrl_set(unit, port,
                        PM4x10_INFO(pm_info), cfg));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_synce_clk_ctrl_set(unit, port, pm_info, cfg));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                   portmod_port_synce_clk_ctrl_t* cfg)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_port_synce_clk_ctrl_get(unit, port,
                        PM4x10_INFO(pm_info), cfg));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_port_synce_clk_ctrl_get(unit, port, pm_info, cfg));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x10_qtc_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                              portmod_egr_1588_timestamp_config_t timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    SOC_INIT_FUNC_DEFS;

    if (timestamp_config.timestamp_mode == portmodTimestampMode32bit) {
        timestamp_mode = PM4X10_QTC_EGR_1588_TIMESTAMP_MODE_32BIT;
    } else if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
        timestamp_mode = PM4X10_QTC_EGR_1588_TIMESTAMP_MODE_48BIT;
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

STATIC
int _pm4x10_qtc_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                              portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, &reg_val));
    timestamp_mode = soc_reg_field_get(unit, GPORT_MODE_REGr, reg_val,
                                       EGR_1588_TIMESTAMPING_MODEf);

    if (timestamp_mode == PM4X10_QTC_EGR_1588_TIMESTAMP_MODE_32BIT) {
        timestamp_config->timestamp_mode = portmodTimestampMode32bit;
    } else {
        timestamp_config->timestamp_mode = portmodTimestampMode48bit;
    }

    timestamp_config->cmic_48_overr_en = 0;

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                              portmod_egr_1588_timestamp_config_t timestamp_config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_egr_1588_timestamp_config_set(unit, port,
                                             PM4x10_INFO(pm_info), timestamp_config));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_egr_1588_timestamp_config_set(unit, port, pm_info,
                                                                   timestamp_config));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x10_qtc_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                             portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    PM4x10_QTC_LOCAL_VARIABLE_DEFS;
    SOC_INIT_FUNC_DEFS;

    PM4x10_QTC_QMOD_GET(pm_info);

    if (!IS_QMOD) {
        _SOC_IF_ERR_EXIT(
            PM_DRIVER(PM4x10_INFO(pm_info))->f_portmod_egr_1588_timestamp_config_get(unit, port,
                                                PM4x10_INFO(pm_info), timestamp_config));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x10_qtc_egr_1588_timestamp_config_get(unit, port, pm_info,
                                                                    timestamp_config));
    }

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM4X10_QTC_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

