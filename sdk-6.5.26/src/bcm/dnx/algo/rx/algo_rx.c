/** \file algo_rx.c
 *
 * Mirror related algorithm functions for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/**
* INCLUDE FILES:
* {
*/

/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>

/*
 * }
 */

/*
 * Other include files. 
 * { 
 */

#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>

/*
 * }
 */

/**
 * }
 */

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_ingress_predefined_allocate(
    int unit,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;

    /*
     * allocate ingress predefined id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_ingress_user_define_allocate(
    int unit,
    int alloc_flags,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    /*
     * allocate ingress user_define trap id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.allocate_single(unit, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP pool
* \par DIRECT INPUT: 
*   \param [in] unit     - Unit id
*   \param [in] flags    - Flags
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ERPP trap_id
*           newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_erpp_allocate(
    int unit,
    int flags,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");
    alloc_flags = 0;
    if (flags == BCM_RX_TRAP_WITH_ID)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /*
     * allocate erpp id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_ pool
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit id
*   \param [in] flags     - Flags(WITH_ID)
*   \param [in] type     -  bcmRxTrapTypeEgTx.. to allocate
*   \param [out] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ETPP trap_id newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_allocate(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int *trap_id_p)
{
    int alloc_flags = 0;
    int nof_etpp_ud_profiles = dnx_data_trap.etpp.nof_etpp_user_configurable_profiles_get(unit);
    int last_ud_profile = dnx_data_trap.etpp.first_etpp_user_configurable_profile_get(unit) + nof_etpp_ud_profiles - 1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");

    if (type == bcmRxTrapEgTxVisibility)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        *trap_id_p = DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_VISIBILITY;
    }
    else if (flags == BCM_RX_TRAP_WITH_ID)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /*
     * allocate etpp id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.allocate_single(unit, alloc_flags, NULL, trap_id_p));

    if ((type == bcmRxTrapEgTxUserDefine) && (*trap_id_p > last_ud_profile))
    {
        SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.free_single(unit, *trap_id_p, NULL));
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Can't allocate element because the resource is already full."
                     " Num of ETPP user defined trap profiles is %d\n", nof_etpp_ud_profiles);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_etpp_oam_allocate(
    int unit,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");

    alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;

    /*
     * allocate etpp oam id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_ingress_predefined_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.free_single(unit, trap_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_user_define_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.free_single(unit, trap_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See alogo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_erpp_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.free_single(unit, trap_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_etpp_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.free_single(unit, trap_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_oam_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.free_single(unit, trap_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_ingress_predefined_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific ingress user define trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit Id
*   \param [in] trap_id         -  User define trap id to check
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if ingress user_define trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_user_define_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific erpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of erpp trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the erpp trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_erpp_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_id_etpp_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific etpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of etpp oam trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the etpp oam trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_oam_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See alogo_rx.h
 */
shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_allocate(
    int unit,
    int alloc_flags,
    int *compressed_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(compressed_type_p, _SHR_E_PARAM, "compressed_type_p");

    /*
     * allocate ICMP Compressed type
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.allocate_single(unit, alloc_flags, NULL, compressed_type_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Check if the ICMP compression type is allocated.
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit id
*   \param [in] compressed_type -  ICMP compressed type
*   \param [out] is_allocated_p -  Is allocated
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_is_allocated(
    int unit,
    int compressed_type,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");
    /*
     * deallocate ICMP compressed type
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.is_allocated(unit, compressed_type, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*   Deallocate ICMP compression type
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit id
*   \param [in] compressed_type -  ICMP compressed type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_deallocate(
    int unit,
    int compressed_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate ICMP compressed type
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.free_single(unit, compressed_type, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for ingress traps.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_ingress_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * Ingress Predefined Trap resource manager  
     */
    {

        /** First trap id in trap res mngr */
        data.first_element = 0;
        /** Number of ingress predefined traps (highset HW trap ID) */
        data.nof_elements = dnx_data_trap.ingress.nof_predefeind_traps_get(unit);
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ingress predefined traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.create(unit, &data, NULL));
    }

    /*
     * Ingress User-Defined Trap resource manager  
     */
    {
        /** First trap id in trap res mngr */
        data.first_element = 0;

        /** Number of ingress user-defined traps */
        if (DNX_RX_TRAP_SYSTEM_HEADERS_MODE_GET(unit) == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            data.nof_elements = dnx_data_trap.ingress.nof_user_defined_traps_in_jr1_mode_get(unit);
        }
        else
        {
            data.nof_elements = dnx_data_trap.ingress.nof_user_defined_traps_get(unit);
        }

        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ingress user-defined traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for ERPP traps
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_erpp_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

    /** ERPP Trap resource manager  */
    {

        /** First trap id in trap res mngr - ID 0 is reserved as BCM_RX_TRAP_EG_TRAP_ID_DEFAULT*/
        data.first_element = DNX_RX_TRAP_ID_TYPE_GET(BCM_RX_TRAP_EG_TRAP_ID_DEFAULT) + 1;
        /** Number of erpp traps */
        data.nof_elements = dnx_data_trap.erpp.nof_erpp_user_configurable_profiles_get(unit);
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_ERPP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ERPP traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for etpp traps.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_etpp_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

    /** ETPP Trap resource manager  */
    {
        /** First user trap index */
        data.first_element = dnx_data_trap.etpp.first_etpp_user_configurable_profile_get(unit);
        /** Number of ETPP user traps: nof_etpp_trap_profile - nof_predefined_profiles */
        data.nof_elements =
            dnx_data_trap.etpp.nof_etpp_trap_profiles_get(unit) -
            dnx_data_trap.etpp.first_etpp_user_configurable_profile_get(unit);
        /** Allocation needs to be by order since ud trap id range is limited */
        data.flags = RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_ETPP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ETPP traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.create(unit, &data, NULL));
    }

    /** ETPP OAM Trap resource manager  */
    if (dnx_data_trap.etpp.feature_get(unit, dnx_data_trap_etpp_oam_traps_predefined_support))
    {
        /** First trap id in trap res mngr */
        data.first_element = 0;
        /** Number of etpp OAM traps */
        data.nof_elements = dnx_data_trap.etpp.nof_etpp_oam_traps_get(unit);
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ETPP OAM traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for ERPP traps
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

    /** ICMP Type compression resource manager  */
    {

        data.first_element = 1;
        /** Number of ICMP Compressed types traps */
        data.nof_elements = dnx_data_trap.ingress.nof_icmp_compressed_types_get(unit) - 1;
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_PROTOCOL_ICMP_TYPE_MAP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for Rx traps.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_res_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_ingress_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_icmp_compressed_type_res_mngr_init(unit));

    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        SHR_IF_ERR_EXIT(dnx_algo_rx_trap_erpp_res_mngr_init(unit));
    }

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_etpp_res_mngr_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   algo_rx.h file
 */
void
dnx_algo_rx_trap_recycle_cmd_print_entry_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    dnx_rx_trap_recycle_cmd_config_t *template_data = (dnx_rx_trap_recycle_cmd_config_t *) data;
#endif
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "recycle_forward_strength",
                                   template_data->recycle_strength, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ingress_trap_id",
                                   template_data->ingress_trap_id, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \see
 *   algo_rx.h file
 */
void
dnx_algo_rx_trap_gport_print_entry_cb(
    int unit,
    const void *data)
{
#ifdef DNX_SW_STATE_DIAGNOSTIC
    bcm_gport_t *template_data = (bcm_gport_t *) data;
#endif
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "trap_gport", *template_data, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief - Create template manager for recycle command. 
 * One profile is used as default profile (profile number 0). 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_recycle_cmd_template_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    dnx_rx_trap_recycle_cmd_config_t default_profile, profile;
    /*
     * Trap IDs reserved for Template manager init 
     */
    dbal_enum_value_field_ingress_trap_id_e reserved_trap_ids[] = { DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_256,
        DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_257, DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_258,
        DBAL_ENUM_FVAL_INGRESS_TRAP_ID_RESERVED_259
    };
    int profile_id, ind;
    int egress_divided_to_erpp_and_etpp =
        dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp);
    int last_reserved_profile =
        egress_divided_to_erpp_and_etpp ? DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_4 :
        DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_3;
    uint8 is_first, is_last;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    sal_memset(&default_profile, 0, sizeof(default_profile));
    sal_memset(&profile, 0, sizeof(profile));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.ingress.nof_recycle_cmds_get(unit);
    data.max_references = dnx_data_trap.etpp.nof_etpp_app_traps_get(unit)
        + dnx_data_trap.etpp.nof_etpp_oam_traps_get(unit);
    data.data_size = sizeof(dnx_rx_trap_recycle_cmd_config_t);
    data.default_data = &default_profile;
    data.default_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_RECYCLE_CMD, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.create(unit, &data, NULL));

    /** Allocate reserved recycle commands */
    /** This structure must be fully initialized since it is use (all of it) as
     * key down in the hash table utilities. (See 'key' in sw_state_hash_table_find_entry())
     */

    /** For united egress devices, following profiles are part of the configurable fwd recycle profiles pool *
     *  and do not need to be allocated separately */
    if (egress_divided_to_erpp_and_etpp)
    {
        profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_EGRESS_TRAPPED;
        profile.ingress_trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_EGRESS_TRAPPED_2ND_PASS;
        profile.recycle_strength = dnx_data_trap.strength.max_strength_get(unit);
        SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                        (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id, &is_first,
                         &is_last));
    }

    profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_OAM_UPMEP_REFLECTOR;
    profile.ingress_trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    profile.recycle_strength = 0;
    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                    (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id, &is_first, &is_last));

    for (ind = 0; profile_id < last_reserved_profile; ind++)
    {
        profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_RESERVED_1 + ind;
        profile.ingress_trap_id = reserved_trap_ids[ind];
        profile.recycle_strength = 0;
        SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                        (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id, &is_first,
                         &is_last));
    }

    profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_DROP;
    profile.ingress_trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_ACCEPTABLE_FRAME_TYPE0;
    profile.recycle_strength = dnx_data_trap.strength.max_strength_get(unit);
    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                    (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id, &is_first, &is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create template manager for ingress lif traps. 
 * One profile is used as default profile (profile number 0). 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_lif_ingress_template_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    bcm_gport_t default_profile = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.ingress.nof_lif_traps_get(unit);
    data.max_references = dnx_data_lif.global_lif.nof_global_in_lifs_get(unit);
    data.data_size = sizeof(bcm_gport_t);
    data.default_data = &default_profile;
    data.default_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_INGRESS_LIF, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create template manager for ETPP lif traps. 
 * One profile is used as default profile (profile number 0). 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_lif_etpp_template_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    bcm_gport_t default_profile = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit);
    data.max_references = dnx_data_lif.global_lif.nof_global_out_lifs_get(unit);
    data.data_size = sizeof(bcm_gport_t);
    data.default_data = &default_profile;
    data.default_profile = DNX_RX_TRAP_ETPP_LIF_PROFILE_DEFAULT;
    data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_ETPP_LIF_PROFILE;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE | DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_ETPP_LIF, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create template manager for MTU traps.
 * One profile is used as default profile (profile number 0).
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_mtu_template_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    bcm_gport_t default_profile = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.etpp.nof_mtu_trap_action_profiles_get(unit);
    data.max_references = dnx_data_trap.etpp.nof_mtu_map_table_entries_get(unit);
    data.data_size = sizeof(bcm_gport_t);
    data.default_data = &default_profile;
    data.default_profile = 0;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_MTU, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_mtu.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    Create Algo Rx traps templates.
* \par DIRECT INPUT: 
*   \param [in] unit  - Unit ID 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_template_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_lif_ingress_template_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_lif_etpp_template_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_recycle_cmd_template_init(unit));

    if (dnx_data_trap.etpp.feature_get(unit, dnx_data_trap_etpp_mtu_trap_action_profile_indirection))
    {
        SHR_IF_ERR_EXIT(dnx_algo_rx_trap_mtu_template_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_rx.h
 */
shr_error_e
dnx_algo_rx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init sw state algo_rx_db.
     */
    SHR_IF_ERR_EXIT(algo_rx_db.init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_template_init(unit));

exit:
    SHR_FUNC_EXIT;
}
