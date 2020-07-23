/*! \file mcs_msg_common.h
 *
 * MCS Messaging common to SDK and uKernel
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_MSG_COMMON_H
#define MCS_MSG_COMMON_H

/* Macros for Bit Manipulation on a 32-bit word */

#ifndef F32_MASK
/*! Create a bit mask of w bits as a 32-bit word. */
#define F32_MASK(w) \
        (((uint32_t)1 << w) - 1)
#endif

#ifndef F32_GET
/*! Extract a field of w bits at offset o from a 32-bit word d. */
#define F32_GET(d,o,w) \
        (((d) >> o) & F32_MASK(w))
#endif

#ifndef F32_SET
/*! Set a field of w bits at offset o in a 32-bit word d. */
#define F32_SET(d,o,w,v) \
        (d = ((d & ~(F32_MASK(w) << o)) | (((v) & F32_MASK(w)) << o)))
#endif

/*! Maximum Number of uC's */
#define MCS_NUM_UC 4

/* For now, all of the interfaces have 16 pending messages */
/*! Number of pending messages */
#define NUM_MCS_MSG_SLOTS 16

/*! Increment message slot with rollover */
#define MCS_MSG_INCR(val) (((val) + 1) & (NUM_MCS_MSG_SLOTS - 1))

/*!
 * \brief Format of the debug data from the MCS processors.
 */
typedef struct mcs_debug_extn_s {
    /*! Version number will be updated when new data is added.
     * Check the version before trying to use some debug data
     */
    uint32_t dbg_version;
    /*! Thread History Index */
    uint32_t thrd_hist_idx;
    /*! Thread switch history of uKernel */
    uint32_t thrd_hist[16];
} mcs_debug_extn_t;

/* This is the format of the status word sent/recvd between the
   processors.  The ack bits is what limits the number of msgs to 16. */
/*! State bit of Message Status */
#define MCS_MSG_STATUS_STATE(d) F32_GET(d,0,2)
/*! Sent Index of Message Status */
#define MCS_MSG_STATUS_SENT_INDEX(d) F32_GET(d,2,4)
/*! Ack Index of Message Status */
#define MCS_MSG_STATUS_ACK_INDEX(d) F32_GET(d,6,4)

/*! Write State bit of Message Status */
#define MCS_MSG_STATUS_STATE_W(d, v) F32_SET(d,0,2,v)
/*! Write Sent Index of Message Status */
#define MCS_MSG_STATUS_SENT_INDEX_W(d, v) F32_SET(d,2,4,v)
/*! Write Ack Index of Message Status */
#define MCS_MSG_STATUS_ACK_INDEX_W(d, v)  F32_SET(d,6,4,v)

/*! Message n'th Ack bit. */
#define MCS_MSG_ACK_BIT(d,n) F32_GET(d,((n)+16),1)
/*! Set Message n'th Ack bit */
#define MCS_MSG_ACK_BIT_SET(d,n) F32_SET(d,((n)+16),1,1)
/*! Clear Message n'th Ack bit */
#define MCS_MSG_ACK_BIT_CLEAR(d,n) F32_SET(d,((n)+16),1,0)

/*! MCS State - Reset/Init/Ready */
typedef uint32_t mcs_msg_host_status_t;

/*! Used to force a reset */
#define MCS_MSG_RESET_STATE 1
/*! Used to indicate initilaized */
#define MCS_MSG_INIT_STATE  2
/*! In normal mode */
#define MCS_MSG_READY_STATE 3

/*!
 * \brief MCS Message Data Format.
 */
typedef union {
    /*! MCS Message data raw access */
    uint32_t   words[3];
    /*! MCS Message data fields Used only by the uKernel. */
    struct {
        uint8_t    mclass;        /* Class of this message */
        uint8_t    subclass;      /* Subclass (or data for raw) */
        uint16_t   len;           /* Data field (usually length) */
        uint32_t   data;          /* Data field (usually LSW of phys addr) */
        uint32_t   data1;         /* Data field (usually MSW of phys addr) */
    } s;
} mcs_msg_data_t;

/* use these macros instead of the struct
 * to be Endian-Agonostic.
 */
/*! Get Message class. */
#define MCS_MSG_MCLASS_GET(s) F32_GET((s.words[0]),24,8)
/*! Set Message class. */
#define MCS_MSG_MCLASS_SET(s,v) F32_SET((s.words[0]),24,8,v)
/*! Get Message Subclass. */
#define MCS_MSG_SUBCLASS_GET(s) F32_GET((s.words[0]),16,8)
/*! Set Message Subclass. */
#define MCS_MSG_SUBCLASS_SET(s,v) F32_SET((s.words[0]),16,8,v)
/*! Get Message Length. */
#define MCS_MSG_LEN_GET(s) F32_GET((s.words[0]),0,16)
/*! Set Message Length. */
#define MCS_MSG_LEN_SET(s,v) F32_SET((s.words[0]),0,16,v)
/*! Get Message Data. */
#define MCS_MSG_DATA_GET(s) (s.words[1])
/*! Set Message Data. */
#define MCS_MSG_DATA_SET(s,v) (s.words[1] = v)
/*! Get Message Data. */
#define MCS_MSG_DATA1_GET(s) (s.words[2])
/*! Set Message Data. */
#define MCS_MSG_DATA1_SET(s,v) (s.words[2] = v)

/* Notes:
 *  - Changing MCS_MSG_CLASS numbers will break backwards compatibility.
 *  - Please contact stakeholders before modifying MCS_MSG_CLASS sections
 */

/*! Highest order bit (0x80) is used for the subclass REPLY message */
#define MCS_MSG_SUBCLASS_REPLY(subclass) (0x80 | (subclass))

/*! This will indicate a "long" message (DMA required) */
#define MCS_MSG_DMA_FLAG                     0x40
/*! Set the DMA message */
#define MCS_MSG_DMA_SET(subclass)          ((subclass) | MCS_MSG_DMA_FLAG)
/*! Clear the DMA message */
#define MCS_MSG_DMA_CLEAR(subclass)        ((subclass) & ~MCS_MSG_DMA_FLAG)
/*! Check for the DMA message */
#define MCS_MSG_DMA_MSG(subclass)          ((subclass) & MCS_MSG_DMA_FLAG)

/* Define the message classes and subclasses */
/*! MCS Messaging System class */
#define MCS_MSG_CLASS_SYSTEM            0
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_PING              1
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_APPL_INIT         2
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_INFO              3
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_VERSION           4
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_DMA_ENDIAN        5
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_EXCEPTION         6
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_CON_OUT           7
/*! Subclass for MCS Messaging System class */
#define   MCS_MSG_SUBCLASS_SYSTEM_CON_IN            8

/*! MCS Messaging System class Version */
#define MCS_MSG_CLASS_VERSION           1

