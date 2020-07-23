/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF PORT
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <shared/bsl.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/stat.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/fabric.h>

#include <bcm_int/common/link.h>

#include <bcm_int/dnxf_dispatch.h>
#include <bcm_int/dnxf/link.h>
#include <bcm_int/dnxf/fabric.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf/tune/tune.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/phyreg.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#include <soc/portmod/portmod_common.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
#include <shared/util.h>

/**********************************************************/
/*                  Defines                               */
/**********************************************************/

#define BCM_DNXF_PORT_INDEX_IN_PM_GET(port) \
     ((port) % DNXC_PORT_FABRIC_LANES_PER_CORE)

#define BCM_DNXF_PORT_GET(pm_id, port_index_in_pm) \
     (((pm_id) * DNXC_PORT_FABRIC_LANES_PER_CORE) + (port_index_in_pm))

#define BCM_DNXF_PORT_MAC_CELL_SHAPPER_SUPPORT_CHECK(unit) \
    do { \
        if (!dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_mac_cell_shaper_supported)) \
        { \
            SHR_ERR_EXIT(_SHR_E_UNIT, "MAC Cell shaper is not supported for this device \n"); \
        } \
    } while(0); \

/**********************************************************/
/*                  Structs                               */
/**********************************************************/
typedef struct _bcm_dnxf_port_resource_multi_pm_config_s {
    int nof_ports;                                                              /* nof ports to configure in current PM */
    int ports_to_set[DNXC_PORT_FABRIC_LANES_PER_CORE];                          /* is it required to set speed config for the port. if ports_to_set[0] == 1 -> need to set speed config for first port in this pm */
    int enable_state[DNXC_PORT_FABRIC_LANES_PER_CORE];                          /* enable state[i] stores original enable state of ports[i] port */
    portmod_speed_config_t port_speed_config[DNXC_PORT_FABRIC_LANES_PER_CORE];  /* port_speed_config[i] holds the speed config of of ports[i] port */
    portmod_vco_type_t tvco;                                                    /* the suggested TVCO for the PM based on PM ports speeds given */
    portmod_vco_type_t ovco;                                                    /* the suggested OVCO for the PM based on PM ports speeds given */
    int is_tvco_new;                                                            /* is the suggested TVCO different from existing one */
    int is_ovco_new;                                                            /* is the suggested OVCO different from existing one */
} _bcm_dnxf_port_resource_multi_pm_config_t;

/**********************************************************/
/*                  Global                                */
/**********************************************************/
static pbmp_t _bcm_dnxf_port_prbs_mac_mode[BCM_MAX_NUM_UNITS];

/**********************************************************/
/*                  Verifications                         */
/**********************************************************/


/**********************************************************/
/*                  Internal functions                    */
/**********************************************************/  

int
dnxf_port_deinit(int unit)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_deinit,(unit));
    SHR_IF_ERR_CONT(rc);

    SHR_FUNC_EXIT;
}

/* get TX FIR parameters of the port from dnxf data:
 * SoC property values or default values, in case property wasn't set */
shr_error_e
_bcm_dnxf_port_phy_tx_params_set(int unit, bcm_port_t port)
{
    bcm_port_phy_tx_t tx;
    int lane_rate;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    bcm_port_phy_tx_t_init(&tx);
    /* get TX FIR parameters from dnxf data - SoC property values if it was set, or dnxf data default init values */
    tx.pre2 = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->pre2;
    tx.pre = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->pre;
    tx.main = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->main;
    tx.post = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->post;
    tx.post2 = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->post2;
    tx.post3 = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->post3;
    tx.tx_tap_mode = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->tx_tap_mode;
    tx.signalling_mode = dnxf_data_port.static_add.serdes_tx_taps_get(unit, port)->signalling_mode;

    /*
     * Get port lane rate
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_lane_rate_get(unit, port, &lane_rate));

    /*
     * If main is INVALID, this means SoC property was not configured.
     * For NRZ rates - do nothing. The default values are configured in portmod attach.
     * For PAM4 rate - get the default values from phymod.
     */
    if ( (tx.main == DNXC_PORT_TX_FIR_INVALID_MAIN_TAP) && (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate)) )
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_taps_default_get(unit, port, &tx));
    }

    /*
     * If main is valid, call TX taps set API.
     */
    if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_port_phy_tx_set(unit, port, &tx));
    }

exit:
    SHR_FUNC_EXIT;
}

/* Get SoC property values of phy lane config bits.
 * set/clear the bits in port_resource.phy_lane_config accordingly */
