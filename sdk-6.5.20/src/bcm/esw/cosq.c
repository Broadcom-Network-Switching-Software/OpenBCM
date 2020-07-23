/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 */
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/format.h>
#include <soc/mcm/formatacc.h>
#endif

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/tas.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/scorpion.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/hurricane.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/apache.h>
#include <bcm_int/esw/monterey.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/greyhound.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw/tomahawk2.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/hurricane4.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/firebolt6.h>
#include <soc/monterey.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/bst.h>
#if defined(INCLUDE_TCB) && defined (BCM_TCB_SUPPORT)
#include <bcm_int/esw/tcb.h>
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define BCM_COSQ_QUEUE_VALID(unit, numq) \
    ((numq) >= 0 && (numq) < NUM_COS(unit))

sal_mutex_t cosq_sync_lock[SOC_MAX_NUM_DEVICES];

/* Cosq Event Handler */
typedef struct cosq_event_handler_s {
    struct cosq_event_handler_s *next;
    bcm_cosq_event_types_t event_types;
    SHR_BITDCL pbmp[_SHR_BITDCLSIZE(SOC_MAX_NUM_PORTS)];
    SHR_BITDCL qbmp[_SHR_BITDCLSIZE(BCM_COS_COUNT)];
    bcm_cosq_event_cb cb;
    void *user_data;
} cosq_event_handler_t;

STATIC cosq_event_handler_t *cosq_event_handler_list[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC int cosq_event_handler_count[SOC_MAX_NUM_DEVICES][bcmCosqEventCount] = {{0}};
STATIC sal_mutex_t cosq_event_lock[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC int bcmi_esw_cosq_event_deinit(int unit);
STATIC int bcmi_esw_cosq_event_init(int unit);
static uint8 cosq_event_initialized = 0;

#define COSQ_EVENT_LOCK(unit) \
    do { \
        if (cosq_event_lock[unit]) { \
            sal_mutex_take(cosq_event_lock[unit], \
                           sal_mutex_FOREVER); \
        } \
} while (0)

#define COSQ_EVENT_UNLOCK(unit) \
    do { \
        if (cosq_event_lock[unit]) { \
            sal_mutex_give(cosq_event_lock[unit]); \
        } \
} while (0)

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_cosq_sync
 * Purpose:
 *      Record COSq module persisitent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_cosq_sync(int unit)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_tas_sync(unit));
    }
#endif
    return mbcm_driver[unit]->mbcm_cosq_sync(unit);
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_esw_cosq_config_property_get
 * Purpose:
 *      Get number COSQ from SOC property.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      Number COSQ from SOC property.
 */
int
_bcm_esw_cosq_config_property_get(int unit)
{
    int  num_cos;

#if defined (BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit)) {
        /* Use 2 as the default number of COS queue for HAWKEYE, 
           if bcm_num_cos is not set in config.bcm */
        num_cos = soc_property_get(unit, spn_BCM_NUM_COS, 2);
    } else 
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined (BCM_HURRICANE_SUPPORT) 
    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        if (soc_feature(unit, soc_feature_wh2)) {
            /* Default 4 COSQ if config file not assigned */
            num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
        } else {
            /* Use 8 as the default number of COS queue for HURRICANE,
               if bcm_num_cos is not set in config.bcm */
            num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_COUNT);
        }
    } else
#endif /* BCM_HURRICANE_SUPPORT */
    {
        num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
    }

    if (num_cos < 1) {
        num_cos = 1;
    } else if (num_cos > NUM_COS(unit)) {
        num_cos = NUM_COS(unit);
    }

    return num_cos;
}

#ifdef BCM_TRIUMPH_SUPPORT
#define _BCM_CPU_COS_MAPS_RSVD   9
#define _BCM_INT_PRI_8_15_MASK   0x8
#define _BCM_INT_PRI_0_7_MASK    0xf
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_esw_cpu_cosq_mapping_default_set 
 * Purpose:
 *      Map 16 internal priorities to CPU COS queues using CPU_COS_MAPm
 *      Default cpu_cos_maps should have lower priorities than user created maps
 * Parameters:
 *      unit - Unit number.
 *      numq - number of COS queues (1-8).
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_cpu_cosq_mapping_default_set(int unit, int numq)
{
#ifdef BCM_TRIUMPH_SUPPORT
    int cos=0, prio, ratio, remain, index;
    cpu_cos_map_entry_t cpu_cos_map_entry;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    uint32 tcam_entry[SOC_MAX_MEM_WORDS];
    uint32 data_entry[SOC_MAX_MEM_WORDS];
    uint32 action_set[4];

    /* initialize the variables */
    sal_memset(action_set,0x0,(sizeof(uint32))*4);
    sal_memset(&tcam_entry, 0, sizeof(tcam_entry));
    sal_memset(&data_entry, 0, sizeof(data_entry));
#endif
    ratio = 8 / numq;
    remain = 8 % numq;

    /* Nothing to do on devices with no CPU_COS_MAP memory */
    if (!SOC_MEM_IS_VALID(unit, CPU_COS_MAPm)) {
        return BCM_E_NONE;
    }

    /* Map internal priorities 0 ~ 7 to appropriate CPU CoS queue */
    sal_memset(&cpu_cos_map_entry, 0, sizeof(cpu_cos_map_entry_t));
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        soc_format_field32_set(unit, CPU_COS_MAP_TCAM_FORMATfmt, tcam_entry, VALIDf, 1);
    } else
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, VALIDf, 3);
    } else
#endif
    {
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, VALIDf, 1);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        action_set[0] = _BCM_INT_PRI_0_7_MASK; 
        soc_format_field_set(unit, CPU_COS_MAP_TCAM_FORMATfmt, tcam_entry, MASKf, action_set);
    } else
#endif
    {
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
                        INT_PRI_MASKf, _BCM_INT_PRI_0_7_MASK);
    }

    index = (soc_mem_index_count(unit, CPU_COS_MAPm) - _BCM_CPU_COS_MAPS_RSVD);
    for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++, index++) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            action_set[0] = prio;
            soc_format_field_set(unit, CPU_COS_MAP_TCAM_FORMATfmt, tcam_entry, KEYf, action_set);
            soc_format_field32_set(unit, CPU_COS_MAP_DATA_FORMATfmt, data_entry, COSf, cos);
            SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                                               tcam_entry));
            SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAP_DATA_ONLYm(unit, MEM_BLOCK_ALL, index,
                                               data_entry));
        } else
#endif
        {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
                            INT_PRI_KEYf, prio);
        soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, COSf, cos);
        SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                                               &cpu_cos_map_entry));
        }


        if ((prio + 1) == (((cos + 1) * ratio) + ((remain < (numq - cos)) ?
                           0 : (remain - (numq - cos) + 1)))) {
            cos++;
        }
    }

    /* Map internal priorities 8 ~ 15 to (numq - 1) CPU CoS queue */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        action_set[0] = _BCM_INT_PRI_8_15_MASK;
        soc_format_field_set(unit, CPU_COS_MAP_TCAM_FORMATfmt, tcam_entry, MASKf, action_set);
        action_set[0] = 0x8;
        soc_format_field_set(unit, CPU_COS_MAP_TCAM_FORMATfmt, tcam_entry, KEYf, action_set);
        soc_format_field32_set(unit, CPU_COS_MAP_DATA_FORMATfmt, data_entry, COSf, numq-1);
        SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                                               tcam_entry));
        SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAP_DATA_ONLYm(unit, MEM_BLOCK_ALL, index,
                                               data_entry));
    } else
#endif
    {
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
                        INT_PRI_KEYf, 0x8);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
                        INT_PRI_MASKf, _BCM_INT_PRI_8_15_MASK);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, COSf, numq - 1);
    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                                           &cpu_cos_map_entry));
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_esw_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_init(int unit)
{
    if (SOC_IS_SHADOW(unit)) {
        return BCM_E_UNAVAIL;
    }
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_tas_init(unit));
    }
#endif
    BCM_IF_ERROR_RETURN(bcmi_esw_cosq_event_init(unit));
    return mbcm_driver[unit]->mbcm_cosq_init(unit);
}

/*
 * Function:
 *      bcm_esw_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_detach(int unit)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_tas_deinit(unit));
    }
#endif
    BCM_IF_ERROR_RETURN(bcmi_esw_cosq_event_deinit(unit));
    return (mbcm_driver[unit]->mbcm_cosq_detach(unit, 0));
}

int bcm_esw_cosq_deinit(int unit)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        BCM_IF_ERROR_RETURN(bcmi_esw_tas_deinit(unit));
    }
#endif
    BCM_IF_ERROR_RETURN(bcmi_esw_cosq_event_deinit(unit));
    return (mbcm_driver[unit]->mbcm_cosq_detach(unit, 1));
}

/*
 * Function:
 *      bcm_esw_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - number of COS queues (1, 2, or 4).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_config_set(int unit, bcm_cos_queue_t numq)
{
    if (!SOC_IS_TOMAHAWKX(unit) && (!SOC_IS_TRIDENT3X(unit)) &&(!SOC_IS_MONTEREY(unit))  
         && (numq > 8)) {
        return BCM_E_PARAM;
    }
    return mbcm_driver[unit]->mbcm_cosq_config_set(unit, numq);
}

/*
 * Function:
 *      bcm_esw_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_config_get(int unit, bcm_cos_queue_t *numq)
{
    return (mbcm_driver[unit]->mbcm_cosq_config_get(unit, numq));
}

/*
 * Function:
 *      bcm_esw_cosq_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_mapping_set(int unit, bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
    return (BCM_E_PARAM);
    }

    return (mbcm_driver[unit]->mbcm_cosq_mapping_set(unit, -1,
                             priority, cosq));
}

int
bcm_esw_cosq_port_mapping_set(int unit, bcm_port_t port,
                  bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    if (!soc_feature(unit, soc_feature_ets)) {
        if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
            return (BCM_E_PARAM);
        }
        if (BCM_GPORT_IS_SET(port) && !SOC_IS_KATANAX(unit)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
        }
    }

    return (mbcm_driver[unit]->mbcm_cosq_mapping_set(unit, port,
                             priority, cosq));
}

/*
 * Function:
 *      bcm_esw_cosq_port_mapping_multi_set
 * Purpose:
 *      mapping multiple priorities to their respective cos in one call
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - Ingress Port
 *      count - Number of entries to be configured
 *      priority_array - Priority array to map
 *      cosq_array - COS queue array to map to
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_port_mapping_multi_set(int unit, bcm_port_t port, int count,
                  bcm_cos_t *priority_array,bcm_cos_queue_t *cosq_array)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {    
        if (!soc_feature(unit, soc_feature_ets)) {
            int index = 0;
            if ((count < 1) || (count > 16)) {
                return (BCM_E_PARAM);
            }
            if ((NULL == cosq_array) || (NULL == priority_array)) {
                return (BCM_E_PARAM);
            }
            for (index = 0; index < count; index++) {
                if (!BCM_COSQ_QUEUE_VALID(unit, cosq_array[index])) {
                    return (BCM_E_PARAM);
                }
            }
        }
        return (bcm_kt2_cosq_port_mapping_multi_set(unit, port,count, 
                    priority_array, cosq_array));
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_stat_config_set 
 * Purpose:
 *      configures the stat types provided in stat_array 
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      stat_count -  number of stat type entries in the stat_array
 *      stat_array - array containing information regarding the stats to be configured 
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_stat_config_set(
        int unit, int stat_count,
        bcm_cosq_stat_t *stat_array)
{
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        return (bcm_kt2_cosq_stat_config_set(unit, stat_count, stat_array));
    }
#endif
    return BCM_E_UNAVAIL;
}


 /*
 * Function:
 *      bcm_esw_cosq_stat_config_get 
 * Purpose:
 *      configures the stat types provided in stat_array 
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      stat_count -  number of stat type entries in the stat_array
 *      stat_array - array containing information regarding the stats to be configured 
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_stat_config_get(
        int unit, int stat_count,
        bcm_cosq_stat_t *stat_array)
{
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        return (bcm_kt2_cosq_stat_config_get(unit, stat_count, stat_array));
    }
#endif
    return BCM_E_UNAVAIL;
}       

/*
 * Function:
 *      bcm_esw_cosq_gport_egress_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      gport - GPORT ID.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 *      flags - specify to which type of mapping
 * Returns:
 *      BCM_E_XXX
 */

    int
bcm_esw_cosq_gport_egress_mapping_set(
        int unit, bcm_port_t gport, bcm_cos_t priority,
        bcm_cos_t cosq, uint32 flags)
{
    switch(flags) {
        case BCM_COSQ_GPORT_EGRESS_MAPPING_REDIRECTION_COS:
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                return bcm_kt2_cosq_gport_egress_mapping_set(
                        unit, gport, priority,
                        cosq, flags);
            }
#endif /* BCM_KATANA2_SUPPORT */
            break;
        default :
            break;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_gport_egress_mapping_get
 * Purpose:
 *      Get cosq for the corresponding priority 
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      gport - GPORT ID.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 *      flags - specify to which type of mapping
 * Returns:
 *      BCM_E_XXX
 */


int
bcm_esw_cosq_gport_egress_mapping_get(
        int unit, bcm_port_t gport, bcm_cos_t *priority,
        bcm_cos_t *cosq, uint32 flags)
{
    switch(flags) {
        case BCM_COSQ_GPORT_EGRESS_MAPPING_REDIRECTION_COS:
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                return bcm_kt2_cosq_gport_egress_mapping_get(
                        unit, gport, priority,
                        cosq, flags);
            }
#endif /* BCM_KATANA2_SUPPORT */
            break;
        default :
            break;
    }

    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_cosq_congestion_mapping_set
 * Purpose:
 *      Set congestion state as per the desired flow control for fabric 
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      fabric_modid - Specify the fabric_modid on which congestion state
 *                     need to be applied  
 *      config - specify the flow control bits, which the fabric is interested
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_congestion_mapping_set(int unit,int fabric_modid ,
        bcm_cosq_congestion_mapping_info_t *config)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_congestion_mapping_set(unit, fabric_modid,
               config);
    }
#endif /* BCM_KATANA2_SUPPORT */

  return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_congestion_mapping_get
 * Purpose:
 *      Get congestion state as per the configured flow control value for fabric 
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      fabric_modid - Specify the fabric_modid on which congestion state
 *                     need to be applied  
 *      config - specify the flow control bits, which the fabric is interested
 * Returns:
 *      BCM_E_XXX
 */


int
bcm_esw_cosq_congestion_mapping_get(int unit,int fabric_modid ,
        bcm_cosq_congestion_mapping_info_t *config)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_congestion_mapping_get(unit, fabric_modid,
               config);
    }
#endif /* BCM_KATANA2_SUPPORT */

  return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_mapping_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value
 *      cosq - (Output) COS queue number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_mapping_get(int unit, bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    return (mbcm_driver[unit]->mbcm_cosq_mapping_get(unit, -1,
                             priority, cosq));
}

int
bcm_esw_cosq_port_mapping_get(int unit, bcm_port_t port,
                  bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    if (!soc_feature(unit, soc_feature_ets)) {
        if (BCM_GPORT_IS_SET(port) && !SOC_IS_KATANAX(unit)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
        }
    }

    return (mbcm_driver[unit]->mbcm_cosq_mapping_get(unit, port,
                             priority, cosq));
}

/*
 * Function:
 *      bcm_esw_cosq_mapping_multi_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - Ingress PORT     
 *      priority - Priority array for which we need cosq values
 *      count - number of entries to be retrieved
 *      cosq - (Output) COS queue array  
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_port_mapping_multi_get(int unit, bcm_port_t port, int count,
                  bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {  
        if (!soc_feature(unit, soc_feature_ets)) {
            if (BCM_GPORT_IS_SET(port)) { 
                BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
            }
        }
        return bcm_kt2_cosq_port_mapping_multi_get(unit, port, count, 
                priority_array, cosq_array);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_priority_mapping_get_all
 *
 * Purpose:
 *      Get all priorities associated to the specific priority_group or
 *      service_pool on one port.
 * Parameters:
 *      unit - Unit number.
 *      gport - Gport ID.
 *      index - Priority group ID or service pool id, depends on type value.
 *      type - Support priority group or ingress service pool.
 *      pri_max - Indicate the cosq number user required.
 *      pri_array - Priority list attached to the priority_group on the port,
 *                  -1 is invalid value.
 *      pri_count - Indicate cosq number returned. When pri_max is zero,
 *                  pri_array is null, pri_count will return the total cosq
 *                  number attached to the port pg/port sp.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_cosq_priority_mapping_get_all(int unit, bcm_gport_t gport,
                          int index, bcm_cosq_priority_mapping_t type,
                          int pri_max, bcm_cos_queue_t *pri_array,
                          int *pri_count)
{

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_priority_mapping_get_all(unit, gport, index, type,
                                             pri_max, pri_array, pri_count);
    }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_priority_mapping_get_all(unit, gport, index, type,
                                             pri_max, pri_array, pri_count);
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_priority_mapping_get_all(unit, gport, index, type,
                                             pri_max, pri_array, pri_count);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        return bcm_td2_cosq_priority_mapping_get_all(unit, gport, index, type,
                                             pri_max, pri_array, pri_count);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_sched_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *    weights - Weights for each COS queue
 *        Unused if mode is BCM_COSQ_STRICT.
 *        Indicates number of packets sent before going on to
 *        the next COS queue.
 *    delay - Maximum delay in microseconds before returning the
 *        round-robin to the highest priority COS queue
 *        (Unused if mode other than BCM_COSQ_BOUNDED_DELAY)
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_sched_set(int unit, int mode, const int weights[], int delay)
{
    bcm_pbmp_t pbmp;

    if (soc_feature(unit, soc_feature_ets)) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_CMIC(unit));
    } else {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    }

    return (mbcm_driver[unit]->mbcm_cosq_port_sched_set(unit, pbmp, mode,
                                                        weights, delay));
}

int
bcm_esw_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
            int mode, const int weights[], int delay)
{
    bcm_pbmp_t pbm_all = PBMP_ALL(unit);
    BCM_PBMP_AND(pbm_all, pbm);
    if (BCM_PBMP_NEQ(pbm_all, pbm)) {
        return BCM_E_PORT;
    }
    return (mbcm_driver[unit]->mbcm_cosq_port_sched_set(unit,
                            pbm,
                            mode, weights, delay));
}

/*
 * Function:
 *      bcm_esw_cosq_sched_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode_ptr - (output) Scheduling mode, one of BCM_COSQ_xxx
 *    weights - (output) Weights for each COS queue
 *        Unused if mode is BCM_COSQ_STRICT.
 *    delay - (output) Maximum delay in microseconds before returning
 *        the round-robin to the highest priority COS queue
 *        Unused if mode other than BCM_COSQ_BOUNDED_DELAY.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_sched_get(int unit, int *mode, int weights[], int *delay)
{
    bcm_pbmp_t pbmp;

    if (soc_feature(unit, soc_feature_ets)) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_CMIC(unit));
    } else {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    }

    return (mbcm_driver[unit]->mbcm_cosq_port_sched_get(unit, pbmp, mode,
                                                        weights, delay));
}

int
bcm_esw_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
            int *mode, int weights[], int *delay)
{
    bcm_pbmp_t pbm_all = PBMP_ALL(unit);
    BCM_PBMP_AND(pbm_all, pbm);
    if (BCM_PBMP_NEQ(pbm_all, pbm)) {
        return BCM_E_PARAM;
    }
    return (mbcm_driver[unit]->mbcm_cosq_port_sched_get(unit,
                            pbm,
                            mode, weights, delay));
}

/*
 * Function:
 *      bcm_esw_cosq_sched_weight_max_get
 * Purpose:
 *      Retrieve maximum weights for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *    weight_max - (output) Maximum weight for COS queue.
 *        0 if mode is BCM_COSQ_STRICT.
 *        1 if mode is BCM_COSQ_ROUND_ROBIN.
 *        -1 if not applicable to mode.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    return (mbcm_driver[unit]->mbcm_cosq_sched_weight_max_get(unit,
                                  mode,
                                  weight_max));
}

/*
 * Function:
 *      bcm_esw_cosq_port_bandwidth_set
 * Purpose:
 *      Set bandwidth values for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *    port - port to configure, -1 for all ports.
 *      cosq - COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - minimum bandwidth, kbits/sec.
 *      kbits_sec_max - maximum bandwidth, kbits/sec.
 *      flags - may include:
 *              BCM_COSQ_BW_EXCESS_PREF
 *              BCM_COSQ_BW_MINIMUM_PREF
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                uint32 kbits_sec_min,
                                uint32 kbits_sec_max,
                                uint32 flags)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    bcm_port_t loc_port;
    bcm_cos_queue_t start_cos, end_cos, loc_cos;
    int num_cosq;

    if (port < 0) {
        if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit) ||
            SOC_IS_SC_CQ(unit) || SOC_IS_ENDURO(unit)  ||
            SOC_IS_KATANA(unit)) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
            loc_port = SOC_PORT_MIN(unit,port);
        } else {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
            loc_port = SOC_PORT_MIN(unit,all);
        }
        num_cosq = NUM_COS(unit);
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, port, &loc_port)); 
        } else {
            loc_port = port;
        }
        if (SOC_PORT_VALID(unit, loc_port)) {
            BCM_PBMP_PORT_SET(pbmp, loc_port);
        } else {
            return BCM_E_PORT;
        }
        num_cosq = IS_CPU_PORT(unit, loc_port) ?
            NUM_CPU_COSQ(unit) : NUM_COS(unit);
    }

    if (soc_feature(unit, soc_feature_wh2)) {
        /* Get the configured cosq number */
        BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
    }

    if (cosq < 0) {
        start_cos = 0;
        end_cos = num_cosq - 1;
    } else if (cosq >= num_cosq) {
        return BCM_E_PARAM;
    } else {
        start_cos = end_cos = cosq;
    }

    BCM_PBMP_ITER(pbmp, loc_port) {
        if ((SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
             SOC_IS_FIREBOLT6(unit)) && IS_FAE_PORT(unit, loc_port)) {
            continue;
        }
        for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
            if ((rv = mbcm_driver[unit]->mbcm_cosq_port_bandwidth_set(unit,
                           loc_port, loc_cos, kbits_sec_min, kbits_sec_max,
                            kbits_sec_max, flags)) < 0) {
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_cosq_port_bandwidth_get
 * Purpose:
 *      Retrieve bandwidth values for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *    port - port to configure, -1 for any port.
 *      cosq - COS queue to configure, -1 for any COS queue.
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT) may include:
 *              BCM_COSQ_BW_EXCESS_PREF
 *              BCM_COSQ_BW_MINIMUM_PREF
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_port_bandwidth_get(int unit, bcm_port_t port, 
                                bcm_cos_queue_t cosq,
                                uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max,
                                uint32 *flags)
{
    bcm_port_t loc_port;
    bcm_cos_queue_t loc_cos;
    int num_cosq;
    uint32 kbits_sec_burst;    /* Dummy variable */

    if (flags == NULL) {
        return BCM_E_PARAM;
    }

    if (port < 0) {
        loc_port = SOC_PORT_MIN(unit,all);
        num_cosq = NUM_COS(unit);
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, port, &loc_port)); 
        } else {
            loc_port = port;
        }
        if (!SOC_PORT_VALID(unit, loc_port)) {
            return BCM_E_PORT;
        }
        num_cosq = IS_CPU_PORT(unit, loc_port) ?
            NUM_CPU_COSQ(unit) : NUM_COS(unit);
    }

    if (soc_feature(unit, soc_feature_wh2)) {
        /* Get the configured max cosq number */
        BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
    }

    if (cosq < 0) {
        loc_cos = 0;
    } else if (cosq >= num_cosq) {
        return BCM_E_PARAM;
    } else {
        loc_cos = cosq;
    }

    return (mbcm_driver[unit]->mbcm_cosq_port_bandwidth_get(unit,
                           loc_port, loc_cos, kbits_sec_min, kbits_sec_max,
                                              &kbits_sec_burst, flags));
}