/*! MCS Messaging Streaming Telemetry(ST) class */
#define MCS_MSG_CLASS_ST                2

/*! MCS Messaging Streaming Telemetry(ST) application ready */
#define MCS_ST_APPL_READY                         0x00
/*! MCS Messaging Streaming Telemetry(ST) application initialization */
#define MCS_ST_INIT                               0x01
/*! MCS Messaging Streaming Telemetry(ST) application detach */
#define MCS_ST_SHUTDOWN                           0x02
/*! MCS Messaging Streaming Telemetry(ST) ports addition */
#define MCS_ST_PORTS_ADD                          0x03
/*! MCS Messaging Streaming Telemetry(ST) ports fetch */
#define MCS_ST_PORTS_GET                          0x04
/*! MCS Messaging Streaming Telemetry(ST) ports delete */
#define MCS_ST_PORTS_DELETE                       0x05
/*! MCS Messaging Streaming Telemetry(ST) instance creation */
#define MCS_ST_INSTANCE_CREATE                    0x06
/*! MCS Messaging Streaming Telemetry(ST) instance fetch */
#define MCS_ST_INSTANCE_GET                       0x07
/*! MCS Messaging Streaming Telemetry(ST) instance delete */
#define MCS_ST_INSTANCE_DELETE                    0x08
/*! MCS Messaging Streaming Telemetry(ST) collector creation */
#define MCS_ST_COLLECTOR_CREATE                   0x09
/*! MCS Messaging Streaming Telemetry(ST) collector delete */
#define MCS_ST_COLLECTOR_DELETE                   0x0A
/*! MCS Messaging Streaming Telemetry(ST) collector association */
#define MCS_ST_INSTANCE_COLLECTOR_ATTACH          0x0B
/*! MCS Messaging Streaming Telemetry(ST) collector detach */
#define MCS_ST_INSTANCE_COLLECTOR_DETACH          0x0C
/*! MCS Messaging Streaming Telemetry(ST) system ID set */
#define MCS_ST_SYSTEM_ID_SET                      0x0D
/*! MCS Messaging Streaming Telemetry(ST) system ID fetch */
#define MCS_ST_SYSTEM_ID_GET                      0x0E
/*! MCS Messaging Streaming Telemetry(ST) stats fetch */
#define MCS_ST_INSTANCE_EXPORT_STATS_GET          0x0F
/*! MCS Messaging Streaming Telemetry(ST) stats reset */
#define MCS_ST_INSTANCE_EXPORT_STATS_SET          0x10
/*! MCS Messaging Streaming Telemetry(ST) configuration set */
#define MCS_ST_CONFIG_SET                         0x11


/*! MCS Messaging Streaming Telemetry(ST) application ready message */
#define MCS_MSG_SUBCLASS_ST_APPL_READY MCS_ST_APPL_READY

/*! MCS Messaging Streaming Telemetry(ST) application init message */
#define MCS_MSG_SUBCLASS_ST_INIT MCS_MSG_DMA_SET(MCS_ST_INIT)

/*! MCS Messaging Streaming Telemetry(ST) application init reply message */
#define MCS_MSG_SUBCLASS_ST_INIT_REPLY MCS_MSG_SUBCLASS_REPLY(MCS_ST_INIT)

/*! MCS Messaging Streaming Telemetry(ST) application detach message */
#define MCS_MSG_SUBCLASS_ST_SHUTDOWN MCS_ST_SHUTDOWN

/*! MCS Messaging Streaming Telemetry(ST) application detach reply message */
#define MCS_MSG_SUBCLASS_ST_SHUTDOWN_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_SHUTDOWN)

/*! MCS Messaging Streaming Telemetry(ST) application ports add message */
#define MCS_MSG_SUBCLASS_ST_PORTS_ADD MCS_MSG_DMA_SET(MCS_ST_PORTS_ADD)

/*! MCS Messaging Streaming Telemetry(ST) application ports add reply message */
#define MCS_MSG_SUBCLASS_ST_PORTS_ADD_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_PORTS_ADD)

/*! MCS Messaging Streaming Telemetry(ST) application ports get message */
#define MCS_MSG_SUBCLASS_ST_PORTS_GET MCS_ST_PORTS_GET

/*! MCS Messaging Streaming Telemetry(ST) application ports get reply message */
#define MCS_MSG_SUBCLASS_ST_PORTS_GET_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_ST_PORTS_GET))

/*! MCS Messaging Streaming Telemetry(ST) application ports delete message */
#define MCS_MSG_SUBCLASS_ST_PORTS_DELETE MCS_MSG_DMA_SET(MCS_ST_PORTS_DELETE)

/*! MCS Messaging Streaming Telemetry(ST) application ports
 * delete reply message
 */
#define MCS_MSG_SUBCLASS_ST_PORTS_DELETE_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_PORTS_DELETE)

/*! MCS Messaging Streaming Telemetry(ST) application instance create message */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_CREATE \
    MCS_MSG_DMA_SET(MCS_ST_INSTANCE_CREATE)

/*! MCS Messaging Streaming Telemetry(ST) application instance
 *  create reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_CREATE_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_INSTANCE_CREATE)

/*! MCS Messaging Streaming Telemetry(ST) application instance get message */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_GET MCS_ST_INSTANCE_GET

/*! MCS Messaging Streaming Telemetry(ST) application instance
 *  get reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_GET_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_ST_INSTANCE_GET))

/*! MCS Messaging Streaming Telemetry(ST) application instance delete message */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_DELETE MCS_ST_INSTANCE_DELETE

/*! MCS Messaging Streaming Telemetry(ST) application instance
 *  delete reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_DELETE_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_INSTANCE_DELETE)

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  create message
 */
#define MCS_MSG_SUBCLASS_ST_COLLECTOR_CREATE \
    MCS_MSG_DMA_SET(MCS_ST_COLLECTOR_CREATE)

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  create reply message
 */
#define MCS_MSG_SUBCLASS_ST_COLLECTOR_CREATE_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_COLLECTOR_CREATE)

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  delete message
 */
#define MCS_MSG_SUBCLASS_ST_COLLECTOR_DELETE MCS_ST_COLLECTOR_DELETE

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  delete reply message
 */
#define MCS_MSG_SUBCLASS_ST_COLLECTOR_DELETE_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_COLLECTOR_DELETE)

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  attach message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_ATTACH \
    MCS_MSG_DMA_SET(MCS_ST_INSTANCE_COLLECTOR_ATTACH)

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  attach reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_ATTACH_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_INSTANCE_COLLECTOR_ATTACH)

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  detach message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_DETACH \
    MCS_ST_INSTANCE_COLLECTOR_DETACH

/*! MCS Messaging Streaming Telemetry(ST) application collector
 *  detach reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_DETACH_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_INSTANCE_COLLECTOR_DETACH)

/*! MCS Messaging Streaming Telemetry(ST) application system
 *  ID set message
 */
