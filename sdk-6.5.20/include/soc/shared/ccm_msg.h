/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ccm_msg.h
 * Purpose: CCM Messages between SDK and uKernel..
 */

#ifndef   _SOC_SHARED_CCM_MSG_H_
#define   _SOC_SHARED_CCM_MSG_H_

#include "soc/shared/ccm.h"
#include <shared/pack.h>

#ifdef BCM_UKERNEL
/* Build for uKernel */
#include "sdk_typedefs.h"
#else
/* Build for UKERNEL */
#include <sal/types.h>
#include <bcm/types.h>

#endif


#define _UC_MSG_TIMEOUT_USECS   5000000 /* UC Msg timeout (5 Sec) */

/* uController Error codes */
typedef enum shr_uc_error_e {
    SHR_UC_E_NONE = 0,
    SHR_UC_E_INTERNAL,
    SHR_UC_E_MEMORY,
    SHR_UC_E_PARAM,
    SHR_UC_E_RESOURCE,
    SHR_UC_E_EXISTS,
    SHR_UC_E_NOT_FOUND,
    SHR_UC_E_UNAVAIL,
    SHR_UC_E_VERSION,
    SHR_UC_E_INIT
} shr_uc_error_t;

#define _CCM_OLP_HEADER_TYPE_RX 0x01 /* _BCM_TD2P_OLP_HDR_TYPE_RX */

/* _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM */
#define _CCM_RX_HEADER_SUBTYPE  0x01
/* _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP */
#define _CCM_RX_UP_MEP_HEADER_SUBTYPE 0x11

#define _CCM_ETHER_TYPE         0x8902 /* CFM Ether type */

#define _CCM_UC_NUM          0   /* UC Core to run CCM */
#define _CCM_RX_CHANNEL_ID   1   /* Channel for CCM Rx */


/* Change this with config property */
#define _CCM_GROUP_COUNT        256
#define _CCM_MEP_COUNT          256

#define _CCM_MAX_GROUPS         1024
#define _CCM_MAX_MEPS           1024
#define _CCM_MEP_BIT_FIELD_LEN  _CCM_MAX_MEPS/32
#define _CCM_GRP_BIT_FILED_LEN  _CCM_MAX_GROUPS/32


#define _CCM_SDK_VERSION     1   /* CCM EApp Version */
#define _CCM_UC_MIN_VERSION  1   /* Minimum version expected from CCM EApp */


/* Group Create/Update Msg Flags */
#define _GRP_REPLACE            (1 << 0)
#define _GRP_REPLACE_SW_RDI     (1 << 1)
#define _GRP_REPLACE_LOWEST_ALARM_PRIO  (1 << 2)
#define _GRP_GET_FAULTS_ONLY    (1 << 3) /* Get Faults only */


/* OAM EP Create/Update Msg Flags */
#define _MEP_REPLACE            (1 << 0) /* Replace existing EP */
#define _MEP_OLP_RECREATE       (1 << 1) /* Replace existing EP */
#define _MEP_L2_ENCAP_RECREATE  (1 << 2) /* Replace existing EP */
#define _REPLACE_INTF_STATUS    (1 << 3) /* Replace Intf status */
#define _REPLACE_PORT_STATUS    (1 << 4) /* Replace Port Status */
#define _REPLACE_TRUNK_INDEX    (1 << 5) /* Replace Trunk Idx */
#define _REPLACE_CTRS           (1 << 6) /* Replace Counter Idx/Action */
#define _MEP_GET_FAULTS_ONLY    (1 << 7) /* Get Faults only */
#define _MEP_GET_CLR_P_FAULTS   (1 << 8) /* Clear persistent faults on GET */


/* Set a bit corresponding to index in 32 bit wide bifield array */
#define _SET_BIT_FLD(bitmap, index) ((bitmap)[(index)/32] |= (1 << (index)%32))

/* Clear a bit corresponding to index in 32 bit wide bifield array */
#define _CLR_BIT_FLD(bitmap, index) ((bitmap)[(index)/32] &= ~(1 << (index)%32))

#define _GET_BIT_FLD(bitmap, index) (((bitmap)[index/32] & (1 << index%32)) ? 1 : 0)

