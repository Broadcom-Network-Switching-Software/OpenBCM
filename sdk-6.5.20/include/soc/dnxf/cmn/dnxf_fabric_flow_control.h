/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 
#ifndef _SOC_DNXF_FABRIC_FLOW_CONTROL_H_
#define _SOC_DNXF_FABRIC_FLOW_CONTROL_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

typedef struct soc_dnxf_fabric_rci_resolution_key_s {
    int pipe;
    uint32 shared_rci;
    uint32 guaranteed_rci;
} soc_dnxf_fabric_rci_resolution_key_t;

typedef struct soc_dnxf_fabric_rci_resolution_config_s {
    uint32 resolved_rci;
} soc_dnxf_fabric_rci_resolution_config_t;

#endif 