static shr_error_e
_bcm_dnxf_port_resource_lane_config_soc_properties_get(int unit, bcm_gport_t port, bcm_port_resource_t* port_resource)
{
    soc_dnxc_port_dfe_mode_t dfe;
    int media_type;
    int unreliable_los;
    int cl72_auto_polarity_enable;
    int cl72_restart_timeout_enable;
    soc_dnxc_port_channel_mode_t channel_mode;
    SHR_FUNC_INIT_VARS(unit);

    dfe = dnxf_data_port.static_add.serdes_lane_config_get(unit, port)->dfe;
    /* if dfe SoC property was configured */
    if (dfe != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (dfe)
        {
            case soc_dnxc_port_dfe_on:
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(port_resource->phy_lane_config);
                break;
            case soc_dnxc_port_dfe_off:
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(port_resource->phy_lane_config);
                break;
            case soc_dnxc_port_lp_dfe:
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(port_resource->phy_lane_config);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_dfe SoC property value. Allowed values are on|off|lp\n", port);
        }
    }

    media_type = dnxf_data_port.static_add.serdes_lane_config_get(unit, port)->media_type;
    /* if media type SoC property was configured */
    if (media_type != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (media_type)
        {
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE:
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE:
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS:
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(port_resource->phy_lane_config, media_type);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_media_type SoC property value. Allowed values are backplane|copper|optics\n", port);
        }
    }

    unreliable_los = dnxf_data_port.static_add.serdes_lane_config_get(unit, port)->unreliable_los;
    /* if unreliable los SoC property was configured */
    if (unreliable_los != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (unreliable_los)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(port_resource->phy_lane_config);
        }
    }

    cl72_auto_polarity_enable = dnxf_data_port.static_add.serdes_lane_config_get(unit, port)->cl72_auto_polarity_enable;
    /* if cl72 auto polarity SoC property was configured */
    if (cl72_auto_polarity_enable != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (cl72_auto_polarity_enable)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(port_resource->phy_lane_config);
        }
    }

    cl72_restart_timeout_enable = dnxf_data_port.static_add.serdes_lane_config_get(unit, port)->cl72_restart_timeout_enable;
    /* if cl72 restart timeout SoC property was configured */
    if (cl72_restart_timeout_enable != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (cl72_restart_timeout_enable)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_CLEAR(port_resource->phy_lane_config);
        }
    }

    channel_mode = dnxf_data_port.static_add.serdes_lane_config_get(unit, port)->channel_mode;
    /* if channel mode SoC property was configured */
    if (channel_mode != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (channel_mode)
        {
            case soc_dnxc_port_force_nr:
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(port_resource->phy_lane_config);
                break;
            case soc_dnxc_port_force_er:
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_SET(port_resource->phy_lane_config);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_channel_mode SoC property value. Allowed values are force_nr|force_er\n", port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**********************************************************/
/*                  Implementation                        */
/**********************************************************/  

/*
 * Function:
 *      bcm_dnxf_port_control_set
 * Purpose:
 *      set various features at the port level.
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Device or logical port number  
 *      type  - (IN)     Port feature enumerator 
 *      value - (IN)     Value of the bit field in port table
 * Returns:
 *      _SHR_E_NONE     No Error  
 *      _SHR_E_UNAVAIL  Feature unavailable  
 *      _SHR_E_PORT     Invalid Port number specified  
 *      _SHR_E_XXX      Error occurred  
 */
 
int 
bcm_dnxf_port_control_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_t type, 
    int value)
{
    int rc = _SHR_E_NONE;
    int prbs_mode = 0;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
      
    /*validation*/
    DNXF_LINK_INPUT_CHECK(unit, port);
    prbs_mode = SOC_PBMP_MEMBER(_bcm_dnxf_port_prbs_mac_mode[unit],port) ? 1: 0;
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    switch(type)
    {
        case bcmPortControlPCS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmPortControlPCS is no longer supported, please use bcm_port_resource_set to control fec\n");
            break;

        case bcmPortControlLinkDownPowerOn:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_power_set,(unit, port, 0, value ? soc_dnxc_port_power_on :  soc_dnxc_port_power_off));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsPolynomial:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_polynomial_set,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxEnable:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_tx_enable_set,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxEnable:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_rx_enable_set,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxInvertData:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_tx_invert_data_set,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxInvertData:
            rc = soc_dnxc_port_prbs_rx_invert_data_set(unit, port, prbs_mode, value);
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsMode:
            if(0 == value) { /*PRBS PHY MODE*/
               SOC_PBMP_PORT_REMOVE(_bcm_dnxf_port_prbs_mac_mode[unit], port); 
            } else if (1 == value){ /*PRBS MAC MODE*/
               SOC_PBMP_PORT_ADD(_bcm_dnxf_port_prbs_mac_mode[unit], port); 
            } else {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid value\n");
            }
            break;

        case bcmPortControlLowLatencyLLFCEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_low_latency_set,(unit, port, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlFecErrorDetectEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_fec_error_detect_set,(unit, port, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLlfcCellsCongestionIndEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_extract_cig_from_llfc_enable_set,(unit, port, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLLFCAfterFecEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_llfc_after_fec_enable_set,(unit, port, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlControlCellsFecBypassEnable:
            rc = soc_dnxc_port_control_cells_fec_bypass_enable_set(unit, port, value);
            SHR_IF_ERR_EXIT(rc);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d\n", type);
            break;
    }
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);    
    SHR_FUNC_EXIT;    
}


/*
 * Function:
 *      bcm_dnxf_port_control_get
 * Purpose:
 *      Get various features at the port level.
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Device or logical port number  
 *      type  - (IN)     Port feature enumerator 
 *      value - (IN)     Value of the bit field in port table
 * Returns:
 *      _SHR_E_NONE     No Error  
 *      _SHR_E_UNAVAIL  Feature unavailable  
 *      _SHR_E_PORT     Invalid Port number specified  
 *      _SHR_E_XXX      Error occurred  
 */
int 
bcm_dnxf_port_control_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_t type, 
    int *value)
{
    int rc = _SHR_E_NONE;
    int prbs_mode;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    /*validation*/
    DNXF_LINK_INPUT_CHECK(unit, port);
    prbs_mode = SOC_PBMP_MEMBER(_bcm_dnxf_port_prbs_mac_mode[unit],port) ? 1: 0;
    
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    DNXF_UNIT_LOCK_TAKE(unit);
    
    switch(type)
    {
        case bcmPortControlPCS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmPortControlPCS is no longer supported, please use bcm_port_resource_get to get the fec\n");
            break;
          
        case bcmPortControlLinkDownPowerOn:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_power_get,(unit, port, (soc_dnxc_port_power_t*)value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsPolynomial:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_polynomial_get,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxEnable:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_tx_enable_get,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxEnable:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_rx_enable_get,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxStatus:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_rx_status_get,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxInvertData:
            rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_prbs_tx_invert_data_get,(unit, port, prbs_mode, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxInvertData:
            rc = soc_dnxc_port_prbs_rx_invert_data_get(unit, port, prbs_mode, value);
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsMode:
            *value = prbs_mode; /*PRBS MAC MODE*/
            break;

        case bcmPortControlLowLatencyLLFCEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_low_latency_get,(unit, port,(int*) value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlFecErrorDetectEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_fec_error_detect_get,(unit, port,(int*) value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLlfcCellsCongestionIndEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_extract_cig_from_llfc_enable_get,(unit, port, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLLFCAfterFecEnable:
            rc=MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_control_llfc_after_fec_enable_get,(unit, port, value));
            SHR_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlControlCellsFecBypassEnable:
            rc = soc_dnxc_port_control_cells_fec_bypass_enable_get(unit, port, value);
            SHR_IF_ERR_EXIT(rc);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d\n", type);
            break;
    }
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);    
    SHR_FUNC_EXIT;  
}


/*
 * Function:
 *      bcm_dnxf_port_stat_get
 * Purpose:
 *      Get port counter value for specified port statistic type.
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Device or logical port number  
 *      stat  - (IN)     Type of the counter to retrieve
 *      val   - (OUT)    Pointer to a counter value
 * Returns:
 *      _SHR_E_XXX 
 */
int
bcm_dnxf_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint64 *val)
{
    bcm_stat_val_t stat_e;
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
   
    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    switch(stat)
    {
        case bcmPortStatIngressPackets:
            stat_e = snmpBcmRxDataCells;
            break;
          
        case bcmPortStatIngressBytes:
            stat_e = snmpBcmRxDataBytes;
            break;
        
        case bcmPortStatEgressPackets:
            stat_e = snmpBcmTxDataCells;
            break;
        
        case bcmPortStatEgressBytes:
            stat_e = snmpBcmTxDataBytes;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported stat %d\n", stat);
    }

    rc = bcm_dnxf_stat_get(unit, port, stat_e, val);
    SHR_IF_ERR_EXIT(rc);

exit:
   DNXF_UNIT_LOCK_RELEASE(unit);    
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      port      - (IN)     Device or logical port number  
 *      nstat     - (IN)     Number of elements in stat array
 *      stat_arr  - (IN)     Collected statistics descriptors array
 *      value_arr - (OUT)    Collected counters values 
 * Returns:
 *      _SHR_E_XXX 
 */
int
bcm_dnxf_port_stat_multi_get(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint64 *value_arr)
{
    int rc = _SHR_E_NONE;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

    for(i=0 ; i<nstat ; i++ ) {
        rc = bcm_dnxf_port_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT; 
}

int
bcm_dnxf_port_stat_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnxf_port_stat_get32 is not available\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_port_stat_multi_get32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnxf_port_stat_multi_get32 is not available\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_port_phy_reset_register(int unit, bcm_port_t port,
                            bcm_port_phy_reset_cb_t callback,
                            void  *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported\n");

exit:
    SHR_FUNC_EXIT; 
}

int
bcm_dnxf_port_phy_reset_unregister(int unit, bcm_port_t port,
                              bcm_port_phy_reset_cb_t callback,
                              void  *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported\n");

exit:
    SHR_FUNC_EXIT; 
}

int
bcm_dnxf_port_phy_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported\n");

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_probe
 * Purpose:
 *      Probe the PHY and set up the PHY and MAC for the specified ports.
 *      This is purely a discovery routine and does no configuration.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbmp - Bitmap of ports to probe.
 *      okay_pbmp (OUT) - Ports which were successfully probed.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_INTERNAL - internal error.
 * Notes:
 *      If error is returned, the port should not be enabled.
 *      Assumes port_init done.
 *      Note that if a PHY is not present, the port will still probe
 *      successfully.  The default driver will be installed.
 */

 int
bcm_dnxf_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp) {

    int rc = _SHR_E_NONE;
    int is_init_sequence = 0;
    bcm_port_t port;
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_info_t          *si;
    uint32 flags;
    int usec;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    /* Check if the ports beening probed are:
     * 1. Whitin a valid range
     * 2. Member of the supported phys PBMP
     */
    BCM_PBMP_ITER(pbmp, port) {
        /* SKU SUPPORT - get all supported device links*/
        if(!(BCM_PBMP_MEMBER(dnxf_data_port.general.supported_phys_get(unit)->pbmp, port))){
            SHR_ERR_EXIT(_SHR_E_PARAM, "Fabric link %d is disabled for device %s\n", port, soc_dev_name(unit));

        }
    }

    flags = soc->counter_flags;
    usec = soc->counter_interval;

    rc = soc_counter_stop(unit);
    SHR_IF_ERR_EXIT(rc);

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_NULL_CHECK(okay_pbmp, _SHR_E_PARAM, "okay_pbmp");
    BCM_PBMP_CLEAR(*okay_pbmp);

    rc = bcm_dnxf_init_check(unit);
    if (rc == _SHR_E_UNIT) /* init*/
    {
        is_init_sequence = 1;
    }
    else if (rc == _SHR_E_NONE)
    {
        is_init_sequence = 0;
    }
    else {
        SHR_IF_ERR_EXIT(rc);
    }

    /* If not in init CHECK If user is trying to probe already enabled link*/
    if (!is_init_sequence)
    {
        si  = &SOC_INFO(unit);
        BCM_PBMP_ITER(pbmp, port) 
        {
            if (SOC_PBMP_MEMBER(si->sfi.bitmap, port))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "link %d is already probed\n", port);
            }
        }
    }

    /*Initial port speed max*/
    BCM_PBMP_ITER(pbmp, port) {
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_speed_max,(unit, port, &(SOC_INFO(unit).port_speed_max[port])));
        SHR_IF_ERR_EXIT(rc);
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_probe, (unit, pbmp, okay_pbmp, is_init_sequence));
    SHR_IF_ERR_EXIT(rc);


    if (is_init_sequence /*init*/)
    {
        /*Do nothing*/
    } else
    {

        /* ALDWP should be configured at init time for every speed change*/
        /*Avoiding ALDWP config at init time*/
        rc = MBCM_DNXF_DRIVER_CALL(unit ,mbcm_dnxf_fabric_links_aldwp_init ,(unit));      
        SHR_IF_ERR_EXIT(rc);

        /*Additinal per port default values*/
        BCM_PBMP_ITER(pbmp, port) {
            SHR_IF_ERR_EXIT(bcm_dnxf_fabric_link_control_set(unit, port, bcmFabricLinkPcpEnable, 0x1));
        }

    } 

    if (!is_init_sequence)
    {
        DNXF_UNIT_LOCK_RELEASE(unit);

        rc = soc_counter_start(unit, flags, usec, PBMP_SFI_ALL(unit));
        SHR_IF_ERR_EXIT(rc);
        /*
         * Update linkscan thread for fabric link
         */
        BCM_PBMP_ITER(*okay_pbmp, port)
        {
            SHR_IF_ERR_EXIT(_bcm_linkscan_port_init(unit, port));
        }
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_detach
 * Purpose:
 *      Detach a port.  Set phy driver to no connection.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbmp - Bitmap of ports to detach.
 *      detached (OUT) - Bitmap of ports successfully detached.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_INTERNAL - internal error.
 * Notes:
 *      If a port to be detached does not appear in detached, its
 *      state is not defined.
 */

int
bcm_dnxf_port_detach(int unit, pbmp_t pbmp, pbmp_t *detached)
{
    int         rc = _SHR_E_NONE;
    bcm_port_t  port;
    soc_control_t       *soc = SOC_CONTROL(unit);
    uint32 flags = 0;
    int usec = 0;
    int linkscan_paused = 0;
    int counter_paused = 0;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    /* Check if the ports beening detached are:
     * 1. Whitin a valid range
     * 2. Member of the SFI enabled PBMP
     */
    BCM_PBMP_ITER(pbmp, port) {
        DNXF_LINK_INPUT_CHECK(unit, port);
    }

    flags = soc->counter_flags;
    usec = soc->counter_interval;

    SHR_NULL_CHECK(detached, _SHR_E_PARAM, "detached");
    SOC_PBMP_CLEAR(*detached);

    /*Pause counter thread*/
    rc = soc_counter_stop(unit);
    SHR_IF_ERR_EXIT(rc);
    counter_paused = 1;

    /*Pause linkscan thread*/
    BCM_PBMP_ITER(pbmp, port)
    {
        rc = bcm_dnxf_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE);
        SHR_IF_ERR_EXIT(rc);
    }
    SHR_IF_ERR_EXIT(_bcm_linkscan_pause(unit));
    linkscan_paused = 1;

    /**
     * Reset loopback to NONE
     */
    PBMP_ITER(pbmp, port) {
        rc = bcm_dnxf_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE);
        SHR_IF_ERR_EXIT(rc);
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    
    PBMP_ITER(pbmp, port) {
        rc = bcm_dnxf_port_enable_set(unit, port,0);
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit ,mbcm_dnxf_port_detach ,(unit, port));
        SHR_IF_ERR_EXIT(rc);

        SOC_PBMP_PORT_ADD(*detached, port);
    }

    DNXF_UNIT_LOCK_RELEASE(unit);

    /*Continue linkscan thread*/
    rc = _bcm_linkscan_continue(unit);
    linkscan_paused = 0;
    SHR_IF_ERR_EXIT(rc);

    /*Continuue counter thread*/
    rc = soc_counter_start(unit, flags, usec, PBMP_SFI_ALL(unit));
    counter_paused = 0;
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    if (linkscan_paused)
    {
        _bcm_linkscan_continue(unit);
    }
    if (counter_paused)
    {
        rc = soc_counter_start(unit, flags, usec, PBMP_SFI_ALL(unit));
        if(rc != _SHR_E_NONE) {
            cli_out("soc_counter_start failed.\n");
        }
    }
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_enable_set
 * Purpose:
 *      Physically enable/disable the MAC/PHY on this port.
 * Parameters:
 *      unit - unit #.
 *      port - port #.
 *      enable - TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      _SHR_E_XXX
 * Notes:
 *      If linkscan is running, it also controls the MAC enable state.
 */

int
bcm_dnxf_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int         rc;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_set, (unit, port, enable)); 
    SHR_IF_ERR_EXIT(rc);

    /*ALDWP should be configured at init time for every speed change*/
    /*Avoiding ALDWP config at init time*/
    rc = bcm_dnxf_init_check(unit);
    if (rc == _SHR_E_UNIT /*init*/)
    {
        /*Do nothing*/
    } else if (rc == _SHR_E_NONE)
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit ,mbcm_dnxf_fabric_links_aldwp_config ,(unit, port));
        SHR_IF_ERR_EXIT(rc);
    } else {
        SHR_IF_ERR_EXIT(rc);
    }

    /*Enable/Disable link pump when leaky bucket is down for better link utilization*/
    rc = MBCM_DNXF_DRIVER_CALL(unit ,mbcm_dnxf_port_pump_enable_set ,(unit, port, enable));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 

}

