/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    subport.c
 * Purpose: Manages SUBPORT functions
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/util.h>

#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/subport.h>
#include <bcm/types.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/xgs5.h>

#if defined(BCM_FIREBOLT_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs3.h>
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
#include <bcm_int/esw/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */



/*
 * Subport Function Vector Driver
 */
bcm_esw_subport_drv_t    *bcm_esw_subport_drv[BCM_MAX_NUM_UNITS] = {0};

/*****************************************
* Subport common section - start
 */


/* The common data structures between KT2 and TD2+ Subport-CoE implementation */

/*
* Bitmap of subport group id
* Each Subport group created has unique group id
* across all ports in the device irrespective of
* group type LinkPHY/SubTag
*/

SHR_BITDCL *_bcm_subport_group_bitmap[BCM_MAX_NUM_UNITS] = { 0 };

/* Count of all subport groups created */
int _bcm_subport_group_count[BCM_MAX_NUM_UNITS] = { 0 };

/* Count subport port per subport group*/
int *(_bcm_subport_group_subport_port_count[BCM_MAX_NUM_UNITS]) = { 0 };

sal_mutex_t _bcm_subport_mutex[BCM_MAX_NUM_UNITS] = {NULL};

/* SubTag subport port array */
_bcm_subtag_subport_port_info_t
           *(_bcm_subtag_subport_port_info[BCM_MAX_NUM_UNITS]) = { 0 };

/*********************************
* Subport common  section - end
 */

/*********************************
* General subport section - start
 */

/* General subport port array */
_bcm_general_subport_port_info_t
           *(_bcm_general_subport_port_info[BCM_MAX_NUM_UNITS]) = { 0 };

/* Bitmap of general subport group id */
SHR_BITDCL *_bcm_general_subport_group_bitmap[BCM_MAX_NUM_UNITS] = { 0 };

/* Count of all general subport groups created */
int _bcm_general_subport_group_count[BCM_MAX_NUM_UNITS] = { 0 };

/*********************************
* General subport section - end
 */

/*********************************
* SubTag subport section - start
 */

/* Bitmap of SubTag subport group id */
SHR_BITDCL *_bcm_subtag_group_bitmap[BCM_MAX_NUM_UNITS] = { 0 };

/* Count of all SubTag subport groups created */
int _bcm_subtag_subport_group_count[BCM_MAX_NUM_UNITS] = { 0 };

SHR_BITDCL *_bcm_subtag_vlan_id_bitmap
                   [BCM_MAX_NUM_UNITS][SOC_MAX_NUM_PORTS] = {{ 0 }};

#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT

/* BCM subport port group bitmap */
_bcm_subport_port_group_state_t subport_port_group_state[BCM_MAX_NUM_UNITS];

#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/*********************************
* SubTag subport section - end
 */

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>

/*
 * Function:
 *      _bcm_esw_subport_wb_scache_alloc
 * Purpose:
 *      Allocates scache space for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_subport_wb_scache_alloc(int unit)
{
    int rv = BCM_E_NONE;
    uint8 *scache_ptr;
    int stable_size = 0;
    int alloc_sz = 0;
    soc_scache_handle_t scache_handle;

    BCM_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

#ifdef BCM_HGPROXY_COE_SUPPORT

   if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) {
        rv = bcmi_xgs5_subport_wb_scache_alloc(unit,
                  BCM_SUBPORT_WB_DEFAULT_VERSION, &alloc_sz);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_HGPROXY_COE_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        rv = bcm_kt2_subport_wb_scache_alloc(unit,
                 BCM_SUBPORT_WB_DEFAULT_VERSION, &alloc_sz);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_KATANA2_SUPPORT */

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SUBPORT, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, &scache_ptr,
                                 BCM_SUBPORT_WB_DEFAULT_VERSION, NULL);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_subport_wb_recover
 * Purpose:
 *      Recover Subport info
 * Parameters:
 *      unit - Unit reference
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_subport_wb_recover(int unit)
{
    int rv;
    uint16 ver;
    uint8 *scache_ptr = NULL;
    soc_scache_handle_t scache_handle;

    /* Level 2 WB support is enabled only for linkphy/subtag-coe/
       hgproxy_subtag_coe */
    if (!(soc_feature(unit, soc_feature_linkphy_coe) ||
          soc_feature(unit, soc_feature_subtag_coe)  ||
          soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
          soc_feature(unit, soc_feature_channelized_switching))) {
        return BCM_E_UNAVAIL;
    }

    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SUBPORT, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                0, &scache_ptr,
                                BCM_SUBPORT_WB_DEFAULT_VERSION, &ver);

    if (rv == BCM_E_NONE) { /* Use Level 2 cache */

#ifdef BCM_HGPROXY_COE_SUPPORT
        if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            soc_feature(unit, soc_feature_channelized_switching)) {
            return bcmi_xgs5_subport_wb_recover(unit,
                      BCM_SUBPORT_WB_DEFAULT_VERSION, &scache_ptr);
		}
