/** \file mirror_rcy.c
 * 
 *
 * Recycle MIRROR functionality for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR

#include <shared/shrextend/shrextend_error.h>
#include <bcm/mirror.h>

#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_mirror_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * MACROs
 * {
 */
#define DNX_MIRROR_RCY_CHANNEL_INVALID (dnx_data_port.general.max_nof_channels_get(unit) - 1)
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_mirror_port_mapped_egress_interface_get(
    int unit,
    bcm_port_t rcy_mirror_port,
    int *mapped_egress_interface)
{
    int core_id;
    int mirror_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, rcy_mirror_port, &mirror_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, rcy_mirror_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_mirror_db.channel_to_egress_if.get(unit, core_id, mirror_channel, mapped_egress_interface));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Store mirror port -> egress interface mapping
 */
static shr_error_e
dnx_mirror_port_mapped_egress_interface_set(
    int unit,
    bcm_port_t rcy_mirror_port,
    int mapped_egress_interface)
{
    int core_id;
    int mirror_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, rcy_mirror_port, &mirror_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, rcy_mirror_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_mirror_db.channel_to_egress_if.set(unit, core_id, mirror_channel, mapped_egress_interface));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get mirror port mapped to the provided forward port
 */
static shr_error_e
dnx_mirror_port_egr_port_to_mirror_get(
    int unit,
    bcm_port_t forward_port,
    bcm_port_t * rcy_mirror_port)
{
    uint32 rcy_channel_id;
    uint32 entry_handle_id;
    int base_q_pair;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, forward_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, forward_port, &core_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_EGRESS_PORT_TO_RCY_CHANNEL_MAP_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CHANNEL_ID, INST_SINGLE, &rcy_channel_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (rcy_channel_id == DNX_MIRROR_RCY_CHANNEL_INVALID)
    {
        *rcy_mirror_port = BCM_PORT_INVALID;
    }
    else
    {
        dnx_algo_port_info_s port_info_rcy_mirror;
        dnx_algo_port_info_indicator_t indicator;
        sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get
                        (unit, BCM_PORT_IF_RCY_MIRROR, &indicator, &port_info_rcy_mirror));
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_and_channel_to_logical_get
                        (unit, core_id, port_info_rcy_mirror, rcy_channel_id, rcy_mirror_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map forward port to mirror port 
 */
static shr_error_e
dnx_mirror_port_egr_port_to_mirror_map(
    int unit,
    bcm_port_t forward_port,
    bcm_port_t rcy_mirror_port)
{
    int cosq, rcy_channel_id;
    uint32 entry_handle_id;
    int base_q_pair, num_priorities, core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (rcy_mirror_port == BCM_PORT_INVALID)
    {
        rcy_channel_id = DNX_MIRROR_RCY_CHANNEL_INVALID;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, rcy_mirror_port, &rcy_channel_id));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, forward_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, forward_port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, forward_port, &core_id));

    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_SNIF_EGRESS_PORT_TO_RCY_CHANNEL_MAP_TABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair + cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL_ID, INST_SINGLE, rcy_channel_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify mirror_port_to_rcy_port map_set parameters
 */
static shr_error_e
dnx_mirror_port_to_rcy_port_map_common_verify(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info)
{
    dnx_algo_port_info_s forward_port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify forward port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, forward_port));

    /** make sure the forward port is Egress TM port */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, forward_port, &forward_port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, forward_port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d should be egress TM port", forward_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify mirror_port_to_rcy_port map_set parameters
 */
