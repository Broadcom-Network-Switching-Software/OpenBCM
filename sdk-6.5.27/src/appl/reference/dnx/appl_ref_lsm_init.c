/** \file appl_ref_debug_lsm_run.c
 *
 * lsm application init procedures for DNX
 *
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LSM

 /*
  * Include files.
  * {
  */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <appl/diag/shell.h>
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/utilex/utilex_str.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm/switch.h>
#include <sal/appl/sal.h>
#include <bcm/pkt.h>
#include <bcm/instru.h>
#include <shared/utilex/utilex_str.h>
#include <soc/sand/dbx_pre_compiled_devices.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include "src/bcm/dnx/algo/port/algo_port_internal.h"
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>
#include <appl/reference/dnx/appl_ref_lsm.h>
#include <soc/dnx/intr/dnx_intr.h>
/*
 * }
 */

/** global to hold the init state of the tool */
static uint8 Lsm_initizilazed = FALSE;
/** states if tool should dump prints to log */
static int Lsm_log_enable = FALSE;
/** states if tool should enable rx callback*/
static int Lsm_rx_cb_enable = FALSE;
/** states if tool should enable interrupt callback */
static uint8 Lsm_interrupt_cb_enable = FALSE;
/** lsm tool CPU port that packets are sent to*/
static int Lsm_cpu_port = 0;
/** path of lsm log file*/
static char Lsm_log_path[MAX_SIZE_PATH];
/** global that holds the last time lsm event happened */
static sal_time_t Lsm_last_call = LSM_NO_LAST_EVENT;
/** current vis mode of the tool */
static int Lsm_vis_mode;

/**
 * List of ports and their visibility status
 */
static lsm_init_vis_t Lsm_vis_modes[MAX_NOF_PORTS];

/**
 * mapping of qualifier enum to string of same enum.
 */
static const lsm_signal_qualifier_enum_string_t Lsm_qualifier_enum_string_map[] = {
    {"lsm_qualifier_signal", lsm_qualifier_signal, {"sig_name", "block", "from", "to"}, LSM_SIG_NOF_ATTRS,
     {LSM_SIG_NAME_STRING_SIZE, LSM_SIG_BLOCK_STRING_SIZE, LSM_SIG_FROM_STRING_SIZE, LSM_SIG_TO_STRING_SIZE}},
    {"lsm_qualifier_register", lsm_qualifier_register, {"register_name", "field_name"}, LSM_REG_NOF_ATTRS,
     {LSM_REG_NAME_STRING_SIZE, LSM_REG_FIELD_NAME_STRING_SIZE}},
    {"lsm_qualifier_interrupt", lsm_qualifier_interrupt, {0}, 0, {0}}
};

/**
 * mapping of operator enum to string of same enum. This mapping should make sure that
 * between lsm_operator_first to lsm_operator_nof (not including lsm_operator_first and lsm_operator_nof)
 * Lsm_operator_enum_string_map[lsm_operator_**] == lsm_operator_**
 */
static const lsm_operator_enum_string_t Lsm_operator_enum_string_map[] = {
    {"eq", lsm_operator_equals},
    {"ne", lsm_operator_not_equals},
    {"lt", lsm_operator_lower},
    {"gt", lsm_operator_greater},
    {"le", lsm_operator_lower_equals},
    {"ge", lsm_operator_greater_equals}
};

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
        char log_cmd[LSM_MAX_ACTION_BUFFER];
        char timestamp[SH_SAND_MAX_TIME_SIZE];
        sh_sand_time_get(timestamp);
        sal_strncpy_s(log_cmd, "log file=", LSM_MAX_ACTION_BUFFER);
        sal_strncat_s(log_cmd, Lsm_log_path, LSM_MAX_ACTION_BUFFER);
        sal_strncat_s(log_cmd, "_", LSM_MAX_ACTION_BUFFER);
        sal_strncat_s(log_cmd, timestamp, LSM_MAX_ACTION_BUFFER);
        sal_strncat_s(log_cmd, ".txt", LSM_MAX_ACTION_BUFFER);
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
    SHR_NULL_CHECK(rx_pkt, BCM_E_INTERNAL, "rx packet pointer is null.");
    tot_len = rx_pkt->tot_len;
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
    /** Need this information to get the core ID for port*/
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;

    lsm_event_info_t info = { 0 };
    int offset = 0;
    int rv = BCM_E_NONE;
    bcm_pkt_t *tx_pkt_info = NULL;
    int pkt_len;
    int tot_len;
    int src_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(rx_pkt, BCM_E_INTERNAL, "rx packet pointer is null.");
    pkt_len = rx_pkt->pkt_len;
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
    info.event = lsm_event_packet;
    /** get core id according to SRC port. LSM information collect logic will work on this core */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
    info.core_id = mapping_info.core;
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
appl_ref_lsm_packet_cb(
    int unit,
    bcm_pkt_t * rx_pkt,
    void *cookie)
{
    char timestamp[SH_SAND_MAX_TIME_SIZE];
    sal_time_t current_time;
    if (rx_pkt == NULL)
    {
        LOG_CLI((BSL_META_U(unit, "rx_pkt in null \n")));
        return BCM_RX_NOT_HANDLED;
    }

    /**
     *  check packet is received on lsm CPU port that was set on lsm init function
     *  No Need to print error as this is valid case that packet was recived on other CPU port not related to LSM
     * */
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
        sh_sand_time_get(timestamp);
        LOG_CLI((BSL_META_U(unit, "Packet received on lsm CPU port \n")));
        LOG_CLI((BSL_META_U(unit, "lsm packet event has started now \n")));
        LOG_CLI((BSL_META_U
                 (unit,
                  "|--------------------------------------------------------------------------------------------------------------|\n")));

        LOG_CLI((BSL_META_U(unit, "start time of event: %s  \n"), timestamp));
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

/**
 * \brief
 *   This is the CB function that will be registered on the configured interrupts
 *   in order to trigger an lsm interrupt event.
 * \param [in] unit - The unit number.
 * \param [in] event - event info bcm_switch_event_t
 * \param [in] arg1 -  Interrupt-event, defined per-device
 * \param [in] arg2 -  Event-index block-index
 * \param [in] arg3 -  Flags for future use
 * \param [in] linkdata - General data
 * \return
 *   void
 * \remark
 *   * Function foes not return error in case of failure in event or opening closing log
 * \see
 *   shr_error_e
 */
static void
appl_ref_lsm_cb_interrupt(
    int unit,
    bcm_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *linkdata)
{

    char timestamp[SH_SAND_MAX_TIME_SIZE];
    lsm_event_info_t info = { 0 };
    time_t current_time;
    /**
     * using timestamp to compare time of event and last evnent
     */
    sh_sand_time_get(timestamp);
    /**
     * using current time to have time in readable format
     */
    current_time = sal_time();
    /**
     * check the delay time between LSM events in order not overload CPU
     */
    if (current_time > Lsm_last_call + LSM_TIME_DELAY)
    {
        /**
         * Do not quit if there is error in log so we can still run the event
         */
        if (appl_ref_lsm_cb_set_log(unit, 1) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error trying to enable logger\n")));
        }

        LOG_CLI((BSL_META_U(unit, "lsm interrupt event has started now \n")));
        LOG_CLI((BSL_META_U(unit, " time of event: %s  \n"), timestamp));
        LOG_CLI((BSL_META_U(unit, "event id: %d  \n"), arg1));

        info.event = lsm_event_interrupt;
        info.interrupt_id = arg1;
        /**
         * Do not return so we can close log file and record time
         */
        if (appl_ref_lsm_run(unit, &info) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error in lsm run \n")));
        }

        if (appl_ref_lsm_cb_set_log(unit, 0) != _SHR_E_NONE)
        {
            LOG_CLI((BSL_META_U(unit, "error closing log file\n")));
        }
        Lsm_last_call = sal_time();
    }
}

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
 *   This function verifies an action table in lsm_actions.xml
 * \param [in] unit - The unit number.
 * \param [in] action_table_node - the root element for the action table to be verified.
 * \param [in] table_id - The table ID of the table to be verified.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   appl_ref_lsm_action_table_parse
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_action_table_parse_verify(
    int unit,
    xml_node action_table_node,
    uint32 table_id)
{
    /**
     * an xml node used to represent the current action tag that is being parsed.
     */
    xml_node current_action;
    /**
     * an xml node used to represent the current tag that is being parsed.
     */
    xml_node current_tag;
    /**
     * One of the verifications done is on the number of tag attributes written in the XML.
     * For that, the XML API to get all tag attributes is being used with an LSM_MAX_TAG_ATTR+1
     * value. If there is an element in index LSM_MAX_TAG_ATTR, then need to report an error
     * of too many attribute tags.
     */
    attribute_param_t attrs[LSM_MAX_TAG_ATTR + 1];

    uint32 nof_tags;
    uint32 str_len;
    uint32 index;
    uint32 prio;
    /**
     * buffer used to get the string values from the XML.
     */
    char target[LSM_MAX_SIZE_STR];

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Verify number of actions in table is less then LSM_MAX_NOF_ACTION_ENTRIES
     */
    RHDATA_GET_NODE_NUM(nof_tags, action_table_node, "action");
    SHR_MAX_VERIFY(nof_tags, LSM_MAX_NOF_ACTION_ENTRIES, BCM_E_PARAM, "number of action in table is less then %d.\n",
                   LSM_MAX_NOF_ACTION_ENTRIES);

    RHDATA_ITERATOR(current_action, action_table_node, "action")
    {
        RHDATA_GET_NODE_NUM(nof_tags, current_action, "key");
        SHR_RANGE_VERIFY(nof_tags, 1, LSM_MAX_NOF_QUAL, BCM_E_PARAM, "each action should have %d-%d keys.\n", 1,
                         LSM_MAX_NOF_QUAL);

        RHDATA_GET_NODE_NUM(nof_tags, current_action, "result");
        SHR_RANGE_VERIFY(nof_tags, 1, LSM_MAX_NOF_ACTIONS, BCM_E_PARAM, "each action should have %d-%d results.\n", 1,
                         LSM_MAX_NOF_ACTIONS);

        current_tag = dbx_xml_child_get_first(current_action, "key_prio");
        SHR_IF_ERR_CONT(dbx_xml_property_get_int(current_tag, "priority", (int *) &prio));
        SHR_MAX_VERIFY(prio, LSM_MAX_ACTION_PRIO, BCM_E_PARAM, "max action priority value is %d.\n",
                       LSM_MAX_ACTION_PRIO);

        RHDATA_ITERATOR(current_tag, current_action, "key")
        {
            /**
             * Verify op max length is LSM_OPERATOR_STRING_SIZE
             */
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_str(current_tag, "op", target, LSM_OPERATOR_STRING_SIZE),
                                      BCM_E_NOT_FOUND);

            /**
             * Verify max number of attribute in a key tag is LSM_MAX_TAG_ATTR
             */
            sal_memset(attrs, 0, sizeof(attrs));
            SHR_IF_ERR_EXIT(dbx_xml_property_get_all(current_tag, attrs, LSM_MAX_TAG_ATTR + 1));
            if (attrs[LSM_MAX_TAG_ATTR].name[0] != '\0')
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "max attributes in tag is %d.", LSM_MAX_TAG_ATTR);
            }

            for (index = 0; index < LSM_MAX_TAG_ATTR && (attrs[index].name[0] != '\0'); index++)
            {
                str_len = sal_strnlen(attrs[index].name, LSM_MAX_SIZE_STR);
                SHR_MAX_VERIFY(str_len, LSM_MAX_CHAR_QUAL_NAME, BCM_E_PARAM,
                               "max length of qualifier name is %d, fix attribute '%s'.\n", LSM_MAX_CHAR_QUAL_NAME,
                               attrs[index].name);
            }
        }

        RHDATA_ITERATOR(current_tag, current_action, "result")
        {
            /**
             * Verify max number of attribute in a result tag is LSM_MAX_TAG_ATTR
             */
            sal_memset(attrs, 0, sizeof(attrs));
            SHR_IF_ERR_EXIT(dbx_xml_property_get_all(current_tag, attrs, LSM_MAX_TAG_ATTR + 1));
            if (attrs[LSM_MAX_TAG_ATTR].name[0] != '\0')
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "max attributes in tag is %d.", LSM_MAX_TAG_ATTR);
            }
            /*
             * coverity explanation: the former loop is used to go over an XMLs tag attributes. Before the loop starts
             * the attrs structure is initialized and assigned relevant values. Both loops look the same but are run on 
             * different parts of the XML. Conclusion: the look the same but are running on different values. 
             */
             /* coverity[copy_paste_error:FALSE]  */
            for (index = 0; index < LSM_MAX_TAG_ATTR && (attrs[index].name[0] != '\0'); index++)
            {
                str_len = sal_strnlen(attrs[index].value, LSM_MAX_SIZE_STR);
                SHR_MAX_VERIFY(str_len, LSM_MAX_ACTION_BUFFER, BCM_E_PARAM,
                               "max length of of print/diag command value is %d, fix attribute '%s'.\n",
                               LSM_MAX_ACTION_BUFFER, attrs[index].value);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies the qualifiers entries in lsm_config.xml
 * \param [in] unit - The unit number.
 * \param [in] qualifier_db_node - the root element for the qualifier table to be verified.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   appl_ref_lsm_qualifier_info_parse
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_qualifier_info_parse_verify(
    int unit,
    xml_node qualifier_db_node)
{
    xml_node current_qualifier;
    xml_node current_tag;
    attribute_param_t attrs[LSM_MAX_TAG_ATTR + 1];

    uint32 nof_tags;
    uint32 str_len;
    uint32 index;
    uint32 enum_index;
    uint32 attr_index;
    uint32 duplicate_count;
    char target[LSM_MAX_SIZE_STR];
    char qualifier_name[LSM_MAX_SIZE_STR];
    const lsm_signal_qualifier_enum_string_t *qual_type;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Verify number of qualifiers in table is less than LSM_MAX_NOF_QUAL_ENTRIES
     */
    RHDATA_GET_NODE_NUM(nof_tags, qualifier_db_node, "qualifier");
    SHR_MAX_VERIFY(nof_tags, LSM_MAX_NOF_QUAL_ENTRIES, BCM_E_PARAM, "number of qualifiers in table is less then %d.\n",
                   LSM_MAX_NOF_QUAL_ENTRIES);

    RHDATA_ITERATOR(current_qualifier, qualifier_db_node, "qualifier")
    {
        sal_memset(attrs, 0, sizeof(attrs));
        qual_type = NULL;
        duplicate_count = 0;

        SHR_IF_ERR_EXIT(dbx_xml_property_get_all(current_qualifier, attrs, LSM_MAX_TAG_ATTR + 1));
        if (attrs[LSM_MAX_TAG_ATTR].name[0] != '\0')
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Attributes buffer overflow. Max number of attributes in tag is: %d.",
                         LSM_MAX_TAG_ATTR);
        }

        for (index = 0; index < LSM_MAX_TAG_ATTR && attrs[index].name[0] != '\0'; ++index)
        {
            if (sal_strncmp(attrs[index].name, "name", LSM_MAX_SIZE_STR) == 0)
            {
                str_len = sal_strnlen(attrs[index].name, LSM_MAX_SIZE_STR);
                SHR_MAX_VERIFY(str_len, LSM_MAX_CHAR_QUAL_NAME, BCM_E_PARAM,
                               "max length of qualifier name is %d, fix attribute '%s'.\n", LSM_MAX_CHAR_QUAL_NAME,
                               attrs[index].name);
                continue;
            }

            /**
             * All qualifier tags are expected to have name & type attributes and upon parsing type
             * further parsing is needed according to it.
             *
             * If type is not set yet, no  advance parsing is possible (only name/type). Once type is set
             * we will not allow to check for another type attribute.
             */
            if (!qual_type)
            {
                if (sal_strncmp(attrs[index].name, "type", LSM_MAX_SIZE_STR) == 0)
                {
                    for (enum_index = 0; enum_index < lsm_qualifier_nof; enum_index++)
                    {
                        if (sal_strncmp
                            (attrs[index].value, Lsm_qualifier_enum_string_map[enum_index].lsm_qual_string,
                             sizeof(attrs[index].value)) == 0)
                        {
                            qual_type = &Lsm_qualifier_enum_string_map[enum_index];
                            break;
                        }
                    }
                    if (!qual_type)
                    {
                        SHR_ERR_EXIT(BCM_E_PARAM, "unfamiliar qualifier type -- %s.\n", attrs[index].value);
                    }
                }
                else
                {
                    SHR_ERR_EXIT(BCM_E_PARAM,
                                 "qualifier type not set yet, need to set before assigning other attributes -- %s.\n",
                                 attrs[index].name);
                }

                /**
                 *
                 */
                if (qual_type->nof_attrs && attrs[qual_type->nof_attrs - 1].name[0] == '\0')
                {
                    SHR_ERR_EXIT(BCM_E_PARAM, "qulifier type %s has %d attributes, not all were configured.",
                                 qual_type->lsm_qual_string, (qual_type->nof_attrs));
                }
            }
            else
            {
                /**
                 * Verify qulifier has all required attributes and their value sizes do not exceed limits.
                 */
                for (attr_index = 0; attr_index < qual_type->nof_attrs; attr_index++)
                {
                    /**
                     * Verify attribute size limit.
                     */
                    if (sal_strncmp(attrs[index].name, qual_type->attrributes[attr_index], LSM_MAX_SIZE_STR) == 0)
                    {
                        str_len = sal_strnlen(attrs[index].name, LSM_MAX_SIZE_STR);
                        SHR_MAX_VERIFY(str_len, qual_type->attr_value_limit[attr_index], BCM_E_PARAM,
                                       "max length of '%s is %d.\n", qual_type->attrributes[attr_index],
                                       qual_type->attr_value_limit[attr_index]);
                        break;
                    }

                    /**
                     * On match, loop should break.
                     * If on last loop iteration still not mached, return error.
                     */
                    if (attr_index == qual_type->nof_attrs - 1)
                    {
                        SHR_ERR_EXIT(BCM_E_PARAM, "qualifier type %s does not have attribute %s.\n",
                                     qual_type->lsm_qual_string, attrs[index].name);
                    }
                }

            }
        }

        if (qual_type)
        {
            for (index = 0; index < qual_type->nof_attrs; ++index)
            {
                for (attr_index = 0; attr_index < LSM_MAX_TAG_ATTR && attrs[attr_index].name[0] != '\0'; ++attr_index)
                {
                    if (sal_strncmp(attrs[attr_index].name, qual_type->attrributes[index], LSM_MAX_SIZE_STR) == 0)
                    {
                        break;
                    }

                    /**
                     * On match, loop should break.
                     * If on last loop iteration still not mached, return error.
                     */
                    if (attr_index == LSM_MAX_TAG_ATTR - 1 || attrs[attr_index + 1].name[0] == '\0')
                    {
                        SHR_ERR_EXIT(BCM_E_PARAM, "qualifier type %s is missing expected attribute %s.\n",
                                     qual_type->lsm_qual_string, qual_type->attrributes[index]);
                    }
                }
            }
        }

        /**
         * verify no duplicate qualifier names.
         */
        dbx_xml_property_get_str(current_qualifier, "name", qualifier_name, LSM_MAX_SIZE_STR);
        RHDATA_ITERATOR(current_tag, qualifier_db_node, "qualifier")
        {
            dbx_xml_property_get_str(current_tag, "name", target, LSM_MAX_SIZE_STR);
            if (sal_strncmp(qualifier_name, target, LSM_MAX_SIZE_STR) == 0 && ++duplicate_count > 1)
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "duplicate qualifier name -- %s.\n", qualifier_name);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_init_operator_enum_string_get(
    int unit,
    lsm_operator_e op_enum,
    char *op_string)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(op_string, BCM_E_PARAM, "op_string");

    if (op_enum < lsm_operator_first || op_enum >= lsm_operator_nof)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "operator enum %d should be in the range of: %d -- %d.\n", op_enum,
                     lsm_operator_first, lsm_operator_nof);
    }

    sal_strncpy_s(op_string, Lsm_operator_enum_string_map[op_enum].string, LSM_OPERATOR_STRING_SIZE);
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
 *   This function resets list of interrupts to init values and unregister callback
 * \param [in] unit - The unit number.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_interrupt_disable(
    int unit)
{
    bcm_switch_event_control_t event;
    int index;
    uint32 nof_interrupts;
    uint32 value;
    int interrupt_event_id = 0;

    lsm_interrupt_t lsm_interrupts[LSM_MAX_NOF_INTERRUPTS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_interrupt_table_get(unit, &nof_interrupts, lsm_interrupts));
    /**
     * loop over list of interrupts
     */
    for (index = 0; index < nof_interrupts; index++)
    {
        /**
         * set the original(prior to LSM init) interrupt status and save to DB
         */
        event.index = 0;
        interrupt_event_id = lsm_interrupts[index].interrupt_id;
        event.event_id = interrupt_event_id;
        event.action = bcmSwitchEventCorrActOverride;
        value = lsm_interrupts[index].interrupt_corract_override_status;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, value));
        event.action = bcmSwitchEventMask;
        value = lsm_interrupts[index].interrupt_masks_status;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, value));
    }

    SHR_IF_ERR_EXIT(bcm_switch_event_unregister(unit, &appl_ref_lsm_cb_interrupt, NULL));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function sets list of interrupts to  triger callback
 * \param [in] unit - The unit number.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_interrupt_enable(
    int unit)
{
    bcm_switch_event_control_t event;
    int index;
    uint32 nof_interrupts;
    uint32 value;
    int interrupt_event_id = 0;

    lsm_interrupt_t lsm_interrupts[LSM_MAX_NOF_INTERRUPTS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_interrupt_table_get(unit, &nof_interrupts, lsm_interrupts));
    /**
        * loop over list of interrupts
     */
    for (index = 0; index < nof_interrupts; index++)
    {
        event.event_id = interrupt_event_id;
        event.index = 0;
        interrupt_event_id = lsm_interrupts[index].interrupt_id;
        /**
         * get the original(prior to LSM init) interrupt status and save to DB
         */

        event.action = bcmSwitchEventCorrActOverride;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, &value));
        lsm_interrupts[index].interrupt_corract_override_status = value;
        event.action = bcmSwitchEventMask;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, &value));
        lsm_interrupts[index].interrupt_masks_status = value;
        /**
         * set the interrupt status to set user callback that will be called immediately after the BCM callback.
         */
        event.action = bcmSwitchEventCorrActOverride;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1));
        /**
         *  unmask the event - which means to enable event
         */
        event.action = bcmSwitchEventMask;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0));
        /**
         *  clear the interrupt
         */
        event.action = bcmSwitchEventClear;
        SHR_IF_ERR_EXIT(bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1));

    }

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_interrupt_table_update(unit, nof_interrupts, lsm_interrupts));
    SHR_IF_ERR_EXIT(bcm_switch_event_register(unit, &appl_ref_lsm_cb_interrupt, NULL));

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
    uint32 nof_traps;
    int trap_id;
    int trap_index;
    bcm_rx_trap_config_t trap_config;
    lsm_init_trap_t lsm_traps[LSM_MAX_NOF_TRAPS];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_trap_table_get(unit, &nof_traps, lsm_traps));

    for (trap_index = 0; trap_index < nof_traps; trap_index++)
    {

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, lsm_traps[trap_index].trap_type, &trap_id));
        if (lsm_traps[trap_index].is_new == TRUE)
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
        }
        else
        {
            bcm_rx_trap_config_t_init(&trap_config);
            SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

            trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
            trap_config.dest_port = lsm_traps[trap_index].port;
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
 * \param [in] port - The port number to trap all lsm events to.
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
    uint32 nof_traps;
    int index;
    lsm_init_trap_t lsm_traps[LSM_MAX_NOF_TRAPS];
    int converted_trap_id = 0;
    uint8 is_trap_allocated;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_trap_table_get(unit, &nof_traps, lsm_traps));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = port;
    trap_config.trap_strength = LSM_TRAP_STRENTGH;
    trap_create_flags = 0;
    /** Creating trapes in list */
    for (index = 0; index < nof_traps; index++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, lsm_traps[index].trap_type, &converted_trap_id));
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, converted_trap_id, &is_trap_allocated));
        /** checking if traps are already allocated or not in order to either update the traps or create and configure*/
        if (is_trap_allocated == TRUE)
        {
            bcm_rx_trap_config_t_init(&trap_config);
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, trap_create_flags, lsm_traps[index].trap_type, &trap_id));
            SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config));

            lsm_traps[index].is_new = FALSE;
            lsm_traps[index].port = trap_config.dest_port;

            trap_config.flags |= BCM_RX_TRAP_UPDATE_DEST;
            trap_config.dest_port = port;
        }
        else
        {
            lsm_traps[index].is_new = TRUE;
            SHR_IF_ERR_CONT(bcm_rx_trap_type_create(unit, trap_create_flags, lsm_traps[index].trap_type, &trap_id));
        }

        SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));
    }

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_trap_table_update(unit, nof_traps, lsm_traps));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   function starts RX thread with default configuration and registers callback
 * \param [in] unit - The unit number.
 * \return
 *  void
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_pkt_start(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    /** callback is needed only if the LSM port is CPU interface.*/

    if (!bcm_rx_active(unit))
    {
       /** Initialize RX as default */
        SHR_IF_ERR_EXIT(bcm_rx_cfg_init(unit));

        /** Start RX with default configuration */
        SHR_IF_ERR_EXIT(bcm_rx_start(unit, NULL));
    }
    SHR_IF_ERR_EXIT(bcm_rx_register(unit, "lsm-RX", appl_ref_lsm_packet_cb, BCM_RX_PRIO_MAX, NULL, BCM_RCO_F_ALL_COS));

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief
 *   function stops callback
 * \param [in] unit - The unit number.
 * \return
 *  void
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_pkt_stop(
    int unit)
{
    return bcm_rx_unregister(unit, appl_ref_lsm_packet_cb, BCM_RX_PRIO_MAX);
}

/**
 * \brief
 *   This function verifies init info input
 * \param [in] unit - The unit number.
 * \param [in] init_info- init info
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_ref_lsm_init_verify(
    int unit,
    lsm_init_info_t * init_info)
{
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;
    SHR_FUNC_INIT_VARS(unit);

    /** The key is the header type of outgoing port */
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, init_info->port, key, &value));
    /** lsm tool can only work  if header type is CPU, because packet will be received with FTMH  */
    if (value.value != BCM_SWITCH_PORT_HEADER_TYPE_CPU)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Port %d is not configured as PORT_HEADER_TYPE_CPU  \n", init_info->port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is used to parse the interrupts part of the config.xml into an table
 *   by using the appl_ref_lsm_db_interrupt_table_create API adds its info the lsm tool's
 *   database.
 *
 * \param [in] unit - The unit number.
 * \param [in] interrupt_db_node - the XML node currently being parsed.
 *
 * \return
 *  shr_error_e
 */
static shr_error_e
appl_ref_lsm_interrupt_info_parse(
    int unit,
    xml_node interrupt_db_node)
{
    xml_node current = NULL;
    uint32 interrupt_count = 0;
    attribute_param_t attrs[LSM_MAX_TAG_ATTR];
    int attr_index;
    lsm_interrupt_t interrupts[LSM_MAX_NOF_INTERRUPTS];

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(interrupts, 0, sizeof(interrupts));
    /**
     * loop over all child elements in interrupt tag
     * current is a child tag of interrupt_db_node
     * interrupt_db_node is head where we start the iterator on
     */
    RHDATA_ITERATOR(current, interrupt_db_node, "interrupt")
    {
        sal_memset(attrs, 0, sizeof(attrs));
        dbx_xml_property_get_all(current, attrs, LSM_MAX_TAG_ATTR);
        /**
         * loop over all attributes of current tag and act (parse) according to attribute's name.
         */
        for (attr_index = 0; (attr_index < LSM_MAX_TAG_ATTR) && (attrs[attr_index].name[0] != '\0'); ++attr_index)
        {
            sal_strncpy_s(interrupts[interrupt_count].name, attrs[attr_index].value, LSM_MAX_SIZE_STR);
            interrupt_count++;
        }
    }

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_interrupt_table_create(unit, interrupt_count, interrupts));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is used to parse the part of the lsm_config.xml into an lsm_qualifiers_fields_t
 *   object and using the appl_ref_lsm_db_qualifiers_info_add API adds it info the lsm tool's
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
    /** index for looping over qualifier */
    uint32 qual_index = 0;
    /** index for looping over qualifier enums */
    uint32 qual_enum_index;
    /** index for looping over xml tag's attributes */
    uint32 attr_index;
    xml_node current = NULL;
    attribute_param_t attrs[LSM_MAX_TAG_ATTR];
    lsm_qualifiers_fields_t qulifier_fields[LSM_MAX_NOF_QUAL_TYPES];

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_qualifier_info_parse_verify(unit, qualifier_db_node));

    sal_memset(qulifier_fields, 0, sizeof(qulifier_fields));
    /**
     * Loop over all child elements in qualifier tag.
     * All qualifier tag will parse qualifier name and type and according to the type
     * further parsing will be needed.
     * For example a qualifier of type "lsm_qualifier_signal" will need parse all fileds that are part
     * of lsm_signal_qualifier_command_t structure.
     */
    RHDATA_ITERATOR(current, qualifier_db_node, "qualifier")
    {
        sal_memset(attrs, 0, sizeof(attrs));
        dbx_xml_property_get_all(current, attrs, LSM_MAX_TAG_ATTR);
        /**
         * loop over all attributes of current tag and act (parse) according to attribute's name.
         */
        for (attr_index = 0; attr_index < LSM_MAX_TAG_ATTR && attrs[attr_index].name[0] != '\0'; ++attr_index)
        {
            if (sal_strncmp(attrs[attr_index].name, "name", LSM_MAX_SIZE_STR) == 0)
            {
                sal_strncpy_s(qulifier_fields[qual_index].name, attrs[attr_index].value, LSM_MAX_SIZE_STR);
                continue;
            }

            if (sal_strncmp(attrs[attr_index].name, "type", LSM_MAX_SIZE_STR) == 0)
            {
                for (qual_enum_index = 0; qual_enum_index < lsm_qualifier_nof; qual_enum_index++)
                {
                    if (sal_strncmp
                        (attrs[attr_index].value, Lsm_qualifier_enum_string_map[qual_enum_index].lsm_qual_string,
                         sizeof(attrs[attr_index].value)) == 0)
                    {
                        qulifier_fields[qual_index].qualifier_type =
                            Lsm_qualifier_enum_string_map[qual_enum_index].lsm_qual_enum;
                        break;
                    }
                }
            }
            else
            {
                switch (qulifier_fields[qual_index].qualifier_type)
                {
                    case lsm_qualifier_signal:
                    {
                        if (sal_strncmp(attrs[attr_index].name, "sig_name", LSM_SIG_NAME_STRING_SIZE) == 0)
                        {
                            sal_strncpy_s(qulifier_fields[qual_index].qualifier_command.sig.signal_name,
                                          attrs[attr_index].value, LSM_SIG_NAME_STRING_SIZE);
                        }
                        else if (sal_strncmp(attrs[attr_index].name, "block", LSM_SIG_BLOCK_STRING_SIZE) == 0)
                        {
                            sal_strncpy_s(qulifier_fields[qual_index].qualifier_command.sig.signal_block,
                                          attrs[attr_index].value, LSM_SIG_BLOCK_STRING_SIZE);
                        }
                        else if (sal_strncmp(attrs[attr_index].name, "to", LSM_SIG_TO_STRING_SIZE) == 0)
                        {
                            sal_strncpy_s(qulifier_fields[qual_index].qualifier_command.sig.signal_to,
                                          attrs[attr_index].value, LSM_SIG_TO_STRING_SIZE);
                        }
                        else if (sal_strncmp(attrs[attr_index].name, "from", LSM_SIG_FROM_STRING_SIZE) == 0)
                        {
                            sal_strncpy_s(qulifier_fields[qual_index].qualifier_command.sig.signal_from,
                                          attrs[attr_index].value, LSM_SIG_FROM_STRING_SIZE);
                        }
                        break;
                    }
                    case lsm_qualifier_register:
                    {
                        if (sal_strncmp(attrs[attr_index].name, "register_name", LSM_REG_NAME_STRING_SIZE) == 0)
                        {
                            sal_strncpy_s(qulifier_fields[qual_index].qualifier_command.reg.register_name,
                                          attrs[attr_index].value, LSM_REG_NAME_STRING_SIZE);
                        }
                        else if (sal_strncmp(attrs[attr_index].name, "field_name", LSM_REG_FIELD_NAME_STRING_SIZE) == 0)
                        {
                            sal_strncpy_s(qulifier_fields[qual_index].qualifier_command.reg.field_name,
                                          attrs[attr_index].value, LSM_REG_FIELD_NAME_STRING_SIZE);
                        }
                        break;
                    }
                    case lsm_qualifier_interrupt:
                    {
                        LOG_DEBUG(BSL_LS_BCMDNX_LSM,
                                  (BSL_META_U(unit, " for qualifier_type interrupt, no need to copy command \n")));
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }
        /**
         * used to keep count of the number of qualifier elements found.
         */
        qual_index++;
    }
    SHR_IF_ERR_EXIT(appl_ref_lsm_db_qualifiers_info_add(unit, qual_index, qulifier_fields));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is used to parse the traps part of the config.xml into an
 *   and using the appl_ref_lsm_db_qualifiers_info_add API adds it info the lsm tool's
 *   database.
 *
 * \param [in] unit - The unit number.
 * \param [in] trap_db_node - the XML node currently being parsed.
 *
 *  \remarks
 *  bcmRxTrapCount is last enum in bcm_rx_trap_t
 * \return
 *  shr_error_e
 */
static shr_error_e
appl_ref_lsm_trap_info_parse(
    int unit,
    xml_node trap_db_node)
{
    xml_node current = NULL;
    uint32 trap_count = 0;
    uint32 start_of_string_to_compare = 9;
    bcm_rx_trap_t trap_find_index;
    dnx_rx_trap_map_type_t trap_type_info;
    attribute_param_t attrs[LSM_MAX_TAG_ATTR];
    lsm_init_trap_t traps[LSM_MAX_NOF_TRAPS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(trap_db_node, BCM_E_PARAM, "trap_db_node");

    /**
     * loop over all child elements in trap tag
     */
    RHDATA_ITERATOR(current, trap_db_node, "trap")
    {
        sal_memset(attrs, 0, sizeof(attrs));
        dbx_xml_property_get_all(current, attrs, LSM_MAX_TAG_ATTR);
        /**
         * loop over all attributes of current tag and act (parse) according to attribute's name.
         */
        for (int attr_index = 0; (attr_index < LSM_MAX_TAG_ATTR) && (attrs[attr_index].name[0] != '\0'); ++attr_index)
        {
            if (sal_strncmp(attrs[attr_index].name, "name", LSM_MAX_SIZE_STR) == 0)
            {
                for (trap_find_index = 0; trap_find_index < bcmRxTrapCount; ++trap_find_index)
                {
                    SHR_IF_ERR_EXIT(dnx_rx_trap_type_map_info_get(unit, trap_find_index, &trap_type_info));
                    /**
                     * expected value in xml starts with bcmRxTrap which we ignore by starting compare after char 9
                     */
                    if (sal_strncmp
                        ((char *) &(attrs[attr_index].value[start_of_string_to_compare]), trap_type_info.trap_name,
                         LSM_MAX_SIZE_STR) == 0)
                    {
                        break;
                    }
                }

                if (trap_find_index == bcmRxTrapCount)
                {
                    SHR_ERR_EXIT(BCM_E_PARAM, "unrecognized trap name %s\n",
                                 (char *) &(attrs[attr_index].value[start_of_string_to_compare]));
                }

                traps[trap_count].trap_type = trap_find_index;
                trap_count++;
            }
            else
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "unrecognized trap attribute %s.\n", attrs[attr_index].name);
            }
        }
    }

    SHR_IF_ERR_EXIT(appl_ref_lsm_db_trap_table_create(unit, trap_count, traps));

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
    uint32 op_enum_index;
    uint32 nof_action_entries;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(appl_ref_lsm_action_table_parse_verify(unit, action_node, table_id));

    RHDATA_GET_NODE_NUM(nof_action_entries, action_node, "action")
        SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_create(unit, table_id, nof_action_entries));
    /**
     * loop over all child elements in action tag
     */
    RHDATA_ITERATOR(current_action, action_node, "action")
    {
        /** init entry values */
        sal_memset(&entry, 0, sizeof(lsm_actions_entry_t));
        for (index = 0; index < LSM_MAX_NOF_QUAL; ++index)
        {
            entry.qualifiers.list_of_qualifiers[index].qualifier_mask = -1;
            entry.qualifiers.list_of_qualifiers[index].lookup_operator = lsm_operator_equals;
        }

        current_key = dbx_xml_child_get_first(current_action, "key_prio");
        SHR_IF_ERR_CONT(dbx_xml_property_get_int(current_key, "priority", (int *) &entry.priority));

        /**
         * each key should hold a name/value pair and a corresponding mask/value pair.
         * loop over all the action's child tags of type key and store the relevant data for later usage.
         */
        RHDATA_ITERATOR(current_key, current_action, "key")
        {
            sal_memset(&attrs, 0, sizeof(attribute_param_t) * RHKEYWORD_MAX_SIZE);
            /**
             * loop over all attributes of current tag and act (parse) according to attribute's name.
             */
            SHR_IF_ERR_EXIT(dbx_xml_property_get_all(current_key, attrs, LSM_MAX_TAG_ATTR));
            for (index = 0; (index < LSM_MAX_TAG_ATTR) && (attrs[index].name[0] != '\0'); ++index)
            {
                if (sal_strncmp(attrs[index].name, "mask", LSM_MAX_SIZE_STR) == 0)
                {
                    entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].qualifier_mask =
                        sal_strtol(attrs[index].value, NULL, 16);
                }
                else if (sal_strncmp(attrs[index].name, "op", LSM_OPERATOR_STRING_SIZE) == 0)
                {
                    /**
                     * op attribute of key tag is set to be lsm_operator_equals for all key by defualt, in case of user
                     * adding an op attribute to a key tag it is compared againt supported op values and sets new value
                     * only if supported.
                     */
                    for (op_enum_index = 0; op_enum_index < lsm_operator_nof; op_enum_index++)
                    {
                        if (sal_strncmp
                            (attrs[index].value, Lsm_operator_enum_string_map[op_enum_index].string,
                             LSM_OPERATOR_STRING_SIZE) == 0)
                        {
                            entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].lookup_operator =
                                Lsm_operator_enum_string_map[op_enum_index].lsm_enum;
                            break;
                        }
                    }
                }
                else
                {
                    sal_strncpy_s(entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].
                                  qualifer_field.name, attrs[index].name,
                                  sizeof(entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].
                                         qualifer_field.name));
                    /**
                     * attrs[index].value currently can be either a string signifying a dbal name, a decimal value or a hexadecimal value.
                     * note that all option are currntly in char* format.
                     *
                     * handle dbal format values
                     */
                    if (!isdigit((unsigned) attrs[index].value[0]))
                    {
                        int value;
                        if (sal_strcmp((attrs[index].name), "interrupt") == 0)
                        {
                            value = soc_dnx_int_name_to_id(unit, attrs[index].value);
                            entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].qualifier_value =
                                value;
                        }
                        else
                        {

                            SHR_IF_ERR_EXIT(dnx_dbal_fields_enum_value_get
                                            (unit, attrs[index].name, attrs[index].value, &value));
                            entry.qualifiers.list_of_qualifiers[entry.qualifiers.nof_qualifiers].qualifier_value =
                                value;
                        }
                    }
                    /**
                     * handle hex or decimal values.
                     */
                    else if (sal_strlen(attrs[index].value) > 2 && attrs[index].value[1] == 'x')
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
            if (sal_strncmp(single_attr.name, "diag_commands", LSM_MAX_SIZE_STR) == 0)
            {
                entry.list_of_action_results[entry.nof_actions].action_type = lsm_action_diag;
            }
            else if (sal_strncmp(single_attr.name, "print", LSM_MAX_SIZE_STR) == 0)
            {
                entry.list_of_action_results[entry.nof_actions].action_type = lsm_action_print;
            }
            else
            {
                LOG_WARN(BSL_LS_BCMDNX_LSM, (BSL_META_U(unit, "Unrecognized result type %s.\n"), single_attr.name));
                continue;
            }

            sal_strncpy_s(entry.list_of_action_results[entry.nof_actions].action, single_attr.value,
                          LSM_MAX_ACTION_BUFFER);
            entry.nof_actions++;
        }
        SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_entry_add(unit, table_id, &entry));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function is used to parse the XML
 *
 * \param [in] unit - The unit number.
 * \param [in] xml_path - The XML folder path.
 *
 * \return
 *  shr_error_e
 */