#endif /* BCM_HGPROXY_COE_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            rv = bcm_kt2_subport_wb_recover(unit, ver, &scache_ptr);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif /* BCM_KATANA2_SUPPORT */
    } else if (rv == SOC_E_NOT_FOUND) {
        /* Use non-Level 2 case for recovery */
        /* and Allocate scache space for further syncs */
        rv = _bcm_esw_subport_wb_scache_alloc(unit);
        BCM_IF_ERROR_RETURN(rv);
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_subport_sync
 * Purpose:
 *      Record rx module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_subport_sync(int unit)
{
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;

	/* Level 2 WB support is enabled only for linkphy/subtag-coe/
	   hgproxy_subtag_coe */
    if (!(soc_feature(unit, soc_feature_linkphy_coe) ||
          soc_feature(unit, soc_feature_subtag_coe)  ||
          soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
          soc_feature(unit, soc_feature_channelized_switching))) {
        return BCM_E_UNAVAIL;
    }

    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SUBPORT, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_SUBPORT_WB_DEFAULT_VERSION, NULL));

#ifdef BCM_HGPROXY_COE_SUPPORT
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        soc_feature(unit, soc_feature_channelized_switching)) {
        return bcmi_xgs5_subport_wb_sync(unit,
                  BCM_SUBPORT_WB_DEFAULT_VERSION, &scache_ptr);
    }
#endif /* BCM_HGPROXY_COE_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        return bcm_kt2_subport_wb_sync(unit,
                  BCM_SUBPORT_WB_DEFAULT_VERSION, &scache_ptr);
    }
#endif /* BCM_KATANA2_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *      bcm_subport_init
 * Purpose:
 *      Initialize the SUBPORT software module
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_init(int unit)
{

    int rv = BCM_E_NONE, l3_enable = 1, subport_init = 0;
#if defined(BCM_WARM_BOOT_SUPPORT)
    int wb_support = 0;
#endif
/* 1. subport is enabled for devices having linkphy_coe/subtag_coe/
 *    hgproxy_subtag_coe/subport_enhanced/subport features
 * 2. WB is enabled for devices having linkphy_coe/subtag_coe/hgproxy_subtag_coe
 *    features
 * 3. L3 is needed for subport initialization in devices having
 *    hgproxy_subtag_coe/subport_enhanced/subport features
 */
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        /* Linkphy/Subtag_Coe doesn't need L3 support */
        rv = bcm_kt2_subport_init(unit);
        subport_init = 1;
#if defined(BCM_WARM_BOOT_SUPPORT)
        wb_support = 1;
#endif
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
    /* l3_enable is set to FALSE if,
     * 1. device won't support L3
     * 2. config variable "l3_enable" is set to FALSE
     * 3. L3 flag is not included in compilation
     */
#if defined(INCLUDE_L3)
    if (!soc_feature(unit, soc_feature_l3) ||
        !soc_property_get(unit, spn_L3_ENABLE, 1)) {
        l3_enable = 0;
    }
#else
        l3_enable = 0;
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        l3_enable) {
#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            SOC_IF_ERROR_RETURN(bcm_apache_subport_coe_init(unit));
        } else
