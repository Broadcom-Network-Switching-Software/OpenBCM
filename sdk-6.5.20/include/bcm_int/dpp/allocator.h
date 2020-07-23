/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        allocator.h
 */

#ifndef _BCM_INT_DPP_ALLOCATOR_H_
#define _BCM_INT_DPP_ALLOCATOR_H_

#include <bcm/types.h>


/*
 * Resource types visible to the user
 */
typedef enum {
    DPP_USR_RES_FEC_L3,
    DPP_USR_RES_FEC_L3_MPATH,
    DPP_USR_RES_FEC_IPMC,
    DPP_USR_RES_FEC_GLOBAL_GPORT,
    DPP_USR_RES_FEC_LOCAL_GPORT,
    DPP_USR_RES_FEC_L2MC,
    DPP_USR_RES_FEC_MPLS,
    DPP_USR_RES_FEC_UNICAST,
    DPP_USR_RES_LIF,
    DPP_USR_RES_EEP,
    DPP_USR_RES_IFID,
    DPP_USR_RES_VRF,
    DPP_USR_RES_VSI,
    DPP_USR_RES_FLOW_SE,
    DPP_USR_RES_FLOW_CONNECTORS,
    DPP_USR_RES_QUEUES,
    DPP_USR_RES_MAX
} _dpp_usr_res_types_t;

typedef enum {
    DPP_HW_RES_FEC = 0,
    DPP_HW_RES_GLOBAL_GPORT_FEC,
    DPP_HW_RES_LOCAL_GPORT_FEC,
    DPP_HW_RES_LIF,
    DPP_HW_RES_EEP,
    DPP_HW_RES_IFID,
    DPP_HW_RES_VRF,
    DPP_HW_RES_VSI,
    DPP_HW_RES_PROTECTION,
    DPP_HW_RES_FLOWS,
    DPP_HW_RES_QUEUES,
    DPP_HW_RES_MAX
} _dpp_hw_res_t;

typedef enum {
    DPP_TABLE_NONE=0,
    DPP_TABLE_FEC,
    DPP_TABLE_VSI,
    DPP_TABLE_LIF,
    DPP_TABLE_EEP,
    DPP_TABLE_VRF,
    DPP_TABLE_PROT,
    DPP_TABLE_IFID,
    DPP_TABLE_FLOW,
    DPP_TABLE_QUEUE,
    DPP_TABLE_MAX
} _dpp_hw_table_t;


/*
 *   Function
 *      _dpp_resource_init
 *   Purpose
 *      Initialize the shr resource manager for all HW resources
 *      for the unit
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE all resources are successfully initialized
 *       BCM_E_* as appropriate otherwise
 *   Notes
 *       Returns error is any of the resources cannot be initialized
 */
extern int
_dpp_resource_init(int unit);

/*
 *   Function
 *      _dpp_alloc_wellknown_resources
 *   Purpose
 *      Allocate well-known resources per unit
 *      and save the same in the soc structure
 *      for sharing between modules.
 *
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE - All required resources are allocated
 *       BCM_E_*    - failure
 *   Notes
 */
extern int
_dpp_alloc_wellknown_resources(int unit);

#endif /* _BCM_INT_DPP_ALLOCATOR_H_ */
