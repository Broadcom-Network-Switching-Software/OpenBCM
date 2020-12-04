/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_MACSEC_H__
#define __BCM_MACSEC_H__

#if defined(INCLUDE_MACSEC)

#include <bcm/types.h>
#include <bmacsec.h>

typedef bmacsec_dev_addr_t bcm_macsec_dev_addr_t;

typedef enum bcm_macsec_packet_format_e {
    BCM_MACSEC_PACKET_FORMAT_ETH_II = BMACSEC_PACKET_FORMAT_ETH_II, 
    BCM_MACSEC_PACKET_FORMAT_802_3 = BMACSEC_PACKET_FORMAT_802_3, 
    BCM_MACSEC_PACKET_FORMAT_SNAP = BMACSEC_PACKET_FORMAT_SNAP, 
    BCM_MACSEC_PACKET_FORMAT_LLC = BMACSEC_PACKET_FORMAT_LLC, 
    BCM_MACSEC_PACKET_FORMAT__COUNT = BMACSEC_PACKET_FORMAT__COUNT 
} bcm_macsec_packet_format_t;

typedef enum bcm_macsec_dir_e {
    BCM_MACSEC_DIR_EGRESS = BMACSEC_DIR_EGRESS, 
    BCM_MACSEC_DIR_INGRESS = BMACSEC_DIR_INGRESS 
} bcm_macsec_dir_t;

typedef enum bcm_macsec_core_e {
    BCM_MACSEC_CORE_UNKNOWN = BMACSEC_CORE_UNKNOWN, 
    BCM_MACSEC_CORE_OCTAL_GIG = BMACSEC_CORE_OCTAL_GIG, 
    BCM_MACSEC_CORE_BCM5458X = BMACSEC_CORE_BCM5458X, 
    BCM_MACSEC_CORE_BCM5458X_B0 = BMACSEC_CORE_BCM5458X_B0, 
    BCM_MACSEC_CORE_BCM8729 = BMACSEC_CORE_BCM8729, 
    BCM_MACSEC_CORE_BCM8483X = BMACSEC_CORE_BCM8483X, 
    BCM_MACSEC_CORE_BCM8433X = BMACSEC_CORE_BCM8433X, 
    BCM_MACSEC_CORE_BCM8484X = BMACSEC_CORE_BCM8484X, 
    BCM_MACSEC_CORE_BCM5438X = BMACSEC_CORE_BCM5438X, 
    BCM_MACSEC_CORE_BCM84756 = BMACSEC_CORE_BCM84756, 
    BCM_MACSEC_CORE_BCM84756_C0 = BMACSEC_CORE_BCM84756_C0, 
    BCM_MACSEC_CORE_BCM53084 = BMACSEC_CORE_BCM53084, 
    BCM_MACSEC_CORE_BCM84749 = BMACSEC_CORE_BCM84749, 
    BCM_MACSEC_CORE_BCM53020 = BMACSEC_CORE_BCM53020 
} bcm_macsec_core_t;

typedef bmacsec_dev_io_f bcm_macsec_dev_io_t;

#define BCM_MACSEC_PORT_ENABLE  BMACSEC_PORT_ENABLE 

#define BCM_MACSEC_PORT_SINGLE_CHANNEL  BMACSEC_PORT_SINGLE_CHANNEL 

#define BCM_MACSEC_PORT_INGRESS_STORE_FORWARD BMACSEC_PORT_INGRESS_STORE_FORWARD 

#define BCM_MACSEC_PORT_INGRESS_SECTAG_MISSING_FORWARD BMACSEC_PORT_INGRESS_SECTAG_MISSING_FORWARD 

#define BCM_MACSEC_PORT_INGRESS_SECTAG_INVALID_FORWARD BMACSEC_PORT_INGRESS_SECTAG_INVALID_FORWARD 

#define BCM_MACSEC_PORT_INGRESS_SECTAG_PRESERVE BMACSEC_PORT_INGRESS_SECTAG_PRESERVE 

#define BCM_MACSEC_PORT_INGRESS_SECTAG_REPLACE_VLANID BMACSEC_PORT_INGRESS_SECTAG_REPLACE_VLANID 

#define BCM_MACSEC_PORT_INGRESS_SECTAG_REPLACE_PN BMACSEC_PORT_INGRESS_SECTAG_REPLACE_PN 

#define BCM_MACSEC_PORT_EGRESS_SECTAG_MISSING_UNCONTROLLED BMACSEC_PORT_EGRESS_SECTAG_MISSING_UNCONTROLLED 