#endif
        if (SOC_IS_TRIDENT2PLUS(unit)) {
            SOC_IF_ERROR_RETURN(bcm_td2plus_subport_coe_init(unit));
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN(bcm_td3_subport_coe_init(unit));
        }
#endif
        subport_init = 1;
#if defined(BCM_WARM_BOOT_SUPPORT)
        wb_support = 1;
#endif
    }
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_subport_enhanced) &&
        l3_enable) {
         rv = bcm_tr2_subport_init(unit);
         subport_init = 1;
    } else
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_subport) &&
        l3_enable) {
        rv = bcm_tr_subport_init(unit);
        subport_init = 1;
    } else
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    {
        if (l3_enable && !subport_init) {
            rv = BCM_E_UNAVAIL;
        }
    }

    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Level 2 WB support is enabled only for linkphy/subtag-coe/
       hgproxy_subtag_coe */
    if (wb_support) {
        if (SOC_WARM_BOOT(unit)) {
            return (_bcm_esw_subport_wb_recover(unit));
        } else {
            return (_bcm_esw_subport_wb_scache_alloc(unit));
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_subport_cleanup
 * Purpose:
 *      Cleanup the SUBPORT software module
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_cleanup(int unit)
{
    if (BCM_ESW_SUBPORT_DRV(unit) != NULL ) {
        if (BCM_ESW_SUBPORT_DRV(unit)->coe_cleanup != NULL) {
            BCM_ESW_SUBPORT_DRV(unit)->coe_cleanup(unit);
        }

        if (BCM_ESW_SUBPORT_DRV(unit)->subport_cleanup != NULL) {
            return BCM_ESW_SUBPORT_DRV(unit)->subport_cleanup(unit);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_subport_group_create
 * Purpose:
 *      Create a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      config - (IN) Subport group config information
 *      group  - (OUT) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_group_create(int unit,
                             bcm_subport_group_config_t *config,
                             bcm_gport_t *group)
{
    if (config == NULL) {
        return (BCM_E_PARAM);
    }
    if (config->flags & BCM_SUBPORT_GROUP_WITH_ID) {
        if (group == NULL) {
            return (BCM_E_PORT);
        }
        if (!BCM_GPORT_IS_SUBPORT_GROUP(*group)) {
            return (BCM_E_PORT);
        }
    }

    if (BCM_ESW_SUBPORT_DRV(unit) != NULL ) {
        if(config->flags & BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_create != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                        coe_group_create(unit, config, group);
            }
        } else if ((config->flags & BCM_SUBPORT_GROUP_TYPE_LINKPHY) &&
                   (soc_feature(unit, soc_feature_linkphy_coe))) {
                       if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_create != NULL) {
                           return BCM_ESW_SUBPORT_DRV(unit)->
                                   coe_group_create(unit, config, group);
                       }
        } else if ((config->flags & BCM_SUBPORT_GROUP_TYPE_GENERAL) &&
                   (soc_feature(unit, soc_feature_general_cascade))) {
                if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_create != NULL) {
                    return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_group_create(unit, config, group);
            }
        } else {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_group_create != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_group_create(unit, config, group);
            }
        }
    }

    return BCM_E_UNAVAIL;
}

#define _TMP_UINT_BUF_COUNT  16

#if defined (BCM_TRX_SUPPORT) && defined(INCLUDE_L3)

STATIC int 
_bcm_esw_subport_port_traverse_cb(int unit, 
                                  bcm_gport_t port, 
                                  bcm_subport_config_t *config,  
                                  void *user_data)
{   
	uint32 *data = (uint32 *)user_data; 
    
    if (config->group == data[0]) {
	    if (data[1] >= (_TMP_UINT_BUF_COUNT - 2)) {
	    	return BCM_E_NONE;
	    }
	    data[1]++;
	    data[data[1]+1] = port;
	}
    
    return BCM_E_NONE;
}

#endif
/*
 * Function:
 *      bcm_subport_group_destroy
 * Purpose:
 *      Destroy a subport group
 * Parameters:
 *      unit  - (IN) Device Number
 *      group - (IN) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_group_destroy(int unit, bcm_gport_t group)
{
#if ((defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) || \
     defined BCM_TRX_SUPPORT || defined (BCM_HGPROXY_COE_SUPPORT)) && \
         defined(INCLUDE_L3))
    uint32 sub_ports[_TMP_UINT_BUF_COUNT];
    uint32 i;
#endif
    
    if (!BCM_GPORT_IS_SUBPORT_GROUP(group)) {
        return (BCM_E_PORT);
    }
    
#if defined(BCM_HGPROXY_COE_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        soc_feature(unit, soc_feature_channelized_switching)) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {

        while (TRUE) {
        	sub_ports[0] = (uint32)group;
        	sub_ports[1] = 0;
	        bcmi_xgs5_subport_coe_port_traverse(unit, 
	            _bcm_esw_subport_port_traverse_cb, sub_ports); 

	        for (i = 0; i < sub_ports[1]; i++) {
	        	bcmi_xgs5_subport_coe_port_delete(unit, 
	        	        (bcm_gport_t)sub_ports[i+2]);
	        }
	        	
	        if (sub_ports[1] != (_TMP_UINT_BUF_COUNT - 2)) {
	        	break;
	        } 	
        } 
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL ) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_destroy != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_group_destroy(unit, group);
            }
        }
    }
#endif /* #if defined(BCM_HGPROXY_COE_SUPPORT) && defined(INCLUDE_L3) */

#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_COE_GPORT_IS_LINKPHY_SUBPORT_GROUP(group)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) ||
        (soc_feature(unit, soc_feature_general_cascade) &&
            _BCM_COE_GPORT_IS_GENERAL_SUBPORT_GROUP(group))) {

        while (TRUE) {
        	sub_ports[0] = (uint32)group;
        	sub_ports[1] = 0;
	        bcm_kt2_subport_port_traverse(unit, 
	            _bcm_esw_subport_port_traverse_cb, sub_ports); 

	        for (i = 0; i < sub_ports[1]; i++) {
	        	bcm_kt2_subport_port_delete(unit, 
	        	        (bcm_gport_t)sub_ports[i+2]);
	        }
	        	
	        if (sub_ports[1] != (_TMP_UINT_BUF_COUNT - 2)) {
	        	break;
	        } 	
        } 
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL ) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_destroy != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_group_destroy(unit, group);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_subport_enhanced)) {
    	while (TRUE) {
        	sub_ports[0] = (uint32)group;
        	sub_ports[1] = 0;
	        bcm_tr2_subport_port_traverse(unit, 
	            _bcm_esw_subport_port_traverse_cb, sub_ports); 

	        for (i = 0; i < sub_ports[1]; i++) {
	        	bcm_tr2_subport_port_delete(unit, 
	        	        (bcm_gport_t)sub_ports[i+2]);
	        }
	        	
	        if (sub_ports[1] != (_TMP_UINT_BUF_COUNT - 2)) {
	        	break;
	        } 	
        }
           
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL ) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_group_destroy != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_group_destroy(unit, group);
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_subport)) {
    	while (TRUE) {
        	sub_ports[0] = (uint32)group;
        	sub_ports[1] = 0;
	        bcm_tr_subport_port_traverse(unit, 
	            _bcm_esw_subport_port_traverse_cb, sub_ports); 

	        for (i = 0; i < sub_ports[1]; i++) {
	        	bcm_tr_subport_port_delete(unit, 
	        	        (bcm_gport_t)sub_ports[i+2]);
	        }
	        	
	        if (sub_ports[1] != (_TMP_UINT_BUF_COUNT - 2)) {
	        	break;
	        } 	
        }
         
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL ) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_group_destroy != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_group_destroy(unit, group);
            }
        }
    }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_subport_group_get
 * Purpose:
 *      Get a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      group  - (IN) GPORT (generic port) identifier
 *      config - (OUT) Subport group config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_group_get(int unit, bcm_gport_t group,
                          bcm_subport_group_config_t *config)
{
    if (!BCM_GPORT_IS_SUBPORT_GROUP(group)) {
        return (BCM_E_PORT);
    }

    if((soc_feature(unit, soc_feature_subtag_coe) ||
         soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
         soc_feature(unit, soc_feature_channelized_switching)) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(group)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                        coe_group_get(unit, group, config);
            }
        }
    } else if ((soc_feature_linkphy_coe) && 
               _BCM_COE_GPORT_IS_LINKPHY_SUBPORT_GROUP(group)){
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                        coe_group_get(unit, group, config);
            }
        }
    } else if (soc_feature(unit, soc_feature_general_cascade) &&
            _BCM_COE_GPORT_IS_GENERAL_SUBPORT_GROUP(group)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                    coe_group_get(unit, group, config);
            }
        }
    } else {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_group_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_group_get(unit, group, config);
            }
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_subport_port_add
 * Purpose:
 *      Add a subport to a subport group
 * Parameters:
 *      unit   - (IN) Device Number
 *      config - (IN) Subport config information
 *      port   - (OUT) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_add(int unit, bcm_subport_config_t *config,
                         bcm_gport_t *port)
{
    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(config->group)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_add != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                        coe_port_add(unit, config, port);
            }
        }
    } else if ((_BCM_COE_GPORT_IS_LINKPHY_SUBPORT_GROUP(config->group)) &&
               (soc_feature(unit, soc_feature_linkphy_coe))){
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_add != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                        coe_port_add(unit, config, port);
            }
        }
    } else if ((_BCM_COE_GPORT_IS_GENERAL_SUBPORT_GROUP(config->group)) &&
              soc_feature(unit, soc_feature_general_cascade)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_add != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                        coe_port_add(unit, config, port);
            }
        }
    } else {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_port_add != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_port_add(unit, config, port);
            }
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_subport_port_delete
 * Purpose:
 *      Delete a subport 
 * Parameters:
 *      unit   - (IN) Device Number
 *      port   - (IN) GPORT (generic port) identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_delete(int unit, bcm_gport_t port)
{
    if (!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        return (BCM_E_PORT);
    }

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
         soc_feature(unit, soc_feature_channelized_switching)) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_delete != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_port_delete(unit, port);
            }
        }
    }
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) ||
        (soc_feature(unit, soc_feature_general_cascade) &&
            _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_PORT(unit, port))) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_delete != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_port_delete(unit, port);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */

    if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
        if (BCM_ESW_SUBPORT_DRV(unit)->subport_port_delete != NULL) {
            return BCM_ESW_SUBPORT_DRV(unit)->
                        subport_port_delete(unit, port);
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_subport_port_get
 * Purpose:
 *      Get a subport
 * Parameters:
 *      unit   - (IN) Device Number
 *      port   - (IN) GPORT (generic port) identifier
 *      config - (OUT) Subport config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_get(int unit, bcm_gport_t port,
                         bcm_subport_config_t *config)
{

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_port_get(unit, port, config);
            }
        }
    }
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) ||
        (soc_feature(unit, soc_feature_general_cascade) &&
            _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_PORT(unit, port))) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_port_get(unit, port, config);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */

    if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
        if (BCM_ESW_SUBPORT_DRV(unit)->subport_port_get != NULL) {
            return BCM_ESW_SUBPORT_DRV(unit)->
                        subport_port_get(unit, port, config);
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_subport_gport_modport_get
 * Purpose:
 *      Given a subport-gport, return teh corresponding mod-port 
 * Parameters:
 *      unit   - (IN) Device Number
 *      subport_gport  - (IN) GPORT (generic port) identifier
 *      module  - (OUT) Module associated with the subport
 *      port  - (OUT) Port associated with the subport  
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_gport_modport_get(int unit, bcm_gport_t subport_gport,
                                  bcm_module_t *module, bcm_port_t *port)
{
    if (!BCM_GPORT_IS_SUBPORT_PORT(subport_gport)) {
        return (BCM_E_PORT);
    }

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport_gport)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_gport_modport_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_gport_modport_get(unit, subport_gport,
                                                      module, port);
            }
        }
    }
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, subport_gport)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport_gport)) ||
        (soc_feature(unit, soc_feature_general_cascade) &&
          _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_PORT(unit, subport_gport))) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_gport_modport_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_gport_modport_get(unit, subport_gport,
                                                      module, port);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_subport_port_traverse
 * Purpose:
 *      Traverse all valid subports and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per subport.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_traverse(int unit,
                              bcm_subport_port_traverse_cb cb,
                              void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
        if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_traverse != NULL) {
            rv = BCM_ESW_SUBPORT_DRV(unit)->
                    coe_port_traverse(unit, cb, user_data);

            if (!(soc_feature(unit, soc_feature_l3))) {
                   return rv;
            }
        }

        if (BCM_ESW_SUBPORT_DRV(unit)->subport_port_traverse != NULL) {
            rv = BCM_ESW_SUBPORT_DRV(unit)->
                        subport_port_traverse(unit, cb, user_data);
        }
    }

    return rv;
}

