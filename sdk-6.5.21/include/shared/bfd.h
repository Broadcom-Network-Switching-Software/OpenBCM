/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bfd.h
 * Purpose:     Defines common BFD parameters.
 */

#ifndef   _SHR_BFD_H_
#define   _SHR_BFD_H_


/* BFD Session States */
typedef enum _shr_bfd_session_state_e {
    _SHR_BFD_SESSION_STATE_ADMIN_DOWN   = 0,
    _SHR_BFD_SESSION_STATE_DOWN         = 1,
    _SHR_BFD_SESSION_STATE_INIT         = 2,
    _SHR_BFD_SESSION_STATE_UP           = 3,
    _SHR_BFD_SESSION_STATE_COUNT        = 4/* MUST BE LAST  */
} _shr_bfd_session_state_t;


/* BFD Diagnostic Codes */
typedef enum _shr_bfd_diag_code_e {
    _SHR_BFD_DIAG_CODE_NONE                             = 0,
    _SHR_BFD_DIAG_CODE_CTRL_DETECT_TIME_EXPIRED         = 1,
    _SHR_BFD_DIAG_CODE_ECHO_FAILED                      = 2,
    _SHR_BFD_DIAG_CODE_NEIGHBOR_SIGNALED_SESSION_DOWN   = 3,
    _SHR_BFD_DIAG_CODE_FORWARDING_PLANE_RESET           = 4,
    _SHR_BFD_DIAG_CODE_PATH_DOWN                        = 5,
    _SHR_BFD_DIAG_CODE_CONCATENATED_PATH_DOWN           = 6,
    _SHR_BFD_DIAG_CODE_ADMIN_DOWN                       = 7,
    _SHR_BFD_DIAG_CODE_REVERSE_CONCATENATED_PATH_DOWN   = 8,
    _SHR_BFD_DIAG_CODE_MIS_CONNECTIVITY_DEFECT          = 9,
    _SHR_BFD_DIAG_CODE_COUNT        = 10/* MUST BE LAST  */
} _shr_bfd_diag_code_t;


/* BFD Authentication Types */
typedef enum _shr_bfd_auth_type_e {
    _SHR_BFD_AUTH_TYPE_NONE                             = 0,
    _SHR_BFD_AUTH_TYPE_SIMPLE_PASSWORD                  = 1,
    _SHR_BFD_AUTH_TYPE_KEYED_MD5                        = 2,
    _SHR_BFD_AUTH_TYPE_METICULOUS_KEYED_MD5             = 3,
    _SHR_BFD_AUTH_TYPE_KEYED_SHA1                       = 4,
    _SHR_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1            = 5,
    _SHR_BFD_AUTH_TYPE_COUNT        = 6/* MUST BE LAST  */
} _shr_bfd_auth_type_t;


/* SHA1 Authentication key length */
#define _SHR_BFD_AUTH_SHA1_KEY_LENGTH               20

/* Simple Password Authentication key length */
#define _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH    16

/* MPLS-TP CV Source MEP-ID TLV length */
#define _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH  32


#endif /* _SHR_BFD_H_ */
