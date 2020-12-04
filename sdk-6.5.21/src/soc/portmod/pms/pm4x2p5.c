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
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/portmod/portmod_system.h>
#include <soc/portmod/pm4x10.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X2P5_SUPPORT

#include <soc/portmod/xlmac.h>
#include <soc/portmod/pm4x2p5.h>
#include <soc/portmod/pm4x2p5_shared.h>

#define PM4x2p5_WB_BUFFER_VERSION        (2)

#define PM4x2P5_SPEED_SET(unit, pm_info, speed, port_index)     \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,           \
                              pm_info->wb_vars_ids[dataRate],       \
                              &speed, port_index)
#define PM4x2P5_SPEED_GET(unit, pm_info, speed, port_index)     \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,           \
                              pm_info->wb_vars_ids[dataRate],       \
                              speed, port_index)
#define PM4x2P5_MAX_SPEED_SET(unit, pm_info, max_speed, port_index) \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,               \
                              pm_info->wb_vars_ids[maxSpeed],           \
                              &max_speed, port_index)
#define PM4x2P5_MAX_SPEED_GET(unit, pm_info, max_speed, port_index) \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,               \
                              pm_info->wb_vars_ids[maxSpeed],           \
                              max_speed, port_index)
#define PM4x2P5_INTERFACE_TYPE_SET(unit, pm_info, interface_type, port_index) \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[interfaceType],                 \
                              &interface_type, port_index)
#define PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, interface_type, port_index) \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[interfaceType],                 \
                              interface_type, port_index)
#define PM4x2P5_SERDES_INTERFACE_TYPE_SET(unit, pm_info, serdes_interface,    \
                                             port_index)                         \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[serdesInterfaceType],         \
                              &serdes_interface, port_index)
#define PM4x2P5_SERDES_INTERFACE_TYPE_GET(unit, pm_info, serdes_interface,    \
                                             port_index)                         \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[serdesInterfaceType],         \
                              serdes_interface, port_index)
#define PM4x2P5_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index) \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                           \
                              pm_info->wb_vars_ids[interfaceModes],                \
                              &interface_modes, port_index)
#define PM4x2P5_INTERFACE_MODES_GET(unit, pm_info, interface_modes, port_index) \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                           \
                              pm_info->wb_vars_ids[interfaceModes],                \
                              interface_modes, port_index)
#define PM4x2P5_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, \
                                            port_index)                          \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[isInterfaceRestore],          \
                              &is_interface_restore, port_index)
#define PM4x2P5_IS_INTERFACE_RESTORE_GET(unit, pm_info, is_interface_restore, \
                                            port_index)                          \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[isInterfaceRestore],          \
                              is_interface_restore, port_index)
#define PM4x2P5_AN_MODE_SET(unit, pm_info, an_mode, port_index)               \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[anMode], &an_mode, port_index)
#define PM4x2P5_AN_MODE_GET(unit, pm_info, an_mode, port_index)               \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[anMode], an_mode, port_index)
#define PM4x2P5_AN_CL37_SET(unit, pm_info, an_cl37, port_index)              \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                        \
                              pm_info->wb_vars_ids[anCl37], &an_cl37, port_index)
#define PM4x2P5_AN_CL37_GET(unit, pm_info, an_cl37, port_index)              \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                        \
                              pm_info->wb_vars_ids[anCl37], an_cl37, port_index)
#define PM4x2P5_CL37_SGMII_CNT_SET(unit, pm_info, cl37_sgmii_cnt, port_index)  \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                          \
                              pm_info->wb_vars_ids[cl37SgmiiCnt], &cl37_sgmii_cnt,\
                              port_index)
#define PM4x2P5_CL37_SGMII_CNT_GET(unit, pm_info, cl37_sgmii_cnt, port_index)  \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                          \
                              pm_info->wb_vars_ids[cl37SgmiiCnt], cl37_sgmii_cnt, \
                              port_index)
#define PM4x2P5_CL37_SGMII_RESTART_CNT_SET(unit, pm_info,                     \
                                              cl37_sgmii_restart_cnt, port_index)\
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[cl37SgmiiRestartCnt],         \
                              &cl37_sgmii_restart_cnt, port_index)
#define PM4x2P5_CL37_SGMII_RESTART_CNT_GET(unit, pm_info,                     \
                                              cl37_sgmii_restart_cnt, port_index)\
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[cl37SgmiiRestartCnt],         \
                              cl37_sgmii_restart_cnt, port_index)
#define PM4x2P5_TX_PARAMS_USER_FLAG_SET(unit, pm_info,                        \
                                           tx_params_user_flag, port_index)      \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[txParamsUserFlag],             \
                              &tx_params_user_flag, port_index)
#define PM4x2P5_TX_PARAMS_USER_FLAG_GET(unit, pm_info,                        \
                                           tx_params_user_flag, port_index)      \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[txParamsUserFlag],             \
                              tx_params_user_flag, port_index)

#define PM4x2P5_EXT_PHY_TX_PARAMS_USER_FLAG_SET(unit, pm_info,                \
                                       ext_phy_tx_params_user_flag, port_index)  \
        SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[extPhyTxParamsUserFlag],      \
                              &ext_phy_tx_params_user_flag, port_index)
#define PM4x2P5_EXT_PHY_TX_PARAMS_USER_FLAG_GET(unit, pm_info,                \
                                       ext_phy_tx_params_user_flag, port_index)  \
        SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,                         \
                              pm_info->wb_vars_ids[extPhyTxParamsUserFlag],      \
                              ext_phy_tx_params_user_flag, port_index)

/* Warm Boot Variables to indicate the boot state */
typedef enum pm4x2p5_fabric_wb_vars{
    isInitialized,
    isActive,
    ports,
    phy_type,
    interfaceConfig,
    portInitConfig,
    phyInitConfig,
    phyInterfaceConfig,
    nofPhys, /* number of phys for the port */
    dataRate,/* port speed - phy_intf_config.data_rate */
    maxSpeed, /* Max Port Speed Supported */
    /* interface type -  portmod_port_interface_config_t.interface */
    interfaceType, /* interface of type soc_port_if_t */
    /* serdes interface -  portmod_port_interface_config_t.serdes_interface */
    serdesInterfaceType,
    interfaceModes, /* see PHYMOD_INTF_MODES_ */
    isInterfaceRestore, /*indicate whether interface should be restored from DB (otherwise retrieved from HW), replace PHYMOD_INTF_F_INTF_PARAM_SET_ONLY*/
    anMode, /* spn_PHY_AN_MODE - Specify the AN mode */
    anCl37, /**< AN CL37 */
    cl37SgmiiCnt, /* store cl37 sgmii count */
    cl37SgmiiRestartCnt, /* spn_SERDES_CL37_SGMII_RESTART_COUNT - cl37_sgmii_cnt threshold*/
    /* tx params flag - phy_init_config_t.tx_params_user_flag */
    txParamsUserFlag,
    /* tx params flag - phy_init_config_t.tx */
    txParams,
    /* ext tx params flag - phy_init_config_t.ext_phy_tx_params_user_flag */
    extPhyTxParamsUserFlag,
    /* ext tx params - phy_init_config_t.ext_phy_tx */
    extPhyTxParams
}pm4x2p5_wb_vars_t;

typedef enum xlmac_port_mode_e{
    XLMAC_4_LANES_SEPARATE    = 0,
    XLMAC_4_LANES_TOGETHER     = 1
} xlmac_port_mode_t;

#define PM4x2P5_QUAD_MODE_IF(interface) \
    ((interface == SOC_PORT_IF_XAUI) || (interface == SOC_PORT_IF_DNX_XAUI))

/* Legacy phymod_tx_t structure format  */
typedef struct pm4x2p5_phymod_tx_s {
    int8_t pre;
    int8_t main;
    int8_t post;
    int8_t post2;
    int8_t post3;
    int8_t amp;
    int8_t drivermode;
} pm4x2p5_phymod_tx_t;

STATIC
int pm4x2p5_port_soft_reset(int unit, int port, pm_info_t pm_info, int enable);

int pm4x2p5_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    SOC_INIT_FUNC_DEFS;
    switch(interface) {
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_XAUI:
        case SOC_PORT_IF_DNX_XAUI:
            *is_supported = TRUE;
            break;
        default:
            *is_supported = FALSE;
            break;
    }
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x2p5_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for ( i = 0 ; i < MAX_PORTS_PER_PM4X2P5; i++) {
       rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD,
                     pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);

       if (tmp_port == port) {
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if (*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in the variables like
 * isInitialized, isActive, ports.. etc.,
 * All of these variables need to be added to warmboot
 * any variables added to save the state of warmboot should be
 * included here.
 */
STATIC
int pm4x2p5_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x2p5",
                            NULL, pm_info, VERSION(1), 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports",
                          wb_buffer_index, sizeof(int), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active",
                          wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type",
                          wb_buffer_index, sizeof(int), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phy_type] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "nof_phys",
                          wb_buffer_index, sizeof(int), NULL,
                          PM4X2P5_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[nofPhys] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "data_rate",
                          wb_buffer_index, sizeof(int), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[dataRate] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "max_speed",
                          wb_buffer_index, sizeof(uint32), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[maxSpeed] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_type",
                          wb_buffer_index, sizeof(uint32), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceType] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "serdes_interface",
                          wb_buffer_index, sizeof(uint32), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[serdesInterfaceType] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_modes",
                          wb_buffer_index, sizeof(uint32), NULL,
                          MAX_PORTS_PER_PM4X2P5, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceModes] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "is_interface_restore", wb_buffer_index,
                          sizeof(uint32), NULL, PM4X2P5_LANES_PER_CORE,
                          VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInterfaceRestore] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode",
                          wb_buffer_index, sizeof(uint32), NULL,
                          PM4X2P5_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_cl37",
                          wb_buffer_index, sizeof(uint32), NULL,
                          PM4X2P5_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anCl37] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "cl37_sgmii_count",
                          wb_buffer_index, sizeof(int), NULL,
                          PM4X2P5_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[cl37SgmiiCnt] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "cl37_sgmii_restart_count", wb_buffer_index,
                          sizeof(int), NULL, PM4X2P5_LANES_PER_CORE, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[cl37SgmiiRestartCnt] = wb_var_id;
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "tx_params_user_flag",
                          wb_buffer_index, sizeof(uint32), NULL,
                          PHYMOD_MAX_LANES_PER_PORT, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[txParamsUserFlag] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "tx_params", wb_buffer_index, sizeof(pm4x2p5_phymod_tx_t), NULL, 1, PHYMOD_MAX_LANES_PER_PORT, 0xffffffff, 0xffffffff, VERSION(1), VERSION(2), NULL);

    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[txParams] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "ext_phy_tx_params_user_flag",
                          wb_buffer_index, sizeof(uint32), NULL,
                          PHYMOD_MAX_LANES_PER_PORT, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[extPhyTxParamsUserFlag] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ext_phy_tx_params", wb_buffer_index, sizeof(pm4x2p5_phymod_tx_t), NULL, 1, PHYMOD_MAX_LANES_PER_PORT, 0xffffffff, 0xffffffff, VERSION(1), VERSION(2), NULL);

    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[extPhyTxParams] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD,
                     wb_buffer_index, FALSE));
    if (!SOC_WARM_BOOT(unit)) {
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
                                      pm_info->wb_vars_ids[interfaceType],
                                      _SHR_PORT_IF_COUNT);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                               pm_info->wb_vars_ids[serdesInterfaceType],
                               _SHR_PORT_IF_COUNT);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                   pm_info->wb_vars_ids[interfaceModes], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                      pm_info->wb_vars_ids[nofPhys], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                 pm_info->wb_vars_ids[isInterfaceRestore], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                      pm_info->wb_vars_ids[anMode],
                                      phymod_AN_MODE_Count);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                      pm_info->wb_vars_ids[anCl37], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                   pm_info->wb_vars_ids[txParamsUserFlag], 0);
        _SOC_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                        pm_info->wb_vars_ids[extPhyTxParamsUserFlag], 0);
        _SOC_IF_ERR_EXIT(rv);
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_pm_destroy(int unit, pm_info_t pm_info)
{
    if (pm_info->pm_data.pm4x2p5_db != NULL) {
        sal_free(pm_info->pm_data.pm4x2p5_db);
        pm_info->pm_data.pm4x2p5_db = NULL;
    }
    return SOC_E_NONE;
}