static shr_error_e
dnx_mirror_port_to_rcy_port_map_set_verify(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info)
{
    dnx_algo_port_info_s mirror_port_info;
    int forward_port_core_id, mirror_port_core_id;
    int mapped_interface_id, forward_interface_id;
    bcm_port_t tmp_mirror_port, curr_egr_port;
    uint8 is_fixed_priority, is_mapped;
    bcm_pbmp_t ports_pbmp;
    bcm_port_t rcy_mirror_port;

    SHR_FUNC_INIT_VARS(unit);

    rcy_mirror_port = rcy_map_info->rcy_mirror_port;

    /** verify forward port */
    SHR_IF_ERR_EXIT(dnx_mirror_port_to_rcy_port_map_common_verify(unit, forward_port, rcy_map_info));

    /** verify mirror port */

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, rcy_mirror_port));

    /** make sure the second port is RCY Mirror */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, rcy_mirror_port, &mirror_port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, mirror_port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d should be RCY_MIRROR port", rcy_mirror_port);
    }

    /** make sure both cores are on the same core */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, forward_port, &forward_port_core_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, rcy_mirror_port, &mirror_port_core_id));
    if (forward_port_core_id != mirror_port_core_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Forward %d and mirror %d ports should be on the same core",
                     forward_port, rcy_mirror_port);
    }

    SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_get(unit, rcy_mirror_port, &mapped_interface_id));

    /** make sure forwrd port is not mapped to a different egress interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, forward_port, &forward_interface_id));
    if (mapped_interface_id != DNX_ALGO_PORT_EGR_IF_INVALID && mapped_interface_id != forward_interface_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mirror port %d already mapped to a different egress interface", rcy_mirror_port);
    }

    /** check if forward port is already mapped */
    SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_get(unit, forward_port, &tmp_mirror_port));
    if (tmp_mirror_port != DNX_ALGO_PORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "forward port %d already mapped", forward_port);
    }

    /*
     * check if there is a limitaion on changing priority 
     */
    SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_get(unit, rcy_mirror_port, &is_fixed_priority));
    if (is_fixed_priority)
    {
        /** check if use existing priorities only */

        /** get all ports of the same interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, forward_port, 0, &ports_pbmp));

        if (rcy_map_info->priority_bitmap & BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH)
        {
            /** High priority required */

            is_mapped = 0;
            BCM_PBMP_ITER(ports_pbmp, curr_egr_port)
            {
                SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_member(unit,
                                                                                 DNX_MIRROR_RCY_PRIORITY_LOSSLESS,
                                                                                 curr_egr_port, &is_mapped));
            }
            if (!is_mapped)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Priority config can't be changed for port %d. Priority can be change only before creating src VSQ and/or setting port compensation on mirror port\n",
                             rcy_mirror_port);

            }
        }

        if (rcy_map_info->priority_bitmap & BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL)
        {
            /** Normal priority required */

            is_mapped = 0;
            BCM_PBMP_ITER(ports_pbmp, curr_egr_port)
            {
                SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_member(unit,
                                                                                 DNX_MIRROR_RCY_PRIORITY_NORMAL,
                                                                                 curr_egr_port, &is_mapped));
            }

            if (!is_mapped)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Priority config can't be changed for port %d. Priority can be change only before creating src VSQ and/or setting port compensation on mirror port\n",
                             rcy_mirror_port);

            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify mirror_port_to_rcy_port unmap_set parameters
 */
static shr_error_e
dnx_mirror_port_to_rcy_port_unmap_set_verify(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info)
{
    bcm_port_t tmp_mirror_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /** convert gport to local port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, forward_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info));
    forward_port = gport_info.local_port;

    /** verify forward port */
    SHR_IF_ERR_EXIT(dnx_mirror_port_to_rcy_port_map_common_verify(unit, forward_port, rcy_map_info));

    /**  make sure the forward port is mapped */
    SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_get(unit, forward_port, &tmp_mirror_port));
    if (tmp_mirror_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "forward port %d is not mapped", forward_port);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - implement mirror_port_to_rcy_port mapping
 */
shr_error_e
dnx_mirror_port_to_rcy_port_map_set(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info)
{
    bcm_port_t rcy_mirror_port;
    int forward_interface_id;
    int per_priority_exist[DNX_MIRROR_RCY_PRIORITY_NOF];
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Make sure that this array, which is used down in dnx_port_ingr_reassembly_per_priority_context_set()
     * has known initial values.
     */
    sal_memset(per_priority_exist, 0, sizeof(per_priority_exist));
    /** convert gport to local port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, forward_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info));
    forward_port = gport_info.local_port;

    rcy_mirror_port = rcy_map_info->rcy_mirror_port;

    SHR_INVOKE_VERIFY_DNX(dnx_mirror_port_to_rcy_port_map_set_verify(unit, forward_port, rcy_map_info));

    /*
     * Set priority
     * If no priority provided, configure all priorities
     */
    if (rcy_map_info->priority_bitmap == 0
        || rcy_map_info->priority_bitmap & BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH)
    {
        per_priority_exist[DNX_MIRROR_RCY_PRIORITY_LOSSLESS] = 1;
        SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_port_add(unit,
                                                                           DNX_MIRROR_RCY_PRIORITY_LOSSLESS,
                                                                           forward_port));
    }

    if (rcy_map_info->priority_bitmap == 0
        || rcy_map_info->priority_bitmap & BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL)
    {
        per_priority_exist[DNX_MIRROR_RCY_PRIORITY_NORMAL] = 1;
        SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_port_add(unit, DNX_MIRROR_RCY_PRIORITY_NORMAL,
                                                                           forward_port));
    }

    /** map egress interface to mirror channel */
    SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_map(unit, forward_port, rcy_mirror_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, forward_port, &forward_interface_id));
    SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_set(unit, rcy_mirror_port, forward_interface_id));

    /*
     * configure reassembly context for the mirror port
     */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_per_priority_context_set(unit, rcy_mirror_port, per_priority_exist));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - implement mirror_port_to_rcy_port unmapping
 */
