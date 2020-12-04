/** \file appl_ref_l2_init.c
 * 
 *
 * L@ application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/multicast.h>
#include <bcm/port.h>
#include <bcm/l2.h>
#include <bcm/switch.h>
#include <src/appl/reference/dnx/appl_ref_sys_device.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <soc/sand/sand_aux_access.h>

#include <appl/reference/sand/appl_ref_sand.h>
#include "appl_ref_l2_init.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** OLP is using by default core 0 */
#define OLP_DEFAULT_CORE 0
#define LEARN_DSP_ETHER_TYPE 0xab00
#define MAC_ADDRESS_SIZE 6
#define DEFAULT_TIME_FOR_AGE_META_CYCLE 40

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/* L2 event callback */
void
l2_olp_event_handle(
    int unit,
    bcm_l2_addr_t * l2addr,
    int operation,
    void *userdata)
{
    int rv = BCM_E_NONE;

    if (operation == BCM_L2_CALLBACK_LEARN_EVENT)
    {
        rv = bcm_l2_addr_add(unit, l2addr);
    }
    else if (operation == BCM_L2_CALLBACK_AGE_EVENT)
    {
        rv = bcm_l2_addr_delete(unit, l2addr->mac, l2addr->vid);
    }
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "L2 event handling failed. " "Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
    }
}

/**
 * \brief - Set the learn mode in case the soc property l2_mode exists
 *
 * \param [in] unit - unit id
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
appl_l2_mode_set(
    int unit)
{
    int learn_mode;
    int mode = 0;

    SHR_FUNC_INIT_VARS(unit);

    learn_mode = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "l2_mode", 0);

    /*
     * Configure L2 mode
     */
    switch (learn_mode)
    {
        case 0:
            mode = BCM_L2_INGRESS_DIST;
            break;
        case 1:
            mode = BCM_L2_INGRESS_CENT;
            break;
        case 2:
            mode = (BCM_L2_INGRESS_CENT | BCM_L2_LEARN_CPU);
            bcm_l2_addr_register(unit, l2_olp_event_handle, NULL);
            break;
        default:
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "appl_l2_mode_set failed. " "learn_mode:%d\n"), learn_mode));
    }

    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchL2LearnMode, mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the OLP logical port.
 *
 * \param [in] unit - unit ID
 * \param [out] olp_port - return OLP logical port. Return -1 if more than 1 OLP port were configured or 0 ports configured.

 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_olp_port_get(
    int unit,
    bcm_gport_t * olp_port)
{
    int num_of_ports;
    bcm_pbmp_t pbmp;
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the OLP port number
     */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    BCM_PBMP_ASSIGN(pbmp, port_config.olp);

    /** Make sure that OLP port was configured. */
    BCM_PBMP_COUNT(pbmp, num_of_ports);
    if (num_of_ports == 0)
    {
        *olp_port = -1;
    }
    else
    {
        /** Take the first OLP port */
        BCM_PBMP_ITER(pbmp, *olp_port)
        {
            /** There is only one OLP port on a device */
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Open a multicast group that contains all the OLP port from all the devices in the fabric
 *
 * \param [in] unit - unit ID
 * \param [in] olp_port - logical port of the OLP
 * \param [out] mc_group_id - newly created mc group ID
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
appl_dnx_open_ingress_mc_group_for_olp(
    int unit,
    bcm_gport_t olp_port,
    bcm_multicast_t * mc_group_id)
{
    int flags;
    appl_dnx_sys_device_t *sys_params;
    bcm_gport_t gport;
    bcm_multicast_replication_t rep_array[SOC_MAX_NUM_DEVICES];
    int rep_array_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mc_group_id, _SHR_E_PARAM, "mc_group_id is not allocated.");

    sal_memset(rep_array, 0, SOC_MAX_NUM_DEVICES * sizeof(bcm_multicast_replication_t));

    /** Get the last MC group ID */
    *mc_group_id = OLP_MC_ID;

    flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    SHR_IF_ERR_EXIT(bcm_multicast_create(unit, flags, mc_group_id));

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** for stand alone configuration - each device has only its own OLP ports */
    if (sys_params->stand_alone_configuration)
    {
        int system_port_id;
        SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert
                        (unit, sys_params->unit_to_device_index[unit], APPL_DNX_NOF_SYSPORTS_PER_DEVICE,
                         olp_port, &system_port_id));
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, system_port_id);
        rep_array[rep_array_size].port = gport;
        ++rep_array_size;
    }
    else
    {
        system_port_mapping_t matching_criteria = {.port_type = APPL_PORT_TYPE_OLP };
        system_port_mapping_t sys_port_map_array[SOC_MAX_NUM_DEVICES];
        int nof_ports = 0;
        /** iterate over all system ports and take only OLP ports */
        SHR_IF_ERR_EXIT(appl_dnx_system_port_db_get_multiple(unit, sys_params->system_port_db, &matching_criteria,
                                                             SOC_MAX_NUM_DEVICES, sys_port_map_array, &nof_ports));

        for (int index_in_array = 0; index_in_array < nof_ports; ++index_in_array)
        {
            /** only a single OLP port per device */
            if (sys_port_map_array[index_in_array].local_port == olp_port)
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport, sys_port_map_array[index_in_array].system_port_id);
                rep_array[rep_array_size].port = gport;
                ++rep_array_size;
            }
        }
    }

    SHR_IF_ERR_EXIT(bcm_multicast_add
                    (unit, *mc_group_id, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, rep_array_size,
                     rep_array));

