/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Timesync Configurations
 * Purpose: API to set and get timesync config profiles for port. 
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_TIMESYNC_V3_SUPPORT) || \
    defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT)
#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/profile_mem.h>

#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/proxy.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif

#include <bcm_int/esw/timesync.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#endif /* PORTMOD_SUPPORT */

/* Timesync module lock */
STATIC sal_mutex_t _bcm_esw_timesync_mutex[BCM_MAX_NUM_UNITS] = {NULL};

/* Timesync control profile    */
static soc_profile_mem_t * _bcm_esw_timesync_control_profile[BCM_MAX_NUM_UNITS]; 
#if defined(BCM_TRIUMPH3_SUPPORT)
/* Timesync mpls label profile */
static soc_profile_mem_t * _bcm_esw_timesync_mpls_label_profile[BCM_MAX_NUM_UNITS];
#endif

/* Timesync defines */
#define TIMESYNC_CONTROL_PROFILE(unit) \
                        _bcm_esw_timesync_control_profile[unit]

#define TIMESYNC_MPLS_LABEL_PROFILE(unit) \
                        _bcm_esw_timesync_mpls_label_profile[unit]

/* Timesync Profile index defines */
#define TS_PORT_INDEX               0
#define TIMESYNC_MAX_PROFILE        1
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
#define TS_MPLS_INDEX_1             1
#define TS_MPLS_INDEX_2             2
#ifdef  TIMESYNC_MAX_PROFILE
#undef  TIMESYNC_MAX_PROFILE
#define TIMESYNC_MAX_PROFILE        3
#endif
#define TIMESYNC_MAX_MPLS_PROFILE   2
#endif

/*
 * TIMESYNC module lock
 */
#define TIMESYNC_MODULE_MUTEX(unit) \
                        _bcm_esw_timesync_mutex[unit]

#define TIMESYNC_MODULE_LOCK(unit) \
        sal_mutex_take(_bcm_esw_timesync_mutex[unit], sal_mutex_FOREVER);

#define TIMESYNC_MODULE_UNLOCK(unit) \
        sal_mutex_give(_bcm_esw_timesync_mutex[unit]);

/* Return On error with link mutex unlocked */
#define _BCM_TIMESYNC_IF_ERROR_RETURN(op)  \
    do {                                        \
        int __rv__;                             \
        if (((__rv__ = (op)) < 0)) {            \
            TIMESYNC_MODULE_UNLOCK(unit);                    \
            return(__rv__);                     \
        }                                       \
    } while(0)

/*
 * Function:
 *      _bcm_esw_port_timesync_profile_init
 * Purpose: 
 *      This function initializes timesync port and mpls profiles
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_port_timesync_profile_init(int unit)
{
    soc_mem_t   mem = 0;
    int         entry_words, alloc_size=0;
    void        *entry;
    uint32      base_index;
    int         rv = BCM_E_NONE;
  
    /*
     * Initialize Timesync control profile 
     */
    if (TIMESYNC_CONTROL_PROFILE(unit) == NULL) {
        TIMESYNC_CONTROL_PROFILE(unit) = sal_alloc (sizeof(soc_profile_mem_t),
                                              "Timesync control profile mem");
        if (TIMESYNC_CONTROL_PROFILE(unit) == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(TIMESYNC_CONTROL_PROFILE(unit));
    } else {
        soc_profile_mem_destroy(unit, TIMESYNC_CONTROL_PROFILE(unit));
    }

     /*
      * Initialize Timesync mutex 
      */
    if (TIMESYNC_MODULE_MUTEX(unit) == NULL) { 
        TIMESYNC_MODULE_MUTEX(unit) = sal_mutex_create ("timesync_mutex");
        if (TIMESYNC_MODULE_MUTEX(unit) == NULL) { 
            return BCM_E_MEMORY; 
        } 
    } 

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_v3) || 
        SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) ||
        SOC_IS_APACHE(unit)) {
        mem = ING_1588_INGRESS_CTRLm;
        entry_words = sizeof(ing_1588_ingress_ctrl_entry_t) / sizeof(uint32);
        alloc_size = sizeof(ing_1588_ingress_ctrl_entry_t);
    }
#endif /* BCM_TIMESYNC_V3_SUPPORT || BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_SABER2(unit)) {
        mem =  ING_1588_TS_DISPOSITION_PROFILE_TABLEm;
        entry_words = sizeof(ing_1588_ts_disposition_profile_table_entry_t) / sizeof(uint32);
        alloc_size = sizeof(ing_1588_ts_disposition_profile_table_entry_t);
    }
#endif /*defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT) */

    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                TIMESYNC_CONTROL_PROFILE(unit)));

    /* Add timesync control default entry */
    entry = sal_alloc(alloc_size, "Timesync control profile mem");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);
    rv = soc_profile_mem_add(unit, TIMESYNC_CONTROL_PROFILE(unit), &entry, 1,
                             &base_index);
    sal_free(entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    /*
     * Initialize Timesync mpls label profile 
     */
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_SABER2(unit) || SOC_IS_KATANA2(unit)) {

        if (TIMESYNC_MPLS_LABEL_PROFILE(unit) == NULL) {
            TIMESYNC_MPLS_LABEL_PROFILE(unit) = sal_alloc (sizeof(soc_profile_mem_t),
                                                  "Timesync mpls label profile mem");
            if (TIMESYNC_MPLS_LABEL_PROFILE(unit) == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(TIMESYNC_MPLS_LABEL_PROFILE(unit));
        } else {
            soc_profile_mem_destroy(unit, TIMESYNC_MPLS_LABEL_PROFILE(unit));
        }

        mem = PTP_LABEL_RANGE_PROFILE_TABLEm;
        entry_words = sizeof(ptp_label_range_profile_table_entry_t) / sizeof(uint32);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                    TIMESYNC_MPLS_LABEL_PROFILE(unit)));

        /* Add timesync mpls label default entry */
        alloc_size = sizeof(ptp_label_range_profile_table_entry_t);
        entry = sal_alloc(alloc_size, "Timesync mpls label profile mem");
        if (entry == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(entry, 0, alloc_size);
        rv = soc_profile_mem_add(unit, TIMESYNC_MPLS_LABEL_PROFILE(unit), &entry, 1,
                                 &base_index);
        sal_free(entry);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

    }
#endif /* BCM_TRIUMPH3_SUPPORT  || BCM_SABER2_SUPPORT */

    /* Clear memories/registers on cold boot only. */
    if (!SOC_WARM_BOOT(unit)) {
#if defined(BCM_TSCF_GEN3_PCS_TIMESTAMP)
         /*
          * MAC timestamping is disabled and PCS timestamping is enabled
          * by default on PM4x25 gen3 port macro, and PCS only support 48-bit.
          * Thus, default set to 48-bit.
          */
        if (soc_feature(unit, soc_feature_tscf_gen3_pcs_timestamp)) {
            int port;
            PBMP_PORT_ITER(unit, port) {
                _bcm_esw_port_timesync_timestamping_mode_set
                    (unit, port, bcmTimesyncTimestampingMode48bit);
            }
        }
#endif /* BCM_TSCF_GEN3_PCS_TIMESTAMP */
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_profile_delete
 * Purpose: 
 *      This function deletes timesync config profiles
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_port_timesync_profile_delete(int unit)
{
    /* Destroy Timesync Control profile */
    if (TIMESYNC_CONTROL_PROFILE(unit)) {
        BCM_IF_ERROR_RETURN(
            soc_profile_mem_destroy(unit, TIMESYNC_CONTROL_PROFILE(unit)));
        sal_free(TIMESYNC_CONTROL_PROFILE(unit));
        TIMESYNC_CONTROL_PROFILE(unit) = NULL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    /* Destroy Timesync Mpls Label profile */
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_SABER2(unit) || SOC_IS_KATANA2(unit)) {
        if (TIMESYNC_MPLS_LABEL_PROFILE(unit)) {
            BCM_IF_ERROR_RETURN(
                soc_profile_mem_destroy(unit, TIMESYNC_MPLS_LABEL_PROFILE(unit)));
            sal_free(TIMESYNC_MPLS_LABEL_PROFILE(unit));
            TIMESYNC_MPLS_LABEL_PROFILE(unit) = NULL;
        }
    }
#endif

     /* Destroy Timesync mutex  */
    if (TIMESYNC_MODULE_MUTEX(unit)) { 
        sal_mutex_destroy(TIMESYNC_MODULE_MUTEX(unit));
        TIMESYNC_MODULE_MUTEX(unit) = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_contorl_profile_entry_add
 * Purpose: 
 *      Internal Funciton adds timesync control profiles
 *
 * Parameters: 
 *      unit      -  (IN) Device number.
 *      ts_config -  (IN) Timesync config 
 *      *index    -  (OUT) profile index
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_port_timesync_control_profile_entry_add(int unit, 
                                bcm_port_timesync_config_t *ts_config, uint32 *index)
{
    int         alloc_size=0;
    void        *entry;
    int         cnt, rv = BCM_E_NONE;

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_v3) || 
        SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
        alloc_size = sizeof(ing_1588_ingress_ctrl_entry_t);
    }
#endif 
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_SABER2(unit)) {
        alloc_size = sizeof(ing_1588_ts_disposition_profile_table_entry_t);
    }
#endif 

    entry = sal_alloc(alloc_size, "Timesync control profile mem");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);


    /* Set timesync message control tocpu/drop fields */
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_v3) || 
        SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
        for ( cnt = 0; cnt < BYTES2BITS(sizeof(uint32)); cnt++ )
        {
            switch (ts_config->pkt_drop & (1 << cnt))
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_SYNC_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_DELAY_REQ_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_PDELAY_REQ_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_PDELAY_RESP_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_FOLLOW_UP_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_DELAY_RESP_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                         RX_TS_PDELAY_RESP_FOLLOW_UP_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_MSG_TYPE_11_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_MSG_TYPE_12_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_MSG_TYPE_13_DROPf, 1);
                    break;
                default:
                    break;
            }

            switch (ts_config->pkt_tocpu & (1 << cnt))
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                               RX_TS_SYNC_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_DELAY_REQ_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_PDELAY_REQ_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_PDELAY_RESP_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_FOLLOW_UP_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_DELAY_RESP_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_PDELAY_RESP_FOLLOW_UP_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_MSG_TYPE_11_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_MSG_TYPE_12_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    soc_ING_1588_INGRESS_CTRLm_field32_set(unit, entry, 
                                                RX_TS_MSG_TYPE_13_TO_CPUf, 1);
                    break;
                default:
                    break;
            }
        }

    }