#define MCS_MSG_SUBCLASS_ST_SYSTEM_ID_SET MCS_MSG_DMA_SET(MCS_ST_SYSTEM_ID_SET)

/*! MCS Messaging Streaming Telemetry(ST) application system
 *  ID set reply message
 */
#define MCS_MSG_SUBCLASS_ST_SYSTEM_ID_SET_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_SYSTEM_ID_SET)

/*! MCS Messaging Streaming Telemetry(ST) application system
 *  ID get message
 */
#define MCS_MSG_SUBCLASS_ST_SYSTEM_ID_GET MCS_ST_SYSTEM_ID_GET

/*! MCS Messaging Streaming Telemetry(ST) application system
 *  ID get reply message
 */
#define MCS_MSG_SUBCLASS_ST_SYSTEM_ID_GET_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_ST_SYSTEM_ID_GET))

/*! MCS Messaging Streaming Telemetry(ST) application stats
 *  fetch message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_GET \
    MCS_ST_INSTANCE_EXPORT_STATS_GET

/*! MCS Messaging Streaming Telemetry(ST) application stats
 *  fetch reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_GET_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_ST_INSTANCE_EXPORT_STATS_GET))

/*! MCS Messaging Streaming Telemetry(ST) application stats
 *  set message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_SET \
    MCS_MSG_DMA_SET(MCS_ST_INSTANCE_EXPORT_STATS_SET)

/*! MCS Messaging Streaming Telemetry(ST) application stats
 *  set reply message
 */
#define MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_SET_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_INSTANCE_EXPORT_STATS_SET)

/*! MCS Messaging Streaming Telemetry(ST) application
 *  configuration message
 */
#define MCS_MSG_SUBCLASS_ST_CONFIG \
    MCS_MSG_DMA_SET(MCS_ST_CONFIG_SET)

/*! MCS Messaging Streaming Telemetry(ST) application
 *  configuration reply message
 */
#define MCS_MSG_SUBCLASS_ST_CONFIG_REPLY \
    MCS_MSG_SUBCLASS_REPLY(MCS_ST_CONFIG_SET)

/*! FT Embedded Application messaging  class */
#define MCS_MSG_CLASS_FT                          3

/*! Message indicating that FT EApp is ready */
#define MCS_FT_APPL_READY                         0x00

/*! Message to initialize FT EApp */
#define MCS_FT_INIT                               0x01

/*! Message to shutdown FT EApp */
#define MCS_FT_SHUTDOWN                           0x02

/*! Message to send EM key format */
#define MCS_FT_EM_KEY_FORMAT                      0x03

/*! Message to create flow group */
#define MCS_FT_GROUP_CREATE                       0x04

/*! Message to get flow group */
#define MCS_FT_GROUP_GET                          0x05

/*! Message to delete flow group */
#define MCS_FT_GROUP_DELETE                       0x06

/*! Message to update flow group */
#define MCS_FT_GROUP_UPDATE                       0x07

/*! Message to get group flow data */
#define MCS_FT_GROUP_FLOW_DATA_GET                0x08

/*! Message to create template */
#define MCS_FT_TEMPLATE_CREATE                    0x09

/*! Message to get tempalte */
#define MCS_FT_TEMPLATE_GET                       0x0A

/*! Message to delete template */
#define MCS_FT_TEMPLATE_DELETE                    0x0B

/*! Message to create collector for FT app */
#define MCS_FT_COLLECTOR_CREATE                   0x0C

/*! Message to get collector */
#define MCS_FT_COLLECTOR_GET                      0x0D

/*! Messsage to delete collector */
#define MCS_FT_COLLECTOR_DELETE                   0x0E

/*! Message to send SER event */
#define MCS_FT_SER_EVENT                          0x0F

/*! Message to start the template set transmit */
#define MCS_FT_TEMPLATE_XMIT                      0x10

/*! Message to get template set transmit */
#define MCS_FT_TEMPLATE_XMIT_GET                  0x11 /* Unimplemented */

/*! Message to create elephant profile */
#define MCS_FT_ELPH_PROFILE_CREATE                0x12

/*! Message to get elephant profile */
#define MCS_FT_ELPH_PROFILE_GET                   0x13

/*! Message to delete elephant profile */
#define MCS_FT_ELPH_PROFILE_DELETE                0x14

/*! Message to get learnt stats */
#define MCS_FT_STATS_LEARN_GET                    0x15

/*! Message to get export stats */
#define MCS_FT_STATS_EXPORT_GET                   0x16

/*! Message to get aging stats */
#define MCS_FT_STATS_AGE_GET                      0x17

/*! Message to get elephant profile stats */
#define MCS_FT_STATS_ELPH_GET                     0x18

/*! Message to get features supported by UC */
#define MCS_FT_FEATURES_EXCHANGE                  0x19

/*! Message to create EM group */
#define MCS_FT_EM_GROUP_CREATE                    0x1A

/*! Message to delete EM group */
#define MCS_FT_EM_GROUP_DELETE                    0x1B

/*! Message to set group actions */
#define MCS_FT_GROUP_ACTIONS_SET                  0x1C

/*! Message to update export interval */
#define MCS_FT_EXPORT_INTERVAL_UPDATE             0x1D

/*! Message to get first flow index data */
#define MCS_FT_GET_FIRST_FLOW                     0x1E

/*! Message to get given flow index data */
#define MCS_FT_GET_NEXT_FLOW                      0x1F

/*! Message to get first static flow index data */
#define MCS_FT_GET_FIRST_STATIC_FLOW              0x20

/*! Message to get given static flow index data */
#define MCS_FT_GET_NEXT_STATIC_FLOW               0x21

/*! Message to clear group flow data */
#define MCS_FT_GROUP_FLOW_DATA_SET                0x22

/*! Message to install static flow */
#define MCS_FT_SET_STATIC_FLOW                    0x23

/*! Message to send UFT bank info */
#define MCS_FT_UFT_BANK_INFO                      0x24

/*! Message to send Flex EM group info */
#define MCS_FT_FLEX_EM_GROUP_CREATE               0x25

/*! Message to send FT observation domain update */
#define MCS_FT_OBS_DOM_UPDATE                     0x26

/*! Message to send static flow delete */
#define MCS_FT_DELETE_STATIC_FLOW                 0x27

/*! Message to send hw learn option info */
#define MCS_FT_HW_LEARN_OPT                       0x28

/*! Message to send rxpmd flex fields info */
#define MCS_FT_RXPMD_FLEX_FORMAT_SET              0x29

/*! Message to send match id info */
#define MCS_FT_MATCH_ID_SET                       0x2A


