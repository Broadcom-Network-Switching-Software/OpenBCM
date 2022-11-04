/**
 * \file fabric_sr_cell.c $Id$ Fabric Source Routing Cell procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/fabric.h>
#include <bcm_int/dnx/auto_generated/dnx_fabric_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <soc/dnxc/dnxc_fabric_cell.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>

#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * Size of Received data cell
 */
#define DNX_FABRIC_DATA_CELL_RECEIVED_SIZE_IN_BYTES  (74)
#define DNX_FABRIC_RECEIVED_DATA_CELL_SIZE_IN_BITS   (512)
/*
 * Length of Source Routed Cell payload (in bits)
 */
#define DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BITS      (1024)
#define DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BYTES     ( BITS2BYTES(DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BITS) )
/*
 * Size of data cell to send
 */
#define DNX_FABRIC_DATA_CELL_U32_SIZE            (36)
#define DNX_FABRIC_DATA_CELL_BYTE_SIZE           DNX_FABRIC_DATA_CELL_U32_SIZE * 4

#define DNX_FABRIC_SR_DATA_CELL_HEADER_LENGTH_IN_BYTES     (DNX_FABRIC_DATA_CELL_BYTE_SIZE - DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BYTES)
/*
 * The position of the device link in the path_links array
 */
#define DNX_FABRIC_CELL_PATH_LINK_FIP_SWITCH_POSITION      (0)
#define DNX_FABRIC_CELL_PATH_LINK_FE1_SWITCH_POSITION      (1)
#define DNX_FABRIC_CELL_PATH_LINK_FE2_SWITCH_POSITION      (2)
#define DNX_FABRIC_CELL_PATH_LINK_FE3_SWITCH_POSITION      (3)

