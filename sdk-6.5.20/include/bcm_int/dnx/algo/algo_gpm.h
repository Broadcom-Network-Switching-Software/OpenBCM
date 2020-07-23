/** \file algo_gpm.h
 * 
 * 
 * Internal DNX Gport Managment APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_GPM_H_INCLUDED
/*
 * { 
 */
#define ALGO_GPM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes.
 * {
 */
#include <bcm/types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/l3/algo_l3_egr_pointed.h>

/*
 * }
 */

/**
 * Invalid LIF indication (both ingress, egress, global and
 * local)
 */
#define DNX_ALGO_GPM_LIF_INVALID (-1)
/**
 * Invalid FEC indication
 */
#define DNX_ALGO_GPM_FEC_INVALID (-1)

/**
 * \addtogroup Flags DNX_ALGO_GPM_GPORT_INFO_F_XXX 
 * Flags for dnx_algo_gpm_gport_phy_info_t struct 
 *  
 * @{ 
 */
/*
 * {
 */
/**
 * Gport is Local Port
 */
#define DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT SAL_BIT(0)
/**
 * Gport is LAG
 */
#define DNX_ALGO_GPM_GPORT_INFO_F_IS_LAG        SAL_BIT(1)
/**
 * Gport is system side
 */
#define DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE   SAL_BIT(2)
/**
 * Gport is Black Hole (drop)
 */
#define DNX_ALGO_GPM_GPORT_INFO_F_IS_BLACK_HOLE SAL_BIT(3)
/*
 * }
 */
/** @}*/

/**
 * \brief GPORT PP port physical information.
 *
 * This structure contains pp ports of a GPORT.
 * In case of LAG this information can contain several pp ports, each configured on a different core.
 * In other cases it will be a single pp port on a single core.
 */
typedef struct
{
    /**
     * Number of PP ports configured on the gport
     * usually 1, can be 0/1/2 for LAGs
     */
    int nof_pp_ports;
    /**
     * Array of internal pp ports configured on the gport, matching the core ids below
     */
    int pp_port[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    /**
     * Array of core ids per pp port, matching the pp ports above
     */
    int core_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
} dnx_algo_gpm_gport_phy_pp_port_info_t;

/**
 * \brief GPORT physical information.
 *
 * This structure contains physical information of a GPORT.
 * Relevant for GPORTs which are of type other than:
 * 1. FEC (like BCM_GPORT_VLAN/MPLS_PORT of subtype BCM_GPORT_SUB_TYPE_FORWARD_GROUP)
 * 2. Lif (like BCM_GPORT_VLAN/MPLS_PORT of subtype BCM_GPORT_SUB_TYPE_LIF)
 * 3. MC pointer (like BCM_GPORT_VLAN/MPLS_PORT of subtype BCM_GPORT_SUB_TYPE_MULTICAST)
 * 4. Push Profile (BCM_GPORT_MPLS_PORT of subtype BCM_GPORT_SUB_TYPE_MPLS_PUSH_PROFILE)
 *  \see 
 * dnx_algo_gpm_gport_phy_info_get
 */
typedef struct
{
    /**
     * Local port, first in case of LAG
     */
    bcm_port_t local_port;
    /**
     * Local ports bitmap (relevant only for LAG)
     */
    bcm_pbmp_t local_port_bmp;
    /**
     * System port
     */
    uint32 sys_port;
    /**
     * flags DNX_ALGO_GPM_GPORT_INFO_F_XXX, for example 
     * \ref DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT 
     */
    uint32 flags;
    /**
     * Lane number
     */
    int lane;
    /**
     * Internal information: pp port per core.
     */
    dnx_algo_gpm_gport_phy_pp_port_info_t internal_port_pp_info;
} dnx_algo_gpm_gport_phy_info_t;

/** 
 * \addtogroup Operations Operations 
 * Field of dnx_algo_gpm_gport_phy_info_get api
 *  
 * @{  
 */
/*
 * {
 */
/**
 * No Operation 
 */
#define DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE                        0
/**
 * Force system port retrieve 
 */
#define DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT    SAL_BIT(0)
/**
 * Force local port retrieve
 */
#define DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY  SAL_BIT(1)
/**
 * Force pp port retrieve
 */
#define DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY SAL_BIT(2)

/*
 * }
 */

/**
 * brief - flag of algo_gpm_gport_core_get()
 * If set throw an error when core == BCM_CORE_ALL
 */
#define ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL       (1)

/**
 *\brief - gport types used by algo_gpm_gport_verify().
 */
typedef enum
{
    ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT,
    ALGO_GPM_GPORT_VERIFY_TYPE_MODPORT,
    ALGO_GPM_GPORT_VERIFY_TYPE_DEVPORT,
    ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT,
    ALGO_GPM_GPORT_VERIFY_TYPE_PORT,
    ALGO_GPM_GPORT_VERIFY_TYPE_CORE,
    ALGO_GPM_GPORT_VERIFY_TYPE_UCAST_QUEUE_GROUP,
    ALGO_GPM_GPORT_VERIFY_TYPE_MCAST_QUEUE_GROUP,
    ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_CPU,
    ALGO_GPM_GPORT_VERIFY_TYPE_PHYN,
    ALGO_GPM_GPORT_VERIFY_TYPE_PHYN_LANE,
    ALGO_GPM_GPORT_VERIFY_TYPE_PHYN_SYS_SIDE,
    ALGO_GPM_GPORT_VERIFY_TYPE_PHYN_SYS_SIDE_LANE,
    ALGO_GPM_GPORT_VERIFY_TYPE_LANE,
    ALGO_GPM_GPORT_VERIFY_TYPE_TRUNK,
    ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_FABRIC,
    ALGO_GPM_GPORT_VERIFY_TYPE_BLACK_HOLE,
    ALGO_GPM_GPORT_VERIFY_TYPE_MCAST,

    ALGO_GPM_GPORT_VERIFY_TYPE_NOF
} algo_gpm_gport_verify_type_e;

/** @}*/

/**
* \brief
*   Given a gport, return physical information on physical ports
*   Return error on unsupported types, see dnx_algo_gpm_gport_phy_info_t
*   for more information on supported types.
*   dnx_algo_gpm_gport_is_physical can be used to check whether GPORT is physical
* 
*   Physical information includes port (system/pp/tm ports) and core id.
*   Operations flags specify whether system port should be retrieved and
*   whether local port is mandatory.
* 
*   Also supports local port input (not encoded as bcm_gport_t).
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] port -
*    In GPORT given by user
*  \param [in] operation -
*    Operation requested according to DNX_ALGO_GPM_GPORT_TO_PHY_OP_XXX,
*    for example \ref DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE
*  \param [out] phy_gport_info -
*    Physical information in case gport is physical port. 
*      In case gport is LAG, only system port will be retrieved. 
*  \return
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_gport_phy_info_t
*    dnx_algo_gpm_gport_is_physical
*    shr_error_e
 */
shr_error_e dnx_algo_gpm_gport_phy_info_get(
    int unit,
    bcm_gport_t port,
    uint32 operation,
    dnx_algo_gpm_gport_phy_info_t * phy_gport_info);

/**
* \brief
*   Given a gport, extract logical port.
*   Note:
*   Not all the gports are covered.
*   Before using this function, make sure that the required gports are already covered.
*   Additional gports support will be added upon requirement.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] flags -
*    Currently no flags supported
*  \param [in] gport -
*    In GPORT given by user.
*    Supported types:
*     - Local port
*     - BCM_GPORT_IS_LOCAL(gport)
*     - BCM_GPORT_IS_LOCAL_INTERFACE(gport)
*     - BCM_COSQ_GPORT_IS_PORT_TC(gport) 
*     - BCM_COSQ_GPORT_IS_PORT_TCG(gport)
*     - BCM_COSQ_GPORT_IS_E2E_PORT(gport)
*     - BCM_COSQ_GPORT_IS_E2E_INTERFACE(gport)
*     - BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport)
*     - BCM_COSQ_GPORT_IS_E2E_PORT_TCG(gport)
*     - BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport) 
*     - BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport)
*     - BCM_GPORT_IS_MODPORT(gport) 
*                             
*  \param [out] port -
*    The extracted logical port
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    None
 */
shr_error_e algo_gpm_tm_gport_to_port_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_port_t * port);

/**
* \brief
*   Given a gport, extract the core,
*   In a case the device is single core, the function will return the 0.
*   In a case the gport points to both cores, BCM_CORE_ALL,
*   In a case core cann't be extracted, BCM_E_PORT error will be raised.
*   Note:
*   Not all the gports are covered.
*   Before using this function, make sure that the required gports are already covered.
*   Additional gports support will be added upon requirement.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] flags -
*    See ALGO_GPM_GPORT_CORE_F_XXX.
*  \param [in] gport -
*    In GPORT given by user. Supported gports are:
*    * BCM_COSQ_GPORT_IS_CORE(gport)
*    * BCM_GPORT_IS_SCHEDULER(gport)
*    * BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)
*    * BCM_COSQ_GPORT_IS_SCHED_CIR(gport)
*    * BCM_COSQ_GPORT_IS_SCHED_PIR(gport)
*    * BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport)
*    * BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport)
*    * BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)
*    * BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)
*    * Local port
*    * BCM_GPORT_IS_LOCAL(gport)
*    * BCM_GPORT_IS_LOCAL_INTERFACE(gport)
*    * BCM_COSQ_GPORT_IS_PORT_TC(gport)
*    * BCM_COSQ_GPORT_IS_PORT_TCG(gport)
*    * BCM_COSQ_GPORT_IS_E2E_PORT(gport)
*    * BCM_COSQ_GPORT_IS_E2E_INTERFACE(gport)
*    * BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport)
*    * BCM_COSQ_GPORT_IS_E2E_PORT_TCG(gport)
*
*  \param [out] core -
*    The extracted core ID or BCM_CORE_ALL for both.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    None
 */
shr_error_e algo_gpm_tm_gport_to_core_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_core_t * core);