#if defined(BCM_XGS_SUPPORT)
/*
 * Function:
 *      bcm_esw_subport_group_traverse
 * Purpose:
 *      Traverse all valid subports under given subport group and call the
 *      supplied callback routine.
 * Parameters:
 *      unit          (IN)Device Number
 *      subport_group (IN) Subport group GPORT (generic port identifier)
 *      cb             User callback function, called once per subport.
 *      user_data      cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_group_traverse(int unit,
                              bcm_gport_t subport_group,
                              bcm_subport_port_traverse_cb cb,
                              void *user_data)
{

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(subport_group)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_traverse != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_group_traverse(unit, subport_group, cb, user_data);
            }
        }
    }
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_COE_GPORT_IS_LINKPHY_SUBPORT_GROUP(subport_group)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(subport_group)) ||
        (soc_feature(unit, soc_feature_general_cascade) &&
           _BCM_COE_GPORT_IS_GENERAL_SUBPORT_GROUP(subport_group))) {
            if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
                if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_traverse != NULL) {
                    return BCM_ESW_SUBPORT_DRV(unit)->
                                coe_group_traverse(unit, subport_group, cb, user_data);
                }
            }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/**
 * Function:
 *      bcm_esw_subport_group_linkphy_config_get
 * Purpose:
 *      Get ALL or selective LinkPHY configuration attributes 
 *      on a given port GPORT.
 * Parameters:
 * Parameters:
 *      uint (IN) BCM device number
 *      port (IN) port GPORT
 *      linkphy_cfg (OUT) LinkPHY configuration 
 * Returns:
 *      BCM_E_xxx
 */

