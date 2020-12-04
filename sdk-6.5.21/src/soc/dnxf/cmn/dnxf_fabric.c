/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF FABRIC
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#ifdef BCM_DNXF_SUPPORT
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <bcm/fabric.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <bcm/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <bcm_int/control.h>

#endif 

#undef BSL_LOG_MODULE

