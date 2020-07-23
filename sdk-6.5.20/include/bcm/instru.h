/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_INSTRU_H__
#define __BCM_INSTRU_H__

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/pp.h>

/* Node type of IPT profile */
typedef enum bcm_instru_ipt_node_type_e {
    bcmInstruIptNodeTypeTail = 0,       /* Tail */
    bcmInstruIptNodeTypeIntOverVxlanFirst = 1, /* Vxlan first node */
    bcmInstruIptNodeTypeIntOverVxlanIntermediate = 2, /* Vxlan intermediate node */
    bcmInstruIptNodeTypeIntOverVxlanLast = 3 /* Vxlan last node */
} bcm_instru_ipt_node_type_t;

/* Control of various Instrumentation features */
typedef enum bcm_instru_control_e {
    bcmInstruControlIptSwitchId = 0,    /* IPT switch ID */
    bcmInstruControlVisMode = 1,        /* Visibility Mode */
    bcmInstruControlVisSampling = 2,    /* Visibility sampling parameters in
                                           Selective mode */
    bcmInstruControlSFlowSamplingRate = 3, /* Global sampling rate for sFlow
                                           datagrams */
    bcmInstruControlSFlowAgentIPAddress = 4, /* Global agent IP address for sFlow
                                           datagrams */
    bcmInstruControlSFlowUpTime = 5,    /* Time stamp that will be stamped on
                                           sFlow packets. Should be updated
                                           periodically. */
    bcmInstruControlSFlowMyRouterASNumber = 6 /* Router/switch (my node) AS number. */
} bcm_instru_control_t;

/* 
 * Supported visibility modes, used in arg when type is 
 * bcmInstruControlVisMode
 */
typedef enum bcm_instru_vis_mode_control_e {
    bcmInstruVisModeNone = 0,       /* No visibility enabled */
    bcmInstruVisModeAlways = 1,     /* Visibility is enabled for any packet from
                                       any port */
    bcmInstruVisModeSelective = 2   /* Selective visibility enabled, user port
                                       control to enable specific resources */
} bcm_instru_vis_mode_control_t;

/* Control of various per-port Instrumentation features */
typedef enum bcm_instru_gport_control_e {
    bcmInstruGportControlTraceProbability = 0, /* Probability in one tenth percentage */
    bcmInstruGportControlVisEnable = 1, /* Enable visibility on per port basis */
    bcmInstruGportControlVisForce = 2,  /* Force visibility on per port basis */
    bcmInstruGportControlIptTrapToRcyEnable = 3, /* Trap IPT packet to Recycle interface */
    bcmInstruGportControlIfaInitiatorRcyEnable = 4 /* Set IFA Initiator recycle enable. */
} bcm_instru_gport_control_t;

/* IPT first node header values */
typedef struct bcm_instru_ipt_first_node_header_s {
    uint32 options; /* Options field */
    uint32 type;    /* Type field */
    uint32 length;  /* IPT header length length in bytes */
} bcm_instru_ipt_first_node_header_t;

/* IPT info */
typedef struct bcm_instru_ipt_s {
    bcm_instru_ipt_node_type_t node_type; /* Node type */
    uint32 metadata_flags;              /* Metadata flags */
    bcm_instru_ipt_first_node_header_t first_node_header; /* First header info */
} bcm_instru_ipt_t;

/* Flags controlling global visibiliy behavior */
#define BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS 0x1        /* Apply control to
                                                          INGRESS direction */
#define BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS 0x2        /* Apply control to
                                                          EGRESS direction */

/* Flags controlling signal get behavior */
#define BCM_INSTRU_VIS_SIGNAL_FLAG_RETURN_ON_1ST_ERROR 0x1        /* Values for all signals
                                                          in the key list should
                                                          be successfully
                                                          obtained */

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get visibility mode */
extern int bcm_instru_control_set(
    int unit, 
    uint32 flags, 
    bcm_instru_control_t type, 
    int arg);

/* Set/Get visibility mode */
extern int bcm_instru_control_get(
    int unit, 
    uint32 flags, 
    bcm_instru_control_t type, 
    int *arg);

/* Set/Get visibility mode */
extern int bcm_instru_gport_control_set(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_instru_gport_control_t type, 
    int arg);

/* Set/Get visibility mode */
extern int bcm_instru_gport_control_get(
    int unit, 
    bcm_gport_t gport, 
    uint32 flags, 
    bcm_instru_gport_control_t type, 
    int *arg);

