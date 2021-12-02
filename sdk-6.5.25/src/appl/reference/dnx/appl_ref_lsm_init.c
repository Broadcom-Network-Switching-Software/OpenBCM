/** \file appl_ref_debug_lsm_run.c
 *
 * lsm application init procedures for DNX
 *
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LSM

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/shell.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/utilex/utilex_str.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm/switch.h>
#include <sal/appl/sal.h>
#include <bcm/pkt.h>
#include <bcm/instru.h>
#include <shared/utilex/utilex_str.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include "src/bcm/dnx/algo/port/algo_port_internal.h"
#include <appl/reference/dnx/appl_ref_lsm.h>
/*
 * }
 */

/** global to hold the init state of the tool */
static uint8 Lsm_initizilazed = FALSE;
/** states if tool should dump prints to log */
static uint8 Lsm_log_enable = FALSE;
/** lsm tool CPU port that packets are sent to*/
static int Lsm_cpu_port = 0;
/** path of lsm log file*/
static char Lsm_log_path[MAX_SIZE_PATH];
/** global that holds the last time lsm event happend */
static sal_time_t Lsm_last_call;
/** current vis mode of the tool */
static int Lsm_vis_mode;

/**
 * List of trap codes lsm init will configure to punt to CPU. set currently
 * to 0 because traps are not configured and no port is configured.
 */
static lsm_init_trap_t Lsm_traps[] = {
    {bcmRxTrapPortNotVlanMember, 0, 0},
    {bcmRxTrapMplsUnknownLabel, 0, 0},
    {bcmRxTrapMyMacAndUnknownL3, 0, 0},
    {bcmRxTrapMyMacAndIpDisabled, 0, 0},
    {bcmRxTrapMyMacAndMplsDisable, 0, 0},
    {bcmRxTrapOamLevel, 0, 0},
    {bcmRxTrapUnknownDest, 0, 0}
};

/**
 * List of ports and their visibility status
 */
static lsm_init_vis_t Lsm_vis_modes[MAX_NOF_PORTS];

/**
 * \brief
 *   This function verifies port is valid and exists is in index range
 * \param [in] unit - The unit number.
 * \param [in] logical_port- The port number
 * \param [in] port_index- The port index used in lsm_vis_mode[port_index]
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_vis_port_verify(
    int unit,
    bcm_port_t logical_port,
    int port_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    if (port_index > MAX_NOF_PORTS)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "wrong port index \n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function set the port to vis=on, force=off and saves the current state of port
 *   This will set the ports to set visibility on packet only when PTCH.qaul=2
 * \param [in] unit - The unit number.
 * \param [in] logical_port- The port number
 * \param [in] port_index- The port index to be saved in lsm_vis_mode[port_index]
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_vis_port_enable(
    int unit,
    bcm_port_t logical_port,
    int port_index)
{
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;
    int vis_force;
    int vis_enable;
    uint32 instru_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_ref_lsm_vis_port_verify(unit, logical_port, port_index));
    flags = 0;
    sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
    sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));

    /** Indicate ingress visibility */
    if (port_mapping_info.pp_port != BCM_PORT_INVALID)
    {
        instru_flags |= BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS;
    }

    /** set old vis state in vis_modes and set vis on port to be enabled but not forces
     * this is needed in order to only catch packets sent from CPU with PTCH.qual=2 */
    if (instru_flags != 0)
    {

        SHR_IF_ERR_EXIT(bcm_instru_gport_control_get(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisForce, &vis_force));
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_get(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisEnable, &vis_enable));

        Lsm_vis_modes[port_index].is_force_enable = vis_force;
        Lsm_vis_modes[port_index].is_vis_enable = vis_enable;
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_set(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisForce, FALSE));
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_set(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisEnable, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all ports to vis=on, force=off (visibility enabled, but only set on packet with ptch.qaul=2)
 * \param [in] unit - The unit number.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_vis_enable(
    int unit)
{
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp;
    int port_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_instru_control_get(unit, 0, bcmInstruControlVisMode, &Lsm_vis_mode));

    SHR_IF_ERR_EXIT(bcm_instru_control_set(unit, 0, bcmInstruControlVisMode, bcmInstruVisModeSelective));

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    BCM_PBMP_ASSIGN(pbmp, port_config.all);
    BCM_PBMP_REMOVE(pbmp, port_config.sfi);
    BCM_PBMP_REMOVE(pbmp, port_config.olp);
    /** set vis mode on all ports */
    port_index = 0;
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_vis_port_enable(unit, logical_port, port_index));
        port_index++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This function sets the port to deinit values
* \param [in] unit - The unit number.
* \param [in] logical_port- The port number
* \param [in] port_index- The port index to be saved
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
static shr_error_e
appl_ref_lsm_vis_port_disable(
    int unit,
    bcm_port_t logical_port,
    int port_index)
{
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;
    uint32 instru_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_vis_port_verify(unit, logical_port, port_index));
    flags = 0;
    sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
    sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));

    /** Indicate ingress visibility */
    if (port_mapping_info.pp_port != -1)
    {
        instru_flags |= BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS;
    }

    /** set visibility */
    if (instru_flags != 0)
    {
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_set(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisForce,
                                                     Lsm_vis_modes[port_index].is_force_enable));
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_set
                        (unit, logical_port, instru_flags, bcmInstruGportControlVisEnable,
                         Lsm_vis_modes[port_index].is_vis_enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This function sets all pots to deinit values
* \param [in] unit - The unit number.
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
static shr_error_e
appl_ref_lsm_vis_disable(
    int unit)
{
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp;
    int port_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_instru_control_set(unit, 0, bcmInstruControlVisMode, Lsm_vis_mode));

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    BCM_PBMP_ASSIGN(pbmp, port_config.all);
    BCM_PBMP_REMOVE(pbmp, port_config.sfi);
    BCM_PBMP_REMOVE(pbmp, port_config.olp);

    port_index = 0;
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_vis_port_disable(unit, logical_port, port_index));
        port_index++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function resets list of traps to init values
 * \param [in] unit - The unit number.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_deinit_traps(
    int unit)
{
    int nof_traps;
    int trap_id;
    bcm_rx_trap_config_t trap_config;

    SHR_FUNC_INIT_VARS(unit);

    nof_traps = COUNTOF(Lsm_traps);

    for (int i = 0; i < nof_traps; i++)
    {

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, Lsm_traps[i].trap_type, &trap_id));
        if (Lsm_traps[i].is_new == TRUE)
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
        }
        else
        {
            bcm_rx_trap_config_t_init(&trap_config);
            SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

            trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
            trap_config.dest_port = Lsm_traps[i].port;
            SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets list of traps to be trapped to port input
 * \param [in] unit - The unit number.
 * \param [in] port - The port number.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_init_traps(
    int unit,
    int port)
{
    int trap_id;
    int trap_create_flags;
    int nof_traps;
    int converted_trap_id = 0;
    uint8 is_trap_allocated;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);
    bcm_rx_trap_config_t_init(&trap_config);

    trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = port;
    trap_config.trap_strength = 8;
    trap_create_flags = 0;
    nof_traps = COUNTOF(Lsm_traps);
    for (int i = 0; i < nof_traps; i++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, Lsm_traps[i].trap_type, &converted_trap_id));
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, converted_trap_id, &is_trap_allocated));
        if (is_trap_allocated == TRUE)
        {
            bcm_rx_trap_config_t_init(&trap_config);
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, trap_create_flags, Lsm_traps[i].trap_type, &trap_id));
            SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

            Lsm_traps[i].is_new = FALSE;
            Lsm_traps[i].port = trap_config.dest_port;

            trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
            trap_config.dest_port = port;
        }
        else
        {
            Lsm_traps[i].is_new = TRUE;
            SHR_IF_ERR_CONT(bcm_rx_trap_type_create(unit, trap_create_flags, Lsm_traps[i].trap_type, &trap_id));
        }

        SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function enables\disable the logger
 * \param [in] unit - The unit number.
 * \param [in] enable - 1 - enable log 0 - disable log
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_cb_set_log(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /** enable only if logs were enabled on lsm init and function is to enable */
    if (Lsm_log_enable & enable)
    {
        char log_cmd[MAX_SIZE_CMD];
        char timestamp[SH_SAND_MAX_TIME_SIZE];
        sh_sand_time_get(timestamp);
        sal_strncpy_s(log_cmd, "log file=", MAX_SIZE_CMD);
        sal_strncat_s(log_cmd, Lsm_log_path, MAX_SIZE_CMD);
        sal_strncat_s(log_cmd, "_", MAX_SIZE_CMD);
        sal_strncat_s(log_cmd, timestamp, MAX_SIZE_CMD);
        sal_strncat_s(log_cmd, ".txt", MAX_SIZE_CMD);
        if (sh_process_command(unit, log_cmd) != CMD_OK)
        {
            SHR_ERR_EXIT(BCM_E_INTERNAL, "Error starting log file\n");
        }
    }
    /** disable only if logs were enabled on lsm init and disabled in function */
    else if (Lsm_log_enable)
    {
        if (sh_process_command(unit, "log off") != CMD_OK)
        {
            SHR_ERR_EXIT(BCM_E_INTERNAL, "Error stopping log file\n");
        }
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function prints the packet
 * \param [in] unit - The unit number.
 * \param [in] rx_pkt - rx packet to print
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_cb_print_packet(
    int unit,
    bcm_pkt_t * rx_pkt)
{
    uint8 *data;

    int rv = BCM_E_NONE;
    int tot_len;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(rx_pkt, BCM_E_INTERNAL, "rx packet pointer is null.") tot_len = rx_pkt->tot_len;
    LOG_CLI((BSL_META_U(unit, "packet received including system headers data=")));
    for (int i = 0; i < tot_len; i++)
    {

        rv = bcm_pkt_byte_index(rx_pkt, i, NULL, NULL, &data);
        if (rv != BCM_E_NONE)
        {
    /** End of packets data */
            if (rv == BCM_E_NOT_FOUND)
            {
                rv = BCM_E_NONE;
            }
            break;
        }
        else
        {
            if (data)
            {
                LOG_CLI((BSL_META_U(unit, "%02x"), *data));
            }
        }

    }
    LOG_CLI((BSL_META_U(unit, "\n")));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function starts a "lsm" packet event
 * \param [in] unit - The unit number.
 * \param [in] rx_pkt -  packet to resend to "lsm" event
 * \return
 *   Error code (as per 'shr_error_e').
 * \remarks
 * * a lsm packet event resends the packet to pipe and collects information from device for this packet
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_cb_packet_event(
    int unit,
    bcm_pkt_t * rx_pkt)
{
    uint8 *data;
    lsm_event_info_t info;
    int offset = 0;
    int rv = BCM_E_NONE;
    bcm_pkt_t *tx_pkt_info = NULL;
    int pkt_len;
    int tot_len;
    int src_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(rx_pkt, BCM_E_INTERNAL, "rx packet pointer is null.") pkt_len = rx_pkt->pkt_len;
    tot_len = rx_pkt->tot_len;
    src_port = rx_pkt->src_port;

    offset = tot_len - pkt_len;
    SHR_IF_ERR_EXIT(bcm_pkt_alloc(unit, pkt_len, 0, &tx_pkt_info));
    tx_pkt_info->src_port = src_port;
    LOG_CLI((BSL_META_U
             (unit,
              "|--------------------------------------------------------------------------------------------------------------|\n")));
    LOG_CLI((BSL_META_U(unit, "Packet will be resent to device: tx 1 PSRC=%d data="), rx_pkt->src_port));
    for (int i = 0; i < pkt_len; i++)
    {
        rv = bcm_pkt_byte_index(rx_pkt, offset, NULL, NULL, &data);
        if (rv != BCM_E_NONE)
        {
            /** End of packets data */
            if (rv == BCM_E_NOT_FOUND)
            {
                rv = BCM_E_NONE;
            }
            break;
        }
        else
        {
            if (data)
            {
                tx_pkt_info->pkt_data[0].data[i] = rx_pkt->pkt_data[0].data[offset];
                LOG_CLI((BSL_META_U(unit, "%02x"), *data));
                offset++;
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));
    info.pkt = tx_pkt_info;
    info.pkt->pkt_data[0].len = pkt_len;
    info.event = lsm_packet_event;
    appl_ref_lsm_run(unit, &info);

exit:
    bcm_pkt_free(unit, tx_pkt_info);
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This CB function prints the packet , prints info  and starts a lsm event.
 *   funtion will always return BCM_RX_NOT_HANDLED so other CBs can happen
 * \param [in] unit - The unit number.
 * \param [in] rx_pkt - rx packet
 * \param [in] cookie - cookie
 * \return
 *  void
 * \see
 *   shr_error_e
 */
bcm_rx_t
appl_ref_lsm_cb(
    int unit,
    bcm_pkt_t * rx_pkt,
    void *cookie)
{

    time_t current_time;
    /** check packet is recived on lsm CPU port that was set on lsm init function*/
    if (Lsm_cpu_port != BCM_GPORT_LOCAL_GET(rx_pkt->dst_gport))
    {
        return BCM_RX_NOT_HANDLED;
    }

    current_time = sal_time();
    if (current_time > Lsm_last_call + 1)
    {
        if (appl_ref_lsm_cb_set_log(unit, 1) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error trying to enable logger\n")));
        }
        LOG_CLI((BSL_META_U
                 (unit,
                  "|--------------------------------------------------------------------------------------------------------------|\n")));

        LOG_CLI((BSL_META_U(unit, "Packet received on lsm CPU port \n")));
        LOG_CLI((BSL_META_U(unit, "lsm packet event has started now \n")));
        LOG_CLI((BSL_META_U
                 (unit,
                  "|--------------------------------------------------------------------------------------------------------------|\n")));

        LOG_CLI((BSL_META_U(unit, "time event started: %lu  \n"), current_time));
        LOG_CLI((BSL_META_U
                 (unit,
                  "|--------------------------------------------------------------------------------------------------------------|\n")));
        if (appl_ref_lsm_cb_print_packet(unit, rx_pkt) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error printing packet\n")));
        }
        if (appl_ref_lsm_cb_packet_event(unit, rx_pkt) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error in lsm event \n")));
            return BCM_RX_NOT_HANDLED;
        }
        if (appl_ref_lsm_cb_set_log(unit, 0) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error closing log file\n")));
        }
        Lsm_last_call = sal_time();
    }

    return BCM_RX_NOT_HANDLED;
}

static shr_error_e
appl_ref_lsm_pkt_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!bcm_rx_active(unit))
    {
       /** Initialize RX as default */
        SHR_IF_ERR_EXIT(bcm_rx_cfg_init(unit));

        /** Start RX with default configraiton */
        SHR_IF_ERR_EXIT(bcm_rx_start(unit, NULL));
    }
    SHR_IF_ERR_EXIT(bcm_rx_register(unit, "lsm-RX", appl_ref_lsm_cb, BCM_RX_PRIO_MAX, NULL, BCM_RCO_F_ALL_COS));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
appl_ref_lsm_pkt_stop(
    int unit)
{
    return bcm_rx_unregister(unit, appl_ref_lsm_cb, BCM_RX_PRIO_MAX);
}

static shr_error_e
appl_ref_lsm_init_verify(
    int unit,
    lsm_init_info_t * event_info)
{
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;
    SHR_FUNC_INIT_VARS(unit);

    /** The key is the header type of outgoing port */
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, event_info->port, key, &value));
    /** lsm tool can only work  if header type is CPU, because packet will be received with FTMH  */
    if (value.value != BCM_SWITCH_PORT_HEADER_TYPE_CPU)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Port %d is not configured as PORT_HEADER_TYPE_CPU  \n", event_info->port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is used to parse the part of the lsm_actions.xml into an lsm_qualifiers_fields_t
 *   object and using the appl_ref_lsm_qualifiers_db_info_add API adds it info the lsm tool's
 *   database.
 *
 * \param [in] unit - The unit number.
 * \param [in] qualifier_db_node - the XML node currently being parsed.
 *
 * \return
 *  shr_error_e
 */
static shr_error_e
appl_ref_lsm_qualifier_info_parse(
    int unit,
    xml_node qualifier_db_node)
{
    int qualifier_index = 0;
    xml_node current = NULL;
    attribute_param_t attrs[MAX_NOF_QUAL];
    lsm_qualifiers_fields_t qulifier_fields[MAX_NOF_QUAL];

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(qulifier_fields, 0, sizeof(lsm_qualifiers_fields_t) * MAX_NOF_QUAL);
    /**
     * loop over all child elements in qualifier tag
     */
    RHDATA_ITERATOR(current, qualifier_db_node, "qualifier")
    {
        sal_memset(attrs, 0, sizeof(attribute_param_t) * MAX_NOF_QUAL);
        dbx_xml_property_get_all(current, attrs, MAX_NOF_QUAL);
        /**
         * loop over all attributes of current tag and act (parse) according to attribute's name.
         */
        for (int attr_index = 0; attr_index < MAX_NOF_QUAL && attrs[attr_index].name[0] != '\0'; ++attr_index)
        {
            if (sal_strncmp(attrs[attr_index].name, "name", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(qulifier_fields[qualifier_index].name, attrs[attr_index].value, MAX_SIZE_STR);
            }
            else if (sal_strncmp(attrs[attr_index].name, "type", MAX_SIZE_STR) == 0)
            {
                qulifier_fields[qualifier_index].qualifier_type = sal_atoi(attrs[attr_index].value);
            }
            else if (sal_strncmp(attrs[attr_index].name, "sig_name", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(qulifier_fields[qualifier_index].qualifier_command.signal_name, attrs[attr_index].value,
                              MAX_SIZE_STR);
            }
            else if (sal_strncmp(attrs[attr_index].name, "block", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(qulifier_fields[qualifier_index].qualifier_command.signal_block,
                              attrs[attr_index].value, MAX_SIZE_STR);
            }
            else if (sal_strncmp(attrs[attr_index].name, "to", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(qulifier_fields[qualifier_index].qualifier_command.signal_to, attrs[attr_index].value,
                              MAX_SIZE_STR);
            }
            else if (sal_strncmp(attrs[attr_index].name, "from", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(qulifier_fields[qualifier_index].qualifier_command.signal_from, attrs[attr_index].value,
                              MAX_SIZE_STR);
            }
            else if (sal_strncmp(attrs[attr_index].name, "size", MAX_SIZE_STR) == 0)
            {
                qulifier_fields[qualifier_index].qual_size = (uint32) sal_atoi(attrs[attr_index].value);
            }
            else
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "unrecognized qualifier attribute %s.\n", attrs[attr_index].name);
            }
        }
        /**
         * used to keep count of the number of qualifier elements found.
         */
        qualifier_index++;
    }
    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifiers_db_info_add(unit, qualifier_index, qulifier_fields));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is used to parse the part of the lsm_actions.xml into an lsm_actions_entry_t
 *   object and using the appl_ref_lsm_action_table_entry_add API adds it info the lsm tool's
 *   database.
 *
 * \param [in] unit - The unit number.
 * \param [in] action_node - the XML node currently being parsed.
 *
 * \return
 *  shr_error_e
 */
static shr_error_e
appl_ref_lsm_action_table_parse(
    int unit,
    xml_node action_node,
    uint32 table_id)
{
    int index;
    xml_node current_action, current_result, current_key;
    attribute_param_t attrs[RHKEYWORD_MAX_SIZE];
    attribute_param_t single_attr;
    lsm_actions_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * loop over all child elements in action tag
     */
    RHDATA_ITERATOR(current_action, action_node, "action")
    {
        /** init entry values */
        sal_memset(&entry, 0, sizeof(lsm_actions_entry_t));
        for (index = 0; index < MAX_NOF_QUAL; ++index)
        {
            entry.qualifiers.list_of_qualifiers[index].qualifier_mask = -1;
        }

        current_key = dbx_xml_child_get_first(current_action, "key_prio");
        SHR_IF_ERR_CONT(dbx_xml_property_get_int(current_key, "priority", (int *) &entry.priority_id));

        /**
         * each key should hold a name/value pair and a corresponding mask/value pair.
         * loop over all the action's child tags of type key and store the relevant data for later usage.
         */
        RHDATA_ITERATOR(current_key, current_action, "key")
        {
            /**
             * loop over all attributes of current tag and act (parse) according to attribute's name.
             */
            SHR_IF_ERR_EXIT(dbx_xml_property_get_all(current_key, attrs, MAX_TAG_ATTR));
            for (index = 0; index < MAX_TAG_ATTR; ++index)
            {
                if (sal_strncmp(attrs[index].name, "mask", MAX_SIZE_STR) == 0)
                {
                    entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].qualifier_mask =
                        sal_strtol(attrs[index].value, NULL, 16);
                }
                else
                {
                    sal_strncpy_s(entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].
                                  qualifer_field.name, attrs[index].name,
                                  sizeof(entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].
                                         qualifer_field.name));

                    /**
                     * handle hex or decimal values.
                     */
                    if (sal_strlen(attrs[index].value) > 2 && attrs[index].value[1] == 'x')
                    {
                        entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].qualifier_value =
                            sal_strtol(&attrs[index].value[2], NULL, 16);
                    }
                    else
                    {
                        entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].qualifier_value =
                            sal_atoi(attrs[index].value);
                    }
                }
            }

            entry.qualifiers.nof_qualifiers++;
        }

        SHR_IF_ERR_EXIT(appl_ref_lsm_qualifiers_db_entry_update(unit, table_id, &entry.qualifiers));

        /**
         * each result command signified the action type to perform and some additional data.
         * loop over all the action's child tags of type result and store the relevant data for later usage.
         */
        RHDATA_ITERATOR(current_result, current_action, "result")
        {
            SHR_IF_ERR_EXIT(dbx_xml_property_get_all(current_result, &single_attr, 1));
            if (sal_strncmp(single_attr.name, "diag_commands", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(entry.list_of_action_results[entry.nof_actions].diags, single_attr.value, MAX_SIZE_DIAG);
            }
            else if (sal_strncmp(single_attr.name, "print", MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(entry.list_of_action_results[entry.nof_actions].prt_info, single_attr.value,
                              MAX_SIZE_PRINT);
            }
            entry.nof_actions++;
        }
        SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_entry_add(unit, table_id, &entry));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_ref_lsm_parse_xml_file(
    int unit,
    char *xml_path)
{
    xml_node qual_top = NULL;
    xml_node action_top = NULL;
    xml_node current = NULL;

    uint32 action_table_id;
    char action_file_path[RHNAME_MAX_SIZE] = { 0 };
    char qualifier_file_path[RHNAME_MAX_SIZE] = { 0 };
    char action_pre_comp[RHNAME_MAX_SIZE] = { 0 };
    char qualifier_pre_comp[RHNAME_MAX_SIZE] = { 0 };
    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbx_file_get_db(unit, db_name, base_db_name));
    /**
     * If a specific XML path given through the diag command use that path to load xmls, otherwise
     * look for xml file under "tools/sand/db/<chip>/internal/lsm/" and if no xmls there eiter
     * load precompiled version.
     */
    if (xml_path[0] != '\0')
    {
        sal_snprintf(action_file_path, RHFILE_MAX_SIZE - 1, "%s/%s", xml_path, LSM_ACTIONS_XML_PATH);
        sal_snprintf(qualifier_file_path, RHFILE_MAX_SIZE - 1, "%s/%s", xml_path, LSM_QUALIFIERS_XML_PATH);
    }
    else
    {
        sal_snprintf(action_file_path, RHFILE_MAX_SIZE - 1, "%s/lsm/%s", db_name, LSM_ACTIONS_XML_PATH);
        sal_snprintf(qualifier_file_path, RHFILE_MAX_SIZE - 1, "%s/lsm/%s", db_name, LSM_QUALIFIERS_XML_PATH);
    }

    sal_snprintf(action_pre_comp, RHFILE_MAX_SIZE - 1, "lsm/%s", LSM_ACTIONS_XML_PATH);
    sal_snprintf(qualifier_pre_comp, RHFILE_MAX_SIZE - 1, "lsm/%s", LSM_QUALIFIERS_XML_PATH);

    qual_top = (xml_node) dbx_file_get_xml_top(unit, qualifier_file_path, "qualifiers", CONF_OPEN_NO_ERROR_REPORT);
    if (qual_top == NULL)
    {
        qual_top =
            (xml_node) dbx_pre_compiled_devices_top_get(unit, qualifier_pre_comp, "qualifiers", CONF_OPEN_PER_DEVICE);
    }

    action_top = (xml_node) dbx_file_get_xml_top(unit, action_file_path, "actions_db", CONF_OPEN_NO_ERROR_REPORT);
    if (action_top == NULL)
    {
        action_top =
            (xml_node) dbx_pre_compiled_devices_top_get(unit, action_pre_comp, "actions_db", CONF_OPEN_PER_DEVICE);
    }

    if (qual_top == NULL || action_top == NULL)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "missing actions/qualifiers xml files.\n");
    }

    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifiers_db_table_create(unit));
    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifier_info_parse(unit, qual_top));

    RHDATA_ITERATOR(current, action_top, "action_table")
    {
        dbx_xml_property_get_int(current, "id", (int *) &action_table_id);
        SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_create(unit, action_table_id));
        SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_parse(unit, current, action_table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_init(
    int unit,
    lsm_init_info_t * init_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_deinit(unit));
    sal_memset(&Lsm_vis_modes, 0, sizeof(lsm_init_vis_t) * MAX_NOF_PORTS);
#ifndef    SAL_THREAD_NAME_PRINT_DISABLE
    SHR_IF_ERR_EXIT(sal_console_thread_name_print_enable(FALSE));
#endif
    Lsm_cpu_port = init_info->port;
    sal_strncpy_s(Lsm_log_path, init_info->log_dir, MAX_SIZE_PATH);

    Lsm_log_enable = !init_info->disable_log;

    SHR_IF_ERR_EXIT(appl_ref_lsm_init_verify(unit, init_info));
    SHR_IF_ERR_EXIT(appl_ref_lsm_vis_enable(unit));

    SHR_IF_ERR_EXIT(appl_ref_lsm_parse_xml_file(unit, init_info->xml_dir));

    SHR_IF_ERR_EXIT(appl_ref_lsm_pkt_start(unit));
    SHR_IF_ERR_EXIT(appl_ref_lsm_init_traps(unit, init_info->port));

    Lsm_initizilazed = TRUE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_void_init(
    int unit)
{
    lsm_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    init_info.port = 0;
    init_info.log_dir = "lsm_log";
    init_info.xml_dir = "";
    init_info.disable_log = TRUE;
    SHR_IF_ERR_EXIT(appl_ref_lsm_init(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifiers_db_table_destroy(unit));
    SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_destroy_all(unit));
    if (Lsm_initizilazed)
    {
        appl_ref_lsm_pkt_stop(unit);
        appl_ref_lsm_deinit_traps(unit);
        appl_ref_lsm_vis_disable(unit);
    }
#ifndef    SAL_THREAD_NAME_PRINT_DISABLE
    SHR_IF_ERR_EXIT(sal_console_thread_name_print_enable(TRUE));
#endif
    Lsm_initizilazed = FALSE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_is_init(
    int unit,
    uint8 *is_initizilazed)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_initizilazed, BCM_E_PARAM, "is_initialized param is null.");

    *is_initizilazed = Lsm_initizilazed;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
