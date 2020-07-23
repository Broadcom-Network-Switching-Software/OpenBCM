/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bhh.h
 * Purpose:     Defines common BHH parameters.
 */

#ifndef   _SHR_BHH_H_
#define   _SHR_BHH_H_


/* BHH Session States */
typedef enum _shr_bhh_session_state_e {
    _SHR_BHH_SESSION_STATE_ADMIN_DOWN,
    _SHR_BHH_SESSION_STATE_DOWN,
    _SHR_BHH_SESSION_STATE_INIT,
    _SHR_BHH_SESSION_STATE_UP,
    _SHR_BHH_SESSION_STATE_COUNT        /* MUST BE LAST  */
} _shr_bhh_session_state_t;


/* BHH Diagnostic Codes */
typedef enum _shr_bhh_diag_code_e {
    _SHR_BHH_DIAG_CODE_NONE,
    _SHR_BHH_DIAG_CODE_CTRL_DETECT_TIME_EXPIRED,
    _SHR_BHH_DIAG_CODE_ECHO_FAILED,
    _SHR_BHH_DIAG_CODE_NEIGHBOR_SIGNALED_SESSION_DOWN,
    _SHR_BHH_DIAG_CODE_FORWARDING_PLANE_RESET,
    _SHR_BHH_DIAG_CODE_PATH_DOWN,
    _SHR_BHH_DIAG_CODE_CONCATENATED_PATH_DOWN,
    _SHR_BHH_DIAG_CODE_ADMIN_DOWN,
    _SHR_BHH_DIAG_CODE_REVERSE_CONCATENATED_PATH_DOWN,
    _SHR_BHH_DIAG_CODE_COUNT        /* MUST BE LAST  */
} _shr_bhh_diag_code_t;


#endif /* _SHR_BHH_H_ */