#endif /* BCM_TIMESYNC_V3_SUPPORT || BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_KATANAX(unit) ||  SOC_IS_SABER2(unit)) {
        for ( cnt = 0; cnt < BYTES2BITS(sizeof(uint32)); cnt++ )
        {
            switch (ts_config->pkt_drop & (1 << cnt))
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_0_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_1_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_2_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_3_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_8_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_9_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_10_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_11_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_12_DROPf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_13_DROPf, 1);
                    break;
                default:
                    break;
            }

            switch (ts_config->pkt_tocpu & (1 << cnt))
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_0_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_1_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_2_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_3_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_8_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_9_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_10_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_11_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_12_COPY_TO_CPUf, 1);
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_set(unit, entry, 
                                                MESSAGE_TYPE_13_COPY_TO_CPUf, 1);
                    break;
                default:
                    break;
            }
        }
    }
#endif /* BCM_KATANA_SUPPORT || BCM_SABER2_SUPPORT */
        
        /* lock and unlocktimesync module for profile operations */
        TIMESYNC_MODULE_LOCK(unit);
        rv = soc_profile_mem_add(unit, TIMESYNC_CONTROL_PROFILE(unit), &entry, 1,
                                  index);
        TIMESYNC_MODULE_UNLOCK(unit);
        sal_free(entry);
        return rv;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_contorl_profile_entry_get
 * Purpose: 
 *      Interal function gets timesync control profiles for the index
 *
 * Parameters: 
 *      unit      -  (IN) Device number.
 *      ts_config -  (OUT) Timesync config 
 *      index    -   (IN) profile index
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_bcm_esw_port_timesync_control_profile_entry_get(int unit, 
                                bcm_port_timesync_config_t *ts_config, uint32 index)
{
    int         alloc_size=0;
    void        *entry;
    int         cnt, rv = BCM_E_NONE;

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_v3) || 
        SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
        alloc_size = sizeof(ing_1588_ingress_ctrl_entry_t);
    }
#endif /* BCM_TIMESYNC_V3_SUPPORT || BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_SABER2(unit)) {
        alloc_size = sizeof(ing_1588_ts_disposition_profile_table_entry_t);
    }
#endif /* BCM_KATANA_SUPPORT ||  BCM_SABER2_SUPPORT */

    entry = sal_alloc(alloc_size, "Timesync control profile mem");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);

    TIMESYNC_MODULE_LOCK(unit);
    rv = soc_profile_mem_get(unit, TIMESYNC_CONTROL_PROFILE(unit), index, 1, &entry );
    if (BCM_FAILURE(rv)) {
        sal_free(entry);
        TIMESYNC_MODULE_UNLOCK(unit);
        return rv;
    }

    /* Set timesync message control tocpu/drop fields */
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_v3) || 
        SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
        for ( cnt = 0; cnt < BYTES2BITS(sizeof(uint32)); cnt++ )
        {
            switch (1 << cnt)
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                                RX_TS_SYNC_DROPf)) {
                        ts_config->pkt_drop |=  BCM_PORT_TIMESYNC_PKT_SYNC; 
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                                RX_TS_DELAY_REQ_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_DELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                                RX_TS_PDELAY_REQ_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    if(soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                                RX_TS_PDELAY_RESP_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                                RX_TS_FOLLOW_UP_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                                RX_TS_DELAY_RESP_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_DELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                       RX_TS_PDELAY_RESP_FOLLOW_UP_DROPf)) {
                        ts_config->pkt_drop |=
                                BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry,
                                              RX_TS_MSG_TYPE_11_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_ANNOUNCE;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                            RX_TS_MSG_TYPE_12_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_SIGNALLING;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                             RX_TS_MSG_TYPE_13_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_MANAGMENT;
                    }
                    break;
                default:
                    break;
            }

            switch (1 << cnt)
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                                RX_TS_SYNC_TO_CPUf)) {
                        ts_config->pkt_tocpu |=  BCM_PORT_TIMESYNC_PKT_SYNC; 
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                                RX_TS_DELAY_REQ_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_DELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                               RX_TS_PDELAY_REQ_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    if(soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                              RX_TS_PDELAY_RESP_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                             RX_TS_FOLLOW_UP_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                              RX_TS_DELAY_RESP_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_DELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                           RX_TS_PDELAY_RESP_FOLLOW_UP_TO_CPUf)) {
                        ts_config->pkt_tocpu |=
                                BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                             RX_TS_MSG_TYPE_11_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_ANNOUNCE;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                              RX_TS_MSG_TYPE_12_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_SIGNALLING;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    if (soc_ING_1588_INGRESS_CTRLm_field32_get(unit, entry, 
                                             RX_TS_MSG_TYPE_13_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_MANAGMENT;
                    }
                    break;
                default:
                    break;
            }
        }
    }
#endif /* BCM_TIMESYNC_V3_SUPPORT || BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_KATANAX(unit) ||  SOC_IS_SABER2(unit)) {
        for ( cnt = 0; cnt < BYTES2BITS(sizeof(uint32)); cnt++ )
        {
            switch (1 << cnt)
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                           unit, entry,MESSAGE_TYPE_0_DROPf)) {
                        ts_config->pkt_drop |=  BCM_PORT_TIMESYNC_PKT_SYNC; 
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                           unit, entry, MESSAGE_TYPE_1_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_DELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                           unit, entry, MESSAGE_TYPE_2_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    if(soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                       unit, entry, MESSAGE_TYPE_3_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                       unit, entry, MESSAGE_TYPE_8_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                       unit, entry, MESSAGE_TYPE_9_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_DELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                      unit, entry, MESSAGE_TYPE_10_DROPf)) {
                        ts_config->pkt_drop |=
                                BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                      unit, entry, MESSAGE_TYPE_11_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_ANNOUNCE;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                      unit, entry, MESSAGE_TYPE_12_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_SIGNALLING;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                       unit, entry, MESSAGE_TYPE_13_DROPf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_MANAGMENT;
                    }
                    break;
                default:
                    break;
            }

            switch (1 << cnt)
            {
                case BCM_PORT_TIMESYNC_PKT_SYNC:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                          unit, entry, MESSAGE_TYPE_0_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |=  BCM_PORT_TIMESYNC_PKT_SYNC; 
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_REQ:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                          unit, entry, MESSAGE_TYPE_1_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_DELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_REQ:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                    unit, entry, MESSAGE_TYPE_2_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP:
                    if(soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_3_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_FOLLOWUP:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_8_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_DELAY_RESP:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_9_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_DELAY_RESP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_10_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |=
                                BCM_PORT_TIMESYNC_PKT_PDELAY_RESP_FOLLOWUP;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_ANNOUNCE:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_11_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_ANNOUNCE;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_SIGNALLING:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_12_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_SIGNALLING;
                    }
                    break;
                case BCM_PORT_TIMESYNC_PKT_MANAGMENT:
                    if (soc_ING_1588_TS_DISPOSITION_PROFILE_TABLEm_field32_get(
                                     unit, entry, MESSAGE_TYPE_13_COPY_TO_CPUf)) {
                        ts_config->pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_MANAGMENT;
                    }
                    break;
                default:
                    break;
            }
        }
    }
