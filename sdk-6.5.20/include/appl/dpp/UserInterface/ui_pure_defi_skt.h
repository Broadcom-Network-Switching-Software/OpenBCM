/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_SKT_INCLUDED
/* { */
#define UI_PURE_DEFI_SKT_INCLUDED

/*
 * Note:
 * the following definitions must range between PARAM_SKT_START_RANGE_ID
 * and PARAM_SKT_END_RANGE_ID.
 * See ui_pure_defi.h
 */
enum {
  PARAM_SKT_TEST_ID       =         PARAM_SKT_START_RANGE_ID,
  PARAM_SKT_SET_ID,
  PARAM_SKT_CLEAR_ID,
  PARAM_SKT_START_ID,
  PARAM_SKT_STOP_ID,
  PARAM_SKT_SUSPEND_ID,
  PARAM_SKT_RESUME_ID,
  PARAM_SKT_SHOW_ID,
  PARAM_SKT_CLIENT_ID,
  PARAM_SKT_SERVER_ID,
  PARAM_SKT_SERVER_PORT_ID,
  PARAM_SKT_SERVER_IP_ID,
  PARAM_SKT_LOG_ID,
  PARAM_SKT_ECHO_ID,
  PARAM_SKT_WAIT_FOR_REPLY_ID,
  PARAM_SKT_NOF_PACKETS_ID,
  PARAM_SKT_PAYLOAD_SIZE_ID,
  PARAM_SKT_IPG_ID,
  PARAM_SKT_DCL_STATUS_ID,
} ;

#endif
