/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    llm_msg.h
 * Purpose: llm Messages definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 */

#ifndef   _SOC_SHARED_LLM_MSG_H_
#define   _SOC_SHARED_LLM_MSG_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/mos_msg_common.h>
/*#include <shared/llm.h>*/



/********************************
 * LLM mutual structure defenition
 *
 *   SDK Host  and uController
 */

/*** 
  * 
  * * Version 001
  *  Basic prototypes of the Control messaging
  *  Learningn and Deleting of the entries
  *  
  * Version 002
  *  Parsing of diag lookup fixed
  *  
  * Version 003
  *  Parsing of diag lookup fixed
  *  
  * Version 004
  *  Transplant added 
  *  
  * Version 005
  *  Artificial delay added to limit rate to 16k
  *  Function _llm_pon_att_mac_bitmap_set modifed to set a range of tunnels
  *  
  * Version 006
  *  Iniitialisation of the database is fixed to enable correct bank configuration
  *  
  * Version 007
  *  VMAC integration. Messaging mechanism improoved. Added notifications. 
 */
#define SHR_LLM_NUM_OF_PORTS            (256)
#define SHR_LLM_APPL_VERSION            0x00000007
#define SHR_LLM_PULL_INDEX_PER_ENTRY    (32)
#define SHR_LLM_PULL_MAX_INDEX_NUM_SIZE (1024*8) /*8K max indexes to manage*/
#define SHR_LLM_PULL_MAX_SIZE           (SHR_LLM_PULL_MAX_INDEX_NUM_SIZE/SHR_LLM_PULL_INDEX_PER_ENTRY)
#define SHR_LLM_MAX_PON_PORT_NUM_OF_BITS (3)/*3 bits*/
#define SHR_LLM_MAX_TUNNEL_NUM_OF_BITS   (11)/* 11 bits*/
#define SHR_LLM_MAX_AC_NUM_OF_BITS       (16)/* 16 bits 0xffff*/
#define SHR_LLM_MAX_PON_PORT_INDEX ((1 << (SHR_LLM_MAX_PON_PORT_NUM_OF_BITS))-1)/*3 bits*/
#define SHR_LLM_MAX_TUNNEL_INDEX   ((1 << (SHR_LLM_MAX_TUNNEL_NUM_OF_BITS))-1)/*11 bits*/
#define SHR_LLM_MAX_AC_INDEX       ((1 << (SHR_LLM_MAX_AC_NUM_OF_BITS))-1)/*15 bits*/      
#define SHR_LLM_PON_DB_SIZE        ((SHR_LLM_MAX_PON_PORT_INDEX+1)*(SHR_LLM_MAX_TUNNEL_INDEX+1))
#define SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY_NUM             0x05
#define SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY_NUM       16
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_LSB        28
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_MSB        31
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_NOF_BITS  (SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_MSB - \
                                                             SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_LSB + 1)
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_BIT             27
#define SHR_LLM_MSG_PON_ATT_REPLY_FIFO_ENABLE_BIT            26
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_MASK            25
#define SHR_LLM_MSG_PON_ATT_REPLY_FIFO_ENABLE_MASK           24
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_PORT_LSB               0
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_PORT_MSB               7
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_PORT_NOF_BITS         (SHR_LLM_MSG_PON_ATT_MAC_LIMIT_PORT_MSB - \
                                                             SHR_LLM_MSG_PON_ATT_MAC_LIMIT_PORT_LSB + 1)
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_TUNNEL_ID_LSB          8
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_TUNNEL_ID_MSB          23
#define SHR_LLM_MSG_PON_ATT_MAC_LIMIT_TUNNEL_ID_NOF_BITS    (SHR_LLM_MSG_PON_ATT_MAC_LIMIT_TUNNEL_ID_MSB - \
                                                             SHR_LLM_MSG_PON_ATT_MAC_LIMIT_TUNNEL_ID_LSB + 1)
#define SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM           2
#define SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX_NUM           4

/* 
 * LLM Aplication
 */
#define SHR_LLM_SDK_VERSION                   0x01000001
#define SHR_LLM_UC_MIN_VERSION                0x1
#define SHR_LLM_UC_MSG_TIMEOUT_USECS          20000000
#define SHR_LLM_EVENT_THREAD_PRIORTY          50

