/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TSN_H__
#define __BCM_TSN_H__

#include <bcm/types.h>

/* Seamless Redundancy (SR) - Port SR type */
typedef enum bcm_tsn_sr_port_type_e {
    bcmTsnSrPortTypeNone = 0,       /* SR disabled on this port */
    bcmTsnSrPortTypeEthernet = 1,   /* Ethernet */
    bcmTsnSrPortTypePrp = 2,        /* PRP */
    bcmTsnSrPortTypeHsr = 3,        /* HSR */
    bcmTsnSrPortTypeDot1cb = 4,     /* 802.1CB */
    bcmTsnSrPortTypeCount = 5       /* Always last. Not a usable value. */
} bcm_tsn_sr_port_type_t;

/* Seamless Redundancy (SR) - Port SR mode */
typedef enum bcm_tsn_sr_port_mode_e {
    bcmTsnSrPortModeDefault = 0,        /* Default mode (custom value 0) */
    bcmTsnSrPortModeInterworkingPrp = 1, /* It's a PRP port in an interworking
                                           box */
    bcmTsnSrPortModeCustom1 = 2,        /* Custom value 1 */
    bcmTsnSrPortModeCustom2 = 3,        /* Custom value 2 */
    bcmTsnSrPortModeCustom3 = 4,        /* Custom value 3 */
    bcmTsnSrPortModeCustom4 = 5,        /* Custom value 4 */
    bcmTsnSrPortModeCustom5 = 6,        /* Custom value 5 */
    bcmTsnSrPortModeCustom6 = 7,        /* Custom value 6 */
    bcmTsnSrPortModeCount = 8           /* Always last. Not a usable value. */
} bcm_tsn_sr_port_mode_t;

/* Seamless Redundancy (SR) - port SR configuration */
typedef struct bcm_tsn_sr_port_config_s {
    bcm_tsn_sr_port_type_t port_type;   /* Port type */
    uint8 interlink_role;               /* Specifies whether it's an interlink
                                           port */
    uint8 mac_da_flows;                 /* SR flows classified by MAC-DA; 0
                                           otherwise */
    uint8 lan_id;                       /* 0 for LAN A, 1 for LAN B */
    uint8 net_id;                       /* NET_ID 0~7, 0 means don't care */
    bcm_tsn_sr_port_mode_t port_mode;   /* Port mode */
} bcm_tsn_sr_port_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the TSN module */
extern int bcm_tsn_init(
    int unit);

/* De-initialize the TSN module */
extern int bcm_tsn_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a port SR configuration structure. */
extern void bcm_tsn_sr_port_config_t_init(
    bcm_tsn_sr_port_config_t *port_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set or get Seamless Redundancy configuration on a port. */
extern int bcm_tsn_sr_port_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_sr_port_config_t *config);

/* Set or get Seamless Redundancy configuration on a port. */
extern int bcm_tsn_sr_port_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_sr_port_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_tsn_pri_map_entry_t */
#define BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET   0x00000001 
#define BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI   0x00000002 
#define BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID    0x00000004 
#define BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS 0x00000008 /* specify TAF gate id
                                                          for express traffic */
#define BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT 0x00000010 /* specify TAF gate id
                                                          for preemptable
                                                          traffic */

/* TSN priority mapping entry (one instance for one priority) */
typedef struct bcm_tsn_pri_map_entry_s {
    uint32 flags;           /* See BCM_TSN_PRI_MAP_ENTRY_xxx flags */
    int flow_offset;        /* Flow ID offset (0~7) */
    int new_int_pri;        /* New internal priority */
    int profile_id;         /* Profile ID */
    int taf_gate_express;   /* TAF gate for express traffic */
    int taf_gate_preempt;   /* TAF gate for preemptable traffic */
} bcm_tsn_pri_map_entry_t;

/* Initialize a bcm_tsn_pri_map_entry_t structure */
extern void bcm_tsn_pri_map_entry_t_init(
    bcm_tsn_pri_map_entry_t *entry);

/* TSN priority map type */
typedef enum bcm_tsn_pri_map_type_e {
    bcmTsnPriMapTypeTsnFlow = 0,        /* Mapping priority to TSN flows (TSN
                                           circuit ID) */
    bcmTsnPriMapTypeSrFlow = 1,         /* Mapping priority to SR flows */
    bcmTsnPriMapTypeEgressMtuProfile = 2, /* Mapping priority to egress MTU
                                           profile index */
    bcmTsnPriMapTypeEgressStuProfile = 3, /* Mapping priority to egress STU
                                           profile index */
    bcmTsnPriMapTypeTafGate = 4,        /* Mapping priority to TAF gate */
    bcmTsnPriMapTypeCount = 5           /* Always last. Not a usable value. */
} bcm_tsn_pri_map_type_t;

/* Used when priority map is not required (eg. default mapping is used). */
#define BCM_TSN_PRI_MAP_INVALID ((bcm_tsn_pri_map_t) 0) 

/* TSN priority map configuration */
typedef struct bcm_tsn_pri_map_config_s {
    bcm_tsn_pri_map_type_t map_type;    /* Type of this priority map */
    int num_map_entries;                /* Number of entries specified */
    bcm_tsn_pri_map_entry_t map_entries[16]; /* Mapping entry. The array index is the
                                           priority value to map. Index 0 (first
                                           entry in array) for priority 0, index
                                           1 for priority 1, and so forth. */
} bcm_tsn_pri_map_config_t;