int pm4x2p5_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x2p5_create_info_t *info;
    pm4x2p5_t pm4x2p5_data = NULL;
    int i,j, pm_is_active, rv;
    int pm_is_initialized;
    SOC_INIT_FUNC_DEFS;

    info = &pm_add_info->pm_specific_info.pm4x2p5;
    pm4x2p5_data = sal_alloc(sizeof(struct pm4x2p5_s), "pm4x2p5_specific_db");
    SOC_NULL_CHECK(pm4x2p5_data);

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.pm4x2p5_db = pm4x2p5_data;
    PORTMOD_PBMP_ASSIGN(pm4x2p5_data->phys, pm_add_info->phys);
    pm4x2p5_data->int_core_access.type = phymodDispatchTypeInvalid;
    pm4x2p5_data->default_fw_loader_is_used = 0;
    pm4x2p5_data->default_bus_is_used = 0;
    pm4x2p5_data->portmod_phy_external_reset = pm_add_info->pm_specific_info.pm4x2p5.portmod_phy_external_reset;
    pm4x2p5_data->portmod_mac_soft_reset = pm_add_info->pm_specific_info.pm4x2p5.portmod_mac_soft_reset;
    pm4x2p5_data->first_phy = -1;

    /* initialize num of phys for each lane */
    for (i = 0 ; i < PM4X2P5_LANES_PER_CORE ; i++) {
        pm4x2p5_data->nof_phys[i] = 0;
    }
    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm4x2p5_data->int_core_access);

    /* initialize lane connections */
    for (i = 0 ; i < MAX_PHYN ; i++) {
        for (j = 0 ; j < PM4X2P5_LANES_PER_CORE ; j++) {
            portmod_xphy_lane_connection_t_init(unit, &pm4x2p5_data->lane_conn[i][j]);
        }
    }

    sal_memcpy(&pm4x2p5_data->polarity, &info->polarity,
                sizeof(phymod_polarity_t));
    sal_memcpy(&(pm4x2p5_data->int_core_access.access), &info->access,
                sizeof(phymod_access_t));
    sal_memcpy(&pm4x2p5_data->lane_map, &info->lane_map,
                sizeof(pm4x2p5_data->lane_map));
    pm4x2p5_data->ref_clk = info->ref_clk;
    pm4x2p5_data->fw_load_method = info->fw_load_method;
    pm4x2p5_data->external_fw_loader = info->external_fw_loader;

    if (info->access.bus == NULL) {
        /* bus must be set */
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                  (_SOC_MSG("Access bus is NULL")));
    }

   /* initialize num of phys for each lane */
    for (i = 0 ; i < PM4X2P5_LANES_PER_CORE ; i++) {
        pm4x2p5_data->nof_phys[i] = 1;
    }

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x2p5_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if (SOC_WARM_BOOT(unit)) {
        rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[phy_type], &pm4x2p5_data->int_core_access.type);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (!SOC_WARM_BOOT(unit)) {

        pm_is_active = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_initialized = 0;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isInitialized], &pm_is_initialized);
        _SOC_IF_ERR_EXIT(rv);

         rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                    pm_info->wb_vars_ids[phy_type],
                    &PM_4x2P5_INFO(pm_info)->int_core_access.type, 0);
         _SOC_IF_ERR_EXIT(rv);
    }

exit:
    if (SOC_FUNC_ERROR) {
        pm4x2p5_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

static int _xport_lag_failover_status_toggle(int unit, int phy_acc)
{
    uint32_t rval;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_LAG_FAILOVER_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));
    soc_reg_field_set(unit, LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x2p5_nof_lanes_get(int unit, int port, pm_info_t pm_info)
{
    int port_index, rv;
    uint32_t bitmap, bcnt = 0;

    rv = _pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap);
    if (rv) return (0); /* bit count is zero will flag error */

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }

    return (bcnt);
}

STATIC
int _pm4x2p5_serdes_reset(int unit, pm_info_t pm_info, int port, int in_reset)
{
    uint32 rval, phy;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_ITER(PM_4x2P5_INFO(pm_info)->phys, phy) {break;};

    SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit, port, &rval));
    if (in_reset)
    {
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, PWRDWNf, 1);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_HWf, 0);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, IDDQf, 1);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_PLLf, 0);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_MDIOREGSf, 0);
    } else {
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, PWRDWNf, 0);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_HWf, 1);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, IDDQf, 0);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_PLLf, 1);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, RSTB_MDIOREGSf, 1);
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, TXD1G_FIFO_RSTBf, 0xf); 
        soc_reg_field_set(unit, XPORT_XGXS_CTRLr, &rval, TXD10G_FIFO_RSTBf, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_XPORT_XGXS_CTRLr(unit, port, rval));

    sal_usleep(1100);

exit:
    SOC_FUNC_RETURN;
}

static int _xport_mode_get(int unit, int phy_acc, int bindex,
                      portmod_core_port_mode_t *core_mode, int *cur_lanes)
{
    uint32_t rval;
    int      port_mode;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_XPORT_MODE_REGr(unit, phy_acc, &rval));

    port_mode = soc_reg_field_get(unit, XPORT_MODE_REGr, rval, PORT_MODEf);

    switch (port_mode) {
        case 0:
            *core_mode = portmodPortModeQuad;
            break;

        case 1:
            *core_mode = portmodPortModeSingle;
            break;

        default:
            return SOC_E_FAIL;
    }

    switch (*core_mode) {
        case portmodPortModeQuad:
            *cur_lanes = 1;
            break;

        case portmodPortModeSingle:
            *cur_lanes = 4;
            break;

        default:
            return SOC_E_FAIL;
    }

exit:
    SOC_FUNC_RETURN;
}

static int _xport_mode_set(int unit, int phy_acc, portmod_core_port_mode_t cur_mode)
{
    uint32_t rval;
    int      port_mode, port_gmii_mii_enable;

    SOC_INIT_FUNC_DEFS;

    switch (cur_mode) {
        case portmodPortModeQuad:
            port_mode = 0;
            port_gmii_mii_enable = 1;
            break;

        case portmodPortModeSingle:
            port_mode = 1;
            port_gmii_mii_enable = 0;
            break;

        default:
            return SOC_E_FAIL;
    }

    _SOC_IF_ERR_EXIT(READ_XPORT_MODE_REGr(unit, phy_acc, &rval));

    soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PORT_MODEf, port_mode);
    soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, PORT_GMII_MII_ENABLEf, port_gmii_mii_enable);

    _SOC_IF_ERR_EXIT(WRITE_XPORT_MODE_REGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (READ_X_GPORT_CNTMAXSIZEr(unit, port, (uint32_t *)val));
}

int pm4x2p5_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    uint32 reg_val;
    int rv = 0;

    rv = READ_X_GPORT_CNTMAXSIZEr(unit, port, &reg_val);
    if (SOC_FAILURE(rv)) return (rv);

    soc_reg_field_set(unit, X_GPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, val);

    return(WRITE_X_GPORT_CNTMAXSIZEr(unit, port, reg_val));
}

int pm4x2p5_port_encap_set (int unit, int port, pm_info_t pm_info,
                                int flags, portmod_encap_t encap)
{
    return (xlmac_encap_set(unit, port, flags, encap));

}

int pm4x2p5_port_encap_get (int unit, int port, pm_info_t pm_info,
                            int *flags, portmod_encap_t *encap)
{
    return (xlmac_encap_get(unit, port, flags, encap));
}