static shr_error_e
appl_ref_lsm_parse_xml_file(
    int unit,
    char *xml_path)
{
    xml_node qual_top = NULL;
    xml_node config_top = NULL;
    xml_node trap_top = NULL;
    xml_node action_top = NULL;
    xml_node current = NULL;
    xml_node interrupt_top = NULL;

    uint32 action_table_id;
    uint32 nof_action_tables;
    uint32 nof_tags;
    uint32 nof_qualifiers;
    char action_file_path[RHNAME_MAX_SIZE] = { 0 };
    char config_file_path[RHNAME_MAX_SIZE] = { 0 };
    char action_pre_comp[RHNAME_MAX_SIZE] = { 0 };
    char config_pre_comp[RHNAME_MAX_SIZE] = { 0 };
    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbx_file_get_db(unit, db_name, base_db_name));
    /**
     * If a specific XML path given through the diag command use that path to load xmls, otherwise
     * look for xml file under "tools/sand/db/<chip>/internal/lsm/" and if no xmls there either
     * load precompiled version. tools/sand/db/<chip> is taken from db_name - output of dbx_file_get_db
     */
    if (xml_path[0] != '\0')
    {
        sal_snprintf(action_file_path, RHFILE_MAX_SIZE - 1, "%s/%s", xml_path, LSM_ACTIONS_XML_PATH);
        sal_snprintf(config_file_path, RHFILE_MAX_SIZE - 1, "%s/%s", xml_path, LSM_CONFIG_XML_PATH);
    }
    else
    {
        sal_snprintf(action_file_path, RHFILE_MAX_SIZE - 1, "%s/lsm/%s", db_name, LSM_ACTIONS_XML_PATH);
        sal_snprintf(config_file_path, RHFILE_MAX_SIZE - 1, "%s/lsm/%s", db_name, LSM_CONFIG_XML_PATH);
    }

    sal_snprintf(action_pre_comp, RHFILE_MAX_SIZE - 1, "lsm/%s", LSM_ACTIONS_XML_PATH);
    sal_snprintf(config_pre_comp, RHFILE_MAX_SIZE - 1, "lsm/%s", LSM_CONFIG_XML_PATH);

    config_top = (xml_node) dbx_file_get_xml_top(unit, config_file_path, "config", CONF_OPEN_NO_ERROR_REPORT);
    if (config_top == NULL)
    {
        config_top = (xml_node) dbx_pre_compiled_devices_top_get(unit, config_pre_comp, "config", CONF_OPEN_PER_DEVICE);
    }

    action_top = (xml_node) dbx_file_get_xml_top(unit, action_file_path, "actions_db", CONF_OPEN_NO_ERROR_REPORT);
    if (action_top == NULL)
    {
        action_top =
            (xml_node) dbx_pre_compiled_devices_top_get(unit, action_pre_comp, "actions_db", CONF_OPEN_PER_DEVICE);
    }

    if (config_top == NULL || action_top == NULL)
    {
        SHR_ERR_EXIT(BCM_E_INTERNAL, "missing actions/config xml files.\n");
    }

    qual_top = (xml_node) dbx_xml_child_get_first(config_top, "qualifiers");
    trap_top = (xml_node) dbx_xml_child_get_first(config_top, "traps");
    interrupt_top = (xml_node) dbx_xml_child_get_first(config_top, "interrupts");

    /**
     * Verify number of tag elements in table are less then maximum supported
     */
    RHDATA_GET_NODE_NUM(nof_tags, trap_top, "trap");
    SHR_MAX_VERIFY(nof_tags, LSM_MAX_NOF_TRAPS, BCM_E_PARAM, "number of traps in table is less then %d.\n",
                   LSM_MAX_NOF_TRAPS);
    RHDATA_GET_NODE_NUM(nof_tags, interrupt_top, "interrupt");
    SHR_MAX_VERIFY(nof_tags, LSM_MAX_NOF_INTERRUPTS, BCM_E_PARAM, "number of interrupts in table is less then %d.\n",
                   LSM_MAX_NOF_INTERRUPTS);

    RHDATA_GET_NODE_NUM(nof_qualifiers, qual_top, "qualifier");
    SHR_IF_ERR_EXIT(appl_ref_lsm_db_qualifiers_table_create(unit, nof_qualifiers));
    SHR_IF_ERR_EXIT(appl_ref_lsm_qualifier_info_parse(unit, qual_top));
    SHR_IF_ERR_EXIT(appl_ref_lsm_trap_info_parse(unit, trap_top));
    SHR_IF_ERR_EXIT(appl_ref_lsm_interrupt_info_parse(unit, interrupt_top));

    RHDATA_GET_NODE_NUM(nof_action_tables, action_top, "action_table");
    SHR_MAX_VERIFY(nof_action_tables, LSM_MAX_NOF_ACTION_TABLES, BCM_E_PARAM,
                   "mac number of action tables supported id %d, found %d.", LSM_MAX_NOF_ACTION_TABLES,
                   nof_action_tables);
    SHR_IF_ERR_EXIT(appl_ref_lsm_action_tables_create(unit, nof_action_tables));

    RHDATA_ITERATOR(current, action_top, "action_table")
    {
        dbx_xml_property_get_int(current, "id", (int *) &action_table_id);
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
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;
    /**
     * The following varibles are used to get the delta of allocated and freed bytes counters
     * by substracting the *_start from *_end value.
     * e.g.
     *       alloc_bytes_count_end - alloc_bytes_count_start = the allocated size from start to end.
     */
    unsigned long alloc_bytes_count_start, free_bytes_count_start, alloc_bytes_count_end, free_bytes_count_end;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_ref_lsm_deinit(unit));

    sal_get_alloc_counters(&alloc_bytes_count_start, &free_bytes_count_start);
    sal_memset(&Lsm_vis_modes, 0, sizeof(lsm_init_vis_t) * MAX_NOF_PORTS);

    /**
     * sal_console_thread_name_print_enable is defined under the SAL_THREAD_NAME_PRINT_DISABLE MACRO.
     * sal_console_thread_name_print_enable is used to disable thread name prefix in console prints.
     *
     * This enables LSM to generate a clearer print as it may use other diagnostics.
     */
#ifndef SAL_THREAD_NAME_PRINT_DISABLE
    SHR_IF_ERR_EXIT(sal_console_thread_name_print_enable(FALSE));
#endif

    Lsm_cpu_port = init_info->port;
    sal_strncpy_s(Lsm_log_path, init_info->log_dir, MAX_SIZE_PATH);

    Lsm_log_enable = !(init_info->disable_log);
    Lsm_rx_cb_enable = !(init_info->disable_rx_cb);
    Lsm_interrupt_cb_enable = !(init_info->disable_interrupt_callback);
    SHR_IF_ERR_EXIT(appl_ref_lsm_init_verify(unit, init_info));
    SHR_IF_ERR_EXIT(appl_ref_lsm_vis_enable(unit));

    SHR_IF_ERR_EXIT(appl_ref_lsm_parse_xml_file(unit, init_info->xml_dir));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, Lsm_cpu_port, &flags, &port_interface_info, &port_mapping_info));
    /** In case port is not CPU port. no need for RX callback to triger LSM*/
    if ((port_interface_info.interface == BCM_PORT_IF_CPU) && (Lsm_rx_cb_enable == TRUE))
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_pkt_start(unit));
    }

    SHR_IF_ERR_EXIT(appl_ref_lsm_init_traps(unit, init_info->port));
    if (Lsm_interrupt_cb_enable == TRUE)
    {
        SHR_IF_ERR_EXIT(appl_ref_lsm_interrupt_enable(unit));
    }

    Lsm_initizilazed = TRUE;

    sal_get_alloc_counters(&alloc_bytes_count_end, &free_bytes_count_end);
    LOG_VERBOSE(BSL_LS_BCMDNX_LSM,
                (BSL_META_U(unit, "LSM: Init mem usage:\n\ralloc %lu\n\rfree %lu\n\r"),
                 alloc_bytes_count_end - alloc_bytes_count_start, free_bytes_count_end - free_bytes_count_start));

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
    init_info.disable_rx_cb = TRUE;
    init_info.disable_interrupt_callback = TRUE;
    SHR_IF_ERR_EXIT(appl_ref_lsm_init(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_ref_lsm_deinit(
    int unit)
{
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;
    /**
     * The following varibles are used to get the delta of allocated and freed bytes counters
     * by substracting the *_start from *_end value.
     * e.g.
     *       alloc_bytes_count_end - alloc_bytes_count_start = the allocated size from start to end.
     */
    unsigned long alloc_bytes_count_start, free_bytes_count_start, alloc_bytes_count_end, free_bytes_count_end;

    SHR_FUNC_INIT_VARS(unit);

    sal_get_alloc_counters(&alloc_bytes_count_start, &free_bytes_count_start);

    if (Lsm_initizilazed)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, Lsm_cpu_port, &flags, &port_interface_info, &port_mapping_info));
        if ((port_interface_info.interface == BCM_PORT_IF_CPU) && (Lsm_rx_cb_enable == TRUE))
        {
            SHR_IF_ERR_EXIT(appl_ref_lsm_pkt_stop(unit));
        }
        SHR_IF_ERR_EXIT(appl_ref_lsm_deinit_traps(unit));
        SHR_IF_ERR_EXIT(appl_ref_lsm_vis_disable(unit));
        if (Lsm_interrupt_cb_enable == TRUE)
        {
            SHR_IF_ERR_EXIT(appl_ref_lsm_interrupt_disable(unit));
        }
    }
    SHR_IF_ERR_EXIT(appl_ref_lsm_db_qualifiers_table_destroy(unit));
    SHR_IF_ERR_EXIT(appl_ref_lsm_db_trap_table_destroy(unit));
    SHR_IF_ERR_EXIT(appl_ref_lsm_db_interrupt_table_destroy(unit));
    SHR_IF_ERR_EXIT(appl_ref_lsm_action_table_destroy_all(unit));

#ifndef    SAL_THREAD_NAME_PRINT_DISABLE
    SHR_IF_ERR_EXIT(sal_console_thread_name_print_enable(TRUE));
#endif
    Lsm_initizilazed = FALSE;

    sal_get_alloc_counters(&alloc_bytes_count_end, &free_bytes_count_end);
    LOG_VERBOSE(BSL_LS_BCMDNX_LSM,
                (BSL_META_U(unit, "LSM: Deinit mem usage:\n\ralloc %lu\n\rfree %lu\n\r"),
                 alloc_bytes_count_end - alloc_bytes_count_start, free_bytes_count_end - free_bytes_count_start));

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