typedef enum
{
    SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_PER_PORT_TUNNEL = 0,
    SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_PER_PORT,
    SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_GLOBAL,
    SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_MAX
}SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE;

typedef enum
{
    SHR_LLM_MSG_PON_ATT_UPDATE_UNDEFINED = 0,
    SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY,
    SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE,
    SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX,
    SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY,
    SHR_LLM_MSG_PON_ATT_UPDATE_MAX
}SHR_LLM_MSG_PON_ATT_UPDATE_DATA_TYPE;

typedef enum
{
    SHR_LLM_MSG_PON_ATT_MAC_BITMAP_SET,
    SHR_LLM_MSG_PON_ATT_MAC_BITMAP_INCREASE,
    SHR_LLM_MSG_PON_ATT_MAC_BITMAP_DECREASE,
    SHR_LLM_MSG_PON_ATT_MAC_BITMAP_MAX
}SHR_LLM_MSG_PON_ATT_MAC_BITMAP_TYPE;

/********************************
 * LLM Control Messages
 *
 *   SDK Host <--> uController
 */

/*
 * LLM Pointer pool database
 */
typedef struct shr_llm_pointer_pool_s{
    uint32    entries[SHR_LLM_PULL_MAX_SIZE];/*Bit maps*/
    uint32    firstIndexOffset; 
    uint16    size;
    uint16    numOfFreeIndexes;
    uint16    numOfReservedIndexes;
    uint8     recentEntry;       
} shr_llm_pointer_pool_t;



typedef struct shr_llm_PON_ID_attributes_s{         
    uint32 entries[SHR_LLM_MAX_TUNNEL_INDEX];  
    uint32 port;   
} shr_llm_PON_ID_attributes_t;


/*
 * LLM Initialization control message
 */
typedef struct shr_llm_msg_ctrl_init_s {
    uint32 dsp_eth_type;        /* Configuration of DSP ETh type packets */
    uint32 voq_mapping_mode;    /* Configraiton of the VOQ mapping mode / direct/indirect */
    uint32 direct_table_bank;   /* Bank ID of the directa table where the indication will be stored*/
    uint32 direct_table_offset; /* Offset from the start of the Bank where the indictaion will be stored */
    uint32 eedb_pointer_database_offset; /* Offset from the start of the EEDB pointer database */
    uint32 eedb_pointer_database_size; /* Max number of entries that can be inserted into the databse*/
    uint8 eedb_omac_table_identifyer;
} shr_llm_msg_ctrl_init_t;


/*
 * LLM application info get message
 */
typedef struct shr_llm_msg_app_info_get_s {
    uint32 counterEvents;       
    uint32 counterEventsLearn;  
    uint32 suc_counterEventsLearn; 
    uint32 counterEventsDelete; 
    uint32 suc_counterEventsDelete; 
    uint32 counterEventsTransp; 
    uint32 suc_counterEventsTransp;
    uint32 counterEventsUnKnown;
    uint32 num_of_activation;
    uint32 time_of_activation; 
} shr_llm_msg_app_info_get_t;

/*
 * LLM info message
 */

typedef enum {
    LLM_SERVICE_NOT_DEFINED=0,
    LLM_SERVICE_MAC_LIMIT ,
    LLM_SERVICE_VMAC,
    LLM_SERVICE_MAX
} SHR_LLM_SERVICE_TYPE;


typedef struct shr_llm_msg_pon_att_s {
    uint32 port;        
    uint32 tunnel;    
    uint32 tunnel_count; 
    uint32 bitmap; 
    uint32 type_of_service;          
} shr_llm_msg_pon_att_t;

/*
 * LLM enable/disable message
 *   This message is used to enable/disable MAC limit function in ARM CPU
 *
 *   uint32 type_of_service: which service will be enabled/disabled.
 *          Now this variable  only be LLM_SERVICE_MAC_LIMIT 
 *   uint32 att_val: This variable include 3 information(type , enable/disable service and port value)
 *          1~4msb: Type, enable/disable MAC limit function base port *tunnel, port or global
 *                  0 ~ Physical port *  tunnel
 *                  1 ~ Physical port , can be PON port or NNI port
 *                  2 ~ Global
 *          5msb: A variable to disable/enable MAC limiting in ARM CPU.
 *          6msb: A variable to disable/enable read reply-FIFO for FLUSH DB function
 *          7~8msb: mast for bit 5~6.
 *          24lsb: Value
 *                  if type is 0, then 8lsb is physical port , and other bits is tunnel ID.
 *                  if type is 1, then 24lsb is physical port.
 *                  if type is 2, not care 24lsb , and value is always 0.
 */