int pm4x2p5_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    uint32 reg_val[1];
    int    rv = 0;

    rv = READ_XPORT_MIB_RESETr(unit, port, reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITSET(reg_val, port_index);

    rv = WRITE_XPORT_MIB_RESETr(unit, port, *reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITCLR(reg_val, port_index);

    return(WRITE_XPORT_MIB_RESETr(unit, port, *reg_val));
}

int pm4x2p5_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                     const portmod_access_get_params_t* params,
                                     int max_phys,
                                     phymod_phy_access_t* phy_access,
                                     int* nof_phys, int* is_most_ext)
{
    int phyn = 0, rv;
    uint32  iphy_lane_mask;
    int port_index=0;
    int serdes_lane=-1;
    int xphy_lane_mask=0;
    uint32 xphy_id = 0;
    uint32 lane_mask[MAX_NUM_CORES];
    phymod_port_loc_t port_loc[MAX_NUM_CORES];
    int i, is_legacy_phy;
    uint32 xphy_idx;
    int num_of_phys;
    int done;
    uint32 int_lane_mask = 0xf;
    phymod_core_access_t core_access;

    SOC_INIT_FUNC_DEFS;

    if (max_phys > MAX_NUM_CORES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("max_phys parameter exceeded the MAX value. max_phys=%d, max allowed %d."),
                       max_phys, MAX_NUM_CORES));
    }

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    if (params->phyn >= PM_4x2P5_INFO(pm_info)->nof_phys[port_index]) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
               PM_4x2P5_INFO(pm_info)->nof_phys[port_index] - 1, params->phyn));
    }


    for ( i = 0 ; i < max_phys; i++) {
        lane_mask[i] = 0;
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    phyn = params->phyn;

    /* if phyn is -1, it is looking for outer most phy.
       assumption is that all lane has same phy depth. */
    if (phyn < 0) {
        phyn = PM_4x2P5_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    if ( phyn == 0 ) {
        /* internal core */
        sal_memcpy (&phy_access[0], &(PM_4x2P5_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        phy_access[0].access.lane_mask = iphy_lane_mask;
        if (params->lane != -1) {
            serdes_lane = port_index + params->lane;
            phy_access[0].access.lane_mask &= (0x1 << serdes_lane );
            *nof_phys = 1;
            if (phy_access[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        } else {
            *nof_phys = 1;
            phy_access[0].access.lane_mask &= int_lane_mask;
        }
        phy_access[0].port_loc = phymodPortLocLine; /* only line is availabe for internal. */

        /* if it is warm boot, get probed information from wb db instead of re-probing. */
        if (SOC_WARM_BOOT(unit)) {
            rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &(phy_access[0].type));
            _SOC_IF_ERR_EXIT(rv);
        }
    } else {

        /* external phy */
        num_of_phys = 0;

        /* xphy idx is 1 less than phyn since internal core does not count */
        xphy_idx = phyn-1;

        if (params->lane != -1) { /* specific lane */
            serdes_lane = port_index + params->lane;
            if ( serdes_lane != -1) {
                xphy_id = PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;

                if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                    _SOC_IF_ERR_EXIT ( portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                    sal_memcpy (&phy_access[num_of_phys], &core_access,
                                sizeof(phymod_phy_access_t));
                    xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                  PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                    if ( xphy_lane_mask != -1 ) {
                        phy_access[0].access.lane_mask = xphy_lane_mask;
                    }
                    phy_access[0].port_loc = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                               phymodPortLocSys : phymodPortLocLine;
                    *nof_phys = 1;
                }
            }
        } else { /* all lanes */
            /* go thru all the lanes related to this port. */
            serdes_lane = 0;

            for (serdes_lane = 0; serdes_lane < MAX_PORTS_PER_PM4X2P5; serdes_lane++)
                if (iphy_lane_mask & (1U<<serdes_lane)) {
                xphy_id = PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
                if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                    _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                    if (num_of_phys == 0 ) {/* first one */
                        sal_memcpy (&phy_access[num_of_phys], &core_access,
                                    sizeof(phymod_phy_access_t));
                        xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                    PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                    PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                        if ( xphy_lane_mask != -1) {
                            lane_mask[num_of_phys] |= xphy_lane_mask;
                        }
                            port_loc[num_of_phys] = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                                            phymodPortLocSys : phymodPortLocLine;
                            num_of_phys++;
                    } else {
                        /* if not the first one, see if this one sharing core with other
                           lanes.  */
                        done = 0;

                        /* Skip if the lane is used by an inactive port marked with :i in the config
                         *  * property.
                         */
                        if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port+serdes_lane)) {
                            continue;
                        }

                        for ( i = 0 ; (i < num_of_phys) &&(!done) ; i++) {
                             if ((!sal_memcmp (&phy_access[i], &core_access,
                                               sizeof(phymod_phy_access_t)))) { /* found a match */
                                /* update lane */
                                xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                            PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                            PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                                if ( xphy_lane_mask != -1) {
                                    lane_mask[i] |= xphy_lane_mask;
                                }
                                done = 1;
                            }
                        }
                        if ((!done) && (num_of_phys < max_phys)) {
                            sal_memcpy (&phy_access[num_of_phys], &core_access,
                            sizeof(phymod_phy_access_t));
                            xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                        PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                        PM_4x2P5_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                            if ( xphy_lane_mask != -1) {
                                lane_mask[num_of_phys] = xphy_lane_mask;
                            }
                                port_loc[num_of_phys] = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                                                phymodPortLocSys : phymodPortLocLine ;
                                num_of_phys++;
                        }
                    }
                }
            }
            *nof_phys = num_of_phys;
            for ( i = 0 ; (i < *nof_phys) ; i++) {
                phy_access[i].access.lane_mask = lane_mask[i];
                phy_access[i].port_loc = port_loc[i];
            }
        }
    } /* ext phys */

    /* assumption is that all logical port have same phy depths, that
       will not true when simplex introduced , until then we can keep
       this code. */
    if (is_most_ext) {
        if (phyn == PM_4x2P5_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
        return(WRITE_MAC_RSV_MASKr(unit, port, rsv_mask));
}

static phymod_dispatch_type_t pm4x2p5_serdes_list[] =
{
#ifdef PHYMOD_VIPER_SUPPORT
    phymodDispatchTypeViper,
#endif /*PHYMOD_VIPER_SUPPORT  */
    phymodDispatchTypeInvalid
};

STATIC
int _pm4x2p5_pm_core_probe (int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int     rv = SOC_E_NONE;
    int    temp, phy;
    portmod_pbmp_t port_phys_in_pm;
    int probe =0;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i, is_legacy_phy;
    int core_probed;
    phymod_core_access_t core_access;
    phymod_firmware_load_method_t fw_load_method;
    SOC_INIT_FUNC_DEFS;

    /* probe phys (both internal and external cores. */

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x2P5_INFO(pm_info)->phys);

    /* if internal core is not probe, probe it. Currently checking to type to see
       phymodDispatchTypeInvalid, later need to move to WB.  */
    _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm4x2p5_serdes_list, &PM_4x2P5_INFO(pm_info)->int_core_access, &probe));

    /* save probed phy type to wb. */
    if (probe) {
         rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                     pm_info->wb_vars_ids[phy_type],
                     &PM_4x2P5_INFO(pm_info)->int_core_access.type);
         _SOC_IF_ERR_EXIT(rv);
    }

    /* probe ext core related to this logical port. */
    temp = 0;
    PORTMOD_PBMP_ITER(PM_4x2P5_INFO(pm_info)->phys, phy) {
        if (PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if ( PM_4x2P5_INFO(pm_info)->nof_phys[temp] >= 2 ) {
                nof_ext_phys = PM_4x2P5_INFO(pm_info)->nof_phys[temp]-1;
                for (i=0 ; i<nof_ext_phys ; i++) {
                    xphy_id = PM_4x2P5_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_probed_get(unit, xphy_id, &core_probed));
                            _SOC_IF_ERR_EXIT(portmod_xphy_fw_load_method_get(unit, xphy_id, &fw_load_method));
                        if ( !core_probed ) {
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                            probe = 0;
                            if (SOC_E_NONE != (portmod_common_ext_phy_probe(unit, port, &core_access, &probe))) {
                                /* Remove un-probed external PHY from phy number count */
                                PM_4x2P5_INFO(pm_info)->nof_phys[temp] = i + 1;
                                /* Set firmware download method to phymodFirmwareLoadMethodNone to avoid image download for unprobed XPHY */
                                fw_load_method = phymodFirmwareLoadMethodNone;
                            } else {
                                if (!probe) {
                                /* case where port_phy_addr is specified and phy is not connetced */
                                /* Remove un-probed external PHY from phy number count */
                                PM_4x2P5_INFO(pm_info)->nof_phys[temp] = i + 1;
                                /* Set firmware download method to phymodFirmwareLoadMethodNone to avoid image download for unprobed XPHY */
                                fw_load_method = phymodFirmwareLoadMethodNone;
                                } else {
                                    core_probed = 1;
                                }
                            }
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_probed_set(unit, xphy_id, core_probed));
                            _SOC_IF_ERR_EXIT(portmod_xphy_fw_load_method_set(unit, xphy_id, fw_load_method));
                            _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_access_set(unit, xphy_id, &core_access, is_legacy_phy));
                        }
                    }
                }
            }
        }
        temp++;
    }

   /*update warmboot engine*/
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
        pm_info->wb_vars_ids[phy_type],
        &PM_4x2P5_INFO(pm_info)->int_core_access.type, 0);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x2p5_pm_ext_phy_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int    temp, phy;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    portmod_pbmp_t port_phys_in_pm;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i, is_legacy_phy;
    int is_initialized;
    int lane;
    int init_all;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    uint32 primary_core_num;

    SOC_INIT_FUNC_DEFS;

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                    !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x2P5_INFO(pm_info)->phys);

    /* core init (both internal and external cores. */

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    core_conf.interface.pll_divider_req = 0xA;  /* FIX THIS - IN PORTCTRL.c */

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));


    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }

    core_conf.op_datapath = add_info->phy_op_datapath;

    /* deal with ext phys now ... */
    if (PORTMOD_PORT_ADD_F_PORT_ATTACH_EXT_PHY_SKIP_GET(add_info)) {
        return(SOC_E_NONE);
    }

    /* adjust config setup for external phys. */

    /* for pm4x2p5, no FW load */
    core_conf.firmware_load_method = phymodFirmwareLoadMethodNone;
    core_conf.firmware_loader = NULL;

    /* config "no swap" for external phys.set later using APIs*/
    for (lane=0 ; lane < (core_conf.lane_map.num_of_lanes); lane++) {
        core_conf.lane_map.lane_map_rx[lane] = lane;
        core_conf.lane_map.lane_map_tx[lane] = lane;
    }

    temp = 0;
    PORTMOD_PBMP_ITER(PM_4x2P5_INFO(pm_info)->phys, phy) {
        if (PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if ( PM_4x2P5_INFO(pm_info)->nof_phys[temp] >= 2 ) {
                nof_ext_phys = PM_4x2P5_INFO(pm_info)->nof_phys[temp]-1;
                for (i=0 ; i<nof_ext_phys ; i++) {
                    xphy_id = PM_4x2P5_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        _SOC_IF_ERR_EXIT(portmod_xphy_lane_map_get(unit, xphy_id, &lane_map));
                        _SOC_IF_ERR_EXIT(portmod_xphy_primary_core_num_get(unit, xphy_id, &primary_core_num));

                        /* config "no swap" for external phys.set later using APIs*/
                        for (lane=0 ; lane < lane_map.num_of_lanes; lane++) {
                            core_conf.lane_map.lane_map_rx[lane] = lane_map.lane_map_rx[lane];
                            core_conf.lane_map.lane_map_tx[lane] = lane_map.lane_map_tx[lane];
                        }
                        if (xphy_id == primary_core_num) {
                            _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_get(unit, xphy_id, &is_initialized));
                             /* Very first time this function gets called, honor the call and
                              * ignore the flags */
                             if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
                                 if (PM_4x2P5_INFO(pm_info)->first_phy == phy) {
                                     _SOC_IF_ERR_EXIT(portmod_xphy_core_config_set(unit, xphy_id, &core_conf));
                                 }
                             }
                        } else {
                            _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_get(unit, primary_core_num, &is_initialized));
                        }

                        if ((PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS1))
                            continue;

                        if ((PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS2))
                            continue;
                        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, core_access.access.addr, &is_legacy_phy));
                        if (is_legacy_phy) {
                            /* Dont do a core init for legacy Phys.*/
                        } else {
                           _SOC_IF_ERR_EXIT(phymod_core_init(&core_access, &core_conf, &core_status));
                        }

                        if (init_all) {
                            is_initialized = (PHYMOD_CORE_INIT_F_EXECUTE_PASS2 | PHYMOD_CORE_INIT_F_EXECUTE_PASS1);
                        }
                        else if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                            is_initialized |= PHYMOD_CORE_INIT_F_EXECUTE_PASS2;
                        } else {
                            is_initialized = PHYMOD_CORE_INIT_F_EXECUTE_PASS1;
                        }

                        _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_set(unit, xphy_id, is_initialized));

                        if (xphy_id != primary_core_num) {
                            _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_set(unit, primary_core_num, is_initialized));
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
int _pm4x2p5_pm_port_init(int unit, int port, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable, pm_info_t pm_info)
{
    int rv;
    uint32 reg_val, flags;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};
    uint32 rsv_mask;
    SOC_INIT_FUNC_DEFS;

    if (enable) {
        /* RSV Mask */
        rsv_mask = 0;
        SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
        SHR_BITSET(&rsv_mask, 4); /* CRC error */
        SHR_BITSET(&rsv_mask, 6); /* PGW_MACTruncated/Frame out of Range */
        SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/

         rv = __portmod__dispatch__[pm_info->type]->f_portmod_port_mac_rsv_mask_set(unit, port, pm_info, rsv_mask);
        _SOC_IF_ERR_EXIT(rv);


        /* Soft reset */
        rv = xlmac_soft_reset_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);
        rv = xlmac_soft_reset_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);


        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_XPORT_PORT_ENABLEr(unit, port, &reg_val));
        soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &reg_val,
                                   port_fields[internal_port], 1);
        _SOC_IF_ERR_EXIT(WRITE_XPORT_PORT_ENABLEr(unit, port, reg_val));

        /* Init MAC */
        flags = 0;
        if (PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_RX_STRIP_CRC;
        }

        if (PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_TX_APPEND_CRC;
        }

        if (PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_TX_REPLACE_CRC;
        }

        if (PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
            flags |= XLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
        }

        rv = xlmac_init(unit, port, flags);
        _SOC_IF_ERR_EXIT(rv);

        /* Counter MAX size */
        rv = __portmod__dispatch__[pm_info->type]->f_portmod_port_cntmaxsize_set(unit, port, pm_info, 1518);
       _SOC_IF_ERR_EXIT(rv);

        /* Reset MIB counters */
        rv = __portmod__dispatch__[pm_info->type]->f_portmod_port_mib_reset_toggle(unit, port, pm_info, internal_port);
       _SOC_IF_ERR_EXIT(rv);

    } else {
        /* Port disable */
        _SOC_IF_ERR_EXIT(READ_XPORT_PORT_ENABLEr(unit, port, &reg_val));
        soc_reg_field_set (unit, XPORT_PORT_ENABLEr, &reg_val,
                           port_fields[internal_port], 0);
        _SOC_IF_ERR_EXIT(WRITE_XPORT_PORT_ENABLEr(unit, port, reg_val));

         /* Soft reset */
        rv = xlmac_soft_reset_set(unit, port, 1);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}


