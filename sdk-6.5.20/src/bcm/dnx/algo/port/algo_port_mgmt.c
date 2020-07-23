/** \file algo_port_mgmt.c
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
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/portmod/portmod.h>

#include <bcm/types.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "algo_port_internal.h"

/*
 * }
 */
/*
 * Defines.
 * {
 */
/** Maximum number of first header size profiles after injected header*/
#define DNX_PORT_MAX_FIRST_HEADER_SIZE_AFTER_INJECTED_PROFILES 1
/*
 * }
 */
/*
 * Macros
 * {
 */

/*
 * }
 */

/*
 * Local functions (documentation in function implementation)
 * {
 */

static shr_error_e dnx_algo_port_tm_add(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_tm_add_t * info,
    dnx_algo_port_db_2d_handle_t tm_interface_id);

static shr_error_e dnx_algo_port_sch_add(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_tm_add_t * info,
    dnx_algo_port_db_2d_handle_t tm_interface_id);
/*
 * }
 */

/*
 * Module Init / Deinit
 * {
 */
/**
 * \brief - set default values for element in logical DB.
 */
static shr_error_e
dnx_algo_port_db_logical_init(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_db_logical_t logical_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&logical_db, 0, sizeof(dnx_algo_port_db_logical_t));
    logical_db.fabric_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.ilkn_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.nif_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.pp_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.pp_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_interface_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_interface_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.interface_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.interface_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_if_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_if_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.flexe_phy_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.flexe_mac_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.flexe_sar_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.flexe_client_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.set(unit, logical_port, &logical_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in fabric DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_fabric_init(
    int unit,
    int fabric_handle)
{
    dnx_algo_port_db_fabric_t fabric_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&fabric_db, 0, sizeof(dnx_algo_port_db_fabric_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.set(unit, fabric_handle, &fabric_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in nif DB.
 *  This function is not called during init since the current default is 0.
 *  In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_nif_init(
    int unit,
    int nif_handle)
{
    int rmc_index;
    dnx_algo_port_db_nif_t nif_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&nif_db, 0, sizeof(dnx_algo_port_db_nif_t));
    nif_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    for (rmc_index = 0; rmc_index < BCM_PORT_MAX_NOF_PRIORITY_GROUPS; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_rmc_info_t_init(unit, &nif_db.logical_fifos[rmc_index]));
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.set(unit, nif_handle, &nif_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in flexe client DB.
 *  This function is not called during init since the current default is 0.
 *  In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_flexe_client_init(
    int unit,
    int flexe_handle)
{
    dnx_algo_port_db_flexe_client_t flexe_client_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flexe_client_db, 0, sizeof(dnx_algo_port_db_flexe_client_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.set(unit, flexe_handle, &flexe_client_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in flexe mac DB.
 *  This function is not called during init since the current default is 0.
 *  In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_flexe_mac_init(
    int unit,
    int flexe_handle)
{
    int rmc_index;
    dnx_algo_port_db_flexe_mac_t flexe_mac_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flexe_mac_db, 0, sizeof(dnx_algo_port_db_flexe_mac_t));
    for (rmc_index = 0; rmc_index < dnx_data_nif.flexe.priority_groups_nof_get(unit); ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_rmc_info_t_init(unit, &flexe_mac_db.logical_fifos[rmc_index]));
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.set(unit, flexe_handle, &flexe_mac_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in flexe sar DB.
 *  This function is not called during init since the current default is 0.
 *  In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_flexe_sar_init(
    int unit,
    int flexe_handle)
{
    dnx_algo_port_db_flexe_sar_t flexe_sar_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flexe_sar_db, 0, sizeof(dnx_algo_port_db_flexe_sar_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.set(unit, flexe_handle, &flexe_sar_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in flexe phy DB.
 *  This function is not called during init since the current default is 0.
 *  In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_flexe_phy_init(
    int unit,
    int flexe_handle)
{
    dnx_algo_port_db_flexe_phy_t flexe_phy_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flexe_phy_db, 0, sizeof(dnx_algo_port_db_flexe_phy_t));
    flexe_phy_db.core_port = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.set(unit, flexe_handle, &flexe_phy_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in ilkn DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_ilkn_init(
    int unit,
    int ilkn_handle)
{
    dnx_algo_port_db_ilkn_t ilkn_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ilkn_db, 0, sizeof(dnx_algo_port_db_ilkn_t));
    ilkn_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.set(unit, ilkn_handle, &ilkn_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in tm interface DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_tm_interface_init(
    int unit,
    dnx_algo_port_db_2d_handle_t tm_interface_handle)
{
    dnx_algo_port_db_tm_interface_t tm_interface_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tm_interface_db, 0, sizeof(dnx_algo_port_db_tm_interface_t));
    tm_interface_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.
                    tm_interface.set(unit, tm_interface_handle.h0, tm_interface_handle.h1, &tm_interface_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in port_to_profile DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_prd_port_to_profile_map_init(
    int unit,
    int interface_handle)
{
    dnx_algo_prd_info_t interface_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&interface_db, 0, sizeof(dnx_algo_prd_info_t));
    interface_db.port_to_profile_map = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.set(unit, interface_handle, &interface_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in interface DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_interface_init(
    int unit,
    dnx_algo_port_db_2d_handle_t interface_handle)
{
    dnx_algo_port_db_interface_t interface_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&interface_db, 0, sizeof(dnx_algo_port_db_interface_t));
    interface_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.set(unit, interface_handle.h0, interface_handle.h1, &interface_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in scheduler DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_sch_if_init(
    int unit,
    dnx_algo_port_db_2d_handle_t interface_handle)
{
    dnx_algo_port_db_sch_if_t sch_if_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sch_if_db, 0, sizeof(dnx_algo_port_db_sch_if_t));
    sch_if_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    sch_if_db.sch_if = -1;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.set(unit, interface_handle.h0, interface_handle.h1, &sch_if_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in tm DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_tm_init(
    int unit,
    dnx_algo_port_db_2d_handle_t tm_handle)
{
    dnx_algo_port_db_tm_t tm_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tm_db, 0, sizeof(dnx_algo_port_db_tm_t));
    tm_db.header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.set(unit, tm_handle.h0, tm_handle.h1, &tm_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in pp DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_pp_init(
    int unit,
    dnx_algo_port_db_2d_handle_t pp_handle)
{
    dnx_algo_port_db_pp_t pp_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&pp_db, 0, sizeof(dnx_algo_port_db_pp_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.set(unit, pp_handle.h0, pp_handle.h1, &pp_db));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp_alloc.free_single(unit, pp_handle.h0, pp_handle.h1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in scheduler DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_sch_init(
    int unit,
    dnx_algo_port_db_2d_handle_t sch_handle)
{
    dnx_algo_port_db_sch_t sch_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sch_db, 0, sizeof(dnx_algo_port_db_sch_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.set(unit, sch_handle.h0, sch_handle.h1, &sch_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize PRT ETH, PTCH1 and PTCH2 template managers
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_db_first_header_size_init_templates(
    int unit)
{
    dnx_algo_template_create_data_t first_header_size_profile_data;
    dnx_algo_skip_first_header_info_t port_first_header_size_data;
    uint32 nof_references;
    SHR_FUNC_INIT_VARS(unit);

    /** Calculate nof_references */
    nof_references = dnx_data_port.general.nof_tm_ports_get(unit) * dnx_data_device.general.nof_cores_get(unit);

    sal_memset(&port_first_header_size_data, 0, sizeof(dnx_algo_skip_first_header_info_t));

    /** Create template for ETH, PTCH1 and PTCH2 ports */
    sal_memset(&first_header_size_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    first_header_size_profile_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    first_header_size_profile_data.first_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1;
    /** Using additional profile as dummy profile*/
    first_header_size_profile_data.nof_profiles = DNX_ALGO_PORT_MAX_FIRST_HEADER_SIZE_PROFILES + 1;
    first_header_size_profile_data.max_references = nof_references;
    first_header_size_profile_data.default_profile = DNX_ALGO_PORT_FIRST_HEADER_SIZE_DEFAULT_PROFILE;
    first_header_size_profile_data.data_size = sizeof(dnx_algo_skip_first_header_info_t);
    first_header_size_profile_data.default_data = &port_first_header_size_data;
    sal_strncpy(first_header_size_profile_data.name, DNX_ALGO_PORT_FIRST_HEADER_SIZE_PROFILE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.
                    skip_first_user_defined_header_size_profile_data.create(unit, &first_header_size_profile_data,
                                                                            NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_init(
    int unit)
{
    bcm_port_t logical_port;
    int nof_fabric_links, nof_clients;
    int nof_nif_phys;
    int nof_tm_ports, nof_pp_ports, nof_hrs;
    int nof_cores;
    int nof_ilkn_if;
    int phy;
    dnx_algo_port_db_2d_handle_t if_id;
    int port_id;
    int channel;
    bcm_core_t core_id;
    dnx_algo_res_create_data_t create_data;
    int nof_interfaces_handle0, nof_interfaces_handle1;
    SHR_FUNC_INIT_VARS(unit);

    if (sw_state_is_warm_boot(unit))
    {
       /** restore soc info data */
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_restore(unit));
    }
    else
    {
        /*
         * Create SW State instance
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.init(unit));

        /*
         * Allocate SW STATE DBs
         */
        {
            /*
             *  pp alloc init
             */
            sal_memset(&create_data, 0, sizeof(create_data));
            create_data.first_element = 0;
            create_data.nof_elements = dnx_data_port.general.nof_pp_ports_get(unit);
            sal_strncpy(create_data.name, "PP_PORT_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp_alloc.alloc(unit, dnx_data_device.general.nof_cores_get(unit)));
            for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.pp_alloc.create(unit, core_id, &create_data, NULL));
            }

            /*
             *  fabric init
             *  allocate db for each fabric link
             */
            nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.alloc(unit, nof_fabric_links));

            /*
             * nif init
             * allocate db for each nif phy
             * allocate entries for fabric phys for ILKN over fabric
             */
            nof_nif_phys = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.alloc(unit, nof_nif_phys));

            /*
             * ILKN init
             * allocate db for each ILKN id
             */
            nof_ilkn_if = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.alloc(unit, nof_ilkn_if));

            /*
             *  tm ports init
             *  allocate db for each core x tm port
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.alloc(unit, nof_cores, nof_tm_ports));

            /*
             *  pp ports init
             *  allocate db for each core x pp port
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_pp_ports = dnx_data_port.general.nof_pp_ports_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.alloc(unit, nof_cores, nof_pp_ports));
            /*
             *  sch init
             *  allocate db for each core x HR
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_hrs = dnx_data_sch.flow.nof_hr_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.alloc(unit, nof_cores, nof_hrs));
            /*
             *  sch IF init
             *  allocate sch interface for each nif phy, special tm interfaces.
             */
            nof_interfaces_handle0 = DNX_ALGO_PORT_TYPE_NOF;
            nof_interfaces_handle1 = 0;
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_NIF_ETH_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_NIF_ILKN_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_ERP_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY_MIRROR_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OLP_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OAMP_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_SAT_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_EVENTOR_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_CLIENT_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_SAR_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_MAC_SIZE(unit));
            nof_interfaces_handle1 =
                UTILEX_MAX(nof_interfaces_handle1, DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CRPS_SIZE(unit));

            SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.alloc(unit, nof_interfaces_handle0, nof_interfaces_handle1));

            /*
             * tm interface init
             * allocate tm interface for each nif phy, special tm interfaces.
             */

            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.alloc(unit, nof_interfaces_handle0, nof_interfaces_handle1));

            /*
             * Interface init
             * allocate tm interface for each nif phy, special tm interfaces.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.alloc(unit, nof_interfaces_handle0, nof_interfaces_handle1));

            /*
             *  flexe client init
             *  allocate db for each flexe client
             */
            nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.alloc(unit, nof_clients));

            nof_clients = dnx_data_nif.flexe.nof_flexe_clients_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.alloc(unit, nof_clients));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.alloc(unit, nof_clients));

            if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
            {
                nof_nif_phys = dnx_data_nif.phys.nof_phys_get(unit);
                SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.alloc(unit, nof_nif_phys));
            }

        }

        /*
         * Set default values for DBs
         */
        /** logicals init - set handles to -1 */
        for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_logical_init(unit, logical_port));
        }
        /** NIF init - init the NIF info DB */
        for (phy = 0; phy < nof_nif_phys; ++phy)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_nif_init(unit, phy));
            if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db_flexe_phy_init(unit, phy));
            }
        }
        /** FlexE mac db init */
        nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
        for (channel = 0; channel < nof_clients; ++channel)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_flexe_mac_init(unit, channel));
        }

        /** Interface init - init the NIF info DB */
        for (if_id.h0 = 0; if_id.h0 < nof_interfaces_handle0; ++if_id.h0)
        {
            for (if_id.h1 = 0; if_id.h1 < nof_interfaces_handle1; ++if_id.h1)
            {

                SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_init(unit, if_id));
                SHR_IF_ERR_EXIT(dnx_algo_port_db_tm_interface_init(unit, if_id));
                SHR_IF_ERR_EXIT(dnx_algo_port_db_sch_if_init(unit, if_id));
            }
        }
        /** PRD Port to Profile Map init - init the port profile info DB */
        for (port_id = 0; port_id < SOC_MAX_NUM_PORTS; ++port_id)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_prd_port_to_profile_map_init(unit, port_id));
        }
        /*
         * Init SOC INFO
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_init(unit));
        /** Create default template managers for ETH, PTCH1 and PTCH2 ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_db_first_header_size_init_templates(unit));

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_init(
    int unit,
    int *is_init)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.is_init(unit, (uint8 *) is_init));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Add / Remove Port
 * {
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_allocate(
    int unit,
    uint32 flags,
    bcm_core_t core,
    bcm_trunk_t lag_id,
    uint32 *pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    uint32 supported_flags;
    uint32 res_mngr_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    pp_handle.h0 = core;
    pp_handle.h1 = DNX_ALGO_PORT_INVALID;

    /*
     * Verify
     */
    /** verify flags */
    supported_flags = DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG | DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_WITH_ID;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "pp port allocate does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);
    /*
     * verify core
     */
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    /** Find new free pp port */
    if (flags & DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_WITH_ID)
    {
        res_mngr_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_port_db.
                             pp_alloc.allocate_single(unit, core, res_mngr_flags, NULL, (int *) pp_port),
                             "Free PP port not found in core %d\n%s%s", core, EMPTY, EMPTY);
    pp_handle.h1 = *pp_port;

    /*
     * allocate the port
     */
    /** mark as valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.set(unit, pp_handle.h0, pp_handle.h1, 1));
    /** Set pp port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.set(unit, pp_handle.h0, pp_handle.h1, *pp_port));
    /** Set core */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.core.set(unit, pp_handle.h0, pp_handle.h1, core));
    /** mark LLVP profile as invalid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ingress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 0));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.egress_llvp_profile_valid.set(unit, pp_handle.h0, pp_handle.h1, 0));

    if (flags & DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG)
    {
        /** Set lag pp port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.set(unit, pp_handle.h0, pp_handle.h1, 1));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.lag_id.set(unit, pp_handle.h0, pp_handle.h1, lag_id));
    }

    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, core, *pp_port, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_free(
    int unit,
    bcm_core_t core,
    uint32 pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle, other_pp_handle;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    /** create handle */
    pp_handle.h0 = core;
    pp_handle.h1 = pp_port;

    /*
     * Verify
     */
    /** verify pp port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));

    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, core, pp_port, FALSE));

    /** Make sure that there are no logical ports with the same handle */
    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &other_pp_handle));

        /** Stop when found a free port */
        if (DNX_ALGO_PORT_2D_HANDLE_EQ(pp_handle, other_pp_handle))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "pp port used by logical port %d\n", logical_port);
        }
    }

   /** free resource */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_pp_init(unit, pp_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create and set general interface database
 */
static int
dnx_algo_port_db_interface_config_set(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_db_2d_handle_t interface_id)
{
    int valid_interface;
    SHR_FUNC_INIT_VARS(unit);

    /** Assign logical port to general interface database */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.set(unit, logical_port, &interface_id));

    /*
     * If  interface_id is allocated -
     * assume that it's channelized interface with already configured interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.valid.get(unit, interface_id.h0, interface_id.h1, &valid_interface));
    if (!valid_interface) /** i.e. master port */
    {
        /** set as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.valid.set(unit, interface_id.h0, interface_id.h1, 1));
        /** Set interface core */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.set(unit, interface_id.h0, interface_id.h1, core));
        /** set master logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                        master_logical_port.set(unit, interface_id.h0, interface_id.h1, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Add new NIF port
 * Relevant for both ethrnet port and interlaken ports.
 * (Configure NIF and ILKN data bases only)
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - required logical port.
 *   \param [in] nif_handle - nif DB handle
 *   \param [in] port_info - see dnx_algo_port_info_s
 *   \param [in] interface_offset - interface offset #
 *   \param [in] phys - bitmap of phy ports
 *   \param [in] phy_offset - offset for phys bitmap (used for ILKN over fabric)
 *   \param [in] ilkn_lanes - for ILKN only. bitmap of ILKN lanes (bits 0-23)
 *   \param [in] is_ilkn_over_fabric - for ILKN only. specify whether ILKN is over fabric.
 *   \param [in] is_elk - for ILKN only. specify whether ILKN it is elk interface.
 *
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_nif_internal_add(
    int unit,
    bcm_port_t logical_port,
    int nif_handle,
    dnx_algo_port_info_s port_info,
    int interface_offset,
    bcm_pbmp_t phys,
    int phy_offset,
    bcm_pbmp_t ilkn_lanes,
    int is_ilkn_over_fabric,
    int is_elk)
{
    int valid_nif_port;
    int phy;
    int first_phy;
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** set logical port as valid and set interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_ADDED));

    /** set port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.set(unit, logical_port, port_info));

    /** Assign logical port to nif */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.set(unit, logical_port, nif_handle));

    /** Assign logical port to ilkn-id (just for ilkn of course) */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.set(unit, logical_port, interface_offset));
    }

    /** set interface offset */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.set(unit, logical_port, interface_offset));
    /*
     * If nif_handle_port is allocated -
     * assume that it's channelized interface with already configured NIF
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, nif_handle, &valid_nif_port));
    if (!valid_nif_port) /** i.e. master port */
    {
        /** set phys bitmap */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.set(unit, nif_handle, phys));
        /** set first phy */
        _SHR_PBMP_FIRST(phys, first_phy);
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.first_phy.set(unit, nif_handle, first_phy));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        {
            /** set ILKN lanes **/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.lanes.set(unit, interface_offset, ilkn_lanes));

            /** set ILKN over fabric indication **/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_over_fabric.set(unit, interface_offset, is_ilkn_over_fabric));

            /** ILKN master and valid */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.valid.set(unit, interface_offset, 1));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.set(unit, interface_offset, logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_elk.set(unit, interface_offset, is_elk));
        }

        /*
         * set valid and master logical port for all phys
         * Relevant fields for non master phys are: valid, master_logical_port.
         */
        BCM_PBMP_ITER(phys, phy)
        {
            /** set as valid */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.set(unit, phy + phy_offset, 1));

            /** set master port */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.set(unit, phy + phy_offset, logical_port));
        }
    }

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        flexe_handle = nif_handle;
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_phy_handle.set(unit, logical_port, flexe_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.valid.set(unit, flexe_handle, 1));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.master_logical_port.set(unit, flexe_handle, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phy_active_get(
    int unit,
    int logical_phy,
    int *is_active)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, logical_phy, is_active));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_if_add_t * info)
{
    int first_phy_port;
    int phy_offset = 0;
    dnx_algo_port_db_2d_handle_t interface_id, tm_interface_id;
    int is_master_port;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_info_indicator_t indicator;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** verify logical port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, logical_port));

    /** add verify  */
    sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    indicator.is_elk = info->ilkn_info.is_elk_if;
    indicator.is_stif = info->eth_info.is_stif;
    indicator.is_stif_data = info->eth_info.is_stif_data;
    indicator.is_flexe_phy = info->eth_info.is_flexe_phy;
    indicator.is_cross_connect = info->is_cross_connect;

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, info->interface, &indicator, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_add_verify(unit, logical_port, port_info, info));

    _SHR_PBMP_FIRST(info->phys, first_phy_port);

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        if (info->ilkn_info.is_ilkn_over_fabric)
        {
            phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
            /** Add fabric links offset for NIF DB handle */
            first_phy_port += phy_offset;
        }
    }

    /** Verify channels just for TM ports */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        /** tm port Verify */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_free_verify(unit, port_info, info->core, info->tm_info.tm_port));

        SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                        (unit, port_info, info->core, info->interface_offset, &info->phys, info->tm_info.channel,
                         &tm_interface_id.h0, &tm_interface_id.h1));

        /** channel verify */
        if (info->tm_info.is_channelized)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_free_verify
                            (unit, logical_port, tm_interface_id, info->tm_info.channel));
        }
    }

    /*
     * Add to data base
     */
    /** Add general interface DataBase */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                    (unit, port_info, info->core, info->interface_offset, &info->phys, -1, &interface_id.h0,
                     &interface_id.h1));

    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_config_set(unit, logical_port, info->core, interface_id));
    /** NIF PORT*/
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_internal_add
                    (unit, logical_port, first_phy_port, port_info, info->interface_offset, info->phys,
                     phy_offset, info->ilkn_info.ilkn_lanes, info->ilkn_info.is_ilkn_over_fabric,
                     info->ilkn_info.is_elk_if));

    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        /** TM PORT */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_add(unit, logical_port, info->core, &info->tm_info, interface_id));
    }

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        /** TM PORT */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_add(unit, logical_port, info->core, &info->tm_info, interface_id));
    }

    /*
     * Set SOC INFO.
     * Until TDM mode will be set assume the port is not TDM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add
                    (unit, logical_port, is_master_port, port_info, info->phys, info->tm_info.tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add new TM port
 *   Relevant for any port type which has tm port (nif, cpu, ..)
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port.
 * \param [in] core - core ID.
 * \param [in] info - Switch port info.
 * \param [in] interface_id - See macros DNX_ALGO_PORT_TM_INTERFACE_*
 *
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * INDIRECT INPUT:
 *   * * dnx data
 *   * * internal algo port data base
 *   * INDIRECT OUTPUT
 *   * * internal algo port data base
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_tm_add(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_tm_add_t * info,
    dnx_algo_port_db_2d_handle_t interface_id)
{
    int valid_tm_interface;
    uint32 pp_port = DNX_ALGO_PORT_TYPE_INVALID;
    dnx_algo_port_db_2d_handle_t tm_handle, pp_handle;
    dnx_algo_port_info_s port_info;
    int tm_port_valid;
    bcm_port_t master_logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    /** Assign logical port to tm port */
    tm_handle.h0 = core;
    tm_handle.h1 = info->tm_port;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.set(unit, logical_port, &tm_handle));

    /** Assign logical port to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.set(unit, logical_port, &interface_id));

    /** Set channel_id */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.channel.set(unit, logical_port, info->channel));

    /*
     * Configure TM port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, tm_handle.h0, tm_handle.h1, &tm_port_valid));
    if (!tm_port_valid)
    {
        /** mark as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.set(unit, tm_handle.h0, tm_handle.h1, 1));
        /** Set tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tm_port.set(unit, tm_handle.h0, tm_handle.h1, info->tm_port));
        /** Set core */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.core.set(unit, tm_handle.h0, tm_handle.h1, core));
        /** set master port (of tm port!)*/
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.set(unit, tm_handle.h0, tm_handle.h1, logical_port));

        /** Egress queuing port only */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            /** Set base_q_pair */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.base_q_pair.set(unit, tm_handle.h0, tm_handle.h1, info->base_q_pair));
            /** Set number of port priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.priorities.set(unit, tm_handle.h0, tm_handle.h1, info->num_priorities));

        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.
                            base_q_pair.set(unit, tm_handle.h0, tm_handle.h1, DNX_ALGO_PORT_TYPE_INVALID));
        }
    }
    else
    {
        /** increment reference  count */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.inc(unit, tm_handle.h0, tm_handle.h1, 1));

    }

    /*
     * If tm_interface_id is allocated -
     * assume that it's channelized interface with already configured interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    valid.get(unit, interface_id.h0, interface_id.h1, &valid_tm_interface));
    if (!valid_tm_interface) /** i.e. master port */
    {
        /** set as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.valid.set(unit, interface_id.h0, interface_id.h1, 1));
        /** Set is channelized */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        is_channelized.set(unit, interface_id.h0, interface_id.h1, info->is_channelized));
        /** Set is ingress interleave */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_ingress_interleave.set
                        (unit, interface_id.h0, interface_id.h1, info->is_ingress_interleave));
        /** Set is egress interleave */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_egress_interleave.set
                        (unit, interface_id.h0, interface_id.h1, info->is_egress_interleave));
        /** Set egress interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        egr_if.set(unit, interface_id.h0, interface_id.h1, info->egress_interface));
        /** set master logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_logical_port.set(unit, interface_id.h0, interface_id.h1, logical_port));
        /** TDM interface mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        tdm_mode.set(unit, interface_id.h0, interface_id.h1, info->if_tdm_mode));
        /** set tdm master port to be invalid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_tdm_logical_port.set(unit, interface_id.h0, interface_id.h1, DNX_ALGO_PORT_INVALID));
        /** set non-tdm master port to be this port - assuming it's non tdm port for now */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_non_tdm_logical_port.set(unit, interface_id.h0, interface_id.h1, logical_port));
    }
    else
    {
        int is_channelized, is_ingress_interleave, is_egress_interleave, egress_interface, if_master_port;
        dnx_algo_port_if_tdm_mode_e if_tdm_mode;
        dnx_algo_port_info_s master_port_info;
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_logical_port.get(unit, interface_id.h0, interface_id.h1, &if_master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, if_master_port, &master_port_info));

        /** Do not allow to add channels to non-channlized interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        is_channelized.get(unit, interface_id.h0, interface_id.h1, &is_channelized));
        if (!is_channelized)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d cannot be added existing non-channlized interface \n", logical_port);
        }

        /** make sure the interface attributes match */
        /** is channelized */
        if (info->is_channelized != is_channelized)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "is_channelized attribute of port %d must match to all the ports over the interface \n",
                         logical_port);
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_ingress_interleave.get(unit,
                                                                                interface_id.h0, interface_id.h1,
                                                                                &is_ingress_interleave));
        /** is ingress interleaved */
        if (info->is_ingress_interleave != is_ingress_interleave)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "is_ingress_interleave attribute of port %d must match to all the ports over the interface \n",
                         logical_port);
        }

        /*
         * Set master_logical_port, is_egress_interleave and egr_if when adding first L2 port
         */
        if (!DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) && DNX_ALGO_PORT_TYPE_IS_L1(unit, master_port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                            master_logical_port.set(unit, interface_id.h0, interface_id.h1, logical_port));

            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                            is_egress_interleave.set(unit, interface_id.h0, interface_id.h1,
                                                     info->is_egress_interleave));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                            egr_if.set(unit, interface_id.h0, interface_id.h1, info->egress_interface));
            /** set non-tdm master port to be this port - assuming it's non tdm port for now */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                            master_non_tdm_logical_port.set(unit, interface_id.h0, interface_id.h1, logical_port));
        }
        else if (!DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
        {
            /** is egress interleave */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_egress_interleave.get
                            (unit, interface_id.h0, interface_id.h1, &is_egress_interleave));
            if (info->is_egress_interleave != is_egress_interleave)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "is_egress_interleave attribute of port %d must match to all the ports over the interface \n",
                             logical_port);
            }
            /** egress interface */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                            egr_if.get(unit, interface_id.h0, interface_id.h1, &egress_interface));
            if (egress_interface != info->egress_interface)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "egress_interface attribute of port %d must match to all the ports over the interface \n",
                             logical_port);
            }
        }

        /** TDM interface mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        tdm_mode.get(unit, interface_id.h0, interface_id.h1, &if_tdm_mode));
        if (if_tdm_mode != info->if_tdm_mode)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "if_tdm_mode attribute of port %d must match to all the ports over the interface \n",
                         logical_port);
        }
    }

    /*
     * Configure PP port
     */
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, 0 /* not in lag */ ))
    {
        /*
         * This code assumes that any PP port is also TM port.
         * This assumption is correct with the current code..
         * If this assumption is ever violated, the code below should be adjusted
         */
        if (!tm_port_valid)
        {
            /*
             * Allocate new pp port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, 0, core, 0, &pp_port));
            pp_handle.h0 = core;
            pp_handle.h1 = pp_port;

            /** set master port (of pp port!)*/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.set(unit, pp_handle.h0, pp_handle.h1,
                                                                        logical_port));
        }
        else
        {
            /** get master port (from TM DB) */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.get(unit, tm_handle.h0, tm_handle.h1,
                                                                        &master_logical_port));
            /** Get the handle from master PP port */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, master_logical_port, &pp_handle));

            /** increment reference count in PP DB */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.inc(unit, pp_handle.h0, pp_handle.h1, 1));

        }

        /** Assign logical port to pp port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.set(unit, logical_port, &pp_handle));
    }

    /*
     * Configure TDM mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_set(unit, logical_port, info->tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add new sch port
 *   Relevant for any port type which has sch database (nif, cpu, ..)
 *
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port.
 * \param [in] core - core ID.
 * \param [in] info - Switch port info.
 * \param [in] interface_id - See macros DNX_ALGO_PORT_TM_INTERFACE_*
 *
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * INDIRECT INPUT:
 *   * * dnx data
 *   * * internal algo port data base
 *   * INDIRECT OUTPUT
 *   * * internal algo port data base
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_sch_add(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_tm_add_t * info,
    dnx_algo_port_db_2d_handle_t interface_id)
{
    int valid_sch_interface;
    dnx_algo_port_db_2d_handle_t sch_handle;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /** Assign logical port to sch */
    sch_handle.h0 = core;
    sch_handle.h1 = info->base_hr;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.set(unit, logical_port, &sch_handle));

    /** Assign logical port to sch interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.set(unit, logical_port, &interface_id));

    /*
     * Configure SCH
     */

    /** Configure SCH info for all port's HRs */
    for (i = 0; i < info->sch_priorities; i++)
    {
        /** mark as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.valid.set(unit, sch_handle.h0, sch_handle.h1 + i, 1));
        /** Set base_hr */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.base_hr.set(unit, sch_handle.h0, sch_handle.h1 + i, info->base_hr));
        /** Set number of sch priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.set(unit, sch_handle.h0, sch_handle.h1 + i,
                                                                info->sch_priorities));
        /** set master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.master_logical_port.set(unit, sch_handle.h0, sch_handle.h1 + i,
                                                                     logical_port));
    }

    /** Configure SCH interface info */

    /*
     * If sch_interface is valid -
     * assume that it's channelized interface with already configured interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                    valid.get(unit, interface_id.h0, interface_id.h1, &valid_sch_interface));
    if (!valid_sch_interface) /** i.e. master port */
    {
        /** set as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.valid.set(unit, interface_id.h0, interface_id.h1, TRUE));
        /** Set priority propagation */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                        priority_propagation_enable.set(unit, interface_id.h0, interface_id.h1,
                                                        info->sch_priority_propagation_enable));
        /** set master logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                        master_logical_port.set(unit, interface_id.h0, interface_id.h1, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_port_flexe_add(
    int unit,
    bcm_port_t logical_port,
    int flexe_index)
{
    dnx_algo_port_info_s port_info;
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);

    flexe_handle = flexe_index;

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_client_handle.set(unit, logical_port, flexe_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.client_index.set(unit, flexe_handle, flexe_index));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.master_logical_port.set(unit, flexe_handle, logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.valid.set(unit, flexe_handle, 1));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_mac_handle.set(unit, logical_port, flexe_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.client_index.set(unit, flexe_handle, flexe_index));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.master_logical_port.set(unit, flexe_handle, logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.valid.set(unit, flexe_handle, 1));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_sar_handle.set(unit, logical_port, flexe_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.client_index.set(unit, flexe_handle, flexe_index));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.master_logical_port.set(unit, flexe_handle, logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.valid.set(unit, flexe_handle, 1));
    }

    /*
     * Set channel on logical DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.channel.set(unit, logical_port, flexe_index));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h
 */