typedef struct shr_llm_msg_pon_att_enable_s {
    uint32 type_of_service;          
    uint32 att_val; 
} shr_llm_msg_pon_att_enable_t;

/* CPU request request register */
typedef struct shr_llm_msg_pon_att_l2_entry_s {
    uint32 entry[SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY_NUM];
} shr_llm_msg_pon_att_l2_entry_t;

/* VMAC encoding */
typedef struct shr_llm_msg_pon_att_vmac_encode_s {
    uint32 value[SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM];
    uint32 mask[SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM];
} shr_llm_msg_pon_att_vmac_encode_t;

/* VMAC encoding */
typedef struct shr_llm_msg_pon_att_vmac_prefix_s {
    uint32 prefix[SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX_NUM]; /* 0 - omac, 1 - omac2vmac, 2 - vmac, 3 - vmac2omac */
} shr_llm_msg_pon_att_vmac_prefix_t;

/* Multi entry encoding */
typedef struct shr_llm_msg_pon_att_multi_l2_entry_s {
    uint32 l2_entry_count;
    shr_llm_msg_pon_att_l2_entry_t l2_entry[SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY_NUM];
} shr_llm_msg_pon_att_multi_l2_entry_t;

/* PON attribution */
typedef union shr_llm_msg_pon_att_data_s {
    shr_llm_msg_pon_att_l2_entry_t l2_entry;
    shr_llm_msg_pon_att_vmac_encode_t vmac_encode;
    shr_llm_msg_pon_att_vmac_prefix_t vmac_prefix;
    shr_llm_msg_pon_att_multi_l2_entry_t multi_l2_entry;
}shr_llm_msg_pon_att_data_t;

/*
 * LLM update message
 *  This message is used to update pon attribution.
 *
 *  uint32 type_of_service: which service, can be LLM_SERVICE_MAC_LIMIT or LLM_SERVICE_VMAC
 *  uint32 data_type: data type, 
 *                    xxx_MAC_ENTRY_DATA_TYPE - add/delete/modify a L2 entry in MACT table
 *                     
 *  uint32 data_len: data length. Now the value is 5 because of the data always is CPU_request_request  registervalue.;
 *  uint32 data[0]:  Value of CPU_request_request  register
 */
typedef struct shr_llm_msg_pon_att_update_s {
    uint32 type_of_service;          
    uint32 data_type;
    uint32 data_len;
    shr_llm_msg_pon_att_data_t data;
} shr_llm_msg_pon_att_update_t;

/*
 * LLM rx mode message
 *  This message is used to set the rx mode.
 *
 *  uint32 mode: 0 -> DSP paring mode, 1 -> MACT event FIFO mode 
 */
typedef struct shr_llm_msg_pon_att_rx_mode_s {
    uint32 mode;
} shr_llm_msg_pon_att_rx_mode_t;

/*
 * LLM MAC move message
 *  This message is used to set the MAC move.
 *
 *  uint32 value: 0 -> disable the MAC move, 1 -> enable the MAC move 
 */
typedef struct shr_llm_msg_pon_att_mac_move_s {
    uint32 port;
    uint32 value;
} shr_llm_msg_pon_att_mac_move_t;


/*
 * LLM Limitation configuration Set/Get control message
 */
typedef struct shr_llm_msg_ctrl_limit_value_s {
    uint32  pon_port_id;                /* Pon Port id to configre*/
    uint32  Tunnel_id;                  /* Tunnel ID to configure*/
    uint32  limit_value;                /* The limit value */
    uint8   limit_reached_direct_get;   /*Valid only for get:Inidication "if the limit was reached" from the direct table*/
    uint8   limit_reached_shadow_get;   /*Valid only for get:Inidication "if the limit was reached" from the shadow table*/
    uint8   above_limit_reported_get;   /*Valid only for get:Inidication "if learning above the limit" was sent to the CPU*/
} shr_llm_msg_ctrl_limit_value_t;