STATIC
int _pm4x2p5_interface_check(int unit, int port,
                               const portmod_port_interface_config_t *interface_config,
                               soc_port_if_t interface, int *is_valid)
{
    int num_lanes;

    *is_valid = 0;
    num_lanes = interface_config->port_num_lanes;

    switch (interface_config->speed) {
        case 10:
        case 100:
        case 1000:
        case 2500:
            if ((SOC_PORT_IF_SGMII == interface) ||
                (SOC_PORT_IF_GMII == interface)) {
                *is_valid = 1;
            }
            break;
        case 10000:
            if (num_lanes == 4) {
                if (SOC_PORT_IF_DNX_XAUI == interface) {
                    *is_valid = 1;
                }
            }
            break;
        default:
            break;
    }

    return SOC_E_NONE;
}


STATIC
int _pm4x2p5_line_side_phymod_interface_get(int unit, int port,
                                pm_info_t pm_info,
                                const portmod_port_interface_config_t *config,
                                phymod_interface_t *interface_type)
{
    int port_index = 0;
    int is_valid = 0, nof_phys = 0;
    uint32 bitmap;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get(unit, port, pm_info, &port_index,
                                            &bitmap));

    _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access_arr ,(1+MAX_PHYN),
                                               &nof_phys));

    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(_pm4x2p5_interface_check(unit, port, config, interface,
                                             &is_valid));

    if (!is_valid) {
        /* interface check validation for internal sedes failed */
        LOG_ERROR(BSL_LS_BCM_PORT,
              (BSL_META_UP(unit, port,
               "ERROR: u=%d p=%d interface type %d not supported by internal SERDES for this speed %d \n"),
               unit, port, interface, config->speed));
        _SOC_IF_ERR_EXIT(SOC_E_PARAM);
    }

    _SOC_IF_ERR_EXIT(PM4x2P5_INTERFACE_TYPE_SET(unit, pm_info, interface,
                                               port_index));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                                                 interface, interface_type));

exit:
    SOC_FUNC_RETURN;
}


int _pm4x2p5_phymod_interface_mode_get(int unit, int port,pm_info_t pm_info,
                                   const portmod_port_interface_config_t *config,
                                   phymod_phy_inf_config_t *phy_config)
{
    int port_index = 0;
    uint32 bitmap;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get(unit, port, pm_info, &port_index,
                                            &bitmap));

    _SOC_IF_ERR_EXIT(PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index));

    phy_config->interface_modes = config->interface_modes;
    _SOC_IF_ERR_EXIT(portmod_media_type_from_port_intf(unit, interface,
                                                       phy_config));
exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x2p5_pm_disable (int unit, int port, pm_info_t pm_info, int phy_acc)
{
    uint32 reg_val;
    int rv = 0;

    /* put MAC in reset */
    rv = READ_XPORT_XLMAC_CONTROLr(unit, phy_acc, &reg_val);
    if (rv) return (rv);

    soc_reg_field_set(unit, XPORT_XLMAC_CONTROLr, &reg_val, XLMAC_RESETf, 1);
    rv = WRITE_XPORT_XLMAC_CONTROLr(unit, phy_acc, reg_val);
    if (rv) return (rv);

    /* Put Serdes in reset*/
    rv = _pm4x2p5_serdes_reset(unit, pm_info, port, 1);

    return (rv);
}

STATIC
int _pm4x2p5_pm_xport_init (int unit, int port, pm_info_t pm_info,
                            int port_index, int interface,
                            int timestamp_mode)
{
    uint32 reg_val, port_mode, port_gmii_mii_enable;
    SOC_INIT_FUNC_DEFS;

    if (PM4x2P5_QUAD_MODE_IF(interface)) { /* single port mode */
        port_mode = XLMAC_4_LANES_TOGETHER;
        port_gmii_mii_enable = 0;
    } else {
        port_mode = XLMAC_4_LANES_SEPARATE;
        port_gmii_mii_enable = 1;
    }

    /* Set MAC reset */
    _SOC_IF_ERR_EXIT(READ_XPORT_XLMAC_CONTROLr(unit, port, &reg_val));
    soc_reg_field_set(unit, XPORT_XLMAC_CONTROLr, &reg_val, XLMAC_RESETf, 1);
    _SOC_IF_ERR_EXIT(WRITE_XPORT_XLMAC_CONTROLr(unit, port, reg_val));

    /* Set port mode (quad mode or single mode) and GMII MII configuration */
    _SOC_IF_ERR_EXIT(READ_XPORT_MODE_REGr(unit, port, &reg_val));
    soc_reg_field_set (unit, XPORT_MODE_REGr, &reg_val, PORT_MODEf,  port_mode);
    soc_reg_field_set (unit, XPORT_MODE_REGr, &reg_val, PORT_GMII_MII_ENABLEf,  port_gmii_mii_enable);
    _SOC_IF_ERR_EXIT(WRITE_XPORT_MODE_REGr(unit, port, reg_val));

    /* Enable port */
    _SOC_IF_ERR_EXIT(READ_XPORT_PORT_ENABLEr(unit, port, &reg_val));
    if (port_mode == XLMAC_4_LANES_TOGETHER) {
        soc_reg_field_set (unit, XPORT_PORT_ENABLEr, &reg_val, PORT0f,  1);
    } else {
        soc_reg_field_set (unit, XPORT_PORT_ENABLEr, &reg_val, PORT0f,  1);
        soc_reg_field_set (unit, XPORT_PORT_ENABLEr, &reg_val, PORT1f,  1);
        soc_reg_field_set (unit, XPORT_PORT_ENABLEr, &reg_val, PORT2f,  1);
        soc_reg_field_set (unit, XPORT_PORT_ENABLEr, &reg_val, PORT3f,  1);
    }
    _SOC_IF_ERR_EXIT(WRITE_XPORT_PORT_ENABLEr(unit, port, reg_val));

    /* Get Serdes OOR */
    _SOC_IF_ERR_EXIT(_pm4x2p5_serdes_reset(unit, pm_info, port, 1));
    _SOC_IF_ERR_EXIT(_pm4x2p5_serdes_reset(unit, pm_info, port, 0));

    /* Bring MAC OOR */
    _SOC_IF_ERR_EXIT(READ_XPORT_XLMAC_CONTROLr(unit, port, &reg_val));
    soc_reg_field_set(unit, XPORT_XLMAC_CONTROLr, &reg_val, XLMAC_RESETf, 0);
    _SOC_IF_ERR_EXIT(WRITE_XPORT_XLMAC_CONTROLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x2p5_pm_serdes_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int     rv;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    const portmod_port_interface_config_t* config;
    uint32 core_is_initialized;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    phymod_core_access_t core;
    int i;
    SOC_INIT_FUNC_DEFS;

    config = &(add_info->interface_config);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    /* set internal setting. */

    core_conf.firmware_load_method = PM_4x2P5_INFO(pm_info)->fw_load_method;
    core_conf.firmware_loader = PM_4x2P5_INFO(pm_info)->external_fw_loader;
    core_conf.lane_map = PM_4x2P5_INFO(pm_info)->lane_map;
    /* set default value firstly, will set the true value after phy init */
    for (i = 0; i < core_conf.lane_map.num_of_lanes; i++) {
        core_conf.lane_map.lane_map_tx[i] = i;
        core_conf.lane_map.lane_map_rx[i] = i;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                        interface, &core_conf.interface.interface_type));
    core_conf.interface.data_rate = config->speed;
    core_conf.interface.interface_modes = config->interface_modes;
    core_conf.interface.ref_clock = PM_4x2P5_INFO(pm_info)->ref_clk;
    core_conf.interface.pll_divider_req = 0xA;  /* FIX THIS - IN PORTCTRL.c */

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));

    core_conf.op_datapath = add_info->phy_op_datapath;

    /* deal witn internal serdes first */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isInitialized],
                                &core_is_initialized);
     _SOC_IF_ERR_EXIT(rv);
    if (!core_is_initialized) {
         sal_memcpy(&core, &PM_4x2P5_INFO(pm_info)->int_core_access, sizeof(phymod_core_access_t));
         core.access.lane_mask = 0xF;
         _SOC_IF_ERR_EXIT(phymod_core_init(&core,
                                          &core_conf,
                                          &core_status));
         core_is_initialized = 1;
         rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isInitialized],
                                &core_is_initialized);
         _SOC_IF_ERR_EXIT(rv);
    }

    /*update warmboot engine*/
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
        pm_info->wb_vars_ids[phy_type],
        &PM_4x2P5_INFO(pm_info)->int_core_access.type, 0);
    _SOC_IF_ERR_EXIT(rv);

    exit:
        SOC_FUNC_RETURN;
}


