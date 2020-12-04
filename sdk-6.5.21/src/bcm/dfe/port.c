/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE PORT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT

#include <shared/bsl.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/stat.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/fabric.h>

#include <bcm_int/common/link.h>

#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/dfe/link.h>
#include <bcm_int/dfe/fabric.h>
#include <bcm_int/dfe/dfe_eyescan.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_port.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/phyreg.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dcmn/dcmn_defs.h>

#include <shared/util.h>

#ifdef BCM_88754_A0
#include <soc/dfe/fe1600/fe1600_drv.h>
#endif
/**********************************************************/
/*                  Global                                */
/**********************************************************/
static pbmp_t _bcm_dfe_port_prbs_mac_mode[BCM_MAX_NUM_UNITS];

/**********************************************************/
/*                  Verifications                         */
/**********************************************************/


/**********************************************************/
/*                  Internal functions                    */
/**********************************************************/  

STATIC int
_bcm_port_control_pcs_set(int unit, bcm_port_t port, int value)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_pcs_set,(unit, port, (soc_dcmn_port_pcs_t)value));
    BCMDNX_IF_ERR_EXIT(rc);

    /*ALDWP should be configured at init time for every speed change*/
    rc = MBCM_DFE_DRIVER_CALL(unit ,mbcm_dfe_fabric_links_aldwp_config ,(unit, port));      
    BCMDNX_IF_ERR_EXIT(rc);
exit:   
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dfe_port_deinit(int unit)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_deinit,(unit));
    BCMDNX_IF_ERR_CONT(rc);

    rc = bcm_dfe_eyescan_deinit(unit);
    BCMDNX_IF_ERR_CONT(rc);    

    BCMDNX_FUNC_RETURN;
}

/**********************************************************/
/*                  Implementation                        */
/**********************************************************/  

/*
 * Function:
 *      bcm_dfe_port_control_set
 * Purpose:
 *      set various features at the port level.
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Device or logical port number  
 *      type  - (IN)     Port feature enumerator 
 *      value - (IN)     Value of the bit field in port table
 * Returns:
 *      BCM_E_NONE     No Error  
 *      BCM_E_UNAVAIL  Feature unavailable  
 *      BCM_E_PORT     Invalid Port number specified  
 *      BCM_E_XXX      Error occurred  
 */
 
int 
bcm_dfe_port_control_set(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_t type, 
    int value)
{
    int rc = BCM_E_NONE;
    int prbs_mode = 0;
    BCMDNX_INIT_FUNC_DEFS;
      
    /*validation*/
    DFE_LINK_INPUT_CHECK(unit, port);
    prbs_mode = SOC_PBMP_MEMBER(_bcm_dfe_port_prbs_mac_mode[unit],port) ? 1: 0;
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    switch(type)
    {
        case bcmPortControlPCS:
            rc = _bcm_port_control_pcs_set(unit, port, value);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLinkDownPowerOn:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_power_set,(unit, port, 0, value ? soc_dcmn_port_power_on :  soc_dcmn_port_power_off));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        
        case bcmPortControlStripCRC:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_strip_crc_set,(unit, port, value ? 1 :  0));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        
        case bcmPortControlRxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_rx_enable_set,(unit, port, 0, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlTxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_tx_enable_set,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsPolynomial:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_polynomial_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_tx_enable_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_enable_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxInvertData:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_tx_invert_data_set,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmPortControlPrbsMode:
            if(0 == value) { /*PRBS PHY MODE*/
               SOC_PBMP_PORT_REMOVE(_bcm_dfe_port_prbs_mac_mode[unit], port); 
            } else if (1 == value){ /*PRBS MAC MODE*/
               SOC_PBMP_PORT_ADD(_bcm_dfe_port_prbs_mac_mode[unit], port); 
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid value\n")));
            }
            break;

        case bcmPortControlLowLatencyLLFCEnable:
            rc=MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_low_latency_set,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlFecErrorDetectEnable:
            rc=MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_fec_error_detect_set,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLlfcCellsCongestionIndEnable:
            rc=MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_extract_cig_from_llfc_enable_set,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d\n"), type));
            break;
    }
    
exit:
    DFE_UNIT_LOCK_RELEASE(unit);    
    BCMDNX_FUNC_RETURN;    
}


