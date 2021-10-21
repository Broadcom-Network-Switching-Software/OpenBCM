/**
 *
 * \file dnx_port_nif_ofr.c
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * NIF OFR procedures for DNX.
 */

/*
 * Include files:
 * {
 */

#include "dnx_port_nif_ofr_internal.h"
#include "dnx_port_nif_link_list_internal.h"
#include "dnx_port_nif_calendar_internal.h"

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>

/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

shr_error_e
dnx_port_ofr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize OFR sw state.
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_info_init(unit));

    /*
     * Initialize ofr link list.
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_init
                    (unit, DNX_PORT_NIF_OFR_LINK_LIST, dnx_data_nif.ofr.nof_rx_memory_sections_get(unit),
                     dnx_data_nif.ofr.nof_total_rmcs_get(unit)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_add(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_port_ofr_port_remove(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Do nothing.
     */
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_mem_link_list_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int speed, nof_sections;
    int nif_interface_id, core;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get OFR context
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    if (enable)
    {
        /*
         * Get nof_sections from port's speed
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        nof_sections = speed / dnx_data_nif.ofr.rx_memory_link_list_speed_granularity_get(unit);

        /*
         * Decide per priority group which memory sections to use : TBD
         * The OFR RX buffer is carved into 640 sections of 512B .
         * We have a total of 640 units, in a link list based on memories. splitted into 4 groups.
         * Please notice that in case of a port working with 2 priorities  (High and Low, TDM and Non-TDM, Express and non express)
         * We have to set the RMC units at two spareted groups.
         * For example the TDM will use units at the range of 0-159 and the non-tdm will use units at the range of 160-319 and 480-639.
         */
        /*
         * Allocate 2 link lists - 1 per priority group : TBD
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_alloc(unit, DNX_PORT_NIF_OFR_LINK_LIST, nof_sections, nif_interface_id));

    }
    else
    {
        /*
         * Free link list
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_free(unit, DNX_PORT_NIF_OFR_LINK_LIST, nif_interface_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_enable(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - set TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - get TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD hard stage priority map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - map type:
 *  see bcm_cosq_ingress_port_drop_map_t.
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [in] priority - priority value to set to the key in
 *        the map.
 * \param [in] is_tdm - TDM signal
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - get priority value mapped to a key in one of the PRD
 *        priority tables
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - type of map. see
 *        bcm_cosq_ingress_port_drop_map_t
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [out] priority - returned priority value
 * \param [out] is_tdm - TDM signal
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;

}