STATIC
int _pm4x2p5_pm_core_init (int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x2p5_pm_serdes_core_init(unit,pm_info,add_info));
    _SOC_IF_ERR_EXIT(_pm4x2p5_pm_ext_phy_core_init(unit,pm_info,add_info));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x2p5_an_mode_update (int unit, int port, pm_info_t pm_info,
                            int an_cl37, int an_cl73)
{
    int rv;
    phymod_an_mode_type_t an_mode;
    int    port_index;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    rv = PM4x2P5_AN_MODE_GET(unit, pm_info, &an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    an_mode = phymod_AN_MODE_CL37;
    if (an_cl37 == PORTMOD_CL37_SGMII_COMBO) {
        an_mode = phymod_AN_MODE_CL37_SGMII;
    }

    rv = PM4x2P5_AN_MODE_SET(unit, pm_info, an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int port_index = -1, init_all=0;
    int rv = 0, port_i, my_i;
    int phy, ii, nof_phys;
    int phys_count = 0, first_phy = -1, is_interface_restore;
    uint32 pm_is_active = 0;
    portmod_pbmp_t port_phys_in_pm;
    phymod_phy_init_config_t init_config;
    phymod_phy_inf_config_t *phy_config = &init_config.interface;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_access_t phy_access_lane;
    portmod_port_ability_t port_ability;
    const portmod_port_interface_config_t *config = &add_info->interface_config;
    phymod_interface_t phymod_interface = phymodInterfaceCount;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access[1+MAX_PHYN];
    SOC_INIT_FUNC_DEFS;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x2P5_INFO(pm_info)->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);

    ii = 0;
    PORTMOD_PBMP_ITER(PM_4x2P5_INFO(pm_info)->phys, phy) {
        if (PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                             pm_info->wb_vars_ids[ports], &port, ii);
            _SOC_IF_ERR_EXIT(rv);
            first_phy = (first_phy == -1) ? phy : first_phy;
            port_index = (port_index == -1 ? ii : port_index);
        }
        ii++;
    }
    PM_4x2P5_INFO(pm_info)->first_phy = first_phy;

    /* init data for port configuration */
    rv = PM4x2P5_AN_MODE_SET(unit, pm_info, add_info->init_config.an_mode,
                            port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_AN_CL37_SET(unit, pm_info, add_info->init_config.an_cl37,
                            port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_CL37_SGMII_CNT_SET(unit, pm_info,
                                   add_info->init_config.cl37_sgmii_cnt,
                                   port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_CL37_SGMII_RESTART_CNT_SET(unit, pm_info,
                                  add_info->init_config.cl37_sgmii_RESTART_CNT,
                                  port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_MAX_SPEED_SET(unit, pm_info,
                              add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x2P5_INTERFACE_TYPE_SET(unit, pm_info,
                                   add_info->interface_config.interface,
                                   port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_SERDES_INTERFACE_TYPE_SET(unit, pm_info,
                              add_info->interface_config.serdes_interface,
                              port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_INTERFACE_MODES_SET(unit, pm_info,
                                    add_info->interface_config.interface_modes,
                                    port_index);
    _SOC_IF_ERR_EXIT(rv);

    is_interface_restore = (add_info->interface_config.flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) ? 1 : 0 ;
    rv = PM4x2P5_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, port_index);
    _SOC_IF_ERR_EXIT(rv);

    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_4x2P5_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_4x2P5_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }

    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                            pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* For Viper, no fw is required, these flags just be usded to avoid rerunning some code */
    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;


    if (!PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_GET(add_info))
    {
        /* update an information before proceeding further */
        SOC_IF_ERROR_RETURN(_pm4x2p5_an_mode_update (unit, port, pm_info,
                                          add_info->init_config.an_cl37,
                                          add_info->init_config.an_cl73));
    }

    /* if not active - initalize PM */
    if (!pm_is_active) {
        if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || (init_all)) {
            /*init the PM*/
            rv = _pm4x2p5_pm_xport_init (unit, port, pm_info, port_index,
                 add_info->interface_config.interface,
                 add_info->flags & PORTMOD_PORT_ADD_F_EGR_1588_TIMESTAMP_MODE_48BIT ? 1 : 0);
            _SOC_IF_ERR_EXIT(rv);
        }

        pm_is_active = 1;
        rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isActive], &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* probe core internal serdes and ext phys if there is any. */
    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] > 0) {
        /* only at  CORE PROBE phase need probe */
        if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || init_all) {
            rv = _pm4x2p5_pm_core_probe(unit, port, pm_info, add_info);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    if (!(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) &&
        (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info))) {
        return (rv);
    }

    /* probe core and core config internal serdes and ext phys if there is any. */
    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] > 0 ) {
        rv = _pm4x2p5_pm_core_init(unit, pm_info, add_info);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (rv);
    }

    /* initalize port */
    rv = _pm4x2p5_pm_port_init(unit, port, port_index, add_info, 1, pm_info);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phys */
    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] > 0) {

        _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

        params.phyn = 0;
        _SOC_IF_ERR_EXIT(pm4x2p5_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access_lane, &nof_phys, NULL));

        init_config.an_en = add_info->autoneg_en;
        init_config.cl72_en = add_info->link_training_en;
        init_config.op_mode = add_info->interface_config.port_op_mode;

        _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                           phy_access ,(1+MAX_PHYN),
                                                           &nof_phys));
        my_i = 0;
        for (ii=0 ; ii<PM4X2P5_LANES_PER_CORE ; ii++) {
            rv = SOC_WB_ENGINE_GET_ARR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[ports], &port_i, ii);
            _SOC_IF_ERR_EXIT(rv);

            if (port_i != port) {
                continue;
            }

            if (SHR_BITGET(&(PM_4x2P5_INFO(pm_info)->polarity.tx_polarity),ii)) {
                SHR_BITSET(&init_config.polarity.tx_polarity, my_i);
            }

            if (SHR_BITGET(&(PM_4x2P5_INFO(pm_info)->polarity.rx_polarity),ii)) {
                SHR_BITSET(&init_config.polarity.rx_polarity, my_i);
            }

#if 0  
            _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access_lane, phymodMediaTypeChipToChip, &init_config.tx[my_i]));
#else
            SOC_IF_ERROR_RETURN(phymod_tx_t_init(&init_config.tx[my_i]));
#endif
            my_i++;

        }

        phy_config->data_rate = config->speed;
        phy_config->pll_divider_req = config->pll_divider_req;
        phy_config->ref_clock = PM_4x2P5_INFO(pm_info)->ref_clk;

        _SOC_IF_ERR_EXIT(_pm4x2p5_line_side_phymod_interface_get(unit, port,
                                                            pm_info, config,
                                                            &phymod_interface));

        phy_config->interface_type = phymod_interface;
        _SOC_IF_ERR_EXIT(_pm4x2p5_phymod_interface_mode_get(unit, port, pm_info, config, phy_config));

        _SOC_IF_ERR_EXIT(portmod_port_phychain_phy_init(unit, phy_access, nof_phys,
                                                            &init_config));
    }

    rv = pm4x2p5_port_interface_config_set(unit, port, pm_info, config, TRUE);
    _SOC_IF_ERR_EXIT(rv);
    /* configure serdes lane mapping */
    _SOC_IF_ERR_EXIT
    (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           (phymod_phy_access_t*)core_access,
                                           (1+MAX_PHYN), &nof_phys));
    lane_map = PM_4x2P5_INFO(pm_info)->lane_map;
    _SOC_IF_ERR_EXIT(phymod_core_lane_map_set(&core_access[0], &lane_map));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] > 0) {
        if (add_info->init_config.fs_cl72) {
            /* config port_init_cl72 is used to enable fs cl72 only in the serdes,
            do not enable cl72 on external phys. Set the no of phys argument to 1 to avoid setting cl72 enable
            on ext phys */
            _SOC_IF_ERR_EXIT(portmod_port_phychain_cl72_set(unit, port, phy_access, 1, 1));
        }
    }

    /* set the default advert ability */
    _SOC_IF_ERR_EXIT
        (pm4x2p5_port_ability_local_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &port_ability));
    _SOC_IF_ERR_EXIT
        (pm4x2p5_port_ability_advert_set(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &port_ability));

    /* De-Assert SOFT_RESET */
    _SOC_IF_ERR_EXIT(pm4x2p5_port_soft_reset(unit, port, pm_info, 0));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_detach(int unit, int port, pm_info_t pm_info)
{
    int     enable, tmp_port, i=0, rv=0, port_index = -1;
    int     invalid_port = -1, is_last_one = TRUE;
    uint32    inactive = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pm4x2p5_port_enable_get(unit, port, pm_info, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                  (_SOC_MSG("can't detach active port %d"), port));
    }

    /*remove from array and check if it was the last one*/
    for ( i = 0 ; i < MAX_PORTS_PER_PM4X2P5; i++) {
       rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                    pm_info->wb_vars_ids[ports], &tmp_port, i);
       _SOC_IF_ERR_EXIT(rv);

       if (tmp_port == port) {
           port_index = (port_index == -1 ? i : port_index);
           rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                    pm_info->wb_vars_ids[ports], &invalid_port, i);
           _SOC_IF_ERR_EXIT(rv);
       } else if (tmp_port != -1) {
           is_last_one = FALSE;
       }
    }

    if (port_index == -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT,(_SOC_MSG("Port %d wasn't found"),port));
    }

    rv = _pm4x2p5_pm_port_init(unit, port, port_index, NULL, 0, pm_info);
    _SOC_IF_ERR_EXIT(rv);

    /*deinit PM in case of last one*/
    if (is_last_one) {
       rv = _pm4x2p5_pm_disable(unit, port, pm_info, port);
       _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                             pm_info->wb_vars_ids[isActive], &inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                                   pm_info->wb_vars_ids[isInitialized],
                                   &inactive);
       _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;
    SOC_INIT_FUNC_DEFS;

    /* replace old port with new port */
    for (i = 0; i < MAX_PORTS_PER_PM4X2P5; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                     pm_info->wb_vars_ids[ports], &tmp_port, i);
        _SOC_IF_ERR_EXIT(rv);

        if (tmp_port == port) {
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                             pm_info->wb_vars_ids[ports], &new_port, i);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    portmod_access_get_params_t params;


    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { /* most external is default */  
        params.phyn = -1 ;  
    } 

    if( PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS ) {
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* most external is default */
        params.sys_side = PORTMOD_SIDE_LINE ;
    }

    SOC_IF_ERROR_RETURN(pm4x2p5_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, phy_access, &nof_phys, NULL));

    return (portmod_phy_port_diag_ctrl(unit, port, phy_access, nof_phys, inst, op_type, op_cmd, arg));
}

int pm4x2p5_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    core_info->ref_clk = PM_4x2P5_INFO(pm_info)->ref_clk;
    return SOC_E_NONE;
}