/*
 * Function:
 *      bcm_dfe_port_control_get
 * Purpose:
 *      Get various features at the port level.
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Device or logical port number  
 *      type  - (IN)     Port feature enumerator 
 *      value - (IN)     Value of the bit field in port table
 * Returns:
 *      BCM_E_NONE     No Error  
 *      BCM_E_UNAVAIL  Feature unavailable  
 *      BCM_E_PORT     Invalid Port number specified  
 *      BCM_E_XXX      Error occurred  
 */
int 
bcm_dfe_port_control_get(
    int unit, 
    bcm_port_t port, 
    bcm_port_control_t type, 
    int *value)
{
    int rc = BCM_E_NONE;
    int prbs_mode;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    DFE_LINK_INPUT_CHECK(unit, port);
    prbs_mode = SOC_PBMP_MEMBER(_bcm_dfe_port_prbs_mac_mode[unit],port) ? 1: 0;
    
    BCMDNX_NULL_CHECK(value);

    DFE_UNIT_LOCK_TAKE(unit);
    
    switch(type)
    {
        case bcmPortControlPCS:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_pcs_get,(unit, port, (soc_dcmn_port_pcs_t*)value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
          
        case bcmPortControlLinkDownPowerOn:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_power_get,(unit, port, (soc_dcmn_port_power_t*)value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        
        case bcmPortControlStripCRC:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_strip_crc_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
            
        case bcmPortControlRxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_rx_enable_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlTxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_tx_enable_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsPolynomial:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_polynomial_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_tx_enable_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_enable_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsRxStatus:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_status_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsTxInvertData:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_tx_invert_data_get,(unit, port, prbs_mode, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlPrbsMode:
            *value = prbs_mode; /*PRBS MAC MODE*/
            break;

        case bcmPortControlLowLatencyLLFCEnable:
            rc=MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_low_latency_get,(unit, port,(int*) value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlFecErrorDetectEnable:
            rc=MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_control_fec_error_detect_get,(unit, port,(int*) value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        case bcmPortControlLlfcCellsCongestionIndEnable:
            rc=MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_extract_cig_from_llfc_enable_get,(unit, port, value));
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d\n"), type));
            break;
    }
    
exit:
    DFE_UNIT_LOCK_RELEASE(unit);    
    BCMDNX_FUNC_RETURN;  
}


/*
 * Function:
 *      bcm_dfe_port_stat_get
 * Purpose:
 *      Get port counter value for specified port statistic type.
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Device or logical port number  
 *      stat  - (IN)     Type of the counter to retrieve
 *      val   - (OUT)    Pointer to a counter value
 * Returns:
 *      BCM_E_XXX 
 */
int
bcm_dfe_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint64 *val)
{
    bcm_stat_val_t stat_e;
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
   
    DFE_LINK_INPUT_CHECK(unit, port);

    BCMDNX_NULL_CHECK(val);

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
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported stat %d\n"), stat));
    }

    rc = bcm_stat_get(unit, port, stat_e, val);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);    
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      port      - (IN)     Device or logical port number  
 *      nstat     - (IN)     Number of elements in stat array
 *      stat_arr  - (IN)     Collected statistics descriptors array
 *      value_arr - (OUT)    Collected counters values 
 * Returns:
 *      BCM_E_XXX 
 */
int
bcm_dfe_port_stat_multi_get(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint64 *value_arr)
{
    int rc = BCM_E_NONE;
    int i;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);

    for(i=0 ; i<nstat ; i++ ) {
        rc = bcm_dfe_port_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN; 
}

int
bcm_dfe_port_stat_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_port_stat_t stat, 
    uint32 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_dfe_port_stat_get32 is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_port_stat_multi_get32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_port_stat_t *stat_arr, 
    uint32 *value_arr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_dfe_port_stat_multi_get32 is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_port_phy_reset_register
 * Description:
 *      Register a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data}
 *                           combination have not been registered before
 */
int
bcm_dfe_port_phy_reset_register(int unit, bcm_port_t port,
                            bcm_port_phy_reset_cb_t callback,
                            void  *user_data)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = soc_phy_reset_register(unit, port, callback, user_data, FALSE);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_phy_reset_unregister
 * Description:
 *      Unregister a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data}
 *                           combination have not been registered before
 */
