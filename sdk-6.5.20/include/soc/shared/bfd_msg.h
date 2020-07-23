/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bfd_msg.h
 * Purpose: BFD Messages definitions common to SDK and uKernel.
 *
 *          Messages between SDK and uKernel.
 */

#ifndef   _SOC_SHARED_BFD_MSG_H_
#define   _SOC_SHARED_BFD_MSG_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <soc/shared/mos_msg_common.h>
#include <soc/shared/bfd_pkt.h>
#include <shared/bfd.h>


/********************************
 * BFD Control Messages
 *
 *   SDK Host <--> uController
 */


/*
 * BFD Encapsulation data
 *
 * The BFD Encapsulation is built in the SDK for each session
 * and stored in the uKernel.  It does NOT include the
 * BFD Control Packet (this is built in the uKernel side).
 *
 * The BFD Encapsulation maximum length SHR_BFD_MAX_ENCAP_LENGTH
 * is calculated using the largest possible encapsulation type size.
 *
 *   HG2 + L2 + L2-Overhead + IPv6 + GRE + IPv6 + UDP
 *   16  + 14 + 36          + 40   + 8   + 40   + 8    =  162 bytes
 *
 * NOTE:
 * L2-Overhead
 *   This value is calculated from the difference between the
 *   latest L2 Ethernet frame and the original L2 Ethernet frame sizes:
 *         1536 - 1500 = 36
 *
 * The Encapsulation length does NOT include BFD Control packet or CRC.

 * The final packet length will add
 *     + BFD + [BFD_AUTH] + FCS
 *     + 24  + [28      ] + 4  = 228
 *
 *
 * See following definitions for a given 'BFD-x' session encapsulation:
 *
 *  |------------ 'encap_length-x'--------|
 *  |
 *  +-------------------------------------+
 *  | BFD-x                               |
 *  | [HG] L2 [MPLS..]    .....[IP/UDP]   |
 *  +-------------------------------------+
 *  |                          ^
 *  |                          |
 *  +----- 'lkey_offset-x'-----+
 *
 * NOTE:
 * The Lookup Key (lkey_etype/lkey_offset/lkey_length) is used by
 * the SW to match an initial RX BFD packet to an existing session entry.
 * An initial BFD packet has 'your_discriminator' set to 0, so HW is not able
 * to make a 'hit' on the corresponding session entry.
 *
 * Note that 'lkey_offset' is the offset relative the start of a
 * given BFD encapsulation.
 */
#define SHR_BFD_MAX_ENCAP_LENGTH    162


/* Config flags sent as part of MOS_MSG_SUBCLASS_BFD_INIT
 * bfd_sdk_msg_ctrl_init_t.config_flags
 * 32 bits.
 */
#define BFD_CONFIG_SESSION_DOWN_EVENT_ON_CREATE  0x00000001
#define BFD_CONFIG_SESSION_ID_AS_DISCR           0x00000002
#define BFD_CONFIG_STATIC_REMOTE_DISCR           0x00000004
#define BFD_CONFIG_CONTROL_PLANE_BIT             0x00000008
#define BFD_CONFIG_NO_DOWN_PKT_ON_SESS_DELETE    0x00000010
#define BFD_CONFIG_NOT_CHECK_TRAP_CODE_ON_RX_PKT 0x00000020
#define BFD_CONFIG_LOCALDISCR_AS_SESSION_ID      0x00000040
#define BFD_CONFIG_TRUNK_AUTO_TX_UPDATE_DISABLE  0x00000080
#define BFD_CONFIG_TX_RAW_INGRESS_ENABLE         0x00000100

/*
 * BFD Session Set control message
 */