int pm4x2p5_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    SOC_INIT_FUNC_DEFS;
    PORTMOD_PBMP_ASSIGN(*phys, PM_4x2P5_INFO(pm_info)->phys);
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int     rv = 0, rst_flags = 0, phychain_flag;
    int     actual_flags = flags, nof_phys = 0, xlmac_flags = 0;

    phymod_phy_power_t             phy_power;
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

     /* if MAC is set and either RX or TX set is invalid combination */
    if ( (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) ) {
        if ((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    phy_power.rx = phymodPowerNoChange;
    phy_power.tx = phymodPowerNoChange;

    if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
        xlmac_flags |= XLMAC_ENABLE_SET_FLAGS_RX_EN;
    }
    if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
        xlmac_flags |= XLMAC_ENABLE_SET_FLAGS_TX_EN;
    }

    /* phychain_flag is meant to be used if we want only part of the phychin to be affected*/
    phychain_flag = PORTMOD_PORT_ENABLE_INTERNAL_PHY_ONLY_GET(flags) ?
        PORTMOD_INIT_F_INTERNAL_SERDES_ONLY :
        PORTMOD_INIT_F_EXTERNAL_MOST_ONLY ;

    if (enable) {
        if (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) {
              if (PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
                rst_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
            }
            rv = xlmac_enable_set(unit, port, rst_flags, 1);
            _SOC_IF_ERR_EXIT(rv);
              if (PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
                _SOC_IF_ERR_EXIT(pm4x2p5_port_soft_reset(unit, port, pm_info, 0));
            }
        }

        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags))
        {
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, phychain_flag, &phy_power));
        }
    } else {
        if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                   &nof_phys));
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, phychain_flag, &phy_power));
        }

        if ((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) ) {
            rv = xlmac_enable_set(unit, port, xlmac_flags, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    int phy_enable = 0;
    int mac_enable = 0;
    phymod_phy_power_t power;

    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    if (PORTMOD_PORT_ENABLE_PHY_GET(flags)||(0 == flags)) {
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                               &nof_phys));

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_power_get(unit, port, phy_access, nof_phys, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY,
                                                   &power));

        phy_enable = ((power.rx == phymodPowerOn) &&
                     (power.tx == phymodPowerOn)) ? 1 : 0;
        if (0 != flags) {
            *enable = phy_enable;
        }
    }

    if (PORTMOD_PORT_ENABLE_MAC_GET(flags)||(0 == flags)) {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, 0, &mac_enable));
        if (0 != flags) {
            *enable = mac_enable;
        }
    } else if (PORTMOD_PORT_ENABLE_RX_GET(flags)) {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, XLMAC_ENABLE_SET_FLAGS_RX_EN, enable));
    } else if (PORTMOD_PORT_ENABLE_TX_GET(flags)) {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, XLMAC_ENABLE_SET_FLAGS_TX_EN, enable));
    }
    if (0 == flags) {
        *enable = (mac_enable || phy_enable) ;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int all_phy)
{
    uint32 pm_is_initialized = 0;
    int    nof_phys = 0, rv = 0, flags = 0;
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    phymod_phy_access_t         phy_access;
    phymod_phy_inf_config_t     phy_interface_config;
    portmod_access_get_params_t params;
    int                         port_index;
    uint32_t                    bitmap;
    portmod_port_interface_config_t  config_temp ;
    phymod_interface_t     phymod_interface;
    phymod_interface_t     phymod_serdes_interface = phymodInterfaceCount;
    soc_port_if_t         interface = SOC_PORT_IF_NULL;
    uint32 is_interface_restore, interface_modes;
    SOC_INIT_FUNC_DEFS;

    /*
     * Ignore the call to interface config set if the pm is not iniialized.
     * This happens if the legacy external phy does a call back prior to initializing
     * and probing internal phys
     */
    rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                pm_info->wb_vars_ids[isInitialized],
                                &pm_is_initialized);
    _SOC_IF_ERR_EXIT(rv);
    if (!pm_is_initialized) return (SOC_E_NONE);

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (config->flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP) {
        int mac_speed = 0, serdes_speed = 0, mac_only = 0;
        int nof_lanes = SOC_INFO(unit).port_num_lanes[port];

        _SOC_IF_ERR_EXIT(portmod_port_speed_get(unit, port, &serdes_speed));
        if (config->speed == serdes_speed) {
            xlmac_speed_get(unit, port, &mac_speed);
            mac_speed *= nof_lanes;
            if (config->speed == mac_speed) {
                SOC_FUNC_RETURN;
            }

            mac_only = 1;
        }

        flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_speed_set(unit, port, flags, config->speed);

        /*Set MAC Timestamp Delay*/
        _SOC_IF_ERR_EXIT(xlmac_timestamp_byte_adjust_set(unit, port, 0, config->speed));

        if (mac_only) {
            /* line side speed and SerDes speed are equal, no need update SerDes configuration */
            SOC_FUNC_RETURN;
        }

        all_phy = PORTMOD_INIT_F_ALL_PHYS;
    }

    rv = PM4x2P5_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = 0;
    params.lane = -1;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(pm4x2p5_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access, &nof_phys, NULL));

    if (config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
        /* Verify if the interface is correct before saving it */
        _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed, interface,
                                              &phymod_interface));

        /* only flag saved by interface_config->flags */
        is_interface_restore = 1;
        rv = PM4x2P5_IS_INTERFACE_RESTORE_SET(unit, pm_info,
                                             is_interface_restore, port_index);
        _SOC_IF_ERR_EXIT(rv);
        rv = PM4x2P5_INTERFACE_TYPE_SET(unit, pm_info, config->interface,
                                       port_index);
        _SOC_IF_ERR_EXIT(rv);
        SOC_FUNC_RETURN;
    }
    rv = PM4x2P5_IS_INTERFACE_RESTORE_GET(unit, pm_info, &is_interface_restore,
                                         port_index);
    _SOC_IF_ERR_EXIT(rv);
    /*
     * if the interface is already set with PARAM_SET_ONLY
     * h/w interface needs to be updated during speed set.
     * So load the interface information and clear the flag;
     */
     if (is_interface_restore) {
         rv = PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index);
         _SOC_IF_ERR_EXIT(rv);
         ((portmod_port_interface_config_t*)config)->interface = interface;

         is_interface_restore = 0;
         rv = PM4x2P5_IS_INTERFACE_RESTORE_SET(unit, pm_info,
                                           is_interface_restore, port_index);
         _SOC_IF_ERR_EXIT(rv);
    }



    if (!(config->flags & (PHYMOD_INTF_F_SET_SPD_TRIGGER | PHYMOD_INTF_F_UPDATE_SPEED_LINKUP))) {

        rv = PM4x2P5_INTERFACE_MODES_GET(unit, pm_info, &interface_modes,
                                        port_index);
        _SOC_IF_ERR_EXIT(rv);

        /* set port speed */
        flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_speed_set(unit, port, flags, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        /*Set MAC Timestamp Delay*/
        _SOC_IF_ERR_EXIT(xlmac_timestamp_byte_adjust_set(unit, port, 0, config->speed));

        /* set encapsulation */
        flags = XLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS;
        rv = xlmac_encap_set (unit, port, flags, config->encap_mode);
        _SOC_IF_ERR_EXIT(rv);

        rv = xlmac_strict_preamble_set(unit, port, (config->speed >= 10000));
        _SOC_IF_ERR_EXIT(rv);

        /* De-Assert SOFT_RESET */
        rv = pm4x2p5_port_soft_reset(unit, port, pm_info, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init (&phy_interface_config));

    _SOC_IF_ERR_EXIT(_pm4x2p5_line_side_phymod_interface_get(unit, port,
                                                            pm_info, config,
                                                            &phymod_interface));
    phy_interface_config.interface_type = phymod_interface;
    _SOC_IF_ERR_EXIT(_pm4x2p5_phymod_interface_mode_get(unit, port, pm_info, config, &phy_interface_config));
    _SOC_IF_ERR_EXIT(PM4x2P5_SPEED_SET(unit, pm_info,
                                      config->speed, port_index));

    phy_interface_config.data_rate         = config->speed;
    phy_interface_config.pll_divider_req = config->pll_divider_req;
    phy_interface_config.ref_clock         = PM_4x2P5_INFO(pm_info)->ref_clk;

    /*
     * copy const *config to config_temp cause
     * config.interface may come from storage
     */
    _SOC_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &config_temp));
    sal_memcpy(&config_temp, config, sizeof(portmod_port_interface_config_t));

    rv = PM4x2P5_IS_INTERFACE_RESTORE_GET(unit, pm_info,
                                         &is_interface_restore, port_index);
    _SOC_IF_ERR_EXIT(rv);

    if (is_interface_restore) {
        /* overwrite config->interface form storage */
         rv = PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index);
         _SOC_IF_ERR_EXIT(rv);
        config_temp.interface  = interface ;
        is_interface_restore = 0;
        rv = PM4x2P5_IS_INTERFACE_RESTORE_SET(unit, pm_info,
                                          is_interface_restore, port_index);
        _SOC_IF_ERR_EXIT(rv);
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access_arr ,(1+MAX_PHYN),
                                           &nof_phys));

    if (config->serdes_interface != SOC_PORT_IF_NULL) {
        _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,
                        config->speed,
                        config->serdes_interface,
                        &phymod_serdes_interface));
    }
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_set(unit, port, phy_access_arr, nof_phys,
                                                config_temp.flags,
                                                &phy_interface_config,
                                                phymod_serdes_interface,
                                                PM_4x2P5_INFO(pm_info)->ref_clk,
                                                all_phy));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_interface_config_get (int unit, int port, pm_info_t pm_info,
                                      portmod_port_interface_config_t* config, int phy_init_flags)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t      phy_interface_config;
    int                          port_index;
    uint32_t                     bitmap, is_interface_restore = 0;
    int flags=0, nof_phys = 0, rv = 0, ref_clock, max_speed;
    phymod_interface_t           prev_phymod_if;
    soc_port_if_t   interface = SOC_PORT_IF_NULL;

    SOC_INIT_FUNC_DEFS;

    ref_clock = PM_4x2P5_INFO(pm_info)->ref_clk;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_interface_config));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    rv = PM4x2P5_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index);
    _SOC_IF_ERR_EXIT(rv);

    phy_interface_config.data_rate =  max_speed;
    phy_interface_config.ref_clock =  ref_clock;

    _SOC_IF_ERR_EXIT( portmod_intf_to_phymod_intf(unit, max_speed, interface,
                                                  &prev_phymod_if));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                    0, ref_clock,
                                                    &phy_interface_config, phy_init_flags));

    _SOC_IF_ERR_EXIT(PM4x2P5_IS_INTERFACE_RESTORE_GET(unit, pm_info,
                                                     &is_interface_restore,
                                                     port_index));
    /*
     * if the interface is already set with PARAM_SET_ONLY,
     * it should use h/w interface to config.
     */
    if (is_interface_restore) {
        rv = portmod_intf_from_phymod_intf(unit,
                    phy_interface_config.interface_type, &(config->interface));
        _SOC_IF_ERR_EXIT(rv);
    }

    config->speed           = phy_interface_config.data_rate;
    config->interface_modes = phy_interface_config.interface_modes;
    config->flags           = 0;
    config->interface       = interface;

    _SOC_IF_ERR_EXIT(
                PM4x2P5_SERDES_INTERFACE_TYPE_GET(unit, pm_info,
                                                 &config->serdes_interface,
                                                 port_index));

    rv = portmod_intf_from_phymod_intf (unit,
             phy_interface_config.interface_type, &(config->interface));
    _SOC_IF_ERR_EXIT(rv);

    rv = xlmac_encap_get (unit, port, &flags, &config->encap_mode);
    _SOC_IF_ERR_EXIT(rv);

    SHR_BITCOUNT_RANGE(&bitmap, config->port_num_lanes, 0,
                       PM4X2P5_LANES_PER_CORE);
    rv = PM4x2P5_MAX_SPEED_GET(unit, pm_info, &config->max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x2P5_INTERFACE_MODES_GET(unit, pm_info, &config->interface_modes,
                                    port_index);
    _SOC_IF_ERR_EXIT(rv);



exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    uint32_t tmp_enable=0;
    phymod_loopback_mode_t phymod_lb_type;
    int rv = PHYMOD_E_NONE;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    switch(loopback_type) {
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(xlmac_loopback_get(unit, port, enable));
            break;

        case portmodLoopbackPhyRloopPCS:
        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
             if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
                (*enable) = 0; /* No phy --> no phy loopback*/
             } else {
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

             }
             break;
        default:
            (*enable) = 0; /* not supported --> no loopback */
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    phymod_loopback_mode_t phymod_lb_type;

    SOC_INIT_FUNC_DEFS;

    /* loopback type validation*/
    switch(loopback_type) {
    case portmodLoopbackMacOuter:
        _SOC_IF_ERR_EXIT(xlmac_loopback_set(unit, port, enable));
        break;

    case portmodLoopbackPhyRloopPCS:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyGloopPCS: /*slide*/
        if (PM_4x2P5_INFO(pm_info)->nof_phys != 0) {
            if (enable) {
                _SOC_IF_ERR_EXIT(xlmac_loopback_set(unit, port, 0));
            }
            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));

            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            if(phymodDispatchTypeViper == phy_access[nof_phys-1].type && loopback_type == portmodLoopbackPhyGloopPMD) {
                phymod_lb_type = phymodLoopbackGlobal;
            }
            _SOC_IF_ERR_EXIT(portmod_port_phychain_loopback_set(unit, port, phy_access,
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

    if (enable)
        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));

exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_ability_local_get (int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                   portmod_port_ability_t* ability)
{
    portmod_port_ability_t legacy_phy_ability;
    int                 fiber_pref, port_index;
    int                 max_speed = 1000;
    int                 num_of_lanes = 0;
    uint32              bitmap, tmpbit, interface_modes;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys, rv = SOC_E_NONE, is_legacy_phy;

    SOC_INIT_FUNC_DEFS;

    sal_memset(ability, 0, sizeof(portmod_port_ability_t));
    sal_memset(&legacy_phy_ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (_pm4x2p5_port_index_get (unit, port, pm_info, &port_index, &bitmap));

    rv = PM4x2P5_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x2P5_INTERFACE_MODES_GET(unit, pm_info, &interface_modes,
                                    port_index);
    _SOC_IF_ERR_EXIT(rv);

    fiber_pref = (interface_modes & PHYMOD_INTF_MODES_FIBER)? 1 : 0;

    tmpbit = bitmap ;
    while(tmpbit) {
        num_of_lanes += ((tmpbit&1)?1:0) ;
        tmpbit >>= 1 ;
    }

    ability->loopback  = SOC_PA_LB_PHY | SOC_PA_LB_MAC;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->fec       = SOC_PA_FEC_NONE;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE;

    if (num_of_lanes == 1) {
        if (!fiber_pref) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        }
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;

        rv = PM4x2P5_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
        _SOC_IF_ERR_EXIT(rv);

        if (max_speed == 2500) {
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;

        /* single lane port */
        ability->flags = SOC_PA_AUTONEG;
    } else {
        rv = PM4x2P5_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
        _SOC_IF_ERR_EXIT(rv);

        if (max_speed == 10000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }

        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->flags     = SOC_PA_AUTONEG;
    }

    /* If legacy external phy present retrieve the local ability from the
     * external phy as it has more variants and the ability might vary for each */
    if (nof_phys > 1) {
        /* check if the legacy phy present */
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, phy_access[nof_phys-1].access.addr, &is_legacy_phy));
        if (is_legacy_phy) {
            /* call portmod phy chain function to retrieve the ability */
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_local_ability_get(unit, port, phy_access,
                                                         nof_phys, &legacy_phy_ability));
            /* PHY driver returns all the speeds supported by PHY
             * Advertise only the speeds supported by both PortMacro and the PHY
             */
            legacy_phy_ability.speed_full_duplex &= ability->speed_full_duplex;
            *ability = legacy_phy_ability;
        } else {
            /* We might need to consider calling portmod phy chain for
             * non legacy phys as well*/
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "%-22s:unit=%d p=%d sp=%08x bitmap=%x\n"),
              __func__, unit, port, ability->speed_full_duplex, bitmap));