#endif /* BCM_KATANA_SUPPORT ||  BCM_SABER2_SUPPORT */

        sal_free(entry);
        TIMESYNC_MODULE_UNLOCK(unit);
        return rv;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_port_profile_entry_delete
 * Purpose: 
 *      Internal function to delete timesync profiles
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_port_timesync_port_profile_entry_delete(int unit, int index)
{
    int rv;
    TIMESYNC_MODULE_LOCK(unit);
    rv = soc_profile_mem_delete(unit, TIMESYNC_CONTROL_PROFILE(unit), index);
    TIMESYNC_MODULE_UNLOCK(unit);
    return rv;
}

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
/*
 * Function:
 *      _bcm_esw_port_timesync_mpls_profile_entry_add
 * Purpose: 
 *      Internal Function to set timesync mpls profiles
 *
 * Parameters: 
 *      unit      -  (IN) Device number.
 *      ts_config -  (IN) Timesync config 
 *      *index    -   (OUT) profile index
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_bcm_esw_port_timesync_mpls_profile_entry_add(int unit, 
                                bcm_port_timesync_config_t *ts_config, uint32 *index)
{
    int         alloc_size;
    void        *entry;
    uint32      control_index;
    int         rv = BCM_E_NONE;

    /* Create timesync control index */
    rv = _bcm_esw_port_timesync_control_profile_entry_add(unit, ts_config,
                                                      &control_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Add  and enable mpls label range */
    alloc_size = sizeof(ptp_label_range_profile_table_entry_t);
    entry = sal_alloc(alloc_size, "Timesync mpls label profile mem");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);

   
    /* set timesync mpls label range */ 
    soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_set(unit, entry,
                            MIN_LABEL_OF_RANGEf, ts_config->mpls_min_label);
    soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_set(unit, entry,
                            MAX_LABEL_OF_RANGEf, ts_config->mpls_max_label);
    if (SOC_IS_SABER2(unit) || SOC_IS_KATANA2(unit)) {
        soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_set(unit, entry,
                            ING_1588_TS_PROFILE_PTRf, control_index);
    } else {
        soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_set(unit, entry,
                            CTRL_PROFILE_INDEX_1588f, control_index);
    }
    soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_set(unit, entry,
                            ENABLE_1588OMPLSf, 1);
  
    /* Set timesync mpls control profiles */ 
    TIMESYNC_MODULE_LOCK(unit); 
    rv = soc_profile_mem_add(unit, TIMESYNC_MPLS_LABEL_PROFILE(unit), &entry, 1,
                             index);
    sal_free(entry);
    TIMESYNC_MODULE_UNLOCK(unit); 
    return rv;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_mpls_profile_entry_get
 * Purpose: 
 *      Internal Function to get timesync mpls profiles
 *
 * Parameters: 
 *      unit      -  (IN) Device number.
 *      ts_config -  (OUT) Timesync config 
 *      index    -   (IN) profile index
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_port_timesync_mpls_profile_entry_get(int unit, 
                            bcm_port_timesync_config_t *ts_config, uint32 index)
{
    int         alloc_size;
    void        *entry;
    int         control_index;
    int         rv = BCM_E_NONE;

    /* Add  and enable mpls label range */
    alloc_size = sizeof(ptp_label_range_profile_table_entry_t);
    entry = sal_alloc(alloc_size, "Timesync mpls label profile mem");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry, 0, alloc_size);

    TIMESYNC_MODULE_LOCK(unit); 
    rv = soc_profile_mem_get(unit, TIMESYNC_MPLS_LABEL_PROFILE(unit), index, 1,
                             &entry);

    if (BCM_FAILURE(rv)) {
        sal_free(entry);
        TIMESYNC_MODULE_UNLOCK(unit);
        return rv;
    }

    /* get mpls label range */
    ts_config->mpls_min_label = soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_get(
                                            unit, entry, MIN_LABEL_OF_RANGEf);
    ts_config->mpls_max_label = soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_get(
                                            unit, entry, MAX_LABEL_OF_RANGEf);
    if (SOC_IS_SABER2(unit) || SOC_IS_KATANA2(unit)) {
        control_index = soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_get(
                                        unit, entry, ING_1588_TS_PROFILE_PTRf);
    } else {
        control_index = soc_PTP_LABEL_RANGE_PROFILE_TABLEm_field32_get(
                                        unit, entry, CTRL_PROFILE_INDEX_1588f);
    }
    /* get mpls control profiles */
    if (control_index > 0) { 
        rv = _bcm_esw_port_timesync_control_profile_entry_get(unit, ts_config,
                                                            control_index);
        if (BCM_FAILURE(rv)) {
            sal_free(entry);
            TIMESYNC_MODULE_UNLOCK(unit); 
            return rv;
        }
    }
    sal_free(entry);
    TIMESYNC_MODULE_UNLOCK(unit); 
    return rv;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_mpls_profile_entry_delete
 * Purpose: 
 *      Internal Function to delete timesync mpls profile
 *
 * Parameters: 
 *      unit       - (IN) bcm device
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_bcm_esw_port_timesync_mpls_profile_entry_delete(int unit, int index)
{
    int rv;
    TIMESYNC_MODULE_LOCK(unit);
    rv = soc_profile_mem_delete(unit, TIMESYNC_MPLS_LABEL_PROFILE(unit), index);
    TIMESYNC_MODULE_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT  || BCM_SABER2_SUPPORT*/

/*
 * Function: 
 *      _bcm_esw__port_timesync_config_set
 * Purpose: 
 *      Set Timesync Configuration profiles for the port
 *
 * Parameters: 
 *      unit            - (IN) bcm device
 *      port            - (IN) port
 *      config_count    - (IN)Count of timesync profile
 *      *config_array   - (IN/OUT) Pointer to array of timesync profiles
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 *     Timesync config set function cleans up previous profiles before setting new
 *     profiles. katana support one timesync port profile and triumph3 supports
 *     one port profile and two mpls timesync profiles. 
 */
int
_bcm_esw_port_timesync_config_set(int unit, bcm_port_t port, int config_count,
                                  bcm_port_timesync_config_t *config_array, int is_remote_port)
{
    int                 cnt, rv = BCM_E_NONE;
    int                 is_proxy_higig = 0, is_proxy_hybrid = 0;
    int                 prev_index = 0;
    uint32              rval, index;
    soc_mem_t           mem;
    egr_1588_sa_entry_t sa_entry;
    port_tab_entry_t    ptab;
    bcm_port_timesync_config_t *ts_config;
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    int                 mpls_index = 0;
    int                 mpls_prev_index = 0;
#endif
    int                 cf_sw_update = 0;

    /* Validate Parameters */
    if ((config_count > TIMESYNC_MAX_PROFILE)
        || ((config_count > 0) && (NULL == config_array))
        || ((config_count == 0) && (NULL != config_array))) {
        return BCM_E_PARAM;
    }

    cf_sw_update = (soc_feature(unit, soc_feature_ptp_cf_sw_update) &&
                    soc_property_get(unit, spn_PTP_CF_SW_UPDATE, 0));

    /* Set timesync profiles */
    for (cnt = 0; cnt < config_count; cnt++)
    {
        ts_config = (bcm_port_timesync_config_t*) (config_array + cnt);
        
        if (ts_config->flags & BCM_PORT_TIMESYNC_DEFAULT) {
            /* Set one-step timestamp configurations */
            if ( ts_config->flags & BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP ) {
    
                /* Enable Correction updates for ingress and egress */

                soc_reg_field32_modify(unit, EGR_1588_INGRESS_CTRLr, port,
                                       CF_UPDATE_ENABLEf, !cf_sw_update ? 1 : (IS_CPU_PORT(unit, port) ? 1 : 0));
#if defined(BCM_KATANA_SUPPORT)
                if (SOC_IS_KATANA(unit)) {
                    SOC_IF_ERROR_RETURN(
                      soc_reg_field32_modify(unit, EGR_1588_EGRESS_CTRLr, port,
                                             CF_UPDATE_ENABLEf, 1));
                }
#endif
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_KATANA2_SUPPORT) || defined(BCM_SABER2_SUPPORT) || \
    defined(BCM_HELIX4_SUPPORT)
                if (soc_feature(unit, soc_feature_timesync_v3) || 
                    SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) || 
                    SOC_IS_KATANA2(unit) || SOC_IS_SABER2(unit) ||
                    SOC_IS_HELIX4(unit) || SOC_IS_APACHE(unit) ) {
                    if (ts_config->flags &
                        BCM_PORT_TIMESYNC_TIMESTAMP_CFUPDATE_ALL) {
                        SOC_IF_ERROR_RETURN(
                          soc_reg_field32_modify(unit, EGR_1588_EGRESS_CTRLr, port,
                                                 CF_UPDATE_MODEf, 1));

                    }
                    else {
                        /* TR3: The ingress port is default enabled for correction
                         * updates for one-step timestamping and hence the
                         * EGR update mode is set to ingress based correction
                         * updates.
                        */
                        SOC_IF_ERROR_RETURN(
                          soc_reg_field32_modify(unit, EGR_1588_EGRESS_CTRLr, port,
                                                 CF_UPDATE_MODEf, 2));
                    }
                }
#endif

#if defined(BCM_KATANA2_SUPPORT)
                /* KT2: Enabling 48bit timestamping mode */
                if (SOC_IS_KATANA2(unit)) {
                    SOC_IF_ERROR_RETURN(
                        soc_reg_field32_modify(unit, EGR_1588_PARSING_CONTROLr,
                                                port, TIMESTAMPING_MODEf, 1));
                }
#endif

                /* Set sign extension from timestamp field, for ingress 
                 * Front Panel port or  HG Proxy port
                 */
#if defined(INCLUDE_L3)
                if (soc_feature(unit, soc_feature_higig_lookup)) {
                    bcm_esw_proxy_server_get(unit, port, 
                                        BCM_PROXY_MODE_HIGIG, &is_proxy_higig);
                    bcm_esw_proxy_server_get(unit, port, 
                                        BCM_PROXY_MODE_HYBRID, &is_proxy_hybrid);
                }
#endif /* INCLUDE_L3 */

                if ((IS_E_PORT(unit, port) && !SOC_IS_STACK_PORT(unit, port)) ||
                     (IS_HG_PORT(unit, port) && 
                      (is_proxy_higig || is_proxy_hybrid))) {
                      soc_reg_field32_modify(unit, EGR_1588_INGRESS_CTRLr, port,
                                             FORCE_ITS_SIGN_FROM_TSf, 1);
                } else {
                      soc_reg_field32_modify(unit, EGR_1588_INGRESS_CTRLr, port,
                                             FORCE_ITS_SIGN_FROM_TSf, 0);

                }

                /* Enable Source Address update for corrections */
                if (!BCM_MAC_IS_ZERO(ts_config->src_mac_addr)) {
                    SOC_IF_ERROR_RETURN(
                        soc_reg_field32_modify(unit, EGR_1588_EGRESS_CTRLr,
                                               port, SA_UPDATE_ENABLEf, 1));
                    sal_memset(&sa_entry, 0, sizeof(egr_1588_sa_entry_t));
                    soc_mem_mac_addr_set(unit, EGR_1588_SAm , &sa_entry,
                                      SAf, ts_config->src_mac_addr);
                    BCM_IF_ERROR_RETURN( 
                        soc_mem_write(unit, EGR_1588_SAm, MEM_BLOCK_ALL, 
                                      port, &sa_entry));
                }
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
                /* Disable automatic timestamp offset balancing in UNIMAC */
                if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
                    SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
                    if (!IS_XL_PORT(unit, port) && !IS_CL_PORT(unit, port) && !IS_CPU_PORT(unit, port)) {
                        if (SOC_REG_IS_VALID(unit, UMAC_TIMESTAMP_ADJUSTr) &&
                            SOC_REG_FIELD_VALID(
                                unit, UMAC_TIMESTAMP_ADJUSTr, AUTO_ADJUSTf) &&
                            SOC_REG_FIELD_VALID(
                                unit, UMAC_TIMESTAMP_ADJUSTr, ADJUSTf) &&
                            SOC_REG_FIELD_VALID(
                                unit, UMAC_TIMESTAMP_ADJUSTr, EN_1588f)) {

                            SOC_IF_ERROR_RETURN(
                                READ_UMAC_TIMESTAMP_ADJUSTr(
                                    unit, port, &rval));
                            soc_reg_field_set(
                                unit, UMAC_TIMESTAMP_ADJUSTr, &rval,
                                AUTO_ADJUSTf, 0);
                            soc_reg_field_set(
                                unit, UMAC_TIMESTAMP_ADJUSTr, &rval,
                                ADJUSTf, 0);
                            soc_reg_field_set(
                                unit, UMAC_TIMESTAMP_ADJUSTr, &rval,
                                EN_1588f, 1);
                            SOC_IF_ERROR_RETURN(
                                WRITE_UMAC_TIMESTAMP_ADJUSTr(
                                    unit, port, rval));
                        }
                    }
                }
#endif /* BCM_HURRICANE2_SUPPORT || BCM_GREYHOUND_SUPPORT */
            }

                /* Set two-step correction update enable */
            if ( ts_config->flags & BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP) {
                /* Set two-step timestamping in mac for all event pkts */
                SOC_IF_ERROR_RETURN(
                    READ_EGR_1588_EGRESS_CTRLr(unit, port,&rval));
                soc_reg_field_set(unit, EGR_1588_EGRESS_CTRLr, &rval, 
                                  TX_TS_SYNCf, 1);
                soc_reg_field_set(unit, EGR_1588_EGRESS_CTRLr, &rval, 
                                  TX_TS_DELAY_REQf, 1);
                soc_reg_field_set(unit, EGR_1588_EGRESS_CTRLr, &rval, 
                                  TX_TS_PDELAY_REQf, 1);
                soc_reg_field_set(unit, EGR_1588_EGRESS_CTRLr, &rval, 
                                  TX_TS_PDELAY_RESPf, 1);
                SOC_IF_ERROR_RETURN(
                    WRITE_EGR_1588_EGRESS_CTRLr(unit, port, rval));
            }

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT)
            if (soc_feature(unit, soc_feature_timesync_v3) || 
                SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) ||
                SOC_IS_SABER2(unit) || SOC_IS_APACHE(unit) || SOC_IS_KATANA2(unit)) {
                if (ts_config->pkt_drop & BCM_PORT_TIMESYNC_PKT_INVALID) {
                    SOC_IF_ERROR_RETURN(
                          soc_reg_field32_modify(unit, EGR_1588_EGRESS_CTRLr, port,
                                                 DROP_INVALID_1588_PKTf, 1));
                }
            }