/*
 * Function:
 *      bcm_dnxf_port_enable_get
 * Purpose:
 *      Gets the enable state as defined by bcm_port_enable_set()
 * Parameters:
 *      unit - unit #.
 *      port - port #.
 *      enable - TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      _SHR_E_XXX
 * Notes:
 *      The PHY enable holds the port enable state set by the user.
 *      The MAC enable transitions up and down automatically via linkscan
 *      even if user port enable is always up.
 */

int
bcm_dnxf_port_enable_get(int unit, bcm_port_t port, int* enable)
{
    int         rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_get, (unit, port, enable)); 
    SHR_IF_ERR_EXIT(rc);


exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 

}

/*
 * Function:
 *      bcm_dnxf_port_link_status_get
 * Purpose:
 *      Return current Link up/down status, queries linkscan, if unable to
 *      retrieve status queries the PHY.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      status - (OUT) BCM_PORT_LINK_STATUS_DOWN \ BCM_PORT_LINK_STATUS_UP
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_link_status_get(int unit, bcm_port_t port, int *status)
{
    int up;
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = _bcm_link_get(unit, port, &up);
    if (rc == BCM_E_DISABLED)
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_link_status_get,(unit, port, &up));
        SHR_IF_ERR_EXIT(rc);
    }
    else
    {
        SHR_IF_ERR_EXIT(rc);
    }


    if(up) {
        *status = BCM_PORT_LINK_STATUS_UP;
    } else {
        *status = BCM_PORT_LINK_STATUS_DOWN;
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

int
bcm_dnxf_port_init(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_port_init
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      _SHR_E_NONE - success (or already initialized)
 *      _SHR_E_INTERNAL- failed to write PTABLE entries
 *      _SHR_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      By default ports come up enabled. They can be made to come up disabled
 *      at startup by a compile-time application policy flag in your Make.local
 *      PTABLE initialized.
 */