/* Get various instrumentation features at the port level. */
extern int bcm_instru_ipt_profile_set(
    int unit, 
    uint32 flags, 
    int ipt_profile, 
    bcm_instru_ipt_t *config);

/* Get various instrumentation features at the port level. */
extern int bcm_instru_ipt_profile_get(
    int unit, 
    uint32 flags, 
    int ipt_profile, 
    bcm_instru_ipt_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize bcm_instru_ipt_t struct. */
extern void bcm_instru_ipt_t_init(
    bcm_instru_ipt_t *config);

/* IPT metadata options */
#define BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD 0x1        /* INGRESS TOD */
#define BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT 0x2        /* INGRESS PORT */
#define BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID 0x4        /* INGRESS DEVICE ID */
#define BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED 0x8        /* INGRESS RESERVED */
#define BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD 0x10       /* EGRESS TOD */
#define BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT 0x20       /* EGRESS PORT */
#define BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID 0x40       /* EGRESS DEVICE ID */
#define BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID 0x80       /* SWITCH ID */
#define BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED 0x100      /* EGRESS RESERVED */
#define BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO 0x200      /* QUEUE INFO */
#define BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_FG 0x400      /* Queue Level Fine
                                                          Granularity */
#define BCM_INSTRU_IPT_METADATA_FLAG_LATENCY 0x800      /* Latency in nanoseconds */
#define BCM_INSTRU_IPT_METADATA_FLAG_PORT_RATE 0x1000     /* NIF Port rate */
#define BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_CG 0x2000     /* Queue Level Coarse
                                                          Granularity */

/* sFlow encap flags. */
#define BCM_INSTRU_SFLOW_ENCAP_WITH_ID      (1 << 0)   /* Allocate with sflow
                                                          encap id. */
#define BCM_INSTRU_SFLOW_ENCAP_REPLACE      (1 << 1)   /* Replace existing
                                                          entry, update
                                                          EEDB_SFLOW,
                                                          EGRESS_OAM_LIF_DB
                                                          tables. */
#define BCM_INSTRU_SFLOW_ENCAP_AGGREGATED   (1 << 2)   /* Sflow samples will be
                                                          aggregated. */
#define BCM_INSTRU_SFLOW_ENCAP_EXTENDED_INITIATOR (1 << 3)   /* Sflow Extended Gateway
                                                          mode (Sflow IPoETH
                                                          sampling) initator. */
#define BCM_INSTRU_SFLOW_ENCAP_EXTENDED_FLOW (1 << 4)   /* Sflow Extended Gateway
                                                          mode (Sflow IPoETH
                                                          sampling) per flow. */
#define BCM_INSTRU_SFLOW_ENCAP_EXTENDED_IPV6 (1 << 5)   /* Sflow Extended Gateway
                                                          mode (Sflow IPoETH
                                                          sampling) IPv6 mode. */
#define BCM_INSTRU_SFLOW_ENCAP_EXTENDED_NOF_DSTS 4          /* MAX number of
                                                          Autonomous system path
                                                          to the destination. */

/* SFlow EEDB instrumentation info */
typedef struct bcm_instru_sflow_encap_info_s {
    uint32 flags;           /* See BCM_INSTRU_SFLOW_ENCAP_XXX flags definitions. */
    int sflow_encap_id;     /* Global lif */
    bcm_gport_t tunnel_id;  /* next eedb pointer - udp tunnel id */
    int stat_cmd;           /* Statistic command - request sequence number */
    int counter_command_id; /* Command id (interface id) */
    int sub_agent_id;       /* sub_agent_id for this sFlow encap */
    uint16 eventor_id;      /* Eventor RX context to use for this flow */
} bcm_instru_sflow_encap_info_t;

/* Sflow Sample Interface info */
typedef struct bcm_instru_sflow_sample_interface_info_s {
    uint32 flags;       /* See BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_XXX flags
                           definitions. */
    bcm_gport_t port;   /* Local or System port */
    int interface;      /* Interface the packet received/sent on */
} bcm_instru_sflow_sample_interface_info_t;

/* Sflow Sample Interface Traverse info */
typedef struct bcm_instru_sflow_sample_interface_traverse_info_s {
    uint32 flags;   /* See BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_XXX flags
                       definitions. */
} bcm_instru_sflow_sample_interface_traverse_info_t;

/* Sflow Sample Interface callback function prototype */
typedef int (*bcm_instru_sflow_sample_traverse_cb)(
    int unit, 
    bcm_instru_sflow_sample_interface_info_t *sample_interface_info, 
    void *user_data);

/* Vswitch callback function prototype */
typedef int (*bcm_instru_sflow_encap_traverse_cb)(
    int unit, 
    bcm_instru_sflow_encap_info_t *sflow_encap_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create/Get/Delete/Traverse sFlow etps types */
extern int bcm_instru_sflow_encap_create(
    int unit, 
    bcm_instru_sflow_encap_info_t *sflow_encap_info);

/* Create/Get/Delete/Traverse sFlow etps types */
extern int bcm_instru_sflow_encap_get(
    int unit, 
    bcm_instru_sflow_encap_info_t *sflow_encap_info);

/* Create/Get/Delete/Traverse sFlow etps types */
extern int bcm_instru_sflow_encap_delete(
    int unit, 
    bcm_instru_sflow_encap_info_t *sflow_encap_info);

/* Create/Get/Delete/Traverse sFlow etps types */
extern int bcm_instru_sflow_encap_traverse(
    int unit, 
    bcm_instru_sflow_encap_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Eventor context ID. */
typedef uint16 bcm_eventor_context_id_t;

/* Eventor event ID. */
typedef uint16 bcm_eventor_event_id_t;

/* Eventor engine ID. */
typedef uint16 bcm_eventor_builder_id_t;

/* Number of Eventor srum bank. */
typedef uint16 bcm_eventor_sram_bank_t;

/* Eventor context type */
typedef enum bcm_eventor_context_type_e {
    bcmEventorContextTypeRx = 0,        /* RX Eventor context type */
    bcmEventorContextTypeTx = 1,        /* TX Eventor context type */
    bcmEventorContextTypeTxFifo = 2,    /* FIFO (TX) Eventor context type */
    bcmEventorContextTypeAny = 255      /* A value used to get any Eventor
                                           context type in a get operation. Not
                                           a real type */
} bcm_eventor_context_type_t;

/* instru access info. */
typedef struct bcm_instru_access_info_s {
    uint32 flags;       /* is register/memory, is broadcast */
    uint32 address;     /* address/offset inside the block */
    uint16 phys_block;  /* CMIC block ID */
    uint16 nof_entries; /* number of consecutive entries to read/write */
    uint16 entry_size;  /* size of register/memory entry in 32 bit words */
} bcm_instru_access_info_t;

/* Configuration for events with a specific event ID. */
typedef struct bcm_instru_eventor_event_id_config_s {
    uint32 flags;                       /* event config flags */
    bcm_eventor_context_id_t context;   /* Context to use for Tx events with
                                           this ID. */
} bcm_instru_eventor_event_id_config_t;

/* Operations in the DMA program filling the buffers. */
typedef struct bcm_instru_eventor_operation_s {
    uint32 flags;                       /* control flags: TX/additional buffer */
    uint32 buf_offset;                  /* offset inside the buffer in 4 byte
                                           words */
    bcm_instru_access_info_t access_info; /* memory,register access information */
} bcm_instru_eventor_operation_t;

/* Constant value to be used in Eventor output. */
typedef struct bcm_instru_eventor_const_value_s {
    uint32 value;   /* 4 byte words constant value to be used in Eventor output */
    uint32 offset;  /* output offset for the constant value in 4 byte words */
} bcm_instru_eventor_const_value_t;

#define BCM_INSTRU_EVENTOR_TIMEOUT_NONE     ((uint32)-1) /* No timeout */
#define BCM_INSTRU_EVENTOR_TIMEOUT_MINIMAL  0          /* smallest timeout
                                                          possible */
#define BCM_INSTRU_ACCESS_FLAG_IS_WRITE     0x1        /*  is a write operation */
#define BCM_INSTRU_ACCESS_FLAG_IS_MEM       0x2        /*  is a memory (and not
                                                          a register) access */

/* Eventor context configuration */
typedef struct bcm_instru_eventor_context_conf_s {
    uint32 flags;                       /* control flags: Was there enough room
                                           to return the full DMA program and
                                           all constant values , auto select
                                           bank/buffer for RX , event type
                                           (RX/FIFO/TX) */
    bcm_eventor_sram_bank_t bank1;      /* SRAM bank used for first buffer */
    bcm_eventor_sram_bank_t bank2;      /* SRAM bank used for second buffer */
    uint32 buffer_size;                 /* buffer size in 4 byte words used for
                                           both buffers */
    uint32 buffer1_start;               /* offset in 4 byte words in the SRAM
                                           bank of the first buffer */
    uint32 buffer2_start;               /* offset in 4 byte words in the SRAM
                                           bank of the second buffer */
    uint32 program1_start;              /* offset in 4 byte words in the SRAM
                                           bank of the first buffer's DMA
                                           program filling the buffer */
    uint32 program2_start;              /* offset in 4 byte words in the SRAM
                                           bank of the second buffer's DMA
                                           program filling the buffer */
    uint32 nof_operations;              /* number operations in the DMA program
                                           filling the buffers */
    bcm_instru_eventor_operation_t *operations; /* The operations in the DMA program
                                           filling the buffers */
    uint32 nof_const_values;            /* number constant values to be used in
                                           Eventor output */
    bcm_instru_eventor_const_value_t *const_values; /* Constant value to be used in Eventor
                                           output */
    uint32 builders;                    /* Context used Builders bitmap */
} bcm_instru_eventor_context_conf_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the configuration of the given context */
extern int bcm_instru_eventor_context_set(
    int unit, 
    uint32 flags, 
    bcm_eventor_context_id_t context, 
    bcm_eventor_context_type_t context_type, 
    bcm_instru_eventor_context_conf_t *conf);

/* Get the configuration of the given context */
extern int bcm_instru_eventor_context_get(
    int unit, 
    uint32 flags, 
    bcm_eventor_context_id_t context, 
    bcm_eventor_context_type_t context_type, 
    bcm_instru_eventor_context_conf_t *out_conf);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Eventor builder configuration */
typedef struct bcm_instru_eventor_builder_conf_s {
    uint32 flags;           /* control flags */
    uint32 thresh_size;     /* When this amount of 4 byte words is reached,
                               generate a packet */
    uint32 thresh_time;     /* When this amount of ms passes since stated
                               receiving data, generate a packet. May take
                               BCM_INSTRU_EVENTOR_TIMEOUT_* */
    uint32 header_length;   /* The length, in bytes, of the header to use in
                               generated packets */
    uint8 *header_data;     /* The data of the header to use in generated
                               packets */
} bcm_instru_eventor_builder_conf_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the configuration of the given builder. */
extern int bcm_instru_eventor_builder_set(
    int unit, 
    uint32 flags, 
    bcm_eventor_builder_id_t builder, 
    bcm_instru_eventor_builder_conf_t *conf);

/* Get the configuration of the given builder. */
extern int bcm_instru_eventor_builder_get(
    int unit, 
    uint32 flags, 
    bcm_eventor_builder_id_t builder, 
    bcm_instru_eventor_builder_conf_t *out_conf);

/* get the low level access information for a memory */
extern int bcm_instru_get_memory_access_info(
    int unit, 
    uint32 flags, 
    int mem, 
    uint32 copyno, 
    uint32 index, 
    uint32 array_index, 
    bcm_instru_access_info_t *access_info);

/* get the low level access information for a register */
extern int bcm_instru_get_register_access_info(
    int unit, 
    uint32 flags, 
    int reg, 
    uint32 instance, 
    uint32 array_index, 
    bcm_instru_access_info_t *access_info);

/* Start or stop the Eventor. */
extern int bcm_instru_eventor_active_set(
    int unit, 
    uint32 flags, 
    int active);

/* Check id the Eventor is active/started. */
extern int bcm_instru_eventor_active_get(
    int unit, 
    uint32 flags, 
    int *out_active);

/* Clean signal cache and allow signals for new packet to be recorded */
extern int bcm_instru_vis_resume(
    int unit, 
    bcm_core_t core_id, 
    uint32 flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Maximum Sizes for different objects used by signal_get API */
#define BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE SHR_PP_SIGNAL_NAME_MAX_SIZE /* Maximum size for
                                                          signal name */
#define BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE  SHR_PP_STAGE_NAME_MAX_SIZE /* Maximum size for
                                                          signal name */
#define BCM_INSTRU_VIS_SIGNAL_VALUE_MAX_SIZE_UINT32 SHR_PP_SIGNAL_VALUE_MAX_SIZE_UINT32 /* Maximum size for
                                                          signal value */
#define BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE  SHR_PP_BLOCK_NAME_MAX_SIZE /* Maximum size for
                                                          signal name */
#define BCM_INSTRU_VIS_SIGNAL_PRINT_VALUE_MAX_SIZE_STR SHR_PP_SIGNAL_PRINT_VALUE_MAX_SIZE_STR /* Maximum size for
                                                          signal print value */

/* Sflow Sample Interface flags. */
#define BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_REPLACE 1          /* Replace existing
                                                          mapping. */
#define BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT 2          /* The Interface is INPUT */
#define BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT 4          /* The Interface is
                                                          OUTPUT */
#define BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_DELETE_ALL 8          /* Delete all interface
                                                          mapping */

/* Result of signal fetching */
typedef struct bcm_instru_vis_signal_result_s {
    uint32 value[BCM_INSTRU_VIS_SIGNAL_VALUE_MAX_SIZE_UINT32]; /* Signal value */
    char print_value[BCM_INSTRU_VIS_SIGNAL_PRINT_VALUE_MAX_SIZE_STR]; /* Signal print value */
    int size;                           /* Signal size in bits */
    bcm_error_t status;                 /* Signal get return status */
} bcm_instru_vis_signal_result_t;

/* Result of signal fetching */
typedef struct bcm_instru_vis_signal_key_s {
    char signal[BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE]; /* Signal name */
    char from[BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE]; /* From stage name */
    char to[BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE]; /* To stage name */
    char block[BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE]; /* Block name */
} bcm_instru_vis_signal_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Clean signal cache and allow signals for new packet to be recorded */
extern int bcm_instru_vis_signal_get(
    int unit, 
    bcm_core_t core_id, 
    uint32 flags, 
    int nof_signals, 
    bcm_instru_vis_signal_key_t *signal_key, 
    bcm_instru_vis_signal_result_t *signal_result);

#endif /* BCM_HIDE_DISPATCHABLE */

/* IFA encap flags. */
#define BCM_INSTRU_IFA_ENCAP_WITH_ID    0x00000001 /* Create IFA entity with
                                                      specific encap_id. */
#define BCM_INSTRU_IFA_ENCAP_REPLACE    0x00000002 /* Replace existing IFA
                                                      entity. */

/* IFA info */
typedef struct bcm_instru_ifa_info_s {
    uint32 flags;               /* See BCM_INSTRU_IFA_ENCAP_XXX flags
                                   definitions. */
    int ifa_encap_id;           /* Global lif */
    int stat_cmd;               /* Counter ID associated with the the ifa
                                   session for sequence number */
    uint8 counter_command_id;   /* Counter interface associated with the ifa
                                   session. */
} bcm_instru_ifa_info_t;

/* callback function prototype */
typedef int (*bcm_instru_ifa_encap_traverse_cb)(
    int unit, 
    bcm_instru_ifa_info_t *ifa_encap_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Fills the IFA types. */
extern int bcm_instru_ifa_encap_create(
    int unit, 
    bcm_instru_ifa_info_t *ifa_info);

/* Get IFA entry. */
extern int bcm_instru_ifa_encap_get(
    int unit, 
    bcm_instru_ifa_info_t *ifa_info);

/* Delete IFA entry. */
extern int bcm_instru_ifa_encap_delete(
    int unit, 
    bcm_instru_ifa_info_t *ifa_info);

/* Traverse IFA entries. */
extern int bcm_instru_ifa_encap_traverse(
    int unit, 
    bcm_instru_ifa_encap_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* sFlow encap extended destination info */
typedef struct bcm_instru_sflow_encap_extended_dst_info_s {
    uint32 flags;                       /* See BCM_INSTRU_SFLOW_ENCAP_xxx flag
                                           definitions. */
    int sflow_encap_extended_dst_id;    /* Global lif */
    uint32 dst_as_path[BCM_INSTRU_SFLOW_ENCAP_EXTENDED_NOF_DSTS]; /* Autonomous System path to the
                                           destination */
    int dst_as_path_length;             /* Autonomous System path list length */
    bcm_ip_t next_hop_ipv4_address;     /* Next Hop IPv4 Address. */
    bcm_ip6_t next_hop_ipv6_address;    /* Next Hop IPv6 Address. */
} bcm_instru_sflow_encap_extended_dst_info_t;

/* Autonomous System Destination callback function prototype */
typedef int (*bcm_instru_sflow_encap_extended_dst_cb)(
    int unit, 
    bcm_instru_sflow_encap_extended_dst_info_t *sflow_encap_ext_dst_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create Autonomous System Destination information for SFLOW Extended
 * Gateway Data Record.
 */
extern int bcm_instru_sflow_encap_extended_dst_create(
    int unit, 
    bcm_instru_sflow_encap_extended_dst_info_t *sflow_encap_ext_dst_info);

/* 
 * Get the Autonomous System Destination information for SFLOW Extended
 * Gateway Data Record.
 */
extern int bcm_instru_sflow_encap_extended_dst_get(
    int unit, 
    bcm_instru_sflow_encap_extended_dst_info_t *sflow_encap_ext_dst_info);

/* 
 * Delete Autonomous System Destination information for SFLOW Extended
 * Gateway Data Record.
 */
extern int bcm_instru_sflow_encap_extended_dst_delete(
    int unit, 
    bcm_instru_sflow_encap_extended_dst_info_t *sflow_encap_ext_dst_info);

/* 
 * Traverse Autonomous System Destination information for SFLOW Extended
 * Gateway Data Record.
 */
extern int bcm_instru_sflow_encap_extended_dst_traverse(
    int unit, 
    bcm_instru_sflow_encap_extended_dst_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* sFlow encap extended source info */
typedef struct bcm_instru_sflow_encap_extended_src_info_s {
    uint32 flags;                       /* See BCM_INSTRU_SFLOW_ENCAP_xxx flag
                                           definitions. */
    int sflow_encap_extended_src_id;    /* Global lif */
    uint32 src_as;                      /* Autonomous system number of source */
    uint32 src_as_peer;                 /* Autonomous system number of source
                                           peer */
} bcm_instru_sflow_encap_extended_src_info_t;

/* Autonomous System Source callback function prototype */
typedef int (*bcm_instru_sflow_encap_extended_src_cb)(
    int unit, 
    bcm_instru_sflow_encap_extended_src_info_t *sflow_encap_ext_src_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Create Autonomous System Source information for SFLOW Extended Gateway
 * Data Record.
 */
extern int bcm_instru_sflow_encap_extended_src_create(
    int unit, 
    bcm_instru_sflow_encap_extended_src_info_t *sflow_encap_ext_src_info);

/* 
 * Get the Autonomous System Source information for SFLOW Extended
 * Gateway Data Record.
 */
extern int bcm_instru_sflow_encap_extended_src_get(
    int unit, 
    bcm_instru_sflow_encap_extended_src_info_t *sflow_encap_ext_src_info);

/* 
 * Delete Autonomous System Source data base for SFLOW Extended Gateway
 * Data Record.
 */
extern int bcm_instru_sflow_encap_extended_src_delete(
    int unit, 
    bcm_instru_sflow_encap_extended_src_info_t *sflow_encap_ext_src_info);

/* 
 * Traverse Autonomous System Source information for SFLOW Extended
 * Gateway Data Record.
 */
extern int bcm_instru_sflow_encap_extended_src_traverse(
    int unit, 
    bcm_instru_sflow_encap_extended_src_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Instrumentation synced counters type of ingress cgm */
typedef enum bcm_instru_synced_counters_icgm_type_e {
    bcmInstruSyncedCountersIcgmTypeInvalid = 0, 
    bcmInstruSyncedCountersIcgmTypeMinFreeOcbBuffers = 1, 
    bcmInstruSyncedCountersIcgmTypeMinFreeOcbPDBs = 2, 
    bcmInstruSyncedCountersIcgmTypeMinFreeDramBDBs = 3, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVoqSharedBytes = 4, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVoqOcbSharedBuffers = 5, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVoqOcbSharedPDs = 6, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVsqHeadroomBytes = 7, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVsqSharedPoolIndexBytes = 8, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbHeadroomBuffers = 9, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbSharedPoolIndexBuffers = 10, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbHeadroomPDBs = 11, 
    bcmInstruSyncedCountersIcgmTypeMinFreeVsqOcbSharedPoolIndexPDBs = 12, 
    bcmInstruSyncedCountersIcgmTypeEnqueuePackets = 13, 
    bcmInstruSyncedCountersIcgmTypeEnqueueBytes = 14, 
    bcmInstruSyncedCountersIcgmTypeRejectPackets = 15, 
    bcmInstruSyncedCountersIcgmTypeRejectBytes = 16, 
    bcmInstruSyncedCountersIcgmTypeDequeuePackets = 17, 
    bcmInstruSyncedCountersIcgmTypeDequeueBytes = 18 
} bcm_instru_synced_counters_icgm_type_t;

/* Instrumentation synced counters type of nif */
typedef enum bcm_instru_synced_counters_nif_type_e {
    bcmInstruSyncedCountersNifTypeInvalid = 1000, 
    bcmInstruSyncedCountersNifTypeReceivedPackets = 1001, 
    bcmInstruSyncedCountersNifTypeReceivedBytes = 1002, 
    bcmInstruSyncedCountersNifTypeDroppedTdmPackets = 1003, 
    bcmInstruSyncedCountersNifTypeDroppedTdmBytes = 1004, 
    bcmInstruSyncedCountersNifTypeDroppedPriorityIndexPackets = 1005, 
    bcmInstruSyncedCountersNifTypeDroppedPriorityIndexBytes = 1006, 
    bcmInstruSyncedCountersNifTypePfcIndexNofOnTransitions = 1007, 
    bcmInstruSyncedCountersNifTypePfcIndexMaxClocksDuration = 1008, 
    bcmInstruSyncedCountersNifTypePfcIndexOnClockCyclesDuration = 1009 
} bcm_instru_synced_counters_nif_type_t;

/* Defines the types of periods for synchronized counters mechanism */
typedef enum bcm_instru_synced_counters_source_type_e {
    bcmInstruSyncedCountersTypeIcgm = 0, 
    bcmInstruSyncedCountersTypeNif = 1, 
    bcmInstruSyncedCountersTypeCrps = 2, 
    bcmInstruSyncedCountersTypeCount = 3 
} bcm_instru_synced_counters_source_type_t;

#define BCM_INSTRU_SYNCED_COUNTERS_RECORDS_MAX 16         /* Max instrumentation
                                                          synced counters
                                                          records in one API
                                                          call */

#define BCM_INSTRU_SYNCED_COUNTERS_ICGM_STATS_NOF 21         /* Total number of synced
                                                          icgm
                                                          counters/statistics
                                                          available */
#define BCM_INSTRU_SYNCED_COUNTERS_NIF_STATS_NOF 36         /* total number of synced
                                                          nif
                                                          counters/statistics
                                                          available */

#define BCM_INSTRU_SYNCED_COUNTERS_STATS_MAX BCM_INSTRU_SYNCED_COUNTERS_NIF_STATS_NOF /* total number of synced
                                                          counters/statistics
                                                          available (max of icgm
                                                          and nif counters).
                                                          Should be max of
                                                          BCM_INSTRU_SYNCED_COUNTERS_* */

/* 
 * Sync counter record, which gathers the counter values and time of day
 * that the counters was taken. Value array is organized according to the
 * 'stat format' (in the config API).
 */
typedef struct bcm_instru_synced_counters_record_s {
    uint64 estimated_time_of_day;       /* Returned IEEE1588 TOD. */
    uint64 value_arr[BCM_INSTRU_SYNCED_COUNTERS_STATS_MAX]; /* Ordered according to the stat format
                                           in the config API. */
} bcm_instru_synced_counters_record_t;

/* Configuration structure for synchronized counters mechanism */
typedef struct bcm_instru_synced_counters_config_s {
    int enable;                 /* if disable, all others params not relevant. */
    uint64 start_time;          /* -1 immediate, or TOD format */
    uint64 period;              /* nanoseconds */
    int is_eventor_collection;  /* For eventor collection nof_interval=1,
                                   otherwise, nof_interval=max (in HW) */
    int is_continuous;          /* if FALSE, HW count one round of interval, if
                                   TRUE, HW count in cyclic mode forever */
} bcm_instru_synced_counters_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API to get synchronized instrumentation counters configuration per
 * type
 */
extern int bcm_instru_synced_counters_config_get(
    int unit, 
    uint32 flags, 
    bcm_instru_synced_counters_source_type_t source_type, 
    bcm_instru_synced_counters_config_t *config);

/* 
 * API configure and enable/disable synchronized instrumentation counters
 * per type
 */
extern int bcm_instru_synced_counters_config_set(
    int unit, 
    uint32 flags, 
    bcm_instru_synced_counters_source_type_t source_type, 
    bcm_instru_synced_counters_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Structure with the information regarding the each counter, what it
 * represents.
 */
typedef struct bcm_instru_synced_counters_type_s {
    int counter_type;   /* The counter type represented as the relevant ENUM
                           (bcm_instru_synced_counters_icgm_type_t or
                           bcm_instru_synced_counters_nif_type_t) */
    int counter_index;  /* The index of the counter type, for counter types that
                           use indices. Relevant only for types index in their
                           name. */
} bcm_instru_synced_counters_type_t;

/* In/out structure, which gather the data reported of synced counters. */
typedef struct bcm_instru_synced_counters_data_s {
    int nof_records;                    /* Nof elements in record_arr. */
    bcm_instru_synced_counters_record_t record_arr[BCM_INSTRU_SYNCED_COUNTERS_RECORDS_MAX]; /* Each record represents the statistics
                                           of one interval */
    int nof_counters;                   /* Nof elements in
                                           record_arr_counters_type. */
    bcm_instru_synced_counters_type_t record_arr_counters_type[BCM_INSTRU_SYNCED_COUNTERS_STATS_MAX]; /* The list of counters in each record. */
} bcm_instru_synced_counters_data_t;

/* Information about the synced counters required. */
typedef struct bcm_instru_synced_counters_records_key_s {
    bcm_core_t core_id;                 /* The core from which we read the
                                           counters. */
    bcm_instru_synced_counters_source_type_t source_type; /* Source type of synchronized counters
                                           mechanism. */
    bcm_gport_t gport;                  /* Encodes the logical port, if relevant
                                           for the source type. */
} bcm_instru_synced_counters_records_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the counter values of sync counters mechanism. */
extern int bcm_instru_synced_counters_records_get(
    int unit, 
    uint32 flags, 
    bcm_instru_synced_counters_records_key_t *key, 
    bcm_instru_synced_counters_data_t *data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize bcm_instru_synced_counters_config_t struct. */
extern void bcm_instru_synced_counters_config_t_init(
    bcm_instru_synced_counters_config_t *config);

/* Initialize bcm_instru_synced_counters_records_key_t struct. */
extern void bcm_instru_synced_counters_records_key_t_init(
    bcm_instru_synced_counters_records_key_t *key);

/* Initialize bcm_instru_synced_counters_data_t struct. */
extern void bcm_instru_synced_counters_data_t_init(
    bcm_instru_synced_counters_data_t *data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add mapping of physical port to Sample Interface. */
extern int bcm_instru_sflow_sample_interface_add(
    int unit, 
    bcm_instru_sflow_sample_interface_info_t *sample_interface_info);

/* Remove mapping of physical port to Sample Interface. */
extern int bcm_instru_sflow_sample_interface_remove(
    int unit, 
    bcm_instru_sflow_sample_interface_info_t *sample_interface_info);

/* Get mapping of physical port to Sample Interface. */
extern int bcm_instru_sflow_sample_interface_get(
    int unit, 
    bcm_instru_sflow_sample_interface_info_t *sample_interface_info);

/* Traverse on the mapping of physical port to Sample Interface */
extern int bcm_instru_sflow_sample_interface_traverse(
    int unit, 
    bcm_instru_sflow_sample_interface_traverse_info_t *sample_interface_traverse_info, 
    bcm_instru_sflow_sample_traverse_cb cb, 
    void *user_data);

/* Enable or disable the specific synchronized triggers. */
extern int bcm_instru_synced_counters_trigger_enable_set(
    int unit, 
    uint32 flags, 
    uint32 nof_sources, 
    bcm_instru_synced_counters_source_type_t *sources, 
    uint32 *enable);

/* Get the specific synchronized triggers. */
extern int bcm_instru_synced_counters_trigger_enable_get(
    int unit, 
    uint32 flags, 
    uint32 nof_sources, 
    bcm_instru_synced_counters_source_type_t *sources, 
    uint32 *enable);

/* Set the configuration of the given Event ID. */
extern int bcm_instru_eventor_event_id_config_set(
    int unit, 
    uint32 flags, 
    bcm_eventor_event_id_t event_id, 
    bcm_instru_eventor_event_id_config_t *config);

/* Get the configuration of the given Event ID. */
extern int bcm_instru_eventor_event_id_config_get(
    int unit, 
    uint32 flags, 
    bcm_eventor_event_id_t event_id, 
    bcm_instru_eventor_event_id_config_t *config);

/* Generate a single eventor event */
extern int bcm_instru_eventor_event_generate(
    int unit, 
    uint32 flags, 
    bcm_eventor_event_id_t event_id);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_INSTRU_H__ */