/**
 * \brief - verify gport type.
 *          The function throws an error in a case the gport type is not included in the array allowed_types.
 *           Note:
 *           Not all gports are covered by this function (the support list can be found in algo_gpm_gport_verify_type_e).
 *           Before using this function, make sure that the required gports are already covered.
 *           Additional gports support will be added upon requirement.
 *
 *           Example:
 *           "
 *           algo_gpm_gport_verify_type_e allowed_types[] = {ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT. ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT, ALGO_GPM_GPORT_VERIFY_TYPE_PORT};
 *           SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, gport, COUNTOF(allowed_types), allowed_types));
 *           "
 * \param [in] unit - unit ID
 * \param [in] gport - gport to verify
 * \param [in] nof_allowed_types - size of array allowed_types
 * \param [in] allowed_types = array of all the supported types
 * \return
 *   shr_error_e - Non-zero in case of an error.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e algo_gpm_gport_verify(
    int unit,
    bcm_gport_t gport,
    uint32 nof_allowed_types,
    algo_gpm_gport_verify_type_e * allowed_types);

/**
* \brief
*   Given a gport, returns whether the gport is physical port.
*   GPORT is physical in case it's not one of the following types: 
*   VPAN PORT / MPLS PORT / MIM PORT / TRILL PORT / TUNNEL /
*   L2GRE PORT / VXLAN PORT / FORWARD PORT / EXTENDER PORT
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] port -
*    In GPORT given by user
*  \param [out] is_physical_port -
*    Boolean return value indicating given gport is physical.
*    Meaning the gport is not of type FEC/Lif/MC pointer/Push Profile
*  \return 
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
shr_error_e dnx_algo_gpm_gport_is_physical(
    int unit,
    bcm_gport_t port,
    uint8 *is_physical_port);

/**
 * \brief
 *      Given a system port, returns the gport it's mapped to.
 *      This returned gport can be either a system gport or a trunk gport.
 *
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] system_port -
 *      System port to translate to gport.
 * \param [out] gport -
 *      Gport mapped to this system port.
 *
 * \remark
 *      None
 */