shr_error_e
dnx_mirror_port_to_rcy_port_unmap_set(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info)
{
    bcm_port_t rcy_mirror_port;
    int priority;
    uint8 priority_exists;

    int core_id;
    bcm_pbmp_t ports_pbmp;
    bcm_port_t curr_egr_port, curr_mirror_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    int found;

    SHR_FUNC_INIT_VARS(unit);

    /** convert gport to local port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, forward_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info));
    forward_port = gport_info.local_port;

    SHR_INVOKE_VERIFY_DNX(dnx_mirror_port_to_rcy_port_unmap_set_verify(unit, forward_port, rcy_map_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, forward_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_get(unit, forward_port, &rcy_mirror_port));

    for (priority = 0; priority < DNX_MIRROR_RCY_PRIORITY_NOF; priority++)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_member(unit, priority,
                                                                         forward_port, &priority_exists));
        if (priority_exists)
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_priority_unset(unit, rcy_mirror_port, priority));
            SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_port_remove(unit, priority, forward_port));
        }
    }

    /** unmap egress interface to mirror channel */
    SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_map(unit, forward_port, rcy_map_info->rcy_mirror_port));

    /** get all ports of the same interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, forward_port, 0, &ports_pbmp));

    /** check if this is the last port mapped to the mirror port */
    found = 0;
    BCM_PBMP_ITER(ports_pbmp, curr_egr_port)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_get(unit, curr_egr_port, &curr_mirror_port));
        if (curr_mirror_port == rcy_mirror_port)
        {
            found = 1;
            break;
        }
    }

    /** if this is the last port -- unset the mapping of mirror port to egress interface */
    if (found == 0)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_set(unit, rcy_mirror_port, 0));
        SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_set(unit, rcy_mirror_port,
                                                                    DNX_ALGO_PORT_EGR_IF_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - implement mirror_port_to_rcy_port get
 */
shr_error_e
dnx_mirror_port_to_rcy_port_map_get(
    int unit,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t * rcy_map_info)
{
    int priority;
    uint8 priority_exists;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_info_s forward_port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** convert gport to local port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, forward_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info));
    forward_port = gport_info.local_port;

    /** If forward port is not of type EGR_TM, return unmapped */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, forward_port, &forward_port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, forward_port_info))
    {
        rcy_map_info->rcy_mirror_port = BCM_PORT_INVALID;
        SHR_EXIT();
    }

    /** get mapped mirror port */
    SHR_IF_ERR_EXIT(dnx_mirror_port_egr_port_to_mirror_get(unit, forward_port, &rcy_map_info->rcy_mirror_port));

    if (rcy_map_info->rcy_mirror_port != DNX_ALGO_PORT_INVALID)
    {
        /** get mapped priorities */
        for (priority = 0; priority < DNX_MIRROR_RCY_PRIORITY_NOF; priority++)
        {
            SHR_IF_ERR_EXIT(dnx_mirror_db.egress_mirror_priority.pbmp_member(unit, priority,
                                                                             forward_port, &priority_exists));
            if (priority_exists)
            {
                rcy_map_info->priority_bitmap |= (priority == DNX_MIRROR_RCY_PRIORITY_NORMAL ?
                                                  BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL :
                                                  BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH);
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
dnx_mirror_rcy_port_add_handle(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * nothing to do
     */

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_rcy_port_remove_handle(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    int mapped_egress_interface;

    SHR_FUNC_INIT_VARS(unit);

    /** get logical port which is about to be removed */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
    {
        /*
         * check if there are egress ports mapped to this mirror port
         * if there are - exit with error 
         */
        SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_get(unit, port, &mapped_egress_interface));
        if (mapped_egress_interface != DNX_ALGO_PORT_EGR_IF_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "There is forward port mapped to this mirror port %d. Need to unmap first",
                         port);
        }

    }

exit:
    SHR_FUNC_EXIT;
}