/*
 * LLM notification message
 */

typedef enum shr_llm_msg_notification_type_e {
    LLM_EVENT_DUMMY = 0,
    LLM_EVENT_OVERWRITE_STATISTICS,
    LLM_EVENT_PON_ID_LIMIT_REACHED,
    LLM_EVENT_PON_ID_LIMIT_REACHED_CLEAR,
    LLM_EVENT_PON_ID_OVER_LIMIT,
    LLM_EVENT_OUT_OF_EEDB_RESOURCES,
    LLM_EVENT_ERROR_VMAC_USED,
    LLM_EVENT_ERROR_MACT_MANAGMENT_FAILURE,
    LLM_EVENT_ERROR_PORT_PRIORTY,
    LLM_EVENT_ERROR_UNEXPECTED_TRANSLPANT_EVENT,
    LLM_EVENT_WARNING_UNKNOWN_MACT_EVENT_TYPE,
    LLM_EVENT_ERROR_WITH_LINE,
    LLM_EVENT_MAX
} shr_llm_msg_notification_type_e;

#ifndef BCM_UKERNEL
#define SHR_LLM_MSG_NOTIFICATION_TYPE_NAME \
    "LLM_EVENT_DUMMY", \
    "LLM_EVENT_OVERWRITE_STATISTICS",\
    "LLM_EVENT_PON_ID_LIMIT_REACHED",\
    "LLM_EVENT_PON_ID_LIMIT_REACHED_CLEAR",\
    "LLM_EVENT_PON_ID_OVER_LIMIT",\
    "LLM_EVENT_OUT_OF_EEDB_RESOURCES",\
    "LLM_EVENT_ERROR_VMAC_USED",\
    "LLM_EVENT_ERROR_MACT_MANAGMENT_FAILURE",\
    "LLM_EVENT_ERROR_PORT_PRIORTY",\
    "LLM_EVENT_ERROR_UNEXPECTED_TRANSLPANT_EVENT",\
    "LLM_EVENT_WARNING_UNKNOWN_MACT_EVENT_TYPE",\
    "LLM_EVENT_ERROR_WITH_LINE",\
    "LLM_EVENT_MAX",
#endif

/****************************************
 * LLM event message
 */
#define LLM_BTE_EVENT_LB_TIMEOUT                 0x0001

/*
 *  The LLM event message is defined as a short message (use mos_msg_data_t).
 *
 *  The fields of mos_msg_data_t are used as followed:
 *      mclass   (uint8)  - MOS_MSG_CLASS_LLM_EVENT
 *      subclass (uint8)  - Unused
 *      len      (uint16) - LLM Session ID
 *      data     (uint32) - Events mask
 */
typedef mos_msg_data_t  llm_msg_event_t;

/*
 *  The LLM MAC move event message
 */
typedef struct shr_llm_msg_pon_att_mac_move_event_s {
    uint8 mac[6];          
    uint16 vsi;
    uint16 incoming_ac;
    uint16 existing_ac; 
} shr_llm_msg_pon_att_mac_move_event_t;

extern int shr_llm_msg_init(int unit);
extern int shr_llm_msg_uninit(int unit);
extern int shr_llm_msg_mac_limit_enable(int unit, uint8 mac_limit_enable, uint8 reply_fifo_enable);
extern int shr_llm_msg_mac_limit_set(int unit, uint8 type, shr_llm_msg_pon_att_t *msg);
extern int shr_llm_msg_mac_limit_get(int unit, shr_llm_msg_pon_att_t *msg, shr_llm_msg_pon_att_t *rsp);
extern int shr_llm_msg_mac_rx_mode_set(int unit, uint32 mode);
extern int shr_llm_msg_mac_move_set(int unit, uint32 port, uint32 value);
extern int shr_llm_msg_mac_move_get(int unit, uint32 port, uint32 *value);
extern int shr_llm_mac_move_event_set(int unit, uint32 event_id, uint32 value);
extern int shr_llm_mac_move_event_get(int unit, uint32 event_id, uint32 *value);
extern int shr_llm_msg_pon_attr_set(int unit, uint32 serv_type, uint32 data_type, void *data);
#endif /* _SOC_SHARED_LMM_MSG_H_ */
