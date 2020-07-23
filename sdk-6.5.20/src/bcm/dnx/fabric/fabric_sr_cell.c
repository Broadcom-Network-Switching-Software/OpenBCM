/**
 * \file fabric_sr_cell.c $Id$ Fabric Source Routing Cell procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>

#include <bcm/fabric.h>
#include <bcm_int/dnx_dispatch.h>
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
/*
 * Size of data cell to send
 */
#define DNX_FABRIC_DATA_CELL_U32_SIZE            (36)
#define DNX_FABRIC_DATA_CELL_BYTE_SIZE           DNX_FABRIC_DATA_CELL_U32_SIZE * 4
/*
 * The position of the device link in the path_links array
 */
#define DNX_FABRIC_CELL_PATH_LINK_FIP_SWITCH_POSITION      (0)
#define DNX_FABRIC_CELL_PATH_LINK_FE1_SWITCH_POSITION      (1)
#define DNX_FABRIC_CELL_PATH_LINK_FE2_SWITCH_POSITION      (2)
#define DNX_FABRIC_CELL_PATH_LINK_FE3_SWITCH_POSITION      (3)
/*
 * Several fields from the DNX_FABRIC_CELL_CAPTURED_SRC_FORMAT table are duplicated like soc_dnxc_fabric_cell_field_fip
 * which causes problem when referring to possible table size based only on soc_dnxc_fabric_cell_field_nof
 */
#define DNX_FABRIC_CELL_CAPTURED_SRC_FORMAT_FIELDS_DUPLICATIONS_NOF     (7)