/* CCM Events */
/* Group Defect Priorities */
#define G_RDI_PRI           1
#define G_CCM_MAC_PRI       2
#define G_CCM_TIMEOUT_PRI   3
#define G_CCM_ERROR_PRI     4
#define G_XCON_PRI          5

/* Group Events */
#define G_RDI           (1 << 0) /* bcmOAMEventGroupRemote */
#define G_CCM_MAC       (1 << 1) /* bcmOAMEventGroupMACStatus */
#define G_CCM_TIMEOUT   (1 << 2) /* bcmOAMEventGroupCCMTimeout */
#define G_CCM_ERROR     (1 << 3) /* bcmOAMEventGroupCCMError */
#define G_XCON          (1 << 4) /* bcmOAMEventGroupCCMxcon */

#define G_RDI_CLR       (1 << 5) /* bcmOAMEventGroupRemoteClear */
#define G_CCM_MAC_CLR   (1 << 6) /* bcmOAMEventGroupMACStatusClear */
#define G_CCM_TIME_IN   (1 << 7) /* bcmOAMEventGroupCCMTimeoutClear */
#define G_CCM_ERROR_CLR (1 << 8) /* bcmOAMEventGroupCCMErrorClear */
#define G_XCON_CLR      (1 << 9) /* bcmOAMEventGroupCCMxconClear */

/* Reserving 2 bits for any additional Group events */
#define G_EVENT_MASK    (0xFFF)  /* LSB 12 bits represent Group Events */


/* Endpoint Events */
#define E_PORT_DOWN     (1 << 12) /* bcmOAMEventEndpointPortDown */
#define E_PORT_UP       (1 << 13) /* bcmOAMEventEndpointPortUp */

#define E_INTF_UP       (1 << 14) /* bcmOAMEventEndpointInterfaceUp */
#define E_INTF_DOWN     (1 << 15) /* bcmOAMEventEndpointInterfaceDown */
#define E_INTF_TEST     (1 << 16) /* bcmOAMEventEndpointInterfaceTesting */
#define E_INTF_UK       (1 << 17) /* bcmOAMEventEndpointInterfaceUnkonwn */
#define E_INTF_DOR      (1 << 18) /* bcmOAMEventEndpointInterfaceDormant */
#define E_INTF_NP       (1 << 19) /* bcmOAMEventEndpointInterfaceNotPresent */
#define E_INTF_LL       (1 << 20) /* bcmOAMEventEndpointInterfaceLLDown */

#define E_CCM_TIMEOUT   (1 << 21) /* bcmOAMEventEndpointCCMTimeout) */
#define E_CCM_TIME_IN   (1 << 22) /* bcmOAMEventEndpointCCMTimein) */
#define E_CCM_RDI       (1 << 23) /* bcmOAMEventEndpointRemote */
#define E_CCM_RDI_CLR   (1 << 24) /* bcmOAMEventEndpointRemoteUp */

#define E_EVENT_MASK    (0xFFFFF << 12) /* MSB 20 bits represent Group Events */


/* CCM Endpoint data */
typedef struct ccm_mep_data_s {

    uint32      flags;          /* Enpoint Flags */

    int         ep_id;          /* Endpoint ID */
    int         group_id;       /* Group ID */

    bcm_mac_t   da;             /* DMAC */
    bcm_mac_t   sa;             /* SMAC */
    uint8       type;           /* MEP Type (Port+C+S, P+C, P+S, P) */
    uint8       level;          /* Maintenance domain level */
    uint16      name;           /* Endpoint name */
    uint32      period;         /* CCM interval encoding */

    uint16      vlan;           /* S Vlan */
    uint16      inner_vlan;     /* C Vlan when double tagged*/
    uint8       pkt_pri;        /* Outer vlan Pri */
    uint8       inner_vlan_pri; /* Inner vlan Pri */
    uint16      tpid;           /* Outer vlan TPID */
    uint16      inner_tpid;     /* Inner vlan TPID */

    uint32      gport;          /* Sw Generic Port */
    int         sglp;           /* Hw Src GLP CCM Rx port/trunk */
    int         dglp;           /* Hw Dest GLP for CCM Tx */
    uint32      trunk_index;    /* Trunk Port index */

    uint32      faults;         /* Faults info */
    uint32      persistent_faults; /* Persistent faults */

    uint8       port_state;     /* Port Status TLV Value */
    uint8       intf_state;     /* Interface Status TLV Value */

    uint8       num_ctrs;                           /* Num of Ctrs to incr */
    uint8       counter_offset[_FP_OAM_MAX_CTRS];   /* Counter offset */
    uint32      flex_stat_id[_FP_OAM_MAX_CTRS];     /* Flex Stat Id */

} ccm_mep_data_t;