shr_error_e dnx_algo_gpm_gport_from_system_port(
    int unit,
    int system_port,
    bcm_gport_t * gport);

/*
 * HARDWARE RESOURCES FUNCTIONS
 */
/*
 * {
 */
/**
 * \brief GPORT hardware resources information.
 *
 * This structure contains HW information of a GPORT. 
 * 1. Local and Global LIFs (like BCM_GPORT_VLAN/MPLS_PORT of subtype 
 * BCM_GPORT_SUB_TYPE_LIF) 
 * 2. FEC (like BCM_GPORT_VLAN/MPLS_PORT of subtype
 * BCM_GPORT_SUB_TYPE_FORWARD_GROUP)
 * Each HW resource field can be filled if valid or mark as invalid if does not exist.
 *  \see 
 * dnx_algo_gpm_gport_to_hw_resources
 */
typedef struct
{
    /**
     * Global-LIF, Invalid when does not exist
     */
    int global_in_lif;
    /**
     * Global-LIF, Invalid when does not exist
     */
    int global_out_lif;
    /**
     * Local-LIF, Invalid when does not exist
     */
    int local_in_lif;
    /**
     * Local inlif core. Can be either a specific core (if lif is DPC and allocated only on one core) or _SHR_CORE_ALL,
     * if lif is SBC or DPC and allocated on all cores.
     */
    int local_in_lif_core;
    /**
     * Local-LIF, Invaild when does not exist
     */
    int local_out_lif;
    /**
     * The table relevant to the inlif DBAL_TABLE_XXX
     */
    dbal_tables_e inlif_dbal_table_id;
    /**
     * The result type relevant to the inlif DBAL_RESULT_TYPE_XXX
     */
    uint32 inlif_dbal_result_type;
    /**
     * The table relevant to the outlif DBAL_TABLE_XXX
     */
    dbal_tables_e outlif_dbal_table_id;
    /**
     * The result type relevant to the outlif DBAL_RESULT_TYPE_XXX
     */
    uint32 outlif_dbal_result_type;
    /**
     * The eedb phase this outlif is mapped to.
     */
    lif_mngr_outlif_phase_e outlif_phase;
    /**
     * FEC, invalid when does not exist
     */
    int fec_id;
} dnx_algo_gpm_gport_hw_resources_t;

