/*! \file stg_stp.c
 * 
 *
 * Spanning Tree procedures for DNX.
 * Allows to set/get the STP state per STG index and Port
 *
 * Note: VLAN means  Virtual Switch Instance (VSI),
 *       rather than the concept in special 802.1Q.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STG

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stg.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm/types.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/stg/stg.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/stg/algo_stg.h>
#include <src/bcm/dnx/stg/stg_stp.h>
#include <src/bcm/dnx/stg/stg_vlan.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/**
 * \brief - Get stg general information from sw table.
 *    See stg_stp.h for more information
 */
shr_error_e
dnx_stg_general_info_get(
    int unit,
    bcm_stg_t * stg_min,
    bcm_stg_t * stg_max,
    bcm_stg_t * stg_defl,
    int *stg_count)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!stg_min && !stg_max && !stg_defl && !stg_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need at least one valid pointer\n");
    }

    /** Check the definitions. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STG_GENERAL_INFO_SW, &entry_handle_id));
    if (stg_min != NULL)
    {
        *stg_min = 0;
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STG_MIN_ID, INST_SINGLE, (uint32 *) stg_min);
    }
    if (stg_max != NULL)
    {
        *stg_max = 0;
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STG_MAX_ID, INST_SINGLE, (uint32 *) stg_max);
    }
    if (stg_defl != NULL)
    {
        *stg_defl = 0;
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STG_DEFAULT_ID, INST_SINGLE, (uint32 *) stg_defl);
    }
    if (stg_count != NULL)
    {
        *stg_count = 0;
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STG_COUNT, INST_SINGLE, (uint32 *) stg_count);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set stg general information into sw table.
 *
 * \param [in] unit - Unit ID
 * \param [out] stg_min - Pointer for the minimum STG ID in the system.
 * \param [out] stg_max - Pointer for the maximum STG ID in the system.
 * \param [out] stg_defl - Pointer for the default STG ID in the system.
 * \param [out] stg_count - Pointer for number of STG IDs allocated.
 *
 * \remark
 *  There must be at least one valid pointer in the inputs.
 *  A null pointer indicates no configuration for the relevant item.
 *
 * \see
 *   * dnx_stg_general_info_set.
 */
static shr_error_e
dnx_stg_general_info_set(
    int unit,
    bcm_stg_t * stg_min,
    bcm_stg_t * stg_max,
    bcm_stg_t * stg_defl,
    int *stg_count)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!stg_min && !stg_max && !stg_defl && !stg_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need at least one valid pointer\n");
    }

    /** Set STG general information to sw state.*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STG_GENERAL_INFO_SW, &entry_handle_id));
    if (stg_min != NULL)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STG_MIN_ID, INST_SINGLE, (uint32) *stg_min);
    }
    if (stg_max != NULL)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STG_MAX_ID, INST_SINGLE, (uint32) *stg_max);
    }
    if (stg_defl != NULL)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STG_DEFAULT_ID, INST_SINGLE, (uint32) *stg_defl);
    }
    if (stg_count != NULL)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STG_COUNT, INST_SINGLE, (uint32) *stg_count);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Check if the stg is in range and if it is allocated.
 *    See stg_stp.h for more information.
 */