/* Initialize a bcm_tsn_pri_map_config_t structure */
extern void bcm_tsn_pri_map_config_t_init(
    bcm_tsn_pri_map_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a Priority Map ID object. */
extern int bcm_tsn_pri_map_create(
    int unit, 
    bcm_tsn_pri_map_config_t *config, 
    bcm_tsn_pri_map_t *map_id);

/* Set the information to specified Priority Map ID. */
extern int bcm_tsn_pri_map_set(
    int unit, 
    bcm_tsn_pri_map_t map_id, 
    bcm_tsn_pri_map_config_t *config);

/* Get the information to specified Priority Map ID. */
extern int bcm_tsn_pri_map_get(
    int unit, 
    bcm_tsn_pri_map_t map_id, 
    bcm_tsn_pri_map_config_t *config);

/* Destroy a Priority Map object. */
extern int bcm_tsn_pri_map_destroy(
    int unit, 
    bcm_tsn_pri_map_t map_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN priority map traverse callback */
typedef int (*bcm_tsn_pri_map_traverse_cb)(
    int unit, 
    bcm_tsn_pri_map_t map_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all Priority Map. */
extern int bcm_tsn_pri_map_traverse(
    int unit, 
    bcm_tsn_pri_map_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* SR flow */
typedef uint32 bcm_tsn_sr_flow_t;

/* Flags for bcm_tsn_sr_tx_flow_config_t */
#define BCM_TSN_SR_TX_FLOW_CONFIG_DO_NOT_CUT_THROUGH 0x00000001 /* Do not cut-through for
                                                          the flow */

/* SR TX flow configuration */
typedef struct bcm_tsn_sr_tx_flow_config_s {
    uint32 flags; 
    uint32 seq_num;             /* Initial sequence number */
    int ingress_mtu_profile; 
} bcm_tsn_sr_tx_flow_config_t;

/* Initialize an SR TX flow configuration structure */
extern void bcm_tsn_sr_tx_flow_config_t_init(
    bcm_tsn_sr_tx_flow_config_t *flow_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an SR TX flow set */
extern int bcm_tsn_sr_tx_flowset_create(
    int unit, 
    bcm_tsn_pri_map_t pri_map, 
    bcm_tsn_sr_tx_flow_config_t *default_config, 
    bcm_tsn_sr_flowset_t *flowset);

/* Retrieve the initial configuration for an SR TX flow set */
extern int bcm_tsn_sr_tx_flowset_config_get(
    int unit, 
    bcm_tsn_sr_flowset_t flowset, 
    bcm_tsn_pri_map_t *pri_map, 
    bcm_tsn_sr_tx_flow_config_t *default_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_tsn_sr_rx_flow_config_t */
#define BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_DUPLICATES 0x00000001 /* Accept (don't drop) SR
                                                          duplicate packets */
#define BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_PKT_IN_DROP_WINDOW 0x00000002 /* Accept (don't drop)
                                                          packets with the
                                                          sequence number in the
                                                          drop window */
#define BCM_TSN_SR_RX_FLOW_CONFIG_DROP_OUT_OF_ORDER 0x00000004 /* Drop out of order
                                                          packets */
#define BCM_TSN_SR_RX_FLOW_CONFIG_DO_NOT_CUT_THROUGH 0x00000008 /* Do not cut-through for
                                                          the flow */

/* SR RX flow configuration */
typedef struct bcm_tsn_sr_rx_flow_config_s {
    uint32 flags; 
    uint32 seqnum_accept_win_size;  /* Sequence number acceptance window: 512,
                                       1024, ... 65536 */
    uint32 seqnum_history_win_size; /* Sequence number history window: 1, 64,
                                       128, ... 4096 */
    uint32 age_timeout;             /* Age timeout in ticks specified by
                                       bcmSrControlAgeTickInterval. 0 to disable
                                       aging. */
    int ingress_mtu_profile; 
} bcm_tsn_sr_rx_flow_config_t;

/* Initialize an SR RX flow configuration structure */
extern void bcm_tsn_sr_rx_flow_config_t_init(
    bcm_tsn_sr_rx_flow_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an SR RX flow set */
extern int bcm_tsn_sr_rx_flowset_create(
    int unit, 
    bcm_tsn_pri_map_t pri_map, 
    bcm_tsn_sr_rx_flow_config_t *default_config, 
    bcm_tsn_sr_flowset_t *flowset);

/* Retrieve the initial configuration for an SR RX flow set */
extern int bcm_tsn_sr_rx_flowset_config_get(
    int unit, 
    bcm_tsn_sr_flowset_t flowset, 
    bcm_tsn_pri_map_t *pri_map, 
    bcm_tsn_sr_rx_flow_config_t *default_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* SR flow set status */
typedef struct bcm_tsn_sr_flowset_status_s {
    int count;  /* Actual number of flows in this flowset */
    int active; /* Indicates whether it's active */
} bcm_tsn_sr_flowset_status_t;

/* Initialize an SR flow set status structure */
extern void bcm_tsn_sr_flowset_status_t_init(
    bcm_tsn_sr_flowset_status_t *status);

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve the current status for an SR TX or RX flow set */
extern int bcm_tsn_sr_flowset_status_get(
    int unit, 
    bcm_tsn_sr_flowset_t flowset, 
    bcm_tsn_sr_flowset_status_t *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* SR flow set traverse callback */
typedef int (*bcm_tsn_sr_flowset_traverse_cb)(
    int unit, 
    bcm_tsn_sr_flowset_t flowset, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all the created SR flow sets */
extern int bcm_tsn_sr_flowset_traverse(
    int unit, 
    int is_rx, 
    bcm_tsn_sr_flowset_traverse_cb cb, 
    void *user_data);

/* Destroy an SR flow set */
extern int bcm_tsn_sr_flowset_destroy(
    int unit, 
    bcm_tsn_sr_flowset_t flowset);

/* 
 * Retrieve an individual SR flow ID based on the flow offset from a flow
 * set.
 */
extern int bcm_tsn_sr_flowset_flow_get(
    int unit, 
    bcm_tsn_sr_flowset_t flowset, 
    int index, 
    bcm_tsn_sr_flow_t *flow_id);

/* Retrieve the flow configuration for an SR TX flow */
extern int bcm_tsn_sr_tx_flow_config_get(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    bcm_tsn_sr_tx_flow_config_t *config);

/* Set (modify) the flow configuration for an SR TX flow */
extern int bcm_tsn_sr_tx_flow_config_set(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    bcm_tsn_sr_tx_flow_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* SR TX flow status */
typedef struct bcm_tsn_sr_tx_flow_status_s {
    uint32 seq_num; /* Next sequence number to use */
} bcm_tsn_sr_tx_flow_status_t;

/* Initialize an SR TX flow status structure */
extern void bcm_tsn_sr_tx_flow_status_t_init(
    bcm_tsn_sr_tx_flow_status_t *status);

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve the current status for an SR TX flow */
extern int bcm_tsn_sr_tx_flow_status_get(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    bcm_tsn_sr_tx_flow_status_t *status);

/* Retrieve the flow configuration for an SR RX flow */
extern int bcm_tsn_sr_rx_flow_config_get(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    bcm_tsn_sr_rx_flow_config_t *config);

/* Set (modify) the flow configuration for an SR RX flow */
extern int bcm_tsn_sr_rx_flow_config_set(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    bcm_tsn_sr_rx_flow_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* SR RX flow status */
typedef struct bcm_tsn_sr_rx_flow_status_s {
    int seqnum_win_in_reset;            /* Sequence number window is in reset
                                           state */
    int seqnum_age_countdown;           /* Sequence number age count down (in
                                           ticks) */
    uint32 last_history_win_seqnum;     /* Last sequence number received in the
                                           sequence number history window */
    uint32 last_accepted_seqnum_lan[2]; /* Last sequence number accepted for LAN
                                           A/B */
    int wrong_lan[2];                   /* Set if any packet received on wrong
                                           LAN A/B */
    int wrong_lan_age_countdown[2];     /* Count down (in ticks) to clear wrong
                                           LAN A/B status */
} bcm_tsn_sr_rx_flow_status_t;

/* Initialize an SR RX flow status structure */
extern void bcm_tsn_sr_rx_flow_status_t_init(
    bcm_tsn_sr_rx_flow_status_t *status);

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve the current status for an SR RX flow */
extern int bcm_tsn_sr_rx_flow_status_get(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    bcm_tsn_sr_rx_flow_status_t *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_tsn_sr_rx_flow_reset */
#define BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_STATE 0x00000001 /* Reset sequence number
                                                          window state */
#define BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_HISTORY 0x00000002 /* Reset sequence number
                                                          history values */
#define BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_AGE_COUNTDOWN 0x00000004 /* Restart sequence
                                                          number age countdown */
#define BCM_TSN_SR_RX_FLOW_RESET_WRONGLAN_A_AGE_COUNTDOWN 0x00000008 /* Restart wrong LAN A
                                                          clear age countdown */
#define BCM_TSN_SR_RX_FLOW_RESET_WRONGLAN_B_AGE_COUNTDOWN 0x00000010 /* Restart wrong LAN B
                                                          clear age countdown */
#define BCM_TSN_SR_RX_FLOW_RESET_ALL        (BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_STATE |\
                                             BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_HISTORY |\
                                             BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_AGE_COUNTDOWN |\
                                             BCM_TSN_SR_RX_FLOW_RESET_WRONGLAN_A_AGE_COUNTDOWN |\
                                             BCM_TSN_SR_RX_FLOW_RESET_WRONGLAN_B_AGE_COUNTDOWN) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Reset an SR RX flow */
extern int bcm_tsn_sr_rx_flow_reset(
    int unit, 
    uint32 flags, 
    bcm_tsn_sr_flow_t flow_id);

/* Set the sequence number history bits in the RX flow. */
extern int bcm_tsn_sr_rx_flow_seqnum_history_set(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    int offset_in_bits, 
    int size_in_bits, 
    uint8 *history_bits);

/* Get the sequence number history bits in the RX flow. */
extern int bcm_tsn_sr_rx_flow_seqnum_history_get(
    int unit, 
    bcm_tsn_sr_flow_t flow_id, 
    int offset_in_bits, 
    int max_size_in_bits, 
    uint8 *history_bits, 
    int *actual_size_in_bits);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN flow */
typedef uint32 bcm_tsn_flow_t;

/* Flags for bcm_tsn_flow_config_t */
#define BCM_TSN_FLOW_CONFIG_DO_NOT_CUT_THROUGH 0x00000001 /* Do not cut-through for
                                                          the flow */

/* TSN flow configuration */
typedef struct bcm_tsn_flow_config_s {
    uint32 flags; 
    int ingress_mtu_profile;    /* Ingress MTU profile ID */
} bcm_tsn_flow_config_t;

/* Initialize a TSN flow configuration structure */
extern void bcm_tsn_flow_config_t_init(
    bcm_tsn_flow_config_t *flow_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a TSN flow set */
extern int bcm_tsn_flowset_create(
    int unit, 
    bcm_tsn_pri_map_t pri_map, 
    bcm_tsn_flow_config_t *default_config, 
    bcm_tsn_flowset_t *flowset);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN flow set status */
typedef struct bcm_tsn_flowset_status_s {
    int count;  /* Actual number of flows in this flowset */
    int active; /* Indicates whether it's active */
} bcm_tsn_flowset_status_t;

/* Initialize a TSN flow set status structure */
extern void bcm_tsn_flowset_status_t_init(
    bcm_tsn_flowset_status_t *status);

#ifndef BCM_HIDE_DISPATCHABLE

/* Retrieve the current status for a TSN flow set */
extern int bcm_tsn_flowset_status_get(
    int unit, 
    bcm_tsn_flowset_t flowset, 
    bcm_tsn_flowset_status_t *status);

/* 
 * Retrieve an individual TSN flow ID based on the flow offset from a
 * flow set.
 */
extern int bcm_tsn_flowset_flow_get(
    int unit, 
    bcm_tsn_flowset_t flowset, 
    int index, 
    bcm_tsn_flow_t *flow_id);

/* Retrieve the initial configuration for a TSN flow set */
extern int bcm_tsn_flowset_config_get(
    int unit, 
    bcm_tsn_flowset_t flowset, 
    bcm_tsn_pri_map_t *pri_map, 
    bcm_tsn_flow_config_t *default_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN flow set traverse callback */
typedef int (*bcm_tsn_flowset_traverse_cb)(
    int unit, 
    bcm_tsn_flowset_t flowset, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all the created TSN flow sets */
extern int bcm_tsn_flowset_traverse(
    int unit, 
    bcm_tsn_flowset_traverse_cb cb, 
    void *user_data);

/* Destroy a TSN flow set */
extern int bcm_tsn_flowset_destroy(
    int unit, 
    bcm_tsn_flowset_t flowset);

/* Retrieve the flow configuration for a TSN flow */
extern int bcm_tsn_flow_config_get(
    int unit, 
    bcm_tsn_flow_t flow_id, 
    bcm_tsn_flow_config_t *config);

/* Set (modify) the flow configuration for a TSN flow */
extern int bcm_tsn_flow_config_set(
    int unit, 
    bcm_tsn_flow_t flow_id, 
    bcm_tsn_flow_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_tsn_sr_auto_learn_group_config_t */
#define BCM_TSN_SR_AUTO_LEARN_GROUP_PROXY_FILTERING 0x00000001 /* Enable Proxy MAC
                                                          filtering */

/* SR auto learn group configuration */
typedef struct bcm_tsn_sr_auto_learn_group_config_s {
    uint32 flags;               /* BCM_TSN_SR_AUTO_LEARN_GROUP_xxx flags */
    bcm_pbmp_t interlink_ports; /* Bitmap of interlink ports in this group */
    bcm_pbmp_t redundant_ports; /* Bitmap of SR redundant ports in this group */
} bcm_tsn_sr_auto_learn_group_config_t;

/* Initialize a bcm_tsn_sr_auto_learn_group_config_t structure */
extern void bcm_tsn_sr_auto_learn_group_config_t_init(
    bcm_tsn_sr_auto_learn_group_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an SR auto learn group. */
extern int bcm_tsn_sr_auto_learn_group_create(
    int unit, 
    bcm_tsn_sr_auto_learn_group_config_t *config, 
    int *group_id);

/* Get the information to specified auto learn group ID. */
extern int bcm_tsn_sr_auto_learn_group_get(
    int unit, 
    int group_id, 
    bcm_tsn_sr_auto_learn_group_config_t *config);

/* Set the information to specified auto learn group ID. */
extern int bcm_tsn_sr_auto_learn_group_set(
    int unit, 
    int group_id, 
    bcm_tsn_sr_auto_learn_group_config_t *config);

/* Destroy a auto learn group object. */
extern int bcm_tsn_sr_auto_learn_group_destroy(
    int unit, 
    int group_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Traverse all SR auto learn groups */
typedef int (*bcm_tsn_sr_auto_learn_group_traverse_cb)(
    int unit, 
    int group_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all auto learn groups. */
extern int bcm_tsn_sr_auto_learn_group_traverse(
    int unit, 
    bcm_tsn_sr_auto_learn_group_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* SR auto learn configuration */
typedef struct bcm_tsn_sr_auto_learn_config_s {
    int num_tx_flows;                   /* Number of TX flows for auto learn */
    int num_rx_flows;                   /* Number of RX flows for auto learn */
    bcm_tsn_sr_tx_flow_config_t tx_flow_config; /* Default TX flow configuration */
    bcm_tsn_sr_rx_flow_config_t rx_flow_config; /* Default RX flow configuration */
} bcm_tsn_sr_auto_learn_config_t;

/* Initialize a bcm_tsn_sr_auto_learn_config_t structure */
extern void bcm_tsn_sr_auto_learn_config_t_init(
    bcm_tsn_sr_auto_learn_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Enable or disable the global SR auto learn feature. */
extern int bcm_tsn_sr_auto_learn_enable(
    int unit, 
    int enable, 
    bcm_tsn_sr_auto_learn_config_t *config);

/* Get the current status of SR auto learn. */
extern int bcm_tsn_sr_auto_learn_enable_get(
    int unit, 
    int *enabled, 
    bcm_tsn_sr_auto_learn_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Sequence number window reset mode when aged out */
#define BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW1 1          /* Hardware mode 1:
                                                          always accept the
                                                          first packet */
#define BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW2 2          /* Hardware mode 2:
                                                          expect sequence number
                                                          0 for the first SR
                                                          packet received */
#define BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW 3          /* Software (custom)
                                                          reset mode */

/* TSN controls */
typedef enum bcm_tsn_control_e {
    bcmTsnControlSrAgeTickInterval = 0, /* Age timer tick interval in msecs */
    bcmTsnControlSrAgeOutEnable = 1,    /* Global age out enable */
    bcmTsnControlSrSeqNumWindowResetMode = 2, /* See
                                           BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_xxx */
    bcmTsnControlSrLooseOutOfOrder = 3, /* Loose out of order definition. An SR
                                           packet is considered out of order if
                                           it's in the acceptance window and the
                                           equence number is less than previous
                                           received packet. */
    bcmTsnControlSrHsrEthertype = 4,    /* The EtherType value for the HSR tag */
    bcmTsnControlSrPrpEthertype = 5,    /* The EtherType (suffix) value for the
                                           PRP RCT trailer */
    bcmTsnControlSrDot1cbEthertype = 6, /* The EtherType value for the 802.1CB
                                           tag */
    bcmTsnControlIngressMtuProfile = 7, /* Ingress MTU profile ID (per port) */
    bcmTsnControlEgressMtuPriorityMap = 8, /* Priority mapping for egress MTU check
                                           (per-port) */
    bcmTsnControlIngressStuProfile = 9, /* Ingress STU profile ID (per port) */
    bcmTsnControlEgressStuPriorityMap = 10, /* Priority mapping for egress STU check
                                           (per-port) */
    bcmTsnControlTafEnable = 11,        /* Enable TAF (Time Aware Filtering)
                                           (per-port) */
    bcmTsnControlTafGatePriMap = 12,    /*  TAF gate priority mapping (per-port) */
    bcmTsnControlTafGatePriMapL2Select = 13, /* TAF gate priority mapping lookup by
                                           MAC-SA(0) or MAC-DA(1) (per-port) */
    bcmTsnControlCount = 14             /* Always last. Not a usable value. */
} bcm_tsn_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure device-wide operation modes for TSN */
extern int bcm_tsn_control_set(
    int unit, 
    bcm_tsn_control_t type, 
    uint32 arg);

/* Configure device-wide operation modes for TSN */
extern int bcm_tsn_control_get(
    int unit, 
    bcm_tsn_control_t type, 
    uint32 *arg);

/* Configure port-specific operation modes for TSN */
extern int bcm_tsn_port_control_set(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_control_t type, 
    uint32 arg);

/* Configure port-specific operation modes for TSN */
extern int bcm_tsn_port_control_get(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_control_t type, 
    uint32 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN stat types */
typedef enum bcm_tsn_stat_e {
    bcmTsnStatIngressSrTaggedPackets = 0, /* Number of SR tagged packets received */
    bcmTsnStatIngressNonLinkLocalPackets = 1, /* Number of all non-link-local packets
                                           received */
    bcmTsnStatIngressSrWrongLanPackets = 2, /* Number of PRP packets with wrong LAN
                                           ID */
    bcmTsnStatIngressSrRxErrorPackets = 3, /* Number of packets received with
                                           invalid flow or dropped due to errors */
    bcmTsnStatIngressSrTagErrorPackets = 4, /* Number of packets received with
                                           invalid SR tag */
    bcmTsnStatIngressSrDuplicatePackets = 5, /* Number of duplicate SR packets
                                           received */
    bcmTsnStatIngressSrOutOfOrderSrPackets = 6, /* Number of SR packets received out of
                                           order */
    bcmTsnStatIngressSrOwnRxPackets = 7, /* Number of SR packets received that
                                           originated from this device */
    bcmTsnStatIngressSrUnexpectedPackets = 8, /* Number of packets received with wrong
                                           SR tag */
    bcmTsnStatIngressMtuErrorPackets = 9, /* Number of packets received that have
                                           violated MTU size */
    bcmTsnStatIngressStuErrorPackets = 10, /* Number of packets received that have
                                           violated STU size */
    bcmTsnStatIngressSrAcceptedSrPackets = 11, /* Number of SR packets that are
                                           accepted and pass the duplicate
                                           discard mechanism */
    bcmTsnStatIngressSrSaSrcFilteredPackets = 12, /* Number of SR packets that dropped due
                                           to source port filtering */
    bcmTsnStatIngressTafMeterDrop = 13, /* Number of packets got dropped by the
                                           meter */
    bcmTsnStatIngressTafGatePass = 14,  /* Number of packets passed through the
                                           TAF gate */
    bcmTsnStatIngressTafGateCloseDrop = 15, /* Number of packets got dropped by TAF
                                           gate during close state */
    bcmTsnStatIngressTafGateNoByteDrop = 16, /* Number of packets got dropped by TAF
                                           gate during open state because of max
                                           bytes check failed */
    bcmTsnStatIngressTafMtuPass = 17,   /* Number of packets passed the MTU
                                           check */
    bcmTsnStatEgressSrTaggedPackets = 18, /* Number of SR tagged packets
                                           transmitted */
    bcmTsnStatEgressNonLinkLocalPackets = 19, /* Number of all non-link-local packets
                                           transmitted */
    bcmTsnStatEgressMtuErrorPackets = 20, /* Number of packets transmitted that
                                           have violated MTU size */
    bcmTsnStatEgressStuErrorPackets = 21, /* Number of packets transmitted that
                                           have violated STU size */
    bcmTsnStatCount = 22                /* Always last. Not a usable value. */
} bcm_tsn_stat_t;

/* TSN TAF gate stat types */
typedef enum bcm_tsn_taf_gate_stat_e {
    bcmTsnTafGateStatPassed = 0,    /* Number of packets passed TAF gate */
    bcmTsnTafGateStatDrop = 1,      /* Number of packets dropped by TAF gate */
    bcmTsnTafGateStatCount = 2      /* Always last. Not a usable value. */
} bcm_tsn_taf_gate_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the specified TSN statistic type to the device. */
extern int bcm_tsn_port_stat_set(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_stat_t stat, 
    uint64 val);

/* Set the specified TSN statistic type to the device. */
extern int bcm_tsn_port_stat_set32(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_stat_t stat, 
    uint32 val);

/* Get the specified TSN statistic type from the device. */
extern int bcm_tsn_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_stat_t stat, 
    uint64 *val);

/* Get the specified TSN statistic type from the device. */
extern int bcm_tsn_port_stat_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_stat_t stat, 
    uint32 *val);

/* Set the specified multiple TSN statistics to the device. */
extern int bcm_tsn_port_stat_multi_set(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint64 *value_arr);

/* Set the specified multiple TSN statistics to the device. */
extern int bcm_tsn_port_stat_multi_set32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint32 *value_arr);

/* Get the specified multiple TSN statistics from the device. */
extern int bcm_tsn_port_stat_multi_get(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint64 *value_arr);

/* Get the specified multiple TSN statistics from the device. */
extern int bcm_tsn_port_stat_multi_get32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Force an immediate counter update and get TSN statistics for a
 * specified device.
 */
extern int bcm_tsn_port_stat_sync_get(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and get TSN statistics for a
 * specified device.
 */
extern int bcm_tsn_port_stat_sync_get32(
    int unit, 
    bcm_gport_t port, 
    bcm_tsn_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_tsn_port_stat_sync_multi_get(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_tsn_port_stat_sync_multi_get32(
    int unit, 
    bcm_gport_t port, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint32 *value_arr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN statistic group */
typedef int bcm_tsn_stat_group_t;

/* TSN statistic group type */
typedef enum bcm_tsn_stat_group_type_e {
    bcmTsnStatGroupTypeIngress = 0, /* Group for bcmTsnStatIngressXXX stats */
    bcmTsnStatGroupTypeEgress = 1,  /* Group for bcmTsnStatEgressXXX stats */
    bcmTsnStatGroupTypeCount = 2    /* Always last. Not a usable value. */
} bcm_tsn_stat_group_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a TSN stat group */
extern int bcm_tsn_stat_group_create(
    int unit, 
    bcm_tsn_stat_group_type_t group_type, 
    int count, 
    bcm_tsn_stat_t *stat_arr, 
    bcm_tsn_stat_group_t *id);

/* Get configuration of the TSN stat group */
extern int bcm_tsn_stat_group_get(
    int unit, 
    bcm_tsn_stat_group_t id, 
    bcm_tsn_stat_group_type_t *group_type, 
    int max, 
    bcm_tsn_stat_t *stat_arr, 
    int *count);

/* Set configuration of the TSN stat group */
extern int bcm_tsn_stat_group_set(
    int unit, 
    bcm_tsn_stat_group_t id, 
    int count, 
    bcm_tsn_stat_t *stat_arr);

/* Destroy a TSN stat group. */
extern int bcm_tsn_stat_group_destroy(
    int unit, 
    bcm_tsn_stat_group_t id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN stat group traverse callback */
typedef int (*bcm_tsn_stat_group_traverse_cb)(
    int unit, 
    bcm_tsn_stat_group_t id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse TSN stat groups */
extern int bcm_tsn_stat_group_traverse(
    int unit, 
    bcm_tsn_stat_group_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Invalid TSN stat group. Used when counting is disabled. */
#define BCM_TSN_STAT_GROUP_INVALID  ((bcm_tsn_stat_group_t) -1) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set flow attached to stat group */
extern int bcm_tsn_sr_flow_stat_group_set(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_group_type_t group_type, 
    bcm_tsn_stat_group_t stat_group);

/* Get the stat group attached on this flow */
extern int bcm_tsn_sr_flow_stat_group_get(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_group_type_t group_type, 
    bcm_tsn_stat_group_t *stat_group);

/* Set the specified TSN statistic type to the flow. */
extern int bcm_tsn_sr_flow_stat_set(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_t stat, 
    uint64 val);

/* Set the specified TSN statistic type to the flow. */
extern int bcm_tsn_sr_flow_stat_set32(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_t stat, 
    uint32 val);

/* Get the specified TSN statistic type on this flow. */
extern int bcm_tsn_sr_flow_stat_get(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_t stat, 
    uint64 *val);

/* Get the specified TSN statistic type on this flow. */
extern int bcm_tsn_sr_flow_stat_get32(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_t stat, 
    uint32 *val);

/* Set the specified multiple TSN statistics to the flow. */
extern int bcm_tsn_sr_flow_stat_multi_set(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint64 *value_arr);

/* Set the specified multiple TSN statistics to the flow. */
extern int bcm_tsn_sr_flow_stat_multi_set32(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint32 *value_arr);

/* Get the specified multiple TSN statistics on the flow. */
extern int bcm_tsn_sr_flow_stat_multi_get(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint64 *value_arr);

/* Get the specified multiple TSN statistics on the flow. */
extern int bcm_tsn_sr_flow_stat_multi_get32(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint32 *value_arr);

/* 
 * Force an immediate counter update and get TSN statistics for a
 * specified flow.
 */
extern int bcm_tsn_sr_flow_stat_sync_get(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and get TSN statistics for a
 * specified flow.
 */
extern int bcm_tsn_sr_flow_stat_sync_get32(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    bcm_tsn_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_tsn_sr_flow_stat_sync_multi_get(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint64 *value_arr);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_tsn_sr_flow_stat_sync_multi_get32(
    int unit, 
    bcm_tsn_sr_flow_t flow, 
    int nstat, 
    bcm_tsn_stat_t *stat_arr, 
    uint32 *value_arr);

/* Set the specified TAF gate statistic. */
extern int bcm_tsn_taf_gate_stat_set(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_stat_t stat, 
    uint64 val);

/* Set the specified TAF gate statistic. */
extern int bcm_tsn_taf_gate_stat_set32(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_stat_t stat, 
    uint32 val);

/* Get the specified TAF gate statistic. */
extern int bcm_tsn_taf_gate_stat_get(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_stat_t stat, 
    uint64 *val);

/* Get the specified TAF gate statistic. */
extern int bcm_tsn_taf_gate_stat_get32(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_stat_t stat, 
    uint32 *val);

/* Set the specified multiple TSN statistics. */
extern int bcm_tsn_taf_gate_stat_multi_set(
    int unit, 
    int taf_gate_id, 
    int nstat, 
    bcm_tsn_taf_gate_stat_t *stat_arr, 
    uint64 *val_arr);

/* Set the specified multiple TSN statistics. */
extern int bcm_tsn_taf_gate_stat_multi_set32(
    int unit, 
    int taf_gate_id, 
    int nstat, 
    bcm_tsn_taf_gate_stat_t *stat_arr, 
    uint32 *val_arr);

/* Get the specified multiple TSN statistics on the gate. */
extern int bcm_tsn_taf_gate_stat_multi_get(
    int unit, 
    int taf_gate_id, 
    int nstat, 
    bcm_tsn_taf_gate_stat_t *stat_arr, 
    uint64 *val_arr);

/* Get the specified multiple TSN statistics on the gate. */
extern int bcm_tsn_taf_gate_stat_multi_get32(
    int unit, 
    int taf_gate_id, 
    int nstat, 
    bcm_tsn_taf_gate_stat_t *stat_arr, 
    uint32 *val_arr);

/* 
 * Force an immediate counter update and get TSN statistics for a
 * specified gate.
 */
extern int bcm_tsn_taf_gate_stat_sync_get(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_stat_t stat, 
    uint64 *val);

/* 
 * Force an immediate counter update and get TSN statistics for a
 * specified gate.
 */
extern int bcm_tsn_taf_gate_stat_sync_get32(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_stat_t stat, 
    uint32 *val);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_tsn_taf_gate_stat_sync_multi_get(
    int unit, 
    int taf_gate_id, 
    int nstat, 
    bcm_tsn_taf_gate_stat_t *stat_arr, 
    uint64 *val_arr);

/* 
 * Force an immediate counter update and get a set of statistics for a
 * specified device.
 */
extern int bcm_tsn_taf_gate_stat_sync_multi_get32(
    int unit, 
    int taf_gate_id, 
    int nstat, 
    bcm_tsn_taf_gate_stat_t *stat_arr, 
    uint32 *val_arr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_tsn_stat_threshold_config_t */
#define BCM_TSN_STAT_THRESHOLD_EVENT_NOTIFICATION 0x00000001 /* Notify via registered
                                                          event notification
                                                          callback */
#define BCM_TSN_STAT_THRESHOLD_COPY_TO_CPU  0x00000002 /* Copy the packet to CPU */

/* TSN stat threshold configuration */
typedef struct bcm_tsn_stat_threshold_config_s {
    uint32 flags;       /* See BCM_TSN_STAT_THRESHOLD_xxx. If it's 0, no
                           threshold checking will be performed. */
    uint32 threshold;   /* Threshold value. 0 means disabled. */
} bcm_tsn_stat_threshold_config_t;

/* Initialize a TSN statistics threshold configuration structure */
extern void bcm_tsn_stat_threshold_config_t_init(
    bcm_tsn_stat_threshold_config_t *config);

/* Stat source for TSN stat threshold check */
typedef enum bcm_tsn_stat_threshold_source_e {
    bcmTsnStatThresholdSourcePort = 0,  /* Threshold check on a specific port */
    bcmTsnStatThresholdSourceSRFlow = 1 /* Threshold check on a specific SR flow */
} bcm_tsn_stat_threshold_source_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure/Get threshold for a specific statistics type on a specific
 * source.
 */
extern int bcm_tsn_stat_threshold_set(
    int unit, 
    bcm_tsn_stat_threshold_source_t source, 
    bcm_tsn_stat_t stat, 
    bcm_tsn_stat_threshold_config_t *config);

/* 
 * Configure/Get threshold for a specific statistics type on a specific
 * source.
 */
extern int bcm_tsn_stat_threshold_get(
    int unit, 
    bcm_tsn_stat_threshold_source_t source, 
    bcm_tsn_stat_t stat, 
    bcm_tsn_stat_threshold_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN event types */
typedef enum bcm_tsn_event_type_e {
    bcmTsnEventTypeCounterExceeded = 0, /* Counter threshold exceeds (flow or
                                           port) */
    bcmTsnEventTypeSrWrongLanA = 1,     /* Wrong LAN packet received on LAN A
                                           (per flow) */
    bcmTsnEventTypeSrWrongLanB = 2,     /* Wrong LAN packet received on LAN B
                                           (per flow) */
    bcmTsnEventTypeSrSeqNumWindowAgeOut = 3, /* Sequence number window aged out if
                                           using software reset mode (per flow) */
    bcmTsnEventTypeCount = 4            /* Always Last. Not a usable value. */
} bcm_tsn_event_type_t;

/* TSN event source types */
#define BCM_TSN_EVENT_SOURCE_TYPE_SYSTEM    0          /* System event or source
                                                          unknown */
#define BCM_TSN_EVENT_SOURCE_TYPE_PORT      1          /* Event from a specific
                                                          port */
#define BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW   2          /* Event from a specific
                                                          SR flow */

/* The source that triggers the TSN event */
typedef struct bcm_tsn_event_source_s {
    int source_type;            /* Type of the event source */
    bcm_gport_t port;           /* Port number. Valid only if the source is a
                                   port */
    bcm_tsn_sr_flow_t sr_flow;  /* SR flow ID. Valid only if the source is an SR
                                   flow */
} bcm_tsn_event_source_t;

/* Initialize a TSN event source structure */
extern void bcm_tsn_event_source_t_init(
    bcm_tsn_event_source_t *source);

/* TSN event notification callback function prototype */
typedef int (*bcm_tsn_event_cb)(
    int unit, 
    bcm_tsn_event_type_t event, 
    bcm_tsn_event_source_t *source, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register/unregister a callback function for handling TSN events */
extern int bcm_tsn_event_register(
    int unit, 
    bcm_tsn_event_type_t event, 
    bcm_tsn_event_source_t *src, 
    bcm_tsn_event_cb cb, 
    void *user_data);

/* Register/unregister a callback function for handling TSN events */
extern int bcm_tsn_event_unregister(
    int unit, 
    bcm_tsn_event_type_t event, 
    bcm_tsn_event_source_t *src, 
    bcm_tsn_event_cb cb);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN event notification traverse callback */
typedef int (*bcm_tsn_event_notification_traverse_cb)(
    int unit, 
    bcm_tsn_event_type_t event, 
    bcm_tsn_event_source_t *src, 
    bcm_tsn_event_cb cb, 
    void *event_cb_user_data, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse all registered event notification callbacks */
extern int bcm_tsn_event_notification_traverse(
    int unit, 
    bcm_tsn_event_notification_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MTU profile configuration for TSN */
typedef struct bcm_tsn_mtu_config_s {
    uint16 size;    /* Size in bytes to check. 0 to disable checking. */
    uint8 drop;     /* Drop violated packets if 1 */
} bcm_tsn_mtu_config_t;

/* Initialize a TSN MTU profile configuration structure */
extern void bcm_tsn_mtu_config_t_init(
    bcm_tsn_mtu_config_t *config);

/* Types of MTU profiles */
typedef enum bcm_tsn_mtu_profile_type_e {
    bcmTsnMtuProfileTypeIngress = 0,    /* MTU checking on ingress */
    bcmTsnMtuProfileTypeEgress = 1,     /* MTU checking on egress */
    bcmTsnMtuProfileTypeCount = 2       /* Always last. Not a usable value. */
} bcm_tsn_mtu_profile_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create an MTU profile with the config file and return the assigned
 * profile id.
 */
extern int bcm_tsn_mtu_profile_create(
    int unit, 
    bcm_tsn_mtu_profile_type_t type, 
    bcm_tsn_mtu_config_t *config, 
    int *mtu_profile_id);

/* Get the existing MTU profile configuration with the profile id. */
extern int bcm_tsn_mtu_profile_get(
    int unit, 
    int mtu_profile_id, 
    bcm_tsn_mtu_profile_type_t *type, 
    bcm_tsn_mtu_config_t *config);

/* Set the existing MTU profile with the profile id and config file. */
extern int bcm_tsn_mtu_profile_set(
    int unit, 
    int mtu_profile_id, 
    bcm_tsn_mtu_config_t *config);

/* Clear the existing MTU profile with the profile id. */
extern int bcm_tsn_mtu_profile_destroy(
    int unit, 
    int mtu_profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN MTU profile traverse callback */
typedef int (*bcm_tsn_mtu_profile_traverse_cb)(
    int unit, 
    int profile_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the created MTU profile. */
extern int bcm_tsn_mtu_profile_traverse(
    int unit, 
    bcm_tsn_mtu_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Sources from which ingress MTU profile to check is determined */
typedef enum bcm_tsn_mtu_source_e {
    bcmTsnMtuSourceTsnFlow = 0,     /* MTU profile from TSN flow */
    bcmTsnMtuSourceSrFlow = 1,      /* MTU profile from SR flow */
    bcmTsnMtuSourcePort = 2,        /* MTU profile from port */
    bcmTsnMtuSourceFieldLookup = 3, /* MTU profile from VFP action */
    bcmTsnMtuSourceCount = 4        /* Always last. Not a usable value. */
} bcm_tsn_mtu_source_t;

/* Ingress MTU system configuration */
typedef struct bcm_tsn_ingress_mtu_config_s {
    bcm_tsn_mtu_source_t source_order[bcmTsnMtuSourceCount]; /* Specify MTU sources in the order of
                                           precedence. Source at array index 0
                                           has highest precedence. */
    int source_order_count; 
} bcm_tsn_ingress_mtu_config_t;

/* Initialize a TSN ingress MTU configuration structure */
extern void bcm_tsn_ingress_mtu_config_t_init(
    bcm_tsn_ingress_mtu_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set up global configuration for ingress MTU, including the priority of
 * the
 * following sources: port, VFP, SR flow and TSN flow.
 */
extern int bcm_tsn_ingress_mtu_config_set(
    int unit, 
    bcm_tsn_ingress_mtu_config_t *config);

/* Get the global configuration for ingress MTU source priority sequence. */
extern int bcm_tsn_ingress_mtu_config_get(
    int unit, 
    bcm_tsn_ingress_mtu_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* STU profile configuration for TSN */
typedef struct bcm_tsn_stu_config_s {
    uint16 size;    /* Size in bytes to check. 0 to disable checking. */
    uint8 drop;     /* Drop violated packets if 1 */
} bcm_tsn_stu_config_t;

/* Initialize a TSN STU profile configuration structure */
extern void bcm_tsn_stu_config_t_init(
    bcm_tsn_stu_config_t *config);

/* Types of STU profiles */
typedef enum bcm_tsn_stu_profile_type_e {
    bcmTsnStuProfileTypeIngress = 0,    /* STU checking on ingress */
    bcmTsnStuProfileTypeEgress = 1,     /* STU checking on egress */
    bcmTsnStuProfileTypeCount = 2       /* Always last. Not a usable value. */
} bcm_tsn_stu_profile_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create an STU profile with the config file and return the assigned
 * profile id.
 */
extern int bcm_tsn_stu_profile_create(
    int unit, 
    bcm_tsn_stu_profile_type_t type, 
    bcm_tsn_stu_config_t *config, 
    int *stu_profile_id);

/* Get the existing STU profile configuration with the profile id. */
extern int bcm_tsn_stu_profile_get(
    int unit, 
    int stu_profile_id, 
    bcm_tsn_stu_profile_type_t *type, 
    bcm_tsn_stu_config_t *config);

/* Set the existing STU profile with the profile id and config file. */
extern int bcm_tsn_stu_profile_set(
    int unit, 
    int stu_profile_id, 
    bcm_tsn_stu_config_t *config);

/* Clear the existing STU profile with the profile id. */
extern int bcm_tsn_stu_profile_destroy(
    int unit, 
    int stu_profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN STU profile traverse callback */
typedef int (*bcm_tsn_stu_profile_traverse_cb)(
    int unit, 
    int profile_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the created STU profile. */
extern int bcm_tsn_stu_profile_traverse(
    int unit, 
    bcm_tsn_stu_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Sources from which ingress STU profile to check is determined */
typedef enum bcm_tsn_stu_source_e {
    bcmTsnStuSourceFieldLookup = 0, /* STU profile from VFP action */
    bcmTsnStuSourcePort = 1,        /* STU profile from port */
    bcmTsnStuSourceCount = 2        /* Always last. Not a usable value. */
} bcm_tsn_stu_source_t;

/* Ingress STU system configuration */
typedef struct bcm_tsn_ingress_stu_config_s {
    bcm_tsn_stu_source_t source_order[bcmTsnStuSourceCount]; /* Specify STU sources in the order or
                                           precedence. Source at array index 0
                                           has highest precedence. */
    int source_order_count; 
} bcm_tsn_ingress_stu_config_t;

/* Initialize a TSN ingress STU configuration structure */
extern void bcm_tsn_ingress_stu_config_t_init(
    bcm_tsn_ingress_stu_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set up global configuration for ingress STU, including the priority of
 * the following sources: port, VFP.
 */
extern int bcm_tsn_ingress_stu_config_set(
    int unit, 
    bcm_tsn_ingress_stu_config_t *config);

/* Get the global configuration for ingress STU source priority sequence. */
extern int bcm_tsn_ingress_stu_config_get(
    int unit, 
    bcm_tsn_ingress_stu_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Use the specified gate Id. */
#define BCM_TSN_TAF_WITH_ID     (1 << 0)   

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/Destroy TAF gate */
extern int bcm_tsn_taf_gate_create(
    int unit, 
    int flags, 
    int *taf_gate_id);

/* Create/Destroy TAF gate */
extern int bcm_tsn_taf_gate_destroy(
    int unit, 
    int taf_gate_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN traverse callback function for TAF gates */
typedef int (*bcm_tsn_taf_gate_traverse_cb)(
    int unit, 
    int taf_gate, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse TAF gate */
extern int bcm_tsn_taf_gate_traverse(
    int unit, 
    bcm_tsn_taf_gate_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAF status type */
typedef enum bcm_tsn_taf_status_e {
    bcmTsnTafStatusGateState = 0,       /* The current TAF gate state. */
    bcmTsnTafStatusGateStateSet = 1,    /* Reflect which control gate source is
                                           selected. The value = 0 indicates the
                                           gate state is from INIT GATE STATE
                                           settings, value = 1 indicates the
                                           gate state is from active control
                                           list table, value = 2 indicates the
                                           gate state is from ERR GATE STATE
                                           settings */
    bcmTsnTafStatusGateCosProfile = 2,  /* The Cos profile this TAF gate use. */
    bcmTsnTafStatusGateMaxBytesProfile = 3, /* The max bytes profile this TAF gate
                                           use. */
    bcmTsnTafStatusTickGranularity = 4, /* The granularity of the calendar table
                                           time clock in ns. */
    bcmTsnTafStatusMaxCalendarEntries = 5, /* The max entries could be support in
                                           calendar table. */
    bcmTsnTafStatusGatMaxBytesLeft = 6, /* Bytes available that can pass through
                                           that gate during current open state. */
    bcmTsnTafStatusCount = 7            /* This should be the last one. */
} bcm_tsn_taf_status_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the TAF status */
extern int bcm_tsn_taf_status_get(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_status_t type, 
    uint32 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAF control type */
typedef enum bcm_tsn_taf_control_e {
    bcmTsnTafControlEnable = 0,         /* Enable the TAF. */
    bcmTsnTafControlUsePTPTime = 1,     /* PTP time is used for TAF synchronous
                                           operation. */
    bcmTsnTafControlInitGateState = 2,  /* Gate state when TAF is disabled. Or
                                           the init gate state in the *Restart*
                                           process. */
    bcmTsnTafControlInitCosProfile = 3, /* Cos Profile when TAF is disabled. Or
                                           the init cos Profile in the *Restart*
                                           process. */
    bcmTsnTafControlInitMaxBytesProfile = 4, /* Max bytes Profile when TAF is
                                           disabled. Or the init max bytes
                                           Profile in the *Restart* process. */
    bcmTsnTafControlErrGateState = 5,   /* Gate state when error condition
                                           occurs during an old cycle is
                                           running. */
    bcmTsnTafControlErrCosProfile = 6,  /* Cos profile when error condition
                                           occurs during an old cycle is
                                           running. */
    bcmTsnTafControlErrMaxByteProfile = 7, /* Max bytes profile when error
                                           condition occurs during an old cycle
                                           is running. */
    bcmTsnTafControlGateCloseControl = 8, /* if set, it will override the gate
                                           purge and drop decisions during gate
                                           close state and it will let the
                                           packet go. */
    bcmTsnTafControlBytesLeftCheckEnable = 9, /* Enable byte left check or not */
    bcmTsnTafControlGateControlTickInterval = 10, /* Gate control ticks interval in ns. */
    bcmTsnTafControlTAFPTPLock = 11,    /* Indicate the PTP time is locked or
                                           out of sync. This type is on system
                                           basis. */
    bcmTsnTafControlCount = 12          /* This should be the last one. */
} bcm_tsn_taf_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/get TAF control configurations. */
extern int bcm_tsn_taf_control_set(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_control_t type, 
    uint32 arg);

/* Set/get TAF control configurations. */
extern int bcm_tsn_taf_control_get(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_control_t type, 
    uint32 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

/* The maximum TAF calendar table size. */
#define BCM_TSN_TAF_CALENDAR_TABLE_SIZE (16)       

/* 
 * The encoded value used to assigned in ticks field on
 * bcm_tsn_taf_entry_t
 */
#define BCM_TSN_TAF_ENTRY_TICKS_STAY        (-1)       /* Specify to stay in
                                                          this entry forever in
                                                          non PTP_MODE or until
                                                          the next cycle in
                                                          PTP_MODE. */
#define BCM_TSN_TAF_ENTRY_TICKS_GO_FIRST    (0)        /* Specify to jump to the
                                                          first entry. The state
                                                          of this entry will be
                                                          ignored. */

/* The TAF entry data structure */
typedef struct bcm_tsn_taf_entry_s {
    uint32 ticks;           /* The scheduling interval in ticks */
    uint32 state;           /* Specify gate state (1:open, 0:close) */
    int cos_profile;        /* Class of service (COS) mapping profile, value 0
                               for default */
    int maxbyte_profile;    /* Max bytes profile, value 0 for default */
} bcm_tsn_taf_entry_t;

/* The TAF profile data structure */
typedef struct bcm_tsn_taf_profile_s {
    int num_entries;                    /* Specify the number of entries in the
                                           calendar table. */
    bcm_tsn_taf_entry_t entries[BCM_TSN_TAF_CALENDAR_TABLE_SIZE]; /* Specify the interval, gate states,
                                           attribute entry sets in the calendar
                                           table. */
    bcm_ptp_timestamp_t ptp_base_time;  /* Specify base time in PTP time. */
    uint32 ptp_cycle_time;              /* Specify cycle time in ns. */
    uint32 ptp_max_cycle_extension;     /* Specify max cycle extension time in
                                           ns. Maximum time allowed for a cycle
                                           to be extended before profile change. */
} bcm_tsn_taf_profile_t;

/* TAF profile state */
typedef enum bcm_tsn_taf_profile_state_e {
    bcmTsnTafProfileInit = 0,       /* the state after profile created. */
    bcmTsnTafProfileCommitted = 1,  /* the state after profile committed but not
                                       yet written to HW table */
    bcmTsnTafProfilePending = 2,    /*  the state after profile is set to
                                       hardware but not yet effective */
    bcmTsnTafProfileActive = 3,     /* the state indicate the profile is
                                       effective */
    bcmTsnTafProfileExpired = 4,    /* The profile is no longer effective. */
    bcmTsnTafProfileError = 5,      /* the state indicate the profile is error
                                       when any error event happens */
    bcmTsnTafProfileCount = 6       /* This should be the last one. */
} bcm_tsn_taf_profile_state_t;

/* TAF profile id */
typedef int bcm_tsn_taf_profile_id_t;

/* Initialize the bcm_tsn_taf_profile_t structure */
extern void bcm_tsn_taf_profile_t_init(
    bcm_tsn_taf_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the TAF profile */
extern int bcm_tsn_taf_profile_create(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_t *profile, 
    bcm_tsn_taf_profile_id_t *pid);

/* Destroy the TAF profile */
extern int bcm_tsn_taf_profile_destroy(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_id_t pid);

/* Destroy all TAF profiles. */
extern int bcm_tsn_taf_profile_destroy_all(
    int unit, 
    int taf_gate);

/* Get/set TAF profile information */
extern int bcm_tsn_taf_profile_set(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_id_t pid, 
    bcm_tsn_taf_profile_t *profile);

/* Get/set TAF profile information */
extern int bcm_tsn_taf_profile_get(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_id_t pid, 
    bcm_tsn_taf_profile_t *profile);

/* Commit the TAF profile */
extern int bcm_tsn_taf_profile_commit(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_id_t pid);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TAF profile traverse callback */
typedef int (*bcm_tsn_taf_profile_traverse_cb)(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_id_t pid, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the TAF profile */
extern int bcm_tsn_taf_profile_traverse(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* The TAF profile status structure */
typedef struct bcm_tsn_taf_profile_status_s {
    bcm_tsn_taf_profile_state_t profile_state; /* profile state */
    bcm_ptp_timestamp_t config_change_time; /* The actual config change time */
    int num_entries;                    /* The actual number of entries in
                                           hardware calendar table. */
    bcm_tsn_taf_entry_t entries[BCM_TSN_TAF_CALENDAR_TABLE_SIZE]; /* The actual entries in hardware
                                           calendar table. */
} bcm_tsn_taf_profile_status_t;

/* Initialize the bcm_tsn_taf_profile_status_t structure */
extern void bcm_tsn_taf_profile_status_t_init(
    bcm_tsn_taf_profile_status_t *profile_status);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the TAF profile status */
extern int bcm_tsn_taf_profile_status_get(
    int unit, 
    int taf_gate, 
    bcm_tsn_taf_profile_id_t pid, 
    bcm_tsn_taf_profile_status_t *status);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Tsn Taf event type */
typedef enum bcm_tsn_taf_event_type_e {
    bcmTsnTafEventTAFProfileChange = 0, /* TAF: the profile change is completed. */
    bcmTsnTafEventTAFECCErr = 1,        /* TAF: uncorrectable ECC error in
                                           profile calendar table. */
    bcmTsnTafEventTAFNextCycleTimeErr = 2, /* TAF: next cycle start time has
                                           passed. */
    bcmTsnTafEventTAFBaseTimeErr = 3,   /* TAF: base time for the new profile
                                           has passed. */
    bcmTsnTafEventTAFProfilePtrErr = 4, /* TAF: index to the profile calendar
                                           table is overrun. */
    bcmTsnTafEventTAFPTPOutOfSyncErr = 5, /* TAF: IEEE1588 PTP time is out of
                                           sync. */
    bcmTsnTafEventCount = 6             /* This should be the last one. */
} bcm_tsn_taf_event_type_t;

typedef struct bcm_tsn_taf_event_types_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(bcmTsnTafEventCount)]; 
} bcm_tsn_taf_event_types_t;

/* helper macro for TAF event handling */
#define BCM_TSN_TAF_EVENT_TYPE_SET(_event_types, _event_type)  SHR_BITSET(((_event_types).w), (_event_type)) 

#define BCM_TSN_TAF_EVENT_TYPE_GET(_event_types, _event_type)  SHR_BITGET(((_event_types).w), (_event_type)) 

#define BCM_TSN_TAF_EVENT_TYPE_CLEAR(_event_types, _event_type)  SHR_BITCLR(((_event_types).w), (_event_type)) 

#define BCM_TSN_TAF_EVENT_TYPE_SET_ALL(_event_types)  SHR_BITSET_RANGE(((_event_types).w), 0, bcmTsnTafEventCount) 

#define BCM_TSN_TAF_EVENT_TYPE_CLEAR_ALL(_event_types)  SHR_BITCLR_RANGE(((_event_types).w), 0, bcmTsnTafEventCount) 

/* Callback function type for Tsn Taf event handling */
typedef int (*bcm_tsn_taf_event_cb)(
    int unit, 
    bcm_tsn_taf_event_type_t event_type, 
    int taf_gate, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register/unregister a callback to handle the TAF events */
extern int bcm_tsn_taf_event_register(
    int unit, 
    bcm_tsn_taf_event_types_t event_types, 
    int taf_gate, 
    bcm_tsn_taf_event_cb cb, 
    void *user_data);

/* Register/unregister a callback to handle the TAF events */
extern int bcm_tsn_taf_event_unregister(
    int unit, 
    bcm_tsn_taf_event_types_t event_types, 
    int taf_gate, 
    bcm_tsn_taf_event_cb cb);

/* Create/destroy profile of maximum bytes that pass through the TAF gate */
extern int bcm_tsn_taf_gate_max_bytes_profile_create(
    int unit, 
    int taf_gate_id, 
    uint64 max_bytes, 
    int *profile_id);

/* Set/get profile of maximum bytes that pass through the TAF gate */
extern int bcm_tsn_taf_gate_max_bytes_profile_set(
    int unit, 
    int taf_gate_id, 
    int profile_id, 
    uint64 max_bytes);

/* Set/get profile of maximum bytes that pass through the TAF gate */
extern int bcm_tsn_taf_gate_max_bytes_profile_get(
    int unit, 
    int taf_gate_id, 
    int profile_id, 
    uint64 *max_bytes);

/* Create/destroy profile of maximum bytes that pass through the TAF gate */
extern int bcm_tsn_taf_gate_max_bytes_profile_destroy(
    int unit, 
    int taf_gate_id, 
    int profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN traverse callback function for TAF gate max bytes */
typedef int (*bcm_tsn_taf_gate_max_bytes_profile_traverse_cb)(
    int unit, 
    int taf_gate_id, 
    int profile_id, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse profile of maximum bytes that pass through the TAF gate */
extern int bcm_tsn_taf_gate_max_bytes_profile_traverse(
    int unit, 
    int taf_gate_id, 
    bcm_tsn_taf_gate_max_bytes_profile_traverse_cb cb, 
    void *user_data);

/* Create/destroy TAF Cos mapping profile */
extern int bcm_tsn_taf_cosq_mapping_profile_create(
    int unit, 
    int *cosq_profile);

/* Set/get TAF Cos mapping profile */
extern int bcm_tsn_taf_cosq_mapping_profile_set(
    int unit, 
    int cosq_profile, 
    bcm_cos_t priority, 
    bcm_cos_queue_t cosq);

/* Set/get TAF Cos mapping profile */
extern int bcm_tsn_taf_cosq_mapping_profile_get(
    int unit, 
    int cosq_profile, 
    bcm_cos_t priority, 
    bcm_cos_queue_t *cosq);

/* Create/destroy TAF Cos mapping profile */
extern int bcm_tsn_taf_cosq_mapping_profile_destroy(
    int unit, 
    int cosq_profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TSN traverse callback function for TAF Cos mapping profile */
typedef int (*bcm_tsn_taf_cosq_mapping_profile_traverse_cb)(
    int unit, 
    int cosq_profile, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse TAF Cos mapping profile */
extern int bcm_tsn_taf_cosq_mapping_profile_traverse(
    int unit, 
    bcm_tsn_taf_cosq_mapping_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TSN_H__ */