int
dnxf_port_init(int unit)
{
    int                 rc, port_enable ;
    pbmp_t              okay_ports, pbmp_disabled, not_pbmp_disabled;
    bcm_port_t          p;
    char                pfmtok[SOC_PBMP_FMT_LEN],
                        pfmtall[SOC_PBMP_FMT_LEN];
    bcm_port_resource_t port_resource;
    int tx_pam4_precoder;
    int lp_tx_precoder;
    SHR_FUNC_INIT_VARS(unit);

    /*Clear PRBS mode bitmap*/
    SOC_PBMP_CLEAR(_bcm_dnxf_port_prbs_mac_mode[unit]);

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_init,(unit));
    SHR_IF_ERR_EXIT(rc);

    /* Probe for ports */
    rc = bcm_dnxf_port_probe(unit, SOC_CONTROL(unit)->info.sfi.bitmap , &okay_ports);   
    SHR_IF_ERR_EXIT_WITH_LOG(rc, "Failed port probe: %s%s%s", bcm_errmsg(rc), EMPTY, EMPTY);
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "unit %d: Probed ports okay: %s of %s\n"), unit, SOC_PBMP_FMT(okay_ports, pfmtok),SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmtall)));

    /* Probe and initialize for ports that were OK */
    if (!SOC_WARM_BOOT(unit)) {
        PBMP_ITER(okay_ports, p) {
       
#ifdef BCM_PORT_DEFAULT_DISABLE
            port_enable = FALSE;
#else
            port_enable = TRUE;
#endif  /* BCM_PORT_DEFAULT_DISABLE */

            if (dnxf_data_port.static_add.port_info_get(unit, p)->speed == DNXC_PORT_INVALID_SPEED)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: speed must be set for an active port\n", p);
            }
            else if (dnxf_data_port.static_add.port_info_get(unit, p)->speed == -1)
            {
                /* If speed value for a given port is set to be -1 then port is disabled by default */
                port_enable = FALSE;
            }
            else
            { /* resource set only in init sequence and only if port speed is defined */
                bcm_port_resource_t_init(&port_resource);

                /* get SoC properties values - speed, encoding, CL72 */
                port_resource.speed = dnxf_data_port.static_add.port_info_get(unit, p)->speed;
                port_resource.fec_type = dnxf_data_port.static_add.port_info_get(unit, p)->fec_type;
                port_resource.link_training = dnxf_data_port.static_add.port_info_get(unit, p)->link_training;
                port_resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

                /* get default recommended encoding, CL72, lane_config based on what we got from SoC properties above (fill values that were not set by SoC properties) */
                SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_default_get(unit, p, 0, &port_resource));

                /* override phy lane config bits with values from SoC property */
                SHR_IF_ERR_EXIT(_bcm_dnxf_port_resource_lane_config_soc_properties_get(unit, p, &port_resource));

                SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_set(unit, p, &port_resource));

                /* set precoder according to SoC property */
                tx_pam4_precoder = dnxf_data_port.static_add.port_info_get(unit, p)->tx_pam4_precoder;

                if (port_resource.link_training == 0)
                {
                    SHR_IF_ERR_EXIT(bcm_dnxf_port_phy_control_set(unit, p, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, tx_pam4_precoder));
                }
                else if ((port_resource.link_training == 1) && (tx_pam4_precoder == 1))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: if link training is enabled, enabling tx_pam4_precoder is not allowed.", p);
                }

                /* set lp precoder according to SoC property */
                lp_tx_precoder = dnxf_data_port.static_add.port_info_get(unit, p)->lp_tx_precoder;

                if (port_resource.link_training == 0)
                {
                    SHR_IF_ERR_EXIT(bcm_dnxf_port_phy_control_set(unit, p, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, lp_tx_precoder));
                }
                else if ((port_resource.link_training == 1) && (lp_tx_precoder == 1))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: if link training is enabled, enabling lp_tx_precoder is not allowed.", p);
                }

                /* if link training is disabled - set TX FIR params according to SoC property value */
                if (port_resource.link_training == 0) {
                    rc = _bcm_dnxf_port_phy_tx_params_set(unit, p);
                    SHR_IF_ERR_EXIT(rc);
                }
            }

            if ((rc = bcm_dnxf_port_enable_set(unit, p, port_enable)) < 0) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Port %s: "
                                     "Failed to %s port: %s\n"),
                          SOC_PORT_NAME(unit, p),(port_enable) ? "enable" : "disable" ,bcm_errmsg(rc)));
            }
        }

        /*Mark links as disabled*/
        SOC_PBMP_ASSIGN(pbmp_disabled, SOC_CONTROL(unit)->info.sfi.disabled_bitmap);

        /*Disable relevant FSRDs and FMACs and update soc info for disabled bitmap*/
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_serdes_power_disable,(unit, pbmp_disabled));
        SHR_IF_ERR_EXIT(rc);
    }

    /*change pbmp bitmap*/
    SOC_PBMP_ASSIGN(pbmp_disabled, SOC_CONTROL(unit)->info.sfi.disabled_bitmap);
    SOC_PBMP_NEGATE(not_pbmp_disabled, pbmp_disabled);
    SOC_PBMP_AND(SOC_CONTROL(unit)->info.sfi.bitmap, not_pbmp_disabled);

    SOC_PBMP_ASSIGN(pbmp_disabled, SOC_CONTROL(unit)->info.port.disabled_bitmap);
    SOC_PBMP_NEGATE(not_pbmp_disabled, pbmp_disabled);
    SOC_PBMP_AND(SOC_CONTROL(unit)->info.port.bitmap, not_pbmp_disabled);

    SOC_PBMP_ASSIGN(pbmp_disabled, SOC_CONTROL(unit)->info.all.disabled_bitmap);
    SOC_PBMP_NEGATE(not_pbmp_disabled, pbmp_disabled);
    SOC_PBMP_AND(SOC_CONTROL(unit)->info.all.bitmap, not_pbmp_disabled);

    /*
     * ALDWP configuration
     * No Deinit step
     * Part of the Tuning
     */
    if (!SOC_WARM_BOOT(unit))
    {
        rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_fabric_links_aldwp_init);
        SHR_IF_ERR_EXIT(rc);
    }
exit:
    SHR_FUNC_EXIT; 
}