/*
 * Function:
 *      bcm_esw_cosq_discard_set
 * Purpose:
 *      Set the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      flags - BCM_COSQ_DISCARD_*
 * Returns:
 *      BCM_E_XXX
 */
 
int     
bcm_esw_cosq_discard_set(int unit, uint32 flags)
{
    return (mbcm_driver[unit]->mbcm_cosq_discard_set(unit, flags));
}
 
/*
 * Function:
 *      bcm_esw_cosq_discard_get
 * Purpose:
 *      Get the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      flags - (OUT) BCM_COSQ_DISCARD_*
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_discard_get(int unit, uint32 *flags)
{
    if (flags == NULL) {
        return BCM_E_PARAM;
    }

    return (mbcm_driver[unit]->mbcm_cosq_discard_get(unit, flags));
}
    
/*
 * Function:
 *      bcm_esw_cosq_discard_port_set
 * Purpose:
 *      Set the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      port  - port to configure (-1 for all ports).
 *      cosq  - COS queue to configure (-1 for all queues).
 *      color - BCM_COSQ_DISCARD_COLOR_*
 *      drop_start -  percentage of queue
 *      drop_slope -  degress 0..90
 *      average_time - in microseconds
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_discard_port_set(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq,
                          uint32 color,
                          int drop_start,
                          int drop_slope,
                          int average_time)
{
    return mbcm_driver[unit]->mbcm_cosq_discard_port_set
        (unit, port, cosq, color, drop_start, drop_slope, average_time);
}

/*
 * Function:
 *      bcm_esw_cosq_discard_port_get
 * Purpose:
 *      Set the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      port  - port to get (-1 for any).
 *      cosq  - COS queue to get (-1 for any).
 *      color - BCM_COSQ_DISCARD_COLOR_*
 *      drop_start - (OUT) percentage of queue
 *      drop_slope - (OUT) degress 0..90
 *      average_time - (OUT) in microseconds
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_discard_port_get(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq,
                          uint32 color,
                          int *drop_start,
                          int *drop_slope,
                          int *average_time)
{
    return mbcm_driver[unit]->mbcm_cosq_discard_port_get
        (unit, port, cosq, color, drop_start, drop_slope, average_time);
}

/*
 * Function:
 *      bcm_cosq_gport_discard_set
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to configure
 *      discard - (IN) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_discard_set(unit, port, cosq, 
            BCM_COSQ_BUFFER_ID_INVALID, discard);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_discard_set(unit, port, cosq,
            BCM_COSQ_BUFFER_ID_INVALID, discard);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
                    return bcm_kt_cosq_gport_discard_set(unit, port, cosq, discard);
                        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                        return bcm_kt2_cosq_gport_discard_set(unit, port, cosq, discard);
                            }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
                if (SOC_IS_TRIUMPH3(unit)) {
                            return bcm_tr3_cosq_gport_discard_set(unit, port, cosq, discard);
                                }
#endif /* BCM_TRIUMPH3_SUPPORT */

/*Removed GPORT_INVALID check for KT/KT2/TR3/TD/TD2 to enable 
  global wred discard profile for all ports when -1 is passed */

    if (port == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if(SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {
            return bcm_hu_cosq_gport_discard_set(unit, port, cosq, discard);
        } else         
#endif /*BCM_HURRICANE_SUPPORT*/
#if defined(BCM_GREYHOUND_SUPPORT)
        if(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            return bcm_gh_cosq_gport_discard_set(unit, port, cosq, discard);
        } else      
#endif /*BCM_GREYHOUND_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
            return bcmi_gh2_cosq_gport_discard_set(unit, port, cosq, discard);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            return bcm_tr_cosq_gport_discard_set(unit, port, cosq, discard);
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_discard_get
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to get
 *      discard - (IN/OUT) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_discard_get(unit, port, cosq,
            BCM_COSQ_BUFFER_ID_INVALID, discard);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_discard_get(unit, port, cosq,
            BCM_COSQ_BUFFER_ID_INVALID, discard);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
                    return bcm_kt_cosq_gport_discard_get(unit, port, cosq, discard);
                        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                        return bcm_kt2_cosq_gport_discard_get(unit, port, cosq, discard);
                            }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
                if (SOC_IS_TRIUMPH3(unit)) {
                            return bcm_tr3_cosq_gport_discard_get(unit, port, cosq, discard);
                                }
#endif /* BCM_TRIUMPH3_SUPPORT */

/*Removed GPORT_INVALID check for KT/KT2/TR3/TD/TD2 to enable 
    global wred discard profile for all ports when -1 is passed */

    if (port == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if(SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {
            return bcm_hu_cosq_gport_discard_get(unit, port, cosq, discard);
        } else             
#endif
#if defined(BCM_GREYHOUND_SUPPORT)
        if(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            return bcm_gh_cosq_gport_discard_get(unit, port, cosq, discard);
        } else      
#endif /*BCM_GREYHOUND_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
            return bcmi_gh2_cosq_gport_discard_get(unit, port, cosq, discard);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */

        {
            return bcm_tr_cosq_gport_discard_get(unit, port, cosq, discard);
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_esw_cosq_gport_discard_extended_set
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      id      - (IN) Integrated cosq index
 *      discard - (IN) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_discard_extended_set(
    int unit, 
    bcm_cosq_object_id_t *id,
    bcm_cosq_gport_discard_t *discard)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_gport_discard_set(unit, 
            id->port, id->cosq, id->buffer, discard);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_gport_discard_extended_get
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      id      - (IN) Integrated cosq index
 *      discard - (OUT) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_discard_extended_get(
    int unit, 
    bcm_cosq_object_id_t *id,
    bcm_cosq_gport_discard_t *discard)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_gport_discard_get(unit, 
            id->port, id->cosq, id->buffer, discard);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_discard_profile_create
 * Purpose:
 *     Create an empty rule profile
 * Parameters:
 *     unit                - (IN) unit number
 *     profile_id          - (OUT) profile index
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_discard_profile_create(
    int unit,
    int *profile_id)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_wred_resolution_entry_create(unit, profile_id);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_wred_resolution_entry_create(unit, profile_id);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_wred_resolution_entry_create(unit, profile_id);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_wred_resolution_entry_create(unit, profile_id);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_wred_resolution_entry_create(unit, profile_id);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_wred_resolution_entry_create(unit, profile_id);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_discard_profile_destroy
 * Purpose:
 *     Destroy a rule profile
 * Parameters:
 *     unit                - (IN) unit number
 *     profile_id          - (IN) profile index
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_discard_profile_destroy(
    int unit,
    int profile_id)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_wred_resolution_entry_destroy(unit, profile_id);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_wred_resolution_entry_destroy(unit, profile_id);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_wred_resolution_entry_destroy(unit, profile_id);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_wred_resolution_entry_destroy(unit, profile_id);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_wred_resolution_entry_destroy(unit, profile_id);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_wred_resolution_entry_destroy(unit, profile_id);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_discard_profile_update
 * Purpose:
 *     Update one rule in a profile
 * Parameters:
 *     unit                - (IN) unit number
 *     profile_id          - (IN) profile index
 *     rule                - (IN) discard rule
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_discard_profile_update(
    int unit,
    int profile_id,
    bcm_cosq_discard_rule_t *rule)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_wred_resolution_set(unit, profile_id, rule);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_wred_resolution_set(unit, profile_id, rule);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_wred_resolution_set(unit, profile_id, rule);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_wred_resolution_set(unit, profile_id, rule);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_wred_resolution_set(unit, profile_id, rule);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_wred_resolution_set(unit, profile_id, rule);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_discard_profile_get
 * Purpose:
 *     Get all rules of a profile
 * Parameters:
 *     unit                - (IN) unit number
 *     profile_id          - (IN) profile index
 *     max                 - (IN) rule array size
 *     rule_array          - (OUT) rule array
 *     rule_count          - (OUT) actual rules count
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_discard_profile_get(
    int unit,
    int profile_id,
    int max,
    bcm_cosq_discard_rule_t *rule_array,
    int *rule_count)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_wred_resolution_get(unit, profile_id, max,
                rule_array, rule_count);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_wred_resolution_get(unit, profile_id, max,
                rule_array, rule_count);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_wred_resolution_get(unit, profile_id, max,
                rule_array, rule_count);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_wred_resolution_get(unit, profile_id, max,
                rule_array, rule_count);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_wred_resolution_get(unit, profile_id, max,
                rule_array, rule_count);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_wred_resolution_get(unit, profile_id, max,
                rule_array, rule_count);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_cosq_gport_add
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Physical port.
 *      numq - (IN) Number of COS queues.
 *      flags - (IN) Flags.
 *      gport - (IN/OUT) GPORT ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_add(int unit, bcm_gport_t port, int numq, 
                       uint32 flags, bcm_gport_t *gport)
{
    if (port == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_add(unit, port, numq, flags, gport); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Return BCM_E_PARAM because of fixed hierarchy gport tree
           is created during init */
        return BCM_E_PARAM;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* Return BCM_E_PARAM because of fixed hierarchy gport tree
           is created during init */
        return BCM_E_PARAM;
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        /* Return BCM_E_PARAM because of fixed hierarchy gport tree
           is created during init */
        return BCM_E_PARAM;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_add(unit, port, numq, flags, gport, 0);
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        if (!SOC_IS_HURRICANEX(unit) && !SOC_IS_GREYHOUND(unit) &&
            !SOC_IS_GREYHOUND2(unit)) {
            return bcm_tr_cosq_gport_add(unit, port, numq, flags, gport);
        }
    }
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_delete
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    if (gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_delete(unit, gport); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_delete(unit, gport);
    }
#endif

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_delete(unit, gport);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_delete(unit, gport);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Return BCM_E_PARAM because of fixed hierarchy gport tree
           is created during init */
        return BCM_E_PARAM;
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* Return BCM_E_PARAM because of fixed hierarchy gport tree
           is created during init */
        return BCM_E_PARAM;
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        /* Return BCM_E_PARAM because of fixed hierarchy gport tree
           is created during init */
        return BCM_E_PARAM;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_delete(unit, gport);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_delete(unit, gport);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_delete(unit, gport);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_delete(unit, gport);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_delete(unit, gport);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    /*no stage 1 queues and s1 scheduler to delete for HU*/
    if (SOC_IS_TR_VL(unit)) {
        if (!SOC_IS_HURRICANEX(unit) && !SOC_IS_GREYHOUND(unit) &&
            !SOC_IS_GREYHOUND2(unit)) {
            return bcm_tr_cosq_gport_delete(unit, gport);
        }
    }
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_delete(unit, gport);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_traverse
 * Purpose:
 *      Walks through the valid COSQ GPORTs and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_traverse(unit, cb, user_data); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_traverse(unit, cb, user_data);
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_traverse(unit, cb, user_data);

    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_traverse(unit, cb, user_data);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    /*for each 24q ports, this calls a call-back func 
    for scheduler gport. 24q ports and scheduler (S1) do not 
    exist for Hurricane*/
    if (SOC_IS_TR_VL(unit)) {
        if (!SOC_IS_HURRICANEX(unit) && !SOC_IS_GREYHOUND(unit) &&
            !SOC_IS_GREYHOUND2(unit)) {
            return bcm_tr_cosq_gport_traverse(unit, cb, user_data);
        }
    }
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_traverse(unit, cb, user_data);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_cosq_gport_traverse_by_port
 * Purpose:
 *     Walks through the valid COSQ GPORTs belong to a port and calls
 *     the user supplied callback function for each entry.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number or GPORT identifier
 *     trav_fn    - (IN) Callback function.
 *     user_data  - (IN) User data to be passed to callback function
 * Returns:
 *     BCM_E_NONE - Success.
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                           bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_traverse_by_port(unit, port, cb, user_data);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_traverse_by_port(unit, port, cb, user_data);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        return bcm_td3_cosq_gport_traverse_by_port(unit, port, cb, user_data);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_traverse_by_port(unit, port, cb, user_data);
    }
#endif /* BCM_URRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_traverse_by_port(unit, port, cb, user_data);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_traverse_by_port(unit, port, cb, user_data);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t gport, bcm_cos_queue_t cosq)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */


    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_mapping_set(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */
    return BCM_E_UNAVAIL;
}

int
bcm_esw_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_KATANA2_SUPPORT */


    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                              gport, cosq);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_mapping_get(unit, ing_port, int_pri, flags,
                                             gport, cosq);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_bandwidth_set
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - (IN) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (IN) maximum bandwidth, kbits/sec.
 *      flags - (IN) BCM_COSQ_BW_*
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport, 
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min, 
                                 uint32 kbits_sec_max, uint32 flags)
{
    if (gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }

    if ((kbits_sec_max != 0) && (kbits_sec_max < kbits_sec_min )) {
        return (BCM_E_PARAM);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_bandwidth_set(unit, gport,
                                               cosq, kbits_sec_min,
                                               kbits_sec_max, flags); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_bandwidth_set(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_bandwidth_set(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_gport_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_bandwidth_set(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_bandwidth_set(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_bandwidth_set(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_bandwidth_set(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_child_node_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_child_node_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_bandwidth_set(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
            return bcm_hu_cosq_gport_bandwidth_set(unit, gport, 
                                                   cosq, kbits_sec_min, 
                                                   kbits_sec_max, flags);
        }           
#endif /*SOC_IS_HURRICANE*/
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
            return bcmi_gh2_cosq_gport_bandwidth_set(unit, gport,
                                                     cosq, kbits_sec_min,
                                                     kbits_sec_max, flags);
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
        return bcm_tr_cosq_gport_bandwidth_set(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_bandwidth_get
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT) BCM_COSQ_BW_*
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport, 
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min, 
                                 uint32 *kbits_sec_max, uint32 *flags)
{
    if (gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_bandwidth_get(unit, gport,
                                               cosq, kbits_sec_min,
                                               kbits_sec_max, flags);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_bandwidth_get(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_bandwidth_get(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_gport_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_bandwidth_get(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_bandwidth_get(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_bandwidth_get(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_bandwidth_get(unit, gport, cosq,
                                                kbits_sec_min, kbits_sec_max,
                                                flags);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_child_node_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_child_node_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_bandwidth_get(unit, gport, cosq,
                                               kbits_sec_min, kbits_sec_max,
                                               flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
            return bcm_hu_cosq_gport_bandwidth_get(unit, gport, 
                                                   cosq, kbits_sec_min, 
                                                   kbits_sec_max, flags);
        } else
#endif /* BCM_HURRICANE_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
            return bcmi_gh2_cosq_gport_bandwidth_get(unit, gport,
                                                     cosq, kbits_sec_min,
                                                     kbits_sec_max, flags);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            return bcm_tr_cosq_gport_bandwidth_get(unit, gport, 
                                                   cosq, kbits_sec_min, 
                                                   kbits_sec_max, flags);
        }
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_sched_set
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      mode - (IN) Scheduling mode, one of BCM_COSQ_xxx
 *    weight - (IN) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_sched_set(int unit, bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, int mode, int weight)
{
    if (gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_sched_set(unit, gport, cosq, mode, weight); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
            return bcm_hu_cosq_gport_sched_set(unit, gport, cosq, mode, weight);            
        } else     
#endif /* BCM_HURRICANE_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
            return bcmi_gh2_cosq_gport_sched_set(
                       unit, gport, cosq, mode, weight);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            return bcm_tr_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
        }
    }
#endif
#ifdef BCM_SCORPION_SUPPORT 
    if (SOC_IS_SC_CQ(unit)) {
        return bcm_sc_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_sched_get
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue
 *      mode - (OUT) Scheduling mode, one of BCM_COSQ_xxx
 *    weight - (OUT) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, int *mode, int *weight)
{
    if (gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
            return bcm_hu_cosq_gport_sched_get(unit, gport, cosq, mode, weight);           
        } else     
#endif /* BCM_HURRICANE_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_GREYHOUND2(unit)) {
            return bcmi_gh2_cosq_gport_sched_get(
                       unit, gport, cosq, mode, weight);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            return bcm_tr_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
        }
    }
#endif
#ifdef BCM_SCORPION_SUPPORT 
    if (SOC_IS_SC_CQ(unit)) {
        return bcm_sc_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_attach
 * Purpose:
 *      
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to attach to.
 *      cosq       - (IN) COS queue to attach to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                          bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    if (sched_gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_attach(unit, sched_gport,
                                        input_gport, cosq); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_attach(unit, sched_gport, 
                                        input_gport, cosq);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_attach(unit, sched_gport, 
                                        input_gport, cosq);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(bcm_kt2_cosq_gport_validate(unit,
        sched_gport, input_gport, cosq));
        return bcm_kt2_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    /*for HU there is no VLAN cosq and scheduler (S1)*/
	if (SOC_IS_TR_VL(unit)) {
        if (!SOC_IS_HURRICANEX(unit) && ! SOC_IS_GREYHOUND(unit) &&
            !SOC_IS_GREYHOUND2(unit)) {
            return bcm_tr_cosq_gport_attach(unit, sched_gport, 
                                        input_gport, cosq);
        }
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_attach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_reattach
 * Purpose:
 *      
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to attach to.
 *      cosq       - (IN) COS queue to attach to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_cosq_gport_reattach(int unit, bcm_gport_t sched_gport, 
                          bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{

    if (sched_gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_reattach(unit, sched_gport,
                                           input_gport, cosq);
    }
#endif /* BCM_KATANA2_SUPPORT */

    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_detach
 * Purpose:
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to detach from.
 *      cosq       - (IN) COS queue to detach from.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                          bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    if (sched_gport == BCM_GPORT_INVALID || input_gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_detach(unit, sched_gport,
                                        input_gport, cosq);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_detach(unit, sched_gport,
                                        input_gport, cosq);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_detach(unit, sched_gport,
                                        input_gport, cosq);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN (bcm_kt2_cosq_gport_validate
                (unit, sched_gport, input_gport, cosq));
        return bcm_kt2_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        if (!SOC_IS_HURRICANEX(unit) && ! SOC_IS_GREYHOUND(unit) &&
            !SOC_IS_GREYHOUND2(unit)) {
            return bcm_tr_cosq_gport_detach(unit, sched_gport,
                                        input_gport, cosq);
        }
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_detach(unit, sched_gport, input_gport, cosq);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_attach_get
 * Purpose:
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (OUT) GPORT attached to.
 *      cosq       - (OUT) COS queue attached to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                              bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    if (sched_gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_attach_get(unit, sched_gport,
                                            input_gport, cosq);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_attach_get(unit, sched_gport, 
                                            input_gport, cosq);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_attach_get(unit, sched_gport, 
                                            input_gport, cosq);
    }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                             cosq);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                             cosq);
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                             cosq);
    }
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                             cosq);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                            cosq);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        if (!SOC_IS_HURRICANEX(unit) && ! SOC_IS_GREYHOUND(unit) &&
            !SOC_IS_GREYHOUND2(unit)) {
            return bcm_tr_cosq_gport_attach_get(unit, sched_gport, 
                                            input_gport, cosq);
        }
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_attach_get(unit, sched_gport, input_gport,
                                              cosq);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_child_get
 * Purpose:
 *
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      in_gport   - (IN)  Scheduler GPORT ID.
 *      cosq       - (IN)  COS queue attached to.
 *      out_gport  - (OUT) child GPORT ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                              bcm_cos_queue_t cosq,
                              bcm_gport_t *out_gport)
{
    if (in_gport == BCM_GPORT_INVALID) {
        return (BCM_E_PORT);
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_child_get(unit, in_gport,cosq,
                                           out_gport);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_child_get(unit, in_gport,cosq,
                                           out_gport);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_child_get(unit, in_gport,cosq,
                                           out_gport);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_child_get(unit, in_gport, cosq,
                                             out_gport);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_child_get(unit, in_gport,cosq,
                                            out_gport);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_gport_t *physical_port, 
    int *num_cos_levels, 
    uint32 *flags)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_get(unit, gport, physical_port,
                num_cos_levels, flags); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_get(unit, gport, physical_port,
                num_cos_levels, flags);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_gport_get(unit, gport, physical_port,
                                      num_cos_levels, flags);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_gport_get(unit, gport, physical_port,
                                      num_cos_levels, flags);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_gport_get(unit, gport, physical_port,
                                      num_cos_levels, flags);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_gport_get(unit, gport, physical_port,
                                      num_cos_levels, flags);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_get(unit, gport, physical_port,
                                     num_cos_levels, flags);
    }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_gport_get(unit, gport, physical_port,
                                       num_cos_levels, flags);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_size_set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_size_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 bytes_min, 
    uint32 bytes_max)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_size_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_size_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 *bytes_min, 
    uint32 *bytes_max)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_enable_set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_enable_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_enable_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_enable_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int *enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_stat_enable_set
 * Purpose:
 *      ?
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_enable_set(
    int unit, 
    bcm_gport_t gport, 
    int enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_stat_enable_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_enable_get(
    int unit, 
    bcm_gport_t gport, 
    int *enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      _bcm_esw_cosq_gport_stat_get
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.  
 *
 *Parameters:
 *      unit        - (IN) Unit number.
 *      sync_mode   - (IN) hwcount is to be synced to sw count 
 *      port        - (IN) GPORT ID.
 *      cosq        - (IN) COS queue to configure
 *      stat        - (IN) Statistic to be retrieved.
 *      value       - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_esw_cosq_gport_stat_get(
    int unit, 
    int sync_mode,
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 *value)
{
#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_cosq_gport_stat_ability)) {
        return bcm_kt_cosq_gport_stat_get(unit, sync_mode, gport, cosq,
                                          stat, value);
    } 
#endif /* BCM_KATANA_SUPPORT */
    return BCM_E_UNAVAIL; 
}



/*
 * Function:
 *      bcm_esw_cosq_gport_stat_get
 * Parameters:
 *      unit  - (IN) Unit number
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 *value)
{

    return _bcm_esw_cosq_gport_stat_get(unit, 0, gport, cosq, stat, value);

}    


/*
 * Function:
 *      bcm_esw_cosq_gport_stat_sync_get
 *      sw accumulated counters synced with hw count.
 * Parameters:
 *      unit  - (IN) Unit number
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_sync_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 *value)
{

    return _bcm_esw_cosq_gport_stat_get(unit, 1, gport, cosq, stat, value);

}    


/*
 * Function:
 *      bcm_cosq_gport_stat_set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 value)
{
#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_cosq_gport_stat_ability)) {
        return bcm_kt_cosq_gport_stat_set(unit, gport, cosq,
                                          stat, value);
    } 
#endif /* BCM_KATANA_SUPPORT */
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_esw_cosq_control_set
 * Purpose:
 *      Set specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      type - (IN) feature
 *      arg  - (IN) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type, int arg)
{
    uint32 kbits_sec_min, kbits_sec_max, kbits_sec_burst;
    uint32 kbits_burst, flags;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_cos_queue_t start_cos, end_cos, loc_cos;
    uint32 cm_val, ocm_val, cmc1_val, ocmc1_val;
    uint64 cmc_val64, ocmc_val64, mask64;
    int num_cos = NUM_COS(unit);
    uint32 kbits_burst_min, kbits_burst_max;
#if defined(BCM_HURRICANE2_SUPPORT)
    soc_reg_t select_reg[8] = { \
        EEE_PROFILE_SEL_0r, EEE_PROFILE_SEL_1r, \
        EEE_PROFILE_SEL_2r, EEE_PROFILE_SEL_3r, \
        EEE_PROFILE_SEL_4r, EEE_PROFILE_SEL_5r, \
        EEE_PROFILE_SEL_6r, EEE_PROFILE_SEL_7r};
#endif /* BCM_HURRICANE2_SUPPORT */

    kbits_burst_min = 0;
    kbits_burst_max = 0;

    COMPILER_REFERENCE(kbits_burst_min);
    COMPILER_REFERENCE(kbits_burst_max);
    
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            return bcm_th3_cosq_control_set(unit, gport, cosq,
                BCM_COSQ_BUFFER_ID_INVALID, type, arg);
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            return bcm_th_cosq_control_set(unit, gport, cosq,
                BCM_COSQ_BUFFER_ID_INVALID, type, arg);
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return (bcm_hr4_cosq_control_set(unit, gport, cosq, type, arg));
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return (bcm_fb6_cosq_control_set(unit, gport, cosq, type, arg));
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return (bcm_hx5_cosq_control_set(unit, gport, cosq, type, arg));
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            return (bcm_td3_cosq_control_set(unit, gport, cosq, type, arg));
        }
#endif /* BCM_TRIDENT3_SUPPORT */

    switch (type) {
    case bcmCosqControlBandwidthBurstMax:
        kbits_burst = arg & 0x7fffffff; /* Convert to uint32  */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN
                (bcm_mn_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_mn_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst_min,
                                                         kbits_burst); 
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_ap_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst_min,
                                                         kbits_burst);
        }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_tr2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_tr2_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                              kbits_burst_min, kbits_burst);
        }
#endif  
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_tr3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_tr3_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                              kbits_burst_min, kbits_burst);
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_td2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_td2_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                kbits_burst_min, kbits_burst);
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_td_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_td_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                kbits_burst_min, kbits_burst);
        }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_kt_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_kt_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst_min,
                                                         kbits_burst);
        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_kt2_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst_min,
                                                         kbits_burst);
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
            if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
                return bcm_hu_cosq_gport_bandwidth_burst_set(unit, gport,
                                                              cosq, kbits_burst);                
            } else    