exit:
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief -
 * This function initialize the OLP's parameters.
 * Configure a wrapper header for the DSPs of the OLP using bcm_l2_learn_msgs_config_set.
 * The DSPs are sent to a MC group of all the OLP ports in the system.
 * Configure this MC group as well.
 */
static shr_error_e
appl_dnx_olp_init(
    int unit)
{
    bcm_l2_learn_msgs_config_t learn_msgs;
    bcm_multicast_t mc_group_id;
    bcm_gport_t olp_port = -1;

    uint8 src_mac_address[MAC_ADDRESS_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x12, 0x55 };
    uint8 dest_mac_address[MAC_ADDRESS_SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x44, 0x88 };

    SHR_FUNC_INIT_VARS(unit);

    /** Create a MC group with all the OLP ports for the DSP distribution */

    /** set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);

    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING | BCM_L2_LEARN_MSG_DEST_MULTICAST | BCM_L2_LEARN_MSG_ETH_ENCAP;

    /*
     * Get the OLP port number
     */
    SHR_IF_ERR_EXIT(appl_dnx_olp_port_get(unit, &olp_port));

    /** Make sure that an OLP port is configured */
    if (olp_port != -1)
    {
        /** Create a mc group with all the OLP ports */
        SHR_IF_ERR_EXIT(appl_dnx_open_ingress_mc_group_for_olp(unit, olp_port, &mc_group_id));

        /** Set the destination of the queue to be multicast group with all the OLP ports in the fabric */
        learn_msgs.dest_group = mc_group_id;

        /**
         *  follow attributes set the encapsulation of the learning
         *  messages learning message encapsulated with ethernet
         *  header
         */
        learn_msgs.ether_type = LEARN_DSP_ETHER_TYPE;
        sal_memcpy(learn_msgs.src_mac_addr, src_mac_address, MAC_ADDRESS_SIZE);
        sal_memcpy(learn_msgs.dst_mac_addr, dest_mac_address, MAC_ADDRESS_SIZE);

        SHR_IF_ERR_EXIT(bcm_l2_learn_msgs_config_set(unit, &learn_msgs));

        /** Configure a default setting for the shadow queue of the OLP. The default is configured in the same way like the LEARN queue */
        learn_msgs.flags &= ~BCM_L2_LEARN_MSG_LEARNING;
        learn_msgs.flags |= BCM_L2_LEARN_MSG_SHADOW;
        SHR_IF_ERR_EXIT(bcm_l2_learn_msgs_config_set(unit, &learn_msgs));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
appl_dnx_l2_init(
    int unit)
{
    int age_meta_cycle = DEFAULT_TIME_FOR_AGE_META_CYCLE;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_machine_pause_after_flush))
    {
        age_meta_cycle = 0;
    }

    /** Configure the DSPs sent by the OLP */
    SHR_IF_ERR_EXIT(appl_dnx_olp_init(unit));

    /** Set default aging time */
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, age_meta_cycle));

    /** Set learning mode */
    SHR_IF_ERR_EXIT(appl_l2_mode_set(unit));

exit:
    SHR_FUNC_EXIT;
}