/*! Message subclass indicating that FT EApp is ready */
#define MCS_MSG_SUBCLASS_FT_APPL_READY                                        \
    MCS_FT_APPL_READY

/*! Message subclass to initialize FT EApp */
#define MCS_MSG_SUBCLASS_FT_INIT                                              \
    MCS_MSG_DMA_SET(MCS_FT_INIT)

/*! Reply message subclass to the FT init message */
#define MCS_MSG_SUBCLASS_FT_INIT_REPLY                                        \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_INIT)

/*! Message subclass to update FT config */
#define MCS_MSG_SUBCLASS_FT_OBS_DOM_UPDATE                             \
    MCS_FT_OBS_DOM_UPDATE

/*! Reply message subclass to update FT config message */
#define MCS_MSG_SUBCLASS_FT_OBS_DOM_UPDATE_REPLY                       \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_OBS_DOM_UPDATE)

/*! Message subclass to shutdown FT EApp */
#define MCS_MSG_SUBCLASS_FT_SHUTDOWN                                          \
    MCS_FT_SHUTDOWN

/*! Reply message subclass to FT shutdown message */
#define MCS_MSG_SUBCLASS_FT_SHUTDOWN_REPLY                                    \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_SHUTDOWN)

/*! Message subclass to send EM key format */
#define MCS_MSG_SUBCLASS_FT_EM_KEY_FORMAT                                     \
    MCS_MSG_DMA_SET(MCS_FT_EM_KEY_FORMAT)

/*! Reply message subclass to send EM key format */
#define MCS_MSG_SUBCLASS_FT_EM_KEY_FORMAT_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_EM_KEY_FORMAT)

/*! Message subclass to create flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_CREATE                                      \
    MCS_MSG_DMA_SET(MCS_FT_GROUP_CREATE)

/*! Reply message subclass to create flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_CREATE_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GROUP_CREATE)

/*! Message subclass to get flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_GET                                         \
    MCS_FT_GROUP_GET

/*! Reply message subclass to get flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_GET_REPLY                                   \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_GROUP_GET))

/*! Reply message subclass subclass to delete flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_DELETE                                      \
    MCS_FT_GROUP_DELETE

/*! Replay message subclass to delete flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_DELETE_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GROUP_DELETE)

/*! Message to update flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_UPDATE                                      \
    MCS_MSG_DMA_SET(MCS_FT_GROUP_UPDATE)

/*! Reply message subclass to update flow group */
#define MCS_MSG_SUBCLASS_FT_GROUP_UPDATE_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GROUP_UPDATE)

/*! Message subclass to get group flow data */
#define MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET                               \
    MCS_MSG_DMA_SET(MCS_FT_GROUP_FLOW_DATA_GET)

/*! Reply message subclass to get group flow data */
#define MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_GET_REPLY                         \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_GROUP_FLOW_DATA_GET))

/*! Message subclass to create template */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_CREATE                                    \
    MCS_MSG_DMA_SET(MCS_FT_TEMPLATE_CREATE)

/*! Reply message subclass to create template */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_CREATE_REPLY                              \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_TEMPLATE_CREATE)

/*! Message subclass to get tempalte */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_GET                                        \
    MCS_FT_TEMPLATE_GET

/*! Reply message subclass to get tempalte */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_GET_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_TEMPLATE_GET))

/*! Message subclass to delete template */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_DELETE                                     \
    MCS_FT_TEMPLATE_DELETE

/*! Reply message subclass to delete template */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_DELETE_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_TEMPLATE_DELETE)

/*! Message subclass to create collector for FT app */
#define MCS_MSG_SUBCLASS_FT_COLLECTOR_CREATE                                    \
    MCS_MSG_DMA_SET(MCS_FT_COLLECTOR_CREATE)

/*! Reply message subclass to create collector for FT app */
#define MCS_MSG_SUBCLASS_FT_COLLECTOR_CREATE_REPLY                              \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_COLLECTOR_CREATE)

/*! Message subclass to get collector */
#define MCS_MSG_SUBCLASS_FT_COLLECTOR_GET                                       \
    MCS_FT_COLLECTOR_GET

/*! Reply message subclass to get collector */
#define MCS_MSG_SUBCLASS_FT_COLLECTOR_GET_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_COLLECTOR_GET))

/*! Messsage to delete collector */
#define MCS_MSG_SUBCLASS_FT_COLLECTOR_DELETE                                     \
    MCS_FT_COLLECTOR_DELETE

/*! Messsage to delete collector */
#define MCS_MSG_SUBCLASS_FT_COLLECTOR_DELETE_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_COLLECTOR_DELETE)

/*! Message subclass to send SER event */
#define MCS_MSG_SUBCLASS_FT_SER_EVENT                                            \
    MCS_MSG_DMA_SET(MCS_FT_SER_EVENT)

/*! Reply message subclass to send SER event */
#define MCS_MSG_SUBCLASS_FT_SER_EVENT_REPLY                                      \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_SER_EVENT)

/*! Message subclass to start the template set transmit */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_XMIT                                        \
    MCS_MSG_DMA_SET(MCS_FT_TEMPLATE_XMIT)

/*! Reply message subclass to start the template set transmit */
#define MCS_MSG_SUBCLASS_FT_TEMPLATE_XMIT_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_TEMPLATE_XMIT)

/*! Message subclass to create elephant profile */
#define MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_CREATE                                  \
    MCS_MSG_DMA_SET(MCS_FT_ELPH_PROFILE_CREATE)

/*! Reply message subclass to create elephant profile */
#define MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_CREATE_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_ELPH_PROFILE_CREATE)

/*! Message subclass to get elephant profile */
#define MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_GET                                     \
    MCS_FT_ELPH_PROFILE_GET

/*! Reply message subclass to get elephant profile */
#define MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_GET_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_ELPH_PROFILE_GET))

/*! Message subclass to delete elephant profile */
#define MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_DELETE                                  \
    MCS_FT_ELPH_PROFILE_DELETE

/*! Reply message subclass to delete elephant profile */
#define MCS_MSG_SUBCLASS_FT_ELPH_PROFILE_DELETE_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_ELPH_PROFILE_DELETE)

/*! Message subclass to get learnt stats */
#define MCS_MSG_SUBCLASS_FT_STATS_LEARN_GET                                      \
    MCS_FT_STATS_LEARN_GET

/*! Reply message subclass to get learnt stats */
#define MCS_MSG_SUBCLASS_FT_STATS_LEARN_GET_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_STATS_LEARN_GET))

/*! Message subclass to get export stats */
#define MCS_MSG_SUBCLASS_FT_STATS_EXPORT_GET                                     \
    MCS_FT_STATS_EXPORT_GET

/*! Reply message subclass to get export stats */
#define MCS_MSG_SUBCLASS_FT_STATS_EXPORT_GET_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_STATS_EXPORT_GET))