#endif /* BCM_HURRICANE_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                return bcmi_gh2_cosq_gport_bandwidth_burst_set(unit, gport,
                                                               cosq,
                                                               kbits_burst);
            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(
                    bcm_tr_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                          &kbits_burst_min, &kbits_burst_max));
                return bcm_tr_cosq_gport_bandwidth_burst_set(unit, gport, cosq, 
                                                  kbits_burst_min , kbits_burst);
            }
        }
#endif  
        if (gport < 0) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        } else {
            /* Must use local ports on legacy devices */
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (SOC_PORT_VALID(unit, port)) {
                BCM_PBMP_PORT_SET(pbmp, port);
            } else {
                return BCM_E_PORT;
            }
        }

        if (cosq < 0) {
            start_cos = 0;
            end_cos = NUM_COS(unit) - 1;
        } else {
            if (cosq < NUM_COS(unit)) {
                start_cos = end_cos = cosq;
            } else {
                return BCM_E_PARAM;
            }
        }

        BCM_PBMP_ITER(pbmp, port) {
            for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
                BCM_IF_ERROR_RETURN
                    (mbcm_driver[unit]->
                     mbcm_cosq_port_bandwidth_get(unit, port, loc_cos,
                                                  &kbits_sec_min,
                                                  &kbits_sec_max,
                                                  &kbits_sec_burst, &flags));
                BCM_IF_ERROR_RETURN
                    (mbcm_driver[unit]->
                     mbcm_cosq_port_bandwidth_set(unit, port, loc_cos,
                                                  kbits_sec_min,
                                                  kbits_sec_max,
                                                  kbits_burst, flags));
            }
        }
        return BCM_E_NONE;

    case bcmCosqControlBandwidthBurstMin:
        kbits_burst = arg & 0x7fffffff; /* Convert to uint32  */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN
                (bcm_mn_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_mn_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst,
                                                         kbits_burst_max);
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_ap_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst,
                                                         kbits_burst_max);
        } else 
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_kt_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_kt_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst,
                                                         kbits_burst_max);
        } else
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_kt2_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst,
                                                         kbits_burst_max);
        } else
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_tr2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_tr2_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                              kbits_burst, kbits_burst_max);
        } else
#endif  
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_tr3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_tr3_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                      kbits_burst, kbits_burst_max);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_td2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_td2_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                        kbits_burst, kbits_burst_max);
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            BCM_IF_ERROR_RETURN(
                bcm_td_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                      &kbits_burst_min, &kbits_burst_max));
            return bcm_td_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                        kbits_burst, kbits_burst_max);
        } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
            /* coverity[dead_error_begin : FALSE] */
            if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
               SOC_IS_GREYHOUND2(unit)) {
             /*need to support later*/
              return BCM_E_UNAVAIL;
            } else
#endif /* BCM_HURRICANE_SUPPORT*/
            {
                BCM_IF_ERROR_RETURN(
                    bcm_tr_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                          &kbits_burst_min, &kbits_burst_max));
                return bcm_tr_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                  kbits_burst, kbits_burst_max);
            }
        } else
#endif /* BCM_TRIUMPH_SUPPORT */
        return BCM_E_UNAVAIL;

    case bcmCosqControlSchedulable:
        if (!soc_reg_field_valid(unit, COSMASKr, COSMASKf)) {
            return BCM_E_UNAVAIL;
        }

        if (gport < 0) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        } else {
            /* Must use local ports on legacy devices */
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (SOC_PORT_VALID(unit, port)) {
                BCM_PBMP_PORT_SET(pbmp, port);
            } else {
                return BCM_E_PORT;
            }
        }
        
        if (BCM_PBMP_MEMBER(pbmp, CMIC_PORT(unit))) {
            
            if (soc_reg_field_valid(unit, COSMASK_CPU1r, COSMASKf)) {
                if (cosq < 0) {
                    start_cos = 0;
                    end_cos = 31; 
                } else {
                    if (cosq < 32) {
                        start_cos = end_cos = cosq;
                    } else {
                        return BCM_E_PARAM;
                    }
                }
                port = CMIC_PORT(unit);
                BCM_IF_ERROR_RETURN(READ_COSMASKr(unit, port, &cm_val));
                BCM_IF_ERROR_RETURN
                    (READ_COSMASK_CPU1r(unit, &cmc1_val));
                ocm_val = cm_val;
                ocmc1_val = cmc1_val;
                for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
                    if (loc_cos < 10) {
                        if (arg) {
                            cm_val &= ~(1 << loc_cos);
                        } else {
                            cm_val |= (1 << loc_cos);
                        }
                    } else {
                        if (arg) {
                            cmc1_val &= ~(1 << loc_cos);
                        } else {
                            cmc1_val |= (1 << loc_cos);
                        }
                    }
                }
                if (ocm_val != cm_val) {
                    BCM_IF_ERROR_RETURN(WRITE_COSMASKr(unit, port, cm_val));
                }
                if (ocmc1_val != cmc1_val) {
                    BCM_IF_ERROR_RETURN
                        (WRITE_COSMASK_CPU1r(unit, cmc1_val));
                }
                
                BCM_PBMP_PORT_REMOVE(pbmp, CMIC_PORT(unit));
            } else if (soc_reg_field_valid(unit, COSMASK_CPUr, COSMASKf)) {
                if (cosq < 0) {
                    start_cos = 0;
                    end_cos = 47; 
                } else {
                    if (cosq < 48) {
                        start_cos = end_cos = cosq;
                    } else {
                        return BCM_E_PARAM;
                    }
                }

                BCM_IF_ERROR_RETURN
                    (READ_COSMASK_CPUr(unit, &cmc_val64));
                ocmc_val64 = cmc_val64;

                for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
                    COMPILER_64_MASK_CREATE(mask64, 1, loc_cos);
                    if (arg) {
                        COMPILER_64_NOT(mask64);
                        COMPILER_64_AND(cmc_val64, mask64);
                    } else {
                        COMPILER_64_OR(cmc_val64, mask64);
                    }
                }

                if (COMPILER_64_NE(ocmc_val64, cmc_val64)) {
                    BCM_IF_ERROR_RETURN
                        (WRITE_COSMASK_CPUr(unit, cmc_val64));
                }

                BCM_PBMP_PORT_REMOVE(pbmp, CMIC_PORT(unit));
            } else {
                /* Handle below */
                num_cos = 10; /* CMIC has more COS queues */
            }
        }

        if (BCM_PBMP_IS_NULL(pbmp)) {
            /* Done */
            return BCM_E_NONE;
        }

        if (cosq < 0) {
            start_cos = 0;
            end_cos = num_cos - 1;
        } else {
            if (cosq < num_cos) {
                start_cos = end_cos = cosq;
            } else {
                return BCM_E_PARAM;
            }
        }

        BCM_PBMP_ITER(pbmp, port) {
            BCM_IF_ERROR_RETURN(READ_COSMASKr(unit, port, &cm_val));
            ocm_val = cm_val;
            for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
                if (arg) {
                    /*
                     * COVERITY
                     *
                     * Max left shift of only 10.
                     * This code is intentional.
                     */
                    /* coverity[large_shift] */
                    cm_val &= ~(1 << loc_cos);
                } else {
                    /*
                     * COVERITY
                     *
                     * Max left shift of only 10.
                     * This code is intentional.
                     */
                    /* coverity[large_shift] */
                    cm_val |= (1 << loc_cos);
                }
            }
            if (ocm_val != cm_val) {
                BCM_IF_ERROR_RETURN(WRITE_COSMASKr(unit, port, cm_val));
            }
        }
        return BCM_E_NONE;

    case bcmCosqControlCongestionManagedQueue:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_queue_set(unit, gport, cosq, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_queue_set(unit, gport, cosq, arg);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_queue_set(unit, gport, cosq, arg);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_queue_set(unit, gport, cosq, arg);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionFeedbackWeight:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_quantize_set(unit, gport, cosq, arg,
                                                       -1);
            }
#endif 

#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_quantize_set(unit, gport, cosq, arg,
                                                       -1);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_quantize_set(unit, gport, cosq, arg,
                                                       -1);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_quantize_set(unit, gport, 
                                                                cosq, arg, -1);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionSetPoint:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_quantize_set(unit, gport, cosq, -1,
                                                       arg);
            }
#endif 
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_quantize_set(unit, gport, cosq, -1,
                                                       arg);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_quantize_set(unit, gport, cosq, -1,
                                                       arg);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_quantize_set(unit, gport, cosq, -1,
                                                       arg);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionSampleBytesMin:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         arg, -1);
            }
#endif 
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         arg, -1);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         arg, -1);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         arg, -1);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionSampleBytesMax:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         -1, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         -1, arg);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         -1, arg);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_sample_int_set(unit, gport, cosq,
                                                         -1, arg);
            }
#endif
        }
        break;

    case bcmCosqControlCongestionProxy:
        if (!SOC_REG_IS_VALID(unit, QCN_CNM_PRP_CTRLr)) {
            return BCM_E_UNAVAIL;
        }
        
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            return bcm_td2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
        break;

    case bcmCosqControlEgressPool:
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressUCQueueColorLimitEnable:
    case bcmCosqControlEgressUCSharedDynamicEnable:        
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
    case bcmCosqControlEgressMCSharedDynamicEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueColorLimitEnable:
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
    case bcmCosqControlIngressPortPGSharedDynamicEnable:
    case bcmCosqControlIngressPortPoolMinLimitBytes:
    case bcmCosqControlDropLimitAlpha:
    case bcmCosqControlUCDropLimitAlpha:
    case bcmCosqControlMCDropLimitAlpha:
    case bcmCosqControlIngressPool:
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
    case bcmCosqControlEgressPortPoolRedLimitBytes:
    case bcmCosqControlUCEgressPool:
    case bcmCosqControlMCEgressPool:
    case bcmCosqControlIngressPoolLimitBytes:    
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
    case bcmCosqControlIngressPortPGResetOffsetBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
    case bcmCosqControlEgressPoolSharedLimitBytes: 
    case bcmCosqControlEgressPoolResumeLimitBytes:
    case bcmCosqControlEgressPoolYellowSharedLimitBytes: 
    case bcmCosqControlEgressPoolYellowResumeLimitBytes:
    case bcmCosqControlEgressPoolRedSharedLimitBytes:
    case bcmCosqControlEgressPoolRedResumeLimitBytes:
    case bcmCosqControlEgressUCQueueGroupMinEnable:
    case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
    case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
    case bcmCosqControlEgressUCQueueGroupDropLimitAlpha:
    case bcmCosqControlObmDiscardLimit:
    case bcmCosqControlObmMinLimit:
    case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressUCQueueRedLimit:
    case bcmCosqControlEgressUCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueRedLimit:
    case bcmCosqControlEgressMCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitEntries:
    case bcmCosqControlEgressMCQueueDataBufferRedLimit:
    case bcmCosqControlEgressMCQueueEntryRedLimit:
    case bcmCosqControlEgressMCQueueDataBufferYellowLimit:
    case bcmCosqControlEgressMCQueueEntryYellowLimit:
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_control_set(unit, gport, cosq, type, arg); 
    }
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            return bcm_ap_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            return bcm_td2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
        if (SOC_IS_TD_TT(unit)) {
            return bcm_td_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlIngressPortPGMinLimitBytes:
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_control_set(unit, gport, cosq, type, arg);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)){
            return bcm_ap_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /*BCM_APACHE_SUPPORT*/
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) {
            return bcm_td2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
        break;

    case bcmCosqControlEfPropagation:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            return bcm_td2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_KATANA_SUPPORT)
        if (SOC_IS_KATANA(unit)) {
            return bcm_kt_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlRedirectQueueSharedLimitBytes:
    case bcmCosqControlRedirectQueueMinLimitBytes:
    case bcmCosqControlRedirectSharedDynamicEnable:
    case bcmCosqControlRedirectQueueLimitEnable:
    case bcmCosqControlRedirectColorDynamicEnable:
    case bcmCosqControlRedirectColorEnable:
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_set(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlEEEQueueThresholdProfileSelect:
        if (soc_feature(unit, soc_feature_eee_bb_mode)) {
            int num_cosq;

#if defined(BCM_GREYHOUND2_SUPPORT)
           if (SOC_IS_GREYHOUND2(unit)) {
               return bcmi_gh2_cosq_control_set(unit, gport, cosq, type, arg);
           }
#endif /* BCM_GREYHOUND2_SUPPORT */

            if (soc_feature(unit, soc_feature_wh2)) {
                /* Get the configured max cosq number */
                BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
            } else {
                num_cosq = NUM_COS(unit);
            }

            if ((cosq < 0) || (cosq >= num_cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
#if defined(BCM_HURRICANE2_SUPPORT)
            if (SOC_IS_HURRICANE2(unit)||SOC_IS_GREYHOUND(unit)||
                SOC_IS_HURRICANE3(unit)) {
                BCM_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, select_reg[cosq], port, 
                    EEE_THRESH_SELf, arg));
                return BCM_E_NONE;
            }
#endif /* BCM_HURRICANE2_SUPPORT */
        }
        break;
    case bcmCosqControlEEEPacketLatencyProfileSelect:
        if (soc_feature(unit, soc_feature_eee_bb_mode)) {
            int num_cosq;
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                return bcmi_gh2_cosq_control_set(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND2_SUPPORT */

            if (soc_feature(unit, soc_feature_wh2)) {
                /* Get the configured max cosq number */
                BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
            } else {
                num_cosq = NUM_COS(unit);
            }

            if ((cosq < 0) || (cosq >= num_cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }

#if defined(BCM_HURRICANE2_SUPPORT)
            if (SOC_IS_HURRICANE2(unit)||SOC_IS_GREYHOUND(unit)||
                SOC_IS_HURRICANE3(unit)) {
                BCM_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, select_reg[cosq], port, 
                    EEE_LATENCY_SELf, arg));
                return BCM_E_NONE;
            }
#endif /* BCM_HURRICANE2_SUPPORT */
        }
        break;
    case bcmCosqControlPFCBackpressureEnable:
        if (soc_feature(unit, soc_feature_priority_flow_control)) {
#if defined(BCM_GREYHOUND_SUPPORT)
            if (soc_feature(unit, soc_feature_gh_style_pfc_config)) {
                return bcm_gh_cosq_control_set(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND_SUPPORT */
        }
        break;
    case bcmCosqControlE2ECCTransmitEnable:
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_gport_e2ecc_transmit_set(unit, gport, cosq,
                                                         type, arg);
        } 
#endif /* BCM_KATANA2_SUPPORT */ 
        break;
    case bcmCosqControlAlternateStoreForward:
        if (soc_feature(unit, soc_feature_asf)) {
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                return bcmi_gh2_cosq_control_set(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
        }
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_esw_cosq_control_get
 * Purpose:
 *      Get specified feature configuration
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      type - (IN) feature
 *      arg  - (OUT) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type, int *arg)
{
    uint32 kbits_sec_min, kbits_sec_max, kbits_burst, flags;
    bcm_port_t port;
    bcm_cos_queue_t loc_cos;
    uint32 cm_val, cmc1_val;
    uint64 cmc_val64;
#if defined(BCM_HURRICANE2_SUPPORT)
    soc_reg_t select_reg[8] = { \
        EEE_PROFILE_SEL_0r, EEE_PROFILE_SEL_1r, \
        EEE_PROFILE_SEL_2r, EEE_PROFILE_SEL_3r, \
        EEE_PROFILE_SEL_4r, EEE_PROFILE_SEL_5r, \
        EEE_PROFILE_SEL_6r, EEE_PROFILE_SEL_7r};
#endif /* BCM_HURRICANE2_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            return bcm_th3_cosq_control_get(unit, gport, cosq,
                BCM_COSQ_BUFFER_ID_INVALID, type, arg);
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            return bcm_th_cosq_control_get(unit, gport, cosq,
                BCM_COSQ_BUFFER_ID_INVALID, type, arg);
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return (bcm_hr4_cosq_control_get(unit, gport, cosq, type, arg));
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return (bcm_fb6_cosq_control_get(unit, gport, cosq, type, arg));
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return (bcm_hx5_cosq_control_get(unit, gport, cosq, type, arg));
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
           return (bcm_td3_cosq_control_get(unit, gport, cosq, type, arg));
        }
#endif /* BCM_TRIDENT3_SUPPORT */

    switch (type) {
    case bcmCosqControlBandwidthBurstMax:
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_mn_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_tr2_cosq_gport_bandwidth_burst_get(unit, gport, cosq, 
                                                        &kburst_tmp, &kbits_burst));
        } else 
#endif  
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_td2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_td_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_kt_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_tr3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                        &kburst_tmp, &kbits_burst));
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            uint32 kburst_tmp;
#if defined(BCM_HURRICANE_SUPPORT)
            if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hu_cosq_gport_bandwidth_burst_get(unit, gport,
                                                           cosq, &kbits_burst));                
            } else
#endif /* BCM_HURRICANE_SUPPORT*/
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_gport_bandwidth_burst_get(unit, gport,
                                                             cosq,
                                                             &kbits_burst));
            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                           &kburst_tmp, &kbits_burst));
            }
        } else  /*SOC_IS_TR_VL*/
#endif  
        {
            if (gport < 0) {
                port = SOC_PORT_MIN(unit,all);
            } else {
                /* Must use local ports on legacy devices */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, gport, &port));
                if (!SOC_PORT_VALID(unit, port)) {
                    return BCM_E_PORT;
                }
            }

            if (cosq < 0) {
                loc_cos = 0;
            } else {
                if (cosq < NUM_COS(unit)) {
                    loc_cos = cosq;
                } else {
                    return BCM_E_PARAM;
                }
            }

            BCM_IF_ERROR_RETURN
                (mbcm_driver[unit]->
                 mbcm_cosq_port_bandwidth_get(unit, port, loc_cos,
                                              &kbits_sec_min, &kbits_sec_max,
                                              &kbits_burst, &flags));
        }
        *arg = kbits_burst & 0x7fffffff; /* Convert to int  */
        return BCM_E_NONE;

    case bcmCosqControlBandwidthBurstMin:
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        uint32 kburst_tmp;
        return bcm_mn_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                     (uint32*)arg, &kburst_tmp); 
        } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            uint32 kburst_tmp;
            return bcm_ap_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                     (uint32*)arg, &kburst_tmp);
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            uint32 kburst_tmp;
            BCM_IF_ERROR_RETURN
                (bcm_tr2_cosq_gport_bandwidth_burst_get(unit, gport, cosq, 
                                                        (uint32*)arg, &kburst_tmp));
        } else 
#endif 
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
            uint32 kburst_tmp;
            return bcm_td2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                     (uint32*)arg, &kburst_tmp);
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            uint32 kburst_tmp;
            return bcm_tr3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                     (uint32*)arg, &kburst_tmp);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
        if (SOC_IS_TD_TT(unit)) {
            uint32 kburst_tmp;
            return bcm_td_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                      (uint32*)arg, &kburst_tmp);
        } else
#endif
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            uint32 kburst_tmp;
            return bcm_kt_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                      (uint32*)arg, &kburst_tmp);
        } else 
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            uint32 kburst_tmp;
            return bcm_kt2_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                      (uint32*)arg, &kburst_tmp);
        } else 
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            uint32 kburst_tmp;
#if defined(BCM_HURRICANE_SUPPORT)
            /* coverity[dead_error_begin : FALSE] */
            if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit)) {
             /*need to support later*/
            } else   
#endif /* BCM_HURRICANE_SUPPORT*/
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                           (uint32*)arg, &kburst_tmp));
            }
        } else  /*BCM_TRIUMPH_SUPPORT*/
#endif 
        /* coverity[dead_error_begin : FALSE] */
        {
            /*do nothing*/
        }

        return BCM_E_UNAVAIL;

    case bcmCosqControlSchedulable:
        if (!soc_reg_field_valid(unit, COSMASKr, COSMASKf)) {
            return BCM_E_UNAVAIL;
        }

        if (gport < 0) {
            port = SOC_PORT_MIN(unit,all);
        } else {
            /* Must use local ports on legacy devices */
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }

        loc_cos = (cosq < 0) ? 0 : cosq;

        if (port == CMIC_PORT(unit)) {
            if (soc_reg_field_valid(unit, COSMASK_CPUr, COSMASKf)) {
                if (loc_cos > 47) {
                    return BCM_E_PARAM;
                }

                BCM_IF_ERROR_RETURN
                    (READ_COSMASK_CPUr(unit, &cmc_val64));
                *arg = COMPILER_64_BITTEST(cmc_val64, loc_cos) ?
                    FALSE : TRUE; 
            } else if (soc_reg_field_valid(unit, COSMASK_CPU1r, COSMASKf)) {
                if (loc_cos < 10) {
                    BCM_IF_ERROR_RETURN(READ_COSMASKr(unit, port, &cm_val));
                    *arg = (cm_val & (1 << loc_cos)) ? FALSE : TRUE; 
                } else if (loc_cos < 32) {
                    BCM_IF_ERROR_RETURN(READ_COSMASK_CPU1r(unit, &cmc1_val));
                    *arg = (cmc1_val & (1 << loc_cos)) ? FALSE : TRUE;
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                if (loc_cos < 10) {
                    BCM_IF_ERROR_RETURN(READ_COSMASKr(unit, port, &cm_val));
                    *arg = (cm_val & (1 << loc_cos)) ? FALSE : TRUE; 
                } else {
                    return BCM_E_PARAM;
                }
            }
        } else {
            if (cosq < NUM_COS(unit)) {
                BCM_IF_ERROR_RETURN(READ_COSMASKr(unit, port, &cm_val));
                *arg = (cm_val & (1 << loc_cos)) ? FALSE : TRUE; 
            } else {
                return BCM_E_PARAM;
            }
        }

        return BCM_E_NONE;

    case bcmCosqControlCongestionManagedQueue:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_queue_get(unit, gport, cosq, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_queue_get(unit, gport, cosq, arg);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_queue_get(unit, gport, cosq, arg);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_queue_get(unit, gport, cosq, arg);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionFeedbackWeight:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_quantize_get(unit, gport, 
                                                        cosq, arg, NULL);
            }
#endif 
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_quantize_get(unit, gport, 
                                                        cosq, arg, NULL);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_quantize_get(unit, gport, 
                                                        cosq, arg, NULL);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_quantize_get(unit, gport, 
                                                        cosq, arg, NULL);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionSetPoint:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_quantize_get(unit, gport, 
                                                            cosq, NULL, arg);
            }