int 
bcm_esw_subport_group_linkphy_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_subport_group_linkphy_config_t *linkphy_config)
{
#if defined(BCM_KATANA2_SUPPORT)
    int rv, port_out;
    soc_info_t *si = &SOC_INFO(unit);

    if (soc_feature(unit, soc_feature_linkphy_coe)) {
        rv = _bcm_esw_port_gport_validate(unit, port, &port_out);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,
                                  "ERROR: Invalid port gport 0x%x for LinkPHY config\n"),
                       port));
            return rv;
        }

        if (!BCM_PBMP_MEMBER(si->linkphy_pbm, port_out)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,
                                  "ERROR: port %d is not member of pbmp_linkphy\n"),
                       port_out));
            return BCM_E_PORT;
        }

        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_linkphy_config_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_group_linkphy_config_get(unit, port_out, linkphy_config);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/**
 * Function:
 *      bcm_esw_subport_group_linkphy_config_set
 * Purpose:
 *      Set ALL or selective LinkPHY configuration attributes 
 *      on a given port GPORT.
 * Parameters:
 *      uint (IN) BCM device number
 *      port (IN) port GPORT
 *      linkphy_cfg (IN) LinkPHY configuration 
 * Returns:
 *      BCM_E_xxx
 */

int
bcm_esw_subport_group_linkphy_config_set(
    int unit,
    bcm_gport_t port,
    bcm_subport_group_linkphy_config_t *linkphy_config)
{
#if defined(BCM_KATANA2_SUPPORT)
    int rv, port_out;
    soc_info_t *si = &SOC_INFO(unit);

    if (soc_feature(unit, soc_feature_linkphy_coe)) {
        rv = _bcm_esw_port_gport_validate(unit, port, &port_out);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,
                                  "ERROR: Invalid port gport 0x%x for LinkPHY config\n"),
                       port));
            return rv;
        }

        if (!BCM_PBMP_MEMBER(si->linkphy_pbm, port_out)) {
            LOG_ERROR(BSL_LS_BCM_SUBPORT,
                      (BSL_META_U(unit,
                                  "ERROR: port %d is not member of pbmp_linkphy\n"),
                       port_out));
            return BCM_E_PORT;
        }

        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_group_linkphy_config_set != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_group_linkphy_config_set(unit,
                                                         port_out,
                                                         linkphy_config);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_kt2_subport_port_stat_set
 * Purpose:
 *      Set the statistics value for a given subport port and statistics type
 * Parameters:
 *      unit      - (IN) Device Number
 *      port      - (IN) subport port
 *      stream_id - (IN) stream ID array index (if LinkPHY subport)
 *      stat_type - (IN) statistics type
 *      val       - (IN) value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_stat_set(
    int unit, 
    bcm_gport_t port, 
    int stream_id, 
    bcm_subport_stat_t stat_type, 
    uint64 val)
{
#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_stat_set != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_port_stat_set(unit, port,stream_id, 
                                              stat_type, val);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_kt2_subport_port_stat_get
 * Purpose:
 *      Get the statistics value for a given subport port and statistics type
 * Parameters:
 *      unit      - (IN) Device Number
 *      port      - (IN) subport port
 *      stream_id - (IN) stream ID array index (if LinkPHY subport)
 *      stat_type - (IN) statistics type
 *      val       - (OUT) value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    int stream_id, 
    bcm_subport_stat_t stat_type, 
    uint64 *val)
{
#if defined(BCM_KATANA2_SUPPORT)
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, port)) ||
        (soc_feature(unit, soc_feature_subtag_coe) &&
            _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->coe_port_stat_get != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            coe_port_stat_get(unit, port, stream_id, 
                                              stat_type, val);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
    return BCM_E_UNAVAIL;
}