/*! Message subclass to get aging stats */
#define MCS_MSG_SUBCLASS_FT_STATS_AGE_GET                                        \
    MCS_FT_STATS_AGE_GET

/*! Reply message subclass to get aging stats */
#define MCS_MSG_SUBCLASS_FT_STATS_AGE_GET_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_STATS_AGE_GET))

/*! Message subclass to get elephant profile stats */
#define MCS_MSG_SUBCLASS_FT_STATS_ELPH_GET                                      \
    MCS_FT_STATS_ELPH_GET

/*! Reply message subclass to get elephant profile stats */
#define MCS_MSG_SUBCLASS_FT_STATS_ELPH_GET_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_STATS_ELPH_GET))

/*! Message subclass to get features supported by UC */
#define MCS_MSG_SUBCLASS_FT_FEATURES_EXCHANGE                                    \
    MCS_MSG_DMA_SET(MCS_FT_FEATURES_EXCHANGE)

/*! Reply message subclass to get features supported by UC */
#define MCS_MSG_SUBCLASS_FT_FEATURES_EXCHANGE_REPLY                              \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(MCS_FT_FEATURES_EXCHANGE))

/*! Message subclass to create EM group */
#define MCS_MSG_SUBCLASS_FT_EM_GROUP_CREATE                                      \
    MCS_MSG_DMA_SET(MCS_FT_EM_GROUP_CREATE)

/*! Reply message subclass to create EM group */
#define MCS_MSG_SUBCLASS_FT_EM_GROUP_CREATE_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_EM_GROUP_CREATE)

/*! Message subclass to create flex EM group */
#define MCS_MSG_SUBCLASS_FT_FLEX_EM_GROUP_CREATE                                 \
    MCS_MSG_DMA_SET(MCS_FT_FLEX_EM_GROUP_CREATE)

/*! Reply message subclass to create flex EM group */
#define MCS_MSG_SUBCLASS_FT_FLEX_EM_GROUP_CREATE_REPLY                           \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_FLEX_EM_GROUP_CREATE)

/*! Message subclass to delete flex EM / EM group */
#define MCS_MSG_SUBCLASS_FT_EM_GROUP_DELETE                                      \
    MCS_FT_EM_GROUP_DELETE

/*! Reply message subclass to delete EM group */
#define MCS_MSG_SUBCLASS_FT_EM_GROUP_DELETE_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_EM_GROUP_DELETE)

/*! Message subclass to set group actions */
#define MCS_MSG_SUBCLASS_FT_GROUP_ACTIONS_SET                                    \
    MCS_MSG_DMA_SET(MCS_FT_GROUP_ACTIONS_SET)

/*! Reply message subclass to set group actions */
#define MCS_MSG_SUBCLASS_FT_GROUP_ACTIONS_SET_REPLY                              \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GROUP_ACTIONS_SET)

/*! Message subclass to update export interval */
#define MCS_MSG_SUBCLASS_FT_EXPORT_INTERVAL_UPDATE                               \
    (MCS_FT_EXPORT_INTERVAL_UPDATE)

/*! Reply message subclass to update export interval */
#define MCS_MSG_SUBCLASS_FT_EXPORT_INTERVAL_UPDATE_REPLY                         \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_EXPORT_INTERVAL_UPDATE)

/*! Message subclass to get first flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_FIRST_FLOW                                       \
    (MCS_FT_GET_FIRST_FLOW)

/*! Reply message subclass to get first flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_FIRST_FLOW_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GET_FIRST_FLOW)

/*! Message subclass to get given flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_NEXT_FLOW                                        \
    (MCS_FT_GET_NEXT_FLOW)

/*! Reply message subclass to get given flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_NEXT_FLOW_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GET_NEXT_FLOW)

/*! Message subclass to get first static flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_FIRST_STATIC_FLOW                                \
    (MCS_FT_GET_FIRST_STATIC_FLOW)

/*! Reply message subclass to get first static flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_FIRST_STATIC_FLOW_REPLY                          \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GET_FIRST_STATIC_FLOW)

/*! Message subclass to get given static flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_NEXT_STATIC_FLOW                                 \
    (MCS_FT_GET_NEXT_STATIC_FLOW)

/*! Reply message subclass to get given static flow index data */
#define MCS_MSG_SUBCLASS_FT_GET_NEXT_STATIC_FLOW_REPLY                           \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GET_NEXT_STATIC_FLOW)

/*! Message subclass to install static flow */
#define MCS_MSG_SUBCLASS_FT_SET_STATIC_FLOW                                      \
    MCS_MSG_DMA_SET(MCS_FT_SET_STATIC_FLOW)

/*! Reply message subclass to install static flow */
#define MCS_MSG_SUBCLASS_FT_SET_STATIC_FLOW_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_SET_STATIC_FLOW)

/*! Message subclass to clear group flow data */
#define MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_SET                                  \
    MCS_MSG_DMA_SET(MCS_FT_GROUP_FLOW_DATA_SET)

/*! Reply message subclass to clear group flow data */
#define MCS_MSG_SUBCLASS_FT_GROUP_FLOW_DATA_SET_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_GROUP_FLOW_DATA_SET)

/*! Message subclass to send UFT bank info */
#define MCS_MSG_SUBCLASS_FT_UFT_BANK_INFO                                        \
    MCS_MSG_DMA_SET(MCS_FT_UFT_BANK_INFO)

/*! Reply message subclass to send UFT bank info */
#define MCS_MSG_SUBCLASS_FT_UFT_BANK_INFO_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_UFT_BANK_INFO)

/*! Message subclass to send static flow delete */
#define MCS_MSG_SUBCLASS_FT_DELETE_STATIC_FLOW                                   \
    MCS_MSG_DMA_SET(MCS_FT_DELETE_STATIC_FLOW)

/*! Reply message subclass for static flow delete */
#define MCS_MSG_SUBCLASS_FT_DELETE_STATIC_FLOW_REPLY                             \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_DELETE_STATIC_FLOW)

/*! Message subclass to send hw learn option info */
#define MCS_MSG_SUBCLASS_FT_HW_LEARN_OPT                                         \
    MCS_MSG_DMA_SET(MCS_FT_HW_LEARN_OPT)

/*! Reply message subclass for hw learn option info */
#define MCS_MSG_SUBCLASS_FT_HW_LEARN_OPT_REPLY                                   \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_HW_LEARN_OPT)

/*! Message subclass to send rxpmd fields info */
#define MCS_MSG_SUBCLASS_FT_RXPMD_FLEX_FORMAT_SET                                \
    MCS_MSG_DMA_SET(MCS_FT_RXPMD_FLEX_FORMAT_SET)