/** 
 * \addtogroup flags DNX_ALGO_GPM_GPORT_HW_RESOURCES_XXX 
 *             Information representing which HW resource to
 *             retrieve. Used by
 *             dnx_algo_gpm_gport_to_hw_resources
 *  
 * @{  
 */
/**
 * Retrieve FEC resource
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC                   SAL_BIT(0)
/**
 * Retrieve Ingress Global LIF
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS    SAL_BIT(1)
/**
 * Retrieve Egress Global LIF
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS     SAL_BIT(2)
/**
 * Retrieve Ingress Local LIF
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS     SAL_BIT(3)
/**
 * Retrieve Egress Local LIF
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS      SAL_BIT(4)
/**
 * Don't return error in case required and invalid resource
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT            SAL_BIT(5)
/**
 * Retrieve DPC local lif. Only relevant for \ref dnx_algo_gpm_gport_from_lif
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS     SAL_BIT(6)
/**
 * Retrieve SBC local lif. Only relevant for \ref dnx_algo_gpm_gport_from_lif
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS     SAL_BIT(7)
/**
 * Retrieve Egress Local LIF
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_VIRTUAL_EGRESS            SAL_BIT(8)
/**
 * Retrieve Local LIF resources
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF \
    (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
/**
 * Retrieve Global LIF resources
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF \
    (DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS)
/**
 * Retrieve Local and Global LIF resources
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF \
    (DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF)
/**
 * Retrieve Egress resources - Local LIF or virtual
 */