#define BCM_MACSEC_PORT_EGRESS_ENCRYPT_DISABLE BMACSEC_PORT_EGRESS_ENCRYPT_DISABLE 

#define BCM_MACSEC_PORT_SECURE_ASSOC_4  BMACSEC_PORT_SECURE_ASSOC_4 

#define BCM_MACSEC_PORT_EGRESS_PN_MATCH_ENABLE BMACSEC_PORT_EGRESS_PN_MATCH_ENABLE 

typedef bmacsec_port_config_t bcm_macsec_port_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set MACSec configuration for the specified port. */
extern int bcm_macsec_port_config_set(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_port_config_t *cfg);

/* Returns the MACSec configuration for the specified port. */
extern int bcm_macsec_port_config_get(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_port_config_t *cfg);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_MACSEC_CAPABILITY_FORMAT_FLOW_MATCH BMACSEC_CAPABILITY_FORMAT_FLOW_MATCH 

#define BCM_MACSEC_CAPABILITY_PRE_EXPIRE_PN BMACSEC_CAPABILITY_PRE_EXPIRE_PN 

#define BCM_MACSEC_CAPABILITY_PN_HIGH_MATCH BMACSEC_CAPABILITY_PN_HIGH_MATCH 

typedef bmacsec_port_capability_t bcm_macsec_port_capability_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the port capability for the specified port. */
extern int bcm_macsec_port_capability_get(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_port_capability_t *cap);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  Prototype for Port interation callback function. */
typedef int (*bcm_macsec_port_traverse_cb)(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_core_t dev_core, 
    bcm_macsec_dev_addr_t dev_addr, 
    int dev_port, 
    bcm_macsec_dev_io_t devio_f, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * bcm_macsec_port_traverse iterates over all the MACSec capable ports in
 * the system and
 * calls user-provided callback function for each port.
 */
extern int bcm_macsec_port_traverse(
    int unit, 
    bcm_macsec_port_traverse_cb callbk, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_MACSEC_SECURE_WITH_ID   BMACSEC_SECURE_WITH_ID 

#define BCM_MACSEC_SECURE_REPLACE   BMACSEC_SECURE_REPLACE 

#define BCM_MACSEC_SECURE_CONFIDENTIAL  BMACSEC_SECURE_CONFIDENTIAL 

#define BCM_MACSEC_SECURE_SECTAG_TX_SCI BMACSEC_SECURE_SECTAG_TX_SCI 

#define BCM_MACSEC_SECURE_SECTAG_ES BMACSEC_SECURE_SECTAG_ES 

#define BCM_MACSEC_SECURE_SECTAG_SCB    BMACSEC_SECURE_SECTAG_SCB 

#define BCM_MACSEC_SECURE_CHAN_INGRESS  BMACSEC_SECURE_CHAN_INGRESS 

#define BCM_MACSEC_SECURE_AUTO_ACTIVATE BMACSEC_SECURE_AUTO_ACTIVATE 

#define BCM_MACSEC_SECURE_CHANNEL_MATCH_SCI BMACSEC_SECURE_CHANNEL_MATCH_SCI 

#define BCM_MACSEC_SECURE_CHANNEL_MATCH_MAC_DA BMACSEC_SECURE_CHANNEL_MATCH_MAC_DA 

#define BCM_MACSEC_SECURE_CHANNEL_MATCH_MAC_SA BMACSEC_SECURE_CHANNEL_MATCH_MAC_SA 

#define BCM_MACSEC_SECURE_CHANNEL_MATCH_ETHERTYPE BMACSEC_SECURE_CHANNEL_MATCH_ETHERTYPE 

#define BCM_MACSEC_SECURE_CHANNEL_MATCH_SECTAG_TCI BMACSEC_SECURE_CHANNEL_MATCH_SECTAG_TCI 

typedef bmacsec_secure_chan_t bcm_macsec_secure_chan_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a MACSec secure channel. */
extern int bcm_macsec_secure_chan_create(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_macsec_secure_chan_t *chan, 
    int *chanId);

/* 
 * Return MACSec secure channel information corresponding to channel
 * identified by chanId.
 */
extern int bcm_macsec_secure_chan_get(
    int unit, 
    bcm_port_t port, 
    int chanId, 
    bcm_macsec_secure_chan_t *chan);

/* Destroy MACSec secure channel corresponding to chanId. */
extern int bcm_macsec_secure_chan_destroy(
    int unit, 
    bcm_port_t port, 
    int chanId);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  MACSEC secure channel traverse callback prototype. */
typedef int (*bcm_macsec_chan_traverse_cb)(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_secure_chan_t *chan, 
    int chanId, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse over all the secure channels for the specified port. */
extern int bcm_macsec_secure_chan_traverse(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_chan_traverse_cb calback, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_macsec_crypto_e {
    BCM_MACSEC_CRYPTO_NONE = BMACSEC_CRYPTO_NONE, 
    BCM_MACSEC_CRYPTO_AES_128_GCM = BMACSEC_CRYPTO_AES_128_GCM 
} bcm_macsec_crypto_t;

#define BCM_MACSEC_CRYPTO_AES128GCM_KEY_SIZE BMACSEC_CRYPTO_AES128GCM_KEY_SIZE 

typedef bmacsec_crypto_aes128_gcm_t bcm_macsec_crypto_aes128_gcm_t;

typedef enum bcm_macsec_secure_assoc_num_e {
    BCM_MACSEC_SECURE_ASSOC_NUM_0 = BMACSEC_SECURE_ASSOC_NUM_0, 
    BCM_MACSEC_SECURE_ASSOC_NUM_1 = BMACSEC_SECURE_ASSOC_NUM_1, 
    BCM_MACSEC_SECURE_ASSOC_NUM_2 = BMACSEC_SECURE_ASSOC_NUM_2, 
    BCM_MACSEC_SECURE_ASSOC_NUM_3 = BMACSEC_SECURE_ASSOC_NUM_3, 
    BCM_MACSEC_SECURE_ASSOC_NUM__COUNT = BMACSEC_SECURE_ASSOC_NUM__COUNT 
} bcm_macsec_secure_assoc_num_t;

#define BCM_MACSEC_ASSOC_ACTIVE BMACSEC_ASSOC_ACTIVE 

#define BCM_MACSEC_ASSOC_ROLLOVER   BMACSEC_ASSOC_ROLLOVER 

#define BCM_MACSEC_ASSOC_ENCAP_REDIRECT BMACSEC_ASSOC_ENCAP_REDIRECT 

#define BCM_MACSEC_ASSOC_SET_NEXT_PN    BMACSEC_ASSOC_SET_NEXT_PN 

#define BCM_MACSEC_ASSOC_REPLAY_PROTECT BMACSEC_ASSOC_REPLAY_PROTECT 

typedef bmacsec_secure_assoc_t bcm_macsec_secure_assoc_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a MACSec Secure association for the specified secure channel. */
extern int bcm_macsec_secure_assoc_create(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int chanId, 
    bcm_macsec_secure_assoc_t *assoc, 
    int *assocId);

/* Return MACSec secure association for the specified assocId. */
extern int bcm_macsec_secure_assoc_get(
    int unit, 
    bcm_port_t port, 
    int assocId, 
    bcm_macsec_secure_assoc_t *assoc, 
    int *chanId);

/* Destroys MACSec secure association for the specified assocId. */
extern int bcm_macsec_secure_assoc_destroy(
    int unit, 
    bcm_port_t port, 
    int assocId);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  Secure assoc traverse callback prototype. */
typedef int (*bcm_macsec_secure_assoc_traverse_cb)(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_secure_assoc_t *chan, 
    int chanId, 
    int assocId, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all the secure associations and call the user-provided
 * callback function for each secure association.
 */
extern int bcm_macsec_secure_assoc_traverse(
    int unit, 
    bcm_port_t port, 
    int chanId, 
    bcm_macsec_secure_assoc_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef bmacsec_flow_match_t bcm_macsec_flow_match_t;

#define BCM_MACSEC_FLOW_MATCH_UNCONTROLLED  BMACSEC_FLOW_MATCH_UNCONTROLLED 

#define BCM_MACSEC_FLOW_MATCH_CONTROLLED    BMACSEC_FLOW_MATCH_CONTROLLED 

#define BCM_MACSEC_FLOW_MATCH_MACSA BMACSEC_FLOW_MATCH_MACSA 

#define BCM_MACSEC_FLOW_MATCH_MACDA BMACSEC_FLOW_MATCH_MACDA 

#define BCM_MACSEC_FLOW_MATCH_ETHTYPE   BMACSEC_FLOW_MATCH_ETHTYPE 

#define BCM_MACSEC_FLOW_MATCH_OUTER_TPID    BMACSEC_FLOW_MATCH_OUTER_TPID 

#define BCM_MACSEC_FLOW_MATCH_OUTER_VLAN    BMACSEC_FLOW_MATCH_OUTER_VLAN 

#define BCM_MACSEC_FLOW_MATCH_OUTER_PKT_PRIO BMACSEC_FLOW_MATCH_OUTER_PKT_PRIO 

#define BCM_MACSEC_FLOW_MATCH_OUTER_PKT_CFI BMACSEC_FLOW_MATCH_OUTER_PKT_CFI 

#define BCM_MACSEC_FLOW_MATCH_INNER_TPID    BMACSEC_FLOW_MATCH_INNER_TPID 

#define BCM_MACSEC_FLOW_MATCH_INNER_VLAN    BMACSEC_FLOW_MATCH_INNER_VLAN 

#define BCM_MACSEC_FLOW_MATCH_INNER_PKT_PRIO BMACSEC_FLOW_MATCH_INNER_PKT_PRIO 

#define BCM_MACSEC_FLOW_MATCH_INNER_PKT_CFI BMACSEC_FLOW_MATCH_INNER_PKT_CFI 

#define BCM_MACSEC_FLOW_MATCH_LLC_HEADER    BMACSEC_FLOW_MATCH_LLC_HEADER 

#define BCM_MACSEC_FLOW_MATCH_SNAP_HEADER   BMACSEC_FLOW_MATCH_SNAP_HEADER 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_C0 BMACSEC_FLOW_MATCH_SECTAG_C0 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_C1 BMACSEC_FLOW_MATCH_SECTAG_C1 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_E0 BMACSEC_FLOW_MATCH_SECTAG_E0 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_E1 BMACSEC_FLOW_MATCH_SECTAG_E1 

#define BCM_MACSEC_FLOW_MATCH_DATA  BMACSEC_FLOW_MATCH_DATA 

#define BCM_MACSEC_FLOW_MATCH_NOT_SHAREABLE BMACSEC_FLOW_MATCH_NOT_SHAREABLE 

#define BCM_MACSEC_FLOW_MATCH_ERROR_ASSOC_MISS BMACSEC_FLOW_MATCH_ERROR_ASSOC_MISS 

#define BCM_MACSEC_FLOW_MATCH_ERROR_CHANNEL_MISS BMACSEC_FLOW_MATCH_ERROR_CHANNEL_MISS 

#define BCM_MACSEC_FLOW_MATCH_ERROR_SECTAG  BMACSEC_FLOW_MATCH_ERROR_SECTAG 

#define BCM_MACSEC_FLOW_MATCH_ERROR_REPLAY  BMACSEC_FLOW_MATCH_ERROR_REPLAY 

#define BCM_MACSEC_FLOW_MATCH_SECURE_CHANNEL BMACSEC_FLOW_MATCH_SECURE_CHANNEL 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_SCI    BMACSEC_FLOW_MATCH_SECTAG_SCI 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_TCI    BMACSEC_FLOW_MATCH_SECTAG_TCI 

#define BCM_MACSEC_FLOW_MATCH_SECTAG_SCI_PRESET BMACSEC_FLOW_MATCH_SECTAG_SCI_PRESET 

typedef bmacsec_flow_action_t bcm_macsec_flow_action_t;

#define BCM_MACSEC_FLOW_ACTION_DROP BMACSEC_FLOW_ACTION_DROP 

#define BCM_MACSEC_FLOW_ACTION_UNCONTROLLED_FORWARD BMACSEC_FLOW_ACTION_UNCONTROLLED_FORWARD 

#define BCM_MACSEC_FLOW_ACTION_CONTROLLED_FORWARD BMACSEC_FLOW_ACTION_CONTROLLED_FORWARD 

#define BCM_MACSEC_FLOW_ACTION_SECTAG_E_VALUE_0 BMACSEC_FLOW_ACTION_SECTAG_E_VALUE_0 

#define BCM_MACSEC_FLOW_ACTION_SECTAG_E_VALUE_1 BMACSEC_FLOW_ACTION_SECTAG_E_VALUE_1 

#define BCM_MACSEC_FLOW_ACTION_SECTAG_C_VALUE_0 BMACSEC_FLOW_ACTION_SECTAG_C_VALUE_0 

#define BCM_MACSEC_FLOW_ACTION_SECTAG_C_VALUE_1 BMACSEC_FLOW_ACTION_SECTAG_C_VALUE_1 

#define BCM_MACSEC_FLOW_ACTION_SECTAG_V_VALUE_0 BMACSEC_FLOW_ACTION_SECTAG_V_VALUE_0 

#define BCM_MACSEC_FLOW_ACTION_SECTAG_V_VALUE_1 BMACSEC_FLOW_ACTION_SECTAG_V_VALUE_1 

#define BCM_MACSEC_FLOW_ACTION_MAP_SECTAG_CE_BITS BMACSEC_FLOW_ACTION_MAP_SECTAG_CE_BITS 

#define BCM_MACSEC_FLOW_ACTION_MAP_SPECIALHDR_CE_BITS BMACSEC_FLOW_ACTION_MAP_SPECIALHDR_CE_BITS 

#define BCM_MACSEC_FLOW_ACTION_MAP_SECTAG_SC_BITS BMACSEC_FLOW_ACTION_MAP_SECTAG_SC_BITS 

#define BCM_MACSEC_FLOW_ACTION_REMOVE_SPECIALHDR BMACSEC_FLOW_ACTION_REMOVE_SPECIALHDR 

#define BCM_MACSEC_FLOW_ACTION_SECURE_CHECK_FAIL_DROP BMACSEC_FLOW_ACTION_SECURE_CHECK_FAIL_DROP 

#define BCM_MACSEC_FLOW_ACTION_SECURE_CHECK_FAIL_FORWARD BMACSEC_FLOW_ACTION_SECURE_CHECK_FAIL_FORWARD 

#define BCM_MACSEC_FLOW_ACTION_ENCAP_REDIRECT BMACSEC_FLOW_ACTION_ENCAP_REDIRECT 

#define BCM_MACSEC_FLOW_ACTION_SECURE_CHECK_FAIL_REDIRECT BMACSEC_FLOW_ACTION_SECURE_CHECK_FAIL_REDIRECT 

#define BCM_MACSEC_FLOW_ACTION_ADD_SPECIALHDR BMACSEC_FLOW_ACTION_ADD_SPECIALHDR 

#define BCM_MACSEC_FLOW_ACTION_SET_HIT_COUNT BMACSEC_FLOW_ACTION_SET_HIT_COUNT 

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/update a MACSec flow object */
extern int bcm_macsec_flow_create(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_macsec_flow_match_t *flow, 
    bcm_macsec_flow_action_t *action, 
    int *flowId);

/* Destroy the MACSec flow object */
extern int bcm_macsec_flow_destroy(
    int unit, 
    bcm_port_t port, 
    int flowId);

/* Find and return the MACSec flow. */
extern int bcm_macsec_flow_get(
    int unit, 
    bcm_port_t port, 
    int flowId, 
    bcm_macsec_flow_match_t *flow, 
    bcm_macsec_flow_action_t *a);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  Flow traverse callback prototype. */
typedef int (*bcm_macsec_flow_traverse_cb)(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_flow_match_t *flow, 
    bcm_macsec_flow_action_t *action, 
    int flowId, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all flows for the specified port and call user-provided
 * callback function for each of the flows.
 */
extern int bcm_macsec_flow_traverse(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_flow_traverse_cb callbk, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_macsec_event_e {
    BCM_MACSEC_EVENT_PRE_KEY_EXPIRE = BMACSEC_EVENT_PRE_KEY_EXPIRE, 
    BCM_MACSEC_EVENT_KEY_EXPIRED = BMACSEC_EVENT_KEY_EXPIRED, 
    BCM_MACSEC_EVENT_KEY_RENEWED = BMACSEC_EVENT_KEY_RENEWED, 
    BCM_MACSEC_EVENT_HW_FAILURE = BMACSEC_EVENT_HW_FAILURE, 
    BCM_MACSEC_EVENT__COUNT = BMACSEC_EVENT__COUNT 
} bcm_macsec_event_t;

/*  Prototype for the event callback function. */
typedef int (*bcm_macsec_event_cb)(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_event_t event, 
    int chanId, 
    int assocId, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register application event handler function */
extern int bcm_macsec_event_register(
    int unit, 
    bcm_macsec_event_cb cb, 
    void *user_data);

/* Unregister the application event handler function. */
extern int bcm_macsec_event_unregister(
    int unit, 
    bcm_macsec_event_cb cb);

/* Enable or disable event notification for the specified event. */
extern int bcm_macsec_event_enable_set(
    int unit, 
    bcm_macsec_event_t t, 
    int enable);

/* Get current event notification status for the specified event. */
extern int bcm_macsec_event_enable_get(
    int unit, 
    bcm_macsec_event_t t, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef enum bcm_macsec_stat_e {
    bcm_rxBytes = rxBytes, 
    bcm_rxUnicastPkts = rxUnicastPkts, 
    bcm_rxMulticastPkts = rxMulticastPkts, 
    bcm_rxBroadcastPkts = rxBroadcastPkts, 
    bcm_rxDiscardPkts = rxDiscardPkts, 
    bcm_rxErrorPkts = rxErrorPkts, 
    bcm_rxUntagPkts = rxUntagPkts, 
    bcm_rxNotagPkts = rxNotagPkts, 
    bcm_rxBadtagPkts = rxBadtagPkts, 
    bcm_rxUnknownsciPkts = rxUnknownsciPkts, 
    bcm_rxNosciPkts = rxNosciPkts, 
    bcm_rxOvErrOverrunPkts = rxOvErrOverrunPkts, 
    bcm_rxSAUnusedPkts = rxSAUnusedPkts, 
    bcm_rxSANotusingPkts = rxSANotusingPkts, 
    bcm_rxSANotValidPkts = rxSANotValidPkts, 
    bcm_rxSAInvalidPkts = rxSAInvalidPkts, 
    bcm_rxSAOkPkts = rxSAOkPkts, 
    bcm_rxSCUnusedPkts = rxSCUnusedPkts, 
    bcm_rxNotusinSCPkts = rxNotusinSCPkts, 
    bcm_rxSCLatePkts = rxSCLatePkts, 
    bcm_rxSCNotValidPkts = rxSCNotValidPkts, 
    bcm_rxSCInvalidPkts = rxSCInvalidPkts, 
    bcm_rxSCDelayedPkts = rxSCDelayedPkts, 
    bcm_rxSCUncheckedPkts = rxSCUncheckedPkts, 
    bcm_rxSCOkPkts = rxSCOkPkts, 
    bcm_rxSCvalidtBytes = rxSCvalidtBytes, 
    bcm_rxSCDCryptoBytes = rxSCDCryptoBytes, 
    bcm_txControlledBytes = txControlledBytes, 
    bcm_txControlledUnicastPkts = txControlledUnicastPkts, 
    bcm_txControlledMulticastPkts = txControlledMulticastPkts, 
    bcm_txControlledBroadcastPkts = txControlledBroadcastPkts, 
    bcm_txControlledErrorPkts = txControlledErrorPkts, 
    bcm_txUncontrolledBytes = txUncontrolledBytes, 
    bcm_txUncontrolledUnicastPkts = txUncontrolledUnicastPkts, 
    bcm_txUncontrolledMulticastPkts = txUncontrolledMulticastPkts, 
    bcm_txUncontrolledBroadcastPkts = txUncontrolledBroadcastPkts, 
    bcm_txUncontrolledErrorPkts = txUncontrolledErrorPkts, 
    bcm_txUntagPkts = txUntagPkts, 
    bcm_txTooLongPkts = txTooLongPkts, 
    bcm_txSAProtectedPkts = txSAProtectedPkts, 
    bcm_txSACryptoPkts = txSACryptoPkts, 
    bcm_txSCProtectedPkts = txSCProtectedPkts, 
    bcm_txSCCryptoPkts = txSCCryptoPkts, 
    bcm_txSCProtectedBytes = txSCProtectedBytes, 
    bcm_txSCCryptoBytes = txSCCryptoBytes, 
    bcm_txSAavailablePkts = txSAavailablePkts, 
    bcm_rxSAavailablePkts = rxSAavailablePkts, 
    bcm_macsec_stat__count = bmacsec_stat__count 
} bcm_macsec_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Clear all the statistics for the specified port. */
extern int bcm_macsec_stat_clear(
    int unit, 
    bcm_port_t port);

/* Return the statistics for the specified statistic type. */
extern int bcm_macsec_stat_get(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_stat_t stat, 
    int chanId, 
    int assocId, 
    uint64 *val);

/* Return the Statistics for the specified statistic type. */
extern int bcm_macsec_stat_get32(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_stat_t stat, 
    int chanId, 
    int assocId, 
    uint32 *val);

/* Sets the value for specified statistic. */
extern int bcm_macsec_stat_set(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_stat_t stat, 
    int chanId, 
    int assocId, 
    uint64 val);

/* Sets the value for the specified statistic. */
extern int bcm_macsec_stat_set32(
    int unit, 
    bcm_port_t port, 
    bcm_macsec_stat_t stat, 
    int chanId, 
    int assocId, 
    uint32 val);

#endif /* defined(INCLUDE_MACSEC) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_MACSEC_H__ */