/*! Reply message subclass for rxpmd fields info */
#define MCS_MSG_SUBCLASS_FT_RXPMD_FLEX_FORMAT_SET_REPLY                          \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_RXPMD_FLEX_FORMAT_SET)

/*! Message subclass to send match id info */
#define MCS_MSG_SUBCLASS_FT_MATCH_ID_SET                                         \
    MCS_MSG_DMA_SET(MCS_FT_MATCH_ID_SET)

/*! Reply message subclass for rxpmd fields info */
#define MCS_MSG_SUBCLASS_FT_MATCH_ID_SET_REPLY                                   \
    MCS_MSG_SUBCLASS_REPLY(MCS_FT_MATCH_ID_SET)

/*! IFA Embedded Application messaging class */
#define MCS_MSG_CLASS_IFA                4

/*! Message indicating that IFA EApp is ready */
#define IFA_APPL_READY                         0x00
/*! Message to initialize the IFA EApp */
#define IFA_INIT                               0x01
/*! Message to shut down the IFA EApp */
#define IFA_SHUTDOWN                           0x02
/*! Message to send information about the format of the flex fields in Rx PMD
 * required by the IFA EApp.
 */
#define IFA_PKT_FLEX_FORMAT_SET                0x03
/*! Message to set the IFA FIFO format. */
#define IFA_FIFO_FORMAT_SET                    0x04
/*! Message to set the IFA metadata format */
#define IFA_MD_FORMAT_SET                      0x05
/*! Message to update an IFA configuration */
#define IFA_CONFIG_UPDATE                      0x06
/*! Message to create a collector for IFA EApp */
#define IFA_COLLECTOR_CREATE                   0x07
/*! Message to delete a collector for IFA EApp */
#define IFA_COLLECTOR_DELETE                   0x08
/*! Message to set the IFA statistics */
#define IFA_STATS_SET                          0x09
/*! Message to get the IFA statistics */
#define IFA_STATS_GET                          0x0A
/*! Message to set the IFA IPFIX template. */
#define IFA_IPFIX_TEMPLATE_SET                 0x0B

/*! Message subclass indicating that IFA EApp is ready */
#define MCS_MSG_SUBCLASS_IFA_APPL_READY                                      \
    IFA_APPL_READY

/*! Message subclass to initialize the IFA EApp */
#define MCS_MSG_SUBCLASS_IFA_INIT                                            \
    MCS_MSG_DMA_SET(IFA_INIT)

/*! Reply message subclass to the IFA init message */
#define MCS_MSG_SUBCLASS_IFA_INIT_REPLY                                      \
    MCS_MSG_SUBCLASS_REPLY(IFA_INIT)

/*! Message subclass to shut down the IFA EApp */
#define MCS_MSG_SUBCLASS_IFA_SHUTDOWN                                        \
    IFA_SHUTDOWN

/*! Reply message subclass to IFA shutdown message */
#define MCS_MSG_SUBCLASS_IFA_SHUTDOWN_REPLY                                  \
    MCS_MSG_SUBCLASS_REPLY(IFA_SHUTDOWN)

/*! Message subclass to send the flex format to IFA EApp. */
#define MCS_MSG_SUBCLASS_IFA_RXPMD_FLEX_FORMAT_SET                           \
    MCS_MSG_DMA_SET(IFA_PKT_FLEX_FORMAT_SET)

/*! Reply message subclass to IFA pkt flex format set. */
#define MCS_MSG_SUBCLASS_IFA_RXPMD_FLEX_FORMAT_SET_REPLY                     \
    MCS_MSG_SUBCLASS_REPLY(IFA_PKT_FLEX_FORMAT_SET)

/*! Message subclass to set the IFA FIFO format. */
#define MCS_MSG_SUBCLASS_IFA_FIFO_FORMAT_SET                                 \
    MCS_MSG_DMA_SET(IFA_FIFO_FORMAT_SET)

/*! Reply message subclass to IFA FIFO format set. */
#define MCS_MSG_SUBCLASS_IFA_FIFO_FORMAT_SET_REPLY                           \
    MCS_MSG_SUBCLASS_REPLY(IFA_FIFO_FORMAT_SET)

/*! Message subclass to set the IFA metadata format */
#define MCS_MSG_SUBCLASS_IFA_MD_FORMAT_SET                                   \
    MCS_MSG_DMA_SET(IFA_MD_FORMAT_SET)

/*! Reply message subclass to IFA MD format set message */
#define MCS_MSG_SUBCLASS_IFA_MD_FORMAT_SET_REPLY                             \
    MCS_MSG_SUBCLASS_REPLY(IFA_MD_FORMAT_SET)

/*! Message subclass to update an IFA configuration */
#define MCS_MSG_SUBCLASS_IFA_CONFIG_UPDATE                                   \
    MCS_MSG_DMA_SET(IFA_CONFIG_UPDATE)

/*! Reply message subclass to IFA config update */
#define MCS_MSG_SUBCLASS_IFA_CONFIG_UPDATE_REPLY                             \
    MCS_MSG_SUBCLASS_REPLY(IFA_CONFIG_UPDATE)

/*! Message subclass to create a collector for IFA EApp */
#define MCS_MSG_SUBCLASS_IFA_COLLECTOR_CREATE                                \
    MCS_MSG_DMA_SET(IFA_COLLECTOR_CREATE)

/*! Reply message subclass to IFA collector create message */
#define MCS_MSG_SUBCLASS_IFA_COLLECTOR_CREATE_REPLY                          \
    MCS_MSG_SUBCLASS_REPLY(IFA_COLLECTOR_CREATE)

/*! Message subclass to delete a collector for IFA EApp */
#define MCS_MSG_SUBCLASS_IFA_COLLECTOR_DELETE                                \
    IFA_COLLECTOR_DELETE

/*! Reply message subclass to IFA collector delete message */
#define MCS_MSG_SUBCLASS_IFA_COLLECTOR_DELETE_REPLY                          \
    MCS_MSG_SUBCLASS_REPLY(IFA_COLLECTOR_DELETE)

/*! Message subclass to set the IFA statistics */
#define MCS_MSG_SUBCLASS_IFA_STATS_SET                                       \
    MCS_MSG_DMA_SET(IFA_STATS_SET)

/*! Reply message subclass to IFA stats set message */
#define MCS_MSG_SUBCLASS_IFA_STATS_SET_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(IFA_STATS_SET)

/*! Message subclass to get the IFA statistics */
#define MCS_MSG_SUBCLASS_IFA_STATS_GET                                       \
    IFA_STATS_GET

/*! Reply message subclass to IFA stat get message */
#define MCS_MSG_SUBCLASS_IFA_STATS_GET_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(IFA_STATS_GET))

/*! Message subclass to set the IFA statistics */
#define MCS_MSG_SUBCLASS_IFA_IPFIX_TEMPLATE_SET                              \
    MCS_MSG_DMA_SET(IFA_IPFIX_TEMPLATE_SET)

