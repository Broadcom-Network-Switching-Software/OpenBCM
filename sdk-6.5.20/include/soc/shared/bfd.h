/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bfd.h
 * Purpose: BFD definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_BFD_H
#define _SOC_SHARED_BFD_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif

#include <shared/bfd.h>


/*****************************************
 * BFD uController Error codes
 */
typedef enum shr_bfd_uc_error_e {
    SHR_BFD_UC_E_NONE = 0,
    SHR_BFD_UC_E_INTERNAL,
    SHR_BFD_UC_E_MEMORY,
    SHR_BFD_UC_E_UNIT,
    SHR_BFD_UC_E_PARAM,
    SHR_BFD_UC_E_EMPTY,
    SHR_BFD_UC_E_FULL,
    SHR_BFD_UC_E_NOT_FOUND,
    SHR_BFD_UC_E_EXISTS,
    SHR_BFD_UC_E_TIMEOUT,
    SHR_BFD_UC_E_BUSY,
    SHR_BFD_UC_E_FAIL,
    SHR_BFD_UC_E_DISABLED,
    SHR_BFD_UC_E_BADID,
    SHR_BFD_UC_E_RESOURCE,
    SHR_BFD_UC_E_CONFIG,
    SHR_BFD_UC_E_UNAVAIL,
    SHR_BFD_UC_E_INIT,
    SHR_BFD_UC_E_PORT
} shr_bfd_uc_error_t;


/*
 * BFD for Unknown Remote Discriminator
 * 'your_discriminator' zero
 */
#define SHR_BFD_DISCR_UNKNOWN_SESSION_ID        0x7FF    /* Session ID */
#define SHR_BFD_DISCR_UNKNOWN_YOUR_DISCR        0        /* Discriminator */

/*
 * BFD Session Set flags
 */
#define SHR_BFD_SESS_SET_F_CREATE               0x00000001
#define SHR_BFD_SESS_SET_F_LOCAL_DISC           0x00000002
#define SHR_BFD_SESS_SET_F_LOCAL_MIN_TX         0x00000004
#define SHR_BFD_SESS_SET_F_LOCAL_MIN_RX         0x00000008
#define SHR_BFD_SESS_SET_F_LOCAL_MIN_ECHO_RX    0x00000010
#define SHR_BFD_SESS_SET_F_LOCAL_DIAG           0x00000020
#define SHR_BFD_SESS_SET_F_LOCAL_DEMAND         0x00000040
#define SHR_BFD_SESS_SET_F_LOCAL_DETECT_MULT    0x00000080
#define SHR_BFD_SESS_SET_F_SESSION_DESTROY      0x00000100
#define SHR_BFD_SESS_SET_F_SHA1_XMT_SEQ_INCR    0x00000200
#define SHR_BFD_SESS_SET_F_ENCAP                0x00000400
#define SHR_BFD_SESS_SET_F_REMOTE_DISC          0x00000800
#define SHR_BFD_SESS_SET_F_AUTH_TYPE            0x00001000
#define SHR_BFD_SESS_SET_F_LOCAL_ECHO           0x00002000
#define SHR_BFD_SESS_SET_F_MEP_ID_LENGTH        0x00004000
#define SHR_BFD_SESS_SET_F_MEP_ID               0x00008000
#define SHR_BFD_SESS_SET_F_REMOTE_MEP_ID_LENGTH 0x00010000
#define SHR_BFD_SESS_SET_F_REMOTE_MEP_ID        0x00020000
#define SHR_BFD_SESS_SET_F_TRUNK                0x00040000
#define SHR_BFD_SESS_SET_F_MICRO_BFD            0x00080000
#define SHR_BFD_SESS_SET_F_SAMPLING_RATIO       0x00100000
#define SHR_BFD_SESS_SET_F_MPLS_PHP             0x00200000
#define SHR_BFD_SESS_SET_F_ADMIN_DOWN_PKT_ON_SESS_DEL 0x00400000
#define SHR_BFD_SESS_SET_F_SSO_UPDATE           0x00800000
#define SHR_BFD_SESS_SET_F_TX_RAW_INGRESS       0x01000000
#define SHR_BFD_SESS_SET_F_TX_LB_PKD_INGRESS    0x02000000
/*
 * BFD sampling flags
 */
#define SHR_BFD_SAMPLING_F_TRIGGER              0x80

/*
 * BFD Encapsulation types
 */
#define SHR_BFD_ENCAP_TYPE_RAW             0
#define SHR_BFD_ENCAP_TYPE_V4UDP           1
#define SHR_BFD_ENCAP_TYPE_V6UDP           2
#define SHR_BFD_ENCAP_TYPE_MPLS_TP_CC      3 /* MPLS-TP CC only mode */   
#define SHR_BFD_ENCAP_TYPE_MPLS_TP_CC_CV   4 /* MPLS-TP CC CV */   
#define SHR_BFD_ENCAP_TYPE_MPLS_PHP        5 /* MPLS PHP*/