/*
 * Function:
 *      bcm_dnxf_port_config_get
 * Purpose:
 *      Get port configuration of a device
 * Parameters:
 *      unit -   Unit #.
 *      config - (OUT) Structure returning configuration
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_config_get(int unit, bcm_port_config_t *config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    bcm_port_config_t_init(config);

    BCM_PBMP_CLEAR(config->fe);
    BCM_PBMP_CLEAR(config->ce);
    BCM_PBMP_CLEAR(config->ge);
    BCM_PBMP_CLEAR(config->xe);
    BCM_PBMP_CLEAR(config->e);
    BCM_PBMP_CLEAR(config->nif);
    BCM_PBMP_CLEAR(config->hg);
    BCM_PBMP_CLEAR(config->sci);
    BCM_PBMP_CLEAR(config->spi);
    BCM_PBMP_CLEAR(config->spi_subport);
    BCM_PBMP_CLEAR(config->port);
    BCM_PBMP_CLEAR(config->stack_int);
    BCM_PBMP_CLEAR(config->stack_ext);

    config->sfi         = PBMP_SFI_ALL(unit);
    config->port         = PBMP_PORT_ALL(unit);
    config->cpu         = PBMP_CMIC(unit);
    config->all         = PBMP_ALL(unit);

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_linkscan_get
 * Purpose:
 *      Get the link scan state of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - (OUT) Linkscan value (None, S/W, H/W)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_linkscan_get(int unit, bcm_port_t port, int *linkscan)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(linkscan, _SHR_E_PARAM, "linkscan");

    /*Should not take API lock - bcm_dnxf_linkscan_mode_get will take lock*/
    rv = bcm_dnxf_linkscan_mode_get(unit, port, linkscan);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_linkscan_set
 * Purpose:
 *      Set the linkscan state for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - Linkscan value (None, S/W, H/W)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_linkscan_set(int unit, bcm_port_t port, int linkscan)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    switch(linkscan) {
    case BCM_LINKSCAN_MODE_SW:
    case BCM_LINKSCAN_MODE_HW:
    case BCM_LINKSCAN_MODE_NONE:
        break;
    default:
        SHR_ERR_EXIT(_SHR_E_CONFIG, "wrong linkscan mode\n");
    }

    /*Should not take API lock - bcm_dnxf_linkscan_mode_set will take lock*/

    rv = bcm_dnxf_linkscan_mode_set(unit, port, linkscan);
    SHR_IF_ERR_EXIT(rv);
    

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_loopback_set
 * Purpose:
 *      Setting loopback for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      loopback - one of:
 *              BCM_PORT_LOOPBACK_NONE
 *              BCM_PORT_LOOPBACK_MAC
 *              BCM_PORT_LOOPBACK_PHY
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    portmod_loopback_mode_t dnxf_loopback;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    DNXF_LINK_INPUT_CHECK(unit, port);

    switch(loopback) {
    case BCM_PORT_LOOPBACK_MAC:
        dnxf_loopback = portmodLoopbackMacAsyncFifo;
        break;
    case BCM_PORT_LOOPBACK_PHY:
        dnxf_loopback = portmodLoopbackPhyGloopPMD;
        break;
    case BCM_PORT_LOOPBACK_NONE:
        dnxf_loopback = portmodLoopbackCount;
        break;
    case BCM_PORT_LOOPBACK_PHY_REMOTE:
        dnxf_loopback = portmodLoopbackPhyRloopPMD;
        break;
    case BCM_PORT_LOOPBACK_MAC_REMOTE:
        dnxf_loopback = portmodLoopbackMacRloop;
        break;
    default:
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported loopback %d\n", loopback);
        break;
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_port_loopback_set(unit, port, dnxf_loopback));
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_loopback_get
 * Purpose:
 *      Getting loopback for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      loopback - one of:
 *              BCM_PORT_LOOPBACK_NONE
 *              BCM_PORT_LOOPBACK_MAC
 *              BCM_PORT_LOOPBACK_PHY
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_loopback_get(int unit, bcm_port_t port, int* loopback)
{
    int rc;
    portmod_loopback_mode_t dnxf_loopback;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(loopback, _SHR_E_PARAM, "loopback");

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_loopback_get,(unit, port, &dnxf_loopback));
    SHR_IF_ERR_EXIT(rc);

    switch(dnxf_loopback) {
    case portmodLoopbackMacAsyncFifo:
        *loopback = BCM_PORT_LOOPBACK_MAC;
        break;
    case portmodLoopbackMacRloop:
        *loopback = BCM_PORT_LOOPBACK_MAC_REMOTE;
        break;
    case portmodLoopbackPhyGloopPMD:
        *loopback = BCM_PORT_LOOPBACK_PHY;
        break;
    case portmodLoopbackPhyRloopPMD:
        *loopback = BCM_PORT_LOOPBACK_PHY_REMOTE;
        break;
    default:
        *loopback = BCM_PORT_LOOPBACK_NONE;
        break;
    }
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_fault_get
 * Purpose:
 *      Get link fault type. 
 * Parameters:
 *      unit -  (IN)  BCM device number 
 *      port -  (IN)  Device or logical port number .
 *      flags - (OUT) Flags to indicate fault type 
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_fault_get(int unit, bcm_port_t port, uint32 *flags)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port); 
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    DNXF_UNIT_LOCK_TAKE(unit);  
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_fault_get,(unit, port, flags)));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_selective_get
 * Purpose:
 *      Get requested port parameters
 * Parameters:
 *      unit - switch Unit
 *      port - switch port
 *      info - (IN/OUT) port information structure
 * Returns:
 *      _SHR_E_XXX
 * Notes:
 *      The action_mask field of the info argument is used as an input
 */
int
bcm_dnxf_port_selective_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int                 rc = _SHR_E_NONE;
    uint32              mask;
    bcm_port_resource_t resource;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_ATTR_INTERFACE_MASK is not supported\n");
    }

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        rc = bcm_dnxf_port_enable_get(unit, port, &info->enable);
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK) {
        rc = bcm_dnxf_port_link_status_get (unit, port, &info->linkstatus);
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        rc = bcm_dnxf_port_linkscan_get (unit, port, &info->linkscan);
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        rc = bcm_dnxf_port_loopback_get (unit, port, &info->loopback);
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        rc = bcm_dnxf_port_resource_get(unit, port, &resource);
        SHR_IF_ERR_EXIT(rc);
        info->speed = resource.speed;
    }

    if(mask & BCM_PORT_ATTR_FAULT_MASK) {
        rc = bcm_dnxf_port_fault_get (unit, port, &info->fault);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_selective_set
 * Purpose:
 *      Set requested port parameters
 * Parameters:
 *      unit - switch unit
 *      port - switch port
 *      info - port information structure
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 * Notes:
 *      Does not set spanning tree state.
 */
int
bcm_dnxf_port_selective_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int                 rc = _SHR_E_NONE;
    uint32              mask;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_ATTR_INTERFACE_MASK is not supported\n");
    }

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        rc = bcm_dnxf_port_enable_set(unit, port, info->enable);
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        rc = bcm_dnxf_port_linkscan_set (unit, port, info->linkscan);
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        rc = bcm_dnxf_port_loopback_set (unit, port, info->loopback); 
        SHR_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        rc = bcm_dnxf_port_speed_set(unit, port, info->speed);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_info_get
 * Purpose:
 *      Get all information on the port
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_info_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    bcm_port_info_t_init(info);
    info->action_mask = BCM_PORT_ATTR_ENABLE_MASK | BCM_PORT_ATTR_LINKSTAT_MASK | BCM_PORT_ATTR_LINKSCAN_MASK | BCM_PORT_ATTR_LOOPBACK_MASK | BCM_PORT_ATTR_SPEED_MASK | BCM_PORT_ATTR_FAULT_MASK;
    rc = bcm_dnxf_port_selective_get(unit, port, info);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT; 
}


/*
 * Function:
 *      bcm_dnxf_port_update
 * Purpose:
 *      Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      link - TRUE - process as link up.
 *             FALSE - process as link down.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_update (int unit, bcm_port_t port, int link)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    
exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnxf_port_info_set
 * Purpose:
 *      Set all information on the port
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_info_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    info->action_mask = BCM_PORT_ATTR_ENABLE_MASK | BCM_PORT_ATTR_LINKSCAN_MASK | BCM_PORT_ATTR_LOOPBACK_MASK | BCM_PORT_ATTR_SPEED_MASK;
    rc = bcm_dnxf_port_selective_set(unit, port, info);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_info_save
 * Purpose:
 *      Save the current settings of a port
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_info_save(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    rc = bcm_dnxf_port_info_get(unit, port, info);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_info_restore
 * Purpose:
 *      Restore port settings saved by info_save
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure with info from port_info_save
 * Returns:
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_info_restore(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    rc =  bcm_dnxf_port_info_set(unit, port, info);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT; 
}


/*
 * Function:
 *     bcm_dnxf_port_phy_control_set
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       new value for the configuration
 * Return:
 *     _SHR_E_XXX
 */
int
bcm_dnxf_port_phy_control_set(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32 value)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_phy_control_set,(unit, port, -1, -1, 0, (soc_phy_control_t) type, value));
    DNXF_UNIT_LOCK_RELEASE(unit);

    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *     bcm_dnxf_port_phy_control_get
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       value for the configuration
 * Return:
 *     _SHR_E_XXX
 */
int
bcm_dnxf_port_phy_control_get(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32 *value)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_phy_control_get,(unit, port, -1, -1, 0, (soc_phy_control_t) type, value));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}


/*
 * Function:
 *      bcm_dnxf_port_speed_set
 * Purpose:
 *      Setting the speed for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      speed - Value in megabits/sec (10, 100, etc)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */

int
bcm_dnxf_port_speed_set(int unit, bcm_port_t port, int speed)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port); 
    
    DNXF_UNIT_LOCK_TAKE(unit);  

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_speed_set is no longer supported, please use bcm_port_resource_set to control the speed\n");

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_speed_get
 * Purpose:
 *      Getting the speed of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_speed_get(int unit, bcm_port_t port, int *speed)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port); 
    SHR_NULL_CHECK(speed, _SHR_E_PARAM, "speed");

    DNXF_UNIT_LOCK_TAKE(unit);  

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_speed_get is no longer supported, please use bcm_port_resource_get to get the speed\n");

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_speed_max
 * Purpose:
 *      Get the current operating speed of a port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */
