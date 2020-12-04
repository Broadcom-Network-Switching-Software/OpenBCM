/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TELEMETRY_H__
#define __BCM_TELEMETRY_H__

#include <bcm/types.h>
#include <bcm/collector.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initializes telemetry module. */
extern int bcm_telemetry_init(
    int unit);

/* Detach telemetry module. */
extern int bcm_telemetry_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Type of telemetry object stats to be collected */
typedef enum bcm_telemetry_object_type_e {
    bcmTelemetryObjectTypeNone = 0,     /* No stats. */
    bcmTelemetryObjectTypeIntfIngress = 1, /* Interface Ingress stats. */
    bcmTelemetryObjectTypeIntfEgress = 2, /* Interface Egress stats. */
    bcmTelemetryObjectTypeIntfEgressQueue = 3, /* Interface Egress Queue stats. */
    bcmTelemetryObjectTypeIntfIngressError = 4, /* Interface Ingress stats. */
    bcmTelemetryObjectTypeIntMetaData = 5, /* Interface Egress Queue stats. */
    bcmTelemetryObjectTypeCount = 6     /* Last Value. Not Usable. */
} bcm_telemetry_object_type_t;

/* Telemetry module defines. */
#define BCM_TELEMETRY_PORT_NAME_SIZE    10         /* Port/Interface name size
                                                      of ProtoBuf Streaming
                                                      telemetry. */

/* Telemetry application instance configuration information. */
typedef struct bcm_telemetry_config_s {
    bcm_telemetry_object_type_t object_type[bcmTelemetryObjectTypeCount]; /* Type of stats need to be collected. */
    uint32 component_id;                /* Component Identification to be used
                                           in export data to collector. */
    bcm_gport_t gport;                  /* port to be used to collect
                                           statistics. */
    uint8 port_name[BCM_TELEMETRY_PORT_NAME_SIZE]; /* Port/Interface name associated with 
                                           port provided. */
} bcm_telemetry_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Creates a telemetry instance. */
extern int bcm_telemetry_config_set(
    int unit, 
    int telemetry_instance, 
    int core, 
    int config_count, 
    bcm_telemetry_config_t *telemetry_config_list);

/* Fetches a telemetry instance whose instance ID is provided. */
extern int bcm_telemetry_config_get(
    int unit, 
    int telemetry_instance, 
    int core, 
    int max_count, 
    bcm_telemetry_config_t *telemetry_config_list, 
    int *config_count);

/* Configures a telemetry instance export information. */
extern int bcm_telemetry_export_config_set(
    int unit, 
    int telemetry_instance, 
    bcm_collector_t collector_id, 
    int export_profile_id);

/* Fetches a telemetry instance export information. */
extern int bcm_telemetry_export_config_get(
    int unit, 
    int telemetry_instance, 
    bcm_collector_t *collector_id, 
    int *export_profile_id);

/* Deletes a telemetry instance export information. */
extern int bcm_telemetry_export_config_delete(
    int unit, 
    int telemetry_instance, 
    bcm_collector_t collector_id, 
    int export_profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initializes a telemetry config information structure. */
extern void bcm_telemetry_config_t_init(
    bcm_telemetry_config_t *telemetry_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Configures  telemetry system ID information. */
extern int bcm_telemetry_system_id_set(
    int unit, 
    int system_id_len, 
    uint8 *system_id);

/* Fetches telemetry system ID information. */
extern int bcm_telemetry_system_id_get(
    int unit, 
    int max_system_id_len, 
    int *system_id_len, 
    uint8 *system_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Telemetry instance export statistics. */
typedef struct bcm_telemetry_instance_export_stats_s {
    uint64 tx_pkts;         /* Number of pkts exported. */
    uint64 tx_pkt_fails;    /* Number of pkts that failed to export. */
} bcm_telemetry_instance_export_stats_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the telemetry instance export statistics. */
extern int bcm_telemetry_instance_export_stats_get(
    int unit, 
    int telemetry_instance, 
    bcm_collector_t collector_id, 
    bcm_telemetry_instance_export_stats_t *stats);

/* Set the telemetry instance export statistics. */
extern int bcm_telemetry_instance_export_stats_set(
    int unit, 
    int telemetry_instance, 
    bcm_collector_t collector_id, 
    bcm_telemetry_instance_export_stats_t *stats);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TELEMETRY_H__ */