shr_error_e
dnx_stg_id_verify(
    int unit,
    bcm_stg_t stg_id,
    int verify_alloc)
{
    uint8 is_allocated = 0;
    bcm_stg_t stg_min, stg_max;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if the STG ID is in range.*/
    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, &stg_min, &stg_max, NULL, NULL));
    if ((stg_id < stg_min) || (stg_id > stg_max))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "STG ID %d is out of range(%d ~ %d)!\n", stg_id, stg_min, stg_max);
    }

    /** Check if the STG ID is allocated.*/
    if (verify_alloc != 0)
    {
        SHR_IF_ERR_EXIT(algo_stg_db.stg_id.is_allocated(unit, stg_id, &is_allocated));
        if (is_allocated == 0)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "STG ID %d is not allocated!\n", stg_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if the port is applicable to spanning tree protocol.
 *    See stg.h for more information.
 */
shr_error_e
dnx_stg_port_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if the port is stp applicable.*/
    if (!BCM_GPORT_IS_SET(port) || !BCM_GPORT_IS_TRUNK(port))
    {
        /** Get Port + Core.*/
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        if (!dnx_drv_is_e_port(unit, gport_info.local_port) &&
            !dnx_drv_is_hg_port(unit, gport_info.local_port)
            && !dnx_drv_is_spi_subport_port(unit, gport_info.local_port))
        {
            SHR_ERR_EXIT(_SHR_E_PORT,
                         "port %d (local port: %d) is not applicable to spanning tree protocol, only eth/hg/spi ports are possible!\n",
                         port, gport_info.local_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify the inputs for API: bcm_dnx_stg_create/bcm_dnx_stg_create_id
 */
static shr_error_e
dnx_stg_create_verify(
    int unit,
    uint32 flags,
    bcm_stg_t * stg_ptr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stg_ptr, _SHR_E_PARAM, "stg_ptr");

    if (_SHR_IS_FLAG_SET(flags, ~DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags 0x%x\n", flags);
    }

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        SHR_IF_ERR_EXIT(dnx_stg_id_verify(unit, *stg_ptr, 0));
    }

    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the stp state for the port in the given STG.
 *
 * \param [in] unit - The unit ID
 * \param [in] stg  - The given stg_id
 * \param [in] port  - The given port
 * \param [in] stp_state  - STP state. See \ref bcm_stg_stp_t
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   dnx_algo_gpm_gport_phy_info_get.
 *   bcm_stg_stp_t
 *   dbal_enum_value_field_stp_state_e
 */
shr_error_e
dnx_stg_stp_port_set(
    int unit,
    bcm_stg_t stg,
    bcm_port_t port,
    int stp_state)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dbal_enum_value_field_stp_state_e ingress_stp_state;
    uint8 egress_stp_state;
    uint8 stp_topology_id;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Translate the state values from application to hardware*/
    switch (stp_state)
    {
        case BCM_STG_STP_BLOCK:
        case BCM_STG_STP_LISTEN:
            ingress_stp_state = DBAL_ENUM_FVAL_STP_STATE_BLOCK;
            break;
        case BCM_STG_STP_LEARN:
            ingress_stp_state = DBAL_ENUM_FVAL_STP_STATE_LEARN;
            break;
        case BCM_STG_STP_DISABLE:
        case BCM_STG_STP_FORWARD:
            ingress_stp_state = DBAL_ENUM_FVAL_STP_STATE_FORWARD;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "Invalid stp state(%d)", stp_state);
    }

    egress_stp_state = (ingress_stp_state == DBAL_ENUM_FVAL_STP_STATE_FORWARD) ?
        DBAL_ENUM_FVAL_STP_STATE_FORWARD : (!DBAL_ENUM_FVAL_STP_STATE_FORWARD);

    stp_topology_id = BCM_DNX_STG_TO_TOPOLOGY_ID_GET(stg);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Write to INGRESS_PORT_STP_table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PORT_STP, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_STP_TOPOLOGY_ID, stp_topology_id);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_STP_STATE, INST_SINGLE, ingress_stp_state);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** Write to EGRESS_PORT_STP_table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_PORT_STP, entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_STP_TOPOLOGY_ID, stp_topology_id);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_STP_STATE, INST_SINGLE, egress_stp_state);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /** Set the input STP_STATE into SW tables */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_STP_STATE_SW, entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_STP_TOPOLOGY_ID, stp_topology_id);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_BCM_STG_STP_STATE, INST_SINGLE, stp_state);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the stp state for the port in the given STG.
 *
 * \param [in] unit - The unit ID
 * \param [in] stg  - The given stg_id
 * \param [in] port  - The given port
 * \param [out] stp_state  - STP state retrieved. See \ref bcm_stg_stp_t
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_stg_stp_t.
 */