/*! Reply message subclass to IFA stats set message */
#define MCS_MSG_SUBCLASS_IFA_IPFIX_TEMPLATE_SET_REPLY                        \
    MCS_MSG_SUBCLASS_REPLY(IFA_IPFIX_TEMPLATE_SET)


/*! BFD Embedded Application messaging class. */
#define MCS_MSG_CLASS_BFD                5

/*! Message indicating that BFD EApp is ready. */
#define BFD_APPL_READY                       0x00
/*! Message to initialize the BFD EApp. */
#define BFD_INIT                             0x01
/*! Message to uninitialize the BFD EApp. */
#define BFD_UNINIT                           0x02
/*! Message to set a BFD endpoint. */
#define BFD_EP_SET                           0x03
/*! Message to get the status of a BFD endpoint. */
#define BFD_EP_STATUS_GET                    0x04
/*! Message to delete a BFD endpoint. */
#define BFD_EP_DELETE                        0x05
/*! Message to set BFD simple password authentication. */
#define BFD_AUTH_SP_SET                      0x06
/*! Message to set BFD SHA1 authentication. */
#define BFD_AUTH_SHA1_SET                    0x07
/*! Message to set statistics of a BFD endpoint. */
#define BFD_EP_STATS_SET                     0x08
/*! Message to get statistics of a BFD endpoint. */
#define BFD_EP_STATS_GET                     0x09
/*! Message to initiate poll sequence on a BFD endpoint. */
#define BFD_EP_POLL                          0x0A
/*! Message to set the  BFD event mask. */
#define BFD_EVENT_MASK_SET                   0x0B
/*! Message to set the BFD global statistics. */
#define BFD_GLOBAL_STATS_SET                 0x0C
/*! Message to get the BFD global statistics. */
#define BFD_GLOBAL_STATS_GET                 0x0D
/*! Message to enable BFD debug trace logging. */
#define BFD_TRACE_LOG_ENABLE                 0x0E
/*! Message to set BFD global parameters. */
#define BFD_GLOBAL_PARAM_SET                 0x0F
/*!
 * Message to start/stop incrementing of BFD endpoint authentication sequence
 * number.
 */
#define BFD_EP_AUTH_SEQ_NUM_INCR             0x10
/*! Message to send information about the format of the flex fields in Rx PMD
 * required by the BFD EApp.
 */
#define BFD_PKT_FLEX_FORMAT_SET              0x11
/*! Message to set the match ID information. */
#define BFD_MATCH_ID_SET                     0x12

/*! Message subclass indicating that BFD EApp is ready. */
#define MCS_MSG_SUBCLASS_BFD_APPL_READY                                        \
    BFD_APPL_READY

/*! Message subclass to initialize the BFD EApp. */
#define MCS_MSG_SUBCLASS_BFD_INIT                                              \
    MCS_MSG_DMA_SET(BFD_INIT)

/*! Reply message subclass to the BFD init message. */
#define MCS_MSG_SUBCLASS_BFD_INIT_REPLY                                        \
    MCS_MSG_SUBCLASS_REPLY(BFD_INIT)

/*! Message subclass to unitialize the BFD EApp. */
#define MCS_MSG_SUBCLASS_BFD_UNINIT                                            \
    BFD_UNINIT

/*! Reply message subclass to the BFD uninit message. */
#define MCS_MSG_SUBCLASS_BFD_UNINIT_REPLY                                       \
    MCS_MSG_SUBCLASS_REPLY(BFD_UNINIT)

/*! Message subclass to set a BFD endpoint. */
#define MCS_MSG_SUBCLASS_BFD_EP_SET                                            \
    MCS_MSG_DMA_SET(BFD_EP_SET)

/*! Reply message subclass to the BFD endpoint set message. */
#define MCS_MSG_SUBCLASS_BFD_EP_SET_REPLY                                      \
    MCS_MSG_SUBCLASS_REPLY(BFD_EP_SET)

/*! Message subclass to get the status of BFD endpoint. */
#define MCS_MSG_SUBCLASS_BFD_EP_STATUS_GET                                     \
    BFD_EP_STATUS_GET

/*! Reply message subclass to the BFD endpoint status get message. */
#define MCS_MSG_SUBCLASS_BFD_EP_STATUS_GET_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(BFD_EP_STATUS_GET))

/*! Message subclass to set the statistics of a BFD endpoint. */
#define MCS_MSG_SUBCLASS_BFD_EP_STATS_SET                                      \
    MCS_MSG_DMA_SET(BFD_EP_STATS_SET)

/*! Reply message subclass to the set BFD endpoint statistics message. */
#define MCS_MSG_SUBCLASS_BFD_EP_STATS_SET_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(BFD_EP_STATS_SET)

/*! Message subclass to get the statistics of a BFD endpoint. */
#define MCS_MSG_SUBCLASS_BFD_EP_STATS_GET                                      \
    BFD_EP_STATS_GET

/*! Reply message subclass to the get BFD endpoint statistics message. */
#define MCS_MSG_SUBCLASS_BFD_EP_STATS_GET_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(BFD_EP_STATS_GET))

/*! Message subclass to delete a BFD endpoint. */
#define MCS_MSG_SUBCLASS_BFD_EP_DELETE                                         \
    BFD_EP_DELETE

/*! Reply message subclass to the BFD endpoint delete message. */
#define MCS_MSG_SUBCLASS_BFD_EP_DELETE_REPLY                                   \
    MCS_MSG_SUBCLASS_REPLY(BFD_EP_DELETE)

/*! Message subclass to set BFD simple password authentication. */
#define MCS_MSG_SUBCLASS_BFD_AUTH_SP_SET                                       \
    MCS_MSG_DMA_SET(BFD_AUTH_SP_SET)

/*! Reply message subclass to the BFD simple password authentication message. */
#define MCS_MSG_SUBCLASS_BFD_AUTH_SP_SET_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(BFD_AUTH_SP_SET)

/*! Message subclass to set BFD SHA1 authentication. */
#define MCS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET                                     \
    MCS_MSG_DMA_SET(BFD_AUTH_SHA1_SET)

/*! Reply message subclass to the BFD SHA1 authentication message. */
#define MCS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET_REPLY                               \
    MCS_MSG_SUBCLASS_REPLY(BFD_AUTH_SHA1_SET)

/*! Message subclass to set a BFD endpoint statistics. */
#define MCS_MSG_SUBCLASS_BFD_EP_STAT_SET                                       \
    MCS_MSG_DMA_SET(BFD_EP_STAT_SET)

/*! Reply message subclass to the BFD endpoint statistics set message. */
#define MCS_MSG_SUBCLASS_BFD_EP_STAT_SET_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(BFD_EP_STAT_SET)

