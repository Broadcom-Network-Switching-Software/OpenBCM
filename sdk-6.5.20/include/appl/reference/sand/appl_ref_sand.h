/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file appl_ref_sand.h
 */

#ifndef _APPL_REF_SAND_H_
#define _APPL_REF_SAND_H_

/*************
 * INCLUDES  *
 */
#include <shared/shrextend/shrextend_debug.h>

/*************
 * TYPE DEFS *
 */

 /**
  * \brief - Default number of queue/connectors per connection
  */
#define APPL_SAND_E2E_SCHEME_COS                           (8)

/*************
 * STRUCTURES *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Convert device and logical port to system port id.
 *
 * \param [in] unit - Unit ID
 * \param [in] device_idx - Index of device (aligned to 0)
 * \param [in] nof_sys_ports_per_device - number of system ports per device
 * \param [in] port - logical port
 * \param [Out] sysport - Returned system port index
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int appl_sand_device_to_sysport_convert(
    int unit,
    int device_idx,
    int nof_sys_ports_per_device,
    int port,
    int *sysport);

/**
 * \brief - Convert sysport id to vase voq id
 *
 * \param [in] unit - Unit ID
 * \param [in] sysport - system port ID
 * \param [Out] base_voq - returned base voq ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_sand_sysport_id_to_base_voq_id_convert(
    int unit,
    int sysport,
    int *base_voq);

/**
 * \brief - Convert modid index and logical port to base voq connector
 *
 * \param [in] unit - Unit ID
 * \param [in] local_port - Logical port
 * \param [in] remote_core_id - Core of remote voq
 * \param [in] ingress_device_idx - Ingress (remote device) device index
 * \param [in] nof_devices - nof devices in the system
 * \param [out] base_voq_connector -   Returned base voq connector
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_sand_device_to_base_voq_connector_id_convert(
    int unit,
    int local_port,
    int remote_core_id,
    int ingress_device_idx,
    int nof_devices,
    int *base_voq_connector);

#endif /*_APPL_REF_SAND_H_ */
