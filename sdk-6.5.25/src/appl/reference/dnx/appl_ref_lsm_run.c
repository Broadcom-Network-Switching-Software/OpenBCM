/** \file appl_ref_lsm_run.c
 *
 * LSM run application procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LSM

 /*
  * Include files.
  * {
  */

#include <appl/diag/shell.h>
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/reference/dnx/appl_ref_lsm.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
/*
 * }
 */

/**
 * \brief
 *   This function sends a packet from CPU with PTCH.qualifier is to 2 in order for visibility to catch packet
 * \param [in] unit - The unit number.
 * \param [in] port - The port to set in ptch.
 * \param [in] *data  - The packet data
 * \param [in]  length   - The packet lentgh
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_pkt_send(
    int unit,
    int port,
    uint8 *data,
    int length)
{

    bcm_pkt_t *pkt = NULL;
    int size = 0;
    int dest_byte = 0;

    uint8 module_header[16] = { 0 };
    uint8 ptch_header[2] = { 0 };
    int module_header_size = 16;
    int ptch_type2_size = 2;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 ptch_ssp = 0;
    int core;
    uint32 flags;

    int port_ndx;

    bcm_port_config_t config;
    int cpu_channel;
    SHR_FUNC_INIT_VARS(unit);
    size = module_header_size + ptch_type2_size + length;
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
    core = mapping_info.core;
    ptch_ssp = mapping_info.pp_port;

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &config));

    BCM_PBMP_ITER(config.cpu, port_ndx)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, port_ndx, &flags, &interface_info, &mapping_info));
        if (mapping_info.core == core)
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_in_channel_get(unit, port_ndx, &cpu_channel));

            break;
        }
    }
    /** PTCH.qualifier is to 2 in order for visibility to catch packet */
    ptch_header[0] = 0xa0;

    ptch_header[1] = ptch_ssp;

    SHR_IF_ERR_EXIT(bcm_pkt_alloc(unit, size, 0, &pkt));

        /** Module Header will be filled in bcm_tx(), just reserve space */
    SHR_IF_ERR_EXIT(bcm_pkt_memcpy(pkt, dest_byte, module_header, sizeof(module_header)));
    dest_byte += module_header_size;

        /** First bit of  PTCH is 0 to indicate that next header is ITMH*/
    SHR_IF_ERR_EXIT(bcm_pkt_memcpy(pkt, dest_byte, ptch_header, sizeof(ptch_header)));
    dest_byte += ptch_type2_size;

    SHR_IF_ERR_EXIT(bcm_pkt_memcpy(pkt, dest_byte, data, length));

    pkt->_dpp_hdr[0] = cpu_channel;
    pkt->blk_count = 1;

    pkt->_pkt_data.len = size;

    /** Flag BCM_TX_CRC_APPEND indicates CRC bytes are appended by switch */
    pkt->flags |= BCM_TX_CRC_APPEND;

    /** Flag BCM_PKT_F_HGHDR indicates Module Header exists */
    pkt->flags |= BCM_PKT_F_HGHDR;

    /** Sent packet out */
    SHR_IF_ERR_EXIT(bcm_tx(unit, pkt, NULL));

exit:
    bcm_pkt_free(unit, pkt);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function collects signal result from the device
 * \param [in] unit - The unit number.
 * \param [in] core - The core number
 * \param [in\out] qualifier_info  - qualifier struct with command information and is also used to return the signal value
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_collect_data_sig(
    int unit,
    int core,
    lsm_qualifier_info_t * qualifier_info)
{
    char *to;
    char *from;
    char *name;
    char *block;
    signal_output_t *signal_out_put = NULL;
    SHR_FUNC_INIT_VARS(unit);
    to = qualifier_info->qualifer_field.qualifier_command.signal_to;
    from = qualifier_info->qualifer_field.qualifier_command.signal_from;
    name = qualifier_info->qualifer_field.qualifier_command.signal_name;
    block = qualifier_info->qualifer_field.qualifier_command.signal_block;
    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, 0, block, from, to, name, &signal_out_put));

    qualifier_info->qualifier_value = signal_out_put->value[0];