int bcm_dnxf_port_speed_max(
    int unit, 
    bcm_port_t port, 
    int *speed)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    DNXF_LINK_INPUT_CHECK(unit, port); 
    SHR_NULL_CHECK(speed, _SHR_E_PARAM, "speed");

    DNXF_UNIT_LOCK_TAKE(unit);  

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_speed_max,(unit, port, speed));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_interface_set
 * Purpose:
 *      Setting the interface for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      intf - physical interface
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */
int bcm_dnxf_port_interface_set(
    int unit,
    bcm_port_t port,
    bcm_port_if_t intf)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_interface_set is no longer supported, please use bcm_port_resource_set to align interface properties\n");

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_port_interface_get
 * Purpose:
 *      Getting the interface of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      intf - physical interface
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 */
int bcm_dnxf_port_interface_get(
    int unit,
    bcm_port_t port,
    bcm_port_if_t *intf)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(intf, _SHR_E_PARAM, "intf");

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_interface_get is no longer supported, please use bcm_port_resource_get to get interface properties\n");

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnxf_port_phy_get
 * Description:
 *      General PHY register read
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - (OUT) Data that was read
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(phy_data, _SHR_E_PARAM, "phy_data"); 
    
    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        DNXF_LINK_INPUT_CHECK(unit, port);
    }

    DNXF_UNIT_LOCK_TAKE(unit); 

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_phy_get,(unit, port, flags, phy_reg_addr, phy_data));
    SHR_IF_ERR_EXIT(rc);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnxf_port_phy_get: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x, phy_data=0x%08x\n"),
              unit, port, flags, phy_reg_addr, *phy_data));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_phy_set
 * Description:
 *      General PHY register write
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        DNXF_LINK_INPUT_CHECK(unit, port);
    }
    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnxf_port_phy_set: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x\n"),
              unit, port, flags, phy_reg_addr, phy_data));

    DNXF_UNIT_LOCK_TAKE(unit); 

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_phy_set,(unit, port, flags, phy_reg_addr, phy_data));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_phy_modify
 * Description:
 *      General PHY register modify
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - Logical OR of one or more of the following flags:
 *              BCM_PORT_PHY_INTERNAL
 *                      Address internal SERDES PHY for port
 *              BCM_PORT_PHY_NOMAP
 *                      Instead of mapping port to PHY MDIO address,
 *                      treat port parameter as actual PHY MDIO address.
 *              BCM_PORT_PHY_CLAUSE45
 *                      Assume Clause 45 device instead of Clause 22
 *      phy_addr - PHY internal register address
 *      phy_data - Data to write
 *      phy_mask - Bits to modify using phy_data
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_phy_modify(int unit, bcm_port_t port, uint32 flags,
                        uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask)
{
    int    rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        DNXF_LINK_INPUT_CHECK(unit, port);
    }


    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dnxf_port_phy_modify: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x phy_mask=0x%08x\n"),
              unit, port, flags, phy_reg_addr, phy_data, phy_mask));

    DNXF_UNIT_LOCK_TAKE(unit); 

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_phy_modify,(unit, port, flags, phy_reg_addr, phy_data, phy_mask));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_rate_egress_pps_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      pps - Rate in packet per second. A value of zero disables rate limiting (full speed).
 *      burst - Maximum burst size .
 * Returns:
 *      _SHR_E_XXX
 */

int 
bcm_dnxf_port_rate_egress_pps_set(
    int unit, 
    bcm_port_t port, 
    uint32 pps, 
    uint32 burst)
{
    uint32 core_clock_speed, nof_tiks/*packets per tiks*/;
    uint64 nof_tiks64, pps64;
    int rc;

    SHR_FUNC_INIT_VARS(unit);

    BCM_DNXF_PORT_MAC_CELL_SHAPPER_SUPPORT_CHECK(unit);

    /*verify*/
    DNXF_LINK_INPUT_CHECK(unit, port);

    if ( (pps == 0 && burst != 0) ||
         (burst == 0 && pps != 0) )
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Both pps and burst must equal 0 in order to disable the shaper\n");
    }

    /*calculate packets per tiks*/
    core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit) * 1000 /*Hz*/;
   /*nof tiks = burst * core_clock / pps */
    COMPILER_64_SET(nof_tiks64, 0, burst);
    COMPILER_64_UMUL_32(nof_tiks64, core_clock_speed); /*packets per tiks to packets per time*/

    nof_tiks = 0;
    /*pps == 0 --> disables rate limiting*/
    if (pps != 0) {
        COMPILER_64_SET(pps64, 0, pps);
        COMPILER_64_UDIV_64(nof_tiks64, pps64);
        nof_tiks = COMPILER_64_LO(nof_tiks64);
    }
    
    /*config shaper*/
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_rate_egress_ppt_set,(unit, port, burst, nof_tiks));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT; 
}


/*
 * Function:
 *      bcm_dnxf_port_rate_egress_pps_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      pps (out)- Rate in packet per second. A value of zero disables rate limiting (full speed).
 *      burst(out) - Maximum burst size.
 * Returns:
 *      _SHR_E_XXX
 */