#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL \
    (DNX_ALGO_GPM_GPORT_HW_RESOURCES_VIRTUAL_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
/** @}*/

/**
* \brief
*   Given a gport, returns related HW resources - LIF or FEC.
*   In case of LIF, both global and local can be retrieved.
*   Also ingress and egress options are available.
*   In case of ergress, virtual gport is supported by filling
*   gport_hw_resources->outlif_dbal_table_id
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    GPORT given by user
*  \param [in] flags -
*     flags of options what to retrieve. See for example
*     \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC in
*          DNX_ALGO_GPM_GPORT_HW_RESOURCES_XXX
*  \param [out] gport_hw_resources -
*    GPORT HW resources. Note that if GPORT is encoded as
*    having subtype _SHR_L3_ITF_SUB_TYPE_VIRTUAL_EGRESS_POINTED,
*    then 'egr_pointed' object is returned on
*    gport_hw_resources->global_out_lif 
*  \return 
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_gport_hw_resources_t
*    _SHR_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED
*    shr_error_e
 */
shr_error_e dnx_algo_gpm_gport_to_hw_resources(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

/**
 * \brief GPORT hardware resources information.
 *
 * This structure contains HW information of a RIF:
 * DBAL table id and result type
 *  \see
 * dnx_algo_gpm_rif_to_hw_resources
 */
typedef struct
{
    /**
     * The table relevant to the outlif DBAL_TABLE_XXX
     */
    dbal_tables_e outlif_dbal_table_id;
    /**
     * The result type relevant to the outlif DBAL_RESULT_TYPE_XXX
     */
    uint32 outlif_dbal_result_type;
} dnx_algo_gpm_rif_hw_resources_t;

/**
* \brief
*   Given an interface , returns related HW OUTLIF resources.
*   In case not found, return error
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] intf -
*    Interface given by user
*  \param [out] rif_hw_resources - GPORT HW resources.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_gport_hw_resources_t
*    shr_error_e
 */
shr_error_e dnx_algo_gpm_rif_intf_to_hw_resources(
    int unit,
    bcm_if_t intf,
    dnx_algo_gpm_rif_hw_resources_t * rif_hw_resources);

/**
* \brief
*   Given a global or local lif and core id, returns related gport.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] flags -
*     flags to indicate ingress/egress lif. Supported values:
*     \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS,
*     \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS, \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS,
*     \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, \ref DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT
*  \param [in] core_id - core
*  \param [in] lif - lif given by user
*  \param [out] gport - gport related to the given lif. See remarks below.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    * Only one flag is allowed to be set!                                                    \n
*    * If 'flags' indicates 'local + egress' then encoded gport may be of                     \n
*    subtype _SHR_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED.                                     \n
*    The reason is that some local out_lifs are assigned to 'egr_pointed' objects while      \n
*    others are assigned to 'global_lif'. This procedure detects which is which and encodes  \n
*    gport accordingly.
*  \see
*    dnx_algo_gpm_gport_hw_resources_t
*    shr_error_e
 */
shr_error_e dnx_algo_gpm_gport_from_lif(
    int unit,
    uint32 flags,
    int core_id,
    int lif,
    bcm_gport_t * gport);

/**
* \brief
*   Given a gport, and it's resources fill a handle with the key for the HW table.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in,out] entry_handle_id -
*    dbal handle id for the HW table (allocated with the correct table id)
*  \param [in] gport -
*    GPORT given by user
*  \param [in] gport_hw_resources -
*    GPORT HW resources. Relevant information will be used to fill the handle key
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_gport_hw_resources_t
 */
shr_error_e algo_gpm_gport_egress_hw_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_gport_t gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);
/*
 * END OF HARDWARE RESOURCES FUNCTIONS
 */
/*
 * }
 */

/*
 * FORWARD INFORMATION FUNCTIONS
 */
/*
 * {
 */
/**
 * No Flags
 */
#define ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE                        0
/**
 * Destination is for egress multicast use
 */
#define ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST                SAL_BIT(0)
/**
* \brief
*   Given a gport, returns forward destination encoded by this gport.
*   The destination can be then used for MACT, Cross-Connect
*   and other forwarding databases.
*   Used to store forwarding information in SW state
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] flags - one of ALGO_GPM_ENCODE_DESTINATION_XXX
*  \param [in] port -
*    GPORT given by user, might be port or gport.
*    Supported gports - physical gports and FEC gports.
*  \param [out] destination -
*     DBAL encoding for destination field.
*     Can be FEC/PORT/LAG/TRAP/FLOW-ID
*  \return
*    shr_error_e - 
*      Error return value
*  \remark
*    This function is used only for filling SW state table
*    DBAL_TABLE_SW_STATE_GPORT_TO_FORWARDING_INFO. For HW tables
*    configuration algo_gpm_gport_and_encap_to_forward_information should be used. 
*  \see
*    algo_gpm_gport_and_encap_to_forward_information
*    shr_error_e
 */
shr_error_e algo_gpm_encode_destination_field_from_gport(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    uint32 *destination);

/**
* \brief
*   Given a gport, returns forward destination encoded by this gport.
*   The destination can be then used for MACT, Cross-Connect
*   and other forwarding databases.
*   Used to store forwarding information in SW state
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] flags - one of ALGO_GPM_ENCODE_DESTINATION_XXX
*  \param [in] destination - given by user
*     DBAL encoding for destination field.
*     Can be FEC/PORT/LAG/TRAP/FLOW-ID
*  \param [out] gport - GPORT mapped by this destination.
*  \return
*    shr_error_e - 
*      Error return value
*  \remark
*    Physical ports have several possible mapping, we are
*    choosing BCM_SYSTEM_PORT for these ports.
*  \see
*    algo_gpm_gport_and_encap_to_forward_information
*    shr_error_e
 */
shr_error_e algo_gpm_gport_from_encoded_destination_field(
    int unit,
    uint32 flags,
    uint32 destination,
    bcm_gport_t * gport);

/**
 * \brief Forward information.
 *
 * This structure contains pre-fec forward information. it's 
 * used when retrieving forward info in 
 * algo_gpm_gport_and_encap_to_forward_information. The type 
 * specifies which fields of forward information are relevant.
 *  \see 
 * algo_gpm_gport_and_encap_to_forward_information
 */
typedef struct
{
    /**
     * type of forwarding information
     */
    dbal_enum_value_result_type_l2_gport_to_forwarding_sw_info_e fwd_info_result_type;
    /**
     * destination field, as set by DBAL in forwarding tables. must 
     * be valid for all types
     */
    uint32 destination;
    /**
     * outlif field, as set by DBAL in forwarding tables. valid for 
     * DEST_OUTLIF type only
     */
    uint32 outlif;
    /** 
     * eei field, as set by DBAL in forwarding tables. valid for 
     * DEST_EEI type only
     */
    uint32 eei;
} dnx_algo_gpm_forward_info_t;

/**
* \brief
*   Given a gport and forward information used in forwarding
*   stage (MAC table result), fill SW state in both directions
*   (gport to and from forward info).
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] is_replace -
*    If forward info is not being replaced, writing to existing entry will return error.
*  \param [in] gport -
*    GPORT given by user
*  \param [in] forward_info -
*      Forwarding information struct
*  \return 
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_forward_info_t
*    algo_gpm_gport_to_and_from_forward_information_free_sw_state
*    algo_gpm_gport_and_encap_to_forward_information
*    algo_gpm_gport_and_encap_from_forward_information
 */
shr_error_e algo_gpm_gport_l2_forward_info_add(
    int unit,
    uint8 is_replace,
    bcm_gport_t gport,
    dnx_algo_gpm_forward_info_t * forward_info);

/**
* \brief
*   Given a gport, get forward information used in forwarding
*   stage (MAC table result) from SW state
*   (gport to forward info).
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    GPORT given by user
*  \param [out] forward_info -
*      Forwarding information struct
*  \return 
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_forward_info_t
*    algo_gpm_gport_to_and_from_forward_information_free_sw_state
*    algo_gpm_gport_and_encap_to_forward_information
*    algo_gpm_gport_and_encap_from_forward_information
 */
shr_error_e algo_gpm_gport_l2_forward_info_get(
    int unit,
    bcm_gport_t gport,
    dnx_algo_gpm_forward_info_t * forward_info);

/**
* \brief
*   Free SW state in both directions - gport to and from forward
*   info.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    GPORT given by user that should be removed from both SW
*    states
*  \return 
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_forward_info_t
*    algo_gpm_gport_to_and_from_forward_information_fill_sw_state
*    algo_gpm_gport_and_encap_to_forward_information
*    algo_gpm_gport_and_encap_from_forward_information
 */
shr_error_e algo_gpm_gport_l2_forward_info_delete(
    int unit,
    bcm_gport_t gport);

/**
* \brief
*   Given a gport and encap id, returns forward information as
*   used in forwarding stage (MAC table result).
*   encap_id can be invalid (indicated by
*   BCM_FORWARD_ENCAP_ID_INVALID), in that case it won't be
*   used.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    GPORT given by user
*  \param [in] encap_id -
*     encapsulation id - can be outlif or EEI, encoded with
*     BCM_FORWARD_ENCAP_ID_XXX macros.
*  \param [out] forward_info -
*        Pointer to forward info structure that was retrieved
*  \return
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_forward_info_t
*    BCM_FORWARD_ENCAP_ID_INVALID
*    algo_gpm_gport_and_encap_from_forward_information
 */
shr_error_e algo_gpm_gport_and_encap_to_forward_information(
    int unit,
    bcm_gport_t gport,
    uint32 encap_id,
    dnx_algo_gpm_forward_info_t * forward_info);

/**
* \brief
*   Given forward information as used in forwarding stage (MAC
*   table result) returns associated gport OR physical port +
*   encap_id. The decision between them is done according to the
*   following logic:
*   IF (advanced rsrc mgmt AND api is forwarding api (MACT,
*   CC...) OR (destination type = destination_only) OR (force
*   destination is set)
*   THEN return port + encap
*   ELSE try to get gport, if not found return port + encap
*
*  \param [in] unit -
*    Relevant unit.
*  \param [out] gport -
*    GPORT retrieved. Can be either logical port or physical
*    port.
*  \param [out] encap_id -
*     encapsulation id, in case returning port+encap and not
*     gport. Can be outlif or EEI(in Jericho mode only),
*     encoded with BCM_FORWARD_ENCAP_ID_XXX macros.
*  \param [in] forward_info -
*     Pointer to forward info structure given by the user
*  \param [in] force_port_and_encap -
*       Force port+encap id to be returned and not gport
*  
*  \return
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_forward_info_t
*    algo_gpm_gport_and_encap_to_forward_information
 */
shr_error_e algo_gpm_gport_and_encap_from_forward_information(
    int unit,
    bcm_gport_t * gport,
    uint32 *encap_id,
    dnx_algo_gpm_forward_info_t * forward_info,
    uint8 force_port_and_encap);

/*
 * END OF FORWARD INFORMATION FUNCTIONS
 */
/*
 * }
 */

/*
 * } 
 */
#endif/*_ALGO_GPM_API_INCLUDED__*/
