/** \file algo_port_verify.c
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

#include <bcm/types.h>

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include "bcm_int/dnx/tdm/tdm.h"
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include "algo_port_internal.h"
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_free_verify(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /** verify port range */
    if (logical_port < 0 || logical_port >= SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port is out of range %d. Max is %d.\n", logical_port, SOC_MAX_NUM_PORTS - 1);
    }

    /** Verify that port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state != DNX_ALGO_PORT_STATE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port expected to be free %d.\n", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_valid_verify(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /** verify port range */
    if (logical_port < 0 || logical_port >= SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port is out of range %d. Max is %d.\n", logical_port, SOC_MAX_NUM_PORTS - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state == DNX_ALGO_PORT_STATE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port expected to be valid %d.\n", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type)
{
    dnx_algo_port_info_s actual_port_info;
    char port_info_sxp[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    char port_info_got[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    SHR_FUNC_INIT_VARS(unit);

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &actual_port_info));

    /** compare port type */
    if (actual_port_info.port_type != port_type)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, port_type, port_info_sxp));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, actual_port_info.port_type, port_info_got));
        SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected port type Expect %s, got %s.\n", port_info_sxp, port_info_got);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_port_valid_verify(
    int unit,
    bcm_core_t core,
    bcm_port_t tm_port)
{
    int valid;
    int nof_cores;
    int nof_tm_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify tm port */
    nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
    if (tm_port < 0 || tm_port >= nof_tm_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port is out of range %d. Max is %d.\n", tm_port, nof_tm_ports - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port expected to be valid (core %d, tm port %d) .\n", core, tm_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_port_free_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    bcm_core_t core,
    bcm_port_t tm_port)
{
    int valid;
    int nof_cores;
    int nof_tm_ports;
    bcm_port_t master_logical_port;
    dnx_algo_port_info_s master_port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify tm port */
    nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
    if (tm_port < 0 || tm_port >= nof_tm_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port is out of range %d. Max is %d.\n", core, nof_tm_ports - 1);
    }

    /** verify tm port is not reserved tm port */
    /** reserved for ERP */
    if (!DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
    {
        if ((tm_port + 1) % dnx_data_port.general.nof_tm_ports_per_fap_id_get(unit) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "TM port reserved for ERP %d.\n", tm_port);
        }
    }
    else
    {
        if (tm_port != dnx_data_port.general.erp_tm_port_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "ERP TM port was set to %d, should be %d.\n", tm_port,
                         dnx_data_port.general.erp_tm_port_get(unit));
        }
    }
    /** reserved TM port (used for returned credits in LAG SCH)*/
    if (tm_port == dnx_data_port.general.reserved_tm_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port %d is reserved and cannot be used.\n", tm_port);
    }

    /** Verify that port is not valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.ref_count.get(unit, core, tm_port, &valid));

    if (valid)
    {
        if (!DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "TM port expected to be free (core %d, tm port %d) .\n", core, tm_port);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.get(unit, core, tm_port, &master_logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, master_logical_port, &master_port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, master_port_info))
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "TM port is in use by non RCY MIRROR port %d (core %d, tm port %d) .\n",
                             master_logical_port, core, tm_port);
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
dnx_algo_port_tm_port_allowed_verify(
    int unit,
    bcm_core_t core,
    bcm_port_t tm_port,
    int nof_fap_ids_per_core)
{
    SHR_FUNC_INIT_VARS(unit);

    if (nof_fap_ids_per_core > 0)
    {
        /*
         * check only if at least one fap ID is defined
         * in no fap ID is defined
         * the check will be executed again in first traffic_enable for all ports
         */
        if (tm_port >= dnx_data_port.general.nof_tm_ports_per_fap_id_get(unit) * nof_fap_ids_per_core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Tm port %d is not allowed with %d FAP IDs per core.\n", tm_port, nof_fap_ids_per_core);

        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_param_verify(
    int unit,
    bcm_core_t core,
    int hr_id)
{
    int nof_cores;
    int nof_hrs;

    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify hr id */
    nof_hrs = dnx_data_sch.flow.nof_hr_get(unit);
    if (hr_id < 0 || hr_id >= nof_hrs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "HR id is out of range %d. Max is %d.\n", hr_id, nof_hrs - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_valid_verify(
    int unit,
    bcm_core_t core,
    int hr_id)
{
    int valid;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_IF_ERR_EXIT(dnx_algo_port_hr_param_verify(unit, core, hr_id));

    /** Verify that hr is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.valid.get(unit, core, hr_id, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "SCH info expected to be valid (core %d, hr %d) .\n", core, hr_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_valid_verify(
    int unit,
    bcm_core_t core,
    bcm_port_t pp_port)
{
    int valid;
    int nof_cores;
    int nof_pp_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify pp port */
    nof_pp_ports = dnx_data_port.general.nof_pp_ports_get(unit);
    if (pp_port < 0 || pp_port >= nof_pp_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "PP port is out of range %d. Max is %d.\n", core, nof_pp_ports - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, core, pp_port, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "PP port expected to be valid (core %d, pp port %d) .\n", core, pp_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_link_valid_verify(
    int unit,
    int fabric_link)
{
    int valid;
    int nof_fabric_links;
    SHR_FUNC_INIT_VARS(unit);

    /** verify fabric link */
    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    if (fabric_link < 0 || fabric_link >= nof_fabric_links)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fabric link is out of range %d. Max is %d.\n", fabric_link, nof_fabric_links - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.valid.get(unit, fabric_link, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fabric link expected to be valid (%d) .\n", fabric_link);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_client_index_valid_verify(
    int unit,
    dnx_algo_port_type_e port_type,
    int client_index)
{
    int valid;
    int nof_clients;
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** verify client-id range */
    nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
    if (client_index < 0 || client_index >= nof_clients)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE client ID is out of range %d. Max is %d.\n", client_index, nof_clients - 1);
    }

    /** Verify that client is valid */
    flexe_handle = client_index;
    if (port_type == DNX_ALGO_PORT_TYPE_FLEXE_CLIENT)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.valid.get(unit, flexe_handle, &valid));
    }
    else if (port_type == DNX_ALGO_PORT_TYPE_FLEXE_MAC)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.valid.get(unit, flexe_handle, &valid));
    }
    else if (port_type == DNX_ALGO_PORT_TYPE_FLEXE_SAR)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.valid.get(unit, flexe_handle, &valid));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected port type\n");
    }

    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE client expected to be valid (%d) .\n", client_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_flexe_client_index_free_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    int client_index)
{
    int valid;
    int nof_clients;
    int flexe_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** verify client-id range */
    nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
    if (client_index < 0 || client_index >= nof_clients)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE client ID is out of range %d. Max is %d.\n", client_index, nof_clients - 1);
    }

    /** Verify that client is valid */
    flexe_handle = client_index;
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.client.valid.get(unit, flexe_handle, &valid));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.valid.get(unit, flexe_handle, &valid));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.sar.valid.get(unit, flexe_handle, &valid));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected port type\n");
    }

    if (valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE client expected to be free (%d) .\n", client_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_phy_verify(
    int unit,
    int fabric_phy)
{
    SHR_FUNC_INIT_VARS(unit);

    if (fabric_phy < 0 || fabric_phy >= dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "phy is out of bound - %d \n", fabric_phy);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_supported_verify(
    int unit,
    dnx_algo_port_type_e port_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that port type is supported.
     * When new device. more sophisticated verify will be requried (using dnx data).
     */
    if (port_type < 0 || port_type >= DNX_ALGO_PORT_TYPE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port type is not supported - %d.\n", port_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_over_fabric_phys_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t phys)
{
    int pm_id = 0;
    int link_id = 0;
    uint32 pm_bypassed_links[1] = { 0 };
    uint32 is_bypassed = 0;
    int nof_links_in_pm = 0;
    int nof_fabric_pms = 0;
    bcm_pbmp_t bypassed_phys_bmp;
    bcm_pbmp_t illegal_phys_bmp;
    bcm_pbmp_t ilkn_port_bmp;
    bcm_pbmp_t fabric_link_bmp;
    bcm_port_t port;
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_pm = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);

    /*
     *  Make sure all requested links are in bypass mode
     */
    BCM_PBMP_CLEAR(bypassed_phys_bmp);

    for (pm_id = 0; pm_id < nof_fabric_pms; pm_id++)
    {
        pm_bypassed_links[0] = dnx_data_fabric.ilkn.bypass_info_get(unit, pm_id)->links;

        for (link_id = 0; link_id < nof_links_in_pm; link_id++)
        {
            is_bypassed = SHR_BITGET(pm_bypassed_links, link_id) ? 1 : 0;
            if (is_bypassed)
            {
                BCM_PBMP_PORT_ADD(bypassed_phys_bmp, pm_id * nof_links_in_pm + link_id);
            }
        }
    }

    BCM_PBMP_ASSIGN(illegal_phys_bmp, phys);
    BCM_PBMP_REMOVE(illegal_phys_bmp, bypassed_phys_bmp);
    if (!BCM_PBMP_IS_NULL(illegal_phys_bmp))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ILKN port %d is assigned to phys which are not in bypass mode\n", logical_port);
    }

    /*
     * Make sure no ILKN ports using those phys
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &ilkn_port_bmp));

    BCM_PBMP_PORT_REMOVE(ilkn_port_bmp, logical_port);
    BCM_PBMP_ITER(ilkn_port_bmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
        if (is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &fabric_link_bmp));

            BCM_PBMP_AND(fabric_link_bmp, phys);
            if (!BCM_PBMP_IS_NULL(fabric_link_bmp))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d and port %d share phys.\n", logical_port, port);
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
dnx_algo_port_ilkn_phys_verify(
    int unit,
    int core,
    bcm_port_t port,
    uint32 ilkn_id,
    bcm_pbmp_t phys)
{
    int iter_phy, pm_index, facing_core_id, is_facing = 1, non_facing_id = -1, first_phy, ethu_id, ilkn_core;
    int ilkn_port_i, second_ilkn_port = -1;
    uint8 non_facing_found = FALSE;
    imb_dispatch_type_t pm_imb_type;
    bcm_pbmp_t all_ilkn_ports;
    bcm_pbmp_t second_port_phys;
    dnx_algo_port_ilkn_access_info_t port_i_ilkn_info;
    SHR_FUNC_INIT_VARS(unit);

    ilkn_core = ilkn_id / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
    _SHR_PBMP_FIRST(phys, first_phy);

    SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, first_phy, &ethu_id));

    pm_imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;

    _SHR_PBMP_ITER(phys, iter_phy)
    {
        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));
        if (dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type != pm_imb_type)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "unsupported phy combination for port %d: ilkn port can't be combined from more than 1 PM type.\n",
                         port);
        }
    }

    if ((pm_imb_type == imbDispatchTypeImb_clu)
        && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_selectors))
    {
        _SHR_PBMP_ITER(phys, iter_phy)
        {
            pm_index =
                dnx_data_nif.eth.phy_map_get(unit,
                                             (iter_phy / dnx_data_nif.eth.phy_map_granularity_get(unit)))->pm_index;

            facing_core_id = dnx_data_nif.ilkn.nif_pms_get(unit, pm_index)->facing_core_index;
            is_facing = (facing_core_id == ilkn_core) ? 1 : 0;
            /*
             * If non-facing PM found store the facing core id and check if it's the first non-facing
             */
            if (!is_facing)
            {
                if (!non_facing_found)
                {
                    non_facing_found = TRUE;
                    non_facing_id = facing_core_id;
                }
                /*
                 * If there is more than 1 non-facing ilkn core for the same ilkn port - return error
                 */
                if (non_facing_id != facing_core_id)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "unsupported phy combination: more than 1 PM which connected to non-facing ilkn core can't be configured for the same ilkn core. please refer to UM for more information ");

                }
            }   /* if (!is_facing) */
        }/*_SHR_PBMP_ITER(phys, iter_phy)*/

        /*
         * If there is lanes which are non-facing core need also to check if there is another port on the same ilkn
         * core, if yes, need to do same check
         */
        if (non_facing_found)
        {
            /*
             * Retrieve all ilkn ports which were already configured
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                             &all_ilkn_ports));
            /*
             * iterate over all Ilkn ports and check if MAC is is the same as given port
             */
            BCM_PBMP_ITER(all_ilkn_ports, ilkn_port_i)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, ilkn_port_i, &port_i_ilkn_info));
                if (port_i_ilkn_info.ilkn_core == ilkn_core)
                {
                    second_ilkn_port = ilkn_port_i;
                    break;
                }
            }

            /*
             * There is another ilkn port on the same ilkn core
             */
            if (second_ilkn_port != -1)
            {
                /*
                 * Extract ilkn phys for the other port
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, second_ilkn_port, 0, &second_port_phys));

                _SHR_PBMP_ITER(second_port_phys, iter_phy)
                {
                    pm_index =
                        dnx_data_nif.eth.phy_map_get(unit,
                                                     (iter_phy /
                                                      dnx_data_nif.eth.phy_map_granularity_get(unit)))->pm_index;

                    facing_core_id = dnx_data_nif.ilkn.nif_pms_get(unit, pm_index)->facing_core_index;
                    is_facing = (facing_core_id == ilkn_core) ? 1 : 0;
                    if (!is_facing)
                    {
                        if (non_facing_id != facing_core_id)
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG,
                                         "unsupported phy combination: more than 1 PM which connected to non-facing ilkn core can't be configured for the same ilkn core. please refer to UM for more information ");

                        }
                    }   /* if (!is_facing) */
                }/*_SHR_PBMP_ITER(second_port_phys, iter_phy)*/
            }   /* if (ilkn_nof_ports == 1 ) */
        }       /* if ( non_facing_found ) */
    }   /* if ((pm_imb_type == imbDispatchTypeImb_clu) && dnx_data_nif.ilkn.feature_get(unit,
         * dnx_data_nif_ilkn_clu_ilkn_selectors)) */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phys_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    bcm_pbmp_t phys)
{
    const dnx_data_nif_phys_general_t *phys_info;
    bcm_pbmp_t unsupported_phys, pm_phys;
    char phys_str[_SHR_PBMP_FMT_LEN];
    bcm_port_t active_nif_port;
    int phy_id, is_active = 0, i, pm_index;
    bcm_pbmp_t phys_to_check;
    dnx_algo_port_info_s active_port_info;
    dnx_algo_pm_info_t pm_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get supported phys per device and throw an error if phys include unsupported phy
     */
    /** get phys info from dnx data */
    phys_info = dnx_data_nif.phys.general_get(unit);

    /** calc bitmap of unsupported phys */
    BCM_PBMP_ASSIGN(unsupported_phys, phys);
    BCM_PBMP_REMOVE(unsupported_phys, phys_info->supported_phys);

    /** if this bitmap is not empty throw an error */
    if (!BCM_PBMP_IS_NULL(unsupported_phys))
    {
        _SHR_PBMP_FMT(unsupported_phys, phys_str);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported nif phys for port %d ( %s ).\n", logical_port, phys_str);
    }

    /*
     * Make sure no one else using those phys
     */
    BCM_PBMP_ITER(phys, phy_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
        if (is_active)
        {
            /*
             * ILKN over fabric port will not go into this API, hence setting
             * "is_over_fabric" flag to "0".
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, phy_id, 0, 0, &active_nif_port));
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d and port %d share phys.\n", logical_port, active_nif_port);
        }
    }

    /*
     * If the port is on PM 4x25.
     * Make sure the PM does not mix ETH and ILKN ports
     */
    _SHR_PBMP_CLEAR(phys_to_check);

    BCM_PBMP_ITER(phys, phy_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_phy_pm_info_get(unit, phy_id, &pm_info));
        if (pm_info.dispatch_type == portmodDispatchTypePm4x25)
        {
            _SHR_PBMP_OR(phys_to_check, pm_info.phys);
        }
    }

    /** remove phys of the new port - they are already checked */
    BCM_PBMP_REMOVE(phys_to_check, phys);

    BCM_PBMP_ITER(phys_to_check, phy_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
        if (is_active)
        {
            /*
             * ILKN over fabric port will not go into this API, hence setting
             * "is_over_fabric" flag to "FALSE".
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, phy_id, FALSE, 0, &active_nif_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, active_nif_port, &active_port_info));
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE, TRUE) !=
                DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, active_port_info, TRUE, TRUE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port %d and port %d are on the same PM 4x25 and are not of the same type\n",
                             logical_port, active_nif_port);
            }
        }
    }

    /*
     * Verify if using correct PHYs for flexe physical port
     */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        for (i = 0; i < dnx_data_nif.flexe.nof_pms_per_feu_get(unit); ++i)
        {
            /*
             * Get supported PM
             */
            pm_index = dnx_data_nif.flexe.supported_pms_get(unit, 0)->nif_pms[i];
            /*
             * Get PM PHYs
             */
            pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
            /*
             * Verify the PHY assignment
             */
            BCM_PBMP_ASSIGN(unsupported_phys, phys);
            BCM_PBMP_REMOVE(unsupported_phys, pm_phys);
            if (BCM_PBMP_IS_NULL(unsupported_phys))
            {
                break;
            }
        }
        if (i == dnx_data_nif.flexe.nof_pms_per_feu_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE phy assignment is incorrect.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phy_verify(
    int unit,
    int nif_phy)
{
    bcm_pbmp_t supported_phys;

    SHR_FUNC_INIT_VARS(unit);

    if (nif_phy < 0 || nif_phy >= dnx_data_nif.phys.nof_phys_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "phy is out of bound - %d \n", nif_phy);
    }
    /*
     * Verify if phy is supported on SKU
     */
    supported_phys = dnx_data_nif.phys.general_get(unit)->supported_phys;
    if (!BCM_PBMP_MEMBER(supported_phys, nif_phy))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "phy is not supported on this SKU - %d \n", nif_phy);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_db_2d_handle_t interface_id,
    dnx_algo_port_tm_add_t * info)
{
    int sch_if_valid;
    bcm_port_t master_port;
    int if_priority_propagation_enable;
    int master_sch_priorities;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                        valid.get(unit, interface_id.h0, interface_id.h1, &sch_if_valid));
        if (sch_if_valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                            priority_propagation_enable.get(unit, interface_id.h0, interface_id.h1,
                                                            &if_priority_propagation_enable));

            if (info->sch_priority_propagation_enable != if_priority_propagation_enable)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port (%d) priority propagation enable should be equal on all ports of the interface\n",
                             logical_port);

            }
            if (info->sch_priority_propagation_enable)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                                master_logical_port.get(unit, interface_id.h0, interface_id.h1, &master_port));
                SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, master_port, &master_sch_priorities));
                if ((info->sch_priorities != 0 && info->sch_priorities != master_sch_priorities) ||
                    (info->sch_priorities == 0 && info->num_priorities != master_sch_priorities))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "port (%d) - in priority propagation mode, num of sch priorities should be equal on all ports of the interface\n",
                                 logical_port);
                }
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
dnx_algo_port_nif_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_if_add_t * info)
{
    int first_phy, first_phy_wo_fabric_offset;
    int nif_handle;
    int valid;
    bcm_port_t master_port;
    dnx_algo_port_info_s master_port_info;
    int master_interface_offset;
    bcm_pbmp_t master_phys;
    int phy;
    uint32 is_over_fabric = 0;
    dnx_algo_port_db_2d_handle_t interface_id;
    dnx_algo_lane_map_type_e lane_map_type;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        is_over_fabric = info->ilkn_info.is_ilkn_over_fabric;
    }

    /*
     * Check if master port or not
     */

    _SHR_PBMP_FIRST(info->phys, first_phy);
    first_phy_wo_fabric_offset = first_phy;
    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_phy_verify(unit, first_phy));

        /** Add fabric links offset for NIF DB handle */
        first_phy += dnx_data_port.general.fabric_phys_offset_get(unit);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_verify(unit, first_phy));
    }

    nif_handle = first_phy;

    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, nif_handle, &valid));

    if (valid)
    {
        /** NIF ETH L1 or FlexE PHY port shouldn't be channelized */
        if (DNX_ALGO_PORT_TYPE_IS_ETH_L1(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port (%d) is not allowed to be channelized.\n", logical_port);
        }

        /*
         * i.e. channelized port
         */
        /** make sure that channel is defined */
        if (info->tm_info.channel == -1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port (%d) is channelized. expecting channel != -1 .\n", logical_port);
        }

        /*
         *  Make sure that indeed the already defined port is identical
         */

        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, nif_handle, &master_port));

        /** port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, master_port, &master_port_info));
        if ((port_info.port_type != master_port_info.port_type)
            && !DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, master_port_info, FALSE, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "port (%d) is channelized. expecting identical port type to master port (%d).\n",
                         logical_port, master_port);
        }
        /** interface offset */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.get(unit, master_port, &master_interface_offset));
        if (info->interface_offset != master_interface_offset)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "port (%d) is channelized. expecting identical interface offset to master port (%d).\n",
                         logical_port, master_port);
        }
        /** phys */

        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, &master_phys));

        if (!BCM_PBMP_EQ(info->phys, master_phys))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port (%d) is channelized. expecting identical phys to master port (%d).\n",
                         logical_port, master_port);
        }
    }
    else
    { /** i.e. master port */

        /*
         * Make sure that no one else is using those phys
         */
        if (!is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_verify(unit, logical_port, port_info, info->phys));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_over_fabric_phys_verify(unit, logical_port, info->phys));
        }

        /** Lane Map verify, make sure all the lanes have been mapped and Lane map boundaries are correct*/

        lane_map_type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;

        if (!dnx_data_dev_init.general.access_only_get(unit) && !dnx_data_dev_init.general.heat_up_get(unit))
        {
            BCM_PBMP_ITER(info->phys, phy)
            {
                SHR_IF_ERR_EXIT(dnx_algo_lane_map_port_add_verify(unit, lane_map_type, port_info, is_over_fabric, phy));
            }
        }
        /*
         * Make sure interface and interface offset are valid, and match number of phys
         */

        
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_stif_verify
                    (unit, logical_port, port_info, first_phy_wo_fabric_offset, is_over_fabric));

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get(unit, port_info, info->core, info->interface_offset,
                                                          &info->phys, -1, &interface_id.h0, &interface_id.h1));
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_add_verify(unit, logical_port, port_info, interface_id, &info->tm_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_mode_supported_verify(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that tdm mode is supported.
     * When new device. more sophisticated verify will be requried (using dnx data).
     */
    if (tdm_mode < 0 || tdm_mode >= DNX_ALGO_PORT_TDM_MODE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM mode is not supported - %d.\n", tdm_mode);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_id_verify(
    int unit,
    int if_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that egress interface id is valid.
     * Assuming there are no overlaps (skip this check)
     */
    if (if_id < 0 || if_id >= dnx_data_port.egress.nof_ifs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EGR interface id is not supported - %d.\n", if_id);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_if_id_verify(
    int unit,
    int if_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that sch interface id is valid.
     * Assuming there are no overlaps (skip this check)
     */
    if (if_id < 0 || if_id >= dnx_data_sch.interface.nof_sch_interfaces_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SCH interface id is not supported - %d.\n", if_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channel_free_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_db_2d_handle_t tm_interface_handle,
    int channel)
{
    int tm_interface_valid;
    bcm_port_t master_logical_port, logical_port_channel;
    int channel_id;
    bcm_pbmp_t logical_port_channels;
    SHR_FUNC_INIT_VARS(unit);

    /** verify range */
    SHR_RANGE_VERIFY(channel, 0, dnx_data_port.general.max_nof_channels_get(unit) - 1,
                     _SHR_E_PARAM, "channel ID out of range for port %d.\n", logical_port);

    /** verify handle */
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_interface_handle);
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    valid.get(unit, tm_interface_handle.h0, tm_interface_handle.h1, &tm_interface_valid));
    if (tm_interface_valid)
    {
        /** get master port of the same interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_logical_port.get(unit, tm_interface_handle.h0, tm_interface_handle.h1,
                                                &master_logical_port));

        /** get all channels */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, master_logical_port, 0, &logical_port_channels));
        /** remove new port */
        BCM_PBMP_PORT_REMOVE(logical_port_channels, logical_port);

        /** iterate over ports and make sure channels is not used */
        BCM_PBMP_ITER(logical_port_channels, logical_port_channel)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port_channel, &channel_id));
            if (channel_id == channel)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Adding port %d channel %d is already used by logical port %d.\n",
                             logical_port, channel, logical_port_channel);
            }
        }
    }
    else
    {
        /** do nothing the interface is not used */
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_special_interface_offset_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    int interface_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
    {
        SHR_RANGE_VERIFY(interface_offset, -1, dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit) - 1,
                         _SHR_E_PARAM, "interface offset %d is not expected.\n", interface_offset);
    }
    else
    {
        SHR_RANGE_VERIFY(interface_offset, -1, -1,
                         _SHR_E_PARAM, "interface offset %d is not expected. (should be set to -1)\n",
                         interface_offset);

    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_stif_verify(
    int unit,
    bcm_port_t port,
    dnx_algo_port_info_s port_info,
    int first_phy,
    int is_over_fabric)
{
    int lane_idx;
    int is_stif;
    int first_lane_in_ethu, nof_lanes_in_ethu;
    bcm_port_t temp_logical_port;
    dnx_algo_port_info_s temp_port_info;
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_mixing_ports_limitation) == TRUE)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE /* elk */ , TRUE /* stif */ , TRUE /* L1 */ , TRUE  /* FLEXE 
                                                                                                                 */ ))
        {
            is_stif = DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info);
            SHR_IF_ERR_EXIT(dnx_algo_phy_ethu_lane_info_get(unit, first_phy, &first_lane_in_ethu, &nof_lanes_in_ethu));

            for (lane_idx = 0; lane_idx < nof_lanes_in_ethu; lane_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get
                                (unit, (first_lane_in_ethu + lane_idx), is_over_fabric, 0, &temp_logical_port));
                if (temp_logical_port != DNX_ALGO_PORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, temp_logical_port, &temp_port_info));
                    if (is_stif != DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, temp_port_info))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Mixing STIF and none STIF ports in the same eth unit (CDU/CLU) is not allowed. ports: {%d,%d} \n",
                                     port, temp_logical_port);
                    }
                }
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
dnx_algo_port_tdm_usage_verify(
    int unit,
    int tdm_is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify is_tdm */
    if (!dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) && tdm_is_set)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM is not supported by device\n");
    }

    if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_NONE && tdm_is_set)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM parameter shouldn't be used when TDM is disabled!\n");
    }
exit:
    SHR_FUNC_EXIT;
}