/* BFD debug log macros */

/* Size of the metadata prefixed to the buffer */
#define SHR_BFD_TRACE_LOG_METADATA_SIZE 6

/* Index 0 has the size */
#define SHR_BFD_TRACE_LOG_SIZE(buf)    (buf)[4]

/* Index 1 has the current index in the buffer */
#define SHR_BFD_TRACE_LOG_CUR_IDX(buf) (buf)[5]

/* Increment the current index */
#define SHR_BFD_TRACE_LOG_IDX_INCR(buf, idx) \
    ((idx) + 1) > SHR_BFD_TRACE_LOG_SIZE(buf) ? SHR_BFD_TRACE_LOG_METADATA_SIZE \
                                            : ((idx) + 1)

/* Decrement the current index */
#define SHR_BFD_TRACE_LOG_IDX_DECR(buf, idx) \
    ((idx) == SHR_BFD_TRACE_LOG_METADATA_SIZE) ? (SHR_BFD_TRACE_LOG_SIZE(buf) - 1) \
                                             : ((idx) - 1)

/* BFD filenames, used for debugging, don't change the order */
#define SHR_BFD_FILENAMES              \
{                                      \
    "Invalid       ",                  \
    "bfd_control.c ",                  \
    "bfd_rx.c      ",                  \
    "bfd_shell.c   ",                  \
    "bfd_static.c  ",                  \
    "bfd_test.c    ",                  \
    "bfd_timer.c   ",                  \
    "bfd_tx.c      ",                  \
    "bfd_uc_pack.c ",                  \
    "bfd_util.c    "                   \
}

/* Enums for BFD filenames, used for debugging, don't change the order */
typedef enum shr_bfd_filenames_e {
    SHR_BFD_FILENAME_INVALID         = 0,
    SHR_BFD_FILENAME_BFD_CONTROL_C   = 1,
    SHR_BFD_FILENAME_BFD_RX_C        = 2,
    SHR_BFD_FILENAME_BFD_SHELL_C     = 3,
    SHR_BFD_FILENAME_BFD_STATIC_C    = 4,
    SHR_BFD_FILENAME_BFD_TEST_C      = 5,
    SHR_BFD_FILENAME_BFD_TIMER_C     = 6,
    SHR_BFD_FILENAME_BFD_TX_C        = 7,
    SHR_BFD_FILENAME_BFD_UC_PACK_C   = 8,
    SHR_BFD_FILENAME_BFD_UTIL_C      = 9,
    SHR_BFD_FILENAME_MAX
} shr_bfd_filenames_t;

/* BFD tunnel types. */
typedef enum shr_bfd_uc_match_id_type_e {
    SHR_BFD_UC_SINGLE_TAG = 0,            /* BFD with single vlan tag. */
    SHR_BFD_UC_DOUBLE_TAG = 1,            /* BFD with double vlan tag. */
    SHR_BFD_UC_OUTER_IPV4 = 2,            /* BFD with outer IPv4 tunnel. */
    SHR_BFD_UC_OUTER_IPV6 = 3,            /* BFD with outer IPv6 tunnel. */
    SHR_BFD_UC_GRE = 4,                   /* BFD with GRE tunnel. */
    SHR_BFD_UC_OUTER_UDP = 5,             /* BFD with outer IPv4/UDP tunnel. */
    SHR_BFD_UC_OUTER_BFD = 6,             /* BFD over IPv4/v6. */
    SHR_BFD_UC_IPV4_IN = 7,               /* BFD with IPv4-in-IPv6/IPv4 tunnel. */
    SHR_BFD_UC_IPV6_IN = 8,               /* BFD with IPv6-in-IPv6/IPv4 tunnel. */
    SHR_BFD_UC_GRE_IN = 9,                /* BFD with GRE-in-IPv6/IPv4 tunnel. */
    SHR_BFD_UC_INNER_UDP = 10,            /* BFD with IPv4-in-IPv6/IPv4 UDP tunnel. */
    SHR_BFD_UC_INNER_BFD = 11,            /* BFD over IPv6/IPv4-in-IPv6/IPv4 tunnel. */
    SHR_BFD_UC_MPLS = 12,                 /* BFD over MPLS. */
    SHR_BFD_UC_MPLS_LABEL = 13,           /* BFD over MPLS label details. */
    SHR_BFD_UC_IPINGRE = 14,              /* BFD with IP-in-GRE */
    SHR_BFD_UC_IPINIP = 15,               /* BFD with IP-in-IP */
    SHR_BFD_UC_MATCH_ID_TYPE_COUNT = 16
} shr_bfd_uc_match_id_type_t;

#endif /* _SOC_SHARED_BFD_H */