#endif 
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_quantize_get(unit, gport, 
                                                            cosq, NULL, arg);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_quantize_get(unit, gport, 
                                                            cosq, NULL, arg);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_quantize_get(unit, gport, 
                                                            cosq, NULL, arg);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionSampleBytesMin:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         arg, NULL);
            }
#endif 
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         arg, NULL);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         arg, NULL);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         arg, NULL);
            }
#endif
        }
        break;
    case bcmCosqControlCongestionSampleBytesMax:
        if (soc_feature(unit, soc_feature_qcn)) {
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         NULL, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         NULL, arg);
            }
#endif
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         NULL, arg);
            }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit)) {
                return bcm_tr3_cosq_congestion_sample_int_get(unit, gport, cosq,
                                                         NULL, arg);
            }
#endif
        }
        break;
    case bcmCosqControlEgressPool:
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressUCQueueColorLimitEnable:
    case bcmCosqControlEgressUCSharedDynamicEnable:        
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
    case bcmCosqControlEgressMCSharedDynamicEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueColorLimitEnable:
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
    case bcmCosqControlIngressPortPGSharedDynamicEnable:
    case bcmCosqControlIngressPortPoolMinLimitBytes:
    case bcmCosqControlDropLimitAlpha:      
    case bcmCosqControlUCDropLimitAlpha:      
    case bcmCosqControlMCDropLimitAlpha:      
    case bcmCosqControlSPPortLimitState:
    case bcmCosqControlPGPortLimitState:
    case bcmCosqControlPGPortXoffState:
    case bcmCosqControlPoolRedDropState:
    case bcmCosqControlPoolYellowDropState:
    case bcmCosqControlPoolGreenDropState:
    case bcmCosqControlIngressPool:
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
    case bcmCosqControlEgressPortPoolRedLimitBytes:
    case bcmCosqControlUCEgressPool:
    case bcmCosqControlMCEgressPool:
    case bcmCosqControlIngressPoolLimitBytes:    
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
    case bcmCosqControlIngressPortPGResetOffsetBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
    case bcmCosqControlEgressPoolSharedLimitBytes: 
    case bcmCosqControlEgressPoolResumeLimitBytes:
    case bcmCosqControlEgressPoolYellowSharedLimitBytes: 
    case bcmCosqControlEgressPoolYellowResumeLimitBytes:
    case bcmCosqControlEgressPoolRedSharedLimitBytes:
    case bcmCosqControlEgressPoolRedResumeLimitBytes:
    case bcmCosqControlEgressUCQueueGroupMinEnable:
    case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
    case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
    case bcmCosqControlEgressUCQueueGroupDropLimitAlpha:
    case bcmCosqControlObmDiscardLimit:
    case bcmCosqControlObmMinLimit:
    case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressUCQueueRedLimit:
    case bcmCosqControlEgressUCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueRedLimit:
    case bcmCosqControlEgressMCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitEntries:
    case bcmCosqControlEgressMCQueueDataBufferRedLimit:
    case bcmCosqControlEgressMCQueueEntryRedLimit:
    case bcmCosqControlEgressMCQueueDataBufferYellowLimit:
    case bcmCosqControlEgressMCQueueEntryYellowLimit:
#ifdef BCM_MONTEREY_SUPPORT
       if (SOC_IS_MONTEREY(unit)) {
           return bcm_mn_cosq_control_get(unit, gport, cosq, type, arg); 
       }
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif
        if (SOC_IS_TD_TT(unit)) {
#if defined(BCM_TRIDENT_SUPPORT)
            if (SOC_IS_TD_TT(unit)) {
                return bcm_td_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif
        }
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlCongestionProxy:
            if (!SOC_REG_IS_VALID(unit, QCN_CNM_PRP_CTRLr)) {
                return BCM_E_UNAVAIL;
            }
#ifdef BCM_MONTEREY_SUPPORT
       if (SOC_IS_MONTEREY(unit)) {
           return bcm_mn_cosq_control_get(unit, gport, cosq, type, arg);
       }
#endif /* BCM_MONTEREY_SUPPORT */

#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                return bcm_td2_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif
        break;
        
    case bcmCosqControlIngressPortPGMinLimitBytes:
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
       if (SOC_IS_MONTEREY(unit)) {
           return bcm_mn_cosq_control_get(unit, gport, cosq, type, arg);
       }
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)){
            return bcm_ap_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_APACHE_SUPPORT*/
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) {
            return bcm_td2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif
        break;
    case bcmCosqControlEfPropagation:
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            return bcm_td2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            return bcm_kt_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlEEEQueueThresholdProfileSelect:
        if (soc_feature(unit, soc_feature_eee_bb_mode)) {
            int num_cosq;
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                return bcmi_gh2_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND2_SUPPORT */

            if (soc_feature(unit, soc_feature_wh2)) {
                /* Get the configured max cosq number */
                BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
            } else {
                num_cosq = NUM_COS(unit);
            }

            if ((cosq < 0) || (cosq >= num_cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }

#if defined(BCM_HURRICANE2_SUPPORT)
            if (SOC_IS_HURRICANE2(unit)||SOC_IS_GREYHOUND(unit)||
                SOC_IS_HURRICANE3(unit)) {
                uint32 rval = 0;

                BCM_IF_ERROR_RETURN(
                    soc_reg32_get(unit, select_reg[cosq], port, 0, &rval));
                *arg = soc_reg_field_get(unit, select_reg[cosq], 
                           rval, EEE_THRESH_SELf);
                return BCM_E_NONE;
            }
#endif /* BCM_HURRICANE2_SUPPORT */
        }
        break;
    case bcmCosqControlEEEPacketLatencyProfileSelect:
        if (soc_feature(unit, soc_feature_eee_bb_mode)) {
            int num_cosq;
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                return bcmi_gh2_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND2_SUPPORT */

            if (soc_feature(unit, soc_feature_wh2)) {
                /* Get the configured max cosq number */
                BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
            } else {
                num_cosq = NUM_COS(unit);
            }

            if ((cosq < 0) || (cosq >= num_cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }

#if defined(BCM_HURRICANE2_SUPPORT)
            if (SOC_IS_HURRICANE2(unit)||SOC_IS_GREYHOUND(unit)||
                SOC_IS_HURRICANE3(unit)) {
                uint32 rval = 0;

                BCM_IF_ERROR_RETURN(
                    soc_reg32_get(unit, select_reg[cosq], port, 0, &rval));
                *arg = soc_reg_field_get(unit, select_reg[cosq], 
                           rval, EEE_LATENCY_SELf);
                return BCM_E_NONE;
            }
#endif /* BCM_HURRICANE2_SUPPORT */
        }
        break;
    case bcmCosqControlPortQueueUcast:
    case bcmCosqControlPortQueueMcast:
#ifdef BCM_MONTEREY_SUPPORT
       if (SOC_IS_MONTEREY(unit)) {
           return bcm_mn_cosq_control_get(unit, gport, cosq, type, arg);
       }
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                return bcm_ap_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            return bcm_td2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            return bcm_tr3_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            return bcm_kt_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif

        if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
            return BCM_E_PARAM;
        }

        if (arg == NULL) {
            return BCM_E_PARAM;
        }

        *arg = cosq;
        break;
    case bcmCosqControlRedirectQueueSharedLimitBytes:
    case bcmCosqControlRedirectQueueMinLimitBytes:
    case bcmCosqControlRedirectSharedDynamicEnable:
    case bcmCosqControlRedirectQueueLimitEnable:
    case bcmCosqControlRedirectColorDynamicEnable:
    case bcmCosqControlRedirectColorEnable:
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_control_get(unit, gport, cosq, type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlPFCBackpressureEnable:
        if (soc_feature(unit, soc_feature_priority_flow_control)) {
#if defined(BCM_GREYHOUND_SUPPORT)
            if (soc_feature(unit, soc_feature_gh_style_pfc_config)) {
                return bcm_gh_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND_SUPPORT */
        }
        break;
    case bcmCosqControlE2ECCTransmitEnable:
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_gport_e2ecc_transmit_get(unit, gport, cosq,
                                                         type, arg);
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case bcmCosqControlAlternateStoreForward:
        if (soc_feature(unit, soc_feature_asf)) {
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                return bcmi_gh2_cosq_control_get(unit, gport, cosq, type, arg);
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
        }
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_control_extended_set
 * Purpose:
 *      Set specified feature configuration
 *
 * Parameters:
 *      unit    - (IN) Unit number
 *      id      - (IN) Integrated cosq index
 *      control - (IN) Integrated cosq control data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_control_extended_set(
    int unit, 
    bcm_cosq_object_id_t *id,
    bcm_cosq_control_data_t *control)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_control_set(unit, 
            id->port, id->cosq, id->buffer, 
            control->type, control->arg);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_control_set(unit, 
            id->port, id->cosq, id->buffer, 
            control->type, control->arg);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_control_extended_get
 * Purpose:
 *      Get specified feature configuration
 *
 * Parameters:
 *      unit    - (IN) Unit number
 *      id      - (IN) Integrated cosq index
 *      control - (INOUT) Integrated cosq control data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_control_extended_get(
    int unit, 
    bcm_cosq_object_id_t *id,
    bcm_cosq_control_data_t *control)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_control_get(unit, 
            id->port, id->cosq, id->buffer, 
            control->type, &(control->arg));
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th_cosq_control_get(unit, 
            id->port, id->cosq, id->buffer, 
            control->type, &(control->arg));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return BCM_E_UNAVAIL;
}


STATIC int _bcm_esw_cosq_num_validate(int cosq, int max_cosq_per_port)
{
    
    if (((cosq >= 0) && (cosq < max_cosq_per_port)) || 
        (cosq == BCM_COS_INVALID)) {
        return BCM_E_NONE;
    }

    return BCM_E_PARAM;
}

/*
 * Function:
 *      _bcm_cosq_stat_get
 * Purpose:
 *      Get MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Triumph and Triumph2 have the following MMU drop counters:
 *        - Per cosq packet drop:            DROP_PKT_CNT.
 *        - Per cosq byte drop:              DROP_BYTE_CNT.
 *        - Additional per port packet drop: DROP_PKT_CNT_ING.
 *        - Additional per port byte drop:   DROP_BYTE_CNT_ING.
 *        - Per port yellow packet drop:     DROP_PKT_CNT_YEL.
 *        - Per port red packet drop:        DROP_PKT_CNT_RED.
 *
 *      Scorpion has the following MMU drop counters:
 *        - Per cosq packet drop:            HOLDROP_PKT_CNT.
 *        - Additional per port packet drop: IBP_DROP_PKT_CNT, CFAP_DROP_PKT_CNT.
 *        - Per port yellow packet drop:     YELLOW_CNG_DROP_CNT.
 *        - Per port red packet drop:        RED_CN_DROP_CNT.
 *
 *      Bradley has the following MMU drop counters:
 *        - Per port packet drop:            DROP_PKT_CNT.
 *
 *      Firebolt and Hurricane have the following MMU drop counters:
 *        - Per port packet drop:            EGRDROPPKTCOUNT.
 *        - Per port yellow packet drop:     CNGDROPCOUNT1.
 *        - Per port red packet drop:        CNGDROPCOUNT0.
 */
int 
_bcm_esw_cosq_stat_get(
                       int unit, 
                       bcm_gport_t gport, 
                       bcm_cos_queue_t cosq, 
                       bcm_cosq_stat_t stat, 
                       int sync_mode, 
                       uint64 *value)
{
    int max_cosq_per_port;
    bcm_port_t local_port;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
   if (SOC_IS_MONTEREY(unit)) {
       return bcm_mn_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
   }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return bcm_kt_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_stat_get(unit, gport, cosq, stat, sync_mode,
                                      value);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
         SOC_IS_TR_VL(unit) || SOC_IS_SC_CQ(unit) ||
         SOC_IS_HB_GW(unit) || SOC_IS_FB_FX_HX(unit)) {
        max_cosq_per_port = 0;
    } else {
        return BCM_E_UNAVAIL;
    }


    if (value == NULL) {
        return BCM_E_PARAM;
    }        

    /*
     * if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;

    COMPILER_64_ZERO(*value);

    switch (stat) {
        case bcmCosqStatDroppedPackets:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit) && !(SOC_IS_HURRICANE(unit))) {
                uint64 val64;
                int i, rv;
                BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
                if (soc_feature(unit, soc_feature_wh2)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
                } else {
                    max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                            NUM_CPU_COSQ(unit) : NUM_COS(unit);
                }
                BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
                if (cosq == BCM_COS_INVALID) {
                    for (i = 0; i < max_cosq_per_port; i++) {
                        rv = counter_get(unit, local_port, 
                                SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, i, &val64);
                        if (SOC_SUCCESS(rv)) {
                            COMPILER_64_ADD_64(*value, val64);
                        } else {
                            break;
                        }
                    }
                } else {
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port, 
                        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, cosq, value));
                } 
            } else 
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_SCORPION_SUPPORT
            if (SOC_IS_SC_CQ(unit)) {
                uint64 val64;
                int i, rv;
                BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? NUM_CPU_COSQ(unit) : NUM_COS(unit);
                BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
                if (cosq == BCM_COS_INVALID) {
                    for (i = 0; i < max_cosq_per_port; i++) {
                        rv = counter_get(unit, local_port, 
                                SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, i, &val64);
                        if (SOC_SUCCESS(rv)) {
                            COMPILER_64_ADD_64(*value, val64);
                        } else {
                            break;
                        }
                    }

                    /* Also add per port IBP_DROP_PKT_CNT */
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_IBP, 0, &val64));
                    COMPILER_64_ADD_64(*value, val64);

                    /* Also add per port CFAP_DROP_PKT_CNT */
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_CFAP, 0, &val64));
                    COMPILER_64_ADD_64(*value, val64);
                } else {
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port, 
                        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, cosq, value));
                } 
            } else 
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
            if (SOC_IS_HB_GW(unit)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? NUM_CPU_COSQ(unit) : NUM_COS(unit);
                BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
                /* Only the per port DROP_PKT_CNT is available */
                if (cosq != BCM_COS_INVALID) {
                    return BCM_E_UNAVAIL;
                }
                SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                       SOC_COUNTER_NON_DMA_PORT_DROP_PKT, 0, value));
            } else 
#endif /* BCM_BRADLEY_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
            if (SOC_IS_FB_FX_HX(unit) || SOC_IS_HURRICANE(unit)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? NUM_CPU_COSQ(unit) : NUM_COS(unit);
                BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port)); 
                /* Only the per port EGRDROPPKTCOUNT is available */
                if (cosq != BCM_COS_INVALID) {
                    return BCM_E_UNAVAIL;
                }
                _bcm_stat_counter_extra_get(unit, EGRDROPPKTCOUNTr,
                    local_port, value);
            } else 
#endif /* BCM_FIREBOLT_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }

            break;

        case bcmCosqStatDroppedBytes:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit) && 
                !(SOC_IS_HURRICANEX(unit)||SOC_IS_GREYHOUND(unit) ||
                SOC_IS_GREYHOUND2(unit))) {
                uint64 val64;
                int i, rv;
                BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? NUM_CPU_COSQ(unit) : NUM_COS(unit);
                BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
                if (cosq == BCM_COS_INVALID) {
                    for (i = 0; i < max_cosq_per_port; i++) {
                        rv = counter_get(unit, local_port, 
                                SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, i, &val64);
                        if (SOC_SUCCESS(rv)) {
                            COMPILER_64_ADD_64(*value, val64);
                        } else {
                            break;
                        }
                    }

                    /* Also add the per port DROP_BYTE_CNT_ING */
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING, 0, &val64));
                    COMPILER_64_ADD_64(*value, val64);
                } else {
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port, 
                        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, cosq, value));
                } 
            } else 
#endif /* BCM_TRIUMPH_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmCosqStatYellowCongestionDroppedPackets:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
            if (soc_feature(unit, soc_feature_wh2)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
            } else {
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                        NUM_CPU_COSQ(unit) : NUM_COS(unit);
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
            if (cosq != BCM_COS_INVALID) {
                /* Yellow dropped packet counters are available only on a per
                 * port basis.
                 */
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0, value));
            break;

        case bcmCosqStatRedCongestionDroppedPackets:
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
            if (soc_feature(unit, soc_feature_wh2)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
            } else {
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                        NUM_CPU_COSQ(unit) : NUM_COS(unit);
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
            if (cosq != BCM_COS_INVALID) {
                /* Red dropped packet counters are available only on a per
                 * port basis.
                 */
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, 0, value));
            break;

        case bcmCosqStatOutPackets:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit)) {
                uint64 val64;
                int i, rv;
                BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
                if (soc_feature(unit, soc_feature_wh2)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
                } else {
                    max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                            NUM_CPU_COSQ(unit) : NUM_COS(unit);
                }
                BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
                COMPILER_64_ZERO(*value);
                if (cosq == BCM_COS_INVALID) {
                    for (i = 0; i < max_cosq_per_port; i++) {
                        rv = counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, i, &val64);
                        if (SOC_SUCCESS(rv)) {
                            COMPILER_64_ADD_64(*value, val64);
                        } else {
                            break;
                        }
                    }
                } else {
                    SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                                                        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, cosq, value));
                }
             
            } else 
#endif
            {
                return BCM_E_UNAVAIL;
            }
        break;

        case bcmCosqStatOutBytes:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit)) {
            uint64 val64;
            int i, rv;
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
            if (soc_feature(unit, soc_feature_wh2)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
            } else {
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                        NUM_CPU_COSQ(unit) : NUM_COS(unit);
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
            COMPILER_64_ZERO(*value);
            if (cosq == BCM_COS_INVALID) {
                for (i = 0; i < max_cosq_per_port; i++) {
                    rv = counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, i, &val64);
                    if (SOC_SUCCESS(rv)) {
                        COMPILER_64_ADD_64(*value, val64);
                    } else {
                        break;
                    }
                }
            } else {
                SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                                                    SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, cosq, value));
            }
        } else 
#endif
        {
            return BCM_E_UNAVAIL;
        }
        break;

        case bcmCosqStatDiscardDroppedPackets:
#if defined(BCM_GREYHOUND_SUPPORT)
        if (soc_feature(unit, soc_feature_wred_drop_counter_per_port)) {

            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
            if (soc_feature(unit, soc_feature_wh2)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
            } else {
                max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                        NUM_CPU_COSQ(unit) : NUM_COS(unit);
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
            COMPILER_64_ZERO(*value);
            if (cosq == BCM_COS_INVALID) {
                SOC_IF_ERROR_RETURN(soc_counter_get(unit, local_port, 
                            SOC_COUNTER_NON_DMA_PORT_WRED_DROP_PKT, 0, value));
                
            } else {
                return BCM_E_UNAVAIL;
            }
        } else 
#endif
        {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmCosqStatHOLDropPackets:
#if defined(BCM_HURRICANE2_SUPPORT)
    if (soc_feature(unit,soc_feature_cosq_hol_drop_packet_count)) {
        bcm_port_t selected_port;
        uint32 rval;
        int i;
        static soc_reg_t hold_reg[] = {
            HOLD_COS0r, HOLD_COS1r, HOLD_COS2r, HOLD_COS3r,
            HOLD_COS4r, HOLD_COS5r, HOLD_COS6r, HOLD_COS7r,
        };
    
        if (!SOC_REG_IS_VALID(unit, HOLD_COS0r)) {
            return BCM_E_UNAVAIL;
        }
        if (!SOC_REG_IS_VALID(unit, HOLD_COS_PORT_SELECTr)) {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
        if (soc_feature(unit, soc_feature_wh2)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_cosq_config_get(unit, &max_cosq_per_port));
        } else {
            max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                    NUM_CPU_COSQ(unit) : NUM_COS(unit);
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));
        COMPILER_64_ZERO(*value);
    
        SOC_IF_ERROR_RETURN(READ_HOLD_COS_PORT_SELECTr(unit, &rval)); 
        selected_port = soc_reg_field_get(unit, HOLD_COS_PORT_SELECTr, rval,
                            PORT_NUMf);
    
        if (selected_port != local_port) {
            return BCM_E_PORT;
        }
        if (cosq == BCM_COS_INVALID) {
            for (i = 0; i < COUNTOF(hold_reg); i++) {
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, hold_reg[i], REG_PORT_ANY, 0, &rval));
                COMPILER_64_ADD_32(*value, rval);
           }
        } else {
            if (cosq >= COUNTOF(hold_reg)) {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, hold_reg[cosq], REG_PORT_ANY, 0, &rval));
            COMPILER_64_ADD_32(*value, rval);
        }
    } else 
#endif
    {
    return BCM_E_UNAVAIL;
    }
break;
            
        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_stat_get
 * Purpose:
 *      Get MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Triumph and Triumph2 have the following MMU drop counters:
 *        - Per cosq packet drop:            DROP_PKT_CNT.
 *        - Per cosq byte drop:              DROP_BYTE_CNT.
 *        - Additional per port packet drop: DROP_PKT_CNT_ING.
 *        - Additional per port byte drop:   DROP_BYTE_CNT_ING.
 *        - Per port yellow packet drop:     DROP_PKT_CNT_YEL.
 *        - Per port red packet drop:        DROP_PKT_CNT_RED.
 *
 *      Scorpion has the following MMU drop counters:
 *        - Per cosq packet drop:            HOLDROP_PKT_CNT.
 *        - Additional per port packet drop: IBP_DROP_PKT_CNT, CFAP_DROP_PKT_CNT.
 *        - Per port yellow packet drop:     YELLOW_CNG_DROP_CNT.
 *        - Per port red packet drop:        RED_CN_DROP_CNT.
 *
 *      Bradley has the following MMU drop counters:
 *        - Per port packet drop:            DROP_PKT_CNT.
 *
 *      Firebolt and Hurricane have the following MMU drop counters:
 *        - Per port packet drop:            EGRDROPPKTCOUNT.
 *        - Per port yellow packet drop:     CNGDROPCOUNT1.
 *        - Per port red packet drop:        CNGDROPCOUNT0.
 */