/* transform a BCM route struct to dnxc_sr_cell_link_list_t sr_link_list struct */
static shr_error_e
dnx_fabric_sr_cell_route_2_link_list(
    int unit,
    const bcm_fabric_route_t * route,
    dnxc_sr_cell_link_list_t * sr_link_list)
{
    int hop_index;
    bcm_fabric_link_connectivity_t connectivity_map;
    uint8 is_multi_stage;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(sr_link_list, 0, sizeof(dnxc_sr_cell_link_list_t));

    /*
     * route->hop_ids are links, but bcm_dnx_fabric_link_connectivity_status_single_get
     * expects port as an input param.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, route->hop_ids[0], &logical_port));
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_link_connectivity_status_single_get(unit, logical_port, &connectivity_map));

    switch (route->number_of_hops)
    {
        case 0:
        {
            sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FIP;
            break;
        }
        case 1:
        {
            if (connectivity_map.device_type == bcmFabricDeviceTypeFE13)
            {
                sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FE1;
            }
            else if (connectivity_map.device_type == bcmFabricDeviceTypeFE2)
            {
                sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FE2;
            }
            else if (connectivity_map.device_type == bcmFabricDeviceTypeFAP)
            {
                sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FOP;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get destination device");
            }
            break;
        }
        case 2:
        {
            if (connectivity_map.device_type == bcmFabricDeviceTypeFE13)
            {
                sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FE2;
            }
            else if (connectivity_map.device_type == bcmFabricDeviceTypeFE2)
            {
                sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FOP;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get destination device");
            }
            break;
        }
        case 3:
        {
            sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FE3;
            break;
        }
        case 4:
        {
            sr_link_list->dest_entity_type = _SHR_FABRIC_DEVICE_TYPE_FOP;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "number_of_hops %d is out of limit", route->number_of_hops);
        }
    }

    is_multi_stage = (connectivity_map.device_type == bcmFabricDeviceTypeFE2) ? FALSE : TRUE;

    if (is_multi_stage)
    {
        for (hop_index = 0; hop_index < route->number_of_hops; ++hop_index)
        {
            sr_link_list->path_links[hop_index] = (uint8) route->hop_ids[hop_index];
        }
    }
    else
    {
        sr_link_list->path_links[0] = (uint8) route->hop_ids[0];
        sr_link_list->path_links[1] = 0;
        sr_link_list->path_links[2] = (uint8) route->hop_ids[1];
        sr_link_list->path_links[3] = 0;
    }

    /*
     * Pipe_id 
     */
    sr_link_list->pipe_id = route->pipe_id;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_sr_cell_parse_table_get(
    int unit,
    soc_dnxc_fabric_cell_parse_table_t * parse_table)
{
    int key = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (key = 0; key < dnx_data_fabric.cell.sr_cell_rx_parsing_table_info_get(unit)->key_size[0]; ++key)
    {
        parse_table[key].dest_id = dnx_data_fabric.cell.sr_cell_rx_parsing_table_get(unit, key)->dest_id;
        parse_table[key].dest = NULL;
        parse_table[key].dest_start_bit = dnx_data_fabric.cell.sr_cell_rx_parsing_table_get(unit, key)->dest_start_bit;
        parse_table[key].src_start_bit = dnx_data_fabric.cell.sr_cell_rx_parsing_table_get(unit, key)->src_start_bit;
        parse_table[key].length = dnx_data_fabric.cell.sr_cell_rx_parsing_table_get(unit, key)->length;
        parse_table[key].field_name = dnx_data_fabric.cell.sr_cell_rx_parsing_table_get(unit, key)->field_name;
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_sr_cell_parse(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry,
    dnxc_sr_cell_t * cell)
{
    soc_dnxc_fabric_cell_info_t cell_info;
    int parse_table_size = dnx_data_fabric.cell.sr_cell_rx_parsing_table_info_get(unit)->key_size[0];
    soc_dnxc_fabric_cell_parse_table_t *parse_table = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(parse_table, sizeof(soc_dnxc_fabric_cell_parse_table_t) * parse_table_size,
                       "SR Cell Parse Table", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_parse_table_get(unit, parse_table));
    /*
     * Currently we support only parsing SR cells for DNX devices
     */
    sal_memset(&cell_info, 0x00, sizeof(soc_dnxc_fabric_cell_info_t));
    cell_info.cell_type = soc_dnxc_fabric_cell_type_sr_cell;

    SHR_IF_ERR_EXIT(soc_dnxc_fabric_cell_parser(unit, entry, parse_table, &cell_info));

    *cell = cell_info.cell.sr_cell;

exit:
    SHR_FREE(parse_table);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Send source routed cell.
 */
static shr_error_e
dnx_fabric_sr_cell_send(
    int unit,
    dnxc_sr_cell_t * data_cell_sent)
{
    uint32 entry_handle_id;
    uint32 *buf;
    int i;
    uint32 packed_cpu_data_cell_sent[DNX_FABRIC_DATA_CELL_U32_SIZE];
    int cell_format = dnx_data_fabric.cell.cell_format_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(packed_cpu_data_cell_sent, 0x0, DNX_FABRIC_DATA_CELL_U32_SIZE * sizeof(uint32));

    /*
     * Build the header
     * Next 2 procedures take VSC cell format structure and build a buffer in order to send it.
     */
    buf = packed_cpu_data_cell_sent + (DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BITS +
                                       dnx_data_fabric.cell.sr_cell_header_offset_get(unit)) / (8 * sizeof(uint32));

    switch (cell_format)
    {
        case BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V2:
            SHR_IF_ERR_EXIT(soc_dnxc_vsc256_sr_cell_build_header(unit, data_cell_sent,
                                                                 DNX_FABRIC_SR_DATA_CELL_HEADER_LENGTH_IN_BYTES, buf));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid cell format for sending a source routed cell %d", cell_format);
            break;
    }

    /*
     * bits 1023:0 - payload
     */
    SHR_BITCOPY_RANGE(packed_cpu_data_cell_sent,
                      DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BITS - DNX_FABRIC_RECEIVED_DATA_CELL_SIZE_IN_BITS,
                      data_cell_sent->payload.data, 0, DNX_FABRIC_RECEIVED_DATA_CELL_SIZE_IN_BITS);
    /*
     * Copy the data
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SR_CELL_TRANSMIT_DATA, &entry_handle_id));
    for (i = 0; i < 3; i++)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SR_CELL_DATA, i,
                                         packed_cpu_data_cell_sent + i * (256 / (8 * sizeof(uint32))));
    }

    /** Fourth part is of different length (376) - so separate field was defined to deal with it */
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_SR_CELL_DATA_3, INST_SINGLE,
                                     packed_cpu_data_cell_sent + i * (256 / (8 * sizeof(uint32))));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Assign LINK_ID
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_SR_CELL_TRANSMIT_CONTROL, entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK_ID, INST_SINGLE,
                                 data_cell_sent->header.fip_link);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set cell size (must be decreased by 1, because register value N produces a N+1 byte cell size)
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CELL_SIZE, INST_SINGLE,
                                 (DNXC_FABRIC_CELL_SOURCE_ROUTED_CELL_SIZE - 1));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Trigger the transmission
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_SR_CELL_TRANSMIT_CONTROL, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRIGGER, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Send Source Routed Cell.
 * \param [in] unit -
 *   The unit number.
 * \param [in] sr_link_list -
 *   The route of the cell.
 * \param [in] data_in_size -
 *   Size of 'data_in' param (in uint32 size).
 * \param [in] data_in -
 *   The payload of the cell.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_sr_cell_write(
    int unit,
    dnxc_sr_cell_link_list_t * sr_link_list,
    uint32 data_in_size,
    uint32 *data_in)
{
    dnxc_sr_cell_t cell;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cell.payload.data[0], 0x0, sizeof(uint32) * DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);

    cell.header.cell_type = soc_dnxc_fabric_cell_type_sr_cell;
    cell.header.source_device = (uint32) unit;
    cell.header.source_level = _SHR_FABRIC_DEVICE_TYPE_FIP;
    cell.header.destination_level = sr_link_list->dest_entity_type;
    cell.header.fip_link = sr_link_list->path_links[DNX_FABRIC_CELL_PATH_LINK_FIP_SWITCH_POSITION];
    cell.header.fe1_link = sr_link_list->path_links[DNX_FABRIC_CELL_PATH_LINK_FE1_SWITCH_POSITION];
    cell.header.fe2_link = sr_link_list->path_links[DNX_FABRIC_CELL_PATH_LINK_FE2_SWITCH_POSITION];
    cell.header.fe3_link = sr_link_list->path_links[DNX_FABRIC_CELL_PATH_LINK_FE3_SWITCH_POSITION];
    cell.header.ack = 0;
    cell.header.is_inband = 0;
    cell.header.pipe_id = sr_link_list->pipe_id;

    sal_memcpy(cell.payload.data, data_in, (data_in_size < DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32) ?
               WORDS2BYTES(data_in_size) : WORDS2BYTES(DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32));

    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_send(unit, &cell));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get data from a received source routed cell.
 * \param [in] unit -
 *   The unit number.
 * \param [out] cell_buffer -
 *   Buffer to which the data is retrieved.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_sr_cell_cpu_data_get(
    int unit,
    uint32 *cell_buffer)
{
    uint32 entry_handle_id;
    int i_sr_instance;
    uint32 fifo_status;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    uint32 sr_cell_nof_instances = dnx_data_fabric.cell.sr_cell_nof_instances_get(unit);
    int cell_rx_fifo_fields[4] = { DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY };
    int sr_cell_rx_fifo_size = dnx_data_fabric.cell.sr_cell_rx_fifo_size_get(unit);
    uint32 i_field = 0, bits_read = 0;
    int sr_cell_data_field_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * The max DBAL field is 512, so the tables can have a different number of fields for different devices.
     */
    cell_rx_fifo_fields[0] = DBAL_FIELD_SR_CELL_DATA_0;
    cell_rx_fifo_fields[1] = DBAL_FIELD_SR_CELL_DATA_512;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SR_CELL_RECEIVE_CONTROL, &entry_handle_id));
    /*
     * Find which MAC CPU-FIFO is not empty.
     */
    for (i_sr_instance = 0; i_sr_instance < sr_cell_nof_instances * nof_cores; i_sr_instance++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, i_sr_instance);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SR_CELL_DATA_READY,
                                                            INST_SINGLE, &fifo_status));
        if (fifo_status == TRUE)
        {
            /*
             * Read MAC CPU-FIFO contents.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_SR_CELL_RECEIVE_DATA, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, i_sr_instance);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            while (bits_read < sr_cell_rx_fifo_size)
            {
                /** Read in chunks of DBAL field size (512bits) */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, cell_rx_fifo_fields[i_field], INST_SINGLE,
                                 cell_buffer + (bits_read + 31) / 32));

                SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_FABRIC_SR_CELL_RECEIVE_DATA,
                                                           cell_rx_fifo_fields[i_field], FALSE, 0, 0,
                                                           &sr_cell_data_field_size));

                bits_read += sr_cell_data_field_size;
                ++i_field;
            }
            SHR_EXIT();
        }
    }

    /** All FIFOs are empty, nothing to read */
    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_sr_cell_receive(
    int unit,
    uint32 flags,
    uint32 data_out_max_size,
    uint32 *data_out_size_p,    /* Units: Words */
    uint32 *data_out)
{
    soc_dnxc_fabric_cell_entry_t entry;
    dnxc_sr_cell_t data_cell_rcv;
    uint32 data_out_size;
    int payload_offset = 0;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data_cell_rcv, 0x0, sizeof(dnxc_sr_cell_t));
    sal_memset(entry, 0x0, sizeof(entry));

    /*
     * Get the data
     */
    rv = dnx_fabric_sr_cell_cpu_data_get(unit, (uint32 *) entry);
    if (rv == _SHR_E_EMPTY)
    {
        /** Set the cell size to be 0 */
        *data_out_size_p = 0;
        /** Return the error, but don't print error message */
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Parsing to a source-routed cell
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_parse(unit, entry, &data_cell_rcv));

    data_out_size = data_out_max_size * 4 < WORDS2BYTES(DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32) ?
        data_out_max_size * 4 : WORDS2BYTES(DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);

    if (flags & _SHR_FABRIC_CELL_RX_HEADER_PREPEND)
    {
        data_out[_shr_dnxc_fabric_cell_header_field_type_source_device] = data_cell_rcv.header.source_device;
        data_out[_shr_dnxc_fabric_cell_header_field_type_fip_link] = data_cell_rcv.header.fip_link;
        data_out[_shr_dnxc_fabric_cell_header_field_type_pipe] = data_cell_rcv.header.pipe_id;
        /*
         * If there is prepend header, the payload starts after the header
         */
        payload_offset = _shr_dnxc_fabric_cell_header_field_type_nof;
    }

    sal_memcpy(data_out + payload_offset, data_cell_rcv.payload.data, data_out_size - payload_offset);

    *data_out_size_p = UTILEX_DIV_ROUND_UP(data_out_size, 4);
    *data_out_size_p += payload_offset;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for Receive Source routed data cells
 *
 * \param [in]  unit                - Unit number.
 * \param [in]  flags               - flags (currently not in use)
 * \param [in]  data_out_max_size   - max "data_out" size in words(32b)
 * \param [out] data_out            - buffer the received data.
 * \param [out] data_out_size       - "data_out" actual size in words(32b)
 *
 * \return
 *      Standard shr_error_e error handling
 * \see
 *      bcm_fabric_route_tx
 */