int bcmi_subport_common_init(int unit, bcm_esw_subport_drv_t *drv)
{
    /* Initialize common SUBPORT module */
    /* Install Tables */
    /* If a driver is not assigned already, assign the driver */
    if(BCM_ESW_SUBPORT_DRV(unit) == NULL) {
        BCM_ESW_SUBPORT_DRV(unit) = drv;
    }

    return BCM_E_NONE;
}

int bcm_esw_subport_linkphy_rx_error_register(
        int unit,
        bcm_subport_linkphy_rx_errors_t errors,
        bcm_subport_linkphy_rx_error_reg_info_t *reg_info,
        void *user_data)
{

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_linkphy_rx_error_register != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_linkphy_rx_error_register(unit, errors, reg_info, 
                                              user_data);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */

    return BCM_E_UNAVAIL;
}

int bcm_esw_subport_linkphy_rx_error_unregister(
        int unit,
        bcm_subport_linkphy_rx_errors_t errors)
{

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe)) {
        if (BCM_ESW_SUBPORT_DRV(unit) != NULL) {
            if (BCM_ESW_SUBPORT_DRV(unit)->subport_linkphy_rx_error_unregister != NULL) {
                return BCM_ESW_SUBPORT_DRV(unit)->
                            subport_linkphy_rx_error_unregister(unit, errors);
            }
        }
    }