int 
bcm_esw_cosq_stat_get(
                      int unit, 
                      bcm_gport_t gport, 
                      bcm_cos_queue_t cosq, 
                      bcm_cosq_stat_t stat, 
                      uint64 *value)
{
    int sync_mode = 0;

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    return _bcm_esw_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_stat_get32
 * Purpose:
 *      Get MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_cosq_stat_get32(
                        int unit, 
                        bcm_gport_t gport, 
                        bcm_cos_queue_t cosq, 
                        bcm_cosq_stat_t stat, 
                        uint32 *value)
{
    uint64 val64;
    int rv;

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    COMPILER_64_ZERO(val64);
    rv = bcm_esw_cosq_stat_get(unit, gport, cosq, stat, &val64);

    if (BCM_SUCCESS(rv)) {
        *value = COMPILER_64_LO(val64);
    }

    return rv;
}

/*
 * Function:
 *      bcm_cosq_stat_sync_get
 * Purpose:
 *      Get MMU statistics on a per port per cosq basis.
 *      The software cached count is synced with the hardware 
 *      value. 
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Triumph and Triumph2 have the following MMU drop counters:
 *        - Per cosq packet drop:            DROP_PKT_CNT.
 *        - Per cosq byte drop:              DROP_BYTE_CNT.
 *        - Additional per port packet drop: DROP_PKT_CNT_ING.
 *        - Additional per port byte drop:   DROP_BYTE_CNT_ING.
 *        - Per port yellow packet drop:     DROP_PKT_CNT_YEL.
 *        - Per port red packet drop:        DROP_PKT_CNT_RED.
 *
 *      Scorpion has the following MMU drop counters:
 *        - Per cosq packet drop:            HOLDROP_PKT_CNT.
 *        - Additional per port packet drop: IBP_DROP_PKT_CNT, CFAP_DROP_PKT_CNT.
 *        - Per port yellow packet drop:     YELLOW_CNG_DROP_CNT.
 *        - Per port red packet drop:        RED_CN_DROP_CNT.
 *
 *      Bradley has the following MMU drop counters:
 *        - Per port packet drop:            DROP_PKT_CNT.
 *
 *      Firebolt and Hurricane have the following MMU drop counters:
 *        - Per port packet drop:            EGRDROPPKTCOUNT.
 *        - Per port yellow packet drop:     CNGDROPCOUNT1.
 *        - Per port red packet drop:        CNGDROPCOUNT0.
 */
int 
bcm_esw_cosq_stat_sync_get(
                           int unit, 
                           bcm_gport_t gport, 
                           bcm_cos_queue_t cosq, 
                           bcm_cosq_stat_t stat, 
                           uint64 *value)
{
    int sync_mode = 1;

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    return _bcm_esw_cosq_stat_get(unit, gport, cosq, stat, sync_mode, value);
}

/*
 * Function:
 *      bcm_esw_cosq_stat_sync_multi_get
 * Purpose:
 *      Get multiple MMU statistics on a per port per cosq basis.
 *      The software cached count is synced with the hardware
 *      value.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of statistics types defined in bcm_cos_queue_t
 *      value_arr - (OUT) Array of collected statistics values
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_stat_sync_multi_get(
                           int unit,
                           bcm_gport_t gport,
                           bcm_cos_queue_t cosq,
                           int nstat,
                           bcm_cosq_stat_t *stat_arr,
                           uint64 *value_arr)
{
    int sync_mode = 1;
    int stix;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_cosq_stat_get(unit, gport, cosq, stat_arr[stix],
                sync_mode,  &(value_arr[stix])));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_stat_get32
 * Purpose:
 *      Get MMU statistics on a per port per cosq basis.
 *      The software cached count is synced with the hardware 
 *      value. 
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_cosq_stat_sync_get32(
                             int unit, 
                             bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, 
                             bcm_cosq_stat_t stat, 
                             uint32 *value)
{
    uint64 val64;
    int rv;

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    COMPILER_64_ZERO(val64);
    rv = bcm_esw_cosq_stat_sync_get(unit, gport, cosq, stat, &val64);

    if (BCM_SUCCESS(rv)) {
        *value = COMPILER_64_LO(val64);
    }

    return rv;
}
/*

 * Function:
 *      bcm_esw_cosq_stat_sync_multi_get32
 * Purpose:
 *      Get multiple MMU statistics on a per port per cosq basis.
 *      The software cached count is synced with the hardware
 *      value.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Array of statistics types defined in bcm_cos_queue_t
 *      value_arr - (OUT) Array of collected statistics values
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_stat_sync_multi_get32(
                           int unit,
                           bcm_gport_t gport,
                           bcm_cos_queue_t cosq,
                           int nstat,
                           bcm_cosq_stat_t *stat_arr,
                           uint32 *value_arr)
{
    int sync_mode = 1;
    int stix;
    uint64 val64;

    if (nstat <= 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

    for (stix = 0; stix < nstat; stix++) {
        COMPILER_64_ZERO(val64);
        BCM_IF_ERROR_RETURN
            (_bcm_esw_cosq_stat_get(unit, gport, cosq, stat_arr[stix],
                sync_mode, &val64));
        value_arr[stix] = COMPILER_64_LO(val64);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_stat_set
 * Purpose:
 *      Set MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be set.
 *      value - (IN) Statistic value to be set.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_cosq_stat_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint64 value)
{
    int max_cosq_per_port;
    bcm_port_t local_port;
    int cosq_allowed=0;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
   if (SOC_IS_MONTEREY(unit)) {
       return bcm_ap_cosq_stat_set(unit, gport, cosq, stat, value); 
   }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        return bcm_fb6_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return bcm_td_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            return bcm_kt_cosq_stat_set(unit, gport, cosq, stat, value);
        }
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            return bcm_kt2_cosq_stat_set(unit, gport, cosq, stat, value);
        }
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_stat_set(unit, gport, cosq, stat, value);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
         SOC_IS_TR_VL(unit) || SOC_IS_SC_CQ(unit) ||
         SOC_IS_HB_GW(unit) || SOC_IS_FB_FX_HX(unit)) {
        max_cosq_per_port = 0;
    } else {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));

    if (soc_feature(unit, soc_feature_wh2)) {
        BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit,
                                                    &max_cosq_per_port));
    } else {
        max_cosq_per_port = IS_CPU_PORT(unit, local_port) ? \
                                NUM_CPU_COSQ(unit) : NUM_COS(unit);
    }

    if (((cosq >= 0) && (cosq < max_cosq_per_port)) ||
        (cosq == BCM_COS_INVALID)) {
        cosq_allowed = 1;
    }

    if (!cosq_allowed) {
        return BCM_E_PARAM;
    }

    switch (stat) {
        case bcmCosqStatDroppedPackets:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit) && !(SOC_IS_HURRICANE(unit))) {
                uint64 val64_zero;
                int i, rv;
                COMPILER_64_ZERO(val64_zero);
                if (SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND(unit) ||
                    SOC_IS_HURRICANE2(unit)) {
                    /* Registers are read-only */
                    return BCM_E_UNAVAIL;
                }
                if (cosq == BCM_COS_INVALID) {
                    /* Write given value to first COS queue */
                    rv = soc_counter_set(unit, local_port, 
                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, 0, value);

                    /* Write zero to all other COS queues */
                    for (i = 1; i < max_cosq_per_port; i++) {
                        rv = soc_counter_set(unit, local_port, 
                                SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, i, val64_zero);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                    }
                } else {
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port, 
                        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, cosq, value));
                } 
            } else 
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_SCORPION_SUPPORT
            if (SOC_IS_SC_CQ(unit)) {
                uint64 val64_zero;
                int i, rv;

                COMPILER_64_ZERO(val64_zero);
                if (cosq == BCM_COS_INVALID) {
                    /* Write given value to first COS queue */
                    rv = soc_counter_set(unit, local_port, 
                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, 0, value);

                    /* Write zero to all other COS queues */
                    for (i = 1; i < max_cosq_per_port; i++) {
                        rv = soc_counter_set(unit, local_port, 
                                SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, i, val64_zero);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                    }

                    /* Also write zero to per port IBP_DROP_PKT_CNT */
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port,
                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_IBP, 0, val64_zero));

                    /* Also write zero to per port CFAP_DROP_PKT_CNT */
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port,
                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_CFAP, 0, val64_zero));
                } else {
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port, 
                        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, cosq, value));
                } 
            } else 
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
            if (SOC_IS_HB_GW(unit)) {
                /* Only the per port DROP_PKT_CNT is available */
                if (cosq != BCM_COS_INVALID) {
                    return BCM_E_UNAVAIL;
                }
                SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port,
                       SOC_COUNTER_NON_DMA_PORT_DROP_PKT, 0, value));
            } else 
#endif /* BCM_BRADLEY_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
            if (SOC_IS_FB_FX_HX(unit) || SOC_IS_HURRICANE(unit)) {
                /* EGRDROPPKTCOUNT is read-only */
                return BCM_E_UNAVAIL;
            } else 
#endif /* BCM_FIREBOLT_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }

            break;

        case bcmCosqStatDroppedBytes:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit) && 
                !(SOC_IS_HURRICANEX(unit)||SOC_IS_GREYHOUND(unit))) {
                uint64 val64_zero;
                int i, rv;

                COMPILER_64_ZERO(val64_zero);
                if (cosq == BCM_COS_INVALID) {
                    /* Write given value to first COS queue */
                    rv = soc_counter_set(unit, local_port, 
                            SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, 0, value);

                    /* Write zero to all other COS queues */
                    for (i = 1; i < max_cosq_per_port; i++) {
                        rv = soc_counter_set(unit, local_port, 
                                SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, i, val64_zero);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                    }

                    /* Also write zero to per port DROP_BYTE_CNT_ING */
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port,
                        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING, 0, val64_zero));
                } else {
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port, 
                        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, cosq, value));
                } 
            } else 
#endif /* BCM_TRIUMPH_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
            break;

        case bcmCosqStatYellowCongestionDroppedPackets:
            if (cosq != BCM_COS_INVALID) {
                /* Yellow dropped packet counters are available only on a per
                 * port basis.
                 */
                return BCM_E_UNAVAIL;
            }
            if (SOC_IS_FB_FX_HX(unit) || SOC_IS_HURRICANEX(unit) ||
                SOC_IS_GREYHOUND(unit)) {
                /* CNGDROPCOUNT1 is read-only */
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port,
                SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0, value));
            break;

        case bcmCosqStatRedCongestionDroppedPackets:
            if (cosq != BCM_COS_INVALID) {
                /* Red dropped packet counters are available only on a per
                 * port basis.
                 */
                return BCM_E_UNAVAIL;
            }
            if (SOC_IS_FB_FX_HX(unit) || SOC_IS_HURRICANEX(unit) ||
                SOC_IS_GREYHOUND(unit)) {
                /* CNGDROPCOUNT0 is read-only */
                return BCM_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port,
                SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, 0, value));
            break;
        case bcmCosqStatOutPackets:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit)) {
                uint64 val64_zero;
                int i, rv;

                COMPILER_64_ZERO(val64_zero);
                if (cosq == BCM_COS_INVALID) {
                    /* Write given value to first COS queue */
                    rv = soc_counter_set(unit, local_port, 
                            SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, 0, value);

                    /* Write zero to all other COS queues */
                    for (i = 1; i < max_cosq_per_port; i++) {
                        rv = soc_counter_set(unit, local_port, 
                                SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, i, val64_zero);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                    }
                } else {
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port, 
                        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, cosq, value));
                } 

            } else 
#endif
            {
                return BCM_E_UNAVAIL;
            }
        break;

        case bcmCosqStatOutBytes:
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_IS_TR_VL(unit)) {
                uint64 val64_zero;
                int i, rv;

                COMPILER_64_ZERO(val64_zero);
                if (cosq == BCM_COS_INVALID) {
                    /* Write given value to first COS queue */
                    rv = soc_counter_set(unit, local_port, 
                            SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, 0, value);

                    /* Write zero to all other COS queues */
                    for (i = 1; i < max_cosq_per_port; i++) {
                        rv = soc_counter_set(unit, local_port, 
                                SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, i, val64_zero);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                    }
                } else {
                    SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port, 
                        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, cosq, value));
                } 

            } else 
#endif
            {
                return BCM_E_UNAVAIL;
            }
        break;		

        case bcmCosqStatDiscardDroppedPackets:
#if defined(BCM_GREYHOUND_SUPPORT)
        if (soc_feature(unit, soc_feature_wred_drop_counter_per_port)) {
            
            if (cosq == BCM_COS_INVALID) {
                SOC_IF_ERROR_RETURN(soc_counter_set(unit, local_port, 
                            SOC_COUNTER_NON_DMA_PORT_WRED_DROP_PKT, 0, value));
            } else {
                return BCM_E_UNAVAIL;
            }
        } else 
#endif
        {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmCosqStatHOLDropPackets:
#if defined(BCM_HURRICANE2_SUPPORT)
        if (soc_feature(unit,soc_feature_cosq_hol_drop_packet_count)) {
            int i;
            static soc_reg_t hold_reg[] = {
                HOLD_COS0r, HOLD_COS1r, HOLD_COS2r, HOLD_COS3r,
                HOLD_COS4r, HOLD_COS5r, HOLD_COS6r, HOLD_COS7r,
            };
        
            if (!SOC_REG_IS_VALID(unit, HOLD_COS0r)) {
                return BCM_E_UNAVAIL;
            }
        
            if (!SOC_REG_IS_VALID(unit, HOLD_COS_PORT_SELECTr)) {
                return BCM_E_UNAVAIL;
            }

            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, max_cosq_per_port));

            soc_reg_field32_modify(unit, HOLD_COS_PORT_SELECTr,
                                REG_PORT_ANY,
                                PORT_NUMf, local_port);
        
            if (cosq == BCM_COS_INVALID) {
                for (i = 0; i < COUNTOF(hold_reg); i++) {
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, hold_reg[i], 
                       REG_PORT_ANY, 0, COMPILER_64_LO(value)));
                }
            } else {
                if (cosq >= COUNTOF(hold_reg)) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, hold_reg[cosq], 
                       REG_PORT_ANY, 0, COMPILER_64_LO(value)));
            }
        } else 
#endif
        {
        return BCM_E_UNAVAIL;
        }
break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_stat_set32
 * Purpose:
 *      Set MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be set.
 *      value - (IN) Statistic value to be set.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_cosq_stat_set32(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_stat_t stat, 
    uint32 value)
{
    uint64 val64;

    COMPILER_64_SET(val64, 0, value);
    return bcm_esw_cosq_stat_set(unit, gport, cosq, stat, val64);
}

/*
 * Function:
 *      bcm_esw_cosq_subport_flow_control_set
 * Purpose:
 *      Set a port or node (L0, L1, L2) to flow control when
 *      a CoE control frame is received on a subtended port.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      subport         - (IN) Subtended port
 *      sched_port      - (IN) Node (Port, L0, L1, L2) to flow control 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_cosq_subport_flow_control_set(    
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t sched_port)
{

#ifdef BCM_HGPROXY_COE_SUPPORT 
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        soc_feature(unit, soc_feature_channelized_switching)) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
           return  bcm_ap_cosq_subport_flow_control_set(unit, subport, sched_port);
        }  
#endif 
        if (SOC_IS_TRIDENT2PLUS(unit)) {
            bcm_td2p_cosq_subport_flow_control_set(unit, subport, sched_port);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_HGPROXY_COE_SUPPORT */
   {
        return BCM_E_UNAVAIL;
   }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_cosq_subport_flow_control_get
 * Purpose:
 *      Get a port or node (L0, L1, L2) that's flow controlled when
 *      a CoE control frame is received on a subtended port.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      subport         - (IN) Subtended port
 *      sched_port      - (IN) Node (Port, L0, L1, L2) being flow controlled 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_cosq_subport_flow_control_get(    
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t * sched_port)
{
#ifdef BCM_HGPROXY_COE_SUPPORT 
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        soc_feature(unit, soc_feature_channelized_switching)) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            return bcm_ap_cosq_subport_flow_control_get(unit, subport, sched_port);
        } 
#endif         
        if (SOC_IS_TRIDENT2PLUS(unit)) {
            bcm_td2p_cosq_subport_flow_control_get(unit, subport, sched_port);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else
#endif /* BCM_HGPROXY_COE_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_cosq_gport_destmod_attach
 * Purpose:
 *      Attach gport mapping from ingress port, dest_modid to
 *      fabric egress port.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      gport           - (IN) GPORT ID
 *      ingress_port    - (IN) Ingress port
 *      dest_modid      - (IN) Destination module ID
 *      fabric_egress_port - (IN) Port number on fabric that 
 *                             is connected to dest_modid
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_cosq_gport_destmod_attach(int unit, bcm_gport_t gport,
                                      bcm_port_t ingress_port, 
                                      bcm_module_t dest_modid, 
                                      int fabric_egress_port)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit) && !SOC_IS_APACHE(unit)) {
        return bcm_td_cosq_gport_destmod_attach(unit, gport, ingress_port,
                                           dest_modid, fabric_egress_port);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_gport_destmod_detach
 * Purpose:
 *      Attach gport mapping from ingress port, dest_modid to
 *      fabric egress port.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      gport           - (IN) GPORT ID
 *      ingress_port    - (IN) Ingress port
 *      dest_modid      - (IN) Destination module ID
 *      fabric_egress_port - (IN) Port number on fabric that 
 *                             is connected to dest_modid
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_cosq_gport_destmod_detach(int unit, bcm_gport_t gport,
                                      bcm_port_t ingress_port, 
                                      bcm_module_t dest_modid, 
                                      int fabric_egress_port)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit) && !SOC_IS_APACHE(unit)) {
        return bcm_td_cosq_gport_destmod_detach(unit, gport, ingress_port,
                                           dest_modid, fabric_egress_port);
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_gport_congestion_config_set(int unit, bcm_gport_t gport, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_congestion_config_set(unit, gport, cosq, config);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_congestion_config_set(unit, gport, cosq, config);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_congestion_config_set(unit, gport, cosq, config);
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_congestion_config_set(unit, gport, cosq, config);
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_congestion_config_set(unit, gport, cosq, config);
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_gport_congestion_config_get(int unit, bcm_gport_t gport, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        return bcm_mn_cosq_gport_congestion_config_get(unit, gport, cosq, config);
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return bcm_ap_cosq_gport_congestion_config_get(unit, gport, cosq, config);
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return bcm_td2_cosq_gport_congestion_config_get(unit, gport, cosq, config);
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return bcm_tr3_cosq_gport_congestion_config_get(unit, gport, cosq, config);
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return bcm_kt2_cosq_gport_congestion_config_get(unit, gport, cosq, config);
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_bst_profile_set(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 bcm_cosq_bst_profile_t *profile)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        int itm;
        for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm++) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_bst_cmn_profile_set
                        (unit, port, cosq, itm, bid, profile));
        }
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_profile_set(unit, port, cosq, bid, profile);
    }
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
         BCM_IF_ERROR_RETURN(
            bcm_mn_cosq_bst_profile_set(unit, port, cosq, bid, profile));
        return BCM_E_NONE; 
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_bst_profile_set(unit, port, cosq, bid, profile);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_bst_profile_set(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_bst_profile_set(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_bst_profile_set(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_bst_profile_get(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 bcm_cosq_bst_profile_t *profile)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        int itm;
        int rv;
        for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm++) {
            rv = _bcm_th3_bst_cmn_profile_get
                        (unit, port, cosq, itm, bid, profile);
            if ((rv != BCM_E_NONE) || (itm == (_TH3_ITMS_PER_DEV - 1))) {
                return rv;
            }
        }
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_profile_get(unit, port, cosq, bid, profile);
    }
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
         BCM_IF_ERROR_RETURN(
            bcm_mn_cosq_bst_profile_get(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_bst_profile_get(unit, port, cosq, bid, profile);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_bst_profile_get(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_bst_profile_get(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_bst_profile_get(unit, port, cosq, bid, profile));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_bst_multi_profile_set(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 int array_size,
                                 bcm_cosq_bst_profile_t *profile_array)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        int i;
        if(bid != bcmBstStatIdDevice) {
            /* only CFAP thresh is non-duplicate */
            return BCM_E_UNAVAIL;
        }
        if(array_size > NUM_ITM(unit)) {
            return BCM_E_PARAM;
        }
        for(i = 0 ; i < array_size; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_cmn_profile_set(unit, port, cosq, i, bid,
                                                         &profile_array[i]));
        }
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_multi_profile_set(unit, port, cosq, bid, 
            array_size, profile_array);
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_bst_multi_profile_get(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 int array_size,
                                 bcm_cosq_bst_profile_t *profile_array,
                                 int *count)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        int i;
        int rv;
        if(bid != bcmBstStatIdDevice) {
            /* only CFAP thresh is non-duplicate */
            return BCM_E_UNAVAIL;
        }
        if(array_size > NUM_ITM(unit)) {
            return BCM_E_PARAM;
        }
        for(i = 0 ; i < array_size; i++) {
            rv = _bcm_th3_bst_cmn_profile_get(unit, port, cosq, i,
                                                bid, &profile_array[i]);
            if ((rv != BCM_E_NONE) || (i == (array_size - 1))) {
                return rv;
            }

        }
        *count = i;
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_multi_profile_get(unit, port, cosq, bid, 
            array_size, profile_array, count);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_get
 * Purpose:
 *      Set the profile for tracking. 
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) GPORT ID
 *      cosq          - (IN) COS queue
 *      bid           - (IN) BST statistics ID.
 *      options       - (IN) Clear-on-read or not
 *      value         - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_cosq_bst_stat_get(int unit, 
                              bcm_gport_t port, 
                              bcm_cos_queue_t cosq, 
                              bcm_bst_stat_id_t bid,
                              uint32 options,
                              uint64 *pvalue)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (pvalue == NULL) {
        return BCM_E_PARAM;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_bst_stat_get
            (unit, port, cosq, BCM_COSQ_BUFFER_ID_INVALID, bid, options, pvalue);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_stat_get
            (unit, port, cosq, BCM_COSQ_BUFFER_ID_INVALID, bid, options, pvalue);
    }
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
         BCM_IF_ERROR_RETURN(
             bcm_mn_cosq_bst_stat_get(unit, port, cosq, bid, options, pvalue)); 
        return BCM_E_NONE;
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_bst_stat_get(unit, port, cosq, bid, options, pvalue);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_bst_stat_get(unit, port, cosq, bid, options, pvalue));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_bst_stat_get(unit, port, cosq, bid, options, pvalue));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_bst_stat_get(unit, port, cosq, bid, options, pvalue));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_get32
 * Purpose:
 *      Set the profile for tracking. 
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) GPORT ID
 *      cosq          - (IN) COS queue
 *      bid           - (IN) BST statistics ID.
 *      options       - (IN) Clear-on-read or not
 *      value         - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_cosq_bst_stat_get32(int unit, 
                              bcm_gport_t port, 
                              bcm_cos_queue_t cosq, 
                              bcm_bst_stat_id_t bid,
                              uint32 options,
                              uint32 *pvalue)
{
    uint64 val64;
    int rv;

    if (NULL == pvalue) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(val64);
    rv = bcm_esw_cosq_bst_stat_get(unit, port, cosq, bid, options, &val64);

    if (BCM_SUCCESS(rv)) {
        *pvalue = COMPILER_64_LO(val64);
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_extended_get
 * Purpose:
 *      Set the profile for tracking. 
 * Parameters:
 *      unit          - (IN) Unit number.
 *      id            - (IN) Integrated cosq index
 *      bid           - (IN) BST statistics ID.
 *      options       - (IN) Clear-on-read or not
 *      value         - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_cosq_bst_stat_extended_get(int unit, 
                              bcm_cosq_object_id_t *id,
                              bcm_bst_stat_id_t bid,
                              uint32 options,
                              uint64 *pvalue)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (pvalue == NULL) {
        return BCM_E_PARAM;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_bst_stat_get
            (unit, id->port, id->cosq, id->buffer, bid, options, pvalue);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_stat_get
            (unit, id->port, id->cosq, id->buffer, bid, options, pvalue);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_extended_get32
 * Purpose:
 *      Set the profile for tracking. 
 * Parameters:
 *      unit          - (IN) Unit number.
 *      id            - (IN) Integrated cosq index
 *      bid           - (IN) BST statistics ID.
 *      options       - (IN) Clear-on-read or not
 *      value         - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_esw_cosq_bst_stat_extended_get32(int unit, 
                              bcm_cosq_object_id_t *id,
                              bcm_bst_stat_id_t bid,
                              uint32 options,
                              uint32 *pvalue)
{
    uint64 val64;
    int rv;

    if (NULL == pvalue) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(val64);
    rv = bcm_esw_cosq_bst_stat_extended_get(unit, id, bid, options, &val64);

    if (BCM_SUCCESS(rv)) {
        *pvalue = COMPILER_64_LO(val64);
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_multi_get
 * Purpose:
 *      To retrieve a list of stats all at once. To match the usual API
 *      multi_get(), the signature for this function should take a list 
 *      of stat IDs to fetch, and return a list of values along with 
 *      the count of items returned.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - port to configure, -1 for any port.
 *      cosq - COS queue to configure, -1 for any COS queue.
 *      options - BCM_COSQ_STAT_CLEAR.
 *      max_values - max value in id_list.
 *      id_list - (IN) IDs to retrieve.
 *      values - (OUT) values retrieved from id_list
 * Returns:
 *      BCM_E_XXX
 */ 
int bcm_esw_cosq_bst_stat_multi_get(int unit,
                                    bcm_gport_t port,
                                    bcm_cos_queue_t cosq,
                                    uint32 options,
                                    int max_values,
                                    bcm_bst_stat_id_t *id_list,
                                    uint64 *values)
{
    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (id_list == NULL || values == NULL) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_bst_stat_multi_get(unit, port, cosq, options,
                                              max_values, id_list, values);
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_stat_multi_get(unit, port, cosq, options,
                                              max_values, id_list, values);
    }
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
         BCM_IF_ERROR_RETURN(
              bcm_mn_cosq_bst_stat_multi_get(unit, port, cosq, options,
                                            max_values, id_list, values));
        return BCM_E_NONE;
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_bst_stat_multi_get(unit, port, cosq, options,
                                               max_values, id_list, values);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_bst_stat_multi_get(unit, port, cosq, options, 
                                            max_values, id_list, values));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_bst_stat_multi_get(unit, port, cosq, options, 
                                            max_values, id_list, values));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_bst_stat_multi_get(unit, port, cosq, options, 
                                            max_values, id_list, values));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_multi_get32
 * Purpose:
 *      To retrieve a list of stats all at once. To match the usual API
 *      multi_get(), the signature for this function should take a list 
 *      of stat IDs to fetch, and return a list of values along with 
 *      the count of items returned.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - port to configure, -1 for any port.
 *      cosq - COS queue to configure, -1 for any COS queue.
 *      options - BCM_COSQ_STAT_CLEAR.
 *      max_values - max value in id_list.
 *      id_list - (IN) IDs to retrieve.
 *      values - (OUT) values retrieved from id_list
 * Returns:
 *      BCM_E_XXX
 */ 
int bcm_esw_cosq_bst_stat_multi_get32(int unit,
                                    bcm_gport_t port,
                                    bcm_cos_queue_t cosq,
                                    uint32 options,
                                    int max_values,
                                    bcm_bst_stat_id_t *id_list,
                                    uint32 *values)
{
    int i, rv;
    uint64 *val64;

    val64 = sal_alloc(max_values * sizeof(uint64), "bst_stat_multi_get32");
    if (val64 == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(val64, 0x0, max_values * sizeof(uint64));

    rv = bcm_esw_cosq_bst_stat_multi_get(unit, port, cosq, options, max_values, \
                                    id_list, val64);
    if (BCM_SUCCESS(rv)) {
        for (i = 0; i < max_values; i++) {
            values[i] = COMPILER_64_LO(val64[i]);
        }
    }

    sal_free(val64);
    return rv;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_clear
 * Purpose:
 *      clear given BST stats to zero
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - port to configure, -1 for any port.
 *      cosq - COS queue to configure, -1 for any COS queue.
 * Returns:
 *      BCM_E_XXX
 */ 
int bcm_esw_cosq_bst_stat_clear(int unit, bcm_gport_t port, 
                                bcm_cos_queue_t cosq, bcm_bst_stat_id_t bid)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_bst_stat_clear
            (unit, port, cosq, BCM_COSQ_BUFFER_ID_INVALID, bid);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_stat_clear
            (unit, port, cosq, BCM_COSQ_BUFFER_ID_INVALID, bid);
    }
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
         BCM_IF_ERROR_RETURN(
             bcm_mn_cosq_bst_stat_clear(unit, port, cosq, bid)); 
        return BCM_E_NONE;
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_bst_stat_clear(unit, port, cosq, bid);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_bst_stat_clear(unit, port, cosq, bid));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_bst_stat_clear(unit, port, cosq, bid));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_bst_stat_clear(unit, port, cosq, bid));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_bst_stat_extended_clear
 * Purpose:
 *      clear given BST stats to zero
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      id   - Integrated cosq index
 * Returns:
 *      BCM_E_XXX
 */ 