shr_error_e
dnx_stg_stp_port_get(
    int unit,
    bcm_stg_t stg,
    bcm_port_t port,
    int *stp_state)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint8 sw_stp_state;
    uint8 stp_topology_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    stp_topology_id = BCM_DNX_STG_TO_TOPOLOGY_ID_GET(stg);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** Get STP state from SW table. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STP_STATE_SW, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_STP_TOPOLOGY_ID, stp_topology_id);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_BCM_STG_STP_STATE, INST_SINGLE, &sw_stp_state);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *stp_state = (int) sw_stp_state;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a Spanning Tree Group.
 *
 * \param [in] unit - unit ID
 * \param [in] flags - flags used to create a stg_id
 * \param [out] stg_ptr - Pointer used to save the stg_id.
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   All Ethernet ports are Initialized with forward state in the created stg.
 *   The procedure support to create a stg with the flag of *_WITH_ID or not.
 *
 * \see
 *   None.
 */
static shr_error_e
dnx_stg_create(
    int unit,
    uint32 flags,
    bcm_stg_t * stg_ptr)
{
    uint32 algo_flags;
    bcm_port_t port;
    shr_error_e ret;

    SHR_FUNC_INIT_VARS(unit);

    algo_flags = 0;
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        algo_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /** Allocate the required stg-id.*/
    ret = algo_stg_db.stg_id.allocate_single(unit, algo_flags, NULL, stg_ptr);
    if ((ret == _SHR_E_RESOURCE) || (ret == _SHR_E_FULL))
    {
        ret = _SHR_E_FULL;
    }
    SHR_IF_ERR_EXIT(ret);

    {
        /** Set all ports to FORWARD state in the stg*/
        int port_min, port_max;
        pbmp_t *pbmp_p;
        dnx_algo_gpm_gport_phy_info_t gport_info;
        bcm_trunk_t trunk_id;
        uint32 trunk_flags;

        SHR_IF_ERR_EXIT(dnx_drv_soc_port_min(unit, ETHER_PTYPE, &port_min));
        SHR_IF_ERR_EXIT(dnx_drv_soc_port_max(unit, ETHER_PTYPE, &port_max));
        /*
         * Get pointer to bitmap corresponding to ETHERNET ports.
         */
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_E_ALL_BITMAP, &pbmp_p));
        for (port = port_min; port >= 0 && port <= port_max; port++)
        {
            if (_SHR_PBMP_MEMBER(*pbmp_p, port))
            {
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));

                /** Check if the port is a lag member */
                SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get
                                (unit, gport_info.sys_port, &trunk_id, &trunk_flags));
                if (trunk_id != -1)
                {
                    /** The local port is a lag member, skip it.*/
                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_stg_stp_port_set(unit, *stg_ptr, port, BCM_STG_STP_FORWARD));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify STG-ID, Port and STP-State for BCM-API: bcm_dnx_stg_stp_set/get(). \n
 */
static shr_error_e
dnx_stg_stp_set_verify(
    int unit,
    bcm_stg_t stg,
    bcm_port_t port,
    int stp_state)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * STG Validation
     */
    SHR_IF_ERR_EXIT(dnx_stg_id_verify(unit, stg, TRUE));

    /*
     * Port validations
     */
    SHR_IF_ERR_EXIT(dnx_stg_port_verify(unit, port));

    /*
     * STP State validations
     */
    if ((stp_state > BCM_STG_STP_FORWARD) || (stp_state < BCM_STG_STP_DISABLE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid stp_state(%d)!\n", stp_state);
    }

exit:
    SHR_FUNC_EXIT;
}

/*!
 * \brief
 * Verify the inputs for BCM-API: bcm_dnx_stg_stp_set/get(). \n
 */