exit:
    SOC_FUNC_RETURN;

}

int pm4x2p5_port_autoneg_set (int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                             const phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys, rv;
    int port_index, an_cl37, cl37_sgmii_cnt;
    uint32_t bitmap, an_done;
    phymod_autoneg_control_t  *pAn = (phymod_autoneg_control_t*)an;
    soc_port_if_t   interface = SOC_PORT_IF_NULL;
    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (pAn->num_lane_adv == 0) {
            pAn->num_lane_adv = _pm4x2p5_nof_lanes_get(unit, port, pm_info);
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            rv = PM4x2P5_AN_MODE_GET(unit, pm_info, &pAn->an_mode, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = phymod_AN_MODE_SGMII;
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        rv = PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index);
        _SOC_IF_ERR_EXIT(rv);
        /* Setting SerDes SGMII AN mode*/
        if (nof_phys <= 1) {
            if ((pAn->an_mode == phymod_AN_MODE_CL37) && (interface == SOC_PORT_IF_SGMII)) {
                pAn->an_mode = phymod_AN_MODE_SGMII;
            }
        }

        /* In case of warm boot, get autoneg informaton from hardware */
        if (SOC_WARM_BOOT(unit)) {
            if (pAn->an_mode >= phymod_AN_MODE_Count) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_autoneg_get(unit, port, phy_access, nof_phys, phy_flags, pAn, &an_done));
            }
        }

        rv = PM4x2P5_AN_CL37_GET(unit, pm_info, &an_cl37, port_index);
        _SOC_IF_ERR_EXIT(rv);
        if (an_cl37 == PORTMOD_CL37_SGMII_COMBO) {
           if (pAn->enable) {
              cl37_sgmii_cnt = 0;
              _SOC_IF_ERR_EXIT(PM4x2P5_CL37_SGMII_CNT_SET(unit, pm_info,
                                                         cl37_sgmii_cnt,
                                                         port_index));
           }
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_set(unit, port, phy_access, nof_phys, phy_flags, an));
    }

exit:
    SOC_FUNC_RETURN;

}

int pm4x2p5_port_autoneg_get (int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                             phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int         nof_phys;
    uint32      an_done;
    int port_index, rv;
    uint32_t bitmap;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        an->num_lane_adv = _pm4x2p5_nof_lanes_get(unit, port, pm_info);
        rv = PM4x2P5_AN_MODE_GET(unit, pm_info, &an->an_mode, port_index);
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

int pm4x2p5_port_autoneg_status_get(int unit, int port, pm_info_t pm_info, phymod_autoneg_status_t* an_status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Autoneg isn't supported")));
    } else {
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_status_get(unit, port, phy_access, nof_phys, an_status));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_link_status_get(unit, port, phy_access, nof_phys, flags, (uint32_t*) link));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_link_latch_down_get(int unit, int port, pm_info_t pm_info, uint32 flags, int* link)
{
    int first_index, rv;
    uint32 reg32_val, bitmap;
    soc_field_t status_field;
    SOC_INIT_FUNC_DEFS;

    *link = 0;

    rv = _pm4x2p5_port_index_get(unit, port, pm_info, &first_index, &bitmap);
    _SOC_IF_ERR_EXIT(rv);

     switch(first_index) {
        case 0:
            status_field = PORT0_LINKSTATUSf;
            break;

        case 1:
            status_field = PORT1_LINKSTATUSf;
            break;

        case 2:
            status_field = PORT2_LINKSTATUSf;
            break;

        case 3:
            status_field = PORT3_LINKSTATUSf;
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

    rv = READ_XPORT_LINKSTATUS_DOWNr(unit, port, &reg32_val);
    _SOC_IF_ERR_EXIT(rv);

    (*link) = soc_reg_field_get(unit, XPORT_LINKSTATUS_DOWNr,
                                        reg32_val, status_field);

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PM4x2P5")));
    }

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                            (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_set(unit, phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;

}

int pm4x2p5_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC PRBS is not supported for PM4x2P5")));
    }

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                       (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_get(unit, phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}



int pm4x2p5_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (
              _SOC_MSG("MAC PRBS is not supported for PM4x2P5")));
    }

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
              _SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_set(unit, phy_access, nof_phys, flags, enable));

exit:
    SOC_FUNC_RETURN;

}

int pm4x2p5_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 is_enabled;
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PM4x2P5")));
    }

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                    (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_get(unit, phy_access, nof_phys, flags, &is_enabled));

    (*enable) = (is_enabled ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("MAC PRBS is not supported for PM4x2P5")));
    }

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                    (_SOC_MSG("phy PRBS isn't supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_status_get(unit, phy_access, nof_phys, flags, status));

exit:
    SOC_FUNC_RETURN;

}


int pm4x2p5_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int* nof_lanes)
{
    int temp_nof_lanes = 0;
    SOC_INIT_FUNC_DEFS;

    temp_nof_lanes = _pm4x2p5_nof_lanes_get(unit, port, pm_info);
    if (0 == temp_nof_lanes) {
        SOC_EXIT;
    }
    else{
        *nof_lanes = temp_nof_lanes;
    }

exit:
    SOC_FUNC_RETURN;

}