int bcm_esw_cosq_bst_stat_extended_clear(int unit, bcm_cosq_object_id_t *id,
                                         bcm_bst_stat_id_t bid)
{
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_bst_stat_clear
            (unit, id->port, id->cosq, id->buffer, bid);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_bst_stat_clear
            (unit, id->port, id->cosq, id->buffer, bid);
    }
#endif
    return BCM_E_UNAVAIL;
}

int bcm_esw_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid)
{
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_bst_stat_sync(unit, bid));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
         BCM_IF_ERROR_RETURN(
             bcm_ap_cosq_bst_stat_sync(unit, bid)); 
        return BCM_E_NONE;
    }
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td3_cosq_bst_stat_sync(unit, bid));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_bst_stat_sync(unit, bid));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_bst_stat_sync(unit, bid));
        return BCM_E_NONE;
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_bst_stat_sync(unit, bid));
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_create
 * Purpose:
 *      Create a cosq classifier.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier    - (IN) Classifier attributes
 *      classifier_id - (IN/OUT) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_create(
    int unit, 
    bcm_cosq_classifier_t *classifier, 
    int *classifier_id)
{
    if ((classifier->flags & BCM_COSQ_CLASSIFIER_VLAN) &&
        (classifier->flags & BCM_COSQ_CLASSIFIER_VFI)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if ((classifier->flags & BCM_COSQ_CLASSIFIER_FIELD) &&
        (!(classifier->flags & BCM_COSQ_CLASSIFIER_WITH_ID))) {
        if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else    
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_field_classifier_id_create(unit, classifier,
                                                            classifier_id));
            } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th_cosq_field_classifier_id_create(unit, classifier,
                                                            classifier_id));
            } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_mn_cosq_field_classifier_id_create(unit, classifier,
                                                    classifier_id));
            } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hr4_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_fb6_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hx5_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td3_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else 
#endif      
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            } else    
#endif
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_cosq_field_classifier_id_create(unit, classifier,
                                                             classifier_id));
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else 
#endif
    if (classifier->flags & BCM_COSQ_CLASSIFIER_L2 ||
        classifier->flags & BCM_COSQ_CLASSIFIER_L3 ||
        classifier->flags & BCM_COSQ_CLASSIFIER_GPORT || 
        classifier->flags & BCM_COSQ_CLASSIFIER_L3_EGRESS) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit) &&
            soc_feature(unit, soc_feature_endpoint_queuing)) {
            BCM_IF_ERROR_RETURN(bcm_ap_cosq_endpoint_create(unit, classifier,
                        classifier_id));
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            BCM_IF_ERROR_RETURN(bcm_td2_cosq_endpoint_create(unit, classifier,
                        classifier_id));
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            return BCM_E_PARAM;
        }
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_VLAN) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_kt2_cosq_service_classifier_id_create(
                             unit, classifier, classifier_id));
            } else
#endif
#if defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_ap_cosq_service_classifier_id_create(unit, classifier,
                                                              classifier_id));
            } else
#endif  /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TRIDENT2(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2_cosq_service_classifier_id_create(unit, classifier,
                                                              classifier_id));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(bcm_tr3_cosq_service_classifier_id_create(
                             unit, classifier, classifier_id));
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            return BCM_E_UNAVAIL;
        }
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_VFI) {
        if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_kt2_cosq_service_classifier_id_create(
                             unit, classifier, classifier_id));
            } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TRIDENT2(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2_cosq_service_classifier_id_create(unit, classifier,
                                                              classifier_id));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_destroy
 * Purpose:
 *      Destroy a cosq classifier.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_destroy(
    int unit, 
    int classifier_id)
{

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_field_classifier_id_destroy(unit, 
                                                         classifier_id));
            } else 
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_field_classifier_id_destroy(unit,
                                                             classifier_id));
            } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th_cosq_field_classifier_id_destroy(unit,
                                                             classifier_id));
            } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_field_classifier_id_destroy(unit,
                                                     classifier_id)); 
            } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hr4_cosq_field_classifier_id_destroy(unit,
                                                              classifier_id));
            } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_fb6_cosq_field_classifier_id_destroy(unit,
                                                              classifier_id));
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hx5_cosq_field_classifier_id_destroy(unit,
                                                              classifier_id));
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td3_cosq_field_classifier_id_destroy(unit,
                                                              classifier_id));
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_field_classifier_id_destroy(unit, 
                                                              classifier_id));
            } else 
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_field_classifier_id_destroy(unit, 
                                                              classifier_id));
            } else 
#endif
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_cosq_field_classifier_id_destroy(unit, 
                                                         classifier_id));
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else 
#endif
    if (_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit) && 
            soc_feature(unit, soc_feature_endpoint_queuing)) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_endpoint_destroy(unit, classifier_id));
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            BCM_IF_ERROR_RETURN
                (bcm_td2_cosq_endpoint_destroy(unit, classifier_id));
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            return BCM_E_PARAM;
        }
    } else if (_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_kt2_cosq_service_classifier_id_destroy(
                                       unit, classifier_id));
            } else 
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_ap_cosq_service_classifier_id_destroy(unit,
                                                               classifier_id));
            } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TRIDENT2(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2_cosq_service_classifier_id_destroy(unit,
                                                               classifier_id));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(bcm_tr3_cosq_service_classifier_id_destroy(
                                       unit, classifier_id));
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            return BCM_E_UNAVAIL;
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_get
 * Purpose:
 *      Get info about a cosq classifier.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 *      classifier    - (OUT) Classifier attributes
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_get(
    int unit, 
    int classifier_id, 
    bcm_cosq_classifier_t *classifier)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_field_classifier_get(unit, classifier_id,
                                                      classifier));
            } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th_cosq_field_classifier_get(unit, classifier_id,
                                                      classifier));
            } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_mn_cosq_field_classifier_get(unit, classifier_id,
                                              classifier));
            } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hr4_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_fb6_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hx5_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td3_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_cosq_field_classifier_get(unit, classifier_id,
                                                       classifier));
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else 
#endif
    if (_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit) &&
            soc_feature(unit, soc_feature_endpoint_queuing)) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_endpoint_get(unit, classifier_id, classifier));
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            BCM_IF_ERROR_RETURN
                (bcm_td2_cosq_endpoint_get(unit, classifier_id, classifier));
        } else
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            return BCM_E_PARAM;
        }
    } else if (_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN(bcm_kt2_cosq_service_classifier_get(unit, 
                                     classifier_id, classifier));
            } else 
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_ap_cosq_service_classifier_get(unit, classifier_id,
                                                        classifier));
            } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TRIDENT2(unit)) {
                BCM_IF_ERROR_RETURN(
                    bcm_td2_cosq_service_classifier_get(unit, classifier_id,
                                                        classifier));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(bcm_tr3_cosq_service_classifier_get(unit, 
                                     classifier_id, classifier));            
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            return BCM_E_UNAVAIL;
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_mapping_multi_set
 * Purpose:
 *      Set the mapping from port, classifier, and multiple internal priorities
 *      to multiple COS queues in a queue group.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) GPORT ID
 *      classifier_id  - (IN) Classifier ID
 *      queue_group    - (IN) Queue group
 *      array_count    - (IN) Number of elements in priority_array and
 *                            cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (IN) Array of COS queues
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_mapping_multi_set(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t queue_group, 
    int array_count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array)
{
    bcm_port_t local_port;
    int sysportid;

    if (array_count <= 0) {
        return BCM_E_PARAM;
    }
    
    if (NULL == priority_array || NULL == cosq_array) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_field_classifier_map_set(unit,
                                                       classifier_id,
                                                       array_count,
                                                       priority_array,
                                                       cosq_array));        
            } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_field_classifier_map_set(unit,
                                                          classifier_id,
                                                          array_count,
                                                          priority_array,
                                                          cosq_array));
            } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th_cosq_field_classifier_map_set(unit,
                                                          classifier_id,
                                                          array_count,
                                                          priority_array,
                                                          cosq_array));
            } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                BCM_IF_ERROR_RETURN
                     (bcm_mn_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));
            } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hr4_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));
            } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_fb6_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hx5_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td3_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));        
            } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_field_classifier_map_set(unit,
                                                           classifier_id,
                                                           array_count,
                                                           priority_array,
                                                           cosq_array));        
            } else
#endif
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_cosq_field_classifier_map_set(unit,
                                                       classifier_id,
                                                       array_count,
                                                       priority_array,
                                                       cosq_array));
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else 
#endif
    {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port,
                                                       &local_port));
        } else if (SOC_PORT_VALID(unit, port)) {
            local_port = port;
        } else {
            return BCM_E_PORT;
        }
        
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(queue_group)) {
            sysportid = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(queue_group);
            if (sysportid != local_port) {
                /* The port the queue_group was created on does not match
                 * the input parameter port.
                 */
                return BCM_E_PORT;
            }
        } else {
            if (!BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(queue_group)) {
                return BCM_E_PARAM;
            }
        }

        if (_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit) &&
                soc_feature(unit, soc_feature_endpoint_queuing)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_endpoint_map_set(unit, local_port,
                                                   classifier_id, queue_group,
                                                   array_count, priority_array,
                                                   cosq_array));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_endpoint_queuing)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_endpoint_map_set(unit, local_port,
                                                   classifier_id, queue_group,
                                                   array_count, priority_array,
                                                   cosq_array));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                return BCM_E_PARAM;
            }
        } else if (_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_service_map_set(unit, local_port,
                                                  classifier_id, queue_group,
                                                  array_count, priority_array,
                                                  cosq_array)); 
                } else 
#endif
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_ap_cosq_service_map_set(unit, local_port,
                                                     classifier_id, queue_group,
                                                     array_count,
                                                     priority_array,
                                                     cosq_array));
                } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
                if (SOC_IS_TRIDENT2(unit)
                    || (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2_cosq_service_map_set(unit, local_port,
                                                     classifier_id, queue_group,
                                                     array_count,
                                                     priority_array,
                                                     cosq_array));
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_cosq_service_map_set(unit, local_port,
                                                  classifier_id, queue_group,
                                                  array_count, priority_array,
                                                  cosq_array));
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_mapping_set
 * Purpose:
 *      Set the mapping from port, classifier, and internal priority to a COS
 *      queue in a queue group.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) GPORT ID
 *      classifier_id - (IN) Classifier ID
 *      queue_group   - (IN) Queue group
 *      priority      - (IN) Internal priority
 *      cosq          - (IN) COS queue in queue group  
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_mapping_set(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t queue_group, 
    bcm_cos_t priority, 
    bcm_cos_queue_t cosq)
{
    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        return (BCM_E_UNAVAIL);
    }
    return bcm_esw_cosq_classifier_mapping_multi_set(unit, port, classifier_id,
            queue_group, 1, &priority, &cosq);
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_mapping_multi_get
 * Purpose:
 *      Get the mapping from port, classifier, and multiple internal priorities
 *      to multiple COS queues in a queue group.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) GPORT ID
 *      classifier_id  - (IN) Classifier ID
 *      queue_group    - (OUT) Queue group
 *      array_max      - (IN) Size of priority_array and cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (OUT) Array of COS queues
 *      array_count    - (OUT) Size of cosq_array
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_mapping_multi_get(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t *queue_group, 
    int array_max, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array, 
    int *array_count)
{
    bcm_port_t local_port;

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                return (bcm_kt2_cosq_field_classifier_map_get(unit,
                                                          classifier_id,
                                                          array_max,
                                                          priority_array,
                                                          cosq_array,
                                                          array_count)); 
            } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                return (bcm_th3_cosq_field_classifier_map_get(unit,
                                                             classifier_id,
                                                             array_max,
                                                             priority_array,
                                                             cosq_array,
                                                             array_count));
            } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                return (bcm_th_cosq_field_classifier_map_get(unit,
                                                             classifier_id,
                                                             array_max,
                                                             priority_array,
                                                             cosq_array,
                                                             array_count));
            } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                return (bcm_mn_cosq_field_classifier_map_get(unit,
                                                             classifier_id,
                                                             array_max,
                                                             priority_array,
                                                             cosq_array,
                                                             array_count));
            } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                return (bcm_hr4_cosq_field_classifier_map_get(unit,
                                                              classifier_id,
                                                              array_max,
                                                              priority_array,
                                                              cosq_array,
                                                              array_count));
            } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                return (bcm_fb6_cosq_field_classifier_map_get(unit,
                                                              classifier_id,
                                                              array_max,
                                                              priority_array,
                                                              cosq_array,
                                                              array_count)); 
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                return (bcm_hx5_cosq_field_classifier_map_get(unit,
                                                              classifier_id,
                                                              array_max,
                                                              priority_array,
                                                              cosq_array,
                                                              array_count)); 
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                return (bcm_td3_cosq_field_classifier_map_get(unit,
                                                              classifier_id,
                                                              array_max,
                                                              priority_array,
                                                              cosq_array,
                                                              array_count)); 
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                return (bcm_ap_cosq_field_classifier_map_get(unit,
                                                              classifier_id,
                                                              array_max,
                                                              priority_array,
                                                              cosq_array,
                                                              array_count)); 
            } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                return (bcm_td2_cosq_field_classifier_map_get(unit,
                                                              classifier_id,
                                                              array_max,
                                                              priority_array,
                                                              cosq_array,
                                                              array_count)); 
            } else
#endif
            {
                return (bcm_tr3_cosq_field_classifier_map_get(unit,
                                                          classifier_id,
                                                          array_max,
                                                          priority_array,
                                                          cosq_array,
                                                          array_count));
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else
#endif
    {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port,
                                                       &local_port));
        } else if (SOC_PORT_VALID(unit, port)) {
            local_port = port;
        } else {
            return BCM_E_PORT;
        }

        if (NULL == queue_group) {
            return BCM_E_PARAM;
        }

        if (array_max > 0) {
            if (NULL == priority_array || NULL == cosq_array) {
                return BCM_E_PARAM;
            }
            
            if (NULL == array_count) {
                return BCM_E_PARAM;
            }
        }

        if (_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit) &&
                soc_feature(unit, soc_feature_endpoint_queuing)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_endpoint_map_get(unit, local_port,
                                                   classifier_id,
                                                   queue_group,
                                                   array_max,
                                                   priority_array,
                                                   cosq_array,
                                                   array_count));
            } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_endpoint_queuing)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_endpoint_map_get(unit, local_port,
                                                   classifier_id,
                                                   queue_group,
                                                   array_max,
                                                   priority_array,
                                                   cosq_array,
                                                   array_count));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                return BCM_E_PARAM;
            }
        } else if (_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_service_map_get(unit, local_port,
                                                  classifier_id,
                                                  queue_group,
                                                  array_max,
                                                  priority_array,
                                                  cosq_array,
                                                  array_count)); 
                } else
#endif
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_ap_cosq_service_map_get(unit, local_port,
                                                     classifier_id,
                                                     queue_group,
                                                     array_max,
                                                     priority_array,
                                                     cosq_array, array_count));
                } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
                if (SOC_IS_TRIDENT2(unit)
                    || (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2_cosq_service_map_get(unit, local_port,
                                                     classifier_id,
                                                     queue_group,
                                                     array_max,
                                                     priority_array,
                                                     cosq_array, array_count));
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_cosq_service_map_get(unit, local_port,
                                                  classifier_id,
                                                  queue_group,
                                                  array_max,
                                                  priority_array,
                                                  cosq_array,
                                                  array_count));
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_mapping_get
 * Purpose:
 *      Get the mapping from port, classifier, and internal priority to a COS
 *      queue in a queue group.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) GPORT ID
 *      classifier_id - (IN) Classifier ID
 *      queue_group   - (OUT) Queue group
 *      priority      - (IN) Internal priority
 *      cosq          - (OUT) COS queue in queue group  
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_mapping_get(
    int unit, 
    bcm_gport_t port, 
    int classifier_id, 
    bcm_gport_t *queue_group, 
    bcm_cos_t priority, 
    bcm_cos_queue_t *cosq)
{
    int array_count;

    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        return (BCM_E_UNAVAIL);
    }

    return bcm_esw_cosq_classifier_mapping_multi_get(unit, port, classifier_id,
            queue_group, 1, &priority, cosq, &array_count);
}

/*
 * Function:
 *      bcm_esw_cosq_classifier_mapping_clear
 * Purpose:
 *      Clear the mapping from port, classifier, and internal priorities
 *      to COS queues in a queue group.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) GPORT ID
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_classifier_mapping_clear(
    int unit, 
    bcm_gport_t port, 
    int classifier_id)
{
    bcm_port_t local_port;

#ifdef BCM_TRIUMPH3_SUPPORT
    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        if (soc_feature(unit, soc_feature_field_ingress_cosq_override)) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_field_classifier_map_clear(unit, 
                                                             classifier_id));
            } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_field_classifier_map_clear(unit,
                                                            classifier_id));
            } else
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_th_cosq_field_classifier_map_clear(unit,
                                                            classifier_id));
            } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_mn_cosq_field_classifier_map_clear(unit,
                                                             classifier_id));
            } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hr4_cosq_field_classifier_map_clear(unit,
                                                             classifier_id));
            } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_fb6_cosq_field_classifier_map_clear(unit,
                                                             classifier_id));
            } else
#endif
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_hx5_cosq_field_classifier_map_clear(unit,
                                                             classifier_id));
            } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td3_cosq_field_classifier_map_clear(unit,
                                                             classifier_id));
            } else
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_field_classifier_map_clear(unit, 
                                                             classifier_id));
            } else
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_field_classifier_map_clear(unit, 
                                                             classifier_id));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_cosq_field_classifier_map_clear(unit, 
                                                             classifier_id));
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else 
#endif
    {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, 
                                                       &local_port));
        } else if (SOC_PORT_VALID(unit, port)) {
            local_port = port;
        } else {
            return BCM_E_PORT;
        }

        if (_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit) &&
                soc_feature(unit, soc_feature_endpoint_queuing)) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_endpoint_map_clear(unit, local_port,
                                                     classifier_id));
            } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_endpoint_queuing)) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_cosq_endpoint_map_clear(unit, local_port,
                                                     classifier_id));
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            {
                return BCM_E_PARAM;
            }
        } else if (_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_service_queuing)) {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_service_map_clear(unit, local_port,
                                                        classifier_id)); 
                } else
#endif
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_ap_cosq_service_map_clear(unit, local_port,
                                                       classifier_id));
                } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
                if (SOC_IS_TRIDENT2(unit)
                    || (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2_cosq_service_map_clear(unit, local_port,
                                                       classifier_id));
                } else
#endif /* BCM_TRIDENT2_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_cosq_service_map_clear(unit, local_port,
                                                        classifier_id));
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                return BCM_E_UNAVAIL;
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    return (BCM_E_NONE);
}

int 
bcm_esw_cosq_classifier_id_get(
    int unit, 
    bcm_cosq_classifier_t *classifier, 
    int array_count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array, 
    int *classifier_id)
{
    int rv = BCM_E_UNAVAIL;
    if ((NULL == priority_array)||(NULL == cosq_array)||
        (NULL == classifier_id) ||(NULL == classifier)||
        ( 0 == array_count)){
        return BCM_E_PARAM;
    }
#ifdef BCM_TRIUMPH3_SUPPORT
    if (classifier->flags & BCM_COSQ_CLASSIFIER_FIELD) { 
        if (SOC_IS_TRIUMPH3(unit)){
            rv = bcm_tr3_cosq_classifier_id_get(unit, classifier, array_count,
                    priority_array, cosq_array, classifier_id);
        }
    }
#endif
    return rv;
}
/*
 * Function:
 *      bcm_esw_cosq_cpu_cosq_enable_set
 * Purpose:
 *      To enable/disable Rx of packets on the specified CPU cosq.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      cosq          - (IN) CPU Cosq ID
 *      enable        - (IN) Enable/Disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_cpu_cosq_enable_set(
    int unit,
    bcm_cos_queue_t cosq,
    int enable)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th3_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
          BCM_IF_ERROR_RETURN(
            bcm_mn_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else 
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_hr4_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_hx5_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)){
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /*BCM_APACHE_SUPPORT*/
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td_cosq_cpu_cosq_enable_set(unit, cosq, enable));
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_cosq_cpu_cosq_enable_get
 * Purpose:
 *      To get enable/disable status on Rx of packets on the specified CPU cosq.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      cosq          - (IN) CPU Cosq ID
 *      enable        - (OUT) Enable/Disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_cosq_cpu_cosq_enable_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *enable)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th3_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
          BCM_IF_ERROR_RETURN(
            bcm_mn_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_hr4_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_hx5_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_ap_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else 
#endif /* BCM_APACHE_SUPPORT*/
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td_cosq_cpu_cosq_enable_get(unit, cosq, enable));
    } else