/*! Message subclass to get a BFD endpoint statistics. */
#define MCS_MSG_SUBCLASS_BFD_EP_STAT_GET                                       \
    BFD_EP_STAT_GET

/*! Reply message subclass to the BFD endpoint statistics get message. */
#define MCS_MSG_SUBCLASS_BFD_EP_STAT_GET_REPLY                                 \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(BFD_EP_STAT_GET))

/*! Message subclass to initiate poll sequence on a BFD endpoint. */
#define MCS_MSG_SUBCLASS_BFD_EP_POLL                                           \
    BFD_EP_POLL

/*! Reply message subclass to the BFD endpoint poll message. */
#define MCS_MSG_SUBCLASS_BFD_EP_POLL_REPLY                                     \
    MCS_MSG_SUBCLASS_REPLY(BFD_EP_POLL)

/*! Message subclass to set BFD event mask. */
#define MCS_MSG_SUBCLASS_BFD_EVENT_MASK_SET                                    \
    BFD_EVENT_MASK_SET

/*! Reply message subclass to the BFD event mask set message. */
#define MCS_MSG_SUBCLASS_BFD_EVENT_MASK_SET_REPLY                              \
    MCS_MSG_SUBCLASS_REPLY(BFD_EVENT_MASK_SET)

/*! Message subclass to set BFD global statistics. */
#define MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_SET                                  \
    MCS_MSG_DMA_SET(BFD_GLOBAL_STATS_SET)

/*! Reply message subclass to the BFD global statistics set message. */
#define MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_SET_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(BFD_GLOBAL_STATS_SET)

/*! Message subclass to get BFD global statistics. */
#define MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_GET                                  \
    BFD_GLOBAL_STATS_GET

/*! Reply message subclass to the BFD global statistics get message. */
#define MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_GET_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(BFD_GLOBAL_STATS_GET))

/*! Message subclass to enable BFD debug trace log. */
#define MCS_MSG_SUBCLASS_BFD_TRACE_LOG_ENABLE                                  \
    BFD_TRACE_LOG_ENABLE

/*! Reply message subclass to BFD debug trace log enable message. */
#define MCS_MSG_SUBCLASS_BFD_TRACE_LOG_ENABLE_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(MCS_MSG_DMA_SET(BFD_TRACE_LOG_ENABLE))

/*! Message subclass to set BFD global parameters. */
#define MCS_MSG_SUBCLASS_BFD_GLOBAL_PARAM_SET                                  \
    MCS_MSG_DMA_SET(BFD_GLOBAL_PARAM_SET)

/*! Reply message subclass to BFD global parameter set message. */
#define MCS_MSG_SUBCLASS_BFD_GLOBAL_PARAM_SET_REPLY                            \
    MCS_MSG_SUBCLASS_REPLY(BFD_GLOBAL_PARAM_SET)

/*!
 * Message subclass to start/stop incrementing of authentication sequence
 * number for a BFD endpoint.
 */
#define MCS_MSG_SUBCLASS_BFD_EP_AUTH_SEQ_NUM_INCR                              \
    BFD_EP_AUTH_SEQ_NUM_INCR

/*!
 * Reply message subclass to BFD endpoint authentication sequence number
 * increment message.
 */
#define MCS_MSG_SUBCLASS_BFD_EP_AUTH_SEQ_NUM_INCR_REPLY                        \
    MCS_MSG_SUBCLASS_REPLY(BFD_EP_AUTH_SEQ_NUM_INCR)

/*! Message subclass to send the flex format to BFD EApp. */
#define MCS_MSG_SUBCLASS_BFD_RXPMD_FLEX_FORMAT_SET                             \
    MCS_MSG_DMA_SET(BFD_PKT_FLEX_FORMAT_SET)

/*! Reply message subclass to BFD pkt flex format set. */
#define MCS_MSG_SUBCLASS_BFD_RXPMD_FLEX_FORMAT_SET_REPLY                       \
    MCS_MSG_SUBCLASS_REPLY(BFD_PKT_FLEX_FORMAT_SET)

/*! Message subclass to send the match ID format to BFD EApp. */
#define MCS_MSG_SUBCLASS_BFD_MATCH_ID_SET                                      \
    MCS_MSG_DMA_SET(BFD_MATCH_ID_SET)

/*! Reply message subclass to the BFD match ID set message. */
#define MCS_MSG_SUBCLASS_BFD_MATCH_ID_SET_REPLY                                \
    MCS_MSG_SUBCLASS_REPLY(BFD_MATCH_ID_SET)

/*! BFD event messaging class. */
#define MCS_MSG_CLASS_BFD_EVENT       6

/* MAX_MCS_MSG_CLASS should == the value of the highest MCS_MSG_CLASS */

/*! Maximum number of MCS Message Classes */
#define MAX_MCS_MSG_CLASS                6


/*! Message area per processor pair/direction */
typedef struct {
    /*! MCS Message status */
    mcs_msg_host_status_t status;
    /*! MCS Message data */
    mcs_msg_data_t data[NUM_MCS_MSG_SLOTS];
} mcs_msg_area_t;

/*!
 * \brief MCS eApps list implemented in uKernel.
 * To Identify App by enum instead of string.
 * All of them may not be implemented for any given chip.
 */
typedef enum {
    /*! Unknown App. Probably not uKernel. */
    MCS_APP_UNKNOWN     = 0,
    /*! Skeletal uKernel eApp. */
    MCS_APP_NOAPP       = 1,
    /*! Tx beacon */
    MCS_APP_TXBCN       = 2,
    /*! BFD */
    MCS_APP_BFD         = 3,
    /*! Flow Tracker */
    MCS_APP_FT          = 4,
    /*! BroadSync */
    MCS_APP_BS          = 5,
    /*! Streaming Telemetry */
    MCS_APP_ST          = 6,
    /*! IFA */
    MCS_APP_IFA         = 7,
    /*! IFA v2 */
    MCS_APP_IFA2        = 8,
    /*! Queue Congestion Management */
    MCS_APP_QCM         = 9,
    /*! INT */
    MCS_APP_INT         = 10,
    /*! SUM */
    MCS_APP_SUM         = 11,
    /*! IMS */
    MCS_APP_IMS         = 12,
    /*! TDPLL */
    MCS_APP_TDPLL       = 13,
    /*! PTPPP */
    MCS_APP_PTPPP       = 14,
    /*! PTPM */
    MCS_APP_PTPM        = 15,
    /*! PTPFULL */
    MCS_APP_PTPFULL     = 16,
    /*! KNETSYNC */
    MCS_APP_KNETSYNC    = 17,
    /*! MoD */
    MCS_APP_MOD         = 18,
    /*! Total number of eApps */
    MCS_APP_COUNT
} mcs_app_type_t;

#endif /* MCS_MSG_COMMON_H */