static shr_error_e
dnx_stg_stp_get_verify(
    int unit,
    bcm_stg_t stg,
    bcm_port_t port,
    int *stp_state)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * STG Validation
     */
    SHR_IF_ERR_EXIT(dnx_stg_id_verify(unit, stg, TRUE));

    /*
     * Port validations
     */
    SHR_IF_ERR_EXIT(dnx_stg_port_verify(unit, port));

    /*
     * stp_state should not be NULL
     */
    SHR_NULL_CHECK(stp_state, _SHR_E_PARAM, "stp_state");

exit:
    SHR_FUNC_EXIT;
}

/**
 * see stg.h file for description
 */
shr_error_e
dnx_stg_init(
    int unit)
{
    int stg_count;
    uint8 is_allocated = 0;
    bcm_stg_t stg_max, stg_min, stg_defl;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Exit directly if in WB mode
     */
    if (sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(BCM_E_NONE);
    }

    /*
     * Initialize stg_min and stg_max first.
     */
    stg_min = BCM_STG_MIN;
    stg_max = dnx_data_stg.stg.nof_topology_ids_get(unit);

    /*
     * Get the default stg in the system to check if it is allocated already.
     */
    stg_defl = BCM_STG_INVALID;
    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, NULL, NULL, &stg_defl, NULL));
    if ((stg_defl < stg_min) || (stg_defl > stg_max))
    {
        stg_defl = BCM_STG_DEFAULT;
    }

    /** Check if the default STG was allocated already first. */
    SHR_IF_ERR_EXIT(algo_stg_db.stg_id.is_allocated(unit, stg_defl, &is_allocated));
    if (is_allocated != 0)
    {
        /** If default stg was created, means stg model was initialized already.*/
        SHR_IF_ERR_EXIT(BCM_E_NONE);
        SHR_EXIT();
    }

    /*
     * Allocate the default stg with flag of *_WITH_ID.
     */
    SHR_IF_ERR_EXIT(dnx_stg_create(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &stg_defl));

    /*
     * Update the STG general information.
     */
    stg_count = 1;
    SHR_IF_ERR_EXIT(dnx_stg_general_info_set(unit, &stg_min, &stg_max, &stg_defl, &stg_count));

exit:
    SHR_FUNC_EXIT;
}

/*
 * APIs
 * {
 */

/**
 * \brief
 *   Set a Spanning Tree Group as the default in the system.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The expected default STG.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   The STG to be set as the default must be created previously.
 *
 * \see
 *   bcm_dnx_stg_default_get
 *   bcm_dnx_stg_create
 *   bcm_dnx_stg_create_id
 */
