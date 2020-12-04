/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_EAV_H__
#define __BCM_EAV_H__

#include <bcm/types.h>
#include <bcm/port.h>

/* bcm_eav_control_e */
typedef enum bcm_eav_control_e {
    bcmEAVControlTimeBase = 1, 
    bcmEAVControlTimeAdjust = 2, 
    bcmEAVControlStreamClassAPCP = 7, 
    bcmEAVControlStreamClassBPCP = 8 
} bcm_eav_control_t;

#define BCM_EAV_TIMEREF_MODE_FREERUN        0x0        
#define BCM_EAV_TIMEREF_MODE_PHASE_ADJUSTMENT 0x1        
#define BCM_EAV_TIMEREF_MODE_FREQ_ADJUMENT1 0x2        
#define BCM_EAV_TIMEREF_MODE_FREQ_ADJUMENT2 0x3        

/* bcm_eav_stream_class_e */
typedef enum bcm_eav_stream_class_e {
    bcmEAVCStreamClassA = 0, 
    bcmEAVCStreamClassB = 1 
} bcm_eav_stream_class_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the Ethernet AV module and enable the EAV support globally. */
extern int bcm_eav_init(
    int unit);

/* Enable or disable EAV processing on a port */
extern int bcm_eav_port_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

/* Enable or disable EAV processing on a port */
extern int bcm_eav_port_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* Get the EAV link status of the specified port. */
extern int bcm_eav_link_status_get(
    int unit, 
    bcm_port_t port, 
    int *link);

/* Set the EAV link status of the specified port */
extern int bcm_eav_link_status_set(
    int unit, 
    bcm_port_t port, 
    int link);

/* Configure EAV operating modes. */
extern int bcm_eav_control_get(
    int unit, 
    bcm_eav_control_t type, 
    uint32 *arg, 
    uint32 *arg2);

/* Configure EAV operating modes. */
extern int bcm_eav_control_set(
    int unit, 
    bcm_eav_control_t type, 
    uint32 arg, 
    uint32 arg2);

/* 
 * Get the timestamp value of the last egressed packet on the specified
 * port.
 */
extern int bcm_eav_timestamp_get(
    int unit, 
    bcm_port_t port, 
    uint32 *timestamp);

/* 
 * Manage the MAC address for the Time Synchronization protocol.
 * Get the destination MAC address for the Time Synchronization protocol.
 */
extern int bcm_eav_timesync_mac_set(
    int unit, 
    bcm_mac_t eav_mac);

/* 
 * Manage the MAC address for the Time Synchronization protocol.
 * Get the destination MAC address for the Time Synchronization protocol.
 */
extern int bcm_eav_timesync_mac_get(
    int unit, 
    bcm_mac_t eav_mac);

/* 
 * Manage the MAC address and EtherType used to trap SRP protocol
 * packets.
 */
extern int bcm_eav_srp_mac_ethertype_set(
    int unit, 
    bcm_mac_t mac, 
    bcm_port_ethertype_t ethertype);

/* 
 * Manage the MAC address and EtherType used to trap SRP protocol
 * packets.
 */
extern int bcm_eav_srp_mac_ethertype_get(
    int unit, 
    bcm_mac_t mac, 
    bcm_port_ethertype_t *ethertype);

/* Manage the PCP(priority) value mapping for each EAV class packets. */
extern int bcm_eav_pcp_mapping_set(
    int unit, 
    bcm_eav_stream_class_t type, 
    int pcp, 
    int remapped_pcp);

/* Manage the PCP(priority) value mapping for each EAV class packets. */
extern int bcm_eav_pcp_mapping_get(
    int unit, 
    bcm_eav_stream_class_t type, 
    int *pcp, 
    int *remapped_pcp);

/* Manage the reserved bandwidth for Class A or B stream traffic */
extern int bcm_eav_bandwidth_set(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 bytes_sec, 
    uint32 bytes_burst);

/* Manage the reserved bandwidth for Class A or B stream traffic */
extern int bcm_eav_bandwidth_get(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 *bytes_sec, 
    uint32 *bytes_burst);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_EAV_H__ */