int
bcm_dfe_port_phy_reset_unregister(int unit, bcm_port_t port,
                              bcm_port_phy_reset_cb_t callback,
                              void  *user_data)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = soc_phy_reset_unregister(unit, port, callback, user_data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_phy_reset
 * Description:
 *      This function performs the low-level PHY reset and is intended to be
 *      called ONLY from callback function registered with
 *      bcm_port_phy_reset_register. Attempting to call it from any other
 *      place will break lots of things.
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_dfe_port_phy_reset(int unit, bcm_port_t port)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);

    rc = soc_phyctrl_reset(unit, port, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_probe
 * Purpose:
 *      Probe the PHY and set up the PHY and MAC for the specified ports.
 *      This is purely a discovery routine and does no configuration.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbmp - Bitmap of ports to probe.
 *      okay_pbmp (OUT) - Ports which were successfully probed.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 * Notes:
 *      If error is returned, the port should not be enabled.
 *      Assumes port_init done.
 *      Note that if a PHY is not present, the port will still probe
 *      successfully.  The default driver will be installed.
 */

 int
bcm_dfe_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp) {

    int rc = BCM_E_NONE;
    int is_init_sequence;
    bcm_port_t port;
    soc_control_t       *soc = SOC_CONTROL(unit);
    uint32 flags;
    int usec;
    BCMDNX_INIT_FUNC_DEFS;


    flags = soc->counter_flags;
    usec = soc->counter_interval;

    rc = soc_counter_stop(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    DFE_UNIT_LOCK_TAKE(unit);

    BCMDNX_NULL_CHECK(okay_pbmp);
    BCM_PBMP_CLEAR(*okay_pbmp);

    rc = bcm_dfe_init_check(unit);
    if (rc == BCM_E_UNIT) /* init*/
    {
        is_init_sequence = 1;
    }
    else if (rc == BCM_E_NONE)
    {
        is_init_sequence = 0;
    }
    else {
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /*Initial port speed max*/
    BCM_PBMP_ITER(pbmp, port) {
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_speed_max,(unit, port, &(SOC_INFO(unit).port_speed_max[port])));
        BCMDNX_IF_ERR_EXIT(rc);
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_probe, (unit, pbmp, okay_pbmp, is_init_sequence));
    BCMDNX_IF_ERR_EXIT(rc);


    if (is_init_sequence /*init*/)
    {
        /*Do nothing*/
    } else
    {

        /* ALDWP should be configured at init time for every speed change*/
        /*Avoiding ALDWP config at init time*/
        rc = MBCM_DFE_DRIVER_CALL(unit ,mbcm_dfe_fabric_links_aldwp_init ,(unit));      
        BCMDNX_IF_ERR_EXIT(rc);

        /*Additinal per port default values*/
        BCM_PBMP_ITER(pbmp, port) {
            BCMDNX_IF_ERR_EXIT(bcm_dfe_fabric_link_control_set(unit, port, bcmFabricLinkPcpEnable, 0x1));
        }

    } 

    if (!is_init_sequence)
    {
        DFE_UNIT_LOCK_RELEASE(unit);

        rc = soc_counter_start(unit, flags, usec, PBMP_SFI_ALL(unit));
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
    
}

/*
 * Function:
 *      bcm_dfe_port_detach
 * Purpose:
 *      Detach a port.  Set phy driver to no connection.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbmp - Bitmap of ports to detach.
 *      detached (OUT) - Bitmap of ports successfully detached.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 * Notes:
 *      If a port to be detached does not appear in detached, its
 *      state is not defined.
 */

int
bcm_dfe_port_detach(int unit, pbmp_t pbmp, pbmp_t *detached)
{
    int         rc = BCM_E_NONE;
    bcm_port_t  port;
    soc_control_t       *soc = SOC_CONTROL(unit);
    uint32 flags;
    int usec;
    int linkscan_paused = 0;
    int counter_paused = 0;
    BCMDNX_INIT_FUNC_DEFS;

    flags = soc->counter_flags;
    usec = soc->counter_interval;

    BCMDNX_NULL_CHECK(detached);
    SOC_PBMP_CLEAR(*detached);

    /*Pause counter thread*/
    rc = soc_counter_stop(unit);
    BCMDNX_IF_ERR_EXIT(rc);
    counter_paused = 1;

    /*Pause linkscan thread*/
    BCM_PBMP_ITER(pbmp, port)
    {
        rc = bcm_dfe_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    BCMDNX_IF_ERR_EXIT(_bcm_linkscan_pause(unit));
    linkscan_paused = 1;

    DFE_UNIT_LOCK_TAKE(unit);
    
    PBMP_ITER(pbmp, port) {
        rc = bcm_dfe_port_enable_set(unit, port,0);
        BCMDNX_IF_ERR_EXIT(rc);

       
        rc = MBCM_DFE_DRIVER_CALL(unit ,mbcm_dfe_port_detach ,(unit, port));
        BCMDNX_IF_ERR_EXIT(rc);


        SOC_PBMP_PORT_ADD(*detached, port);
    }

    DFE_UNIT_LOCK_RELEASE(unit);

    /*Continue linkscan thread*/
    rc = _bcm_linkscan_continue(unit);
    linkscan_paused = 0;
    BCMDNX_IF_ERR_EXIT(rc);

    /*Continuue counter thread*/
    rc = soc_counter_start(unit, flags, usec, PBMP_SFI_ALL(unit));
    counter_paused = 0;
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    if (linkscan_paused)
    {
        _bcm_linkscan_continue(unit);
    }
    if (counter_paused)
    {
        rc = soc_counter_start(unit, flags, usec, PBMP_SFI_ALL(unit));
        if(rc != BCM_E_NONE) {
            cli_out("soc_counter_start failed.\n");
        }
    }
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_enable_set
 * Purpose:
 *      Physically enable/disable the MAC/PHY on this port.
 * Parameters:
 *      unit - unit #.
 *      port - port #.
 *      enable - TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If linkscan is running, it also controls the MAC enable state.
 */

int
bcm_dfe_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int         rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);

    DFE_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_enable_set, (unit, port, enable)); 
    BCMDNX_IF_ERR_EXIT(rc);

    /*ALDWP should be configured at init time for every speed change*/
    /*Avoiding ALDWP config at init time*/
    rc = bcm_dfe_init_check(unit);
    if (rc == BCM_E_UNIT /*init*/)
    {
        /*Do nothing*/
    } else if (rc == BCM_E_NONE)
    {
        rc = MBCM_DFE_DRIVER_CALL(unit ,mbcm_dfe_fabric_links_aldwp_config ,(unit, port));      
        BCMDNX_IF_ERR_EXIT(rc);
    } else {
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /*Enable/Disable link pump when leaky bucket is down for better link utilization*/
    rc = MBCM_DFE_DRIVER_CALL(unit ,mbcm_dfe_port_pump_enable_set ,(unit, port, enable));      
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 

}

/*
 * Function:
 *      bcm_dfe_port_enable_get
 * Purpose:
 *      Gets the enable state as defined by bcm_port_enable_set()
 * Parameters:
 *      unit - unit #.
 *      port - port #.
 *      enable - TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The PHY enable holds the port enable state set by the user.
 *      The MAC enable transitions up and down automatically via linkscan
 *      even if user port enable is always up.
 */

int
bcm_dfe_port_enable_get(int unit, bcm_port_t port, int* enable)
{
    int         rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(enable);

    DFE_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_enable_get, (unit, port, enable)); 
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 

}

/*
 * Function:
 *      bcm_dfe_port_link_status_get
 * Purpose:
 *      Return current Link up/down status, queries linkscan, if unable to
 *      retrieve status queries the PHY.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      status - (OUT) BCM_PORT_LINK_STATUS_DOWN \ BCM_PORT_LINK_STATUS_UP
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_link_status_get(int unit, bcm_port_t port, int *status)
{
    int up;
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(status);

    DFE_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_link_status_get,(unit, port, &up));
    BCMDNX_IF_ERR_EXIT(rc);

    if(up) {
        *status = BCM_PORT_LINK_STATUS_UP;
    } else {
        *status = BCM_PORT_LINK_STATUS_DOWN;
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_init
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_INTERNAL- failed to write PTABLE entries
 *      BCM_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      By default ports come up enabled. They can be made to come up disabled
 *      at startup by a compile-time application policy flag in your Make.local
 *      PTABLE initialized.
 */

int
bcm_dfe_port_init(int unit)
{
    int                 rc, port_enable ;
    pbmp_t              okay_ports, pbmp_disabled, not_pbmp_disabled;
    bcm_port_t          p;
    int                 speed;
    char                pfmtok[SOC_PBMP_FMT_LEN],
                        pfmtall[SOC_PBMP_FMT_LEN];
    BCMDNX_INIT_FUNC_DEFS;

    /*Clear PRBS mode bitmap*/
    SOC_PBMP_CLEAR(_bcm_dfe_port_prbs_mac_mode[unit]);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_init,(unit));
    BCMDNX_IF_ERR_EXIT(rc);

    /* Probe for ports */
    rc = bcm_dfe_port_probe(unit, SOC_CONTROL(unit)->info.sfi.bitmap , &okay_ports);   
    BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed port probe: %s"), bcm_errmsg(rc)));
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

            /*If speed not set then port is disabled by default*/
            speed = soc_property_port_get(unit, p, spn_PORT_INIT_SPEED, -1);
            if(-1 == speed) {
                port_enable = FALSE;
            }

            if ((rc = bcm_dfe_port_enable_set(unit, p, port_enable)) < 0) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Port %s: "
                                     "Failed to %s port: %s\n"),
                          SOC_PORT_NAME(unit, p),(port_enable) ? "enable" : "disable" ,bcm_errmsg(rc)));
            }
        }
        /*disable disabled macs*/
        
        /*first disable links*/
        SOC_PBMP_ASSIGN(pbmp_disabled, SOC_CONTROL(unit)->info.sfi.disabled_bitmap);
        /*Removed the code below (disable links) due to error in init sequence when qrtts are disabled - the links should be disabled nevertheless*/
        /*SOC_PBMP_ITER(pbmp_disabled, p) {
           if ((rc = bcm_dfe_port_enable_set(unit, p, 0)) < 0) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Port %s: "
                                     "Failed to %s port: %s\n"),
                          SOC_PORT_NAME(unit, p),"disable" ,bcm_errmsg(rc)));

            } 
        } */
        /*disable macs and update soc info disabled bitmap*/
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_serdes_power_disable,(unit, pbmp_disabled));
        BCMDNX_IF_ERR_EXIT(rc);
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


    /*Eyescan init*/
    rc = bcm_dfe_eyescan_init(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}



