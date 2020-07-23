/*  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        credit_request_thresholds.h
 * Purpose:     Extern declarations for DPP init ips thresholds routines.
 */

#ifndef __BCM_CREDIT_REQUEST_THRESHOLDS_H__
#define __BCM_CREDIT_REQUEST_THRESHOLDS_H__

#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

typedef enum {
    APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_SINGLE_FAP=0,
    APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_MGMT=1,
    APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE_HIGH_PRIO=2
} APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE;

int appl_dpp_cosq_diag_credit_request_thresholds_set(int unit, uint32 port_speed, int is_slow, APPL_DPP_COSQ_DIAG_CREDIT_REQUEST_THRESHOLDS_MODE mode);

#endif /* __BCM_CREDIT_REQUEST_THRESHOLDS_H__ */
