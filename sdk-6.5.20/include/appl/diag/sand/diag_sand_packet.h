/**
 * \file diag_sand_packet.h
 *
 * Header for diag command 'dnx packet'.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_PACKET_INCLUDED
#define DIAG_SAND_PACKET_INCLUDED

/*
 * Dependencies (includes)
 * {
 */
#include <soc/sand/sand_pparse.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <bcm/sat.h>
/*
 * }
 * Dependencies (includes)
 */

#define SAND_PACKET_DEFAULT_SIZE_BYTES      128

#define TAGGED_PACKET_LENGTH           68
#define UNTAGGED_PACKET_LENGTH         64

/*
 * Flags controlling packet send behavior
 * {
 */
/*
 *  signal cache will be cleared before send, otherwise left untouched
 */
#define SAND_PACKET_RESUME              0x01
/*
 * Initiate recording receive packets in dedicated global queue
 */
#define SAND_PACKET_RX                  0x02
/*
 * Send packet stream from SAT port
 */
#define SAND_PACKET_SAT                 0x04
/*
 * Stop running SAT stream
 */
#define SAND_PACKET_STOP                0x08

/*
 * Flag that indicates that a SA field will be incremented during sending packet stream
 */
#define SAND_PACKET_STREAM_SA_INCREMENTAL_FIELD  0x01
/*
 * }
 */

#define SAND_STREAM_TIME_MAX                3600
/*
 * In order to avoid changing granularity on SDK level use BY_BCM
 */
#define SAT_GRANULARITY_SET_BY_BCM              0
/*
 * In order to granularity adopted to rate use BY_DIAG
 */
#define SAT_GRANULARITY_SET_BY_DIAG             -1

typedef struct
{
    int gtf_id;
    int rate_unit;
    int rate;
    int burst;
    int granularity;            /* clocks per cycle */
    int duration;
    int running;
} sand_packet_stream_t;

typedef struct
{
    int ctf_id;
    bcm_field_entry_t entry_id;
} sand_packet_collector_t;

typedef struct
{
    rhentry_t entry;
    rhlist_t *proto_list;
    int core_id;
    int header_size;
    int packet_size;
    uint32 flags;
    sand_packet_stream_t stream;
    sand_packet_collector_t collector;
} sand_packet_t;

#define PACKET_GRANULARITY(packet_h_mac)        (((sand_packet_t *)packet_h_mac)->stream.granularity)
#define PACKET_DURATION(packet_h_mac)           (((sand_packet_t *)packet_h_mac)->stream.duration)
#define PACKET_RATE(packet_h_mac)               (((sand_packet_t *)packet_h_mac)->stream.rate)
#define PACKET_BURST(packet_h_mac)              (((sand_packet_t *)packet_h_mac)->stream.burst)
#define PACKET_GTF_ID(packet_h_mac)             (((sand_packet_t *)packet_h_mac)->stream.gtf_id)
#define PACKET_PROTO_LIST(packet_h_mac)         (((sand_packet_t *)packet_h_mac)->proto_list)
#define PACKET_SIZE(packet_h_mac)               (((sand_packet_t *)packet_h_mac)->packet_size)
#define PACKET_CORE(packet_h_mac)               (((sand_packet_t *)packet_h_mac)->core_id)

#define PACKET_CTF_ID(packet_h_mac)             (((sand_packet_t *)packet_h_mac)->collector.ctf_id)

shr_error_e diag_sand_packet_sat_rx_get_stats(
    int unit,
    int ctf_id,
    uint64 *rcv_packet_count_p,
    uint64 *err_packet_count_p,
    uint64 *ooo_packet_count_p);

shr_error_e diag_sand_packet_sat_rx_start(
    int unit);

shr_error_e diag_sand_packet_sat_rx_destroy(
    int unit);

shr_error_e diag_sand_packet_sat_rx_create(
    int unit,
    rhhandle_t packet_h,
    uint32 out_port);