/*
 * Function:
 *      bcm_dfe_port_config_get
 * Purpose:
 *      Get port configuration of a device
 * Parameters:
 *      unit -   Unit #.
 *      config - (OUT) Structure returning configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_dfe_port_config_get(int unit, bcm_port_config_t *config)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(config);

    BCM_PBMP_CLEAR(config->fe);
    BCM_PBMP_CLEAR(config->ce);
    BCM_PBMP_CLEAR(config->ge);
    BCM_PBMP_CLEAR(config->xe);
    BCM_PBMP_CLEAR(config->e);
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
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_linkscan_get
 * Purpose:
 *      Get the link scan state of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - (OUT) Linkscan value (None, S/W, H/W)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_linkscan_get(int unit, bcm_port_t port, int *linkscan)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(linkscan);

    /*Should not take API lock - bcm_dfe_linkscan_mode_get will take lock*/
    rv = bcm_dfe_linkscan_mode_get(unit, port, linkscan);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_linkscan_set
 * Purpose:
 *      Set the linkscan state for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      linkscan - Linkscan value (None, S/W, H/W)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_linkscan_set(int unit, bcm_port_t port, int linkscan)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);

    switch(linkscan) {
    case BCM_LINKSCAN_MODE_SW:
    case BCM_LINKSCAN_MODE_HW:
    case BCM_LINKSCAN_MODE_NONE:
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("wrong linkscan mode")));
    }

    /*Should not take API lock - bcm_dfe_linkscan_mode_set will take lock*/

    rv = bcm_dfe_linkscan_mode_set(unit, port, linkscan);
    BCMDNX_IF_ERR_EXIT(rv);
    

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_loopback_set
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
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    soc_dcmn_loopback_mode_t dfe_loopback;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_LINK_INPUT_CHECK(unit, port);

    switch(loopback) {
    case BCM_PORT_LOOPBACK_MAC:
        dfe_loopback = soc_dcmn_loopback_mode_mac_async_fifo;
        break;
    case BCM_PORT_LOOPBACK_PHY:
        dfe_loopback = soc_dcmn_loopback_mode_phy_gloop;
        break;
    case BCM_PORT_LOOPBACK_NONE:
        dfe_loopback = soc_dcmn_loopback_mode_none;
        break;
    case BCM_PORT_LOOPBACK_PHY_REMOTE:
        dfe_loopback = soc_dcmn_loopback_mode_phy_rloop;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported loopback %d"), loopback)); 
        break;
    }

    DFE_UNIT_LOCK_TAKE(unit);

    BCMDNX_IF_ERR_EXIT(soc_dfe_port_loopback_set(unit, port, dfe_loopback));
    
exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_loopback_get
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
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_loopback_get(int unit, bcm_port_t port, int* loopback)
{
    int rc;
    soc_dcmn_loopback_mode_t dfe_loopback;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(loopback);

    DFE_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_loopback_get,(unit, port, &dfe_loopback));
    BCMDNX_IF_ERR_EXIT(rc);

    switch(dfe_loopback) {
    case soc_dcmn_loopback_mode_mac_async_fifo:
        *loopback = BCM_PORT_LOOPBACK_MAC;
        break;
    case soc_dcmn_loopback_mode_phy_gloop:
        *loopback = BCM_PORT_LOOPBACK_PHY;
        break;
    case soc_dcmn_loopback_mode_phy_rloop:
        *loopback = BCM_PORT_LOOPBACK_PHY_REMOTE;
        break;
    default:
        *loopback = BCM_PORT_LOOPBACK_NONE;
        break;
    }
    
exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_fault_get
 * Purpose:
 *      Get link fault type. 
 * Parameters:
 *      unit -  (IN)  BCM device number 
 *      port -  (IN)  Device or logical port number .
 *      flags - (OUT) Flags to indicate fault type 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_dfe_port_fault_get(int unit, bcm_port_t port, uint32 *flags)
{
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port); 
    BCMDNX_NULL_CHECK(flags);

    DFE_UNIT_LOCK_TAKE(unit);  
    BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_fault_get,(unit, port, flags)));

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_selective_get
 * Purpose:
 *      Get requested port parameters
 * Parameters:
 *      unit - switch Unit
 *      port - switch port
 *      info - (IN/OUT) port information structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The action_mask field of the info argument is used as an input
 */