shr_error_e
dnx_algo_port_db_interface_id_get(
    int unit,
    dnx_algo_port_info_s port_info,
    bcm_core_t core,
    int interface_offset,
    bcm_pbmp_t * phys,
    int flexe_client_channel,
    int *if_id_h0,
    int *if_id_h1)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 1, 1, 1, 1))
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        {
            *if_id_h0 = DNX_ALGO_PORT_TYPE_NIF_ILKN;
            *if_id_h1 = interface_offset;
        }
        else
        {
            *if_id_h0 = DNX_ALGO_PORT_TYPE_NIF_ETH;
            _SHR_PBMP_FIRST(*phys, *if_id_h1);
        }
    }
    else
    {
        /*
         * Get interface id and port type
         */
        *if_id_h0 = port_info.port_type;
        switch (port_info.port_type)
        {
            case DNX_ALGO_PORT_TYPE_CPU:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CPU(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_RCY:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY(unit, core, interface_offset);
                break;
            }
            case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY_MIRROR(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_ERP:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_ERP(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_OLP:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OLP(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_OAMP:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_OAMP(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_SAT:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_SAT(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_EVENTOR:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_EVENTOR(unit, core);
                break;
            }
            case DNX_ALGO_PORT_TYPE_FLEXE_CLIENT:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_CLIENT(unit, flexe_client_channel);
                break;
            }
            case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_MAC(unit, flexe_client_channel);
                break;
            }
            case DNX_ALGO_PORT_TYPE_FLEXE_SAR:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_FLEXE_SAR(unit, flexe_client_channel);
                break;
            }

            case DNX_ALGO_PORT_TYPE_SCH_ONLY:
            {
                int base_interface, index, valid;

                *if_id_h1 = -1;

                base_interface = DNX_ALGO_PORT_INTERFACE_HANDLE1_BASE_FOR_SCH_ONLY(unit, core);
                for (index = 0; index < dnx_data_sch.interface.nof_sch_interfaces_get(unit); index++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                                    valid.get(unit, DNX_ALGO_PORT_TYPE_SCH_ONLY, base_interface + index, &valid));
                    if (!valid)
                    {
                        *if_id_h1 = base_interface + index;
                        break;
                    }

                }

                if (*if_id_h1 == -1)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "Cannot allocate SCH ONLY interface id\n");

                }
                break;
            }
            case DNX_ALGO_PORT_TYPE_CRPS:
            {
                *if_id_h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CRPS(unit, core);
                break;
            }

            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported interface type for port\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - input verification for special interface adding
 */