#endif /* BCM_KATANA2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_subport_port_group_id_create
 * Purpose:
 *      Create the identification for port group.
 * Parameters:
 *      unit - SOC device unit number
 *      port_group_id - (OUT) subport port group id.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_id_create(
    int unit,
    uint32 options,
    bcm_subport_port_group_t *port_group_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_id_create(unit, options, port_group_id);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_subport_port_group_id_destroy
 * Purpose:
 *      Destroy port group id.
 * Parameters:
 *      unit - SOC device unit number
 *      port_group_id - (IN)subport port group id
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_id_destroy(
    int unit,
    bcm_subport_port_group_t port_group_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_id_destroy(unit, port_group_id);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *       bcm_esw_subport_port_group_id_destroy_all
 * Purpose:
 *      Destroy all the group ids in the system.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_id_destroy_all(
    int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_id_destroy_all(unit);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *       bcm_esw_subport_port_group_id_get
 * Purpose:
 *      Get the port group id.
 * Parameters:
 *      unit - SOC device unit number
 *      port_group_id - (IN)port group id.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_id_get(
    int unit,
    bcm_subport_port_group_t port_group_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_id_get(unit, port_group_id);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *       bcm_esw_subport_port_group_id_get_all
 * Purpose:
 *       Get all the groups ids in the system.
 * Parameters:
 *      unit - SOC device unit number
 *      max_size - (IN)maximum number of port group ids.
 *      port_group_id_array - (OUT)array of port group ids.
 *      array_size - (IN/OUT)actual number of port group ids.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_id_get_all(
    int unit,
    int max_size,
    bcm_subport_port_group_t *port_group_id_array,
    int *array_size)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_id_get_all(unit, max_size,
            port_group_id_array, array_size);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *       bcm_esw_subport_port_group_subport_multi_get
 * Purpose:
 *      Get multiple subports of a subport port group.
 * Parameters:
 *      unit - SOC device unit number
 *      port_group_id - (IN) subport port group id.
 *      max_subports - (IN) maximum number of subports.
 *      subport_gports - (OUT) array of subport gports.
 *      num_subports - (OUT) actual numbe of subports.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_subport_multi_get(
    int unit,
    bcm_subport_port_group_t port_group_id,
    int max_subports,
    bcm_gport_t *subport_gports,
    int *num_subports)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_subport_multi_get(unit, port_group_id,
            max_subports, subport_gports, num_subports);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

/*
 * Function:
 *       bcm_esw_subport_port_group_subport_multi_set
 * Purpose:
 *      Set multiple subports in a subport port group.
 * Parameters:
 *      unit - SOC device unit number
 *      port_group_id - (IN) subport port group id.
 *      num_subports - (IN) number of subports to be added.
 *      subport_gports - (IN) array of subport gports.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_subport_port_group_subport_multi_set(
    int unit,
    bcm_subport_port_group_t port_group_id,
    int num_subports,
    bcm_gport_t *subport_gports)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_port_group_subport_multi_set(unit, port_group_id,
            num_subports, subport_gports);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

bcm_error_t
bcm_esw_subport_tunnel_pbmp_profile_create(int unit,
                                       int num_subports,
                                       bcm_gport_t *subport_gports,
                                       int *index)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_tunnel_pbmp_profile_create(unit,
            num_subports, subport_gports, index);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}


bcm_error_t
bcm_esw_subport_tunnel_pbmp_profile_destroy(int unit,
                                            int index)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_tunnel_pbmp_profile_destroy(unit, index);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

bcm_error_t
bcm_esw_subport_tunnel_pbmp_profile_get(int unit,
                                        int num_subports,
                                        bcm_gport_t *subport_gports,
                                        int *index)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_tunnel_pbmp_profile_get(unit,
            num_subports, subport_gports, index);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

bcm_error_t
bcm_esw_subport_tunnel_pbmp_profile_destroy_all(int unit)
{

    int rv = BCM_E_UNAVAIL;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        rv = bcmi_subport_tunnel_pbmp_profile_destroy_all(unit);
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    return rv;
}

#endif /* BCM_XGS_SUPPORT */