#endif
            /* Create timesync control index */
            rv = _bcm_esw_port_timesync_control_profile_entry_add(unit, ts_config,
                                                              &index);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            if (SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, port,
                    CTRL_PROFILE_INDEX_1588f, &prev_index));
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                    _BCM_CPU_TABS_ETHER, CTRL_PROFILE_INDEX_1588f, index));
                BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                    _BCM_CPU_TABS_ETHER, IEEE_802_1AS_ENABLEf, 1));
            } else {
                soc_mem_lock(unit, PORT_TABm);
                mem = SOC_PORT_MEM_TAB(unit, port);
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ptab);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, PORT_TABm);
                    return BCM_E_FAIL;

                }

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                if (soc_feature(unit, soc_feature_timesync_v3) ||
                    SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
                    prev_index = soc_PORT_TABm_field32_get(unit,
                                              &ptab, CTRL_PROFILE_INDEX_1588f);
                    soc_PORT_TABm_field32_set(unit, &ptab,
                                              CTRL_PROFILE_INDEX_1588f, index);
                    /* Enable Timesync for the port */
                    soc_PORT_TABm_field32_set(unit, &ptab,
                                              IEEE_802_1AS_ENABLEf, 1);
                }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
                if (SOC_IS_KATANAX(unit) || SOC_IS_SABER2(unit)) {
                    prev_index = soc_PORT_TABm_field32_get(unit,  &ptab,
                                              ING_1588_TS_PROFILE_PTRf);
                    soc_PORT_TABm_field32_set(unit, &ptab,
                                              ING_1588_TS_PROFILE_PTRf, index);
                    /* Enable Timesync for the port */
                    soc_PORT_TABm_field32_set(unit, &ptab,
                                              ING_1588_TS_ENABLEf, 1);
                }