int
bcm_dfe_port_selective_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int                 rc = BCM_E_NONE;
    uint32              mask;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(info);

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        rc = bcm_dfe_port_enable_get(unit, port, &info->enable);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK) {
        rc = bcm_dfe_port_link_status_get (unit, port, &info->linkstatus);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        rc = bcm_dfe_port_linkscan_get (unit, port, &info->linkscan);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        rc = bcm_dfe_port_loopback_get (unit, port, &info->loopback);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        rc = bcm_dfe_port_speed_get (unit, port, &info->speed);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if(mask & BCM_PORT_ATTR_FAULT_MASK) {
        rc = bcm_dfe_port_fault_get (unit, port, &info->fault);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
       rc = bcm_dfe_port_interface_get(unit, port, &info->interface);
       BCMDNX_IF_ERR_EXIT(rc);
   }

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_selective_set
 * Purpose:
 *      Set requested port parameters
 * Parameters:
 *      unit - switch unit
 *      port - switch port
 *      info - port information structure
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      Does not set spanning tree state.
 */
int
bcm_dfe_port_selective_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int                 rc = BCM_E_NONE;
    uint32              mask;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(info);

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENABLE_MASK) {
        rc = bcm_dfe_port_enable_set(unit, port, info->enable);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK) {
        rc = bcm_dfe_port_linkscan_set (unit, port, info->linkscan);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK) {
        rc = bcm_dfe_port_loopback_set (unit, port, info->loopback); 
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK) {
        rc = bcm_dfe_port_speed_set(unit, port, info->speed);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK) {
        rc = bcm_dfe_port_interface_set(unit, port, info->interface);
        BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_info_get
 * Purpose:
 *      Get all information on the port
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_dfe_port_info_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(info);

    bcm_port_info_t_init(info);
    info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    rc = bcm_dfe_port_selective_get(unit, port, info);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}