exit:
    sand_signal_output_free(signal_out_put);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function collects results from the device
 * \param [in] unit - The unit number.
 * \param [in] core - The core number
 * \param [in\out] entry- struct that contains the commands to run on device and is used to return qualifier value collected from device
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_collect_data(
    int unit,
    int core,
    lsm_qualifiers_entry_t * entry)
{
    int num_of_qualifiers;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "entry");
    num_of_qualifiers = entry->nof_qualifiers;

    for (int i = 0; i < num_of_qualifiers; i++)
    {

        if (entry->list_of_qualifiers[i].qualifer_field.qualifier_type == lsm_signal)
        {
            SHR_IF_ERR_EXIT(appl_ref_lsm_run_collect_data_sig(unit, core, &entry->list_of_qualifiers[i]));
            LOG_CLI((BSL_META_U(unit, " qualifier val %d  \n"), entry->list_of_qualifiers[i].qualifier_value));

        }
        else
        {
            LOG_CLI((BSL_META_U(unit, " qualifier %d - only signal qualifier is supported \n"), i));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function runs diag command on device
 * \param [in] unit - The unit number.
 * \param [in] diags - diag command to run on device
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_action_bcm_diags(
    int unit,
    bcm_diags diags)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_printf
        ("|--------------------------------------------------------------------------------------------------------------|\n");
    LOG_CLI((BSL_META_U(unit, "running command : %s \n"), diags));
    if (sh_process_command(unit, diags) != CMD_OK)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "Error running diag command\n");
    }
    sal_printf
        ("|--------------------------------------------------------------------------------------------------------------|\n");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function runs diag command on device
 * \param [in] unit - The unit number.
 * \param [in] print - string to print
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_action_print_info(
    int unit,
    print_info print)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META_U(unit, "%s \n"), print));
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function runs actions that should run on device
 * \param [in] unit - The unit number.
 * \param [in] action- struct that contains list of actions
 *         prt_info - print action
 *         diags - diag action. executes a bcm diag
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_actions(
    int unit,
    lsm_actions_entry_t * action)
{

    int num_of_actions;
    SHR_FUNC_INIT_VARS(unit);
    num_of_actions = action->nof_actions;
    for (int i = 0; i < num_of_actions; i++)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_run_action_bcm_diags(unit, action->list_of_action_results[i].diags));
    }

    for (int i = 0; i < num_of_actions; i++)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_run_action_print_info(unit, action->list_of_action_results[i].prt_info));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function verifies event info input
 * \param [in] unit - The unit number.
 * \param [in] event- event information
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_verify(
    int unit,
    lsm_event_info_t * info)
{
    uint8 init;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_ref_lsm_is_init(unit, &init));

    if (!init)
    {
        SHR_ERR_EXIT(BCM_E_INIT, "Application lsm is not initialized  \n");
    }
    if (info->event != lsm_packet_event)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "only packet event type is supported \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function runs actions per event type
 *   for packet event - it will resend packet to device
 * \param [in] unit - The unit number.
 * \param [out] info - event information
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_pre_actions(
    int unit,
    lsm_event_info_t * info)
{

    int size;
    uint8 pkt_data[MAX_PKT_HDR_SIZE];
    SHR_FUNC_INIT_VARS(unit);
    size = MAX_PKT_HDR_SIZE;
    if (info->pkt->pkt_data[0].len < size)
    {
        size = info->pkt->pkt_data[0].len;
    }
    for (int i = 0; i < size; i++)
    {
        pkt_data[i] = info->pkt->pkt_data[0].data[i];
    }

    if (info->event == lsm_packet_event)
    {

        SHR_IF_ERR_EXIT(appl_ref_lsm_run_pkt_send(unit, info->pkt->src_port, pkt_data, size));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see appl_ref_lsm.h*/
shr_error_e
appl_ref_lsm_run(
    int unit,
    lsm_event_info_t * info)
{
    lsm_actions_entry_t actions;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_run_verify(unit, info));

    SHR_IF_ERR_EXIT(appl_ref_lsm_run_pre_actions(unit, info));
    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifiers_db_entry_get(unit, 0, &actions.qualifiers));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, info->pkt->src_port, &flags, &interface_info, &mapping_info));
    core = mapping_info.core;
    SHR_IF_ERR_EXIT(appl_ref_lsm_run_collect_data(unit, core, &actions.qualifiers));
    SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_find_match(unit, 0, &actions));
    SHR_IF_ERR_EXIT(appl_ref_lsm_run_actions(unit, &actions));

exit:
    SHR_FUNC_EXIT;

}
