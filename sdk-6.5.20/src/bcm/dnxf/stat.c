/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF STAT
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/stat.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_stat.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>

int
bcm_dnxf_stat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_init
 * Purpose:
 *      Initialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE        Success  
 *      _SHR_E_INTERNAL    Device access failure  
 */
int 
dnxf_stat_init(
    int unit)
{
    int rc = _SHR_E_NONE;
    int interval;
    uint32 flags;
#if 0
    bcm_port_t port;
#endif
    soc_control_t       *soc = SOC_CONTROL(unit);

    SHR_FUNC_INIT_VARS(unit);

    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_stat_init);
    SHR_IF_ERR_EXIT(rc);

    interval = (SAL_BOOT_BCMSIM) ?
               SOC_COUNTER_INTERVAL_SIM_DEFAULT : SOC_COUNTER_INTERVAL_DEFAULT;
    interval = soc_property_get(unit, spn_BCM_STAT_INTERVAL, interval);
    flags = soc_property_get(unit, spn_BCM_STAT_FLAGS, 0);

    rc = soc_counter_start(unit, flags, interval, SOC_CONTROL(unit)->info.sfi.bitmap);
    SHR_IF_ERR_EXIT(rc);

    /*update counters bitmap in case counter thread is not start*/
    if (interval == 0) {
        SOC_PBMP_ASSIGN(soc->counter_pbmp, SOC_CONTROL(unit)->info.sfi.bitmap);
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      _bcm_dnxf_stat_deinit
 * Purpose:
 *      Internal function  - Deinitialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE        Success  
 *      _SHR_E_INTERNAL    Device access failure  
 */
int 
dnxf_stat_deinit(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_counter_stop(unit);
    SHR_IF_ERR_CONT(rc);

    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnxf_stat_clear
 * Purpose:
 *      Clear the port-based statistics for the indicated device port
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 * Returns:
 *      _SHR_E_NONE        Success  
 *      _SHR_E_INTERNAL    Device access failure  
 */
int 
bcm_dnxf_stat_clear(
    int unit, 
    bcm_port_t port)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    
    DNXF_LINK_INPUT_CHECK(unit, port);
      
    rc = soc_controlled_counter_clear(unit, port);
    SHR_IF_ERR_EXIT(rc);
 
exit:   
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_sync
 * Purpose:
 *      Synchronize software counters with hardware
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE        Success  
 *      _SHR_E_INTERNAL    Device access failure  
 *      _SHR_E_DISABLED    Unit's counter disabled  
 */
int 
bcm_dnxf_stat_sync(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
     
    rc = soc_counter_sync(unit);
    SHR_IF_ERR_EXIT(rc);

exit:   
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value 
 * Returns:
 *      _SHR_E_NONE       Success.  
 *      _SHR_E_PARAM      Illegal parameter.  
 *      _SHR_E_BADID      Illegal port number.  
 *      _SHR_E_INTERNAL   Device access failure.  
 *      _SHR_E_UNAVAIL    Counter/variable is not implemented on this current chip.  
 */
int 
bcm_dnxf_stat_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint64 *value)
{
    int rc = _SHR_E_NONE;
	int counters[SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE]={0};
	int array_size=0;
    SHR_FUNC_INIT_VARS(unit);
    
    DNXF_LINK_INPUT_CHECK(unit, port);

    if(_SHR_E_NONE != MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_stat_is_supported_type,(unit, port, type))) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type %d isn't supported for port %d", type, port);
    }
    
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
	rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_mapping_stat_get,(unit,port ,counters,&array_size,type,SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE));
	SHR_IF_ERR_EXIT(rc);
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_get,(unit, port, value, counters,array_size));
    SHR_IF_ERR_EXIT(rc);
    
exit:   
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_multi_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      port      - (IN)     Zero-based device or logical port number 
 *      nstat     - (IN)     Number of elements in stat array
 *      stat_arr  - (IN)     Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT)    Collected 64-bit or 32-bit statistics values 
 * Returns:
 *  _SHR_E_XXX
 */
int 
bcm_dnxf_stat_multi_get(
    int unit, 
    bcm_port_t port, 
    int nstat,
    bcm_stat_val_t *stat_arr, 
    uint64 *value_arr)
{
    int rc = _SHR_E_NONE;
    int i;
    SHR_FUNC_INIT_VARS(unit);
   
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

    for(i=0 ; i<nstat ; i++ ) {
        rc = bcm_dnxf_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rc);
    }

exit:   
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_get32
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value 
 * Returns:
 *      _SHR_E_NONE       Success.  
 *      _SHR_E_PARAM      Illegal parameter.  
 *      _SHR_E_BADID      Illegal port number.  
 *      _SHR_E_INTERNAL   Device access failure.  
 *      _SHR_E_UNAVAIL    Counter/variable is not implemented on this current chip.
 *      _SHR_E_FAIL       Counter overflowed
 */
int 
bcm_dnxf_stat_get32(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 *value)
{
    int rc = _SHR_E_NONE;
    uint64 value64;
    uint32 value64_hi;
    SHR_FUNC_INIT_VARS(unit);

    rc = bcm_dnxf_stat_get(unit, port, type, &value64);
    SHR_IF_ERR_EXIT(rc);

    COMPILER_64_TO_32_HI(value64_hi, value64);
    if (value64_hi > 0) {
        SHR_ERR_EXIT(_SHR_E_FAIL, "statictics counter type %d overflow", type);
    }

    COMPILER_64_TO_32_LO(*value, value64); 

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnxf_stat_multi_get32(
        int unit,
        bcm_port_t port,
        int nstat,
        bcm_stat_val_t *stat_arr,
        uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnxf_stat_multi_get32 is not available");
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