/*
 * Function:
 *      bcm_dfe_port_update
 * Purpose:
 *      Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      link - TRUE - process as link up.
 *             FALSE - process as link down.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_update (int unit, bcm_port_t port, int link)
{
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dfe_port_info_set
 * Purpose:
 *      Set all information on the port
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_dfe_port_info_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(info);

    info->action_mask = BCM_PORT_ATTR_ALL_MASK;
    rc = bcm_dfe_port_selective_set(unit, port, info);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_info_save
 * Purpose:
 *      Save the current settings of a port
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure in which to save values
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_dfe_port_info_save(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(info);

    rc = bcm_dfe_port_info_get(unit, port, info);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_info_restore
 * Purpose:
 *      Restore port settings saved by info_save
 * Parameters:
 *      unit - unit #
 *      port - port #
 *      info - Pointer to structure with info from port_info_save
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_dfe_port_info_restore(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(info);

    rc =  bcm_dfe_port_info_set(unit, port, info);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}


/*
 * Function:
 *     bcm_dfe_port_phy_control_set
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       new value for the configuration
 * Return:
 *     BCM_E_XXX
 */
int
bcm_dfe_port_phy_control_set(int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32 value)
{
    int rc, enable;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);

    if(BCM_PORT_PHY_CONTROL_CL72 == type || BCM_PORT_PHY_CONTROL_FIRMWARE_MODE == type) {
        BCMDNX_IF_ERR_EXIT(bcm_dfe_port_enable_get(unit, port, &enable));
        if(enable) {
            BCMDNX_IF_ERR_EXIT(bcm_dfe_port_enable_set(unit, port, 0));
        }
    }

    DFE_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_set,(unit, port, -1, -1, 0, (soc_phy_control_t) type, value));
    DFE_UNIT_LOCK_RELEASE(unit);

    if(BCM_PORT_PHY_CONTROL_CL72 == type || BCM_PORT_PHY_CONTROL_FIRMWARE_MODE == type) {
        if(enable) {
            BCMDNX_IF_ERR_EXIT(bcm_dfe_port_enable_set(unit, port, 1));
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *     bcm_dfe_port_phy_control_get
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       value for the configuration
 * Return:
 *     BCM_E_XXX
 */
int
bcm_dfe_port_phy_control_get(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32 *value)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(value);

    DFE_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_get,(unit, port, -1, -1, 0, (soc_phy_control_t) type, value));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}


/*
 * Function:
 *      bcm_dfe_port_speed_set
 * Purpose:
 *      Setting the speed for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      speed - Value in megabits/sec (10, 100, etc)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_dfe_port_speed_set(int unit, bcm_port_t port, int speed)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port); 
    
    DFE_UNIT_LOCK_TAKE(unit);  

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_speed_set,(unit, port, speed));
    BCMDNX_IF_ERR_EXIT(rc);

    /* 
     * Adjust empty cell size for links connected to repeater 
     */
    rc = _bcm_dfe_fabric_link_repeater_update(unit, port);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_speed_get
 * Purpose:
 *      Getting the speed of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_dfe_port_speed_get(int unit, bcm_port_t port, int *speed)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port); 
    BCMDNX_NULL_CHECK(speed);

    DFE_UNIT_LOCK_TAKE(unit);  

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_speed_get,(unit, port, speed));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_speed_max
 * Purpose:
 *      Get the current operating speed of a port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int bcm_dfe_port_speed_max(
    int unit, 
    bcm_port_t port, 
    int *speed)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_LINK_INPUT_CHECK(unit, port); 
    BCMDNX_NULL_CHECK(speed);

    DFE_UNIT_LOCK_TAKE(unit);  

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_speed_max,(unit, port, speed));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_interface_set
 * Purpose:
 *      Setting the interface for a given port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      intf - physical interface
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int bcm_dfe_port_interface_set(
    int unit,
    bcm_port_t port,
    bcm_port_if_t intf)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);

    DFE_UNIT_LOCK_TAKE(unit);

    rv = (MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_interface_set,(unit, port, intf)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_port_interface_get
 * Purpose:
 *      Getting the interface of the port
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      intf - physical interface
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int bcm_dfe_port_interface_get(
    int unit,
    bcm_port_t port,
    bcm_port_if_t *intf)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_LINK_INPUT_CHECK(unit, port);
    BCMDNX_NULL_CHECK(intf);

    DFE_UNIT_LOCK_TAKE(unit);

    rv = (MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_interface_get,(unit, port, intf)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dfe_port_phy_get
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
 *      BCM_E_XXX
 */
int
bcm_dfe_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(phy_data); 
    
    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        DFE_LINK_INPUT_CHECK(unit, port);
    }

    DFE_UNIT_LOCK_TAKE(unit); 

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_get,(unit, port, flags, phy_reg_addr, phy_data));
    BCMDNX_IF_ERR_EXIT(rc);

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dfe_port_phy_get: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x, phy_data=0x%08x\n"),
              unit, port, flags, phy_reg_addr, *phy_data));

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_phy_set
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
 *      BCM_E_XXX
 */