#endif /* BCM_TRIDENT_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

int
bcm_esw_cosq_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool)
{  
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th3_cosq_service_pool_set(unit, id, cosq_service_pool));
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_service_pool_set(unit, id, cosq_service_pool));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

int
bcm_esw_cosq_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th3_cosq_service_pool_get(unit, id, cosq_service_pool));
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_service_pool_get(unit, id, cosq_service_pool));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_class_mapping_set
 * Purpose:
 *     Set PFC mapping for port. PFC class is mapped to cosq or scheduler node gports.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping
 *     mapping_array    - (IN) mappings 
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_pfc_class_mapping_set(
    int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_pfc_class_mapping_set(unit, port,
                                              array_count, mapping_array));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_mn_cosq_pfc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hr4_cosq_pfc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_fb6_cosq_pfc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hx5_cosq_pfc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td3_cosq_pfc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_pfc_class_mapping_set(unit, port, 
                                               array_count, mapping_array));
    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_pfc_class_mapping_set(unit, port, 
                                               array_count, mapping_array));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td_cosq_pfc_class_mapping_set(unit, port, 
                                              array_count, mapping_array));
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit))  {
        BCM_IF_ERROR_RETURN(
            bcm_kt2_cosq_pfc_class_mapping_set(unit, port, 
                                               array_count, mapping_array));
    } else 
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit))  {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_pfc_class_mapping_set(unit, port, 
                                               array_count, mapping_array));
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_class_mapping_get
 * Purpose:
 *     Get PFC mapping of a port. 
 *     Retrieves cosq or scheduler node gports associated to PFC classes.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_max        - (IN) max retrieve count
 *     mapping_array    - (OUT) mappings 
 *     array_count      - (OUT) retrieved mapping count
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_pfc_class_mapping_get(
    int unit,
    bcm_gport_t port,
    int array_max,
    bcm_cosq_pfc_class_mapping_t *mapping_array,
    int *array_count)
{
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_pfc_class_mapping_get(unit, port, array_max,
                                              mapping_array, array_count));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_mn_cosq_pfc_class_mapping_get(unit, port, array_max,
                                               array_count, mapping_array)); 
    } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hr4_cosq_pfc_class_mapping_get(unit, port, array_max,
                                               mapping_array,array_count));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_fb6_cosq_pfc_class_mapping_get(unit, port, array_max,
                                               mapping_array,array_count));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hx5_cosq_pfc_class_mapping_get(unit, port, array_max,
                                               mapping_array,array_count));
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td3_cosq_pfc_class_mapping_get(unit, port, array_max,
                                               mapping_array,array_count));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_pfc_class_mapping_get(unit, port, array_max, 
                                               array_count, mapping_array));
    } else
#endif 
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_pfc_class_mapping_get(unit, port, array_max, 
                                               mapping_array, array_count));
    } else
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td_cosq_pfc_class_mapping_get(unit, port, array_max, 
                                              mapping_array, array_count));
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_kt2_cosq_pfc_class_mapping_get(unit, port, array_max, 
                                               array_count, mapping_array));
    } else
#endif 
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_tr3_cosq_pfc_class_mapping_get(unit, port, array_max, 
                                               mapping_array, array_count));
    } else
#endif 
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_safc_class_mapping_set
 * Purpose:
 *     Set SAFC mapping for port. SAFC class
 *     is mapped to cosq or scheduler node gports.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping
 *     mapping_array    - (IN) mappings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_safc_class_mapping_set(
    int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_safc_class_mapping_t *mapping_array)
{
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_safc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hr4_cosq_safc_class_mapping_set(unit, port,
                                                array_count, mapping_array));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hx5_cosq_safc_class_mapping_set(unit, port,
                                                array_count, mapping_array));
    } else
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_fb6_cosq_safc_class_mapping_set(unit, port,
                                                array_count, mapping_array));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td3_cosq_safc_class_mapping_set(unit, port,
                                                array_count, mapping_array));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_kt2_cosq_safc_class_mapping_set(unit, port,  
                                               array_count, mapping_array));
    } else
#endif 
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_mn_cosq_safc_class_mapping_set(unit, port,
                                               array_count, mapping_array)); 
    } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_safc_class_mapping_set(unit, port,
                                               array_count, mapping_array));
    } else
#endif 
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_safc_class_mapping_get
 * Purpose:
 *     Get SAFC mapping of a port.
 *     Retrieves cosq or scheduler node
 *     gports associated to SAFC classes.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_max        - (IN) max retrieve count
 *     mapping_array    - (OUT) mappings
 *     array_count      - (OUT) retrieved mapping count
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_safc_class_mapping_get(
    int unit,
    bcm_gport_t port,
    int array_max,
    bcm_cosq_safc_class_mapping_t *mapping_array,
    int *array_count)
{
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_th_cosq_safc_class_mapping_get(unit, port, array_max,
                                               mapping_array, array_count));
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hr4_cosq_safc_class_mapping_get(unit, port, array_max,
                                                mapping_array, array_count));
    } else
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_hx5_cosq_safc_class_mapping_get(unit, port, array_max,
                                                mapping_array, array_count));
    } else
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_fb6_cosq_safc_class_mapping_get(unit, port, array_max,
                                                mapping_array, array_count));
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_td3_cosq_safc_class_mapping_get(unit, port, array_max,
                                                mapping_array, array_count));
    } else
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcm_kt2_cosq_safc_class_mapping_get(unit, port, array_max, 
                                               array_count, mapping_array ));
    } else
#endif
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_mn_cosq_safc_class_mapping_get(unit, port, array_max,
                                               mapping_array, array_count)); 
    } else
#endif /* BCM_MONTEREY_SUPPORT */ 
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_safc_class_mapping_get(unit, port, array_max,
                                               mapping_array, array_count));
    } else
#endif 
    {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_config_set
 * Purpose:
 *     Setup PFC deadlock feature to monitor for the given priority with
 *     associated values.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) priority
 *     config           - (IN) Config for a given priority
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_config_set(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_config_t *config)
{

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        return _bcm_pfc_deadlock_config_set(unit, priority, config);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_config_get
 * Purpose:
 *     Get config values for a given priority in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) priority
 *     config           - (OUT) Config for a given priority
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_config_get(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_config_t *config)
{

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        return _bcm_pfc_deadlock_config_get(unit, priority, config);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_info_get
 * Purpose:
 *     Get bitmap of Enabled(Admin) and Current Deadlock ports status for a
 *     given priority in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) priority
 *     info             - (OUT) Info for a given priority
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_info_get(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info)
{
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if(SOC_IS_TOMAHAWK3(unit)) {
          return _bcm_th3_pfc_deadlock_info_get(unit, priority, pfc_deadlock_info);
        } else
#endif
        {
        return _bcm_pfc_deadlock_info_get(unit, priority, pfc_deadlock_info);
        }
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_queue_config_set
 * Purpose:
 *     Enable or disable the given Port/Queue for PFC deadlock feature to
 *     monitor.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) priority
 *     config           - (IN) Config for a given UC Queue Gport
 * Returns:
 *     BCM_E_XXX
 */

int bcm_esw_cosq_pfc_deadlock_queue_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_pfc_deadlock_queue_config_t *q_config)
{

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        return _bcm_pfc_deadlock_queue_config_set(unit, gport, q_config);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_queue_config_get
 * Purpose:
 *     Get the Enable or disable monitoring status for the given Port/Queue.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) priority
 *     config           - (OUT) Config for a given UC Queue Gport
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_queue_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_pfc_deadlock_queue_config_t *q_config)
{

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        return _bcm_pfc_deadlock_queue_config_get(unit, gport, q_config);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_queue_status_get
 * Purpose:
 *     Get the current Deadlock status for the given Port/Queue.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) priority
 *     deadlock_status  - (OUT) Deatlock status for the given UC Queue Gport
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_queue_status_get(
    int unit,
    bcm_gport_t gport,
    uint8 *deadlock_status)
{
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if(SOC_IS_TOMAHAWK3(unit)) {
            return _bcm_th3_pfc_deadlock_port_status_get
                       (unit, gport, deadlock_status);
        }
#endif
        return _bcm_pfc_deadlock_queue_status_get(unit, gport, deadlock_status);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcmi_esw_cosq_handle_interrupt
 * Purpose:
 *     Internal interrupt event callback function to invoke registered callback
 *     function (by bcm_esw_cosq_event_register).
 * Parameters:
 *     unit             - (IN) unit number
 *     event            - (IN) event which trigger the interrupt.
 *     port             - (IN) port which event happened. If the event is
 *                             system wise or not port related, port = -1.
 *     cosq             - (IN) cosq which event happened. If the event is
 *                             system wise or not cosq related, cosq = -1.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_esw_cosq_handle_interrupt(int unit,
                               bcm_cosq_event_type_t event,
                               bcm_port_t port,
                               bcm_cos_queue_t cosq)
{
    cosq_event_handler_t *e_handler;
    bcm_gport_t gport;
    uint8 do_cb = 0, pbmp_valid, qbmp_valid, port_match, cosq_match;
    int rv = BCM_E_NONE;

    COSQ_EVENT_LOCK(unit);
    for (e_handler = cosq_event_handler_list[unit];
         e_handler != NULL;
         e_handler = e_handler->next) {
        if (BCM_COSQ_EVENT_TYPE_GET(e_handler->event_types, event)){
            pbmp_valid = 0;
            qbmp_valid = 0;
            port_match = 0;
            cosq_match = 0;
            if (port != -1) {
                pbmp_valid = !SHR_BITNULL_RANGE(e_handler->pbmp, 0,
                                                SOC_MAX_NUM_PORTS);
                port_match = SHR_BITGET(e_handler->pbmp, port) ? 1 : 0;
            }
            if (cosq != -1) {
                qbmp_valid = !SHR_BITNULL_RANGE(e_handler->qbmp, 0,
                                                BCM_COS_COUNT);
                cosq_match = SHR_BITGET(e_handler->qbmp, cosq) ? 1 : 0;
            }

            if (port == -1) {
                gport = BCM_GPORT_INVALID;
            } else {
                rv = bcm_esw_port_gport_get(unit, port, &gport);
                if (BCM_FAILURE (rv)) {
                    COSQ_EVENT_UNLOCK(unit);
                    return rv;
                }
            }
            if (pbmp_valid && qbmp_valid) {
                /* If pbmp and qbmp are specified, invoke cb
                 * when port/cosq both matched */
                if (port_match && cosq_match) {
                    do_cb = 1;
                }
            } else if (pbmp_valid || qbmp_valid) {
                if (pbmp_valid && port_match) {
                    do_cb = 1;
                }
                if (qbmp_valid && cosq_match) {
                    do_cb = 1;
                }
            } else {
                do_cb = 1;
            }
            if (do_cb){
                if (e_handler->cb != NULL) {
                    e_handler->cb(unit, event, gport, cosq,
                                  e_handler->user_data);
                }
            }
        }
    }
    COSQ_EVENT_UNLOCK(unit);
    return BCM_E_NONE;
}

STATIC int
bcmi_esw_cosq_event_mask_set(int unit, bcm_cosq_event_type_t event_type,
                             int val)
{
    /* Interrupt mask definitions are device specific. */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_event_mask_set(unit, event_type, val);
    }
#endif
    return BCM_E_UNAVAIL;
}

STATIC int
bcmi_esw_cosq_events_validate(int unit, bcm_cosq_event_types_t event_types)
{
    /* Event support validation(checking) is device specific.*/
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        return bcmi_gh2_cosq_events_validate(unit, event_types);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function: 
 *     bcm_esw_cosq_pfc_deadlock_callback_register
 * Purpose:
 *      Register pfc deadlock recovery callback in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     callback         - (IN) callback function
 *     userdata         - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_recovery_event_register(
    int unit, 
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback, 
    void *userdata)
{
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        return _bcm_cosq_pfc_deadlock_recovery_event_register(unit, callback, userdata);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_recovery_start
 * Purpose:
 *     Start pfc deadlock recovery.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number 
 *     pri              - (IN) priority 
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_pfc_deadlock_recovery_start(int unit, bcm_port_t port,
                                         bcm_cos_t pri)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_th3_pfc_deadlock_recovery_start(unit, port, pri);
        }
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_th_pfc_deadlock_recovery_start(unit, port, pri);
        }
    }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_hr4_pfc_deadlock_recovery_start(unit, port, pri);
        }
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_hx5_pfc_deadlock_recovery_start(unit, port, pri);
        }
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_fb6_pfc_deadlock_recovery_start(unit, port, pri);
        }
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_td3_pfc_deadlock_recovery_start(unit, port, pri);
        }
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_recovery_exit
 * Purpose:
 *     Exit pfc deadlock recovery.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number 
 *     pri              - (IN) priority 
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_pfc_deadlock_recovery_exit(int unit, bcm_port_t port,
                                        bcm_cos_t pri)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_th3_pfc_deadlock_recovery_exit(unit, port, pri);
        }
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_th_pfc_deadlock_recovery_exit(unit, port, pri);
        }
    }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_hr4_pfc_deadlock_recovery_exit(unit, port, pri);
        }
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_hx5_pfc_deadlock_recovery_exit(unit, port, pri);
        }
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_fb6_pfc_deadlock_recovery_exit(unit, port, pri);
        }
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0)) {
            return bcm_td3_pfc_deadlock_recovery_exit(unit, port, pri);
        }
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_pfc_deadlock_callback_unregister
 * Purpose:
 *      Unregister pfc deadlock recovery callback in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     callback         - (IN) callback function
 *     userdata         - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */
int bcm_esw_cosq_pfc_deadlock_recovery_event_unregister(
    int unit, 
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback, 
    void *userdata)
{
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        return _bcm_cosq_pfc_deadlock_recovery_event_unregister(unit, callback, userdata);
    } else {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_cosq_buffer_id_multi_get
 * Purpose:
 *      Get mutliple buffer ids associated with a specified port.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      gport - (IN) Port
 *      cosq - (IN) Reserved field
 *      direction - (IN) Specify ingress or egress direction
 *      array_max - (IN) number of entries to be retrieved
 *      buf_id_array - (OUT) Buffer id array
 *      array_count - (OUT) Actural buffer id count in buf_id_array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_buffer_id_multi_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 bcm_cosq_dir_t direction,
                                 int array_max,
                                 bcm_cosq_buffer_id_t *buf_id_array,
                                 int *array_count)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_th3_cosq_buffer_id_multi_get(unit, gport, cosq, direction,
                                               array_max, buf_id_array, array_count);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return bcm_th_cosq_buffer_id_multi_get(unit, gport, 0, direction,
                                               array_max, buf_id_array, array_count);
    }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        return bcm_hr4_cosq_buffer_id_multi_get(unit, gport, 0, direction,
                                                array_max, buf_id_array, array_count);
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        return bcm_hx5_cosq_buffer_id_multi_get(unit, gport, 0, direction,
                                                array_max, buf_id_array, array_count);
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        return bcm_td3_cosq_buffer_id_multi_get(unit, gport, 0, direction,
                                                array_max, buf_id_array, array_count);
    }
#endif
    return BCM_E_UNAVAIL;
}

STATIC int
bcmi_esw_cosq_event_unregister_all(int unit)
{
    cosq_event_handler_t *e_handler;
    cosq_event_handler_t *e_handler_to_delete;

    COSQ_EVENT_LOCK(unit);
    e_handler = cosq_event_handler_list[unit];
    while (e_handler != NULL) {
        e_handler_to_delete = e_handler;
        e_handler = e_handler->next;
        sal_free(e_handler_to_delete);
    }
    cosq_event_handler_list[unit] = NULL;
    COSQ_EVENT_UNLOCK(unit);

    return BCM_E_NONE;
}

STATIC int
bcmi_esw_cosq_event_deinit(int unit)
{

    if (cosq_event_handler_list[unit] != NULL) {
        bcmi_esw_cosq_event_unregister_all(unit);
    }
    sal_memset(&cosq_event_handler_count[unit], 0,
               sizeof(int) * bcmCosqEventCount);

    if (cosq_event_lock[unit] != NULL) {
        sal_mutex_destroy(cosq_event_lock[unit]);
        cosq_event_lock[unit] = NULL;
    }
    return BCM_E_NONE;
}


STATIC int
bcmi_esw_cosq_event_init(int unit)
{
    int i;
    if (cosq_event_initialized == 0) {
        for (i = 0; i < SOC_MAX_NUM_DEVICES; i ++) {
            cosq_event_lock[i] = NULL;
            cosq_event_handler_list[i] = NULL;
            sal_memset(&cosq_event_handler_count[i], 0,
                       sizeof(int) * bcmCosqEventCount);
        }
        cosq_event_initialized = 1;
    } else {
        BCM_IF_ERROR_RETURN(bcmi_esw_cosq_event_deinit(unit));
    }
    if (cosq_event_lock[unit] == NULL) {
        cosq_event_lock[unit] = sal_mutex_create("cosq event lock");
        if (cosq_event_lock[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_event_register
 * Purpose:
 *     The callback function registration by specifying port or traffic class to
 *     handle the COSQ events.
 * Parameters:
 *     unit             - (IN) unit number
 *     event_types      - (IN) The set of Cosq events for which the specified
 *                             callback should be invoked.
 *     port             - (IN) gport id
 *     cosq             - (IN) cos queue
 *     cb               - (IN) callback function
 *     user_data        - (IN) Pinter to user data to supply in the callback.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If the event is not on port or traffic class basis, the gport and cosq
 *     value should be -1.
 *     If user do not want to register the event on specific port or queue basis,
 *     The gport or cosq value should assign to -1.
 */
int
bcm_esw_cosq_event_register(
    int unit,
    bcm_cosq_event_types_t event_types,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_event_cb cb,
    void *user_data)
{
    cosq_event_handler_t *e_handler;
    cosq_event_handler_t *previous = NULL;
    bcm_cosq_event_type_t e_type;
    int result;
    int rv = BCM_E_NONE;
    bcm_port_t lport;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    SHR_BITTEST_RANGE(event_types.w, 0, bcmCosqEventCount, result);
    if (result == 0) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_cosq_events_validate(unit, event_types));

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &lport));
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, NUM_COS(unit)));

    COSQ_EVENT_LOCK(unit);
    for (e_handler = cosq_event_handler_list[unit];
         e_handler != NULL;
         e_handler = e_handler->next) {
        if (e_handler->cb == cb) {
            break;
        }
        previous = e_handler;
    }

    if (e_handler == NULL) {
        /* This handler hasn't been registered yet */
        e_handler = sal_alloc(sizeof(*e_handler), "Cosq event handler");
        if (e_handler == NULL) {
            COSQ_EVENT_UNLOCK(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(e_handler, 0, sizeof(*e_handler));

        e_handler->next = NULL;
        e_handler->cb = cb;

        if (previous != NULL) {
            previous->next = e_handler;
        } else {
            cosq_event_handler_list[unit] = e_handler;
        }
    }

    for (e_type = 0; e_type < bcmCosqEventCount; ++e_type) {
        if (BCM_COSQ_EVENT_TYPE_GET(event_types, e_type)) {
            if (!BCM_COSQ_EVENT_TYPE_GET(e_handler->event_types, e_type)) {
                /* This handler isn't handling this event yet */
                BCM_COSQ_EVENT_TYPE_SET(e_handler->event_types, e_type);
                if (cosq_event_handler_count[unit][e_type] == 0) {
                    rv = bcmi_esw_cosq_event_mask_set(unit, e_type, 1);
                    if (BCM_FAILURE(rv)) {
                        COSQ_EVENT_UNLOCK(unit);
                        return rv;
                    }
                }
                cosq_event_handler_count[unit][e_type] += 1;
            }
        }
    }

    e_handler->user_data = user_data;
    if (port != BCM_GPORT_INVALID) {
        SHR_BITSET(e_handler->pbmp, lport);
    } else {
        /* clear bitmap */
        SHR_BITCLR_RANGE(e_handler->pbmp, 0, SOC_MAX_NUM_PORTS);
    }
    if (cosq != BCM_COS_INVALID) {
        SHR_BITSET(e_handler->qbmp, cosq);
    } else {
        /* clear bitmap */
        SHR_BITCLR_RANGE(e_handler->qbmp, 0, BCM_COS_COUNT);
    }
    COSQ_EVENT_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *     bcm_esw_cosq_event_unregister
 * Purpose:
 *     The callback function unregistration of COSQ events by specifying port or
 *     traffic class.
 * Parameters:
 *     unit             - (IN) unit number
 *     event_types      - (IN) The set of Cosq events for which the specified
 *                             callback should be invoked.
 *     port             - (IN) gport id
 *     cosq             - (IN) cos queue
 *     cb               - (IN) callback function
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_event_unregister(
    int unit,
    bcm_cosq_event_types_t event_types,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_event_cb cb)
{
    cosq_event_handler_t *e_handler;
    cosq_event_handler_t *previous = NULL;
    bcm_cosq_event_type_t e_type;
    int result;
    int rv = BCM_E_NONE;
    bcm_port_t  lport;
    uint8 pbmp_valid = 0, qbmp_valid = 0;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    SHR_BITTEST_RANGE(event_types.w, 0, bcmCosqEventCount, result);
    if (result == 0) {
        return BCM_E_PARAM;
    }
    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_port_gport_validate(unit, port, &lport));
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_validate(cosq, NUM_COS(unit)));

    COSQ_EVENT_LOCK(unit);
    for (e_handler = cosq_event_handler_list[unit];
         e_handler != NULL;
         e_handler = e_handler->next) {
        /* Find the e_handler by specified cb */
        if (e_handler->cb == cb) {
            break;
        }
        previous = e_handler;
    }

    if (e_handler == NULL) {
        /* Could not find the registed e_handler */
        COSQ_EVENT_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    for (e_type = 0; e_type < bcmCosqEventCount; ++e_type) {
        if (BCM_COSQ_EVENT_TYPE_GET(event_types, e_type)) {
            /* Iterate the specified events  */
            if ((BCM_COSQ_EVENT_TYPE_GET(e_handler->event_types, e_type)) &&
                (cosq_event_handler_count[unit][e_type] > 0)) {
                /* Check if the event was among the registered events. */
                cosq_event_handler_count[unit][e_type] -= 1;
                BCM_COSQ_EVENT_TYPE_CLEAR(e_handler->event_types, e_type);
                /* Clear event mask when event is not associated with others.*/
                if (cosq_event_handler_count[unit][e_type] == 0) {
                    rv = bcmi_esw_cosq_event_mask_set(unit, e_type, 0);
                    if (BCM_FAILURE(rv)) {
                        COSQ_EVENT_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        }
    }

    SHR_BITTEST_RANGE(e_handler->event_types.w, 0, bcmCosqEventCount, result);

    if (0 == result) {
        /* Free the handler if no more events for this handler to handle */
        if (previous != NULL) {
            previous->next = e_handler->next;
        } else {
            cosq_event_handler_list[unit] = e_handler->next;
        }
        sal_free(e_handler);
    } else {
        pbmp_valid = !SHR_BITNULL_RANGE(e_handler->pbmp, 0,
                                        SOC_MAX_NUM_PORTS);
        /* Ignore events associated with certain port */
        if (port != BCM_GPORT_INVALID) {
            if (!pbmp_valid) {
                /* The orginal registered to ignore port case.
                   set bitmaps to 1 */
                SHR_BITSET_RANGE(e_handler->pbmp, 0, SOC_MAX_NUM_PORTS);
            }
            SHR_BITCLR(e_handler->pbmp, lport);
        }

        qbmp_valid = !SHR_BITNULL_RANGE(e_handler->qbmp, 0,
                                        BCM_COS_COUNT);
        /* Ignore events associated with certian cosq */
        if (cosq != BCM_COS_INVALID) {
            if (!qbmp_valid) {
                /* The orginal registered to ignore cosq case.
                   set bitmaps to 1 */
                SHR_BITSET_RANGE(e_handler->qbmp, 0, BCM_COS_COUNT);
            }
            SHR_BITCLR(e_handler->qbmp, cosq);
        }
    }
    COSQ_EVENT_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_control_get
 * Purpose:
 *     Get various configurations for TAS.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas control type defined in bcm_cos_tas_control_t
 *     arg              - (OUT) tas control value
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Most of the TAS configurations are on port basis. For some configuration
 *     which is on system basis, the gport should assign to -1.
 */
int
bcm_esw_cosq_tas_control_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_control_t type,
    int *arg)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_control_get(unit, port, type, arg);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_control_set
 * Purpose:
 *     Set various configurations for TAS.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas control type defined in bcm_cos_tas_control_t
 *     arg              - (IN) tas control value
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Most of the TAS configurations are on port basis. For some configuration
 *     which is on system basis, the gport should assign to -1.
 */
int
bcm_esw_cosq_tas_control_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_control_t type,
    int arg)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_control_set(unit, port, type, arg);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_status_get
 * Purpose:
 *     Get TAS status by specifying the type.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas status type defined in bcm_cos_tas_status_t
 *     arg              - (OUT) tas status value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_status_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_status_t type,
    int *arg)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_status_get(unit, port, type, arg);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_commit
 * Purpose:
 *     Commit the profile to indicate the profile is ready to write to hardware.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_commit(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_commit(unit, port, pid);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_create
 * Purpose:
 *     Create a profile by specifying the profile settings including calendar
 *     table, ptp time information for PTP mode.
 *     A profile id will be assigned along with the settings.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     profile          - (IN) Pointer to profile structure which specifies
 *                             entries in calendar table, ptp time information
 *                             for PTP mode
 *     pid              - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_create(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_t *profile,
    bcm_cosq_tas_profile_id_t *pid)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_create(unit, port, profile, pid);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_get
 * Purpose:
 *     Get the profile information by specifying the profile id.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 *     profile          - (OUT) pointer to profile structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t *profile)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_get(unit, port, pid, profile);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_set
 * Purpose:
 *     Modify the profile information
 * NOTE:
 *     This API is used to modify the profile if needed before doing commit.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 *     profile          - (IN) pointer to profile structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t *profile)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_set(unit, port, pid, profile);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_traverse
 * Purpose:
 *     Traverse the set of profiles associated with the specified port.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     cb               - (IN) A pointer to callback function to call
 *                             for each profile in the specified gport.
 *     user_data        - (IN) Pointer to user data to supply in the callback
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_traverse(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_traverse_cb cb,
    void *user_data)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_traverse(unit, port, cb, user_data);
    }