#endif
                rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &ptab);
                soc_mem_unlock(unit, PORT_TABm);
            }

            /* delete old config profile */
            if (prev_index > 0) {
                _bcm_esw_port_timesync_port_profile_entry_delete(unit, prev_index);
            }

        }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
        /* Set Timesync MPLS label profile */
        if ( ts_config->flags & BCM_PORT_TIMESYNC_MPLS ) {

            if (SOC_IS_TD2_TT2(unit)) {
                return BCM_E_UNAVAIL;
            }

            /* Check for max mpls profile entries */ 
            if (++mpls_index > TIMESYNC_MAX_PROFILE - 1) {
                return BCM_E_PARAM;
            }

            /* Create timesync mpls index */
            rv = _bcm_esw_port_timesync_mpls_profile_entry_add(unit, ts_config,
                                                              &index);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

            soc_mem_lock(unit, PORT_TABm);;
            mem = SOC_PORT_MEM_TAB(unit, port);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ptab);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, PORT_TABm);;
                return BCM_E_FAIL;

            }

            switch (mpls_index)
            {
                case TS_MPLS_INDEX_1:
                    mpls_prev_index = 
                        soc_PORT_TABm_field32_get(unit, &ptab, 
                                                  PTP_LABEL_RANGE_INDEX_1f);
                    soc_PORT_TABm_field32_set(unit, &ptab, 
                                              PTP_LABEL_RANGE_INDEX_1f,index);
                    break;
                case TS_MPLS_INDEX_2:
                    mpls_prev_index = 
                        soc_PORT_TABm_field32_get(unit,&ptab, 
                                                  PTP_LABEL_RANGE_INDEX_2f);
                    soc_PORT_TABm_field32_set(unit, &ptab, 
                                              PTP_LABEL_RANGE_INDEX_2f,index);
                    break;
                /* coverity [dead_error_begin] */
                default:
                    mpls_prev_index = 0;
                    break;
            }

            /* Enable 1588 over MPLS for the port */
            soc_PORT_TABm_field32_set(unit, &ptab, ENABLE_1588OMPLSf, 1 );

            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &ptab);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, PORT_TABm);;
                return BCM_E_FAIL;

            }

            soc_mem_unlock(unit, PORT_TABm);;
           
            /* Delete Previous Mpls indexes */
            if (mpls_index == TS_MPLS_INDEX_1 && mpls_prev_index != 0) {
                _bcm_esw_port_timesync_mpls_profile_entry_delete(unit,
                                    mpls_prev_index);
            }

            if (mpls_index == TS_MPLS_INDEX_2 && mpls_prev_index != 0) {
                _bcm_esw_port_timesync_mpls_profile_entry_delete(unit,
                                        mpls_prev_index);
            } 

        }
#endif
    }

    /* Disable timesync for NULL timesync configuration */
    if (config_count == 0) {
        if (!is_remote_port) {
            /* Disable one step and two step timesync */
            SOC_IF_ERROR_RETURN(
                WRITE_EGR_1588_EGRESS_CTRLr(unit, port, 0));
            SOC_IF_ERROR_RETURN(
                WRITE_EGR_1588_INGRESS_CTRLr(unit, port, 0));
        }

        if (SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_set(unit, port,
                _BCM_CPU_TABS_BOTH, IEEE_802_1AS_ENABLEf, 0));
        } else {
            /* Disable timesync packet parsing */
            soc_mem_lock(unit, PORT_TABm);
            mem = SOC_PORT_MEM_TAB(unit, port);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ptab);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, PORT_TABm);;
                return BCM_E_FAIL;

            }
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT)
            if (soc_feature(unit, soc_feature_timesync_v3) ||
                SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) ||
                SOC_IS_SABER2(unit)) {
                if (!SOC_IS_SABER2(unit)) {
                    /* Disable Timesync for the port */
                    soc_PORT_TABm_field32_set(unit, &ptab,
                            IEEE_802_1AS_ENABLEf, 0);
                }
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
                if (SOC_MEM_FIELD_VALID(unit, mem, ENABLE_1588OMPLSf)) {
                    soc_PORT_TABm_field32_set(unit, &ptab,
                                          ENABLE_1588OMPLSf, 0);
                }
#endif
            }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_KATANAX(unit) || SOC_IS_SABER2(unit)) {
                /* Disable Timesync for the port */
                soc_PORT_TABm_field32_set(unit, &ptab,
                                          ING_1588_TS_ENABLEf, 0);

            }
#endif
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &ptab);
            soc_mem_unlock(unit, PORT_TABm);;
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_port_timesync_config_get
 * Purpose: 
 *      Get Timesync configuration profiles for the port.
 *
 * Parameters: 
 *      unit            - (IN) bcm device
 *      port            - (IN) port
 *      array_size      - (IN) Count of timesync profile
 *      *config_array   - (IN/OUT) Pointer to array of timesync profiles
 *      *array_count    - (OUT) Pointer to array of timesync profiles
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 * Notes:
 *     Timesync config get function is overloaded to return supported profile 
 *     counts when config_array is passed as null. 
 */
int
_bcm_esw_port_timesync_config_get(int unit, bcm_port_t port, int array_size,
                                  bcm_port_timesync_config_t *config_array, 
                                  int *array_count, int is_remote_port)
{
    int                 cnt, config_cnt =0, rv = BCM_E_NONE;
    uint32              rval, rval_1, index = 0, value;
    uint32              ts_index[TIMESYNC_MAX_PROFILE];
    soc_mem_t           mem;
    egr_1588_sa_entry_t sa_entry;
    port_tab_entry_t    ptab;
    bcm_port_timesync_config_t *ts_config;

    /* Check for array_count */
    if ((NULL == array_count) ||
       (array_size > 0 && NULL == config_array)) {
        return BCM_E_PARAM;
    }

    /* Read Port Table, do limited operations, holding port tab memory lock*/
    sal_memset(ts_index, 0, sizeof(ts_index));
    ts_index[TS_PORT_INDEX] = 0xffffffff;

    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, port,
            CTRL_PROFILE_INDEX_1588f, (int *)&index));
        /* Store all 1588 port indexes for later use */
        if (ts_index[TS_PORT_INDEX] == 0xffffffff) {
            ts_index[TS_PORT_INDEX] = index;
            config_cnt++;
        }
    } else {
        mem = SOC_PORT_MEM_TAB(unit, port);
        soc_mem_lock(unit, PORT_TABm);;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &ptab);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_TABm);;
            return rv;
        }

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if (soc_feature(unit, soc_feature_timesync_v3) ||
            SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
            index = soc_PORT_TABm_field32_get(unit,
                                      &ptab, CTRL_PROFILE_INDEX_1588f);
        }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_KATANAX(unit) || SOC_IS_SABER2(unit)) {
            index = soc_PORT_TABm_field32_get(unit,  &ptab,
                                      ING_1588_TS_PROFILE_PTRf);
        }
#endif

        /* Store all 1588 port indexes for later use */
        if (ts_index[TS_PORT_INDEX] == 0xffffffff) {
            ts_index[TS_PORT_INDEX] = index;
            config_cnt++;
        }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_SABER2(unit)) {
            index = soc_PORT_TABm_field32_get(unit,
                                              &ptab, PTP_LABEL_RANGE_INDEX_1f);
            if (index) {
                ts_index[TS_MPLS_INDEX_1] = index;
                config_cnt++;
            }

            index = soc_PORT_TABm_field32_get(unit,
                                              &ptab, PTP_LABEL_RANGE_INDEX_2f);
            if (index) {
                ts_index[TS_MPLS_INDEX_2] = index;
                config_cnt++;
            }
        }
