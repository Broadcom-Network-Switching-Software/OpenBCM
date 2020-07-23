/** \file algo_port_internal.h
 *  
 *  PORT manager, Port utilities, and Port verifiers.
 *  
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *  
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_PORT_INTERNAL_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <bcm/port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
/**
 * \brief - Invalid value for handle to Data Base
 */
#define DNX_ALGO_PORT_HANDLE_INVALID        (-1)

/**
 * \brief Creating TM interface id (key to TM interface DB) for CPU port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_NIF_ETH(unit, first_phy) (first_phy)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_NIF_ETH_SIZE(unit) (dnx_data_nif.phys.nof_phys_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for CPU port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_NIF_ILKN(unit, ilkn_id) (ilkn_id)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_NIF_ILKN_SIZE(unit) (dnx_data_nif.ilkn.ilkn_if_nof_get(unit))
/**
 * \brief Creating TM interface id (key to TM interface DB) for CPU port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CPU(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CPU_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))
/**
 * \brief Creating TM interface id (key to TM interface DB) for RCY MIRROR port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY_MIRROR(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY_MIRROR_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for ERP port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_ERP(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_ERP_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for OLP port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OLP(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OLP_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))
/**
 * \brief Creating TM interface id (key to TM interface DB) for OLP port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OAMP(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OAMP_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))
/**
 * \brief Creating TM interface id (key to TM interface DB) for OLP port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_SAT(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_SAT_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for RCY Mirror port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_EVENTOR(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_EVENTOR_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for RCY port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY(unit, core, interface_id) (core * dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit) + interface_id)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit) * dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for FLEXE CLIENT.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_CLIENT(unit, client_index) (client_index)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_CLIENT_SIZE(unit) (dnx_data_nif.flexe.nof_flexe_clients_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for FLEXE SAR.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_SAR(unit, client_index) (client_index)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_SAR_SIZE(unit) (dnx_data_nif.flexe.nof_flexe_clients_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for FLEXE MAC.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_MAC(unit, client_index) (client_index)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_MAC_SIZE(unit) (dnx_data_nif.flexe.nof_clients_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for FLEXE MAC.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_BASE_FOR_SCH_ONLY(unit, core) (core * dnx_data_sch.interface.nof_sch_interfaces_get(unit))
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_SCH_ONLY_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit) * dnx_data_sch.interface.nof_sch_interfaces_get(unit))

/**
 * \brief Creating TM interface id (key to TM interface DB) for CRPS port.
 */
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CRPS(unit, core) (core)
#define DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CRPS_SIZE(unit) (dnx_data_device.general.nof_cores_get(unit))

/**
 * \brief verify that handle is valid (otherwise throw an error)
 */
#define DNX_ALGO_PORT_HANDLE_VERIFY(handle)     \
        if (handle == DNX_ALGO_PORT_HANDLE_INVALID) \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Not supported for port. Handle is invalid \n")

/**
 * \brief true iff both keys are valid
 */
#define DNX_ALGO_PORT_IS_2D_HANDLE_VALID(handle)     \
        (handle.h0 != DNX_ALGO_PORT_HANDLE_INVALID && handle.h1 != DNX_ALGO_PORT_HANDLE_INVALID)

/**
 * \brief verify that handle is valid (otherwise throw an error)
 */
#define DNX_ALGO_PORT_2D_HANDLE_VERIFY(handle)     \
        if (!DNX_ALGO_PORT_IS_2D_HANDLE_VALID(handle)) \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Not supported for port. Handle is invalid \n")

/**
 * \brief compare two handles - return 1 if identical
 */
#define DNX_ALGO_PORT_2D_HANDLE_EQ(handle0, handle1)     \
        ((handle0.h0 == handle1.h0) && (handle0.h1 == handle1.h1))

/*
 * }
 */
/*
 * Functions
 * {
 */

/**
 * \brief - 
 * Resotre information stored in soc info for each valid logical port. 
 * This API should called only upon warm reboot 
 *  
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO - relevant port members
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_restore(
    int unit);

/**
 * \brief - 
 * Init SOC INFO members (set members value to default)
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO structure
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_init(
    int unit);

/**
 * \brief - Init all SOC_INFO relevant to port members 
 *          The function removes a port from SOC info data base
 *  
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] port_info - port info of the removed port
 *   \param [in] new_master_port - logical port of interface master port (after port remove), -1 if no master port
 *   \param [in] phy_ports - relevant just for nif ports - physical port bitmaps (one based)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO members which relevant for port access
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_remove(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    bcm_port_t new_master_port,
    bcm_pbmp_t phy_ports);

/**
 * \brief - Set all SOC_INFO relevant to port members 
 *          The function configure a port from SOC info data base
 *          This configuration should be manually restored after warm reboot
 *  
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] is_master_port - see function dnx_algo_port_master_get for more information
 *   \param [in] port_info - port info of the added port
 *   \param [in] phy_ports - relevant just for nif ports - physical port bitmaps (one based)
 *   \param [in] tdm_mode - tdm mode of the relevant port (DNX_ALGO_PORT_TDM_MODE_NONE if not relevant)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO members which relevant for port access
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_add(
    int unit,
    bcm_port_t logical_port,
    int is_master_port,
    dnx_algo_port_info_s port_info,
    bcm_pbmp_t phy_ports,
    dnx_algo_port_tdm_mode_e tdm_mode);

/**
 * \brief - Sets default fabric port names
 *
 * \param [in] unit - Relevant unit
 * \param [in] fabric_port_base - Fabric port base
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_soc_info_fabric_default_port_names_set(
    int unit,
    uint32 fabric_port_base);

/**
 * \brief -
 * Verify that the required channel_id is free
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - new logical port
 * \param [in] tm_interface_handle - the tm interface of the new logical port
 * \param [in] channel - the required channel for the new logical port
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channel_free_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_db_2d_handle_t tm_interface_handle,
    int channel);

/**
 * \brief -
 * Verify SCH info when adding a port
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - logical port
 * \param [in] port_info - basic port info
 * \param [in] interface_id - interface id to be used for this port
 * \param [in] info - info required for adding a port (see struct definition)
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_sch_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_db_2d_handle_t interface_id,
    dnx_algo_port_tm_add_t * info);

/**
 * \brief -
 * Get port master from interface id obtained by dnx_algo_port_db_interface_id_get().
 * When a few logical port represent the same physical interface, one of them are marked as master.
 * Might be used as single representative of the physical interface.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] interface_id - DB interface id obtained by dnx_algo_port_db_interface_id_get()
 * \par INDIRECT INPUT:
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] master_port - master logical port #.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_if_id_to_master_get(
    int unit,
    dnx_algo_port_db_2d_handle_t interface_id,
    int *master_port);

/*
 * }
 */
#endif/*_ALGO_PORT_API_INCLUDED__*/