#endif
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcm_esw_cosq_tas_profile_destroy
 * Purpose:
 *     Destroy the profile and associated resources.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_destroy(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_destroy(unit, port, pid);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_destroy_all
 * Purpose:
 *     Destroy all the profile associated with the port.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_destroy_all(
    int unit,
    bcm_gport_t port)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_destroy_all(unit, port);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_cosq_tas_profile_status_get
 * Purpose:
 *     Get the profile status like profile state, config change time and
 *     entries in the hardware calendar table.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     pid              - (IN) profile id
 *     status           - (OUT) pointer to profile status structure
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_cosq_tas_profile_status_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_status_t *status)
{
#ifdef BCM_TAS_SUPPORT
    if (soc_feature(unit, soc_feature_tas)) {
        return bcmi_esw_tas_profile_status_get(unit, port, pid, status);
    }
#endif
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_TCB)

#if defined (BCM_TCB_SUPPORT)
_bcm_cosq_tcb_unit_ctrl_t  *_bcm_cosq_tcb_unit_control[BCM_MAX_NUM_UNITS];
#endif

/*
 * Function:
 *      bcm_esw_cosq_tcb_config_set
 * Purpose:
 *      Set TCB instance config attributes.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (IN) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_config_set(int unit,
                            bcm_cosq_buffer_id_t buffer_id,
                            bcm_cosq_tcb_config_t *config)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);

#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
        rv = bcm_th2_cosq_tcb_config_set(unit, buffer_id, config);
        }
#endif

#ifdef BCM_HELIX5_SUPPORT
       if (SOC_IS_HELIX5(unit)) {
           rv = bcm_hx5_cosq_tcb_config_set(unit, buffer_id, config);
       } 
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit)) {
            rv = bcm_td3_cosq_tcb_config_set(unit, buffer_id, config);
        }
#endif

        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_config_get
 * Purpose:
 *      Get TCB instance control status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_config_get(int unit,
                            bcm_cosq_buffer_id_t buffer_id,
                            bcm_cosq_tcb_config_t *config)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
        rv = bcm_th2_cosq_tcb_config_get(unit, buffer_id, config);
        }
#endif

#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = bcm_hx5_cosq_tcb_config_get(unit, buffer_id, config);
        } 
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit)) {
               rv = bcm_td3_cosq_tcb_config_get(unit, buffer_id, config);
        }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_gport_threshold_mapping_set
 * Purpose:
 *      Set threshold profile for a UCQ or a port.
 * Parameters:
 *      unit  - (IN) unit number
 *      id    - (IN) Integrated cosq index
 *      index - (IN) Threshold profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When gport is a port, SDK will attach all Ucqs in this port to one threshold profile.
 */
int
bcm_esw_cosq_tcb_gport_threshold_mapping_set(int unit,
                                             bcm_cosq_object_id_t *id,
                                             int index)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
        rv = bcm_th2_cosq_tcb_gport_threshold_mapping_set(unit, 
            id->port, id->cosq, id->buffer, index);
        }
#endif
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = bcm_hx5_cosq_tcb_gport_threshold_mapping_set(unit,
                    id->port, id->cosq, id->buffer, index);
        }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit)) {
                rv = bcm_td3_cosq_tcb_gport_threshold_mapping_set(unit,
                     id->port, id->cosq, id->buffer, index);
        }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_gport_threshold_mapping_get
 * Purpose:
 *      Set threshold profile for a UCQ or a port.
 * Parameters:
 *      unit  - (IN) unit number
 *      id    - (IN) Integrated cosq index
 *      index - (OUT) Threshold profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When gport is a port, SDK will return the threshold profile index of the first Ucq in this
 *      port.
 */
int
bcm_esw_cosq_tcb_gport_threshold_mapping_get(int unit,
                                             bcm_cosq_object_id_t *id,
                                             int *index)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
        rv =  bcm_th2_cosq_tcb_gport_threshold_mapping_get(unit, 
            id->port, id->cosq, id->buffer, index);
        }
#endif

#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = bcm_hx5_cosq_tcb_gport_threshold_mapping_get(unit,
                    id->port, id->cosq, id->buffer, index);
        } 
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit)) {
                rv = bcm_td3_cosq_tcb_gport_threshold_mapping_get(unit,
                     id->port, id->cosq, id->buffer, index);
        }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_threshold_profile_create
 * Purpose:
 *      Create a threshold profile for a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      flags - (IN) BCM_COSQ_TCB_THRESHOLD_PROFILE_WITH_ID: Index arg is given
 *                       BCM_COSQ_TCB_THRESHOLD_PROFILE_REPLACE: Replace existing profile
 *      buffer_id - (IN) TCB buffer id.
 *      threshold - (IN) Threshold profile
 *      index - (INOUT) profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If user do not specify index, SDK will alloc a profile index and return to user.
 *      With BCM_COSQ_TCB_THRESHOLD_PROFILE_REPLACE flag user could directly update a
 *      existing threshold profile to new values.
 */
int
bcm_esw_cosq_tcb_threshold_profile_create(int unit,
                                          int flags,
                                          bcm_cosq_buffer_id_t buffer_id,
                                          bcm_cosq_tcb_threshold_profile_t *threshold,
                                          int *index)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = bcm_hx5_cosq_tcb_threshold_profile_create(unit, flags, buffer_id,
                                                           threshold, index);
        }
#endif
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TRIDENT3_SUPPORT)
        if ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) && !SOC_IS_HELIX5(unit)) {
            rv = bcm_th2_cosq_tcb_threshold_profile_create(unit, flags, buffer_id,
                    threshold, index);
        }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_threshold_profile_get
 * Purpose:
 *      Get a threshold profile for a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      index - (IN) profile index
 *      threshold - (OUT) Threshold profile
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_threshold_profile_get(int unit,
                                       bcm_cosq_buffer_id_t buffer_id,
                                       int index,
                                       bcm_cosq_tcb_threshold_profile_t *threshold)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = bcm_hx5_cosq_tcb_threshold_profile_get(unit, buffer_id, index,
                                                        threshold);
        } else
#endif /* BCM_HELIX5_SUPPORT */
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TRIDENT3_SUPPORT)
        if ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) && !SOC_IS_HELIX5(unit)) {
            rv = bcm_th2_cosq_tcb_threshold_profile_get(unit, buffer_id, index,
                                                        threshold);
        }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_threshold_profile_destroy
 * Purpose:
 *      Destory a existing threshold profile for a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      index - (IN) profile index
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_threshold_profile_destroy(int unit,
                                           bcm_cosq_buffer_id_t buffer_id,
                                           int index)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = bcm_hx5_cosq_tcb_threshold_profile_destroy(unit, buffer_id, index);
        } else
#endif /* BCM_HELIX5_SUPPORT */
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TRIDENT3_SUPPORT)
        if ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) && !SOC_IS_HELIX5(unit)) {
            rv = bcm_th2_cosq_tcb_threshold_profile_destroy(unit, buffer_id, index);
        }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_buffer_multi_get
 * Purpose:
 *      Get packet records in a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) buffer id
 *      array_max - (IN) Number of entries to be retrieved
 *      buffer_array - (OUT) Pakcet record array
 *      array_count - (OUT) Number of packet records returned in buffer_array
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When array_max is 0, buffer_array is NULL, array_count will return actural packet record
 *      count in the TCB buffer.
 */
int
bcm_esw_cosq_tcb_buffer_multi_get(int unit,
                                  bcm_cosq_buffer_id_t buffer_id,
                                  int array_max,
                                  bcm_cosq_tcb_buffer_t *buffer_array,
                                  int *array_count)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        rv = bcm_hx5_cosq_tcb_buffer_multi_get(unit, buffer_id, array_max,
                                                 buffer_array, array_count);
    }
#endif /* BCM_HELIX5_SUPPORT */
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TRIDENT3_SUPPORT)
    if ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) && !SOC_IS_HELIX5(unit)) {
        rv = bcm_th2_cosq_tcb_buffer_multi_get(unit, buffer_id, array_max,
                                                buffer_array, array_count);
    }
#endif
        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_event_multi_get
 * Purpose:
 *      Get drop event records in a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) buffer id
 *      array_max - (IN) Number of entries to be retrieved
 *      event_array - (OUT) Drop event record array
 *      array_count - (OUT) Number of drop event records returned in event_array
 *      overflow_count - (OUT) Drop event count exceed the event buffer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When array_max is 0, event_array is NULL, array_count will return actural drop event
 *      record count in the TCB buffer.
 */
int
bcm_esw_cosq_tcb_event_multi_get(int unit,
                                 bcm_cosq_buffer_id_t buffer_id,
                                 int array_max,
                                 bcm_cosq_tcb_event_t *event_array,
                                 int *array_count,
                                 int *overflow_count)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        rv = bcm_hx5_cosq_tcb_event_multi_get(unit, buffer_id, array_max,
                                              event_array, array_count,
                                              overflow_count);
    }
#endif /* BCM_HELIX5_SUPPORT */
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TRIDENT3_SUPPORT)
    if ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) && !SOC_IS_HELIX5(unit)) {
        rv = bcm_th2_cosq_tcb_event_multi_get(unit, buffer_id, array_max,
                                              event_array, array_count,
                                              overflow_count);
    }
#endif
    TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_control_set
 * Purpose:
 *      Set TCB instance control status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      type - (IN) Control type
 *      arg - (OUT) argument.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When control type is bcmCosqTcbControlFreeze, if buffer_id is -1, this API will freeze all
 *      TCB instance at a once.
 */
int
bcm_esw_cosq_tcb_control_set(int unit,
                             bcm_cosq_buffer_id_t buffer_id,
                             bcm_cosq_tcb_control_t type,
                             int arg)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        if (type == bcmCosqTcbControlEnable) {
            TCB_LOCK(unit);
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
            rv = bcm_th2_cosq_tcb_enable(unit, buffer_id, arg);
            }
#endif
#ifdef BCM_HELIX5_SUPPORT
           if (SOC_IS_HELIX5(unit)) {
                rv = bcm_hx5_cosq_tcb_enable(unit, buffer_id, arg);
           }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit) && !SOC_IS_HELIX5(unit)) {
                rv = bcm_td3_cosq_tcb_enable(unit, buffer_id, arg);
            }
#endif
            TCB_UNLOCK(unit);
            return rv;
        } else if (type == bcmCosqTcbControlFreeze) {
            TCB_LOCK(unit);
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
            rv = bcm_th2_cosq_tcb_freeze(unit, buffer_id, arg);
            }
#endif
#ifdef BCM_HELIX5_SUPPORT
           if (SOC_IS_HELIX5(unit)) {
                rv = bcm_hx5_cosq_tcb_freeze(unit, buffer_id, arg);
           }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)  && !SOC_IS_HELIX5(unit)) {
                rv = bcm_td3_cosq_tcb_freeze(unit, buffer_id, arg);
            }
#endif
            TCB_UNLOCK(unit);
            return rv;
        } else {
            return BCM_E_PARAM;
        }
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_control_get
 * Purpose:
 *      Get TCB instance control status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      type - (IN) Control type
 *      arg - (OUT) argument.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_control_get(int unit,
                             bcm_cosq_buffer_id_t buffer_id,
                             bcm_cosq_tcb_control_t type,
                             int *arg)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    if (soc_feature(unit, soc_feature_tcb)) {
#if defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TRIDENT3_SUPPORT)
        TCB_INIT(unit);
        TCB_LOCK(unit);
        rv = bcm_th2_cosq_tcb_control_get(unit, buffer_id, type, arg);
        TCB_UNLOCK(unit);
        return rv;
#endif
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_cb_register
 * Purpose:
 *      Register a callback for handling TCB freeze events.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      fn - (IN) A pointer to the callback function to call for TCB freeze events.
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_cb_register(int unit, bcm_cosq_tcb_callback_t fn, void *user_data)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);

        if(NULL == tcb_unit_info->tcb_evt_cb) {
            tcb_unit_info->tcb_evt_cb = fn;
            tcb_unit_info->tcb_evt_user_data = user_data;
        } else {
            /* Callback function is already registered */
            rv = BCM_E_EXISTS;
        }

        TCB_UNLOCK(unit);
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_cosq_tcb_cb_unregister
 * Purpose:
 *      Unregister a callback for handling TCB freeze events.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      fn - (IN) A pointer to the callback function to unregister from TCB freeze events
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_cosq_tcb_cb_unregister(int unit, bcm_cosq_tcb_callback_t fn)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_NONE;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    if (soc_feature(unit, soc_feature_tcb)) {
        TCB_INIT(unit);
        TCB_LOCK(unit);

        if(fn == tcb_unit_info->tcb_evt_cb) {
            tcb_unit_info->tcb_evt_cb = NULL;
            tcb_unit_info->tcb_evt_user_data = NULL;
        } else {
            /* Callback function is not registered */
            rv = BCM_E_NOT_FOUND;
        }

        TCB_UNLOCK(unit);
        return rv;
    }
#endif

    return BCM_E_UNAVAIL;
}

int
_bcm_cosq_tcb_uc_queue_resolve(int unit, bcm_cosq_buffer_id_t buffer_id, int queue,
                               bcm_gport_t *uc_gport)
{
#ifdef BCM_TCB_SUPPORT
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        rv = bcm_th2_cosq_tcb_uc_queue_resolve(unit, buffer_id, queue, uc_gport);
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        rv = bcm_hx5_cosq_tcb_uc_queue_resolve(unit, buffer_id, queue, uc_gport);
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)  && !SOC_IS_HELIX5(unit)) {
        rv = bcm_td3_cosq_tcb_uc_queue_resolve(unit, buffer_id, queue, uc_gport);
    }
#endif
    return rv;
#endif
    return BCM_E_UNAVAIL;
}
#endif /* INCLUDE_TCB && BCM_TCB_SUPPORT */

int
_bcm_esw_cosq_num_get(
    int unit,
    int *num_cos)
{
#ifdef BCM_TRIDENT_SUPPORT
    int index, cosq;
    cpu_cos_map_entry_t cpu_cos_map_entry;

    if (SOC_IS_TD_TT(unit)) {
        index = soc_mem_index_count(unit, CPU_COS_MAPm) - _BCM_CPU_COS_MAPS_RSVD +
                (BCM_PRIO_MAX - BCM_PRIO_MIN + 1);
        /* Read the entry */
        SOC_IF_ERROR_RETURN
            (READ_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, index, &cpu_cos_map_entry));

        cosq = soc_mem_field32_get(unit, CPU_COS_MAPm, &cpu_cos_map_entry, COSf);
        *num_cos = cosq + 1;
    }
#endif

    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_cosq_sw_dump(int unit)
{
    mbcm_driver[unit]->mbcm_cosq_sw_dump(unit);
}
#endif /*  BCM_SW_STATE_DUMP_DISABLE */

int bcm_esw_cosq_burst_monitor_init(int unit)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_init(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_detach(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_flow_view_config_set(int unit,
                                                    uint32 options,
                                                    bcm_cosq_burst_monitor_flow_view_info_t *view_cfg)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_flow_view_config_set(unit, options,
                                                                view_cfg);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_flow_view_config_get(int unit,
                                                    bcm_cosq_burst_monitor_flow_view_info_t *view_cfg)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_flow_view_config_get(unit,
                                                              view_cfg);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_flow_view_data_clear(int unit)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_flow_view_data_clear(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_flow_view_data_get(int unit,
                                                  uint32 usr_mem_size,
                                                  uint8 *usr_mem_addr)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_flow_view_data_get(unit, usr_mem_size,
                                                              usr_mem_addr);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_get_current_time(int unit,
                                                uint64 *time_usecs)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_get_current_time(unit,
                                                            time_usecs);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_view_summary_get(int unit, uint32 mem_size,
                                                uint8 *mem_addr, int max_num_views,
                                                int *num_views)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_view_summary_get(unit, mem_size,
                                                            mem_addr, max_num_views,
                                                            num_views);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_flow_view_stats_get(int unit, uint32 mem_size,
                                                   uint8 *mem_addr, int max_num_flows,
                                                   int view_id, uint32 flags, int *num_flows)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_flow_view_stats_get(unit, mem_size,
                                                               mem_addr, max_num_flows,
                                                               view_id, flags, num_flows);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_dma_config_set(int unit,
                                              uint32 host_mem_size,
                                              uint32 **host_mem_addr)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_dma_config_set(unit, host_mem_size,
                                                          host_mem_addr);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        result = bcm_td3_cosq_burst_monitor_dma_config_set(unit, host_mem_size,
                                                           host_mem_addr);
    }
#endif

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_dma_config_get(int unit,
                                              uint32 *host_mem_size,
                                              uint32 **host_mem_addr)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);
    /* QCM app microburst APIs.  */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
       result = bcm_th_cosq_burst_monitor_dma_config_get(unit, host_mem_size,
                                                         host_mem_addr);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
       result = bcm_td3_cosq_burst_monitor_dma_config_get(unit, host_mem_size,
                                                          host_mem_addr);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_set(int unit, int num_gports,
                                   bcm_gport_t *gport_list)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);

    /* QCM app microburst APIs.  */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_set(unit, num_gports,
                                               gport_list);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        result = bcm_td3_cosq_burst_monitor_set(unit, num_gports,
                                                gport_list);
    }
#endif

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

int bcm_esw_cosq_burst_monitor_get(int unit, int  max_gports,
                                   bcm_gport_t *gport_list, int *num_gports)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    COSQ_EVENT_LOCK(unit);

    /* QCM app microburst APIs.  */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        result = bcm_th_cosq_burst_monitor_get(unit, max_gports,
                                               gport_list, num_gports);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
        result = bcm_td3_cosq_burst_monitor_get(unit, max_gports,
                                                gport_list, num_gports);
    }
#endif

    /* Release lock */
    COSQ_EVENT_UNLOCK(unit);
    return result;
}

/*
 * Function:
 *     bcm_esw_cosq_gport_info_get
 * Purpose:
 *     Get cosq gport info.
 * Parameters:
 *     unit             - (IN) unit number
 *     gport            - (IN) priority
 *     info             - (OUT) Info of the given cosq gport
 * Returns:
 *     BCM_E_XXX
 */
 int bcm_esw_cosq_gport_info_get(
     int unit,
     bcm_gport_t gport,
     bcm_cosq_gport_level_info_t *info)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        rv  = bcm_hr4_cosq_gport_info_get(unit, gport, info);
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        rv  = bcm_hx5_cosq_gport_info_get(unit, gport, info);
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        rv  = bcm_fb6_cosq_gport_info_get(unit, gport, info);
    }
#endif
  return rv;
}
int
bcm_esw_obm_cb_register(int unit,
                         bcm_obm_callback_fn  fn,
                         void *fn_data)
{

    int rv = BCM_E_UNAVAIL;

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        rv = _soc_monterey_obm_cb_register(unit ,fn, fn_data);
    }
#endif
    return rv;
}

int
bcm_esw_obm_cb_unregister(int unit,
                         bcm_obm_callback_fn  fn,
                         void *fn_data)
{

    int rv = BCM_E_UNAVAIL;

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        rv = _soc_monterey_obm_cb_unregister(unit,fn, fn_data);
    }
#endif
    return rv;
}


int
bcm_esw_cosq_subport_congestion_config_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cosq_subport_congestion_type_t congestion_type,
                                           bcm_cosq_subport_congestion_config_t *congestion_config)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        rv = bcm_fb6_cosq_subport_congestion_config_set(unit, gport, congestion_type, congestion_config);
    }
#endif
    return rv;
}



int
bcm_esw_cosq_subport_congestion_config_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cosq_subport_congestion_type_t congestion_type,
                                           bcm_cosq_subport_congestion_config_t *congestion_config)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        rv = bcm_fb6_cosq_subport_congestion_config_get(unit, gport, congestion_type, congestion_config);
    }
#endif
    return rv;
}


int
bcm_esw_cosq_subport_pfc_class_mapping_set(int unit,
                                           bcm_gport_t subport_gport,
                                           bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map,
                                           int *cos_to_pfc_map_profile_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        rv = bcm_fb6_cosq_subport_pfc_class_mapping_set(unit, subport_gport, cos_to_pfc_map, cos_to_pfc_map_profile_id);
    }
#endif
    return rv;
}


int
bcm_esw_cosq_subport_pfc_class_mapping_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map)

{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        rv = bcm_fb6_cosq_subport_pfc_class_mapping_get(unit, gport, cos_to_pfc_map);
    }
#endif
    return rv;
}