int 
bcm_dnxf_port_rate_egress_pps_get(
    int unit, 
    bcm_port_t port, 
    uint32 *pps, 
    uint32 *burst)
{
    uint32 core_clock_speed, nof_tiks/*packets per tiks*/;
    int rc;
    uint64 pps64, nof_tiks64;
    SHR_FUNC_INIT_VARS(unit);

    BCM_DNXF_PORT_MAC_CELL_SHAPPER_SUPPORT_CHECK(unit);

    /*verify*/
    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(pps, _SHR_E_PARAM, "pps");
    SHR_NULL_CHECK(burst, _SHR_E_PARAM, "burst");

    /*get shaper info*/
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_rate_egress_ppt_get,(unit, port, burst, &nof_tiks));
    SHR_IF_ERR_EXIT(rc);

    if (nof_tiks == 0) {
        *pps = 0;
        SHR_EXIT();
    }

    /*calculate packets per sec*/
    /*pps = burst * core_clock / nof_tiks */
    core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);
    COMPILER_64_SET(pps64, 0, *burst);
    COMPILER_64_UMUL_32(pps64, core_clock_speed); /*packets per tiks to packets per time*/
    COMPILER_64_UMUL_32(pps64, 1000); /*KHz to Hz*/

    COMPILER_64_SET(nof_tiks64, 0, nof_tiks);
    COMPILER_64_UDIV_64(pps64, nof_tiks64);

    *pps = COMPILER_64_LO(pps64);

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      bcm_dnxf_port_lane_to_serdes_map_set
 * Description:
 *      set lane map (swap) configuration for all lanes in SW state database
 * Parameters:
 *      unit - Device number
 *      flags - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE flag is supposed to be set
 *      map size - Size of serdes_map array (should be equal to nof lanes of the unit)
 *      serdes_map- An array holding the lane mapping configuration: serdes_map[lane] = {serdes rx, serdes tx}
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_lane_to_serdes_map_set(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t *serdes_map)
{
    int rc, i;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(serdes_map, _SHR_E_PARAM, "serdes_map");

    if (!(flags &  BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flags, need to set BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE\n");
    }

    if (map_size != dnxf_data_port.general.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid map_size parameter\n");
    }

    for (i = 0; i < map_size; ++i)
    {
        lane2serdes[i].rx_id = serdes_map[i].serdes_rx_id;
        lane2serdes[i].tx_id = serdes_map[i].serdes_tx_id;
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_lane_map_db_map_set,(unit, map_size, lane2serdes, 0));
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_port_lane_to_serdes_map_get
 * Description:
 *      get lane map (swap) configuration for all lanes from SW state database
 * Parameters:
 *      unit - Device number
 *      flags - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE flag is supposed to be set
 *      map size - Size of serdes_map array (should be equal to nof lanes of the unit)
 *      serdes_map- An array holding the lane mapping configuration: serdes_map[lane] = {serdes rx, serdes tx}
 * Returns:
 *      _SHR_E_XXX
 */
int
bcm_dnxf_port_lane_to_serdes_map_get(
    int unit, 
    int flags, 
    int map_size, 
    bcm_port_lane_to_serdes_map_t *serdes_map)
{
    int rc, i;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(serdes_map, _SHR_E_PARAM, "serdes_map");

    if (!(flags &  BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flags, need to set BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE\n");
    }

    if (map_size != dnxf_data_port.general.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid map_size parameter\n");
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_lane_map_db_map_get,(unit, map_size, lane2serdes));
    SHR_IF_ERR_EXIT(rc);

    for (i = 0; i < map_size; ++i)
    {
        serdes_map[i].serdes_rx_id = lane2serdes[i].rx_id;
        serdes_map[i].serdes_tx_id = lane2serdes[i].tx_id;
    }

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_port_resource_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t *resource)
{
    bcm_port_resource_t resource_copy;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API\n", port);
    }

    resource_copy = *resource;
    resource_copy.port = port;
    SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_multi_set(unit, 1, &resource_copy));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_port_resource_multi_set(
    int unit, 
    int nport, 
    bcm_port_resource_t *resource)
{
    int rc;
    int resource_index, port_index, enable, pm_id;
    bcm_gport_t port;
    /* we assume that pm_ids of fabric ports of an FE device start from 0 and are sequential. in case of ramon pm_ids are expected to be 0 - 23 -> will allocate 24 entries for pm_config */
    _bcm_dnxf_port_resource_multi_pm_config_t* pm_config = NULL;
    uint32 pm_config_nof_bytes;
    portmod_speed_config_t speed_config_list[DNXC_PORT_FABRIC_LANES_PER_CORE];
    int speed_config_index;
    portmod_pbmp_t pbmp;
    portmod_pm_vco_setting_t vco_setting;
    portmod_vco_type_t vco[DNXF_DATA_MAX_PORT_PLL_NOF_PLLS_PER_PM];
    int is_init_sequence = 0;
    int speed_for_bypass_port[DNXC_PORT_FABRIC_LANES_PER_CORE] = {0};
    SHR_FUNC_INIT_VARS(unit);

    /* validation */
    if (nport <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nport %d is invalid\n", nport);
    }
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        DNXF_LINK_INPUT_CHECK(unit, resource[resource_index].port);
    }

    /* basic validation that given resource configurations are legal per port */
    for(resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_resource_validate(unit, &resource[resource_index]));
    }

    /*
     * Validate PHY loopback is not set on fabric ports
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        int loopback;
        SHR_IF_ERR_EXIT(bcm_dnxf_port_loopback_get(unit, resource[resource_index].port, &loopback));

        if (loopback == BCM_PORT_LOOPBACK_PHY)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Can't configure resource while PHY loopback is set on port %d.",
                         resource[resource_index].port);
        }
    }

     /* pm_config array allocation and initialization */
     pm_config_nof_bytes = sizeof(_bcm_dnxf_port_resource_multi_pm_config_t)*DNXF_DATA_MAX_PORT_GENERAL_NOF_PMS;
     SHR_ALLOC(pm_config, pm_config_nof_bytes, "pm_config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
     sal_memset(pm_config, 0, pm_config_nof_bytes);

     for(pm_id = 0; pm_id < DNXF_DATA_MAX_PORT_GENERAL_NOF_PMS; ++pm_id)
     {
         for(port_index = 0; port_index < DNXC_PORT_FABRIC_LANES_PER_CORE; ++port_index)
         {
             SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &pm_config[pm_id].port_speed_config[port_index]));
             SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &speed_config_list[port_index]));
         }
         pm_config[pm_id].tvco = portmodVCOInvalid;
         pm_config[pm_id].ovco = portmodVCOInvalid;
     }

     /* get if init sequence */
     rc = bcm_dnxf_init_check(unit);
     if (rc == _SHR_E_UNIT) /* init*/
     {
         is_init_sequence = 1;
     }
     else if (rc == _SHR_E_NONE)
     {
         is_init_sequence = 0;
     }
     else {
         SHR_IF_ERR_EXIT(rc);
     }

     /** 1. divide the given ports to PMs, save enable state of each port, transform resource input into portmod struct format */
     for(resource_index = 0; resource_index < nport; ++resource_index)
     {
         /* get pm id of the port */
         SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, resource[resource_index].port, &pm_id));
         if (pm_id < 0 || pm_id >= DNXF_DATA_MAX_PORT_GENERAL_NOF_PMS)
         {
             SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d: pm_id %d is not between 0 and %d\n", resource[resource_index].port, pm_id, DNXF_DATA_MAX_PORT_GENERAL_NOF_PMS-1);
         }

         port_index = BCM_DNXF_PORT_INDEX_IN_PM_GET(resource[resource_index].port);

         /* make sure a port configuration isn't given twice */
         if (pm_config[pm_id].ports_to_set[port_index] == 1)
         {
             SHR_ERR_EXIT(_SHR_E_PARAM, "port %d configuration is given twice\n", resource[resource_index].port);
         }
         /* mark the port in pm_config of the relevant PM */
         pm_config[pm_id].ports_to_set[port_index] = 1;

         /* store enable state of the port */
         SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, resource[resource_index].port, &enable));
         pm_config[pm_id].enable_state[port_index] = enable;

         /* increase nof ports in current PM */
         ++(pm_config[pm_id].nof_ports);

         /* store relevant fields of bcm_port_resource_t struct in portmod_speed_config_t struct */
         pm_config[pm_id].port_speed_config[port_index].speed = resource[resource_index].speed;
         pm_config[pm_id].port_speed_config[port_index].fec = (portmod_fec_t)resource[resource_index].fec_type;
         pm_config[pm_id].port_speed_config[port_index].link_training = resource[resource_index].link_training;
         pm_config[pm_id].port_speed_config[port_index].lane_config = resource[resource_index].phy_lane_config;

         if (DNXC_PORT_PHY_SPEED_IS_PAM4(pm_config[pm_id].port_speed_config[port_index].speed))
         {
             PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_SET(pm_config[pm_id].port_speed_config[port_index].lane_config);
             PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(pm_config[pm_id].port_speed_config[port_index].lane_config);
         }
         else
         {
             PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(pm_config[pm_id].port_speed_config[port_index].lane_config);
             PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_SET(pm_config[pm_id].port_speed_config[port_index].lane_config);
         }
     }

     /** 2. for each PM: portmod_pm_speed_config_validate - decide if TVCO and/or OVCO need to be changed in this PM, based on given ports speeds */
     for (pm_id = 0; pm_id < DNXF_DATA_MAX_PORT_GENERAL_NOF_PMS; ++pm_id)
     {
         /* no ports to configure in current PM */
         if (pm_config[pm_id].nof_ports == 0)
         {
             continue;
         }

         PORTMOD_PBMP_CLEAR(pbmp);
         speed_config_index = 0;
         for(port_index = 0; port_index < DNXC_PORT_FABRIC_LANES_PER_CORE; ++port_index)
         {
             if (pm_config[pm_id].ports_to_set[port_index] == 1)
             {
                 port = BCM_DNXF_PORT_GET(pm_id, port_index);
                 PORTMOD_PBMP_PORT_ADD(pbmp, port);

                 speed_config_list[speed_config_index] = pm_config[pm_id].port_speed_config[port_index];
                 ++speed_config_index;
             }
         }

         SHR_IF_ERR_EXIT(portmod_pm_vco_setting_t_init(unit, &vco_setting));
         vco_setting.num_speeds = pm_config[pm_id].nof_ports;
         vco_setting.speed_config_list = speed_config_list;
         vco_setting.speed_for_pcs_bypass_port = speed_for_bypass_port;

         SHR_IF_ERR_EXIT(portmod_pm_speed_config_validate(unit, pm_id, &pbmp, PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_DEFAULT, &vco_setting));

         /* store result of portmod_pm_speed_config_validate */
         pm_config[pm_id].tvco = vco_setting.tvco;
         pm_config[pm_id].ovco = vco_setting.ovco;
         pm_config[pm_id].is_tvco_new = vco_setting.is_tvco_new;
         pm_config[pm_id].is_ovco_new = vco_setting.is_ovco_new;
     }

     /** 3. for each PM: reconfigure TVCO and/or OVCO if needed and set speed_config of all ports in the PM
      *  done under disabled port */
     for (pm_id = 0; pm_id < DNXF_DATA_MAX_PORT_GENERAL_NOF_PMS; ++pm_id)
     {
         /* no ports to configure in current PM */
         if (pm_config[pm_id].nof_ports == 0)
         {
             continue;
         }

         /* disable all relevant ports that are enabled */
         for (port_index = 0; port_index < DNXC_PORT_FABRIC_LANES_PER_CORE; ++port_index)
         {
             if (pm_config[pm_id].enable_state[port_index] == 1)
             {
                 port = BCM_DNXF_PORT_GET(pm_id, port_index);
                 SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0));
             }
         }

         if (pm_config[pm_id].is_tvco_new || pm_config[pm_id].is_ovco_new)
         {
             vco[0] = pm_config[pm_id].tvco;
             vco[1] = pm_config[pm_id].ovco;
             SHR_IF_ERR_EXIT(portmod_pm_vco_reconfig(unit, pm_id, vco));
         }

         for(port_index = 0; port_index < DNXC_PORT_FABRIC_LANES_PER_CORE; ++port_index)
         {
             if (pm_config[pm_id].ports_to_set[port_index] == 1)
             {
                 portmod_speed_config_t prev_speed_config;
                 uint32 lp_prec;

                 port = BCM_DNXF_PORT_GET(pm_id, port_index);
                 /* lp precoder bit in phy_lane_config bitmap can be set only via bcm_port_phy_control API.
                  * in resource API, leave HW value of this bit as is, don't change it no matter what is passed to the function */
                 SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &prev_speed_config));
                 lp_prec = PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_GET(prev_speed_config.lane_config);
                 if (lp_prec)
                 {
                     PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_SET(pm_config[pm_id].port_speed_config[port_index].lane_config);
                 }
                 else
                 {
                     PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(pm_config[pm_id].port_speed_config[port_index].lane_config);
                 }

                 /* if previous FEC and speed are the same as current FEC and speed - no need to change FEC and FEC-related configurations */
                 if (!is_init_sequence && (prev_speed_config.fec == pm_config[pm_id].port_speed_config[port_index].fec) &&
                         (prev_speed_config.speed == pm_config[pm_id].port_speed_config[port_index].speed))
                 {
                     PORTMOD_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_SET(&(pm_config[pm_id].port_speed_config[port_index]));
                 }

                 SHR_IF_ERR_EXIT(portmod_port_speed_config_set(unit, port, &(pm_config[pm_id].port_speed_config[port_index])));
             }
         }

         /* re-enable all relevant ports that were initially enabled */
         for (port_index = 0; port_index < DNXC_PORT_FABRIC_LANES_PER_CORE; ++port_index)
         {
             if (pm_config[pm_id].enable_state[port_index] == 1)
             {
                 port = BCM_DNXF_PORT_GET(pm_id, port_index);
                 SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 1));
             }
         }
     }