typedef struct shr_bfd_msg_ctrl_sess_set_s {
    uint32  sess_id;
    uint32  flags;
    uint8   passive;
    uint8   local_demand;
    uint8   local_diag;
    uint8   local_detect_mult;
    uint32  local_discriminator;
    uint32  remote_discriminator;
    uint32  local_min_tx;
    uint32  local_min_rx;
    uint32  local_min_echo_rx;
    uint8   auth_type;
    uint32  auth_key;
    uint32  xmt_auth_seq;
    uint8   encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32  encap_length;  /* BFD encapsulation length */
    uint8   encap_data[SHR_BFD_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint16  lkey_etype;    /* Lookup key Ether Type */
    uint16  lkey_offset;   /* Lookup key offset */
    uint16  lkey_length;   /* Lookup key length */
    uint32  mep_id_length; /* MPLS-TP CV Source MEP-ID TLV length */
    uint8   mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                            Source MEP-ID */
    uint8   mpls_label[SHR_BFD_MPLS_LABEL_LENGTH]; /* Incoming inner MPLS
                                                      label packet format */
    uint32  tx_port;
    uint32  tx_cos;
    uint32  tx_pri;
    uint32  tx_qnum;
#ifdef SDK_45223_DISABLED
    uint32  remote_mep_id_length;
    uint8   remote_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                                 Remote MEP-ID */
#endif
} shr_bfd_msg_ctrl_sess_set_t;

/*
 * BFD Session Get control message
 */
typedef struct shr_bfd_msg_ctrl_sess_get_s {
    uint32   sess_id;
    uint8    enable;
    uint8    passive;
    uint8    local_demand;
    uint8    remote_demand;
    uint8    local_diag;
    uint8    remote_diag;
    uint8    local_sess_state;
    uint8    remote_sess_state;
    uint8    local_detect_mult;
    uint8    remote_detect_mult;
    uint32   local_discriminator;
    uint32   remote_discriminator;
    uint32   local_min_tx;
    uint32   remote_min_tx;
    uint32   local_min_rx;
    uint32   remote_min_rx;
    uint32   local_min_echo_rx;
    uint32   remote_min_echo_rx;
    uint8    auth_type;
    uint32   auth_key;
    uint32   xmt_auth_seq;
    uint32   rcv_auth_seq;
    uint8    encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32   encap_length;  /* BFD encapsulation length */
    uint8    encap_data[SHR_BFD_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint16   lkey_etype;    /* Lookup key Ether Type */
    uint16   lkey_offset;   /* Lookup key offset */
    uint16   lkey_length;   /* Lookup key length */
    uint32   mep_id_length; /* MPLS-TP CV Source MEP-ID TLV length */
    uint8    mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                             Source MEP-ID */
    uint8    mpls_label[SHR_BFD_MPLS_LABEL_LENGTH]; /* Incoming inner MPLS
                                                       label packet format */
    uint32   tx_port;
    uint32   tx_cos;
    uint32   tx_pri;
    uint32   tx_qnum;
#ifdef SDK_45223_DISABLED
    uint32   remote_mep_id_length;
    uint8    remote_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                                 Remote MEP-ID */
    uint32   mis_conn_mep_id_length;
    uint8    mis_conn_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                       Mis connectivity MEP-ID */
#endif
} shr_bfd_msg_ctrl_sess_get_t;

/*
 * BFD SHA1 Authentication control message
 */
typedef struct shr_bfd_msg_ctrl_auth_sha1_s {
    uint32  index;
    uint8   enable;
    uint32  sequence;
    uint8   key[_SHR_BFD_AUTH_SHA1_KEY_LENGTH];
} shr_bfd_msg_ctrl_auth_sha1_t;

/*
 * BFD Simple Password Authentication control message
 *
 * Set length to zero to disable entry.
 */
typedef struct shr_bfd_msg_ctrl_auth_sp_s {
    uint32  index;
    uint8   length;
    uint8   password[_SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH];
} shr_bfd_msg_ctrl_auth_sp_t;

/*
 * BFD Statistics control messages (Request/Reply)
 */
typedef struct shr_bfd_msg_ctrl_stat_req_s {
    uint32  sess_id;    /* BFD session (endpoint) id */
    uint32  clear;      /* Clear stats */
} shr_bfd_msg_ctrl_stat_req_t;

typedef struct shr_bfd_msg_ctrl_stat_reply_s {
    uint32  sess_id;           /* BFD session (endpoint) id */
    uint32  packets_in;        /* Total packets in */
    uint32  packets_out;       /* Total packets out */
    uint32  packets_drop;      /* Total packets drop */
    uint32  packets_auth_drop; /* Packets drop due to authentication failure */
} shr_bfd_msg_ctrl_stat_reply_t;



/****************************************
 * BFD event message
 */
#define BFD_BTE_EVENT_STATE                      0x0001
#define BFD_BTE_EVENT_REMOTE_STATE_DIAG          0x0002
#define BFD_BTE_EVENT_DISCRIMINATOR              0x0004
#define BFD_BTE_EVENT_AUTHENTICATION             0x0008
#define BFD_BTE_EVENT_PARAMETER                  0x0010
#define BFD_BTE_EVENT_ERROR                      0x0020
#define BFD_BTE_EVENT_FLAGS_CHANGE               0x0800
#define BFD_BTE_EVENT_REMOTE_POLL_BIT_SET        0x0040
#define BFD_BTE_EVENT_REMOTE_FINAL_BIT_SET       0x0080
#define BFD_BTE_EVENT_SESSION_STATE_ADMIN_DOWN   0x0100
#define BFD_BTE_EVENT_SESSION_STATE_DOWN         0x0200
#define BFD_BTE_EVENT_SESSION_STATE_INIT         0x0400
#define BFD_BTE_EVENT_SESSION_STATE_UP           0x1000
#define BFD_BTE_EVENT_SESSION_MISCONN_DEFECT     0x2000
#define BFD_BTE_EVENT_SESSION_MISCONN_CLEAR      0x4000
#define BFD_BTE_EVENT_SESSION_TIMEOUT            0x8000
#define BFD_BTE_EVENT_SESSION_TIMEIN            0x10000
#define BFD_BTE_EVENT_UNEXPECTED_MEG            0x20000
#define BFD_BTE_EVENT_UNEXPECTED_MEG_CLEAR      0x40000

/* BFD EVENT flags. */
#define BFD_BTE_EVENT_LOCAL_ADMIN_DOWN     0x00000001 /* BFD Local State Admin. */
#define BFD_BTE_EVENT_LOCAL_DOWN           0x00000002 /* BFD Local State Down. */
#define BFD_BTE_EVENT_LOCAL_INIT           0x00000004 /* BFD Local State Init. */
#define BFD_BTE_EVENT_LOCAL_UP             0x00000008 /* BFD Local State Up. */
#define BFD_BTE_EVENT_REMOTE_ADMIN_DOWN    0x00000010 /* BFD Remote State Admin. */
#define BFD_BTE_EVENT_REMOTE_DOWN          0x00000020 /* BFD Remote State Down. */
#define BFD_BTE_EVENT_REMOTE_INIT          0x00000040 /* BFD Remote State Init. */
#define BFD_BTE_EVENT_REMOTE_UP            0x00000080 /* BFD Remote State Up. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_NONE 0x00000100 /* BFD Local Diag None. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_CTRL_DETECT_TIME_EXPIRED 0x00000200 /* BFD Local Diag Detect
                                                          Time Expired. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_ECHO_FAILED 0x00000400 /* BFD Local Diag Echo Failed. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_NEIGHBOR_SIGNALED_SESSION_DOWN 0x00000800 /* BFD Neighbor Signaled
                                                          Session Down. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_FORWARDING_PLANE_RESET 0x00001000 /* Local diag Forwarding
                                                          Plane Reset. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_PATH_DOWN 0x00002000 /* Local diag Path Down. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_CONCATENATED_PATH_DOWN 0x00004000 /* Local Diag
                                                          Concatenated Path Down. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_ADMIN_DOWN 0x00008000 /* Local Diag Administratively Down. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_REVERSE_CONCATENATED_PATH_DOWN 0x00010000 /* Local Diag Reverse
                                                          Concatenated Path Down. */
#define BFD_BTE_EVENT_LOCAL_DIAG_CODE_MIS_CONNECTIVITY_DEFECT 0x00020000 /* Local Diag Mis conn Defect. */

/*
 *  The BFD event message is defined as a short message (use mos_msg_data_t).
 *
 *  The fields of mos_msg_data_t are used as followed:
 *      mclass   (uint8)  - MOS_MSG_CLASS_BFD_EVENT
 *      subclass (uint8)  - Unused
 *      len      (uint16) - BFD Session ID
 *      data     (uint32) - Events mask
 */
typedef mos_msg_data_t  bfd_msg_event_t;

#endif /* _SOC_SHARED_BFD_MSG_H_ */