/* CCM Group data */
typedef struct shr_ccm_group_data_s {
    uint32      flags;                          /* Group Flags */
    int         group_id;                       /* Group id */
    int         lmep_id;                        /* LMEP IDx */
    /* Alarms are generated for priority greater than or equal to this */
    uint32      lowest_alarm_priority;
    uint32      faults;                         /* Group Faults */
    uint32      persistent_faults;              /* Persistent faults */
    uint8       name[_FP_OAM_MAID_LEN];         /* Group name */
    uint32      rmep_bitmap[_CCM_MEP_BIT_FIELD_LEN];/* Group RMEP List */
} ccm_group_data_t;


typedef struct shr_ccm_msg_ctrl_init_s {
    uint32 max_groups;
    uint32 max_meps;
    uint32 rx_channel;
} shr_ccm_msg_ctrl_init_t;


typedef struct shr_ccm_msg_ctrl_mep_data_s {
    uint32 flags;               /* This can be used to indicate if there are
                                    any mods to message structure in future */
    ccm_mep_data_t mep_data;    /* MEP Data */
    uint32 l2_encap_len;        /* L2 Encap Len */
    uint8 L2_encap[_FP_OAM_CCM_L2_ENCAP_LEN_MAX]; /* L2 encap data */
    uint8 OLP_Tx_Hdr[_FP_OAM_OLP_TX_LEN]; /* OLP L2 Hdr */
} shr_ccm_msg_ctrl_mep_data_t;


typedef struct shr_ccm_msg_ctrl_group_data_s {
    uint32      flags;
    ccm_group_data_t group_data;
} shr_ccm_msg_ctrl_group_data_t;


typedef struct shr_ccm_msg_ctrl_app_data_s {
    uint32      flags;
    uint32      group_bitmap[_CCM_GRP_BIT_FILED_LEN];
} shr_ccm_msg_ctrl_app_data_t;

#define MAX_PORTS 137 /* 137 is the current max number of ports supported in Tomahawk.
                       * Other devices has lesser MAX values than them */
#define CCM_INVALID_TRUNK_NUM (0xFFFF) /* Max value of uint16_t */

typedef struct shr_ccm_msg_ctrl_port_trunk_upd_s {
    uint16 num_ports;
    uint16 trunk_id;
    uint16 list_of_ports[MAX_PORTS];
} shr_ccm_msg_ctrl_port_trunk_upd_t;

typedef union shr_ccm_msg_ctrl_s {
    shr_ccm_msg_ctrl_init_t init_msg;
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    shr_ccm_msg_ctrl_group_data_t group_msg;
    shr_ccm_msg_ctrl_app_data_t app_data_msg;
    shr_ccm_msg_ctrl_port_trunk_upd_t port_trunk_upd_msg;
} shr_ccm_msg_ctrl_t;


extern uint8 *
shr_ccm_msg_ctrl_init_pack(uint8 *buf, shr_ccm_msg_ctrl_init_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_init_unpack(uint8 *buf, shr_ccm_msg_ctrl_init_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_mep_set_pack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_mep_set_unpack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_mep_get_pack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_mep_get_unpack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_group_set_pack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_group_set_unpack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_group_get_pack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_group_get_unpack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_app_data_pack(uint8 *buf, shr_ccm_msg_ctrl_app_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_app_data_unpack(uint8 *buf, shr_ccm_msg_ctrl_app_data_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_port_trunk_upd_pack(uint8 *buf, shr_ccm_msg_ctrl_port_trunk_upd_t *msg);

extern uint8 *
shr_ccm_msg_ctrl_port_trunk_upd_unpack(uint8 *buf, shr_ccm_msg_ctrl_port_trunk_upd_t *msg);

#endif
