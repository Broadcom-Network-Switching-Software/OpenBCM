/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        telemetry.h
 * Purpose:     
 */
#ifndef _BCM_INT_TELEMETRY_H_
#define _BCM_INT_TELEMETRY_H_

#if defined(INCLUDE_TELEMETRY)

#include <bcm/telemetry.h>
#include <bcm/collector.h>
#include <shared/idxres_fl.h>
#include <soc/shared/st.h>

/* Maximum number of telemetry instances. */
#define BCM_INT_TELEMETRY_MAX_INSTANCES 1

/* Telemetry module invalid index definition */
#define BCM_TELEMETRY_INVALID_INDEX -1

/* Telemetry module MAX system ID  length */
#define BCM_TELEMETRY_MAX_SYSTEM_ID_LENGTH 32

/*
 * Macro:
 *     _BCM_TELEMETRY_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_TELEMETRY_ALLOC(_ptr_,_ptype_,_size_,_descr_)               \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_TELEMETRY, \
                              (BSL_META("Telemetry Error: Allocation"    \
                              " failure %s\n"),                          \
                               (_descr_)));                              \
                }                                                        \
            } while (0)

/* Internal data structure to store
 * telemetry global information.
 */
typedef struct _bcm_telemetry_global_info_s {

    /* flags, See BCM_INT_TELEMETRY_INFO_FLAGS_XXX */
    uint32 flags;
    
    /* Number of telemrty instances configured */
    uint32 num_telemetry_instances;

    /* System Identification to be used in export data to collector. */
    uint8 system_id[BCM_TELEMETRY_MAX_SYSTEM_ID_LENGTH];
    
    /* system ID length */
    uint8 system_id_length;

    /* Maximum size of the packet sent by telemetry application */
    uint32 max_pkt_size;

    /* Internal BTE Export profile ID pool */
    shr_idxres_list_handle_t int_profile_pool;

    /* Internal BTE Collector ID pool */
    shr_idxres_list_handle_t int_collector_pool;

    /* Number of valid ports at any point of time */
    uint16 num_ports;

    /* List of ports used for stats monitoring */
    uint16 lports[SHR_ST_MAX_PORTS];

    /* Maximum number of ports to be monitored */
    uint16 max_num_ports;

    /* DMA Buffer used for messaging between SDK and
     * UKERNEL.
     */
    uint8 *dma_buffer;
    /* Length of the DMA buffer */
    int dma_buffer_len;

    /* Flag to indicate whether the app is loaded */
    uint8 uc_loaded;

    /* uC number on which the app is loaded */
    uint8 uC;
} _bcm_telemetry_global_info_t;

/* Internal data structure to store per telemetry information
 */
typedef struct _bcm_telemetry_instance_info_s {
    /* Telemetry instance ID configured by applocation */
    int instance_id;
    
    /* Fw ARM core on which telemetry instance is being run */
    int core;

    /* Telemetry config object count */
    int config_count;

    /* Telemetry config structure */
    bcm_telemetry_config_t *telemetry_config;

    /* Collector associated with this instance */
    bcm_collector_t collector_id;

    /* Export profile  associated with this instance */
    int export_profile_id;

    /* Number of collectors configured on this instance */
    uint32 num_collectors;

} _bcm_telemetry_instance_info_t;

/* Global data structures extern declarations */
extern _bcm_telemetry_global_info_t *telemetry_global_info[BCM_MAX_NUM_UNITS];

/* Utility functions */
extern void _bcm_esw_telemetry_sw_dump(int unit);
extern void _bcm_xgs5_telemetry_sw_dump(int unit);

/* Gets the TELEMETRY_GLOBAL_INFO struct for the unit */
#define TELEMETRY_GLOBAL_INFO_GET(_u) telemetry_global_info[(_u)]

extern int _bcm_xgs5_telemetry_instance_config_update(int unit,
                           int telemetry_instance_id,
                           int core,
                           int config_count,
                           bcm_telemetry_config_t *telemetry_config);
extern int _bcm_xgs5_telemetry_instance_config_get(int unit,
                           int telemetry_instance_id,
                           int core,
                           int max_count,
                           bcm_telemetry_config_t *telemetry_config,
                           int *config_count);
extern int _bcm_xgs5_telemetry_instance_export_config_update(int unit,
                           int telemetry_instance_id,
                           bcm_collector_t collector_id,
                           int export_profile_id);

extern int _bcm_xgs5_telemetry_instance_export_config_get(int unit,
                           int telemetry_instance_id,
                           bcm_collector_t *collector_id,
                           int *export_profile_id);
extern int _bcm_xgs5_telemetry_system_id_get(int unit, int max_system_id_len,
                           int *system_id_len, uint8 *system_id);
extern int _bcm_xgs5_telemetry_system_id_set(int unit, int system_id_len,
                           uint8 *system_id);

extern int bcm_xgs5_telemetry_instance_export_stats_get(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats);

extern int bcm_xgs5_telemetry_instance_export_stats_set(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats);

extern int _bcm_xgs5_telemetry_init(int unit);
extern int _bcm_xgs5_telemetry_detach(int unit);
extern int _bcm_xgs5_telemetry_sync(int unit);

extern int _bcm_esw_telemetry_sync(int unit);
#endif /* INCLUDE_TELEMETRY */
#endif  /* !_BCM_INT_TELEMETRY_H_ */