#endif
        /* Release the port memory lock */
        soc_mem_unlock(unit, PORT_TABm);
    }
    /* update config cnt */
    *array_count = config_cnt;

    /* if config_array is null, return array count */
    if ((NULL == config_array)) {
        return BCM_E_NONE;
    }

    /* lock timesync module */
    TIMESYNC_MODULE_LOCK(unit);

    for (cnt = 0; (cnt < array_size) && (array_size <= config_cnt); cnt++)
    {
        ts_config = (bcm_port_timesync_config_t*) (config_array + cnt );

        /* Get timesync port control profile */
        if ((cnt == TS_PORT_INDEX) && (ts_index[TS_PORT_INDEX] != 0xffffffff)) {
            ts_config->flags |= BCM_PORT_TIMESYNC_DEFAULT;
            index = ts_index[TS_PORT_INDEX];
            if (!is_remote_port) {
                value = 0;
                _BCM_TIMESYNC_IF_ERROR_RETURN(
                        READ_EGR_1588_EGRESS_CTRLr(unit, port,&rval));
                _BCM_TIMESYNC_IF_ERROR_RETURN(
                        READ_EGR_1588_INGRESS_CTRLr(unit, port,&rval_1));

                value |= soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                          TX_TS_SYNCf);
                value |= soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                           TX_TS_DELAY_REQf);
                value |= soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                           TX_TS_PDELAY_REQf);
                value |= soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                           TX_TS_PDELAY_RESPf);
                if (value) {
                   ts_config->flags|=  BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP;
                }

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT)
                if (soc_feature(unit, soc_feature_timesync_v3) ||
                    SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) ||
                    SOC_IS_SABER2(unit) || SOC_IS_APACHE(unit) || SOC_IS_KATANA2(unit)) {
                    if (soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                          DROP_INVALID_1588_PKTf)) {
                        ts_config->pkt_drop |= BCM_PORT_TIMESYNC_PKT_INVALID;
                    }
                }
#endif

#if defined(BCM_KATANA_SUPPORT)
                if (SOC_IS_KATANA(unit)) {
                    value = (soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                          CF_UPDATE_ENABLEf)
                        && soc_reg_field_get(unit, EGR_1588_INGRESS_CTRLr, rval_1,
                                          CF_UPDATE_ENABLEf));
                }
#endif
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
                defined(BCM_KATANA2_SUPPORT) || defined(BCM_SABER2_SUPPORT) ||\
                defined(BCM_HELIX4_SUPPORT)
                if (soc_feature(unit, soc_feature_timesync_v3) ||
                    SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) ||
                    SOC_IS_KATANA2(unit) || SOC_IS_SABER2(unit) ||
                    SOC_IS_HELIX4(unit) || SOC_IS_APACHE(unit)) {

                    value = soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                          CF_UPDATE_MODEf);
                    if (value == 1) {
                        ts_config->flags |= BCM_PORT_TIMESYNC_TIMESTAMP_CFUPDATE_ALL;
                    }
                    value = soc_reg_field_get(unit, EGR_1588_INGRESS_CTRLr, rval_1,
                                          CF_UPDATE_ENABLEf);
                }
#endif
                if (value) {
                   ts_config->flags |=  BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP;

                   if (soc_reg_field_get(unit, EGR_1588_EGRESS_CTRLr, rval,
                                          SA_UPDATE_ENABLEf)) {
                        sal_memset(&sa_entry, 0, sizeof(egr_1588_sa_entry_t));
                        _BCM_TIMESYNC_IF_ERROR_RETURN(
                            soc_mem_read(unit, EGR_1588_SAm, MEM_BLOCK_ANY,
                                          port, &sa_entry));
                        soc_mem_mac_addr_get(unit, EGR_1588_SAm ,&sa_entry,
                                          SAf, ts_config->src_mac_addr);
                    }
                }
            }

            rv = _bcm_esw_port_timesync_control_profile_entry_get(unit, ts_config,
                                                           index);
            if (BCM_FAILURE(rv)) {
                TIMESYNC_MODULE_UNLOCK(unit);
                return rv;
            }
        }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    /* Get timesync MPLS profile */
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_SABER2(unit)) {
        if (cnt == TS_MPLS_INDEX_1 && ts_index[TS_MPLS_INDEX_1]) {
            ts_config->flags|= BCM_PORT_TIMESYNC_MPLS;
            rv = _bcm_esw_port_timesync_mpls_profile_entry_get(unit, ts_config,
                                                           ts_index[TS_MPLS_INDEX_1]);
        }

        if (cnt == TS_MPLS_INDEX_2 && ts_index[TS_MPLS_INDEX_2]) {
            ts_config->flags|= BCM_PORT_TIMESYNC_MPLS;
            rv = _bcm_esw_port_timesync_mpls_profile_entry_get(unit, ts_config,
                                                           ts_index[TS_MPLS_INDEX_2]);
        }
    }
#endif
    }

    /* unlock timesync module */ 
    TIMESYNC_MODULE_UNLOCK(unit);

    return rv;
} 

/*
 * Function:
 *     _bcm_esw_port_timesync_timestamping_mode_set
 * Purpose:
 *      Sets 32bit or 48bit Timestamping mode
 *
 * Parameters:
 *      unit                - (IN) bcm device
 *      timestamping_mode   - (IN) 32bit or 48bit timestamping mode
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 */
int
_bcm_esw_port_timesync_timestamping_mode_set(int unit, bcm_port_t port,
                    bcm_switch_timesync_timestamping_mode_t timestamping_mode)
{
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || \
defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT) || \
defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    uint32  rval;
#endif
#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int val, index, rv = BCM_E_NONE;
    uint32 mode;
#ifdef PORTMOD_SUPPORT
    portmod_egr_1588_timestamp_config_t config;
#endif /* PORTMOD_SUPPORT */
#endif /* BCM_HURRICANE2_SUPPORT || BCM_GREYHOUND_SUPPORT ||
          BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || \
defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT) || \
defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
defined(BCM_TOMAHAWK_SUPPORT)
    /* Enabling 48bit timestamping mode (in egress pipeline)*/
    if (soc_feature(unit, soc_feature_timesync_v3) ||
        SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit) ||
            SOC_IS_SABER2(unit) || SOC_IS_TD2P_TT2P(unit) ||
                SOC_IS_HELIX4(unit) || SOC_IS_APACHE(unit)) {
        if (SOC_IS_KATANA2(unit)) {
            if (soc_reg_field_valid(unit, TOP_MISC_CONTROL_0r, TIMESTAMP_VAL_MODE_SELf)) {
                SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval));
                soc_reg_field_set(unit, TOP_MISC_CONTROL_0r, &rval,
                        TIMESTAMP_VAL_MODE_SELf,
                        (timestamping_mode ==
                         bcmTimesyncTimestampingMode48bit) ? 0: 1);
                SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_0r(unit, rval));

            }
        }

        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_HELIX4(unit)) {
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EGR_1588_PARSING_CONTROLr,
                                        port, MODE_48BITS_TIMESTAMPf,
                                        (timestamping_mode ==
                                        bcmTimesyncTimestampingMode48bit) ?  1: 0));
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, PORT_MODE_REGr,
                                        port,
                                        EGR_1588_TIMESTAMPING_MODEf,
                                        (timestamping_mode ==
                                        bcmTimesyncTimestampingMode48bit) ?  1: 0));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, EGR_1588_PARSING_CONTROLr,
                                        port, TIMESTAMPING_MODEf,
                                        (timestamping_mode ==
                                        bcmTimesyncTimestampingMode48bit) ?  1: 0));
        }
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    /* in SB2, port number 1 -24 are viper ports (XPORT..) and 25-28 are eagle
     * ports (XLPORT..), enable 48bit timestamping mode in xlpor
     t block,
     * viper ports do not have configuration in port block.
     */
    if (SOC_IS_SABER2(unit) && IS_XL_PORT(unit, port)) {
        soc_reg_field32_modify(
            unit, XLPORT_MODE_REGr, port, EGR_1588_TIMESTAMPING_MODEf,
            (timestamping_mode == bcmTimesyncTimestampingMode48bit) ? 1: 0);
        soc_reg_field32_modify(
            unit, XLPORT_MODE_REGr, port, EGR_1588_TIMESTAMPING_CMIC_48_ENf,
            (timestamping_mode == bcmTimesyncTimestampingMode48bit) ? 1: 0);
    }
#endif

#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    /* Enabling 48bit timestamping mode (in port block) */
    if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_TD2P_TT2P(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_APACHE(unit) || SOC_IS_GREYHOUND2(unit) ||
        SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit) ||
        SOC_IS_HURRICANE4(unit) || SOC_IS_HELIX5(unit) ||
        SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit)) {
        for(index = 0;
            index < SOC_DRIVER(unit)->port_num_blktype;
            index++) {
            /* Get the block where the port belongs to */
            val = SOC_PORT_IDX_BLOCK(unit,
                                     SOC_INFO(unit).port_l2p_mapping[port],
                                     index);
            if (val < 0) {
                break;
            }
            /* Check if it's valid in the bitmap */
            if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, val), port)){
                continue;
            }

#if defined(BCM_TSCF_GEN3_PCS_TIMESTAMP)
            if (soc_feature(unit, soc_feature_tscf_gen3_pcs_timestamp)) {
                /* Get the block type */
                val = SOC_BLOCK_TYPE(unit, val);
                if (val == SOC_BLK_CLPORT) {
                    int mac_ts_enable = FALSE, read_from_reg = TRUE;
#ifdef PORTMOD_SUPPORT
                    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                        rv = portmod_port_mac_timestamp_enable_get
                                (unit, port, &mac_ts_enable);
                        if (BCM_SUCCESS(rv)) {
                            read_from_reg = FALSE;
                        }
                    }
#endif /* PORTMOD_SUPPORT */
                    if (read_from_reg &&
                        SOC_REG_IS_VALID(unit, CLPORT_MAC_CONTROLr) &&
                        SOC_REG_FIELD_VALID(unit,
                                            CLPORT_MAC_CONTROLr,
                                            CLMAC_TS_DISABLEf)) {
                        uint32 value;
                        SOC_IF_ERROR_RETURN(
                            READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
                        value = soc_reg_field_get(unit, CLPORT_MAC_CONTROLr,
                                                  rval, CLMAC_TS_DISABLEf);
                        mac_ts_enable = (value == 1) ? FALSE : TRUE;
                    }

                    if (mac_ts_enable == FALSE &&
                        (timestamping_mode ==
                         bcmTimesyncTimestampingMode32bit)) {
                         /*
                          * PCS 1588 doesn't support 32-bit timestamp on
                          * PM4x25 gen3 port macro.
                          */
                        return BCM_E_UNAVAIL;
                    }
                }
            }
#endif /* BCM_TSCF_GEN3_PCS_TIMESTAMP */

#ifdef PORTMOD_SUPPORT
            if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                if (timestamping_mode == bcmTimesyncTimestampingMode32bit) {
                    config.timestamp_mode = portmodTimestampMode32bit;
                    config.cmic_48_overr_en = 0;
                } else if (timestamping_mode == bcmTimesyncTimestampingMode48bit){
                    config.timestamp_mode = portmodTimestampMode48bit;
                    config.cmic_48_overr_en = 1;
                } else {
                    continue;
                }

                rv = portmod_egr_1588_timestamp_config_set(unit, port, config);
                if (BCM_SUCCESS(rv)) {
                    continue;
                }
            }
#endif /* PORTMOD_SUPPORT */

            /* Portmod API doesn't support. */
            if (!SOC_PORT_USE_PORTCTRL(unit, port) ||
                !BCM_SUCCESS(rv)) {
                /* Get the block type */
                val = SOC_BLOCK_TYPE(unit, val);
                if (val == SOC_BLK_XLPORT) {
                    if (SOC_IS_HURRICANE2(unit) || SOC_IS_TD2P_TT2P(unit)) {
                        mode = (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 0 : 1;
                    } else if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                               SOC_IS_GREYHOUND2(unit) || SOC_IS_HURRICANE4(unit) ||
                               SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWKX(unit) ||
                               SOC_IS_SABER2(unit) || SOC_IS_APACHE(unit) ||
                               SOC_IS_HELIX5(unit) || SOC_IS_FIRELIGHT(unit)) {
                        mode = (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 1 : 0;
                    } else {
                        continue;
                    }

                    /* Write mode */
                    if (SOC_REG_IS_VALID(unit, XLPORT_MODE_REGr) &&
                        SOC_REG_FIELD_VALID(
                            unit, XLPORT_MODE_REGr, EGR_1588_TIMESTAMPING_MODEf)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_field32_modify(
                                unit, XLPORT_MODE_REGr,
                                port, EGR_1588_TIMESTAMPING_MODEf,
                                mode));
                    }

                    /* Enable for 48-Bit mode */
                    if (SOC_REG_IS_VALID(unit, XLPORT_MODE_REGr) &&
                        SOC_REG_FIELD_VALID(
                            unit, XLPORT_MODE_REGr, EGR_1588_TIMESTAMPING_CMIC_48_ENf)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_field32_modify(
                                unit, XLPORT_MODE_REGr,
                                port, EGR_1588_TIMESTAMPING_CMIC_48_ENf,
                                (timestamping_mode ==
                                    bcmTimesyncTimestampingMode48bit) ? 1: 0));
                    }
                } else if (val == SOC_BLK_GPORT ||
                           val == SOC_BLK_GXPORT ||
                           val == SOC_BLK_GXFPPORT) {
                    if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) ||
                        SOC_IS_GREYHOUND2(unit) || SOC_IS_HURRICANE4(unit) ||
                        SOC_IS_HELIX5(unit) || SOC_IS_FIRELIGHT(unit)) {
                        mode = (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 0 : 1;
                    } else if (SOC_IS_GREYHOUND(unit)) {
                        mode = (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 1 : 0;
                    } else {
                        continue;
                    }

                    /* Write mode */
                    if (SOC_REG_IS_VALID(unit, GPORT_MODE_REGr) &&
                        SOC_REG_FIELD_VALID(
                            unit, GPORT_MODE_REGr, EGR_1588_TIMESTAMPING_MODEf)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_field32_modify(
                                unit, GPORT_MODE_REGr,
                                port, EGR_1588_TIMESTAMPING_MODEf,
                                mode));
                    }
                } else if (val == SOC_BLK_CPORT ||
                           val == SOC_BLK_CLG2PORT ||
                           val == SOC_BLK_CLPORT) {
                    soc_reg_t config_reg = INVALIDr;
                    if (SOC_IS_TD2P_TT2P(unit)) {
                        mode = (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 0 : 1;
                        config_reg = CPORT_MODE_REGr;
                    } else if (SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWKX(unit) ||
                               SOC_IS_GREYHOUND2(unit) || SOC_IS_TRIDENT3(unit) ||
                               SOC_IS_HURRICANE4(unit) || SOC_IS_HELIX5(unit) ||
                               SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit)) {
                        mode = (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 1 : 0;
                        config_reg = CLPORT_MODE_REGr;
                    } else {
                        continue;
                    }

                    /* Write mode */
                    if (SOC_REG_IS_VALID(unit, config_reg) &&
                        SOC_REG_FIELD_VALID(
                            unit, config_reg, EGR_1588_TIMESTAMPING_MODEf)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_field32_modify(
                                unit, config_reg,
                                port, EGR_1588_TIMESTAMPING_MODEf,
                                mode));
                    }

                    /* Enable for 48-Bit mode */
                    if (SOC_REG_IS_VALID(unit, config_reg) &&
                        SOC_REG_FIELD_VALID(
                            unit, config_reg, EGR_1588_TIMESTAMPING_CMIC_48_ENf)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_field32_modify(
                                unit, config_reg,
                                port, EGR_1588_TIMESTAMPING_CMIC_48_ENf,
                                (timestamping_mode ==
                                    bcmTimesyncTimestampingMode48bit) ? 1: 0));
                    }
                } else if (val == SOC_BLK_XLPORTB0) {
                    if (SOC_IS_APACHE(unit)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_field32_modify(
                                unit, XLPORT_MODE_REGr,
                                port, EGR_1588_TIMESTAMPING_MODEf,
                                (timestamping_mode ==
                                bcmTimesyncTimestampingMode48bit) ? 1: 0));
                    }
                }
            }
        }
    }
#endif /* BCM_HURRICANE2_SUPPORT || BCM_GREYHOUND_SUPPORT */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_esw_port_timesync_timestamping_mode_get
 * Purpose:
 *      Sets 32bit or 48bit Timestamping mode
 *
 * Parameters:
 *      unit                - (IN) bcm device
 *      timestamping_mode   - (IN/OUT) 32bit or 48bit timestamping mode
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 */
int
_bcm_esw_port_timesync_timestamping_mode_get(int unit, bcm_port_t port,
                    bcm_switch_timesync_timestamping_mode_t *timestamping_mode)
{
#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || \
defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_SABER2_SUPPORT) || \
defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    uint32  rval, rval_1;
#endif
 #if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
 defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int val, index, rv = BCM_E_NONE;
#ifdef PORTMOD_SUPPORT
    portmod_egr_1588_timestamp_config_t config;
#endif /* PORTMOD_SUPPORT */
#endif