int pm4x2p5_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
    int port_index;
    uint32 bitmap;
    int index;
    int done=0;
    int i,j, is_legacy_phy;
    uint32 xphy_id;
    phymod_core_access_t core_access;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (phyn > PM_4x2P5_INFO(pm_info)->nof_phys[port_index]) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
                       PM_4x2P5_INFO(pm_info)->nof_phys[port_index] - 1, phyn));
    }

    if (phyn < 0)
    {
        phyn = PM_4x2P5_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if ( phyn == 0 ) {
        sal_memcpy(&core_access_arr[0], &(PM_4x2P5_INFO(pm_info)->int_core_access),
                     sizeof(phymod_core_access_t));
        *nof_cores = 1;
    } else {
        index = 0;
        done = 0;
        for (i=0 ; (i< MAX_PORTS_PER_PM4X2P5); i++) {
            if (bitmap & (0x1U << i)) {
                xphy_id = PM_4x2P5_INFO(pm_info)->lane_conn[phyn-1][i].xphy_id;
                if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                    if (index == 0) {
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        sal_memcpy(&core_access_arr[index], &core_access,
                                   sizeof(phymod_core_access_t));
                        index++;
                    } else {
                        for ( j = 0 ; (j < index) &&(!done) ; j++) {
                             if ((!sal_memcmp (&core_access_arr[j], &core_access,
                                               sizeof(phymod_core_access_t)))) { /* found a match */
                                done = 1;
                            }
                        }
                        if ((!done) && (index < max_cores)) {
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
        if (phyn == PM_4x2P5_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_check_legacy_phy(int unit, int port, pm_info_t pm_info, int* legacy_phy)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    *legacy_phy = 0;

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
     while (nof_phys > 1) {
         nof_phys--;

         _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, phy_access[nof_phys].access.addr, legacy_phy));
         if (*legacy_phy) {
             break;
         }
     }

exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_reset_set(int unit, int port, pm_info_t pm_info, int mode, int opcode, int value)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           (phymod_phy_access_t*)core_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_set(unit, core_access, nof_phys,
                                            (phymod_reset_mode_t)mode,
                                            (phymod_reset_direction_t)value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_reset_get(int unit, int port, pm_info_t pm_info, int mode, int opcode, int* value)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           (phymod_phy_access_t*)core_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_get(unit, core_access, nof_phys,
                       (phymod_reset_mode_t)mode,
                       (phymod_reset_direction_t*)value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    phymod_phy_access_t              phy_access[1+MAX_PHYN];
    phymod_autoneg_ability_t         an_ability;
    portmod_port_ability_t        port_ability;

    int     nof_phys = 0, port_index, flags;
    uint32    bitmap;
    int     rv;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_get(unit, port, phy_access, nof_phys, phy_flags, &an_ability, &port_ability));

    portmod_common_phy_to_port_ability(&an_ability, ability);

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    rv = PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &ability->interface,
                                   port_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(xlmac_encap_get (unit, port, &flags, &ability->encap));

exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0, port_index, rv;
    uint32 bitmap, interface_modes;
    portmod_port_ability_t        port_ability;
    phymod_autoneg_ability_t    an_ability;
    portmod_access_get_params_t params;

    int port_num_lanes, line_interface, cx4_10g = 0;
    int an_cl72 = 0, an_fec = 0, hg_mode=0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = 0;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT
        (pm4x2p5_port_ability_local_get(unit, port, pm_info, phy_flags, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.pause               &= ability->pause;
    port_ability.interface           &= ability->interface;
    port_ability.medium            &= ability->medium;
    port_ability.eee               &= ability->eee;
    port_ability.loopback           &= ability->loopback;
    port_ability.flags               &= ability->flags;
    port_ability.speed_half_duplex &= ability->speed_half_duplex;
    port_ability.speed_full_duplex &= ability->speed_full_duplex;

    SHR_BITCOUNT_RANGE(&bitmap, port_num_lanes, 0, PM4X2P5_LANES_PER_CORE);
    rv = PM4x2P5_INTERFACE_TYPE_GET(unit, pm_info, &line_interface, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x2P5_INTERFACE_MODES_GET(unit, pm_info, &interface_modes,
                                    port_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    portmod_common_port_to_phy_ability(&port_ability, &an_ability,
                                       port_num_lanes, line_interface, cx4_10g,
                                       an_cl72, an_fec, hg_mode);

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_set(unit, port, phy_access, nof_phys, phy_flags, &an_ability, &port_ability));

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

static int _xport_pfc_config_set(int unit, int port, uint32 value)
{
    int rv = SOC_E_UNAVAIL;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,MAC_RSV_MASKr)) {
        rv = READ_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* MAC_RSV_MASK: Bit 18 PFC frame detected
             * Enable  PFC Frame : Set 0. Go through
             * Disable PFC Frame : Set 1. Purged.
             */
            if (value) {
                rval32 &= ~(1 << 18);
            } else {
                rval32 |= (1 << 18);
            }
            rv = WRITE_MAC_RSV_MASKr(unit, port, rval32);
        }
    }

    return rv;
}


int pm4x2p5_port_pfc_config_set(int unit, int port, pm_info_t pm_info, const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(xlmac_pfc_config_set(unit, port, pfc_cfg));
    _SOC_IF_ERR_EXIT(_xport_pfc_config_set(unit, port, pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN;

}

static int _xport_pfc_config_get(int unit, int port, uint32* value)
{
    int rv = SOC_E_NONE;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,MAC_RSV_MASKr)) {
        rv = READ_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
             /* MAC_RSV_MASK: Bit 18 PFC frame detected
             * Enable  PFC Frame : Set 0. Go through
             * Disable PFC Frame : Set 1. Purged.
             */
            *value = ((rval32 & (0x1 << 18)) >> 18) ? 0 : 1;
        }
    }

    return rv;
}

int pm4x2p5_port_pfc_config_get(int unit, int port, pm_info_t pm_info, portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(xlmac_pfc_config_get(unit, port, pfc_cfg));

    _SOC_IF_ERR_EXIT(_xport_pfc_config_get(unit, port, &pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    int link, flags = 0;
    int duplex = 0, tx_pause = 0, rx_pause = 0;
    phymod_autoneg_status_t autoneg;
    portmod_pause_control_t pause_control;
    portmod_port_ability_t local_advert, remote_advert;
    portmod_port_interface_config_t interface_config;

    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    uint32 int_lane_mask=0xf;
    int is_xlmac_en = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_autoneg_status_t_init(&autoneg));
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    sal_memset(&local_advert, 0, sizeof(portmod_port_ability_t));
    sal_memset(&remote_advert, 0, sizeof(portmod_port_ability_t));

    if (update_control->link_status == -1) {
        _SOC_IF_ERR_EXIT(pm4x2p5_port_link_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY,&link));
    } else {
        link = update_control->link_status;
    }

    if (PORTMOD_PORT_UPDATE_F_UPDATE_SERDES_LINK_GET(update_control)) {
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));

        /* only look at the lanes that are used. */
        phy_access[0].access.lane_mask &= int_lane_mask;
        return (SOC_E_NONE);
    }



    if (!link) {
        /* PHY is down.  Disable the MAC. */
        if (PORTMOD_PORT_UPDATE_F_DISABLE_MAC_GET(update_control)) {
            _SOC_IF_ERR_EXIT(xlmac_enable_set(unit, port, 0, 0));
        }

        /* TBD - do we need phymod_link_down_event ? */
    } else {
        _SOC_IF_ERR_EXIT(xlmac_enable_get(unit, port, 0, &is_xlmac_en));
        if (is_xlmac_en == 0) {
            _SOC_IF_ERR_EXIT(xlmac_enable_set(unit, port, 0, 1));
            if(PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
                _SOC_IF_ERR_EXIT(pm4x2p5_port_soft_reset(unit, port, pm_info, 0));
            }
        }

        /* TBD - do we need phymod_link_up_event ? */

        /* In case of AN - need to update MAC settings*/
        _SOC_IF_ERR_EXIT(pm4x2p5_port_autoneg_status_get(unit, port, pm_info, &autoneg));

        if (autoneg.enabled && autoneg.locked) {
            /* update MAC */
            _SOC_IF_ERR_EXIT(pm4x2p5_port_interface_config_get(unit, port, pm_info, &interface_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
            flags = XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
            _SOC_IF_ERR_EXIT(xlmac_speed_set(unit, port, flags, interface_config.speed));

            /*Set MAC Timestamp Delay*/
            _SOC_IF_ERR_EXIT(xlmac_timestamp_byte_adjust_set(unit, port, 0, interface_config.speed));

            if(PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
                _SOC_IF_ERR_EXIT(pm4x2p5_port_soft_reset(unit, port, pm_info, 0));
            }
            /* update pause in MAC on the base of local and remote pause ability*/
            _SOC_IF_ERR_EXIT(pm4x10_port_duplex_get(unit, port, pm_info, &duplex));
            _SOC_IF_ERR_EXIT(pm4x2p5_port_ability_advert_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &local_advert));
            _SOC_IF_ERR_EXIT(pm4x10_port_ability_remote_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &remote_advert));
            /* IEEE 802.3 Flow Control Resolution.
                   * Please see $SDK/doc/pause-resolution.txt for more information. */
            if (duplex) {
                tx_pause = \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    (local_advert.pause & SOC_PA_PAUSE_RX)) || \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    !(remote_advert.pause & SOC_PA_PAUSE_TX) && \
                    (local_advert.pause & SOC_PA_PAUSE_TX));
                rx_pause = \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    (local_advert.pause & SOC_PA_PAUSE_RX)) || \
                    ((local_advert.pause & SOC_PA_PAUSE_RX) && \
                    (remote_advert.pause & SOC_PA_PAUSE_TX) && \
                    !(local_advert.pause & SOC_PA_PAUSE_TX));
            }
            else {
                rx_pause = tx_pause = 0;
            }
            _SOC_IF_ERR_EXIT(pm4x10_port_pause_control_get(unit, port, pm_info, &pause_control));
            if ((pause_control.rx_enable != rx_pause) || (pause_control.tx_enable != tx_pause)) {
                pause_control.rx_enable = rx_pause;
                pause_control.tx_enable = tx_pause;
                _SOC_IF_ERR_EXIT(pm4x10_port_pause_control_set(unit, port, pm_info, &pause_control));
            }
        }

        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_drv_name_get(int unit, int port, pm_info_t pm_info, char* name, int len)
{
    strncpy(name, "PM4X2P5 Driver", len);
    return (SOC_E_NONE);
}

int pm4x2p5_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    SOC_INIT_FUNC_DEFS;

    /* Validate parameters */
    _SOC_IF_ERR_EXIT(portmod_xphy_lane_connection_t_validate(unit,lane_connection));

    if (MAX_PHYN <= phyn) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    PORTMOD_PBMP_ITER(PM_4x2P5_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x2P5_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    sal_memcpy(candidate_lane_conn, lane_connection, sizeof(portmod_xphy_lane_connection_t));

    /* set the number of phys to current iphy. If nof_phy is already counted for this
       phyn, keep the same nof_phys. */
    if (PM_4x2P5_INFO(pm_info)->nof_phys[lane_index] <= phyn) {
        PM_4x2P5_INFO(pm_info)->nof_phys[lane_index] = phyn+1;
    }

exit:
   SOC_FUNC_RETURN;

}


int pm4x2p5_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    SOC_INIT_FUNC_DEFS;

    if (MAX_PHYN <= phyn) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    PORTMOD_PBMP_ITER(PM_4x2P5_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x2P5_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    sal_memcpy( lane_connection, candidate_lane_conn, sizeof(portmod_xphy_lane_connection_t));
    portmod_xphy_lane_connection_t_init(unit, candidate_lane_conn); 

    /* decrement the nof_phys for that lane */
    if (phyn > 0 ) {
        PM_4x2P5_INFO(pm_info)->nof_phys[lane_index] = phyn;
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x2p5_port_ref_clk_get(int unit, int port, pm_info_t pm_info, int* ref_clk)
{
    *ref_clk = pm_info->pm_data.pm4x2p5_db->ref_clk;

    return (SOC_E_NONE);
}

int pm4x2p5_port_lag_failover_status_toggle(int unit, int port, pm_info_t pm_info)
{
    return(_xport_lag_failover_status_toggle(unit, port));
}


int pm4x2p5_port_mode_set(int unit, int port, pm_info_t pm_info, const portmod_port_mode_info_t* mode)
{
    return (_xport_mode_set(unit, port, mode->cur_mode));
}

int pm4x2p5_port_mode_get(int unit, int port, pm_info_t pm_info, portmod_port_mode_info_t* mode)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x2p5_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (rv) return (rv);

    return(_xport_mode_get(unit, port, port_index, &mode->cur_mode, &mode->lanes));
}

int pm4x2p5_port_mac_reset_set(int unit, int port, pm_info_t pm_info, int val)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    /* if callback defined, go to local soft reset function */
    rv = (PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) ? pm4x2p5_port_soft_reset(unit, port, pm_info, val) : xlmac_soft_reset_set(unit, port, val);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;

}

int pm4x2p5_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (xlmac_soft_reset_get(unit, port, val));
}

int pm4x2p5_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    uint64 reg_val, old_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
    old_val = reg_val;

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, SOFT_RESETf, 1);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, old_val));

exit:
    SOC_FUNC_RETURN;
}

/* in_out: 1= in reset, 0= in and out of reset */
STATIC
int pm4x2p5_port_soft_reset(int unit, int port, pm_info_t pm_info, int in_out)
{
    int rv, rx_enable = 0;
    int phy_port, block, bindex, i;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    uint64 mac_ctrl;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    /* Callback soft reset function */
    if (PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (in_out == 0) {

        rv = xlmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* Drain cells */
        rv = xlmac_drain_cell_get(unit, port, &drain_cells);
        _SOC_IF_ERR_EXIT(rv);

        /* Start TX FIFO draining */
        rv = xlmac_drain_cell_start(unit, port);
        _SOC_IF_ERR_EXIT(rv);

        /* De-assert SOFT_RESET to let the drain start */
        rv = xlmac_soft_reset_set(unit, port, 0);
        _SOC_IF_ERR_EXIT(rv);

        /* Wait until TX fifo cell count is 0 */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            rv = xlmac_txfifo_cell_cnt_get(unit, port, &cell_count);
               _SOC_IF_ERR_EXIT(rv);
               if (cell_count == 0) {
                    break;
            }
            if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_UP(unit, port,
                                   "ERROR: u=%d p=%d timeout draining "
                                   "TX FIFO (%d cells remain)\n"),
                               unit, port, cell_count));
                    return SOC_E_INTERNAL;
            }
        }

        /* Stop TX FIFO draining */
        rv = xlmac_drain_cell_stop (unit, port, &drain_cells);
        _SOC_IF_ERR_EXIT(rv);

        /* Reset port FIFO */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
       for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                if ((SOC_BLOCK_INFO(unit, block).type == SOC_BLK_XLPORT) ||
                    (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_XLPORTB0)) {
                    bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                    _SOC_IF_ERR_EXIT(portmod_port_soft_reset_toggle(unit, port, bindex));
                    break;
            }
        }
    }

    /* Put port into SOFT_RESET */
    rv = xlmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

    if (in_out == 0) {

        /* Callback soft reset function */
       if (PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
            rv = PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
               _SOC_IF_ERR_EXIT(rv);
               soc_reg64_field32_set (unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0); /*make sure restored data wont put mac back in reset*/
        }

        /* Enable TX, set RX, de-assert SOFT_RESET */
       rv = xlmac_egress_queue_drain_rx_en(unit, port, rx_enable);
        _SOC_IF_ERR_EXIT(rv);

        /* Restore XLMAC_CTRL to original value */
        rv = xlmac_mac_ctrl_set(unit, port, mac_ctrl);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* Callback soft reset function */
    if (PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x2P5_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy failover is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_failover_mode_set(unit, phy_access, nof_phys, failover));

exit:
    SOC_FUNC_RETURN;
}


int pm4x2p5_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x2p5_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x2P5_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy failover is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_failover_mode_get(unit, phy_access, nof_phys, failover));

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{

    SOC_INIT_FUNC_DEFS;

    PM_4x2P5_INFO(pm_info)->lane_map = *lane_map;


    SOC_FUNC_RETURN;

}

int pm4x2p5_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{

    SOC_INIT_FUNC_DEFS;

    *lane_map = PM_4x2P5_INFO(pm_info)->lane_map;


    SOC_FUNC_RETURN;

}


int pm4x2p5_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    SOC_INIT_FUNC_DEFS;

    PM_4x2P5_INFO(pm_info)->polarity = *polarity;


    SOC_FUNC_RETURN;
}

int pm4x2p5_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    SOC_INIT_FUNC_DEFS;

    *polarity = PM_4x2P5_INFO(pm_info)->polarity;


    SOC_FUNC_RETURN;
}

int pm4x2p5_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _rv = portmod_port_phychain_phy_link_up_event(unit, port, phy_access, nof_phys);

exit:
    SOC_FUNC_RETURN;
}

int pm4x2p5_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _rv = portmod_port_phychain_phy_link_down_event(unit, port, phy_access, nof_phys);

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM4X2P5_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