int
bcm_dfe_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        DFE_LINK_INPUT_CHECK(unit, port);
    }
    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dfe_port_phy_set: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x\n"),
              unit, port, flags, phy_reg_addr, phy_data));

    DFE_UNIT_LOCK_TAKE(unit); 

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_set,(unit, port, flags, phy_reg_addr, phy_data));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_phy_modify
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
 *      BCM_E_XXX
 */
int
bcm_dfe_port_phy_modify(int unit, bcm_port_t port, uint32 flags,
                        uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask)
{
    int    rc;
    BCMDNX_INIT_FUNC_DEFS;

    if (!(flags & BCM_PORT_PHY_NOMAP)) {
        DFE_LINK_INPUT_CHECK(unit, port);
    }


    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "bcm_dfe_port_phy_modify: u=%d p=%d flags=0x%08x "
                         "phy_reg=0x%08x phy_data=0x%08x phy_mask=0x%08x\n"),
              unit, port, flags, phy_reg_addr, phy_data, phy_mask));

    DFE_UNIT_LOCK_TAKE(unit); 

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_modify,(unit, port, flags, phy_reg_addr, phy_data, phy_mask));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_rate_egress_pps_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      pps - Rate in packet per second. A value of zero disables rate limiting (full speed).
 *      burst - Maximum burst size .
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_dfe_port_rate_egress_pps_set(
    int unit, 
    bcm_port_t port, 
    uint32 pps, 
    uint32 burst)
{
    uint32 core_clock_speed, nof_tiks/*packets per tiks*/;
    uint64 nof_tiks64, pps64;
    int rc;

    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    DFE_LINK_INPUT_CHECK(unit, port);
    
    /*calculate packets per tiks*/
    core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed * 1000 /*Hz*/;
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
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_rate_egress_ppt_set,(unit, port, burst, nof_tiks));
    BCMDNX_IF_ERR_EXIT(rc);

    /* 
     * Adjust empty cell size for links connected to repeater 
     */
    rc = _bcm_dfe_fabric_link_repeater_update(unit, port);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_dfe_port_rate_egress_pps_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      pps (out)- Rate in packet per second. A value of zero disables rate limiting (full speed).
 *      burst(out) - Maximum burst size.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_dfe_port_rate_egress_pps_get(
    int unit, 
    bcm_port_t port, 
    uint32 *pps, 
    uint32 *burst)
{
    uint32 core_clock_speed, nof_tiks/*packets per tiks*/;
    int rc;
    uint64 pps64, nof_tiks64;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    DFE_LINK_INPUT_CHECK(unit, port); 
    BCMDNX_NULL_CHECK(pps);
    BCMDNX_NULL_CHECK(burst);

    /*get shaper info*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_rate_egress_ppt_get,(unit, port, burst, &nof_tiks));
    BCMDNX_IF_ERR_EXIT(rc);

    if (*burst == 0 || nof_tiks == 0) {
        *pps = 0;
        *burst = 0;
        BCM_EXIT;
    }

    /*calculate packets per sec*/
    /*pps = burst * core_clock / nof_tiks */
    core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed;
    COMPILER_64_SET(pps64, 0, *burst);
    COMPILER_64_UMUL_32(pps64, core_clock_speed); /*packets per tiks to packets per time*/
    COMPILER_64_UMUL_32(pps64, 1000); /*KHz to Hz*/

    COMPILER_64_SET(nof_tiks64, 0, nof_tiks);
    COMPILER_64_UDIV_64(pps64, nof_tiks64);

    *pps = COMPILER_64_LO(pps64); 

exit:
    BCMDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME

