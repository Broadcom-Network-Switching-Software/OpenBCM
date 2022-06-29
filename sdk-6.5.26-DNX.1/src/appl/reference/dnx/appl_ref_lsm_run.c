/** \file appl_ref_lsm_run.c
 *
 * LSM run application procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <appl/diag/system.h>
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/drv.h>
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

#if defined(ADAPTER_SERVER_MODE)
    /** module header is 0 in case of adapter*/
    int module_header_size = 0;
#else
    int module_header_size = 16;
#endif
    int ptch_type2_size = 2;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 ptch_ssp = 0;
    int core;
    uint32 flags;

    int port_ndx;

    bcm_port_config_t config;
    int cpu_channel = 0;
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
    SHR_NULL_CHECK(qualifier_info, _SHR_E_PARAM, "qualifier_info");
    to = qualifier_info->qualifer_field.qualifier_command.sig.signal_to;
    from = qualifier_info->qualifer_field.qualifier_command.sig.signal_from;
    name = qualifier_info->qualifer_field.qualifier_command.sig.signal_name;
    block = qualifier_info->qualifer_field.qualifier_command.sig.signal_block;
#if defined(ADAPTER_SERVER_MODE)
    /** in adapter it takes time for packet to complete pipe. sleeping before collecting signals*/
    sal_sleep(3);
#endif

    SHR_IF_ERR_EXIT(sand_signal_output_find(unit, core, 0, block, from, to, name, &signal_out_put));

    qualifier_info->qualifier_value = signal_out_put->value[0];
exit:
    sand_signal_output_free(signal_out_put);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function collects register field's values from the device,
 *   specifically the reigster and field names.
 *
 * \param [in] unit - The unit number.
 * \param [in\out] qualifier_info  - qualifier struct pointer of a command's information.
 *                  [in] qualifier_info->qualifer_field.qualifier_command.reg.register_name - register name.
 *                  [in] qualifier_info->qualifer_field.qualifier_command.reg.field_name - register's field name.
 *                  [out] qualifier_info->qualifier_value - found value of register's field name.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_collect_data_reg(
    int unit,
    lsm_qualifier_info_t * qualifier_info)
{

    soc_reg_above_64_val_t reg_val;
    soc_reg_above_64_val_t field_val;
    soc_regaddrinfo_t *adress_info;
    int field_index;
    soc_reg_info_t *reg_info;
    soc_regaddrlist_t adress_list = { 0 };
    soc_field_info_t *field_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(field_val);
    SHR_IF_ERR_EXIT_WITH_LOG(soc_regaddrlist_alloc(&adress_list),
                             "Could not allocate address list.  Memory error.\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT_WITH_LOG(parse_symbolic_reference(unit, &adress_list,
                                                      qualifier_info->qualifer_field.qualifier_command.
                                                      reg.register_name), "Syntax error parsing \"%s\"\n%s%s",
                             qualifier_info->qualifer_field.qualifier_command.reg.register_name, EMPTY, EMPTY);

    adress_info = adress_list.ainfo;
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, adress_info->reg, adress_info->port, 0, reg_val));

    reg_info = &SOC_REG_INFO(unit, adress_info->reg);
    for (field_index = reg_info->nFields - 1; field_index >= 0; field_index--)
    {
        field_info = &reg_info->fields[field_index];
        if (sal_strcasecmp
            (SOC_FIELD_NAME(unit, field_info->field), qualifier_info->qualifer_field.qualifier_command.reg.field_name))
        {
            continue;
        }
        soc_reg_above_64_field_get(unit, adress_info->reg, reg_val, field_info->field, field_val);
        break;
    }

    qualifier_info->qualifier_value = field_val[0];

exit:
    soc_regaddrlist_free(&adress_list);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function collects results from the device. This is a wrapper function that will call all child functions
 *   according to encountered qualifier types.
 *
 * \param [in] unit - The unit number.
 * \param [in] info- event information
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
    lsm_event_info_t * info,
    lsm_qualifiers_entry_t * entry)
{
    int num_of_qualifiers;
    int core = 0;

    uint32 qual_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "entry");
    num_of_qualifiers = entry->nof_qualifiers;

    if (info->event == lsm_event_packet)
    {
        core = info->core_id;
    }
    for (qual_index = 0; qual_index < num_of_qualifiers; qual_index++)
    {
        switch (entry->list_of_qualifiers[qual_index].qualifer_field.qualifier_type)
        {
            case lsm_qualifier_signal:
            {
                SHR_IF_ERR_EXIT(appl_ref_lsm_run_collect_data_sig(unit, core, &entry->list_of_qualifiers[qual_index]));
                LOG_CLI((BSL_META_U(unit, " signal qualifier val %d  \n"),
                         entry->list_of_qualifiers[qual_index].qualifier_value));

                break;
            }
            case lsm_qualifier_register:
            {
                SHR_IF_ERR_EXIT(appl_ref_lsm_run_collect_data_reg(unit, &entry->list_of_qualifiers[qual_index]));
                LOG_CLI((BSL_META_U(unit, " register qualifier val %d  \n"),
                         entry->list_of_qualifiers[qual_index].qualifier_value));

                break;
            }
            case lsm_qualifier_interrupt:
            {
                entry->list_of_qualifiers[qual_index].qualifier_value = info->interrupt_id;

                break;
            }
            default:
                SHR_ERR_EXIT(BCM_E_PARAM, "qualifier %d not supported, see lsm_qualifier_e for supported qualifiers.\n",
                             qual_index);
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
    lsm_action_buffer_t diags)
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
    lsm_action_buffer_t print)
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

    int num_of_actions, ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action, _SHR_E_PARAM, "action");
    num_of_actions = action->nof_actions;
    for (ii = 0; ii < num_of_actions; ii++)
    {
        if (action->list_of_action_results[ii].action_type == lsm_action_diag)
        {
            /**
             * running actions of type diag, see function brief for further information.
             */
            SHR_IF_ERR_EXIT(appl_ref_lsm_run_action_bcm_diags(unit, action->list_of_action_results[ii].action));
        }
    }

    for (ii = 0; ii < num_of_actions; ii++)
    {
        if (action->list_of_action_results[ii].action_type == lsm_action_print)
        {
            /**
             * running actions of type print, see function brief for further information.
             */
            SHR_IF_ERR_EXIT(appl_ref_lsm_run_action_print_info(unit, action->list_of_action_results[ii].action));
        }
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
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(appl_ref_lsm_is_init(unit, &init));

    if (!init)
    {
        SHR_ERR_EXIT(BCM_E_INIT, "Application lsm is not initialized  \n");
    }
    if (info->event >= lsm_event_nof || info->event < lsm_event_invalid)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "unsupported LSM event \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function runs actions per event type
 *   for packet event - it will resend packet to device
 * \param [in] unit - The unit number.
 * \param [in] info - event information
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

    int index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    if ((info->event == lsm_event_packet) && (info->no_resend_packet_enable == 0))
    {
        int size;
        uint8 pkt_data[MAX_PKT_HDR_SIZE];
        size = MAX_PKT_HDR_SIZE;
        if (info->pkt->pkt_data[0].len < size)
        {
            size = info->pkt->pkt_data[0].len;
        }
        for (index = 0; index < size; index++)
        {
            pkt_data[index] = info->pkt->pkt_data[0].data[index];
        }
        SHR_IF_ERR_EXIT(appl_ref_lsm_run_pkt_send(unit, info->pkt->src_port, pkt_data, size));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function returns the relavent table ID based on the event information
 *   currently it is hardcoded. table 0 for packet event, 1 for interrupt event
 * \param [in] unit - The unit number.
 * \param [in] info - event information
 * \param [out] table_id - action table_id
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_run_get_table_id(
    int unit,
    lsm_event_info_t * info,
    uint32 *table_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    if (info->event == lsm_event_packet)
    {
        *table_id = LSM_PACKET_EVENT_TABLE_NUM;
    }
    else if (info->event == lsm_event_interrupt)
    {
        *table_id = LSM_INTERRUPT_EVENT_TABLE_NUM;
    }
    else
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "unsupported LSM event \n");
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
    uint32 table_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    SHR_IF_ERR_EXIT(appl_ref_lsm_run_verify(unit, info));
    SHR_IF_ERR_EXIT(appl_ref_lsm_run_get_table_id(unit, info, &table_id));

    SHR_IF_ERR_EXIT(appl_ref_lsm_run_pre_actions(unit, info));
    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifiers_db_entry_get(unit, table_id, &actions.qualifiers));

    SHR_IF_ERR_EXIT(appl_ref_lsm_run_collect_data(unit, info, &actions.qualifiers));
    SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_find_match(unit, table_id, &actions));
    SHR_IF_ERR_EXIT(appl_ref_lsm_run_actions(unit, &actions));

exit:
    SHR_FUNC_EXIT;

}