/**
 * \brief
 *    Generate gtf for SAT packet streaming
 * \param [in] unit - Unit #
 * \param [in] core_id - core id  #*
 * \param [in] packet_handle - handle to the object which holds all packet/stream related params
 * \param [in] pkt_length - full packet length (units: bytes)
 * \param [in] time - time period for stream (units: seconds)
 * \param [in] rate_unit - unit for rate parameter - either kbits or packets per second
 * \param [in] rate - stream rate (units: kbits per second)
 * \param [in] burst - stream burst size (units: bytes)
 * \param [in] granularity - globally clocks per cycle,
 *                         Special values
 *                         SAT_GRANULARITY_SET_BY_DIAG - when there is a need to have optimized flat flow with high
 *                                                       rates, not to be used when multiple flows with different rates
 *                                                       are in use. If there is a need for multiple, for the first
 *                                                       stream use BY_DIAG, for the rest BY_BCM
 *                         SAT_GRANULARITY_SET_BY_BCM  - for multiple flows with lower rates
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \see
 *   diag_sand_packet_create
 */
shr_error_e diag_sand_packet_stream_create(
    int unit,
    int core_id,
    rhhandle_t packet_handle,
    int pkt_length,
    int time,
    int rate_unit,
    int rate,
    int burst,
    int granularity);

/**
 * \brief
 *    Fetch statistics for SAT packet streaming
 * \param [in] unit - Unit #
 * \param [in] packet_handle   - handle to the object which holds all packet/stream related params
 * \param [out] bytes_count_p  - pointer to byte count after running SAT stream
 * \param [out] packet_count_p - pointer to packet count after running SAT stream
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \see
 *   diag_sand_packet_create
 */
shr_error_e diag_sand_packet_stream_get_stats(
    int unit,
    rhhandle_t packet_handle,
    uint64 *bytes_count_p,
    uint64 *packet_count_p);

/**
 * \brief
 *    Start streams for all packet handles in array
 * \param [in] unit - Unit #
 * \param [in] packet_handles   - pointer to array of handles to the object which holds all packet/stream related params
 * \param [in] stream_nof - number of streams
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \see
 *   diag_sand_packet_send
 */
shr_error_e diag_sand_packet_stream_send(
    int unit,
    rhhandle_t * packet_handles,
    int stream_nof);

/**
 * \brief
 *    Send packet from port
 * \param [in] unit - Unit #
 * \param [in] src_port - port id, if you don't know what to do use 0 - default CPU IN_TM_PORT
 * \param [in] packet_length - full packet length including headers and payload. O - no payload
 * \param [in] data_string - data to be sent
 * \param [in] flags - misc flags controlling packet tx
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \see
 *   diag_sand_packet_create
 */
shr_error_e diag_sand_packet_tx(
    int unit,
    bcm_port_t src_port,
    int packet_length,
    char *data_string,
    uint32 flags);

/**
 * \brief
 *    Allocate new packet object to be used by other diag_sand_packet* APIs
 * \param [in] unit - Unit #
 * \param [in,out] packet_handle_p - pointer to packet control handle allocated
 *
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_MEMORY problem with allocating new object
 * \retval shr_error_e Standard error handling
 * \remark
 *   Each invocation will create new packet, do not forget to free it using diag_sand_packet_free
 * \see
 *   diag_sand_packet_free
 */
shr_error_e diag_sand_packet_create(
    int unit,
    rhhandle_t * packet_handle_p);

/**
 * \brief
 *    Add proto to header stack of packet, order of adding will be reflected in packet structure, handle to added header an output
 * \param [in] unit - Unit #
 * \param [in] packet_handle - packet control handle
 * \param [in] proto - name of protocol to be used when adding header, see NetworkStructures.xml for the list of supported ones.
 *
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with protocol
 * \retval shr_error_e Standard error handling
 * \remark
 *   use "sig struct pkt" to list all supported protocols
 * \see
 *   NetworkStructures.xml
 */
shr_error_e diag_sand_packet_proto_add(
    int unit,
    rhhandle_t packet_handle,
    char *proto);