exit:
    SHR_FREE(pm_config);
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_port_resource_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t *resource)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, resource));

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_port_resource_default_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    bcm_port_resource_t *resource)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_resource_default_get(unit, port, flags, resource));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_port_phy_tx_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_tx_t* tx)
{
    int rc;
    bcm_port_resource_t resource;
    int is_init_sequence = 0;
    int orig_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, port, &resource));

    if (resource.link_training == 1)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: setting TX FIR parameters when link training is enabled is not allowed. First disable link training and then set TX FIR parameters.", port);
    }

    /* check if init sequence */
    rc = bcm_dnxf_init_check(unit);
    if (rc == _SHR_E_UNIT) /* init*/
    {
        is_init_sequence = 1;
    }
    else if (rc == _SHR_E_NONE)
    {
        is_init_sequence = 0;
    }
    else {
        SHR_IF_ERR_EXIT(rc);
    }

    /*
     * disable port if not in init sequence. In init seqence port shouldn't be enabled yet.
     */
    if (!is_init_sequence)
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_port_enable_get(unit, port, &orig_enabled));
        if (orig_enabled)
        {
            SHR_IF_ERR_EXIT(bcm_dnxf_port_enable_set(unit, port, 0));
        }
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_set(unit, port, -1, -1, 0, tx));

    /*
     * in init sequence port shouldn't be enabled yet.
     */
    if (!is_init_sequence && orig_enabled)
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_port_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_port_phy_tx_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_phy_tx_t* tx)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_get(unit, port, -1, -1, 0, tx));

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_port_resource_speed_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_resource_speed_get API is not supported. Use bcm_port_resource_get API instead\n");

exit:
    SHR_FUNC_EXIT;
}


int 
bcm_dnxf_port_resource_speed_set(
    int unit, 
    bcm_gport_t port, 
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_resource_speed_set API is not supported. Use bcm_port_resource_set API instead\n");

exit:
    SHR_FUNC_EXIT;
}


int 
bcm_dnxf_port_resource_speed_multi_set(
    int unit, 
    int nport, 
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_resource_speed_multi_set API is not supported. Use bcm_port_resource_multi_set API instead\n");

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
int
dnxf_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    int fabric_mac_bus_size = 0, counter_disabled = 0, clock_speed_int = 0, clock_speed_remainder = 0;
    uint64 rate_start, rate_end;
    uint32 rate;
    int override_gtimer_config = FALSE, rv;
    bcm_stat_val_t counter_type;
    bcm_port_resource_t resource;
    soc_dnxc_port_stat_gtimer_config_t gtimer_default, gtimer_config;

    SHR_FUNC_INIT_VARS(unit);

    phy_measure->lane = port;
    /*
     * Get the default value
     */
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_stat_gtimer_config_get,(unit, port, &gtimer_default)));
    /*
     * Reset Gtimer
     */
    gtimer_config.enable = 0;
    gtimer_config.nof_periods = DNXC_PORT_STAT_GTIMER_NOF_CYCLES;
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_stat_gtimer_config_set,(unit, port, &gtimer_config)));
    override_gtimer_config = TRUE;
    /*
     * clear stats
     */
    rv = bcm_dnxf_stat_sync(unit);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_DISABLED);
    /*
     * if counter thread is disabled, no need to sync
     */
    if (rv == _SHR_E_DISABLED)
    {
        counter_disabled = 1;
    }
    counter_type = is_rx ? snmpBcmRxAsynFifoRate : snmpBcmTxAsynFifoRate;
    SHR_IF_ERR_EXIT(bcm_dnxf_stat_get(unit, port, counter_type, &rate_start));
    
    /*
     * start gtimer stats
     */
    gtimer_config.enable = 1;
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_stat_gtimer_config_set,(unit, port, &gtimer_config)));
    sal_usleep(10);
    /*
     * getting rate
     */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_dnxf_stat_sync(unit), _SHR_E_DISABLED);
    SHR_IF_ERR_EXIT(bcm_dnxf_stat_get(unit, port, counter_type, &rate_end));
    /*
     * If counter thread is enabled, subtract collected rate_end from
     * collected rate_start.
     * If counter thread is disabled, rate_start clears counter, and therefore rate_end is correct rate 
     */
    if (!counter_disabled)
    {
        COMPILER_64_SUB_64(rate_end, rate_start);
    }
    rate = COMPILER_64_LO(rate_end);
    
    SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, port, &resource));
    /*
     * Calculate the serdes rate
     */
    fabric_mac_bus_size = dnxf_data_port.general.fmac_bus_size_get(unit, resource.fec_type)->size;
    clock_speed_int = 1000000 / dnxf_data_port.general.fmac_clock_khz_get(unit);
    clock_speed_remainder = (1000000 / (dnxf_data_port.general.fmac_clock_khz_get(unit) / 1000)) % 1000;

    if (((DNXC_PORT_STAT_GTIMER_NOF_CYCLES / 1000) * (clock_speed_int * 1000 + (clock_speed_remainder % 1000))) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bad calculation value! \n");
    }
    
    phy_measure->serdes_freq_int =
        (rate * fabric_mac_bus_size * 1000) / (DNXC_PORT_STAT_GTIMER_NOF_CYCLES *
                                               (clock_speed_int * 1000 + (clock_speed_remainder % 1000)));
    phy_measure->serdes_freq_remainder =
        ((rate * fabric_mac_bus_size * 1000) /
         ((DNXC_PORT_STAT_GTIMER_NOF_CYCLES / 1000) * (clock_speed_int * 1000 + (clock_speed_remainder % 1000)))) % 1000;
exit:
    /*
     * Restore original Gtimer configuration
     */
    if (override_gtimer_config)
    {
        if((_func_rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_stat_gtimer_config_set,(unit, port, &gtimer_default))) < 0)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error in mbcm_dnxf_port_stat_gtimer_config_set: Failed to recover the default Gtimer configuration!\n")));
        }
    }
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
int
bcm_dnxf_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    bcm_port_ber_proj_params_t *ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t *errcnt_array,
    int *actual_errcnt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_get(unit, port, -1, ber_proj,
                                                    max_errcnt, errcnt_array, actual_errcnt));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
