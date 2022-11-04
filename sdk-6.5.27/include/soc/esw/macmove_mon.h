/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        macmove_mon.h
 * Purpose:     Definitions and declarations for Mac move dampening feature
 */
#ifndef   _SOC_ESW_MACMOVE_MON_H_
#define   _SOC_ESW_MACMOVE_MON_H_
/*
 * Default values for threshold and duration. Must be same as in the BCM layer
 */
#define SOC_L2_MAC_MOVE_MON_STNMOVE_THRESHOLD_DEFAULT 5
#define SOC_L2_MAC_MOVE_MON_STNMOVE_DURATION_DEFAULT  1

/* Per-mac information maintained for station move monitoring.
 * Filled by the learn thread for every mac move
 */
typedef struct soc_l2_mac_move_mon_entry_info_s {
    sal_mac_addr_t mac; /* MAC address being monitored for station move */

    vlan_id_t vlan; /* VLAN id to which the mac is associated */

    /* If dst_type_* is TRUE, portnum_or_trunknum* will hold the trunk number.
     * If dst_type_* is FALSE, portnum_or_trunknum* will hold logical port
     * number
     */
    int dst_type_cur; /* Destination type after station move */
    /* Logical port number, or trunk id associated with the MAC address */
    int portnum_or_trunk_id_cur; /* Port number, or trunk id after stn move */
    int modid_cur;     /* Current module ID */

    int dst_type_prev; /* Destination type before station move */
    /* Logical port number, or trunk id associated with the MAC address */
    int portnum_or_trunk_id_prev; /* Port number, or trunk id before stn move */
    int modid_prev;     /* Previous module ID */

    uint32 flags;       /* For marking state of an entry as valid,
                           or invalid, and passing additional info from producer
                           thread. Not used by bcm layer */
#define _SOC_L2_MAC_MOVE_MON_ENTRY_VALID 0x1
} soc_l2_mac_move_mon_entry_info_t;

typedef void (*soc_l2_mac_move_mon_cb)(
    int unit,
    soc_l2_mac_move_mon_entry_info_t *info,
    void *user_data);

/* Queue size. One extra entry allocated, because is used for determining
 * queue full condition
 */
#define MAC_MOVE_MON_QSIZE 1025

/* Queue used for passing mac moved entries from the learn thread to the
 * mac move monitoring thread
 */
typedef struct soc_l2_mac_move_mon_queue_s {
    /* Mac move list passed from the learn thread */
    soc_l2_mac_move_mon_entry_info_t *list;
    volatile int in;             /* 'in' ptr incr by the learn thread */
    volatile int out;            /* 'out' ptr incr by the mon thread */
} soc_l2_mac_move_mon_queue_t;

/* Flags to indicate feature state (initialized, started/stopped */
#define SOC_L2_MAC_MOVE_MON_STATE_INITED   0x1
#define SOC_L2_MAC_MOVE_MON_STATE_STARTED  0x2

/* Internal, shared queue to pass information from the producer thread to the
 * mac move monitoring thread. On TH3, producer is the learn thread
 */
extern soc_l2_mac_move_mon_queue_t *mac_move_mon_queue[SOC_MAX_NUM_DEVICES];

extern int
soc_l2_mac_move_mon_signalling_sem_create(int unit);

extern int
soc_l2_mac_move_mon_signalling_sem_destroy(int unit);

extern int
soc_l2_mac_move_mon_signalling_sem_take(int unit);

extern int
soc_l2_mac_move_mon_signalling_sem_give(int unit);

extern int
soc_l2_mac_move_mon_resource_init(int unit);

extern int
soc_l2_mac_move_mon_resource_alloc(int unit);

extern int
soc_l2_mac_move_mon_delete_all(int unit);

extern int
soc_l2_mac_move_mon_queue_add(int unit,
                            soc_l2_mac_move_mon_entry_info_t *l2_mmm_entry);

extern int
soc_l2_move_mac_mon_entry_from_l2x(int unit,
                                   l2x_entry_t *old_l2x_entry,
                                   l2x_entry_t *l2x_entry,
                                soc_l2_mac_move_mon_entry_info_t *l2_mmm_entry);

extern int
soc_l2_mac_move_mon_resource_destroy(int unit, int cleanup_all);

extern int
soc_l2_mac_move_mon_thread_stop(int unit);

extern int
soc_l2_mac_move_mon_thread_start(int unit);

extern int
soc_l2_mac_move_mon_thread_running(int unit);

extern int
soc_l2_mac_move_mon_thread_stop(int unit);

extern int
soc_l2_mac_move_mon_cb_register(int unit, soc_l2_mac_move_mon_cb fn,
                                void *fn_data);
extern int
soc_l2_mac_move_mon_cb_unregister(int unit, soc_l2_mac_move_mon_cb fn,
                                  void *fn_data);

extern int
soc_l2_mac_move_mon_thread_intvl_set(int unit, sal_usecs_t interval);

extern int
soc_l2_mac_move_mon_thread_intvl_get(int unit, sal_usecs_t *interval);

extern int
soc_l2_mac_move_mon_feature_init_set(int unit);

extern int
soc_l2_mac_move_mon_feature_init_clear(int unit);

extern int
soc_l2_mac_move_mon_feature_start_set(int unit);

extern int
soc_l2_mac_move_mon_feature_start_clear(int unit);

extern int
soc_l2_mac_move_mon_feature_state_get(int unit, uint32 *state);

extern int
soc_l2_mac_move_mon_params_set(int unit, uint32 threshold, uint32 duration);

extern int
soc_l2_mac_move_mon_params_get(int unit, uint32 *threshold, uint32 *duration);

/* Entries are inserted by the producer thread */
extern int
soc_l2_mac_move_mon_entry_delete(int unit, sal_mac_addr_t mac, vlan_id_t vlan);

extern int
soc_l2_mac_move_mon_entry_lookup(int unit, sal_mac_addr_t mac, vlan_id_t vlan,
                                 int *found);

#endif /* _SOC_ESW_MACMOVE_MON_H_ */
