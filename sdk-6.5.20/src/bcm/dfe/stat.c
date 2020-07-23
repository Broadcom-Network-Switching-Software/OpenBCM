/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE STAT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_STAT
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/stat.h>

#include <soc/dcmn/dcmn_wb.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_stat.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

/*
 * Function:
 *      bcm_dfe_stat_init
 * Purpose:
 *      Initialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 */
int 
bcm_dfe_stat_init(
    int unit)
{
    int rc = BCM_E_NONE;
    int interval;
    uint32 flags;
#if 0
    bcm_port_t port;
#endif
    soc_control_t       *soc = SOC_CONTROL(unit);

    BCMDNX_INIT_FUNC_DEFS;

    rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_stat_init);
    BCMDNX_IF_ERR_EXIT(rc);

    interval = (SAL_BOOT_BCMSIM) ?
               SOC_COUNTER_INTERVAL_SIM_DEFAULT : SOC_COUNTER_INTERVAL_DEFAULT;
    interval = soc_property_get(unit, spn_BCM_STAT_INTERVAL, interval);
    flags = soc_property_get(unit, spn_BCM_STAT_FLAGS, 0);

    rc = soc_counter_start(unit, flags, interval, SOC_CONTROL(unit)->info.sfi.bitmap);
    BCMDNX_IF_ERR_EXIT(rc);

    /*update counters bitmap in case counter thread is not start*/
    if (interval == 0) {
        SOC_PBMP_ASSIGN(soc->counter_pbmp, SOC_CONTROL(unit)->info.sfi.bitmap);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dfe_stat_deinit
 * Purpose:
 *      Internal function  - Deinitialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 */
int 
_bcm_dfe_stat_deinit(
    int unit)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = soc_counter_stop(unit);
    BCMDNX_IF_ERR_CONT(rc);

    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dfe_stat_clear
 * Purpose:
 *      Clear the port-based statistics for the indicated device port
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 */
int 
bcm_dfe_stat_clear(
    int unit, 
    bcm_port_t port)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_LINK_INPUT_CHECK(unit, port);
      
    rc = soc_controlled_counter_clear(unit, port);
    BCMDNX_IF_ERR_EXIT(rc);
 
exit:   
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stat_sync
 * Purpose:
 *      Synchronize software counters with hardware
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      BCM_E_NONE        Success  
 *      BCM_E_INTERNAL    Device access failure  
 *      BCM_E_DISABLED    Unit's counter disabled  
 */
int 
bcm_dfe_stat_sync(
    int unit)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
     
    rc = soc_counter_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:   
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stat_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value 
 * Returns:
 *      BCM_E_NONE       Success.  
 *      BCM_E_PARAM      Illegal parameter.  
 *      BCM_E_BADID      Illegal port number.  
 *      BCM_E_INTERNAL   Device access failure.  
 *      BCM_E_UNAVAIL    Counter/variable is not implemented on this current chip.  
 */
int 
bcm_dfe_stat_get(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint64 *value)
{
    int rc = BCM_E_NONE;
	int counters[SOC_DFE_MAX_NOF_COUNTERS_PER_STAT_TYPE]={0};
	int array_size=0;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_LINK_INPUT_CHECK(unit, port);

    if(SOC_E_NONE != MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_stat_is_supported_type,(unit, port, type))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type %d isn't supported for port %d"), type, port));
    }
    
    BCMDNX_NULL_CHECK(value);
	rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_mapping_stat_get,(unit,port ,counters,&array_size,type,SOC_DFE_MAX_NOF_COUNTERS_PER_STAT_TYPE));
	BCMDNX_IF_ERR_EXIT(rc);
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stat_get,(unit, port, value, counters,array_size));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stat_multi_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      port      - (IN)     Zero-based device or logical port number 
 *      nstat     - (IN)     Number of elements in stat array
 *      stat_arr  - (IN)     Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT)    Collected 64-bit or 32-bit statistics values 
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_dfe_stat_multi_get(
    int unit, 
    bcm_port_t port, 
    int nstat,
    bcm_stat_val_t *stat_arr, 
    uint64 *value_arr)
{
    int rc = BCM_E_NONE;
    int i;
    BCMDNX_INIT_FUNC_DEFS;
   
    BCMDNX_NULL_CHECK(stat_arr);
    BCMDNX_NULL_CHECK(value_arr);

    for(i=0 ; i<nstat ; i++ ) {
        rc = bcm_dfe_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:   
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_stat_get32
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number 
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value 
 * Returns:
 *      BCM_E_NONE       Success.  
 *      BCM_E_PARAM      Illegal parameter.  
 *      BCM_E_BADID      Illegal port number.  
 *      BCM_E_INTERNAL   Device access failure.  
 *      BCM_E_UNAVAIL    Counter/variable is not implemented on this current chip.
 *      BCM_E_FAIL       Counter overflowed
 */
int 
bcm_dfe_stat_get32(
    int unit, 
    bcm_port_t port, 
    bcm_stat_val_t type, 
    uint32 *value)
{
    int rc = BCM_E_NONE;
    uint64 value64;
    uint32 value64_hi;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_stat_get(unit, port, type, &value64);
    BCMDNX_IF_ERR_EXIT(rc);

    COMPILER_64_TO_32_HI(value64_hi, value64);
    if (value64_hi > 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("statictics counter type %d overflow"), type));
    }

    COMPILER_64_TO_32_LO(*value, value64); 

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dfe_stat_multi_get32(
        int unit,
        bcm_port_t port,
        int nstat,
        bcm_stat_val_t *stat_arr,
        uint32 *value_arr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_dfe_stat_multi_get32 is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