#define DNX_FABRIC_CELL_CAPTURED_SRC_FORMAT {\
  /*{dest_id,                                     dest_ptr, dest_start_bit, src_start_bit, length, name)*/\
    {soc_dnxc_fabric_cell_field_cell_type,          NULL,     0,              583,            2,     "CELL_TYPE"},\
    {soc_dnxc_fabric_cell_field_src_device,         NULL,     0,              555,            11,    "SOURCE_DEVICE"},\
    {soc_dnxc_fabric_cell_field_src_level,          NULL,     0,              552,            3,     "SOURCE_LEVEL"},\
    {soc_dnxc_fabric_cell_field_dest_level,         NULL,     0,              549,            3,     "DEST_LEVEL"},\
    {soc_dnxc_fabric_cell_field_fip,                NULL,     0,              544,            5,     "FIP[0:4]"},\
    {soc_dnxc_fabric_cell_field_fip,                NULL,     5,              527,            1,     "FIP[5]"},\
    {soc_dnxc_fabric_cell_field_fe1,                NULL,     0,              539,            5,     "FE1[0:4]"},\
    {soc_dnxc_fabric_cell_field_fe1,                NULL,     5,              526,            1,     "FE1[5]"},\
    {soc_dnxc_fabric_cell_field_fe1,                NULL,     6,              569,            2,     "FE1[6:7]"},\
    {soc_dnxc_fabric_cell_field_fe2,                NULL,     0,              533,            6,     "FE2[0:5]"},\
    {soc_dnxc_fabric_cell_field_fe2,                NULL,     6,              525,            1,     "FE2[6]"},\
    {soc_dnxc_fabric_cell_field_fe2,                NULL,     7,              568,            1,     "FE2[7]"},\
    {soc_dnxc_fabric_cell_field_fe3,                NULL,     0,              528,            5,     "FE3[0:4]"},\
    {soc_dnxc_fabric_cell_field_fe3,                NULL,     5,              524,            1,     "FE3[5]"},\
    {soc_dnxc_fabric_cell_field_fe3,                NULL,     6,              566,            2,     "FE3[6:7]"},\
    {soc_dnxc_fabric_cell_field_is_inband,          NULL,     0,              523,            1,     "IS_INBAND"},\
    {soc_dnxc_fabric_cell_field_pipe_id,            NULL,     0,              571,            3,     "PIPE_ID"},\
    {soc_dnxc_fabric_cell_field_payload,            NULL,     0,              0,              512,   "PAYLOAD"},\
    /** Always must be last */ \
    {soc_dnxc_fabric_cell_field_invalid,            NULL,     0,              -1,            -1,     "INVALID"}\
    }

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
            sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFIP;
            break;
        }
        case 1:
        {
            if (connectivity_map.device_type == bcmFabricDeviceTypeFE13)
            {
                sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFE1;
            }
            else if (connectivity_map.device_type == bcmFabricDeviceTypeFE2)
            {
                sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFE2;
            }
            else if (connectivity_map.device_type == bcmFabricDeviceTypeFAP)
            {
                sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFOP;
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
                sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFE2;
            }
            else if (connectivity_map.device_type == bcmFabricDeviceTypeFE2)
            {
                sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFOP;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to get destination device");
            }
            break;
        }
        case 3:
        {
            sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFE3;
            break;
        }
        case 4:
        {
            sr_link_list->dest_entity_type = dnxcFabricDeviceTypeFOP;
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
    soc_dnxc_fabric_cell_parse_table_t temp_table[] = DNX_FABRIC_CELL_CAPTURED_SRC_FORMAT;
    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(parse_table, temp_table, sizeof(temp_table));

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_sr_cell_parse(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry,
    dnxc_vsc256_sr_cell_t * cell)
{
    soc_dnxc_fabric_cell_info_t cell_info;
    soc_dnxc_fabric_cell_parse_table_t parse_table[soc_dnxc_fabric_cell_field_nof +
                                                   DNX_FABRIC_CELL_CAPTURED_SRC_FORMAT_FIELDS_DUPLICATIONS_NOF];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_parse_table_get(unit, parse_table));
    /*
     * Currently we support only parsing SR cells for DNX devices
     */
    cell_info.cell_type = soc_dnxc_fabric_cell_type_sr_cell;

    SHR_IF_ERR_EXIT(soc_dnxc_fabric_cell_parser(unit, entry, parse_table, &cell_info));

    *cell = cell_info.cell.sr_cell;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Send source routed cell.
 */
static shr_error_e
dnx_fabric_sr_cell_send(
    int unit,
    dnxc_vsc256_sr_cell_t * data_cell_sent)
{
    uint32 entry_handle_id;
    uint32 *buf;
    int i;
    uint32 packed_cpu_data_cell_sent[DNX_FABRIC_DATA_CELL_U32_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(packed_cpu_data_cell_sent, 0x0, DNX_FABRIC_DATA_CELL_U32_SIZE * sizeof(uint32));

    /*
     * Build the header - VSC256 cell
     * Next 2 procedures take a structure vsc256_sr_cell_t and build a buffer in order to send it.
     */
    buf = packed_cpu_data_cell_sent + (DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BITS +
                                       dnx_data_fabric.cell.sr_cell_header_offset_get(unit)) / (8 * sizeof(uint32));

    SHR_IF_ERR_EXIT(soc_dnxc_vsc256_build_header(unit, data_cell_sent,
                                                 DNX_FABRIC_DATA_CELL_BYTE_SIZE -
                                                 (DNX_FABRIC_SR_DATA_CELL_PAYLOAD_LENGTH_IN_BITS / 8), buf));
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
    dnxc_vsc256_sr_cell_t cell;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cell.payload.data[0], 0x0, sizeof(uint32) * DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);

    cell.header.cell_type = soc_dnxc_fabric_cell_type_sr_cell;
    cell.header.source_device = (uint32) unit;
    cell.header.source_level = dnxcFabricDeviceTypeFIP;
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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
            int sr_cell_data_lsb;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_SR_CELL_RECEIVE_DATA, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SR_CELL_INSTANCE, i_sr_instance);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            /** Read LSB - probably 512 */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SR_CELL_DATA_LSB,
                                                                    INST_SINGLE, cell_buffer));

            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_FABRIC_SR_CELL_RECEIVE_DATA,
                                                       DBAL_FIELD_SR_CELL_DATA_LSB, FALSE, 0, 0, &sr_cell_data_lsb));
            /** Now read what is left */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SR_CELL_DATA_MSB,
                                                                    INST_SINGLE, cell_buffer + sr_cell_data_lsb / 8));
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_EMPTY, "All FIFO's are empty, nothing to read");

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
    dnxc_vsc256_sr_cell_t data_cell_rcv;
    uint32 data_out_size;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data_cell_rcv, 0x0, sizeof(dnxc_vsc256_sr_cell_t));
    sal_memset(entry, 0x0, sizeof(entry));

    /*
     * Get the data
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_cpu_data_get(unit, (uint32 *) entry));

    /*
     * Parsing to a source-routed cell
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_parse(unit, entry, &data_cell_rcv));

    data_out_size = data_out_max_size * 4 < WORDS2BYTES(DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32) ?
        data_out_max_size * 4 : WORDS2BYTES(DNXC_VSC256_SR_DATA_CELL_PAYLOAD_MAX_LENGTH_U32);

    sal_memcpy(data_out, data_cell_rcv.payload.data, data_out_size);

    *data_out_size_p = UTILEX_DIV_ROUND_UP(data_out_size, 4);
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
int
bcm_dnx_fabric_route_rx(
    int unit,
    uint32 flags,
    uint32 data_out_max_size,
    uint32 *data_out,
    uint32 *data_out_size)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_route_rx_verify(unit, flags, data_out_max_size, data_out, data_out_size));

    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_receive(unit, flags, data_out_max_size, data_out_size, data_out));

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
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_route_tx_verify(unit, flags, route, data_in_size, data_in));

    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_route_2_link_list(unit, route, &sr_link_list));

    /**
     * data_in_size is in uint32s so no need to change the format
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sr_cell_write(unit, &sr_link_list, data_in_size, data_in));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