bcm_error_t
bcm_dnx_stg_default_set(
    int unit,
    bcm_stg_t stg)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_id_verify(unit, stg, TRUE));

    SHR_IF_ERR_EXIT(dnx_stg_general_info_set(unit, NULL, NULL, &stg, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the default Spanning Tree Group.
 *
 * \param [in] unit - Relevant unit.
 * \param [out] stg_ptr - Pointer to receive default stg_id.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   bcm_dnx_stg_default_set.
 */
bcm_error_t
bcm_dnx_stg_default_get(
    int unit,
    bcm_stg_t * stg_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(stg_ptr, _SHR_E_PARAM, "stg_ptr");

    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, NULL, NULL, stg_ptr, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the total number of Spanning Tree Groups.
 *
 * \param [in] unit - Relevant unit.
 * \param [out] max_stg - Pointer to receive the total number of STGs.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   None.
 *
 * \see
 *   None.
 */
bcm_error_t
bcm_dnx_stg_count_get(
    int unit,
    int *max_stg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(max_stg, BCM_E_PARAM, "max_stg");
    *max_stg = dnx_data_stg.stg.nof_topology_ids_get(unit);

    SHR_IF_ERR_EXIT(BCM_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create a Spanning Tree Group.
 *
 * \param [in] unit - Relevant unit.
 * \param [out] stg_ptr - Pointer to receive the created stg_id.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   All ethernet ports will be initialized with forward state in the created stg.
 *
 * \see
 *   bcm_dnx_stg_create_id
 */
bcm_error_t
bcm_dnx_stg_create(
    int unit,
    bcm_stg_t * stg_ptr)
{
    uint32 algo_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    algo_flags = 0;
    SHR_IF_ERR_EXIT(dnx_stg_create_verify(unit, algo_flags, stg_ptr));
    SHR_IF_ERR_EXIT(dnx_stg_create(unit, algo_flags, stg_ptr));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create a Spanning Tree Group with the given STG ID.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The given Spanning Tree Group ID.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   All ethernet ports will be initialized with forward state in the stg.
 *
 * \see
 *   bcm_dnx_stg_create
 */
bcm_error_t
bcm_dnx_stg_create_id(
    int unit,
    bcm_stg_t stg)
{
    uint32 algo_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    algo_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    SHR_IF_ERR_EXIT(dnx_stg_create_verify(unit, algo_flags, &stg));
    SHR_IF_ERR_EXIT(dnx_stg_create(unit, algo_flags, &stg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Destroy a Spanning Tree Group. VSIs in the Spanning Tree Group
 *   are moved to the default Spanning Tree Group instead.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - Relevant stg.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   Trying to destroy the default STG will get failure.
 *
 * \see
 *   bcm_dnx_stg_vlan_remove_all
 */
bcm_error_t
bcm_dnx_stg_destroy(
    int unit,
    bcm_stg_t stg)
{
    bcm_stg_t stg_defl;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_id_verify(unit, stg, TRUE));

    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, NULL, NULL, &stg_defl, NULL));
    if (stg == stg_defl)
    {
        LOG_WARN(BSL_LS_BCM_STG, (BSL_META_U(unit, "unit(%d) default stg(%d) won't be destroyed!\n"), unit, stg));
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "Default stg(%d) won't be destroyed!\n", stg);
    }
    else
    {
        /** Step 1: remove all VSIs from the stg.*/
        SHR_IF_ERR_EXIT(bcm_dnx_stg_vlan_remove_all(unit, stg));

        /** Step 2: De-allocate the stg-id.*/
        SHR_IF_ERR_EXIT(algo_stg_db.stg_id.free_single(unit, stg));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Destroy all Spanning Tree Groups. Recover the stg model to the initial state.
 *
 * \param [in] unit - Relevant unit.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   Default stg won't be destroyed here.
 *
 * \see
 *   bcm_dnx_stg_destroy
 *   bcm_dnx_stg_init
 */
bcm_error_t
bcm_dnx_stg_clear(
    int unit)
{
    uint8 is_allocated;
    bcm_stg_t stg_min, stg_max, stg_defl, stg_id;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, &stg_min, &stg_max, &stg_defl, NULL));
    /*
     * Iterate all STGs. Destroy it if it is allocated except for the default STG.
     */
    for (stg_id = stg_min; stg_id <= stg_max; stg_id++)
    {
        if (stg_id == stg_defl)
        {
            continue;
        }
        is_allocated = 0;
        SHR_IF_ERR_EXIT(algo_stg_db.stg_id.is_allocated(unit, stg_id, &is_allocated));
        if (is_allocated)
        {
            /** Destroy this stg*/
            SHR_IF_ERR_EXIT(bcm_dnx_stg_destroy(unit, stg_id));
        }
    }

    /*
     * Recover the STG model to initial state.
     */
    SHR_IF_ERR_EXIT(dnx_stg_init(unit));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve all STGs that were in use.
 *
 * \param [in] unit - Relevant unit.
 * \param [out] list - The poniter to receive the STGs.
 * \param [out] count - The poniter to receive number of STGs in the list.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   Generally, bcm_dnx_stg_list_destroy should be called after this API if
 *    pointer of list is NULL.
 *
 * \see
 *   bcm_dnx_stg_list_destroy
 */
bcm_error_t
bcm_dnx_stg_list(
    int unit,
    bcm_stg_t ** list,
    int *count)
{
    int stg_count;
    int list_index = 0;
    uint8 is_allocated;
    bcm_error_t rv = BCM_E_NONE;

    bcm_stg_t stg_min, stg_max, stg_defl, stg_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(count, BCM_E_PARAM, "count");

    SHR_IF_ERR_EXIT(dnx_stg_general_info_get(unit, &stg_min, &stg_max, &stg_defl, &stg_count));
    SHR_IF_ERR_EXIT(algo_stg_db.stg_id.nof_free_elements_get(unit, &stg_count));

    /** Calculate the number of allocated STGs.*/
    stg_count = dnx_data_stg.stg.nof_topology_ids_get(unit) - stg_count;

    if (0 == stg_count)
    {
        *count = 0;
    }
    else
    {
        *count = stg_count;
        if (*list != NULL)
        {
            /** User should ensure the space in the list should be enough.*/
            LOG_WARN(BSL_LS_BCM_STG, (BSL_META_U(unit, "User should ensure enough space in the list\n")));
        }
        else
        {
            SHR_ALLOC(*list, stg_count * sizeof(bcm_stg_t), "bcm_dnx_stg_list", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        }

        if (NULL == *list)
        {
            rv = BCM_E_MEMORY;
        }
        else
        {
            for (stg_id = stg_min; stg_id <= stg_max; stg_id++)
            {
                rv = algo_stg_db.stg_id.is_allocated(unit, stg_id, &is_allocated);
                SHR_IF_ERR_EXIT(rv);
                if (is_allocated)
                {
                    assert(list_index < *count);
                    (*list)[list_index++] = stg_id;
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Free the memory used to recieve the STGs in bcm_stg_list.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] list - The poniter used to receive the STGs.
 * \param [in] count - The number of STGs in the list.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   See bcm_dnx_stg_list.
 *
 * \see
 *   bcm_dnx_stg_list
 */
bcm_error_t
bcm_dnx_stg_list_destroy(
    int unit,
    bcm_stg_t * list,
    int count)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (NULL != list)
    {
        SHR_FREE(list);
    }

    SHR_IF_ERR_EXIT(BCM_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the STP state for a port in the specified Spanning Tree Group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The specified Spanning Tree Group.
 * \param [in] port - The given port.
 * \param [in] stp_state - The Spanning Tree state to set.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t

 * \remark
 *   The BCM stp state should be saved, since it can't be recover from H/W.
 *
 * \see
 *   bcm_dnx_stg_stp_get
 */
bcm_error_t
bcm_dnx_stg_stp_set(
    int unit,
    bcm_stg_t stg,
    bcm_port_t port,
    int stp_state)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_stp_set_verify(unit, stg, port, stp_state));

    /** Set the STP state for the given port.*/
    SHR_IF_ERR_EXIT(dnx_stg_stp_port_set(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the STP state for a port in the specified Spanning Tree Group.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] stg - The specified Spanning Tree Group.
 * \param [in] port - The given port.
 * \param [out] stp_state - The Spanning Tree state to receive.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t

 * \remark
 *   The STP state is retrieved from sw state.
 *
 * \see
 *   bcm_dnx_stg_stp_set
 */
bcm_error_t
bcm_dnx_stg_stp_get(
    int unit,
    bcm_stg_t stg,
    bcm_port_t port,
    int *stp_state)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stg_stp_get_verify(unit, stg, port, stp_state));

    /** Get the STP state for the given port.*/
    SHR_IF_ERR_EXIT(dnx_stg_stp_port_get(unit, stg, port, stp_state));

exit:
    SHR_FUNC_EXIT;
}