static shr_error_e
dnx_algo_port_special_interface_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_special_interface_add_t * info)
{
    bcm_core_t core;
    dnx_algo_port_db_2d_handle_t cpu_interface_id, rcy_interface_id;
    int interface_offset;
    dnx_algo_port_db_2d_handle_t interface_id;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_info_indicator_t indicator;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get tm interface id and port type
     */
    sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    indicator.is_flexe_mac = info->flexe_info.is_flexe_mac;
    indicator.is_flexe_sar = info->flexe_info.is_flexe_sar;
    indicator.is_cross_connect = info->flexe_info.is_cross_connet;
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, info->interface, &indicator, &port_info));
    /*
     * No need to verify flexe here, set "flexe_client_channel" to 0.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                    (unit, port_info, info->core, info->interface_offset, NULL, 0, &interface_id.h0, &interface_id.h1));

    /*
     * Verify
     */
    /** Verify logical port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, logical_port));
    /** Core verify*/
    DNXCMN_CORE_VALIDATE(unit, info->core, 0);
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        /** Verify tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_free_verify(unit, port_info, info->core, info->tm_info.tm_port));
        /** Verify interface offset */
        SHR_IF_ERR_EXIT(dnx_algo_port_special_interface_offset_verify(unit, port_info, info->interface_offset));
    }
    /** Verify channel */
    if (info->tm_info.is_channelized)
    {
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            /** special check for cpu - make sure that the channel is not used on all cores */
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                cpu_interface_id.h0 = interface_id.h0;
                cpu_interface_id.h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_CPU(unit, core);
                SHR_IF_ERR_EXIT(dnx_algo_port_channel_free_verify
                                (unit, logical_port, cpu_interface_id, info->tm_info.channel));
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        {
            /** special check for recycle - make sure that the channel is not used on all interface offsets */
            for (interface_offset = 0;
                 interface_offset < dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit); interface_offset++)
            {
                rcy_interface_id.h0 = interface_id.h0;
                rcy_interface_id.h1 = DNX_ALGO_PORT_INTERFACE_HANDLE1_FOR_RCY(unit, info->core, interface_offset);
                SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_port_channel_free_verify
                                         (unit, logical_port, rcy_interface_id, info->tm_info.channel),
                                         "Port %d channel %d is already used by one of the RCY interfaces. (RCY interfaces shared the channel ids).\n%s",
                                         logical_port, info->tm_info.channel, EMPTY);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_free_verify(unit, logical_port, interface_id, info->tm_info.channel));
        }
    }
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_add_verify(unit, logical_port, port_info, interface_id, &info->tm_info));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_special_interface_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_special_interface_add_t * info)
{
    dnx_algo_port_db_2d_handle_t interface_id;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t dummy_pbmp;
    int is_master_port;
    dnx_algo_port_info_indicator_t indicator;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_port_special_interface_add_verify(unit, logical_port, info));

    sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    indicator.is_flexe_mac = info->flexe_info.is_flexe_mac;
    indicator.is_flexe_sar = info->flexe_info.is_flexe_sar;
    indicator.is_cross_connect = info->flexe_info.is_cross_connet;
    /*
     * Get interface id and port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, info->interface, &indicator, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                    (unit, port_info, info->core, info->interface_offset, NULL, info->flexe_info.client_channel,
                     &interface_id.h0, &interface_id.h1));

    /*
     * Add to data base
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_ADDED));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.set(unit, logical_port, port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.set(unit, logical_port, info->interface_offset));

    /** Add general interface Data Base */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_config_set(unit, logical_port, info->core, interface_id));
    /** add tm info */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_add(unit, logical_port, info->core, &info->tm_info, interface_id));
    }

    /** add sch info */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_add(unit, logical_port, info->core, &info->tm_info, interface_id));
    }

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, TRUE, FALSE))
    {
        /** FLEXE */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_add(unit, logical_port, info->flexe_info.client_channel));
    }

    /*
     *  Set SOC INFO
     */
    BCM_PBMP_CLEAR(dummy_pbmp);
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add
                    (unit, logical_port, is_master_port, port_info, dummy_pbmp, info->tm_info.tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_only_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tm_add_t * tm_info)
{
    dnx_algo_port_db_2d_handle_t interface_handle;
    dnx_algo_port_info_s port_info;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get parameters from the DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(interface_handle);

    /** Get core */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, interface_handle.h0, interface_handle.h1, &core));

    /*
     * Update port info in the data base
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
    port_info.is_sch = 1;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.set(unit, logical_port, port_info));

    /** add sch info */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_add(unit, logical_port, core, tm_info, interface_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_and_channel_to_logical_get(
    int unit,
    int core_id,
    dnx_algo_port_info_s port_info,
    int channel_id,
    bcm_port_t * logical_port)
{
    dnx_algo_port_info_s port_info_i;
    bcm_port_t logical_port_i;
    bcm_pbmp_t logical_ports;
    int channel_i;
    bcm_core_t core_i;
    SHR_FUNC_INIT_VARS(unit);

    *logical_port = DNX_ALGO_PORT_INVALID;

    /*
     * iterate over valid logical ports bitmap and compare channel and type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0,
                                               &logical_ports));
    BCM_PBMP_ITER(logical_ports, logical_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port_i, &port_info_i));
        if (port_info_i.port_type == port_info.port_type)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port_i, &core_i));
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port_i, &channel_i));
            if ((core_id == core_i) && (channel_id == channel_i))
            {
                *logical_port = logical_port_i;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_phy_active_get(
    int unit,
    int fabric_phy,
    int *is_active)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.valid.get(unit, fabric_phy, is_active));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_fabric_add_t * info)
{
    bcm_pbmp_t dummy_pbmp;
    dnx_algo_port_info_s port_info_fabric;
    dnx_algo_port_info_indicator_t indicator;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, logical_port));

    /** Verify that fabric link id is supported by device*/
    /** TBD SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_supported_verify(unit, info->interface, info->interface_offset)); */

    /*
     * Add to data base
     */
    /** Logical DB - port type and interface type*/
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_ADDED));
    sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, BCM_PORT_IF_SFI, &indicator, &port_info_fabric));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.set(unit, logical_port, port_info_fabric));

    /** Fabric data base (handle, mark as valid and link id */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.set(unit, logical_port, info->link_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.valid.set(unit, info->link_id, 1));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.link.set(unit, info->link_id, info->link_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.master_logical_port.set(unit, info->link_id, logical_port));

    /*
     *  Set SOC INFO
     */
    BCM_PBMP_CLEAR(dummy_pbmp);
    /** Set logical_port in bitmap, even though we currently don't use it */
    BCM_PBMP_PORT_ADD(dummy_pbmp, logical_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add
                    (unit, logical_port, 1, port_info_fabric, dummy_pbmp, DNX_ALGO_PORT_TDM_MODE_NONE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - update sch DBs when removing port (or SCH from the port)
 */
static shr_error_e
dnx_algo_port_remove_update_sch_db(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    dnx_algo_port_db_2d_handle_t sch_if_handle;
    bcm_port_t next_master_port = DNX_ALGO_PORT_INVALID, current_master_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Release SCH Data Base */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(sch_handle))
    {
        int i, sch_priorities;
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.get(unit, sch_handle.h0, sch_handle.h1, &sch_priorities));
        for (i = 0; i < sch_priorities; i++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_sch_init(unit, sch_handle));
            sch_handle.h1++;
        }
    }

    /** Scheduler interface Data Base */
    /** SCH master might be different than master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_if_handle));
    if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(sch_if_handle))
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, DNX_ALGO_PORT_MASTER_F_SCH, &current_master_port));
        if (logical_port == current_master_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port,
                                                     DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_SCH,
                                                     &next_master_port));
            if (next_master_port == DNX_ALGO_PORT_INVALID)
            {
                /** Release scheduler interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db_sch_if_init(unit, sch_if_handle));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.master_logical_port.set(unit, sch_if_handle.h0,
                                                                                       sch_if_handle.h1,
                                                                                       next_master_port));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_remove(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    dnx_algo_port_info_s port_info;
    int is_master_port;
    int nif_handle;
    int ilkn_handle;
    int fabric_handle;
    int flexe_handle;
    dnx_algo_port_db_2d_handle_t tm_handle, pp_handle;
    dnx_algo_port_db_2d_handle_t tm_interface_handle, interface_handle;
    bcm_port_t next_master_port = DNX_ALGO_PORT_INVALID;
    bcm_pbmp_t phy_ports;
    int is_lag;
    int phy;
    int client_channel;

    int tm_ref_count;
    bcm_port_t tm_master_logical_port, new_tm_master_logical_port = -1, logical_port_i;
    bcm_pbmp_t logical_port_channels;
    dnx_algo_port_db_2d_handle_t tm_handle_i;

    bcm_port_t pp_master_logical_port;
    int pp_ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));

    /*
     * See if current logical port is master
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));

    /*
     * If port is valid (i.e. removing existing port)
     * Remove non-logical data base in case it's the master port
     */
    if (state != DNX_ALGO_PORT_STATE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
        /*
         * if it is tm port
         * first set tdm mode to be invalid, which will update tdm and non-tdm master ports
         */
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_set(unit, logical_port, DNX_ALGO_PORT_TDM_MODE_INVALID));
        }

        if (is_master_port)
        {
            /*
             * If we remove port that is currently the master port. We need to find the next master.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                            (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
        }
        else
        {
            /*
             *  In case the current logical_port that will be removed is not Master port,
             *  we will set "next_master_port" to be same as the current master port.
             *  Current master port should not be changed.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &next_master_port));
        }

        /*
         * Remove port from SOC INFO
         */
        BCM_PBMP_CLEAR(phy_ports);
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phy_ports));
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_remove(unit, logical_port, port_info, next_master_port, phy_ports));

        /*
         * Remove the port from 'e' bitmap
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, ETHER_PTYPE, logical_port));

        /*
         * Remove the prd port profile mapping
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_prd_port_to_profile_map_init(unit, logical_port));
        }
        /*
         * Release data base which are one per logical ports
         */
        /** Fabric Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        if (fabric_handle != DNX_ALGO_PORT_HANDLE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_fabric_init(unit, fabric_handle));
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(tm_handle))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, tm_handle.h0, tm_handle.h1, &tm_ref_count));
            if (tm_ref_count <= 1)
            {
                /** last port pointing to this TM DB -  release TM port Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db_tm_init(unit, tm_handle));
            }
            else
            {
                /** decrement TM DB reference count */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.dec(unit, tm_handle.h0, tm_handle.h1, 1));

                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.get(unit, tm_handle.h0, tm_handle.h1,
                                                                            &tm_master_logical_port));
                if (logical_port == tm_master_logical_port)
                {
                    /*
                     * master port is about to be removed
                     * need to find a new master port
                     */

                    /** get channels ports bitmap */
                    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                                    (unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY, &logical_port_channels));

                    BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                    {
                        /** filter master port */
                        if (logical_port_i == tm_master_logical_port)
                        {
                            continue;
                        }
                        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port_i, &tm_handle_i));
                        if (DNX_ALGO_PORT_2D_HANDLE_EQ(tm_handle_i, tm_handle))
                        {
                            /** new master found - set it to DB */
                            new_tm_master_logical_port = logical_port_i;
                            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.set(unit,
                                                                                        tm_handle.h0, tm_handle.h1,
                                                                                        new_tm_master_logical_port));
                            break;
                        }
                    }
                }
            }
        }

        /** Release PP port Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(pp_handle))
        {
            /** release just in case pp ported wasn't allocated for LAG */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
            if (!is_lag)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, pp_handle.h0, pp_handle.h1, &pp_ref_count));
                if (pp_ref_count <= 1)
                {
                    /** last port pointing to this PP DB -  release PP port Data Base */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_pp_init(unit, pp_handle));
                }
                else
                {
                    /** decrement PP DB reference count */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.dec(unit, pp_handle.h0, pp_handle.h1, 1));

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.get(unit, pp_handle.h0, pp_handle.h1,
                                                                                &pp_master_logical_port));
                    if (logical_port == pp_master_logical_port)
                    {
                        /*
                         * master port is about to be removed.
                         * new master port is set according to TM master port,
                         * as we assume TM master port and PP master port are always equal
                         */

                        /** new master found - set it to DB */
                        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.set(unit, pp_handle.h0, pp_handle.h1,
                                                                                    new_tm_master_logical_port));
                    }
                }
            }
        }

        /** free the channel in allocation manager */
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, TRUE, FALSE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_channel));
            /*
             * Dont need to free client channel for OAM and PTP channel.
             */
            if (!DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_client_channel_free(unit, port_info, client_channel));
            }
        }
        /** Flexe client Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_client_handle.get(unit, logical_port, &flexe_handle));
        if (flexe_handle != DNX_ALGO_PORT_HANDLE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_flexe_client_init(unit, flexe_handle));
        }

        /** Flexe MAC Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_mac_handle.get(unit, logical_port, &flexe_handle));
        if (flexe_handle != DNX_ALGO_PORT_HANDLE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_flexe_mac_init(unit, flexe_handle));
        }

        /** Flexe SAR Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_sar_handle.get(unit, logical_port, &flexe_handle));
        if (flexe_handle != DNX_ALGO_PORT_HANDLE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_flexe_sar_init(unit, flexe_handle));
        }

        /** Flexe phy Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_phy_handle.get(unit, logical_port, &flexe_handle));
        if (flexe_handle != DNX_ALGO_PORT_HANDLE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_flexe_phy_init(unit, flexe_handle));
        }

        /*
         * For data bases which might be relevant more then one logical port.
         * Release it only it there is no other master port.
         * If it master port and there are additional ports - just reset the pointers to the new master port
         */
        if (is_master_port)
        {
            /** get next master */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                            (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));

            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
            if (nif_handle != DNX_ALGO_PORT_HANDLE_INVALID)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, &phy_ports));
            }

            /** if no next master - release all data bases */
            if (next_master_port == DNX_ALGO_PORT_INVALID)
            {
                /** Release NIF Data Base - all related phys */
                BCM_PBMP_ITER(phy_ports, phy)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_nif_init(unit, phy));
                }

                /** Release ILKN Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
                if (ilkn_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_ilkn_init(unit, ilkn_handle));
                }

                /** Release TM interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                                tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
                if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(tm_interface_handle))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_tm_interface_init(unit, tm_interface_handle));
                }

                /** Release interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
                if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(interface_handle))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_init(unit, interface_handle));
                }
            }
            else
            {
                /** update next master port */
                /** NIF Data Base */
                BCM_PBMP_ITER(phy_ports, phy)
                {
                    /** set master port */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.set(unit, phy, next_master_port));
                }

                /** ILKN Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
                if (ilkn_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.set(unit, ilkn_handle, next_master_port));
                }

                /** TM interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                                tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
                if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(tm_interface_handle))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_logical_port.set(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                            next_master_port));
                }

                /** Interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
                if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(interface_handle))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    master_logical_port.set(unit, interface_handle.h0, interface_handle.h1,
                                                            next_master_port));
                }

            }
        }

        /** Release SCH DBs */
        SHR_IF_ERR_EXIT(dnx_algo_port_remove_update_sch_db(unit, logical_port));
    }

    /*
     * Init logical DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_logical_init(unit, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_remove_sch_only(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    dnx_algo_port_db_logical_t logical_db;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));

    /*
     * If port is valid
     */
    if (state != DNX_ALGO_PORT_STATE_INVALID)
    {
        /** Release SCH DBs */
        SHR_IF_ERR_EXIT(dnx_algo_port_remove_update_sch_db(unit, logical_port));

        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.get(unit, logical_port, &logical_db));

        logical_db.sch_if_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
        logical_db.sch_if_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
        logical_db.sch_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
        logical_db.sch_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;

        /** mark the port as non SCH */
        logical_db.info.is_sch = 0;

        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.set(unit, logical_port, &logical_db));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_remove_process_start(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Mark as port under remove process
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_REMOVED));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_removed_port_get(
    int unit,
    bcm_port_t * logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over all ports and return the first port in removed state.
     * (Assuming just one port is in this state)
     */
    for ((*logical_port) = 0; (*logical_port) < SOC_MAX_NUM_PORTS; (*logical_port)++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, *logical_port, &state));
        if (state == DNX_ALGO_PORT_STATE_REMOVED)
        {
            break;
        }
    }

    /** if not found throw an error */
    if (*logical_port == SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "removed port wasn't found");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_added_port_get(
    int unit,
    bcm_port_t * logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over all ports and return the first port in added state.
     * (Assuming just one port is in this state)
     */
    for ((*logical_port) = 0; (*logical_port) < SOC_MAX_NUM_PORTS; (*logical_port)++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, *logical_port, &state));
        if (state == DNX_ALGO_PORT_STATE_ADDED)
        {
            break;
        }
    }

    /** if not found throw an error */
    if (*logical_port == SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "added port wasn't found");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_add_process_done(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify (i.e. make sure the port is not free)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Mark as valid port - added process completed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_VALID));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * Mapping from / to logical port
 * {
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_port_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core,
    uint32 *tm_port)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /** Get core and tm_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.core.get(unit, tm_handle.h0, tm_handle.h1, core));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tm_port.get(unit, tm_handle.h0, tm_handle.h1, tm_port));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_core_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core)
{
    dnx_algo_port_db_2d_handle_t interface_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to interface DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(interface_handle);

    /** Get core and tm_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, interface_handle.h0, interface_handle.h1, core));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_in_lag(
    int unit,
    bcm_port_t logical_port,
    int *port_in_lag)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    SHR_FUNC_INIT_VARS(unit);

    *port_in_lag = 0;

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to PP DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
    if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(pp_handle))
    {
        /** set port_in_lag if port is part of a lag */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, port_in_lag));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_lag_id_get(
    int unit,
    bcm_port_t logical_port,
    bcm_trunk_t * lag_id)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    int port_in_lag;
    SHR_FUNC_INIT_VARS(unit);

    /** set to invalid */
    *lag_id = BCM_TRUNK_INVALID;

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** check if port in lag */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, logical_port, &port_in_lag));

    /** set lag_id if port is part of a lag */
    if (port_in_lag)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.lag_id.get(unit, pp_handle.h0, pp_handle.h1, lag_id));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_header_type_in_set(
    int unit,
    bcm_port_t logical_port,
    int header_type)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.header_type_in.set(unit, tm_handle.h0, tm_handle.h1, header_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_header_type_in_get(
    int unit,
    bcm_port_t logical_port,
    int *header_type)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.header_type_in.get(unit, tm_handle.h0, tm_handle.h1, header_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_header_type_out_set(
    int unit,
    bcm_port_t logical_port,
    int header_type)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.header_type_out.set(unit, tm_handle.h0, tm_handle.h1, header_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_header_type_out_get(
    int unit,
    bcm_port_t logical_port,
    int *header_type)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.header_type_out.get(unit, tm_handle.h0, tm_handle.h1, header_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core,
    uint32 *pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    int is_lag;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to PP DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(pp_handle);

    /** Don't allow getting pp port for LAG member */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
    if (is_lag)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "getting pp port for a LAG member %d is not allowed.\n"
                     "Use LAG gport to get the required pp port\n", logical_port);
    }

    /** Get core and pp_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.core.get(unit, pp_handle.h0, pp_handle.h1, core));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.get(unit, pp_handle.h0, pp_handle.h1, pp_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_modify(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    uint32 pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    uint32 tm_port;
    bcm_core_t tm_core;
    int is_lag;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** Verify pp port */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));
    /** Verify that tm core equals to pp core */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &tm_core, &tm_port));
    if (tm_core != core)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d core must not changed dynamically\n", core);
    }

    /*
     * Set handle to PP DB
     */
    pp_handle.h0 = core;
    pp_handle.h1 = pp_port;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.set(unit, logical_port, &pp_handle));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
    if (!is_lag)
    {
        /** set master port (of pp port!) if not in lag */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.set(unit, pp_handle.h0, pp_handle.h1, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_link_get(
    int unit,
    bcm_port_t logical_port,
    int *fabric_link)
{
    int fabric_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to FABRIC DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

    /** Get fabric link */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.link.get(unit, fabric_handle, fabric_link));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_prbs_mode_set(
    int unit,
    bcm_port_t logical_port,
    portmod_prbs_mode_t prbs_mode)
{
    int fabric_handle = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to FABRIC DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));

    /*
     * For non Fabric ports, prbs mode is always portmodPrbsModePhy.
     * For Fabric ports, prbs mode can be either portmodPrbsModePhy or portmodPrbsModeMac.
     */
    if (fabric_handle == DNX_ALGO_PORT_HANDLE_INVALID)
    {
        /** Verify prbs mode */
        SHR_VAL_VERIFY(prbs_mode, portmodPrbsModePhy, _SHR_E_PARAM, "PRBS mode must be PHY for this port");
    }
    else
    {
        /** Verify prbs mode */
        SHR_RANGE_VERIFY(prbs_mode, portmodPrbsModePhy, portmodPrbsModeMac, _SHR_E_PARAM,
                         "prbs mode %d is not supported", prbs_mode);

        /** Set PRBS mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.prbs_mode.set(unit, fabric_handle, prbs_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_prbs_mode_get(
    int unit,
    bcm_port_t logical_port,
    portmod_prbs_mode_t * prbs_mode)
{
    int fabric_handle = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to FABRIC DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));

    /*
     * For non Fabric ports, prbs mode is always portmodPrbsModePhy.
     * For Fabric ports, prbs mode can be either portmodPrbsModePhy or portmodPrbsModeMac.
     */
    if (fabric_handle == DNX_ALGO_PORT_HANDLE_INVALID)
    {
        *prbs_mode = portmodPrbsModePhy;
    }
    else
    {
        /** Get PRBS mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.prbs_mode.get(unit, fabric_handle, (int *) prbs_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 tm_port,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_valid_verify(unit, core, tm_port));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.get(unit, core, tm_port, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 pp_port,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify pp port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.get(unit, core, pp_port, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 hr_id,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_hr_valid_verify(unit, core, hr_id));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.master_logical_port.get(unit, core, hr_id, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_is_valid_get(
    int unit,
    bcm_core_t core,
    uint32 hr_id,
    int *is_valid)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_hr_param_verify(unit, core, hr_id));

    /** Get is_valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.valid.get(unit, core, hr_id, is_valid));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_link_to_logical_get(
    int unit,
    int fabric_link,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify fabric link valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_valid_verify(unit, fabric_link));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.master_logical_port.get(unit, fabric_link, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_to_gport_get(
    int unit,
    bcm_core_t core,
    uint32 pp_port,
    bcm_gport_t * gport)
{

    int is_lag = 0;
    bcm_trunk_t lag_id;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify pp port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));

    /** Check if PP port was allocated for LAG */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, core, pp_port, &is_lag));
    if (is_lag)
    {
        /** Get LAG ID*/
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.lag_id.get(unit, core, pp_port, &lag_id));
        BCM_GPORT_TRUNK_SET(*gport, lag_id);
    }
    else
    {
        /** Get logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.get(unit, core, pp_port, &logical_port));
        *gport = logical_port;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Set / Get  APIs
 * {
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_master_get(
    int unit,
    bcm_port_t logical_port,
    int *is_master_port)
{
    bcm_port_t master_port;
    SHR_FUNC_INIT_VARS(unit);

    /** get master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));

    /** return true iff master port is logical port */
    *is_master_port = (logical_port == master_port);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * is interface can be configured as channelized
 */
static shr_error_e
dnx_algo_port_is_channelized_supported(
    int unit,
    bcm_port_t logical_port,
    int *is_channelized_supported)
{

    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get port type - different behavior per port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1))
    {
        *is_channelized_supported = FALSE;
    }
    else
    {
        *is_channelized_supported = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_master_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_port_t * master_logical_port)
{
    dnx_algo_port_db_2d_handle_t tm_interface_handle, interface_handle;
    uint32 supported_flags;
    bcm_port_t logical_port_i;
    int is_channelized_supported;
    bcm_port_t cur_master_logical_port;
    bcm_pbmp_t logical_port_channels;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Init default */
    *master_logical_port = DNX_ALGO_PORT_INVALID;

    /*
     * Verify
     */
    /** Verify that the required flags are supported */
    supported_flags = DNX_ALGO_PORT_MASTER_F_MASK;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "get master flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_supported(unit, logical_port, &is_channelized_supported));
    if (!is_channelized_supported)
    {

        if (flags & DNX_ALGO_PORT_MASTER_F_NEXT)
        { /** no next master*/
            *master_logical_port = DNX_ALGO_PORT_INVALID;
        }
        else
        { /** the logical port is the master port */
            *master_logical_port = logical_port;
        }
    }
    else
    {
        /** get current master port */
        /** Get handle to tm interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
        DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);

        /** iterate the port to find the requested port */
        if (flags & DNX_ALGO_PORT_MASTER_F_NEXT)
        {
            if (flags & DNX_ALGO_PORT_MASTER_F_TDM_BYPASS)
            {
                /*
                 * TDM-bypass master port
                 */
                /** get channels ports bitmap */
                SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_ONLY,
                                                           &logical_port_channels));
                /** get current master port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                master_tdm_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                            &cur_master_logical_port));
                BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                {
                    /** filter master port */
                    if (logical_port_i == cur_master_logical_port)
                    {
                        continue;
                    }
                    *master_logical_port = logical_port_i;
                    break;
                }
            }
            else if (flags & DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS)
            {
                /*
                 * NON TDM-bypass master port
                 */
                /** get channels ports bitmap */
                SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                                (unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_NON_TDM_BYPASS_ONLY,
                                 &logical_port_channels));
                /** get current master port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                master_non_tdm_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                                &cur_master_logical_port));
                BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                {
                    /** filter master port */
                    if (logical_port_i == cur_master_logical_port)
                    {
                        continue;
                    }
                    *master_logical_port = logical_port_i;
                    break;
                }
            }
            else if (flags & DNX_ALGO_PORT_MASTER_F_SCH)
            {
                /*
                 * SCH master port
                 */

                /** Get handle to tm interface */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &interface_handle));
                DNX_ALGO_PORT_2D_HANDLE_VERIFY(interface_handle);

                /** get channels ports bitmap -- it might include non SCH ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                                (unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY, &logical_port_channels));

                /** get current SCH master port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                                master_logical_port.get(unit, interface_handle.h0, interface_handle.h1,
                                                        &cur_master_logical_port));
                BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                {
                    /** filter master port */
                    if (logical_port_i == cur_master_logical_port)
                    {
                        continue;
                    }

                    /** skip non SCH ports */
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port_i, &port_info));
                    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
                    {
                        continue;
                    }

                    *master_logical_port = logical_port_i;
                    break;
                }
            }
            else
            {
                /*
                 * Regular master port
                 */
                /** get channels ports bitmap */
                SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                                (unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY, &logical_port_channels));
                /** get current master port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                master_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                        &cur_master_logical_port));
                BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                {
                    /** filter master port */
                    if (logical_port_i == cur_master_logical_port)
                    {
                        continue;
                    }
                    *master_logical_port = logical_port_i;
                    break;
                }
            }
        }
        else
        {   /** current master port */

            /** read master, tdm-bypass master, non-tdm-bypass master */
            if (flags & DNX_ALGO_PORT_MASTER_F_TDM_BYPASS)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                master_tdm_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                            master_logical_port));
            }
            else if (flags & DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                master_non_tdm_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                                master_logical_port));
            }
            else if (flags & DNX_ALGO_PORT_MASTER_F_SCH)
            {
                /*
                 * SCH master port
                 */

                /** Get handle to tm interface */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &interface_handle));
                DNX_ALGO_PORT_2D_HANDLE_VERIFY(interface_handle);

                /** get current SCH master port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                                master_logical_port.get(unit, interface_handle.h0, interface_handle.h1,
                                                        master_logical_port));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                master_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                        master_logical_port));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_if_id_to_master_get(
    int unit,
    dnx_algo_port_db_2d_handle_t interface_id,
    int *master_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                    master_logical_port.get(unit, interface_id.h0, interface_id.h1, master_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_to_ilkn_master_get(
    int unit,
    bcm_port_t logical_port,
    int *master_port)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.get(unit, ilkn_handle, master_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_inteface_to_master_get(
    int unit,
    int if_id_h0,
    int if_id_h1,
    int *master_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.master_logical_port.get(unit, if_id_h0, if_id_h1, master_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s * port_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port exist */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, port_info));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_offset_get(
    int unit,
    bcm_port_t logical_port,
    int *interface_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port exist */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.get(unit, logical_port, interface_offset));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_offset_logical_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int interface_offset,
    bcm_gport_t * gport)
{
    bcm_port_t valid_port;
    bcm_pbmp_t valid_ports;
    int interface_offset_i;
    dnx_algo_port_info_s port_info_i;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Iter over all valid ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0, &valid_ports));
    BCM_PBMP_ITER(valid_ports, valid_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, valid_port, &port_info_i));
        if (port_info_i.port_type != port_type)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, valid_port, &interface_offset_i));

        if (interface_offset_i != interface_offset)
        {
            continue;
        }

        BCM_GPORT_LOCAL_SET(*gport, valid_port);
        /** If port is found go directly to "exit" and skip NOT_FOUND error */
        SHR_EXIT();
    }

    /** If we reach this point it means that Port and COSQ were not found */
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Logical port for interface offset %d not found.", interface_offset);
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channel_set(
    int unit,
    bcm_port_t logical_port,
    int channel_id)
{
    int flexe_handle;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        /*
         * Store Bus A channel
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_client_handle.get(unit, logical_port, &flexe_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.client_index.set(unit, flexe_handle, channel_id));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        /*
         * Store BusB channel
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_mac_handle.get(unit, logical_port, &flexe_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.client_index.set(unit, flexe_handle, channel_id));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        /*
         * Store BusC channel
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_sar_handle.get(unit, logical_port, &flexe_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.client_index.set(unit, flexe_handle, channel_id));
    }

    {
        /*
         * Set data on logical DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.channel.set(unit, logical_port, channel_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channel_get(
    int unit,
    bcm_port_t logical_port,
    int *channel_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    *channel_id = 0;

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.channel.get(unit, logical_port, channel_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_app_flags_set(
    int unit,
    bcm_port_t logical_port,
    uint32 app_flags)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);
    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.port_app_flags.set(unit, tm_handle.h0, tm_handle.h1, app_flags));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_app_flags_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *app_flags)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    *app_flags = 0;

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.port_app_flags.get(unit, tm_handle.h0, tm_handle.h1, app_flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_base_q_pair_get(
    int unit,
    bcm_port_t logical_port,
    int *base_q_pair)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.base_q_pair.get(unit, tm_handle.h0, tm_handle.h1, base_q_pair));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_q_pair_port_cosq_get(
    int unit,
    int q_pair,
    bcm_core_t core,
    bcm_port_t * logical_port,
    bcm_cos_queue_t * cosq)
{
    bcm_port_t port;
    bcm_pbmp_t port_bm;
    int num_priorities, base_q_pair;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &port_bm));
    BCM_PBMP_ITER(port_bm, port)
    {
        /*
         * Get the Q-pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
        /*
         * Get number of priorities for current port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
        /*
         * If q_pair is in the range of Q-pairs for this port, return the information and break the loop
         */
        if ((q_pair >= base_q_pair) && (q_pair < (base_q_pair + num_priorities)))
        {
            *logical_port = port;
            *cosq = q_pair - base_q_pair;
            /** If Port and COSQ are found go directly to "exit" and skip NOT_FOUND error */
            SHR_EXIT();
        }
    }
    /** If we reach this point it means that Port and COSQ were not found */
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Port and COSQ for specified Q-pair not found.");
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_base_hr_get(
    int unit,
    bcm_port_t logical_port,
    int *base_hr)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.base_hr.get(unit, sch_handle.h0, sch_handle.h1, base_hr));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_get(
    int unit,
    bcm_port_t logical_port,
    int sch_priority,
    int *hr)
{
    int nof_priorities, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    /** verify valid scheduler priority is given */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (sch_priority > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given Schedule priority (%d) is bigger than number of scheduler priorities (%d)",
                     sch_priority, nof_priorities);
    }

    /** get base hr */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** the returned HR ID is computed by (base_hr + sch_priority) */
    *hr = base_hr + sch_priority;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_priorities_nof_set(
    int unit,
    bcm_port_t logical_port,
    int num_priorities)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.priorities.set(unit, tm_handle.h0, tm_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_priorities_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *num_priorities)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.priorities.get(unit, tm_handle.h0, tm_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_priorities_nof_set(
    int unit,
    bcm_port_t logical_port,
    int num_priorities)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_handle);

    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.set(unit, sch_handle.h0, sch_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_priorities_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *num_priorities)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.get(unit, sch_handle.h0, sch_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_channelized_get(
    int unit,
    bcm_port_t logical_port,
    int *is_channelized)
{
    dnx_algo_port_db_2d_handle_t tm_interface_handle;
    int is_channelized_supported;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_supported(unit, logical_port, &is_channelized_supported));
    if (is_channelized_supported)
    {
        /** Get handle to tm interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
        DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);

        /*
         * Get data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        is_channelized.get(unit, tm_interface_handle.h0, tm_interface_handle.h1, is_channelized));
    }
    else
    {
        *is_channelized = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_egress_interleave_get(
    int unit,
    bcm_port_t logical_port,
    int *is_egress_interleave)
{
    dnx_algo_port_db_2d_handle_t tm_interface_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_egress_interleave.get
                    (unit, tm_interface_handle.h0, tm_interface_handle.h1, is_egress_interleave));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_ingress_interleave_get(
    int unit,
    bcm_port_t logical_port,
    int *is_ingress_interleave)
{
    dnx_algo_port_db_2d_handle_t tm_interface_handle;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_ingress_interleave.get
                    (unit, tm_interface_handle.h0, tm_interface_handle.h1, is_ingress_interleave));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_priority_propagation_enable_get(
    int unit,
    bcm_port_t logical_port,
    int *sch_priority_propagation_enable)
{
    dnx_algo_port_db_2d_handle_t sch_interface_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_interface_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                    priority_propagation_enable.get(unit, sch_interface_handle.h0, sch_interface_handle.h1,
                                                    sch_priority_propagation_enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_speed_set(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int speed)
{
    dnx_algo_port_db_2d_handle_t interface_handle;
    dnx_algo_port_info_s port_info;
    int fabric_handle;
    dnx_algo_port_tdm_mode_e tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /** Get handle to fabric link */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /*
         * Set data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.speed.set(unit, fabric_handle, speed));
    }
    else
    {

        /** Get handle to tm interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
        DNX_ALGO_PORT_2D_HANDLE_VERIFY(interface_handle);

        /*
         * Return error if neither RX nor TX flag is not set.
         */
        if (!DNX_ALGO_PORT_SPEED_RX_GET(flags) && !DNX_ALGO_PORT_SPEED_TX_GET(flags))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Please specify the speed is for Rx or Tx for port %d.\n", logical_port);
        }
        /*
         * Set data for Rx and Tx separately. The Rx and Tx speed may be
         * asymmetric for FlexE MAC and SAR client
         */
        if (DNX_ALGO_PORT_SPEED_RX_GET(flags))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                            rx_speed.set(unit, interface_handle.h0, interface_handle.h1, speed));
        }
        if (DNX_ALGO_PORT_SPEED_TX_GET(flags))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                            tx_speed.set(unit, interface_handle.h0, interface_handle.h1, speed));
        }
    }

    /** update SOC_INFO with new port's speed */
    tdm_mode = DNX_ALGO_PORT_TDM_MODE_NONE;
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tdm_mode));
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set(unit, logical_port, port_info, tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * internal implementation of speed_get
 */
static shr_error_e
dnx_algo_port_speed_get_impl(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *speed)
{
    dnx_algo_port_db_2d_handle_t interface_handle;
    dnx_algo_port_info_s port_info;
    int rx_speed, tx_speed;
    int fabric_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /** Get handle to fabric link */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /*
         * Set data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.speed.get(unit, fabric_handle, speed));

    }
    else
    {

        /** Get handle to tm interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
        DNX_ALGO_PORT_2D_HANDLE_VERIFY(interface_handle);

        /*
         * Get data for Rx and Tx separately. If the Rx/Tx speed are asymmetric and both RX/TX flags are set,
         * return error directly.
         */
        if (DNX_ALGO_PORT_SPEED_RX_GET(flags) && !DNX_ALGO_PORT_SPEED_TX_GET(flags))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                            rx_speed.get(unit, interface_handle.h0, interface_handle.h1, speed));
        }
        else if (DNX_ALGO_PORT_SPEED_TX_GET(flags) && !DNX_ALGO_PORT_SPEED_RX_GET(flags))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                            tx_speed.get(unit, interface_handle.h0, interface_handle.h1, speed));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                            rx_speed.get(unit, interface_handle.h0, interface_handle.h1, &rx_speed));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                            tx_speed.get(unit, interface_handle.h0, interface_handle.h1, &tx_speed));
            if (rx_speed != tx_speed)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Rx speed and Tx speed are asymmetric for port %d, please specify the Rx or Tx flag.\n",
                             logical_port);
            }
            *speed = rx_speed;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_has_speed(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *has_speed)
{
    int speed;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get_impl(unit, logical_port, flags, &speed));

    *has_speed = (speed != 0);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_speed_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *speed)
{
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify that the required flags are supported */
    supported_flags =
        DNX_ALGO_PORT_SPEED_F_MBPS | DNX_ALGO_PORT_SPEED_F_KBPS | DNX_ALGO_PORT_SPEED_F_RX | DNX_ALGO_PORT_SPEED_F_TX;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "get speed flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);
    /*
     * Get speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get_impl(unit, logical_port, flags, speed));

    if (*speed == 0)
    {
        
        /** SHR_ERR_EXIT(_SHR_E_INTERNAL, "speed wasn't set for port %d.\n", logical_port); */
    }

    if (flags & DNX_ALGO_PORT_SPEED_F_KBPS)
    {
        *speed = *speed * 1000;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phys_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * phys)
{
    int nif_handle;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, phys));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_first_phy_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *first_phy_port)
{
    int nif_handle;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.first_phy.get(unit, nif_handle, first_phy_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_lanes_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * ilkn_lanes)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.lanes.get(unit, ilkn_handle, ilkn_lanes));

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_nof_segments_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_segments)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.nof_segments.get(unit, ilkn_handle, nof_segments));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_nof_segments_set(
    int unit,
    bcm_port_t logical_port,
    int nof_segments)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.nof_segments.set(unit, ilkn_handle, nof_segments));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_is_over_fabric_get(
    int unit,
    bcm_port_t logical_port,
    int *is_over_fabric)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_over_fabric.get(unit, ilkn_handle, is_over_fabric));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_is_over_fabric_set(
    int unit,
    bcm_port_t logical_port,
    int is_over_fabric)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_over_fabric.set(unit, ilkn_handle, is_over_fabric));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_priority_get(
    int unit,
    bcm_port_t logical_port,
    bcm_port_nif_scheduler_t * priority)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.priority.get(unit, ilkn_handle, priority));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_priority_set(
    int unit,
    bcm_port_t logical_port,
    bcm_port_nif_scheduler_t priority)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.priority.set(unit, ilkn_handle, priority));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_core_aggregated_pms_get(
    int unit,
    bcm_port_t logical_port,
    int is_over_fabric,
    int *nof_aggregated_pms,
    portmod_pm_identifier_t * controlled_pms)
{
    int i, max_nof_pms, phy, table_iter, tmp_pm_id = 0, next_free = 0, phy_offset = 0;
    int pm_arr[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
    uint32 pm_first_phy = 0;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;
    bcm_pbmp_t ilkn_phys;
    uint8 pm_found = FALSE, phy_found = FALSE;
    const dnxc_data_table_info_t *ilkn_pms_table_info;
    const dnx_data_nif_ilkn_nif_pms_t *ilkn_pm_info;
    const dnx_data_nif_ilkn_fabric_pms_t *ilkn_fabric_pm_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &ilkn_phys));
    ilkn_pms_table_info =
        (is_over_fabric) ? dnx_data_nif.ilkn.fabric_pms_info_get(unit) : dnx_data_nif.ilkn.nif_pms_info_get(unit);
    max_nof_pms = dnx_data_nif.ilkn.pms_nof_get(unit);
    *nof_aggregated_pms = 0;

    for (i = 0; i < DNX_DATA_MAX_NIF_ILKN_PMS_NOF; i++)
    {
        pm_arr[i] = -1;
    }

    BCM_PBMP_ITER(ilkn_phys, phy)
    {
        for (table_iter = 0; table_iter < ilkn_pms_table_info->key_size[0]; table_iter++)
        {
            if (is_over_fabric)
            {
                ilkn_fabric_pm_info = dnx_data_nif.ilkn.fabric_pms_get(unit, table_iter);

                phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);

                /*
                 * If device lane is in range, take range info
                 */
                if ((phy >= ilkn_fabric_pm_info->first_phy)
                    && (phy < ilkn_fabric_pm_info->first_phy + ilkn_fabric_pm_info->nof_phys))
                {
                    tmp_pm_id = table_iter;
                    pm_first_phy = ilkn_fabric_pm_info->first_phy;
                    pm_type = ilkn_fabric_pm_info->dispatch_type;
                    phy_found = TRUE;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                ilkn_pm_info = dnx_data_nif.ilkn.nif_pms_get(unit, table_iter);

                phy_offset = 0;

                /*
                 * If device lane is in range, take range info
                 */
                if ((phy >= ilkn_pm_info->first_phy) && (phy < ilkn_pm_info->first_phy + ilkn_pm_info->nof_phys))
                {
                    tmp_pm_id = table_iter;
                    pm_first_phy = ilkn_pm_info->first_phy;
                    pm_type = ilkn_pm_info->dispatch_type;
                    phy_found = TRUE;
                    break;
                }
                else
                {
                    continue;
                }
            }

        }

        pm_found = FALSE;
        for (i = 0; i < max_nof_pms; i++)
        {
            if (pm_arr[i] == tmp_pm_id)
            {
                pm_found = TRUE;
                break;
            }
        }

        if (!pm_found)
        {
            controlled_pms[*nof_aggregated_pms].phy = pm_first_phy + phy_offset;
            controlled_pms[*nof_aggregated_pms].type = pm_type;
            pm_arr[next_free++] = tmp_pm_id;
            (*nof_aggregated_pms)++;
        }

        if (!phy_found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ilkn phy id can't be found.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_latch_down_get(
    int unit,
    bcm_port_t logical_port,
    int *is_latch_down)
{
    int nif_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.latch_down.get(unit, nif_handle, is_latch_down));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_latch_down_set(
    int unit,
    bcm_port_t logical_port,
    int is_latch_down)
{
    int nif_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Set data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** set data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.latch_down.set(unit, nif_handle, is_latch_down));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_loopback_original_link_training_get(
    int unit,
    bcm_port_t logical_port,
    int *link_training_enable)
{
    int fabric_handle;
    int nif_handle;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /*
         * Get data
         */
        /** get fabric handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.
                        loopback_original_link_training.get(unit, fabric_handle, link_training_enable));
    }
    else
    {
        /*
         * Get data
         */
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                        loopback_original_link_training.get(unit, nif_handle, link_training_enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_loopback_original_link_training_set(
    int unit,
    bcm_port_t logical_port,
    int link_training_enable)
{
    int fabric_handle;
    int nif_handle;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /*
         * Set data
         */
        /** get fabric handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /** set data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.
                        loopback_original_link_training.set(unit, fabric_handle, link_training_enable));
    }
    else
    {
        /*
         * Set data
         */
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** set data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                        loopback_original_link_training.set(unit, nif_handle, link_training_enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_loopback_original_phy_lane_config_get(
    int unit,
    bcm_port_t logical_port,
    int *phy_lane_config)
{
    int fabric_handle;
    int nif_handle;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /*
         * Get data
         */
        /** get fabric handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.
                        loopback_original_phy_lane_config.get(unit, fabric_handle, phy_lane_config));
    }
    else
    {
        /*
         * Get data
         */
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.loopback_original_phy_lane_config.get(unit, nif_handle, phy_lane_config));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_loopback_original_phy_lane_config_set(
    int unit,
    bcm_port_t logical_port,
    int phy_lane_config)
{
    int fabric_handle;
    int nif_handle;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /*
         * Set data
         */
        /** get fabric handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /** set data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.
                        loopback_original_phy_lane_config.set(unit, fabric_handle, phy_lane_config));
    }
    else
    {
        /*
         * Set data
         */
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** set data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.loopback_original_phy_lane_config.set(unit, nif_handle, phy_lane_config));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phy_to_logical_get(
    int unit,
    int phy,
    int is_over_fabric,
    uint32 flags,
    bcm_port_t * logical_port)
{
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_phy_verify(unit, phy));
        phy += dnx_data_port.general.fabric_phys_offset_get(unit);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_verify(unit, phy));
    }

    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, phy, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_logical_fifo_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int logical_fifo_index,
    dnx_algo_port_rmc_info_t * logical_fifo_info)
{
    int nif_handle, flexe_handle;
    uint32 supported_flags;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        /** get flexe client handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_mac_handle.get(unit, logical_port, &flexe_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.
                        logical_fifos.get(unit, flexe_handle, logical_fifo_index, logical_fifo_info));
    }
    else
    {
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                        logical_fifos.get(unit, nif_handle, logical_fifo_index, logical_fifo_info));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_logical_fifo_set(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int logical_fifo_index,
    dnx_algo_port_rmc_info_t * logical_fifo_info)
{
    int nif_handle, flexe_handle;
    uint32 supported_flags;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        /** get flexe client handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_mac_handle.get(unit, logical_port, &flexe_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.
                        logical_fifos.set(unit, flexe_handle, logical_fifo_index, *logical_fifo_info));
    }
    else
    {
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                        logical_fifos.set(unit, nif_handle, logical_fifo_index, *logical_fifo_info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_rmc_info_t_init(
    int unit,
    dnx_algo_port_rmc_info_t * rmc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize rmc info to invalid values
     */
    sal_memset(rmc_info, 0, sizeof(dnx_algo_port_rmc_info_t));
    rmc_info->rmc_id = -1;
    rmc_info->sch_priority = bcmPortNifSchedulerLow;
    rmc_info->prd_priority = -1;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_logical_fifo_pbmp_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_fifo_pbmp)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*logical_fifo_pbmp);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    /*
     * iterate over all logical fifos of the port
     */
    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, logical_port, 0, ii, &rmc));
        if (rmc.rmc_id != DNX_ALGO_PORT_INVALID)
        {
            /*
             * add each logical fifo id to bitmap
             */
            _SHR_PBMP_PORT_ADD(*logical_fifo_pbmp, rmc.rmc_id);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_to_port_info_get(
    int unit,
    bcm_port_if_t interface_type,
    dnx_algo_port_info_indicator_t * indicator,
    dnx_algo_port_info_s * port_info)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(port_info, 0, sizeof(dnx_algo_port_info_s));

    switch (interface_type)
    {
        case BCM_PORT_IF_NULL:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_INVALID;
            break;
        }
        case BCM_PORT_IF_SFI:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_FABRIC;
            port_info->is_fabric = 1;
            port_info->is_imb = 1;
            break;
        }
        case BCM_PORT_IF_RCY:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_RCY;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_egr_tm = 1;
            port_info->is_sch = 1;
            port_info->is_ing_pp = 1;
            port_info->is_egr_pp = 1;
            break;
        }
        case BCM_PORT_IF_RCY_MIRROR:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_RCY_MIRROR;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_ing_pp = 1;
            break;
        }
        case BCM_PORT_IF_CPU:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_CPU;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_egr_tm = 1;
            port_info->is_sch = 1;
            port_info->is_ing_pp = 1;
            port_info->is_egr_pp = 1;
            break;
        }
        case BCM_PORT_IF_ERP:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_ERP;
            port_info->is_tm = 1;
            port_info->is_sch = 1;
            break;
        }
        case BCM_PORT_IF_SCH:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_SCH_ONLY;
            port_info->is_sch = 1;
            break;
        }
        case BCM_PORT_IF_OLP:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_OLP;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_egr_tm = 1;
            port_info->is_sch = 1;
            port_info->is_ing_pp = 1;
            port_info->is_egr_pp = 1;
            break;
        }
        case BCM_PORT_IF_OAMP:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_OAMP;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_egr_tm = 1;
            port_info->is_sch = 1;
            port_info->is_ing_pp = 1;
            port_info->is_egr_pp = 1;
            break;
        }
        case BCM_PORT_IF_EVENTOR:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_EVENTOR;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_egr_tm = 1;
            port_info->is_sch = 1;
            port_info->is_ing_pp = 1;
            port_info->is_egr_pp = 1;
            break;
        }
        case BCM_PORT_IF_SAT:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_SAT;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_egr_tm = 1;
            port_info->is_sch = 1;
            port_info->is_ing_pp = 1;
            port_info->is_egr_pp = 1;
            break;
        }
        case BCM_PORT_IF_ILKN:
        {
            if (indicator->is_cross_connect)
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_NIF_ILKN;
                port_info->is_nif = 1;
                port_info->is_nif_ilkn = 1;
                port_info->is_imb = 1;
                port_info->is_l1 = 1;
                port_info->is_tm = 1;
            }
            else if (indicator->is_elk)
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_NIF_ILKN;
                port_info->is_nif = 1;
                port_info->is_nif_ilkn = 1;
                port_info->is_elk = 1;
                port_info->is_imb = 1;
            }
            else
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_NIF_ILKN;
                port_info->is_nif = 1;
                port_info->is_nif_ilkn = 1;
                port_info->is_imb = 1;
                port_info->is_tm = 1;
                port_info->is_ing_tm = 1;
                port_info->is_egr_tm = 1;
                port_info->is_sch = 1;
                port_info->is_ing_pp = 1;
                port_info->is_egr_pp = 1;
            }
            break;
        }
        case BCM_PORT_IF_XFI:
        case BCM_PORT_IF_XLAUI:
        case BCM_PORT_IF_XLAUI2:
        case BCM_PORT_IF_CAUI:
        case BCM_PORT_IF_NIF_ETH:
        {

            if (indicator->is_flexe_phy)
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_FLEXE_PHY;
                port_info->is_nif = 1;
                port_info->is_nif_eth = 1;
                port_info->is_imb = 1;
                port_info->is_flexe_phy = 1;
            }
            else if (indicator->is_cross_connect)
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_NIF_ETH;
                port_info->is_nif = 1;
                port_info->is_nif_eth = 1;
                port_info->is_imb = 1;
                port_info->is_l1 = 1;
                port_info->is_tm = 1;
            }
            else if (indicator->is_stif)
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_NIF_ETH;
                port_info->is_nif = 1;
                port_info->is_nif_eth = 1;
                port_info->is_imb = 1;
                port_info->is_stif = 1;

                /** mark STIF port as data as well (only ingress path) */
                if (indicator->is_stif_data)
                {
                    port_info->is_tm = 1;
                    port_info->is_ing_pp = 1;
                    port_info->is_ing_tm = 1;
                }
            }
            else
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_NIF_ETH;
                port_info->is_nif = 1;
                port_info->is_nif_eth = 1;
                port_info->is_imb = 1;
                port_info->is_tm = 1;
                port_info->is_ing_tm = 1;
                port_info->is_egr_tm = 1;
                port_info->is_sch = 1;
                port_info->is_ing_pp = 1;
                port_info->is_egr_pp = 1;
            }

            break;
        }
        case BCM_PORT_IF_FLEXE_CLIENT:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_FLEXE_CLIENT;
            port_info->is_imb = 1;
            port_info->is_flexe_client = 1;
            break;
        }
        case BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT:
        {
            if (indicator->is_flexe_mac)
            {
                if (indicator->is_cross_connect)
                {
                    port_info->port_type = DNX_ALGO_PORT_TYPE_FLEXE_MAC;
                    port_info->is_flexe_client = 1;
                    port_info->is_imb = 1;
                    port_info->is_l1 = 1;
                }
                else
                {
                    port_info->port_type = DNX_ALGO_PORT_TYPE_FLEXE_MAC;
                    port_info->is_tm = 1;
                    port_info->is_ing_tm = 1;
                    port_info->is_egr_tm = 1;
                    port_info->is_sch = 1;
                    port_info->is_ing_pp = 1;
                    port_info->is_imb = 1;
                    port_info->is_egr_pp = 1;
                    port_info->is_flexe_client = 1;
                }
            }
            else if (indicator->is_flexe_sar)
            {
                port_info->port_type = DNX_ALGO_PORT_TYPE_FLEXE_SAR;
                port_info->is_imb = 1;
                port_info->is_flexe_client = 1;
            }
            break;
        }
        case BCM_PORT_IF_CRPS:
        {
            port_info->port_type = DNX_ALGO_PORT_TYPE_CRPS;
            port_info->is_tm = 1;
            port_info->is_ing_tm = 1;
            port_info->is_ing_pp = 1;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported interface type (%d).\n", interface_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_to_interface_type_get(
    int unit,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_info_indicator_t * indicator,
    bcm_port_if_t * interface_type)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_INVALID:
        {
            *interface_type = BCM_PORT_IF_NULL;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FABRIC:
        {
            *interface_type = BCM_PORT_IF_SFI;
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY:
        {
            *interface_type = BCM_PORT_IF_RCY;
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        {
            *interface_type = BCM_PORT_IF_RCY_MIRROR;
            break;
        }
        case DNX_ALGO_PORT_TYPE_CPU:
        {
            *interface_type = BCM_PORT_IF_CPU;
            break;
        }
        case DNX_ALGO_PORT_TYPE_ERP:
        {
            *interface_type = BCM_PORT_IF_ERP;
            break;
        }
        case DNX_ALGO_PORT_TYPE_OLP:
        {
            *interface_type = BCM_PORT_IF_OLP;
            break;
        }
        case DNX_ALGO_PORT_TYPE_OAMP:
        {
            *interface_type = BCM_PORT_IF_OAMP;
            break;
        }
        case DNX_ALGO_PORT_TYPE_EVENTOR:
        {
            *interface_type = BCM_PORT_IF_EVENTOR;
            break;
        }
        case DNX_ALGO_PORT_TYPE_SAT:
        {
            *interface_type = BCM_PORT_IF_SAT;
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        {
            *interface_type = BCM_PORT_IF_ILKN;
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        {
            *interface_type = BCM_PORT_IF_NIF_ETH;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FLEXE_PHY:
        {
            *interface_type = BCM_PORT_IF_NIF_ETH;
            indicator->is_flexe_phy = 1;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
        {
            *interface_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
            indicator->is_flexe_mac = 1;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FLEXE_SAR:
        {
            *interface_type = BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT;
            indicator->is_flexe_sar = 1;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FLEXE_CLIENT:
        {
            *interface_type = BCM_PORT_IF_FLEXE_CLIENT;
            break;
        }
        case DNX_ALGO_PORT_TYPE_SCH_ONLY:
        {
            *interface_type = BCM_PORT_IF_SCH;
            break;
        }
        case DNX_ALGO_PORT_TYPE_CRPS:
        {
            *interface_type = BCM_PORT_IF_CRPS;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port type (%d).\n", port_info.port_type);
            break;
        }
    }

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
    {
        indicator->is_elk = 1;
    }
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
    {
        indicator->is_stif = 1;

        /** some STIF ports support data on ingress path */
        if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
        {
            indicator->is_stif_data = 1;
        }
    }

    if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
    {
        indicator->is_cross_connect = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_legacy_interface_to_nof_lanes_get(
    int unit,
    bcm_port_if_t interface_type,
    int *nof_lanes)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (interface_type)
    {
        case BCM_PORT_IF_XFI:
        {
            *nof_lanes = 1;
            break;
        }
        case BCM_PORT_IF_XLAUI:
        case BCM_PORT_IF_CAUI:
        {
            *nof_lanes = 4;
            break;
        }
        case BCM_PORT_IF_XLAUI2:
        {
            *nof_lanes = 2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported interface type (%d).\n", interface_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_set(
    int unit,
    bcm_port_t logical_port,
    int if_id)
{
    dnx_algo_port_db_2d_handle_t tm_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Verify interface id */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_id_verify(unit, if_id));

    /** Get handle to TM IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_if_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_if_handle);

    /*
     * Set Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.egr_if.set(unit, tm_if_handle.h0, tm_if_handle.h1, if_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_unset(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_db_2d_handle_t tm_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_if_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_if_handle);

    /*
     * Set Data to default
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.egr_if.set(unit, tm_if_handle.h0, tm_if_handle.h1,
                                                             DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See algo_port_mgmt.h file
 */
shr_error_e
dnx_algo_port_egr_if_get(
    int unit,
    bcm_port_t logical_port,
    int *if_id)
{
    dnx_algo_port_db_2d_handle_t tm_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_if_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_if_handle);

    /*
     * Get Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.egr_if.get(unit, tm_if_handle.h0, tm_if_handle.h1, if_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_if_set(
    int unit,
    bcm_port_t logical_port,
    int if_id)
{
    dnx_algo_port_db_2d_handle_t sch_if_handle;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Verify interface id */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_id_verify(unit, if_id));

    /** Get handle to SCH IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_if_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_if_handle);

    /*
     * Set Data
     */
    /** set scheduler interface id */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.sch_if.set(unit, sch_if_handle.h0, sch_if_handle.h1, if_id));

exit:
    SHR_FUNC_EXIT;

}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_if_get(
    int unit,
    bcm_port_t logical_port,
    int *if_id)
{
    dnx_algo_port_db_2d_handle_t sch_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to SCH IF DB */ ;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_if_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_if_handle);

    /*
     * Get Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.sch_if.get(unit, sch_if_handle.h0, sch_if_handle.h1, if_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    dnx_algo_port_db_2d_handle_t tm_interface_handle;
    bcm_port_t master_tdm_logical_port, master_non_tdm_logical_port;
    bcm_port_t next_tdm_master_port, next_non_tdm_master_port;
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Verify TDM mode */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state != DNX_ALGO_PORT_STATE_REMOVED)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_mode_supported_verify(unit, tdm_mode));
    }

    /** Get handle to TM DB and TM interface DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);

    /*
     * Set Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tdm_mode.set(unit, tm_handle.h0, tm_handle.h1, tdm_mode));

    /*
     * Update tdm master port
     */
    /** get tdm master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    master_tdm_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                &master_tdm_logical_port));
    /** if first tdm port added - define as master tdm port */
    if (master_tdm_logical_port == DNX_ALGO_PORT_INVALID && tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_tdm_logical_port.set(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                    logical_port));
    }

    /** if master tdm port removed / changed - define as master tdm port */
    if (master_tdm_logical_port == logical_port && tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_TDM_BYPASS,
                         &next_tdm_master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_tdm_logical_port.set(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                    next_tdm_master_port));
    }

    /*
     * Update Non TDM master port
     */
     /** get non tdm master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    master_non_tdm_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                    &master_non_tdm_logical_port));
    /** if first non-tdm port added - define next one as master non-tdm port */
    if (master_non_tdm_logical_port == DNX_ALGO_PORT_INVALID && tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_non_tdm_logical_port.set(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                        logical_port));
    }

    /** if master non-tdm port removed / changed - define next one as master non-tdm port */
    if (master_non_tdm_logical_port == logical_port
        && (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS || tdm_mode == DNX_ALGO_PORT_TDM_MODE_INVALID))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS,
                         &next_non_tdm_master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_non_tdm_logical_port.set(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                        next_non_tdm_master_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tdm_mode_e * tdm_mode)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tdm_mode.get(unit, tm_handle.h0, tm_handle.h1, tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_if_tdm_mode_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_if_tdm_mode_e * if_tdm_mode)
{
    dnx_algo_port_db_2d_handle_t tm_interface_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    tdm_mode.get(unit, tm_interface_handle.h0, tm_interface_handle.h1, if_tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_client_id_get(
    int unit,
    bcm_port_t logical_port,
    int *client_id)
{
    int flexe_handle;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to flexe DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_client_handle.get(unit, logical_port, &flexe_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.client_id.get(unit, flexe_handle, client_id));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected port type\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_client_index_to_port_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int client_index,
    bcm_port_t * logical_port)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_valid_verify(unit, port_type, client_index));

    flexe_handle = client_index;
    if (port_type == DNX_ALGO_PORT_TYPE_FLEXE_CLIENT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.master_logical_port.get(unit, flexe_handle, logical_port));
    }
    else if (port_type == DNX_ALGO_PORT_TYPE_FLEXE_MAC)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.master_logical_port.get(unit, flexe_handle, logical_port));
    }
    else if (port_type == DNX_ALGO_PORT_TYPE_FLEXE_SAR)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.master_logical_port.get(unit, flexe_handle, logical_port));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected port type\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_client_is_bypass_get(
    int unit,
    bcm_port_t logical_port,
    int *is_bypass)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get flexe client handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_client_handle.get(unit, logical_port, &flexe_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.is_bypass.get(unit, flexe_handle, is_bypass));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_client_is_bypass_set(
    int unit,
    bcm_port_t logical_port,
    int is_bypass)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get flexe client handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_client_handle.get(unit, logical_port, &flexe_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.is_bypass.set(unit, flexe_handle, is_bypass));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_flexe_core_port_get(
    int unit,
    bcm_port_t logical_port,
    int *core_port)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get flexe phy handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_phy_handle.get(unit, logical_port, &flexe_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.core_port.get(unit, flexe_handle, core_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_core_port_set(
    int unit,
    bcm_port_t logical_port,
    int core_port)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get flexe phy handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_phy_handle.get(unit, logical_port, &flexe_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.core_port.set(unit, flexe_handle, core_port));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_flexe_logical_phy_id_get(
    int unit,
    bcm_port_t logical_port,
    int *logical_phy_id)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get flexe phy handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_phy_handle.get(unit, logical_port, &flexe_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.logical_phy_id.get(unit, flexe_handle, logical_phy_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_logical_phy_id_set(
    int unit,
    bcm_port_t logical_port,
    int logical_phy_id)
{
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get flexe phy handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.flexe_phy_handle.get(unit, logical_port, &flexe_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(flexe_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.phy.logical_phy_id.set(unit, flexe_handle, logical_phy_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
void
dnx_algo_port_db_skip_first_ud_header_size_print_cb(
    int unit,
    const void *data)
{
    dnx_algo_skip_first_header_info_t *first_header_size = (dnx_algo_skip_first_header_info_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "first header size",
                                   first_header_size->first_header_size_to_skip, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "first header size",
                                   first_header_size->port_type, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "first header size",
                                   first_header_size->is_before, NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "first header size",
                                   first_header_size->is_after, NULL, NULL);
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_counter_sets_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_types,
    dnx_algo_port_mib_counter_set_type_e counter_set_types[DNX_ALGO_PORT_COUNTER_SETS_MAX])
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    int ilkn_over_fabric_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    *nof_types = 0;

    /** mark port as fabric */
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        /** verify array size to avoid memory corruption */
        SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                       "Port=%d counter_set_types array size=%d is too small, required=%d",
                       logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

        counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FABRIC;
        (*nof_types)++;
    }

    /** mark port as NIF */
    if (DNX_ALGO_PORT_TYPE_IS_NIF
        (unit, port_info, TRUE /** ELK*/ , TRUE /** STIF */ , TRUE /** L1 */ , FALSE /** FLEXE*/ ))
    {
        /** verify array size to avoid memory corruption */
        SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                       "Port=%d counter_set_types array size=%d is too small, required=%d",
                       logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

        counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF;
        (*nof_types)++;
    }

    /** mark port as ILKN */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE /** ELK*/ , TRUE /** L1 */ ))
    {
        /** verify array size to avoid memory corruption */
        SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                       "Port=%d counter_set_types array size=%d is too small, required=%d",
                       logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

        counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN;
        (*nof_types)++;

        /** mark port as ILKN over fabric */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, logical_port, &ilkn_over_fabric_port));
        if (ilkn_over_fabric_port)
        {
            /** verify array size to avoid memory corruption */
            SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                           "Port=%d counter_set_types array size=%d is too small, required=%d",
                           logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

            counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_ILKN_OVER_FABRIC;
            (*nof_types)++;
        }
    }

    /** mark port as NIF_ETH */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE /** STIF */ , TRUE /** L1 */ ))
    {
        /** verify array size to avoid memory corruption */
        SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                       "Port=%d counter_set_types array size=%d is too small, required=%d",
                       logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

        counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH;
        (*nof_types)++;

        /** mark port PM type (CDU/CLU)) */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));
        if (ethu_info.imb_type == imbDispatchTypeImb_cdu)
        {
            /** verify array size to avoid memory corruption */
            SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                           "Port=%d counter_set_types array size=%d is too small, required=%d",
                           logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

            counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CDU;
            (*nof_types)++;
        }
        else if (ethu_info.imb_type == imbDispatchTypeImb_clu)
        {
            /** verify array size to avoid memory corruption */
            SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                           "Port=%d counter_set_types array size=%d is too small, required=%d",
                           logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

            counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_NIF_ETH_CLU;
            (*nof_types)++;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported pm_type=%d for port=%d", ethu_info.imb_type, logical_port);
        }
    }

    /** mark port as flexe mac */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 1))
    {
        /** verify array size to avoid memory corruption */
        SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                       "Port=%d counter_set_types array size=%d is too small, required=%d",
                       logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

        counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_FLEX_MAC;
        (*nof_types)++;
    }
    /** mark port as cpu */
    if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
    {
        /** verify array size to avoid memory corruption */
        SHR_MAX_VERIFY(*nof_types, DNX_ALGO_PORT_COUNTER_SETS_MAX, _SHR_E_INTERNAL,
                       "Port=%d counter_set_types array size=%d is too small, required=%d",
                       logical_port, DNX_ALGO_PORT_COUNTER_SETS_MAX, *nof_types);

        counter_set_types[*nof_types] = DNX_ALGO_PORT_MIB_COUNTER_SET_TYPE_CPU;
        (*nof_types)++;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_valid_get(
    int unit,
    bcm_port_t logical_port,
    int *is_valid)
{
    dnx_algo_port_state_e state;

    SHR_FUNC_INIT_VARS(unit);

    *is_valid = FALSE;

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state != DNX_ALGO_PORT_STATE_INVALID)
    {
        *is_valid = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