/**
 * \brief
 *    Add field that need to be different from default value(provided by NetworkStructures.xml). Input value is provided by string
 * \param [in] unit - Unit #
 * \param [in] packet_handle - handle to the packet, the field is added to,
 * \param [in] field_name - name of protocol field, including protocol name itself. E.g. VLAN.VID, ETH.DA.
 *                             Field name may be complex one and actually be subfield of a field. e.g. IPv6.sip.low
 * \param [in] str_value -  pointer to the string representing value of field specified by field_name,
 *                             string size should match field definition and in anyway not bigger than DSIG_MAX_SIZE_STR
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with field name
 * \retval shr_error_e Standard error handling
 * \remark
 *   use "sig struct pkt" to list all supported protocols
 *   Add second time same field to the same header will replace previous value, useful for testing different values of specific qualifiers
 * \see
 *   NetworkStructures.xml
 */
shr_error_e diag_sand_packet_field_add_str(
    int unit,
    rhhandle_t packet_handle,
    char *field_name,
    char *str_value);

/**
 * \brief
 *    Add field that need to be different from default value(provided by NetworkStructures.xml). Input value is provided by uint32 array
 * \param [in] unit - Unit #
 * \param [in] packet_handle - handle to the packet, the field is added to, obtained from diag_sand_packet_proto_add
 * \param [in] field_name - name of protocol field, including protocol name itself. E.g. VLAN.VID, ETH.DA.
 *                             Field name may be complex one and actually be subfield of a field. e.g. IPv6.sip.low
 * \param [in] uint32_value - pointer to uint32 array representing value of field specified by field_name, size should match field definition
 * \param [in] bit_size - size(in bits) should match field definition
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with field name
 * \retval shr_error_e Standard error handling
 * \remark
 *   use "sig struct pkt" to list all supported protocols
 *   Add second time same field to the same header will replace previous value, useful for testing different values of specific qualifiers
 * \see
 *   NetworkStructures.xml
 */
shr_error_e diag_sand_packet_field_add_uint32(
    int unit,
    rhhandle_t packet_handle,
    char *field_name,
    uint32 *uint32_value,
    int bit_size);

/**
 * \brief
 *    Frees packet control structure and all underlying allocated objects
 * \param [in] unit - Unit #
 * \param [in] packet_handle - packet control handle
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \remark
 *   If the handle is NULL - no error will be issued and no log will be recorder
 * \see
 *   diag_sand_packet_create
 */
void diag_sand_packet_free(
    int unit,
    rhhandle_t packet_handle);

/**
 * \brief
 *    Send the packet. For adapter there is no need to define the port, it is just presented to the beginning of the pipe
 * \param [in] unit - Unit #
 * \param [in] src_port - src port for CPU packet
 * \param [in] packet_handle - packet control handle
 * \param [in] flags - misc flags controlling packet tx
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \remark
 *   Packet control structure may be used multiple times, until diag_sand_packet_free is called,
 *   useful to have one field modification where others stay constant
 * \see
 */
shr_error_e diag_sand_packet_send(
    int unit,
    bcm_port_t src_port,
    rhhandle_t packet_handle,
    uint32 flags);

/**
 * \brief
 *   Given name of packet-prototype in TestCases.xml, get the value of the first occurrence
 *   of the specified unsigned integer field under specified protocol.
 *   Also get 'length' of packet. Note that if any of protocol/filed pointers is null,
 *   only 'length' is returned.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] packet_n -
 *   char *. Name of packet-prototype within file TestCases.xml
 * \param [in] proto_n -
 *   char *. Name of protocol within packet-prototype 'packet_n'.
 *   If NULL, get only '*length_p'.
 * \param [in] field_n -
 *   char *. Name of field within protocol within packet-prototype 'packet_n'.
 *   If NULL, get only '*length_p'.
 * \param [out] lenght_p -
 *   Pointer to uint32. This procedure loads pointed memory by the length of the packet, including
 *   PTCH (but not including CRC). If NULL then no data is retrieved.
 * \param [out] uint32_field_val_p -
 *   Pointer to uint32. This procedure loads pointed memory by the integer value corresponding to the
 *   value specified within the given name of packet-prototype.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    If any of the specified inputs does not exist (e.g., no such 'packet_n') then a 'not found'
 *    error is returned.
 * \see
 *    None
 */
shr_error_e diag_sand_packet_proto_get_uint_field(
    int unit,
    char *packet_n,
    char *proto_n,
    char *field_n,
    uint32 *lenght_p,
    uint32 *uint32_field_val_p);
/**
 * \brief
 *    Create the packet and assign all the available parameters from the XML case
 * \param [in] unit - Unit #
 * \param [in] packet_n - packet case name from XML file
 * \param [in] packet_h - pointer to packet handle, if provided it is not free by send and may be used further,
 *                          should be freed by caller
 * \param [out] src_port_p - pointer where source port should be assigned
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \remark
 *     XML case may be added/modified on the fly without need for restart, each invocation looks in actual file
 * \see
 */
shr_error_e diag_sand_packet_load(
    int unit,
    char *packet_n,
    rhhandle_t packet_h,
    bcm_port_t * src_port_p);

/**
 * \brief
 *    Send the packet defined by XML packet case
 * \param [in] unit - Unit #
 * \param [in] packet_n - packet case name from XML file
 * \param [in] packet_h_p - pointer to packet handle, if provided it is not free by send and may be used further,
 *                          should be freed by caller
 * \param [in] flags - misc flags controlling packet tx
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \remark
 *     XML case may be added/modified on the fly without need for restart, each invocation looks in actual file
 * \see
 */
shr_error_e diag_sand_packet_send_by_case(
    int unit,
    char *packet_n,
    rhhandle_t * packet_h_p,
    uint32 flags);

/**
 * \brief
 *    Send simple packet only to allow meaningful output for different visibility commands tests
 * \param [in] unit - Unit #
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 */
shr_error_e diag_sand_packet_send_simple(
    int unit);

/**
 * \brief
 *   Prints the content of last packet sent on specific unit/core
 * \param [in] unit - Unit #
 * \param [in] core - Core #
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_sand_packet_last_show(
    int unit,
    int core);

/**
 * \brief
 *   Prints the information tree after parsing
 * \param [in] unit - Unit #
 * \param [in] pit - Parsed information tree holding all the data to be printed
 * \param [in] sand_control - Control structure for DNX shell framework
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e packet_decode_info_print(
    int unit,
    rhlist_t * pit,
    sh_sand_control_t * sand_control);

/*
 * }
 * PParse defines and macros
 */

/*
 * Return number of packet in rx list
 */
int diag_sand_rx_nof(
    int unit);

/*
 * Start recording received packets into rx list
 */
shr_error_e diag_sand_rx_start(
    int unit);

/*
 * Stop recording received packets into rx list
 */
shr_error_e diag_sand_rx_stop(
    int unit);

/*
 * Clear all packets from rx list
 */
shr_error_e diag_sand_rx_clean(
    int unit);

/*
 * Dump all packets from rx list on the screen
 */
shr_error_e diag_sand_rx_dump(
    int unit,
    sh_sand_control_t * sand_control);

/*
 * Compare packet constructed as the the one for tx with the packet in rx list and return number of matches
 */
shr_error_e diag_sand_rx_compare(
    int unit,
    rhhandle_t packet_handle,
    int *pkt_match_count_p);

/*
 * Definitions for legacy command list in DPP only
 * {
 */
cmd_result_t cmd_sand_packet(
    int unit,
    args_t * args);

void diag_sand_packet_flags_set(
    int unit,
    rhhandle_t * packet_h_p,
    uint32 flags);

extern const char cmd_sand_packet_usage[];
/*
 * }
 */

/*
 * Definitions for shell framework
 */
extern sh_sand_man_t sh_sand_packet_man;
extern sh_sand_cmd_t sh_sand_packet_cmds[];

extern char *example_data_string;

#endif /* DIAG_SAND_PACKET_INCLUDED */