#if defined(BCM_TIMESYNC_V3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || \
defined(BCM_TRIUMPH3_SUPPORT)  || defined(BCM_SABER2_SUPPORT) || \
defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    /* Enabling 48bit timestamping mode (in egress pipeline)*/
    if (soc_feature(unit, soc_feature_timesync_v3) ||
        SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_SABER2(unit) || SOC_IS_TD2P_TT2P(unit) ||
        SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit)) {
        if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
            if (soc_reg_field_valid(unit, TOP_MISC_CONTROL_0r, TIMESTAMP_VAL_MODE_SELf)) {
            SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_0r(unit, &rval_1));
            SOC_IF_ERROR_RETURN(READ_EGR_1588_PARSING_CONTROLr(unit, &rval));

            *timestamping_mode = (bcm_switch_timesync_timestamping_mode_t)(
                (!soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval_1,
                                    TIMESTAMP_VAL_MODE_SELf)) 
                && soc_reg_field_get(unit, EGR_1588_PARSING_CONTROLr, rval,
                                  TIMESTAMPING_MODEf));
            } 
        } else if (SOC_IS_TRIUMPH3(unit)) {
            SOC_IF_ERROR_RETURN(
                    READ_EGR_1588_PARSING_CONTROLr(unit, &rval));
            SOC_IF_ERROR_RETURN(
                    READ_PORT_MODE_REGr(unit, port,&rval_1));

            *timestamping_mode = (bcm_switch_timesync_timestamping_mode_t)(
                soc_reg_field_get(unit,
                    EGR_1588_PARSING_CONTROLr, rval, MODE_48BITS_TIMESTAMPf)
                && soc_reg_field_get(unit, PORT_MODE_REGr, rval_1,
                    EGR_1588_TIMESTAMPING_MODEf));
        } else {
            SOC_IF_ERROR_RETURN(READ_EGR_1588_PARSING_CONTROLr(unit, &rval));

            *timestamping_mode = (bcm_switch_timesync_timestamping_mode_t)(
                soc_reg_field_get(unit, EGR_1588_PARSING_CONTROLr, rval,
                                  TIMESTAMPING_MODEf));

        }
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    /* in SB2, port number 1 -24 are viper ports (XPORT..)and 25-28 are eagle
     * ports (XLPORT..)
     * enable 48bit timestamping mode in xlport block, viper ports do not have
     * configuration in port block.
     */
    if (SOC_IS_SABER2(unit) && IS_XL_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(
                READ_XLPORT_MODE_REGr(unit, port,&rval));
        *timestamping_mode = (bcm_switch_timesync_timestamping_mode_t)(
            soc_reg_field_get(unit, XLPORT_MODE_REGr, rval,
                                           EGR_1588_TIMESTAMPING_MODEf));
    }
#endif


#if defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    /* Enabling 48bit timestamping mode (in port block) */
    if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_HURRICANE3(unit) || SOC_IS_APACHE(unit) ||
        SOC_IS_GREYHOUND2(unit) || SOC_IS_HURRICANE4(unit) ||
        SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit)) {
        for(index=0;
            index<SOC_DRIVER(unit)->port_num_blktype;
            index++) {
            /* Get the block where the port belongs to */
            val = SOC_PORT_IDX_BLOCK(unit,
                        SOC_INFO(unit).port_l2p_mapping[port],
                        index);
            if (val < 0) {
                break;
            }
            /* Check if it's valid in the bitmap */
            if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, val), port)){
                continue;
            }

#ifdef PORTMOD_SUPPORT
            if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                rv = portmod_egr_1588_timestamp_config_get(unit, port, &config);
                if (BCM_SUCCESS(rv)) {
                    if (config.timestamp_mode == portmodTimestampMode32bit) {
                        *timestamping_mode = bcmTimesyncTimestampingMode32bit;
                    } else if (config.timestamp_mode == portmodTimestampMode48bit){
                        config.timestamp_mode = portmodTimestampMode48bit;
                        *timestamping_mode = bcmTimesyncTimestampingMode48bit;
                    }
                    continue;
                }
            }
#endif /* PORTMOD_SUPPORT */
            /* Portmod API doesn't support. */
            if (!SOC_PORT_USE_PORTCTRL(unit, port) ||
                !BCM_SUCCESS(rv)) {
                /* Get the block type */
                val = SOC_BLOCK_TYPE(unit, val);
                if (val == SOC_BLK_XLPORT) {
                    if (SOC_IS_HURRICANE2(unit) || SOC_IS_TD2P_TT2P(unit)) {
                        SOC_IF_ERROR_RETURN(
                            READ_XLPORT_MODE_REGr(unit, port,&rval));
                        *timestamping_mode =
                            (bcm_switch_timesync_timestamping_mode_t)(
                            !soc_reg_field_get(unit, XLPORT_MODE_REGr, rval,
                                               EGR_1588_TIMESTAMPING_MODEf));
                    } else if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                               SOC_IS_GREYHOUND2(unit) || SOC_IS_HURRICANE4(unit) ||
                               SOC_IS_TRIDENT3(unit) || SOC_IS_TOMAHAWKX(unit) ||
                               SOC_IS_SABER2(unit) || SOC_IS_APACHE(unit)) {
                        SOC_IF_ERROR_RETURN(
                            READ_XLPORT_MODE_REGr(unit, port,&rval));
                        *timestamping_mode =
                            (bcm_switch_timesync_timestamping_mode_t)(
                            soc_reg_field_get(unit, XLPORT_MODE_REGr, rval,
                                              EGR_1588_TIMESTAMPING_MODEf));
                    }
                } else if (val == SOC_BLK_GPORT ||
                           val == SOC_BLK_GXPORT ||
                           val == SOC_BLK_GXFPPORT) {
                    if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) ||
                        SOC_IS_GREYHOUND2(unit) || SOC_IS_HURRICANE4(unit)) {
                        SOC_IF_ERROR_RETURN(
                            READ_GPORT_MODE_REGr(unit, port,&rval));
                        *timestamping_mode =
                            (bcm_switch_timesync_timestamping_mode_t)(
                            !soc_reg_field_get(unit, GPORT_MODE_REGr, rval,
                                               EGR_1588_TIMESTAMPING_MODEf));
                    } else if (SOC_IS_GREYHOUND(unit)) {
                        SOC_IF_ERROR_RETURN(
                            READ_GPORT_MODE_REGr(unit, port,&rval));
                        *timestamping_mode =
                            (bcm_switch_timesync_timestamping_mode_t)(
                            soc_reg_field_get(unit, GPORT_MODE_REGr, rval,
                                              EGR_1588_TIMESTAMPING_MODEf));
                    }
                } else if (val == SOC_BLK_CPORT ||
                           val == SOC_BLK_CLG2PORT ||
                           val == SOC_BLK_CLPORT) {
                    if (SOC_IS_TD2P_TT2P(unit)) {
                        SOC_IF_ERROR_RETURN(
                            READ_CLPORT_MODE_REGr(unit, port,&rval));
                        *timestamping_mode =
                            (bcm_switch_timesync_timestamping_mode_t)(
                            !soc_reg_field_get(unit, CPORT_MODE_REGr, rval,
                                               EGR_1588_TIMESTAMPING_MODEf));
                    } else if (SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWKX(unit) ||
                               SOC_IS_GREYHOUND2(unit) || SOC_IS_TRIDENT3(unit) ||
                               SOC_IS_HURRICANE4(unit)) {
                        SOC_IF_ERROR_RETURN(
                            READ_CLPORT_MODE_REGr(unit, port,&rval));
                        *timestamping_mode =
                            (bcm_switch_timesync_timestamping_mode_t)(
                            soc_reg_field_get(unit, CLPORT_MODE_REGr, rval,
                                              EGR_1588_TIMESTAMPING_MODEf));
                    }
                }
            }
        }
    }
#endif /* BCM_HURRICANE2_SUPPORT || BCM_GREYHOUND_SUPPORT || BCM_APACHE_SUPPORT */

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_esw_port_timesync_tx_timestamp_and_seqid_get
 * Purpose:
 *     Retrives two-step PTP timestamp and seqid from MAC FIFO after packet TX 
 *
 * Parameters:
 *      unit                - (IN) bcm device
 *
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_XXX
 */
int
_bcm_esw_port_timesync_tx_info_get(int unit, bcm_port_t port,
                    bcm_port_timesync_tx_info_t *tx_info)
{
    int rv = BCM_E_NONE;
#ifdef PORTMOD_SUPPORT
    portmod_fifo_status_t info;

    if (soc_feature(unit, soc_feature_timesync_support)) {
        PORT_LOCK(unit);
        rv = portmod_port_timesync_tx_info_get(unit, port, &info);
        PORT_UNLOCK(unit);
        COMPILER_64_SET(tx_info->timestamp, info.timestamps_in_fifo_hi, info.timestamps_in_fifo);
        tx_info->sequence_id = info.sequence_id;
        tx_info->sub_ns = info.timestamp_sub_nanosec;
    }
#endif    
    return rv;
}



#endif /* BCM_TIMESYNC_V3_SUPPORT || BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT */
