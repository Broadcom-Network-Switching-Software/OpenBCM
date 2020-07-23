/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_SUBPORT_H__
#define __BCM_SUBPORT_H__

#include <bcm/types.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize/Cleanup the subport subsystem. */
extern int bcm_subport_init(
    int unit);

/* Initialize/Cleanup the subport subsystem. */
extern int bcm_subport_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Subport group create flags. */
#define BCM_SUBPORT_GROUP_WITH_ID           0x00000001 /* Add using the
                                                          specified ID. */
#define BCM_SUBPORT_GROUP_TYPE_LINKPHY      0x00000002 /* Add a LinkPHY subport
                                                          group. */
#define BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG  0x00000004 /* Add a 'subport tag'
                                                          (triple vlan tag)
                                                          subport group. */
#define BCM_SUBPORT_GROUP_TYPE_SUBPORT_PKTTAG 0x00000008 /* Add a 'LLTAG tag'
                                                          (triple vlan tag)
                                                          subport group. */
#define BCM_SUBPORT_GROUP_QUEUE             0x00000010 /* If set, use subport
                                                          group queues; Else,
                                                          use egress port
                                                          DiffServ queues. */
#define BCM_SUBPORT_GROUP_TYPE_GENERAL      0x00000020 /* Add a general purpose
                                                          subport group. */

/* Subport port property flags. */
#define BCM_SUBPORT_PROPERTY_PHB    0x00000001 /* Subport port PHB is valid.
                                                  Packet's 'internal priority'
                                                  and 'color' will be derived
                                                  from subport port lookup. */

#define BCM_SUBPORT_CONFIG_MAX_STREAMS  2          /* Max number of streams per
                                                      LinkPHY subport */

/* bcm_subport_group_config_t */
typedef struct bcm_subport_group_config_s {
    uint32 flags;           /* BCM_SUBPORT_GROUP_xxx. */
    bcm_gport_t port;       /* Physical GPORT Identifier */
    bcm_vlan_t vlan;        /* VLAN the group is attached to */
    int if_class;           /* Interface class ID */
    uint32 prop_flags;      /* Property flags for this subport group */
    bcm_gport_t queue_base; /* Subport group base queue */
    int qos_map_id;         /* QoS map identifier which defines priorities 1-1
                               mapping to queue offsets (0 ~ 7). */
} bcm_subport_group_config_t;

/* Initialize a subport group config structure. */
extern void bcm_subport_group_config_t_init(
    bcm_subport_group_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a subport group. */
extern int bcm_subport_group_create(
    int unit, 
    bcm_subport_group_config_t *config, 
    bcm_gport_t *group);

/* 
 * Destroy a subport group and all the subports that are part of the
 * group.
 */
extern int bcm_subport_group_destroy(
    int unit, 
    bcm_gport_t group);

/* Get a subport group. */
extern int bcm_subport_group_get(
    int unit, 
    bcm_gport_t group, 
    bcm_subport_group_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_subport_match_t */
typedef enum bcm_subport_match_e {
    BCM_SUBPORT_MATCH_INVALID = 0,      /* Illegal. */
    BCM_SUBPORT_MATCH_PKT_VID = 1,      /* Mod/port/trunk + LLTAG VID. */
    BCM_SUBPORT_MATCH_PKT_VID_INNER_VLAN = 2, /* Mod/port/trunk + LLTAG VID + inner
                                           VLAN. */
    BCM_SUBPORT_MATCH_PKT_VID_OUTER_VLAN = 3, /* Mod/port/trunk + LLTAG VID + outer
                                           VLAN. */
    BCM_SUBPORT_MATCH_COUNT = 4         /* Must be last. */
} bcm_subport_match_t;

/* bcm_subport_config_t */
typedef struct bcm_subport_config_s {
    bcm_gport_t group;                  /* Subport group */
    bcm_vlan_t pkt_vlan;                /* Packet 16-bit VLAN tag */
    bcm_vlan_t inner_vlan;              /* 12-bit inner VLAN */
    bcm_vlan_t outer_vlan;              /* 12-bit outerVLAN */
    int int_pri;                        /* Internal Priority for this subport
                                           port */
    uint32 prop_flags;                  /* Property flags for this subport port */
    uint16 stream_id_array[BCM_SUBPORT_CONFIG_MAX_STREAMS]; /* LinkPHY fragment header Stream ID
                                           array to be associated with the
                                           LinkPHY subport port. */
    int num_streams;                    /* Number of streams associated with the
                                           LinkPHY subport port. */
    bcm_color_t color;                  /* (internal) color or drop precedence
                                           for this subport port. */
    bcm_subport_match_t criteria;       /* match criteria. */
    bcm_gport_t subport_modport;        /* CoE - System port associated with the
                                           subport tag */
} bcm_subport_config_t;

/* Initialize a subport config structure. */
extern void bcm_subport_config_t_init(
    bcm_subport_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a subport to a subport group. */
extern int bcm_subport_port_add(
    int unit, 
    bcm_subport_config_t *config, 
    bcm_gport_t *port);

/* Delete a subport. */
extern int bcm_subport_port_delete(
    int unit, 
    bcm_gport_t port);

/* Get the subport configuration for the given gport identifier. */
extern int bcm_subport_port_get(
    int unit, 
    bcm_gport_t port, 
    bcm_subport_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_subport_port_traverse_cb */
typedef int (*bcm_subport_port_traverse_cb)(
    int unit, 
    bcm_gport_t port, 
    bcm_subport_config_t *config, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse the existing subports and call the user-supplied callback
 * function for each one.
 */
extern int bcm_subport_port_traverse(
    int unit, 
    bcm_subport_port_traverse_cb cb, 
    void *user_data);

/* 
 * Traverse the existing subports under a given subport group and call
 * the
 * user-supplied callback function for each one.
 */
extern int bcm_subport_group_traverse(
    int unit, 
    bcm_gport_t subport_group, 
    bcm_subport_port_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* LinkPHY configuration action mask */
#define BCM_SUBPORT_LINKPHY_BASE_STREAM_ID  0x00000001 
#define BCM_SUBPORT_LINKPHY_HEADER_MODE     0x00000002 
#define BCM_SUBPORT_LINKPHY_RX_CHECK_FLAGS  0x00000004 
#define BCM_SUBPORT_LINKPHY_RX_VLAN_TPID    0x00000008 
#define BCM_SUBPORT_LINKPHY_RX_NEAREND_MAC  0x00000010 
#define BCM_SUBPORT_LINKPHY_RX_FAREND_MAC   0x00000020 
#define BCM_SUBPORT_LINKPHY_RX_DFC_DEST_MAC 0x00000040 
#define BCM_SUBPORT_LINKPHY_RX_DFC_OPCODE   0x00000080 
#define BCM_SUBPORT_LINKPHY_RX_DFC_TIME     0x00000100 
#define BCM_SUBPORT_LINKPHY_RX_DFC_MAC_TYPE 0x00000200 
#define BCM_SUBPORT_LINKPHY_TX_VLAN_TPID    0x00000400 
#define BCM_SUBPORT_LINKPHY_TX_NEAREND_MAC  0x00000800 
#define BCM_SUBPORT_LINKPHY_TX_FAREND_MAC   0x00001000 
#define BCM_SUBPORT_LINKPHY_TX_PENULTIMATE_MODE 0x00002000 /* If this flag is
                                                          selected then TX
                                                          penultimate mode is
                                                          enabled. The
                                                          penultimate-fragment
                                                          of the frame would be
                                                          64-bytes(Payload-size)
                                                          to avoid padding in
                                                          the last-fragment. If
                                                          this flag is
                                                          deselected then TX
                                                          penultimate mode is
                                                          disabled. */
#define BCM_SUBPORT_LINKPHY_ALL             0xFFFFFFFF 

/* Check flags for received LinkPHY fragments */
#define BCM_SUBPORT_LINKPHY_RX_NEAREND_MAC_CHECK 0x00000001 /* enable/disable nearend
                                                          MAC compare */
#define BCM_SUBPORT_LINKPHY_RX_FAREND_MAC_CHECK 0x00000002 /* enable/disable far end
                                                          MAC compare */
#define BCM_SUBPORT_LINKPHY_RX_DFC_DEST_MAC_CHECK 0x00000004 /* enable/disable DFC
                                                          destination MAC
                                                          compare */
#define BCM_SUBPORT_LINKPHY_RX_DFC_OPCODE_CHECK 0x00000008 /* enable/disable DFC
                                                          opcode compare */
#define BCM_SUBPORT_LINKPHY_RX_DFC_TIME_CHECK 0x00000010 /* enable/disable DFC
                                                          time compare */

/* LinkPHY fragment header mode */
typedef enum bcm_subport_group_linkphy_header_mode_e {
    bcmSubportLinkphyHeaderModeEthAdapt = 0, /* Ethernet-Adaptation LinkPHY header
                                           mode */
    bcmSubportLinkphyHeaderModeTciLength = 1 /* TCI+Length LinkPHY header mode */
} bcm_subport_group_linkphy_header_mode_t;

/* Subport statistics */
typedef enum bcm_subport_stat_e {
    bcmSubportLinkPhyStatRxFragmentBytes = 0, /* LinkPHY subport Rx fragment bytes
                                           statistics */
    bcmSubportLinkPhyStatRxFragments = 1, /* LinkPHY subport Rx fragments
                                           statistics */
    bcmSubportLinkPhyStatRxFrameBytes = 2, /* LinkPHY subport Rx frame bytes
                                           statistics */
    bcmSubportLinkPhyStatRxFrames = 3,  /* LinkPHY subport Rx frames statistics */
    bcmSubportLinkPhyStatTxFragmentBytes = 4, /* LinkPHY subport Tx fragment bytes
                                           statistics */
    bcmSubportLinkPhyStatTxFragments = 5, /* LinkPHY subport Tx fragments
                                           statistics */
    bcmSubportLinkPhyStatTxFrameBytes = 6, /* LinkPHY subport Tx frame bytes
                                           statistics */
    bcmSubportLinkPhyStatTxFrames = 7   /* LinkPHY subport Tx frames statistics */
} bcm_subport_stat_t;

/* List of Linkphy RX errors */
typedef enum bcm_subport_linkphy_rx_error_e {
    bcmSubportLinkphyRxErrorMaxFrameSize = 0, /* Linkphy RX error due to Frame with
                                           max frame size */
    bcmSubportLinkphyRxErrorRuntFrame = 1, /* Linkphy RX error due to Runt frame */
    bcmSubportLinkphyRxErrorDfcLength = 2, /* Linkphy RX error due to a DFC Frame
                                           with unacceptable length */
    bcmSubportLinkphyRxErrorDfcDropPortPurge = 3, /* Linkphy RX error due to a DFC Frame
                                           drop due to port purge */
    bcmSubportLinkphyRxErrorDfcTime = 4, /* Linkphy RX error due to a DFC Frame
                                           with Time error */
    bcmSubportLinkphyRxErrorDfcOpcode = 5, /* Linkphy RX error due to a DFC Frame
                                           with Opcode error */
    bcmSubportLinkphyRxErrorLengthZero = 6, /* Linkphy RX error due to Fragment with
                                           header length zero */
    bcmSubportLinkphyRxErrorNonEofPaddedFragment = 7, /* Linkphy RX error due to Frame with
                                           non-EOF padded fragment */
    bcmSubportLinkphyRxErrorHdrLengthGtFragmentLength = 8, /* Linkphy RX error due to Fragment with
                                           header length greater than actual
                                           fragment length */
    bcmSubportLinkphyRxErrorFraming = 9, /* Linkphy RX error due to Frame with
                                           SOF/EOF/MOF errors */
    bcmSubportLinkphyRxErrorFrameCrc = 10, /* Linkphy RX error due to Frame with
                                           CRC error. NA for DFC frame */
    bcmSubportLinkphyRxErrorStreamIdUnexpected = 11, /* Linkphy RX error due to Fragment with
                                           unexpected streamId */
    bcmSubportLinkphyRxErrorMacSA = 12, /* Linkphy RX error due to Data fragment
                                           or DFC frame that does not match
                                           configured MAC SA */
    bcmSubportLinkphyRxErrorMacDA = 13, /* Linkphy RX error due to Data fragment
                                           or DFC frame that does not match
                                           configured MAC DA */
    bcmSubportLinkphyRxErrorEthertype = 14, /* Linkphy RX error due to Data fragment
                                           or DFC frame that does not match
                                           configured data TPID or DFC EtherType
                                           respectively */
    bcmSubportLinkphyRxErrorCount = 15  /* Not an usable value. Always last */
} bcm_subport_linkphy_rx_error_t;

/* Bitmap of Linkphy RX errors */
typedef struct bcm_subport_linkphy_rx_errors_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(bcmSubportLinkphyRxErrorCount)]; 
} bcm_subport_linkphy_rx_errors_t;

/* Macros to manipulate the bitmap of Linkphy RX errors */
#define BCM_SUBPORT_LINKPHY_RX_ERROR_SET(_error_types, _error_type)  SHR_BITSET(((_error_types).w), (_error_type)) 
#define BCM_SUBPORT_LINKPHY_RX_ERROR_GET(_error_types, _error_type)  SHR_BITGET(((_error_types).w), (_error_type)) 
#define BCM_SUBPORT_LINKPHY_RX_ERROR_CLEAR(_error_types, _error_type)  SHR_BITCLR(((_error_types).w), (_error_type)) 
#define BCM_SUBPORT_LINKPHY_RX_ERROR_SET_ALL(_error_types)  SHR_BITSET_RANGE(((_error_types).w), 0, bcmSubportLinkphyRxErrorCount) 
#define BCM_SUBPORT_LINKPHY_RX_ERROR_CLEAR_ALL(_error_types)  SHR_BITCLR_RANGE(((_error_types).w), 0, bcmSubportLinkphyRxErrorCount) 

/* LinkPHY configuration parameters per port. */
typedef struct bcm_subport_group_linkphy_config_s {
    uint32 action_mask;                 /* BCM_SUBPORT_LINKPHY_xxx flags */
    uint16 base_stream_id;              /* External Stream ID base */
    bcm_subport_group_linkphy_header_mode_t header_mode; /* LinkPHY fragment header mode */
    uint32 rx_check_flags;              /* flags
                                           BCM_SUBPORT_LINKPHY_RX_XXX_CHECK for
                                           comparing RXed fragment header
                                           parameters */
    uint16 rx_vlan_tpid;                /* Expected Vlan TPID in header of
                                           LinkPHY fragments RXed */
    bcm_mac_t rx_nearend_mac;           /* Expected near end MAC address in
                                           header of LinkPHY fragments RXed */
    bcm_mac_t rx_farend_mac;            /* Expected far end MAC address in
                                           header of LinkPHY fragments RXed */
    bcm_mac_t rx_dfc_dest_mac;          /* Expected Destination MAC address in
                                           header of DFC fragments RXed */
    uint16 rx_dfc_opcode;               /* Expected opcode in header of DFC
                                           fragments RXed */
    uint16 rx_dfc_time;                 /* Expected TIME in header of DFC
                                           fragments RXed */
    uint16 rx_dfc_mac_type;             /* Expected EtherType in header of DFC
                                           fragments RXed */
    uint16 tx_vlan_tpid;                /* Vlan TPID to be set in header of
                                           LinkPHY fragments TXed */
    bcm_mac_t tx_nearend_mac;           /* Near end MAC address to be set in
                                           header of LinkPHY fragments TXed */
    bcm_mac_t tx_farend_mac;            /* Far end MAC address to be set in
                                           header of LinkPHY fragments TXed */
} bcm_subport_group_linkphy_config_t;

/* The maximum size of Linkphy RX fragment */
#define BCM_SUBPORT_LINKPHY_MAX_FRAGMENT_SIZE 192        

/* Data related to the fragment with the error */
typedef struct bcm_subport_linkphy_rx_error_data_s {
    bcm_port_t port;                    /* Physical port on which the errored
                                           fragment was received */
    uint16 ext_stream_id;               /* The external stream id in the errored
                                           fragment */
    uint8 fragment_data[BCM_SUBPORT_LINKPHY_MAX_FRAGMENT_SIZE]; /* The entire fragment data */
} bcm_subport_linkphy_rx_error_data_t;

/* Initialize a LinkPHY port config structure. */
extern void bcm_subport_group_linkphy_config_t_init(
    bcm_subport_group_linkphy_config_t *linkphy_config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set LinkPHY configuration parameters for a given port. */
extern int bcm_subport_group_linkphy_config_set(
    int unit, 
    bcm_gport_t port, 
    bcm_subport_group_linkphy_config_t *linkphy_config);

/* Get LinkPHY configuration parameters for a given port. */
extern int bcm_subport_group_linkphy_config_get(
    int unit, 
    bcm_gport_t port, 
    bcm_subport_group_linkphy_config_t *linkphy_config);

/* Set the statistics value for a given subport and statistics type. */
extern int bcm_subport_port_stat_set(
    int unit, 
    bcm_gport_t port, 
    int stream_id, 
    bcm_subport_stat_t stat_type, 
    uint64 val);

/* Get the statistics value for a given subport and statistics type. */
extern int bcm_subport_port_stat_get(
    int unit, 
    bcm_gport_t port, 
    int stream_id, 
    bcm_subport_stat_t stat_type, 
    uint64 *val);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * BCM_SUBPORT_LINKPHY_RX_ERROR_CB_XXX flags - Used in callback functions
 * for Linkphy RX errors
 */
#define BCM_SUBPORT_LINKPHY_RX_ERROR_CB_DATA_VALID 0x1        /* Indicates that the
                                                          callback data is valid */

/* Callback function for a given Linkphy RX error type */
typedef int (*bcm_subport_linkphy_rx_error_callback)(
    int unit, 
    bcm_subport_linkphy_rx_error_t error_type, 
    uint32 flags, 
    bcm_subport_linkphy_rx_error_data_t *error_data, 
    void *user_data);

/* Information structure passed to Linkphy RX error registration */
typedef struct bcm_subport_linkphy_rx_error_reg_info_s {
    bcm_subport_linkphy_rx_error_callback cb; /* Callback to be registered */
    int data_log_enable;                /* To enable/disable data logging for
                                           given set of errors */
} bcm_subport_linkphy_rx_error_reg_info_t;

/* Initialize the bcm_subport_linkphy_rx_error_reg_info_t structure */
extern void bcm_subport_linkphy_rx_error_reg_info_t_init(
    bcm_subport_linkphy_rx_error_reg_info_t *reg_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register a callback function for a Linkphy RX error. */
extern int bcm_subport_linkphy_rx_error_register(
    int unit, 
    bcm_subport_linkphy_rx_errors_t errors, 
    bcm_subport_linkphy_rx_error_reg_info_t *reg_info, 
    void *user_data);

/* De-Register a callback function for a Linkphy RX error. */
extern int bcm_subport_linkphy_rx_error_unregister(
    int unit, 
    bcm_subport_linkphy_rx_errors_t errors);

/* 
 * Given a subport gport, returns the corresponding module and port
 * associated with that subport.
 * This is only applicable on devices BCM5645x, BCM5626x, BCM5646x,
 * BCM5686x, BCM5656x and
 * BCM5676x that support CoE applications like Subtag and LinkPhy.
 */
extern int bcm_subport_gport_modport_get(
    int unit, 
    bcm_gport_t subport_gport, 
    bcm_module_t *module, 
    bcm_port_t *port);

#endif /* BCM_HIDE_DISPATCHABLE */

/* subport port group identifier. */
typedef uint32 bcm_subport_port_group_t;

#define BCM_SUBPORT_PORT_GROUP_WITH_ID  (1 << 0)   /* Port group is created with
                                                      id. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create port group id. */
extern int bcm_subport_port_group_id_create(
    int unit, 
    uint32 options, 
    bcm_subport_port_group_t *port_group_id);

/* Destroy port group id. */
extern int bcm_subport_port_group_id_destroy(
    int unit, 
    bcm_subport_port_group_t port_group_id);

/* Get port group id. */
extern int bcm_subport_port_group_id_get(
    int unit, 
    bcm_subport_port_group_t port_group_id);

/* 
 * Destroy all the ports groups which do not have subports attached to
 * them.
 */
extern int bcm_subport_port_group_id_destroy_all(
    int unit);

/* 
 * Get all the port group ids created in the system. User will have to
 * call this API first with max_size = 0 to get the exact number of
 * elements. array_size will return the actual number of ids present in
 * the system. In the subsequent call, user can allocate memory and ask
 * for less/equal number of ids than array_size.
 */
extern int bcm_subport_port_group_id_get_all(
    int unit, 
    int max_size, 
    bcm_subport_port_group_t *port_group_id_array, 
    int *array_size);

/* Set multiple subports gports in port group. */
extern int bcm_subport_port_group_subport_multi_set(
    int unit, 
    bcm_subport_port_group_t port_group_id, 
    int num_subports, 
    bcm_gport_t *subport_gports);

/* 
 * Get all the subports added in this port group. User will have to call
 * this API first with max_subports = 0 to get the exact number of
 * elements. num_subports will return the actual number of subports
 * present in this port group. In the subsequent call, user can allocate
 * memory and ask for less/equal number of subports than num_subports.
 */
extern int bcm_subport_port_group_subport_multi_get(
    int unit, 
    bcm_subport_port_group_t port_group_id, 
    int max_subports, 
    bcm_gport_t *subport_gports, 
    int *num_subports);

/* Create subport profile id for l3 tunnel allowed subports. */
extern int bcm_subport_tunnel_pbmp_profile_create(
    int unit, 
    int num_subports, 
    bcm_gport_t *subport_gport, 
    int *subport_pbmp_profile_id);

/* Destoy subport profile. */
extern int bcm_subport_tunnel_pbmp_profile_destroy(
    int unit, 
    int subport_pbmp_profile_id);

/* Get subport profile for l3 tunnel allowed subports. */
extern int bcm_subport_tunnel_pbmp_profile_get(
    int unit, 
    int num_subports, 
    bcm_gport_t *subport_gport, 
    int *subport_pbmp_profile_id);

/* 
 * Destroy all the subport pbmp profiles which are not linked to any l3
 * tunnel.
 */
extern int bcm_subport_tunnel_pbmp_profile_destroy_all(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_SUBPORT_H__ */