static int
dnx_fabric_route_rx_verify(
    int unit,
    uint32 flags,
    uint32 data_out_max_size,
    uint32 *data_out,
    uint32 *data_out_size)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");
    SHR_NULL_CHECK(data_out_size, _SHR_E_PARAM, "data_out_size");
    if (data_out_max_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "buffer is too small");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Receive Source routed data cells
 *
 * \param [in] unit                 - Unit number.
 * \param [in] flags                - flags (currently not in use)
 * \param [in] data_out_max_size    - max "data_out" size in words(32b)
 * \param [out] data_out            - buffer the received data.
 * \param [out] data_out_size       - "data_out" actual size in words(32b)
 *
 * \return
 *      _SHR_E_EMPTY in case no cell was received.
 *      Standard shr_error_e error handling
 * \see
 *      bcm_fabric_route_tx
 */
int
bcm_dnx_fabric_route_rx(
    int unit,
    uint32 flags,
    uint32 data_out_max_size,
    uint32 *data_out,
    uint32 *data_out_size)
{
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_fabric_route_rx_verify(unit, flags, data_out_max_size, data_out, data_out_size));

    rv = dnx_fabric_sr_cell_receive(unit, flags, data_out_max_size, data_out_size, data_out);
    if (rv == _SHR_E_EMPTY)
    {
        /** Return the error, but don't print error message */
        SHR_IF_ERR_EXIT_NO_MSG(rv);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify routine for Send Source routed data cells
 *
 * \param [in]  unit                - Unit number.
 * \param [in]  flags               - flags (currently not in use)
 * \param [in]  route               - Specify the path for the generated cells
 * \param [in]  data_in_size        - "data_in" size in words(32b)
 * \param [in]  data_in             - pointer to the payload data for transmission
 *
 * \return
 *      Standard shr_error_e error handling
 * \see
 *      bcm_fabric_route_rx
 */
static int
dnx_fabric_route_tx_verify(
    int unit,
    uint32 flags,
    bcm_fabric_route_t * route,
    uint32 data_in_size,
    uint32 *data_in)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * verify
     */
    SHR_NULL_CHECK(route, _SHR_E_PARAM, "route");
    SHR_NULL_CHECK(route->hop_ids, _SHR_E_PARAM, "route->hop_ids");
    SHR_NULL_CHECK(data_in, _SHR_E_PARAM, "data_in");
    if (data_in_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "no data to send\n");
    }

    if ((route->number_of_hops <= 0) || (route->number_of_hops > DNXC_FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number_of_hops:%d is out of range:1-%d\n", route->number_of_hops,
                     DNXC_FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Send Source routed data cells
 *
 * \param [in] unit                - Unit number.
 * \param [in] flags               - flags (currently not in use)
 * \param [in] route               - Specify the path for the generated cells
 * \param [in] data_in_size        - "data_in" size in words(32b)
 * \param [in] data_in             - pointer to the payload data for transmission
 *
 * \return
 *      Standard shr_error_e error handling
 * \see
 *      bcm_fabric_route_rx
 */
int
bcm_dnx_fabric_route_tx(
    int unit,
    uint32 flags,
    bcm_fabric_route_t * route,
    uint32 data_in_size,
    uint32 *data_in)
{
    dnxc_sr_cell_link_list_t sr_link_list;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * verify
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_fabric_route_tx_verify(unit, flags, route, data_in_size, data_in));

    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_route_2_link_list(unit, route, &sr_link_list));

    /**
     * data_in_size is in uint32s so no need to change the format
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_write(unit, &sr_link_list, data_in_size, data_in));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
