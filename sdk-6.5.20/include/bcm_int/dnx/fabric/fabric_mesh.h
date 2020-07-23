/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/fabric/fabric.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_MESH_H_INCLUDED_
/*
 * { 
 */
#define _DNX_FABRIC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <soc/types.h>
#include <soc/dnxc/fabric.h>
#include <bcm/types.h>

/*************
 * DEFINES   *
 */
/* { */
/*
 * Group context length in bits
 */
#define DNX_FABRIC_GROUP_CTX_LENGTH         (4)

/*
 * MESH MC defines
 */
#define DNX_FABRIC_MESH_MC_TABLE_WIDTH (169)
#define DNX_FABRIC_MESH_MC_FAP_GROUP_SIZE       (32)
#define DNX_FABRIC_MESH_MC_REPLICATION_LENGTH   (5)
#define DNX_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT    (0x1)
#define DNX_FABRIC_MESH_MC_REPLICATION_DEST_1_BIT    (0x2)
#define DNX_FABRIC_MESH_MC_REPLICATION_DEST_2_BIT    (0x4)
#define DNX_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT   (0x8)
#define DNX_FABRIC_MESH_MC_REPLICATION_LOCAL_1_BIT   (0x10)
/* } */

/*
 * Typedefs:
 * {
 */
/*
 * } 
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief
 *   Initialization of MESH mode configurations.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_mesh_init(
    int unit);

/**
 * \brief
 *   Set local devices ids of Mesh system.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   local core id.
 * \param [in] modid_index -
 *   modid index.
 * \param [in] modid -
 *   The modid to set.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Setting modid_index 0 will configure all modid_indexes
 *     of that core with the same modid.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_mesh_device_id_local_set(
    int unit,
    bcm_core_t core,
    int modid_index,
    bcm_module_t modid);

/**
 * \brief
 *   Set local devices ids of Mesh system.
 * \param [in] unit -
 *   The unit number.
 * \param [in] dest_group_id -
 *   destination group id.
 * \param [in] modid_index -
 *   modid index.
 * \param [in] modid -
 *   The modid to set.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Setting modid_index 0 will configure all modid_indexes
 *     of that group with the same modid.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_mesh_device_id_dest_set(
    int unit,
    int dest_group_id,
    int modid_index,
    bcm_module_t modid);

/**
 * \brief
 *   Initialize Mesh Topology.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_mesh_topology_init(
    int unit);

 /**
  * \brief
  *   Retreive mesh topology statuses.
  * \param [in] unit -
  *   The unit number.
  * \param [out] mesh_topology_diag -
  *   Struct to receive all mesh topology statuses.
  * \return
  *   See \ref shr_error_e
  * \remark
  *   * None.
  * \see
  *   * None.
  */
shr_error_e dnx_fabric_mesh_topology_get(
    int unit,
    soc_dnxc_fabric_mesh_topology_diag_t * mesh_topology_diag);

#endif /*_DNX_FABRIC_MESH_H_INCLUDED_*/
