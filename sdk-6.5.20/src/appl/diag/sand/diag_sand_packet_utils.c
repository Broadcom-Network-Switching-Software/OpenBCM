/**
 * \file diag_sand_packet_utils.c
 *
 * Implementation of diag command 'dnx packet'.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
 /*
  * sal
  */
#include <stdlib.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*
 * bcm
 */
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/vlan.h>
#include <bcm/switch.h>
#include <bcm/vswitch.h>
#include <bcm/pkt.h>
#include <bcm/error.h>
#include <bcm/tx.h>
#include <bcm/rx.h>
#include <bcm/sat.h>
#include <bcm/field.h>
/*
 * bcm_int
 */
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/port_sw_db.h>
#endif
/*
 * appl
 */
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*
 * soc
 */
#include <soc/drv.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_common.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif
/*
 * }
 */

#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
/** Size of module header on DNX devices, it should be written at the start of the packet */
#define DNX_MODULE_HEADER_SIZE         16
#else
/** No Module Header on ADAPTER and non DNX*/
#define DNX_MODULE_HEADER_SIZE         0
#endif

/*
 * GLOBAL DATA
 * {
 */

char *example_data_string =
    "0x0000000000010000000000028100000108004500002e0000000040ff79d20000000000000000000102030405060708090a0b0c0d0e0f1011";

static rhlist_t *rx_pkt_list = NULL;

static bcm_rx_t
dnx_sand_packet_receive(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
    int i;
    pparse_packet_info_t *packet_info;
    rhhandle_t temp = NULL;
    int up32_size;
    parsing_db_t pdb;

    LOG_CLI(("Packet with size:%d received with dest GPORT:0x%08x\n", pkt->tot_len, pkt->dst_gport));

    up32_size = UTILEX_DIV_ROUND_UP(pkt->pkt_len, 4);

    if (utilex_rhlist_entry_add_tail(rx_pkt_list, "ETH", RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
    {
        LOG_CLI(("Failed to add packet info to the list\n"));
        goto exit;
    }
    packet_info = (pparse_packet_info_t *) temp;
    packet_info->data_desc = "ETH";
    packet_info->data_size = up32_size * 32;
    packet_info->gport = pkt->dst_gport;
    {
        uint32 *src = (uint32 *) pkt->_pkt_data.data;
        for (i = 0; i < up32_size; i++)
        {
            packet_info->data[up32_size - (i + 1)] = bcm_ntohl(src[i]);
        }
    }

    if (pparse_init(unit, &pdb) != _SHR_E_NONE)
    {
        LOG_CLI(("Failed to init parser on rx packet\n"));
    }
    else
    {
        pparse_packet_process(unit, 0, &pdb, packet_info, &packet_info->pit);
    }

exit:
    return 3;
}

int
diag_sand_rx_nof(
    int unit)
{
    int pkt_num;

    if (rx_pkt_list == NULL)
    {
        pkt_num = 0;
    }
    else
    {
        pkt_num = RHLNUM(rx_pkt_list);
    }

    return pkt_num;
}

shr_error_e
diag_sand_rx_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * On stop list will be destroyed - so no leftovers
     */
    if (rx_pkt_list == NULL)
    {
        if ((rx_pkt_list = utilex_rhlist_create("rx_packets", sizeof(pparse_packet_info_t), 0)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_sand_rx_clean(unit));
    }

    SHR_IF_ERR_EXIT(bcm_rx_register(unit, "test", dnx_sand_packet_receive, 1, (void *) rx_pkt_list, BCM_RCO_F_ALL_COS));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_rx_clean(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (rx_pkt_list != NULL)
    {
        pparse_packet_info_t *pkt_entry;
        RHITERATOR(pkt_entry, rx_pkt_list)
        {
            pparse_parsed_info_tree_free(unit, pkt_entry->pit);
        }
        utilex_rhlist_clean(rx_pkt_list);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_rx_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Wait a little for other threads to process
     */
    sal_usleep(50000);

    bcm_rx_unregister(unit, dnx_sand_packet_receive, 1);

    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_rx_dump(
    int unit,
    sh_sand_control_t * sand_control)
{
    pparse_packet_info_t *pkt_entry;

    SHR_FUNC_INIT_VARS(unit);

    RHITERATOR(pkt_entry, rx_pkt_list)
    {
        SHR_IF_ERR_EXIT(packet_decode_info_print(unit, pkt_entry->pit, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_rx_compare_field(
    int unit,
    char *field_in_n,
    rhlist_t * field_list,
    char *print_value,
    uint32 *value)
{
    uint32 realtokens = 0;
    char **tokens = NULL;
    signal_output_t *field_cur;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * In processed packet field appears without its proto
     */
    if ((tokens = utilex_str_split(field_in_n, ".", 2, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Memory allocation failed on splitting of input field\"%s\"\n", field_in_n);
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    RHITERATOR(field_cur, field_list)
    {
        if ((sal_strcasecmp(tokens[0], RHNAME(field_cur)) == 0) ||
            (sal_strcasecmp(tokens[0], field_cur->expansion) == 0))
        {
            if (realtokens == 1)
            {
                /*
                 * We reach last field in chain, compare values - either string or binary
                 */
                /*
                 * If there was input string compare print string
                 */
                if (!ISEMPTY(print_value))
                {
                    if (sal_strcasecmp(print_value, field_cur->print_value))
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "In Field value:%s do not match packet:%s\n",
                                     print_value, field_cur->print_value);
                    }
                }
                else if (sal_memcmp(value, field_cur->value, BITS2WORDS(field_cur->size) * 4))
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "In Field:%s value:%u do not match packet:%u\n",
                                 field_in_n, *value, *field_cur->value);
                }
                /*
                 * We found the field and it matches the value
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            }
            else
            {
                /*
                 * We need to dig deeper
                 */
                SHR_SET_CURRENT_ERR(diag_sand_rx_compare_field
                                    (unit, tokens[1], field_cur->field_list, print_value, value));
            }
            break;
        }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_rx_compare(
    int unit,
    rhhandle_t packet_h,
    int *pkt_match_count_p)
{
    pparse_packet_info_t *pkt_entry;
    int pkt_match_count = 0;
    rhlist_t *proto_list_in;
    uint32 realtokens = 0;
    char **tokens = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt_match_count_p, _SHR_E_PARAM, "pkt_match_count_p");
    SHR_NULL_CHECK(packet_h, _SHR_E_PARAM, "packet_h_p");

    proto_list_in = ((sand_packet_t *) packet_h)->proto_list;

    RHITERATOR(pkt_entry, rx_pkt_list)
    {
        signal_output_t *proto_next_pkt, *proto_next_in, *field_in;

        if (pkt_entry->pit == NULL)
            continue;

        /*
         * Verify that all protos present in input packet exist in current one
         */
        RHITERATOR(proto_next_in, proto_list_in)
        {
            RHITERATOR(proto_next_pkt, pkt_entry->pit)
            {
                if (sal_strcasecmp(RHNAME(proto_next_in), RHNAME(proto_next_pkt)) == 0)
                {
                    /*
                     * We found the proto - now lets find the field
                     */
                    RHITERATOR(field_in, proto_next_in->field_list)
                    {
                        /*
                         * In processed packet field appears without its proto
                         */
                        if ((tokens = utilex_str_split(RHNAME(field_in), ".", 2, &realtokens)) == NULL)
                        {
                            SHR_CLI_EXIT(_SHR_E_MEMORY, "Memory allocation failed on splitting of input field\"%s\"\n",
                                         RHNAME(field_in));
                        }

                        SHR_SET_CURRENT_ERR(diag_sand_rx_compare_field(unit, tokens[1], proto_next_pkt->field_list,
                                                                       field_in->print_value, field_in->value));
                        utilex_str_split_free(tokens, realtokens);
                        tokens = NULL;
                        /*
                         * Field is requested to be found, check if there was a problem with it - no need to continue
                         */
                        if (SHR_FUNC_ERR())
                            break;
                    }
                    /*
                     * We have found the proto and verified the field list -
                     * no need to continue on protos of cur packet
                     */
                    break;
                }
            }
            /*
             * We have not found the proto or there was a problem with fields
             */
            if (proto_next_pkt == NULL)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }

            if (SHR_FUNC_ERR())
                break;
        }
        /*
         * End of rx single packet processing - no error means that it was a match
         */
        if (!SHR_FUNC_ERR())
        {
            pkt_match_count++;
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }

    *pkt_match_count_p = pkt_match_count;

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_packet_parse_data(
    int unit,
    char *packet_data,
    uint8 **target_p,
    int *length_p)
{
    uint8 *p;
    char tmp, data_iter;
    int i, data_len, pkt_len, data_base;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If string data starts with 0x or 0X, skip it
     */
    if ((packet_data[0] == '0') && ((packet_data[1] == 'x') || (packet_data[1] == 'X')))
    {
        data_base = 2;
    }
    else
    {
        data_base = 0;
    }

    data_len = strlen(packet_data) - data_base;
    /*
     * Each character represents 1 nibble - each 2 characters byte
     */
    pkt_len = UTILEX_DIV_ROUND_UP(data_len, 2);
    if (pkt_len < 64)
    {
        pkt_len = 64;
    }

    if ((p = soc_cm_salloc(unit, pkt_len, "tx_string_packet")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to allocate memory for packet\n");
    }

    sal_memset(p, 0, pkt_len);

    /*
     * Convert char to value
     */
    i = 0;
    while (i < data_len)
    {
        data_iter = packet_data[data_base + i];
        if (('0' <= data_iter) && (data_iter <= '9'))
        {
            tmp = data_iter - '0';
        }
        else if (('a' <= data_iter) && (data_iter <= 'f'))
        {
            tmp = data_iter - 'a' + 10;
        }
        else if (('A' <= data_iter) && (data_iter <= 'F'))
        {
            tmp = data_iter - 'A' + 10;
        }
        else if (0 == data_iter)
        {
            /*
             * pad by zeroes in case of input string is not enough for ETH packet
             */
            tmp = 0;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Unexpected char: %c\n", data_iter);
        }

        /*
         * String input is in 4b unit. Below we're filling in 8b:
         * offset is /2, and we shift by 4b if the input char is odd
         */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
    }

    *length_p = pkt_len;
    *target_p = p;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_packet_generate(
    int unit,
    rhhandle_t packet_h,
    bcm_pkt_t * bcm_pkt)
{
    uint32 data_size = 0;
    char data_string[DSIG_MAX_SIZE_STR];
    uint32 data[DSIG_MAX_SIZE_UINT32];
    uint8 *pkt_data = NULL;
    int pkt_data_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(pparse_compose(unit, PACKET_PROTO_LIST(packet_h), PACKET_SIZE(packet_h),
                                   DSIG_MAX_SIZE_BITS, data, &data_size));

    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, data, data_string, data_size, PRINT_BIG_ENDIAN);

    ((sand_packet_t *) packet_h)->header_size = data_size;

    SHR_IF_ERR_EXIT(diag_sand_packet_parse_data(unit, data_string, &pkt_data, &pkt_data_size));

    if ((bcm_pkt->_pkt_data.data = (uint8 *) soc_cm_salloc(unit, pkt_data_size, "tx")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "packet memory allocation failed\n");
    }

    bcm_pkt->pkt_data = &bcm_pkt->_pkt_data;
    bcm_pkt->blk_count = 1;
    bcm_pkt->_pkt_data.len = pkt_data_size;

    /*
     * Data was read from file or string. Copy into packet
     */
    sal_memcpy(bcm_pkt->pkt_data[0].data, pkt_data, pkt_data_size);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_tx(
    int unit,
    bcm_port_t src_port,
    int packet_length,
    char *data_string,
    uint32 flags)
{
    uint8 *pkt_data = NULL;
    int data_size = 0;
    bcm_pkt_t pkt_info;
    int vlan_present;
    enet_hdr_t *ep;
    int cpu_channel = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&pkt_info, 0, sizeof(pkt_info));

    SHR_IF_ERR_EXIT(diag_sand_packet_parse_data(unit, data_string, &pkt_data, &data_size));

    /*
     * Default packet length will be just headers size
     */
    if (packet_length == 0)
    {
        packet_length = data_size;
    }
    else if (packet_length < data_size)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Full packet length:%d is less then header size:%d\n", packet_length, data_size);
    }

    pkt_info.unit = unit;
    if ((pkt_info._pkt_data.data = (uint8 *) soc_cm_salloc(unit, packet_length + DNX_MODULE_HEADER_SIZE, "tx")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "packet memory allocation failed\n");
    }

    pkt_info.pkt_data = &pkt_info._pkt_data;
    pkt_info.blk_count = 1;
    pkt_info._pkt_data.len = packet_length + DNX_MODULE_HEADER_SIZE;

    if (!(flags & SAND_PACKET_RESUME))
    {
        pkt_info.flags |= BCM_TX_NO_VISIBILITY_RESUME;
    }

    sal_memset(pkt_info.pkt_data[0].data, 0, pkt_info.pkt_data[0].len);
    /*
     * Data was read from file or string.  Copy into pkt_info
     */
    sal_memcpy(pkt_info.pkt_data[0].data + DNX_MODULE_HEADER_SIZE, pkt_data, data_size);

    ep = (enet_hdr_t *) BCM_PKT_IEEE(&pkt_info);

    if (!ENET_TAGGED(ep))
    {
        vlan_present = FALSE;
    }
    else
    {
        vlan_present = TRUE;
    }

    /*
     * Setup the packet
     */
    pkt_info.flags |= BCM_TX_CRC_APPEND;
    if (data_size < (vlan_present != 0 ? TAGGED_PACKET_LENGTH : UNTAGGED_PACKET_LENGTH))
    {
        pkt_info.flags |= BCM_TX_NO_PAD;
    }

    pkt_info.flags |= BCM_PKT_F_HGHDR;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &cpu_channel));
    }
#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_JERICHO(unit))
    {
        int soc_sand_rv = soc_port_sw_db_channel_get(unit, src_port, (uint32 *) &cpu_channel);
        if (BCM_FAILURE(handle_sand_result(soc_sand_rv)))
        {
            return (CMD_FAIL);
        }
    }
#endif
    pkt_info._dpp_hdr[0] = cpu_channel;

#ifdef ADAPTER_SERVER_MODE
    pkt_info.src_gport = src_port;
#endif

    if (flags & SAND_PACKET_RX)
    {
        SHR_IF_ERR_EXIT(diag_sand_rx_start(unit));
    }
    if (bcm_tx(unit, &pkt_info, NULL) != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "bcm_tx failed\n");
    }
    if (flags & SAND_PACKET_RX)
    {
#ifdef ADAPTER_SERVER_MODE
        /** In case bcm TX is called with RX for adapter there is a need to wait for rx */
        sal_usleep(20000000);
#endif
        SHR_IF_ERR_EXIT(diag_sand_rx_stop(unit));
    }

#ifdef ADAPTER_SERVER_MODE
    /*
     * Special case: In case bcm TX is called and RX isnt
     * There is a need to wait for adapter to complete packet process
     */
    if (!(flags & SAND_PACKET_RX))
    {
        sal_usleep(20000000);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_create(
    int unit,
    rhhandle_t * packet_h_p)
{
    sand_packet_t *sand_packet;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_h_p, _SHR_E_PARAM, "packet_h_p");

    if ((sand_packet = sal_alloc(sizeof(sand_packet_t), "Sand Packet")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for sand packet\n");
    }

    sal_memset(sand_packet, 0, sizeof(sand_packet_t));

    sand_packet->core_id = -1;
    sand_packet->packet_size = SAND_PACKET_DEFAULT_SIZE_BYTES;

    sand_packet->flags = 0;
    sand_packet->stream.gtf_id = -1;
    sand_packet->stream.rate = 1000;
    sand_packet->stream.burst = 1000;
    sand_packet->stream.granularity = SAT_GRANULARITY_SET_BY_DIAG;
    sand_packet->stream.duration = 1;
    sand_packet->stream.rate_unit = BCM_SAT_GTF_RATE_IN_BYTES;

    sand_packet->collector.ctf_id = -1;

    if ((sand_packet->proto_list = utilex_rhlist_create("packet", sizeof(signal_output_t), 0)) == NULL)
    {
        sal_free(sand_packet);
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for headers list\n");
    }

    *packet_h_p = (rhhandle_t) sand_packet;
exit:
    SHR_FUNC_EXIT;
}

void
diag_sand_packet_flags_set(
    int unit,
    rhhandle_t * packet_h_p,
    uint32 flags)
{
    sand_packet_t *sand_packet = (sand_packet_t *) packet_h_p;

    SHR_FUNC_INIT_VARS(unit);
    sand_packet->flags = flags;

    *packet_h_p = (rhhandle_t) sand_packet;
    SHR_VOID_FUNC_EXIT;
}
shr_error_e
diag_sand_packet_proto_add(
    int unit,
    rhhandle_t packet_h,
    char *header_name)
{
    rhhandle_t header_handle;
    rhlist_t *proto_list;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_h, _SHR_E_PARAM, "packet_h_p");

    proto_list = ((sand_packet_t *) packet_h)->proto_list;

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(proto_list, header_name, RHID_TO_BE_GENERATED, &header_handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_proto_exists(
    int unit,
    rhhandle_t packet_h,
    char *header_name)
{
    SHR_FUNC_INIT_VARS(unit);

    if (utilex_rhlist_entry_get_by_name(((sand_packet_t *) packet_h)->proto_list, header_name) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_field_add_str(
    int unit,
    rhhandle_t packet_h,
    char *field_name,
    char *str_value)
{
    signal_output_t *header_h;
    rhhandle_t field_h;
    int str_length;
    uint32 realtokens = 0;
    char **tokens = NULL;
    rhlist_t *proto_list;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_h, _SHR_E_PARAM, "packet_h_p");

    proto_list = ((sand_packet_t *) packet_h)->proto_list;

    /*
     * We need to verify that there is proto this field relates to was already added
     */
    if ((tokens = utilex_str_split(field_name, ".", 2, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Memory allocation failed on splitting of input field\"%s\"\n", field_name);
    }
    /*
     * Check that there are indeed 2 tokens
     */
    if (realtokens != 2)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "%s does not match HeaderName.FieldName paradigm\n", field_name);
    }
    /*
     * Check for header in packet - starting from tail
     */
    if ((header_h = (signal_output_t *) utilex_rhlist_entry_get_last_by_name(proto_list, tokens[0])) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "There is no %s header in packet\n", tokens[0]);
    }

    if (((str_length = sal_strlen(str_value)) >= DSIG_MAX_SIZE_STR - 1))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Input value string is too long:%d\n", str_length);
    }

    /*
     * Create field list if not exists already
     */
    if (header_h->field_list == NULL)
    {
        if ((header_h->field_list = utilex_rhlist_create("fields", sizeof(signal_output_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for field list of header '%s'.\n", RHNAME(header_h));
        }
    }
    /*
     * First check if such field was already added, positive answer means that request is to replace the value
     * On negative we must add the field to the list
     */
    if ((field_h = utilex_rhlist_entry_get_by_name(header_h->field_list, field_name)) == NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header_h->field_list, field_name, RHID_TO_BE_GENERATED, &field_h));
    }
    /*
     * + 1 for null termination, because there is a previous value in the string
     */
    sal_strncpy(((signal_output_t *) field_h)->print_value, str_value, str_length + 1);

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_field_add_uint32(
    int unit,
    rhhandle_t packet_h,
    char *field_name,
    uint32 *uint32_value,
    int bit_size)
{
    signal_output_t *header_h;
    rhhandle_t field_h;
    int uint32_size;
    uint32 realtokens = 0;
    char **tokens = NULL;
    rhlist_t *proto_list;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_h, _SHR_E_PARAM, "packet_h_p");

    proto_list = ((sand_packet_t *) packet_h)->proto_list;

    /*
     * We need to verify that there is proto this field relates to was already added
     */
    if ((tokens = utilex_str_split(field_name, ".", 2, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Memory allocation failed on splitting of input field\"%s\"\n", field_name);
    }
    /*
     * Check that there are indeed 2 tokens
     */
    if (realtokens != 2)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "%s does not match HeaderName.FieldName paradigm\n", field_name);
    }
    /*
     * Check for header in packet - starting from tail
     */
    if ((header_h = (signal_output_t *) utilex_rhlist_entry_get_last_by_name(proto_list, tokens[0])) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "There is no %s header in packet\n", tokens[0]);
    }

    if ((uint32_size = BITS2WORDS(bit_size)) > DSIG_MAX_SIZE_UINT32)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Input value size is too long:%d\n", uint32_size);
    }

    /*
     * Create field list if not exists already
     */
    if (header_h->field_list == NULL)
    {
        if ((header_h->field_list = utilex_rhlist_create("fields", sizeof(signal_output_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for field list of header '%s'.\n", RHNAME(header_h));
        }
    }
    /*
     * First check if such field was already added, positive answer means that request is to replace the value
     * On negative we must add the field to the list
     */
    if ((field_h = utilex_rhlist_entry_get_by_name(header_h->field_list, field_name)) == NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header_h->field_list, field_name, RHID_TO_BE_GENERATED, &field_h));
    }

    sal_memcpy(((signal_output_t *) field_h)->value, uint32_value, uint32_size * 4);
    ((signal_output_t *) field_h)->size = bit_size;
exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

void
diag_sand_packet_free(
    int unit,
    rhhandle_t packet_h)
{
    rhlist_t *proto_list;

    SHR_FUNC_INIT_VARS(unit);

    if (packet_h != NULL)
    {
        sand_packet_t *sand_packet = (sand_packet_t *) packet_h;
        if ((proto_list = sand_packet->proto_list) != NULL)
        {
            signal_output_t *header;
            RHITERATOR(header, proto_list)
            {
                if (header->field_list)
                {
                    utilex_rhlist_free_all(header->field_list);
                    header->field_list = NULL;
                }
            }
            utilex_rhlist_free_all(proto_list);
        }
        if (sand_packet->stream.gtf_id != -1)
        {
            SHR_IF_ERR_EXIT(bcm_sat_gtf_destroy(unit, sand_packet->stream.gtf_id));
        }
        sal_free(packet_h);
    }

exit:
    SHR_VOID_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_sat_rx_get_stats(
    int unit,
    int ctf_id,
    uint64 *rcv_packet_count_p,
    uint64 *err_packet_count_p,
    uint64 *ooo_packet_count_p)
{
    bcm_sat_ctf_stat_t stats;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_sat_ctf_stat_get(unit, ctf_id, 0, &stats));

    COMPILER_64_COPY(*rcv_packet_count_p, stats.received_packet_cnt);
    COMPILER_64_COPY(*err_packet_count_p, stats.err_packet_cnt);
    COMPILER_64_COPY(*ooo_packet_count_p, stats.out_of_order_packet_cnt);

exit:
    SHR_FUNC_EXIT;
}

typedef struct
{
    int sat_trap_id;
    bcm_gport_t sat_trap_gport;
    bcm_field_group_t fg_id;
    bcm_field_context_t context_id;
    bcm_field_presel_t presel_id;
} sh_pkt_sat_rx_info_t;

static sh_pkt_sat_rx_info_t pkt_sat_rx_info = { -1, 0, -1, 0, -1 };

static shr_error_e
diag_sand_packet_sat_trap_create(
    int unit,
    uint32 out_port)
{
    bcm_rx_trap_config_t trap_config;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create a trap for SAT - it will be enough to have one global trap_id for all SAT streams/collectors
     */
    if (pkt_sat_rx_info.sat_trap_id != -1)
    {
        /** We already have created trap - just leave peacefully */
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &pkt_sat_rx_info.sat_trap_id));

    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    trap_config.trap_strength = 0;
    trap_config.dest_port = out_port;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, pkt_sat_rx_info.sat_trap_id, &trap_config));
    /*
     * We need gport for PMF rules
     */
    BCM_GPORT_TRAP_SET(pkt_sat_rx_info.sat_trap_gport, pkt_sat_rx_info.sat_trap_id, 7, 0);

    SHR_IF_ERR_EXIT(bcm_sat_ctf_trap_add(unit, pkt_sat_rx_info.sat_trap_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_packet_collector_create(
    int unit,
    rhhandle_t packet_h)
{
    bcm_sat_ctf_packet_info_t packet_info;
    bcm_sat_ctf_identifier_t identifier;
    bcm_sat_ctf_stat_config_t stat_config;
    sand_packet_t *sand_packet = (sand_packet_t *) packet_h;
    uint32 session_id;
    uint8 payload_pattern[BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create only if it was not created already
     */
    if (sand_packet->collector.ctf_id == -1)
    {
        SHR_IF_ERR_EXIT(bcm_sat_ctf_create(unit, 0, &sand_packet->collector.ctf_id));
    }
    /*
     * Configure packet
     */
    bcm_sat_ctf_packet_info_t_init(&packet_info);
    packet_info.sat_header_type = bcmSatHeaderUserDefined;
    packet_info.payload.payload_type = bcmSatPayloadPRBS;
    sal_memcpy(packet_info.payload.payload_pattern, payload_pattern, BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);

    packet_info.offsets.payload_offset = sand_packet->header_size;
    packet_info.offsets.seq_number_offset = 0;
    packet_info.offsets.timestamp_offset = 0;   /** Currently redundant - for future use */
    packet_info.offsets.crc_byte_offset = 0;    /** Currently redundant - for future use */
    packet_info.flags = 0;                      /** Currently redundant - for future use */
    SHR_IF_ERR_EXIT(bcm_sat_ctf_packet_config(unit, sand_packet->collector.ctf_id, &packet_info));
    /*
     * Use ctf_id as trap_qualifier to connect it to session_id
     * Make session id the same number as ctf
     */
    session_id = sand_packet->collector.ctf_id;
    SHR_IF_ERR_EXIT(bcm_sat_ctf_trap_data_to_session_map(unit, sand_packet->collector.ctf_id, 0, session_id));
    /*
     * Connect Session ID to collector
     */
    identifier.session_id = session_id;
    identifier.trap_id = 0;
    identifier.color = 0;
    identifier.tc = 0;
    SHR_IF_ERR_EXIT(bcm_sat_ctf_identifier_map(unit, &identifier, sand_packet->collector.ctf_id));
    /*
     * CTF Statistics configuration
     */
    bcm_sat_ctf_stat_config_t_init(&stat_config);
    stat_config.update_counters_in_unvavail_state = 1;

    SHR_IF_ERR_EXIT(bcm_sat_ctf_stat_config_set(unit, sand_packet->collector.ctf_id, &stat_config));
exit:
    SHR_FUNC_EXIT;
}

static rhlist_t *rx_sat_list = NULL;

static shr_error_e
diag_sand_packet_sat_rx_add(
    int unit,
    rhhandle_t packet_h)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add packet to the list
     */
    if (rx_sat_list == NULL)
    {
        if ((rx_sat_list = utilex_rhlist_create("rx_packets", sizeof(sand_packet_t), 0)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
        }
    }

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_append(rx_sat_list, packet_h));

exit:
    SHR_FUNC_EXIT;
}

typedef struct
{
    char *name;
    bcm_field_qualify_t qual_id;
    bcm_field_input_types_t qual_type;
    int qual_arg;
    int offset;
    uint32 mask;
} sh_pkt_qualifier_t;

sh_pkt_qualifier_t pkt_qualifiers[] = {
    {"ETH1.VLAN.VID", bcmFieldQualifyVlanId, bcmFieldInputTypeLayerAbsolute, 0, 96, 0xFFF},
    {"ETH1.Type", bcmFieldQualifyTpid, bcmFieldInputTypeLayerAbsolute, 0, 0, 0xFFFF},
    {NULL},
};

static sh_pkt_qualifier_t *
diag_sand_packet_get_qual_info(
    char *qualifier_name)
{
    sh_pkt_qualifier_t *pkt_qualifier = &pkt_qualifiers[0];

    for (; pkt_qualifier->name != NULL; pkt_qualifier++)
    {
        if (!sal_strcasecmp(pkt_qualifier->name, qualifier_name))
        {
            return pkt_qualifier;
        }
    }
    return NULL;
}

shr_error_e
diag_sand_packet_sat_rx_start(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    sand_packet_t *sand_packet;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * IF list is null no ctf is required
     */
    if (pkt_sat_rx_info.fg_id >= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SAT Rx already started, to restart - destroy the current");
    }
    if (rx_sat_list == NULL)
    {
        SHR_EXIT();
    }
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy((char *) context_info.name, "SAT_ENTRANCE", BCM_FIELD_MAX_SHORT_NAME_LEN);
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info,
                                             &pkt_sat_rx_info.context_id));

    /*
     * Create presel entry to map relevant traffic to the context
     */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /*
     * Choice of 12 here is random - may be any free entry if in preselect TCAM
     */
    presel_entry_id.presel_id = pkt_sat_rx_info.presel_id = 8;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = pkt_sat_rx_info.context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
    presel_entry_data.qual_data[0].qual_value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    presel_entry_data.qual_data[0].qual_mask = 0;
    presel_entry_data.qual_data[0].qual_arg = 0;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*
     * Now go through all packets in list and generate the field group with all fields mentioned inside
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy((char *) fg_info.name, "SAT_PACKETS", BCM_FIELD_MAX_SHORT_NAME_LEN);
    RHITERATOR(sand_packet, rx_sat_list)
    {
        signal_output_t *proto, *field_in;
        /*
         * Iterate on protos
         */
        RHITERATOR(proto, sand_packet->proto_list)
        {
            /*
             * Now take each field and add it to the group
             */
            RHITERATOR(field_in, proto->field_list)
            {
                int i_q;
                sh_pkt_qualifier_t *pkt_qual_info;
                /*
                 * Fetch BCM qualifier for the field
                 */
                if ((pkt_qual_info = diag_sand_packet_get_qual_info(RHNAME(field_in))) == NULL)
                {
                    /*
                     * Qualifier not found - Let user know and continue
                     */
                    LOG_CLI(("Field:%s does not have bcm qualifier attached\n", RHNAME(field_in)));
                    continue;
                }
                /*
                 * Add qualifier to the group if not already on it
                 */
                for (i_q = 0; i_q < fg_info.nof_quals; i_q++)
                {
                    if (pkt_qual_info->qual_id == fg_info.qual_types[i_q])
                        break;
                }
                /*
                 * No match n existing add new
                 */
                if (i_q == fg_info.nof_quals)
                {
                    if (fg_info.nof_quals == BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
                    {
                        SHR_CLI_EXIT(_SHR_E_EXISTS, "Too much qualifiers:%d for SAT Trap\n", fg_info.nof_quals);
                    }
                    fg_info.qual_types[fg_info.nof_quals++] = pkt_qual_info->qual_id;
                    attach_info.key_info.qual_types[attach_info.key_info.nof_quals] = pkt_qual_info->qual_id;
                    attach_info.key_info.qual_info[attach_info.key_info.nof_quals].input_type =
                        pkt_qual_info->qual_type;
                    attach_info.key_info.qual_info[attach_info.key_info.nof_quals].input_arg = pkt_qual_info->qual_arg;
                    attach_info.key_info.qual_info[attach_info.key_info.nof_quals].offset = pkt_qual_info->offset;
                    attach_info.key_info.nof_quals++;
                }
            }
        }
    }

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrap;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &pkt_sat_rx_info.fg_id));
    /*
     * Attach the FG to context
     */
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, pkt_sat_rx_info.fg_id, pkt_sat_rx_info.context_id,
                                                   &attach_info));
    /*
     * Now add entries
     */
    RHITERATOR(sand_packet, rx_sat_list)
    {
        bcm_field_entry_qual_t *entry_qual;
        signal_output_t *proto, *field_in;

        bcm_field_entry_info_t_init(&ent_info);

        ent_info.priority = 1;
        ent_info.nof_entry_quals = 0;
        /*
         * Iterate on protos
         */
        RHITERATOR(proto, sand_packet->proto_list)
        {
            /*
             * Now take each field and add it to the group
             */
            RHITERATOR(field_in, proto->field_list)
            {
                sh_pkt_qualifier_t *pkt_qual_info;
                /*
                 * Fetch BCM qualifier for the field
                 */
                if ((pkt_qual_info = diag_sand_packet_get_qual_info(RHNAME(field_in))) == NULL)
                {
                    /*
                     * Qualifier not found - Let user know and continue
                     */
                    LOG_CLI(("Field:%s does not have bcm qualifier attached\n", RHNAME(field_in)));
                    continue;
                }
                entry_qual = &ent_info.entry_qual[ent_info.nof_entry_quals];

                entry_qual->type = pkt_qual_info->qual_id;
                entry_qual->value[0] = field_in->value[0];
                entry_qual->mask[0] = pkt_qual_info->mask;

                ent_info.nof_entry_quals++;
            }
        }

        if (pkt_sat_rx_info.sat_trap_id == -1)
        {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Rx Trap was not configured for SAT rx")}
        ent_info.nof_entry_actions = 1;
        ent_info.entry_action[0].type = bcmFieldActionTrap;
        /** We need to write the LSB of the drop action.*/
        ent_info.entry_action[0].value[0] = pkt_sat_rx_info.sat_trap_gport;
        /*
         * Trap qualifier is just ctf id
         */
        ent_info.entry_action[0].value[1] = sand_packet->collector.ctf_id;
        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, pkt_sat_rx_info.fg_id, &ent_info,
                                            &sand_packet->collector.entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_sat_rx_destroy(
    int unit)
{
    sand_packet_t *sand_packet;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_sat_ctf_identifier_t identifier;

    SHR_FUNC_INIT_VARS(unit);
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = pkt_sat_rx_info.presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = pkt_sat_rx_info.context_id;
    presel_entry_data.entry_valid = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, pkt_sat_rx_info.fg_id, pkt_sat_rx_info.context_id));

    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, pkt_sat_rx_info.context_id));

    {
        RHSAFE_ITERATOR(sand_packet, rx_sat_list)
        {
            SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, pkt_sat_rx_info.fg_id, NULL, sand_packet->collector.entry_id));

            identifier.session_id = sand_packet->collector.ctf_id;
            identifier.trap_id = 0;
            identifier.color = 0;
            identifier.tc = 0;

            SHR_IF_ERR_EXIT(bcm_sat_ctf_identifier_unmap(unit, &identifier));
            SHR_IF_ERR_EXIT(bcm_sat_ctf_trap_data_to_session_unmap(unit, sand_packet->collector.ctf_id, 0));
            SHR_IF_ERR_EXIT(bcm_sat_ctf_destroy(unit, sand_packet->collector.ctf_id));
            SHR_IF_ERR_EXIT(utilex_rhlist_entry_remove(rx_sat_list, sand_packet));
            diag_sand_packet_free(unit, sand_packet);
        }

    }
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, pkt_sat_rx_info.fg_id));

    pkt_sat_rx_info.fg_id = -1;
    pkt_sat_rx_info.context_id = 0;
    /*
     * Rx Trap is not deleted - may be reused - consider deleting it as well.
     */
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_sat_rx_create(
    int unit,
    rhhandle_t packet_h,
    uint32 out_port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create TRAP
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_trap_create(unit, out_port));
    /*
     * First create collector
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_collector_create(unit, packet_h));
    /*
     * Connect packet fields to the trap qualifier
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_sat_rx_add(unit, packet_h));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_packet_stream_start(
    int unit,
    rhhandle_t packet_h)
{
    int i_b;
    bcm_sat_gtf_bandwidth_t bw;
    uint8 gtf_payload_pattern[BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 };
    bcm_sat_gtf_rate_pattern_t rate_pattern;
    bcm_sat_gtf_packet_config_t packet_config;
    sand_packet_t *sand_packet;

    SHR_FUNC_INIT_VARS(unit);

    sand_packet = (sand_packet_t *) packet_h;

    /*
     * Create only if it was not created already
     */
    if (sand_packet->stream.gtf_id == -1)
    {
        bcm_sat_gtf_stat_multi_t stats;

        SHR_IF_ERR_EXIT(bcm_sat_gtf_create(unit, 0, &sand_packet->stream.gtf_id));
        /*
         * Read the value to ensure nullifying
         */
        SHR_IF_ERR_EXIT(bcm_sat_gtf_stat_multi_get(unit, sand_packet->stream.gtf_id, &stats));
    }

    if (sand_packet->stream.granularity == SAT_GRANULARITY_SET_BY_DIAG)
    {
        if (sand_packet->stream.rate < 1000000)
        {
            sand_packet->stream.granularity = sand_packet->stream.rate;
        }
        else
        {
            sand_packet->stream.granularity = 1000000;
        }
    }

    if (sand_packet->stream.running == TRUE)
    {
        SHR_CLI_EXIT(_SHR_E_EXISTS, "SAT Stream:%d is already running\n", sand_packet->stream.gtf_id);
    }

    /*
     * 2. Set Bandwidth
     */
    bw.rate = sand_packet->stream.rate;
    bw.max_burst = sand_packet->stream.burst;
    bw.flags = sand_packet->stream.rate_unit;
    bw.granularity = sand_packet->stream.granularity;
    if (bw.granularity != SAT_GRANULARITY_SET_BY_BCM)
    {
        bw.flags |= BCM_SAT_GTF_RATE_WITH_GRANULARITY;
    }

    SHR_IF_ERR_EXIT(bcm_sat_gtf_bandwidth_set(unit, sand_packet->stream.gtf_id, 0, &bw));

    /*
     * 3. Set Rate Pattern
     */
    rate_pattern.rate_pattern_mode = bcmSatGtfRatePatternContinues;
    rate_pattern.high_threshold = 10;
    rate_pattern.low_threshold = 10;
    rate_pattern.stop_iter_count = 0;
    rate_pattern.stop_burst_count = 0;
    rate_pattern.stop_interval_count = 0;
    rate_pattern.burst_packet_weight = 0;
    rate_pattern.interval_packet_weight = 0;
    rate_pattern.flags = 0;
    SHR_IF_ERR_EXIT(bcm_sat_gtf_rate_pattern_set(unit, sand_packet->stream.gtf_id, 0, &rate_pattern));

    sal_memset(&packet_config, 0, sizeof(bcm_sat_gtf_packet_config_t));

    /*
     * Configure SAT gtf packet
     */
    packet_config.sat_header_type = bcmSatHeaderUserDefined;
    packet_config.payload.payload_type = bcmSatPayloadConstant4Bytes;
    for (i_b = 0; i_b < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i_b++)
    {
        packet_config.payload.payload_pattern[i_b] = gtf_payload_pattern[i_b];
    }

    packet_config.packet_edit[0].packet_length[0] = sand_packet->packet_size;
    packet_config.packet_edit[0].packet_length_pattern[0] = 0;
    packet_config.packet_edit[0].pattern_length = 1;
    packet_config.packet_edit[0].number_of_ctfs = 1;

    packet_config.packet_context_id = sand_packet->core_id;

    /*
     * The SA field is incremented.
     */
    if (sand_packet->flags & SAND_PACKET_STREAM_SA_INCREMENTAL_FIELD)
    {
        packet_config.packet_edit[0].number_of_stamps = 1;
        packet_config.packet_edit[0].stamps[0].stamp_type = bcmSatStampCounter16Bit;
        packet_config.packet_edit[0].stamps[0].field_type = bcmSatStampFieldSrcMac;
        packet_config.packet_edit[0].stamps[0].inc_step = 1;
        packet_config.packet_edit[0].stamps[0].inc_period_packets = 0;
        packet_config.packet_edit[0].stamps[0].offset = 96;
    }
    SHR_IF_ERR_EXIT(diag_sand_packet_generate(unit, packet_h, &packet_config.header_info));
    /** Header Size is resolved only after actual packet generation */
    packet_config.offsets.seq_number_offset = sand_packet->header_size;

    SHR_IF_ERR_EXIT(bcm_sat_gtf_packet_config(unit, sand_packet->stream.gtf_id, &packet_config));

    SHR_IF_ERR_EXIT(bcm_sat_gtf_packet_start(unit, sand_packet->stream.gtf_id, 0));

    sand_packet->stream.running = TRUE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_packet_stream_stop(
    int unit,
    rhhandle_t packet_h)
{
    sand_packet_t *sand_packet;
    SHR_FUNC_INIT_VARS(unit);

    sand_packet = (sand_packet_t *) packet_h;

    if (sand_packet->stream.running == FALSE)
    {
        SHR_CLI_EXIT(_SHR_E_EXISTS, "SAT Stream:%d is not running\n", sand_packet->stream.gtf_id);
    }

    SHR_IF_ERR_EXIT(bcm_sat_gtf_packet_stop(unit, sand_packet->stream.gtf_id, 0));
    sand_packet->stream.running = FALSE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_stream_create(
    int unit,
    int core_id,
    rhhandle_t packet_h,
    int pkt_length,
    int duration,
    int rate_unit,
    int rate,
    int burst,
    int granularity)
{
    sand_packet_t *sand_packet;

    SHR_FUNC_INIT_VARS(unit);

    sand_packet = (sand_packet_t *) packet_h;

    if (core_id != -1)
    {
        sand_packet->core_id = core_id;
    }
    if ((rate_unit == BCM_SAT_GTF_RATE_IN_BYTES) || (rate_unit == BCM_SAT_GTF_RATE_IN_PACKETS))
    {
        sand_packet->stream.rate_unit = rate_unit;
    }
    if (rate != 0)
    {
        sand_packet->stream.rate = rate;
    }
    if (burst != 0)
    {
        sand_packet->stream.burst = burst;
    }
    /*
     * Granularity may be SAT_GRANULARITY_SET_BY_DIAG, SAT_GRANULARITY_SET_BY_BCM or specific number
     */
    sand_packet->stream.granularity = granularity;
    if (duration >= 0)
    {
        sand_packet->stream.duration = duration;
    }
    if (pkt_length)
    {
        sand_packet->packet_size = pkt_length;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_stream_get_stats(
    int unit,
    rhhandle_t packet_h,
    uint64 *bytes_count_p,
    uint64 *packet_count_p)
{
    bcm_sat_gtf_stat_multi_t stats;
    int gtf_id;

    SHR_FUNC_INIT_VARS(unit);

    if ((gtf_id = ((sand_packet_t *) packet_h)->stream.gtf_id) == -1)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Stream was not generated yet, on generation counters will be nullified");
    }
    SHR_IF_ERR_EXIT(bcm_sat_gtf_stat_multi_get(unit, gtf_id, &stats));

    COMPILER_64_COPY(*bytes_count_p, stats.transmitted_byte_cnt);
    COMPILER_64_COPY(*packet_count_p, stats.transmitted_packet_cnt);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_send(
    int unit,
    bcm_port_t src_port,
    rhhandle_t packet_h,
    uint32 flags)
{
    char data_string[DSIG_MAX_SIZE_STR];
    uint32 data[DSIG_MAX_SIZE_UINT32];
    uint32 size = 0;
    sand_packet_t *sand_packet = (sand_packet_t *) packet_h;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_h, _SHR_E_PARAM, "packet_h_p");

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        uint32 pp_port;
        dnx_algo_port_info_s port_info;

        /*
         * STOP flag means just stop
         */
        if (flags & SAND_PACKET_STOP)
        {
            SHR_CLI_EXIT(diag_sand_packet_stream_stop(unit, packet_h), "");
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, src_port, &sand_packet->core_id, &pp_port));

        if (diag_sand_packet_proto_exists(unit, packet_h, "PTCH_2") == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &pp_port, 16));
        }

        if ((flags & SAND_PACKET_SAT) || (DNX_ALGO_PORT_TYPE_IS_SAT(unit, port_info)))
        {
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_start(unit, packet_h));
            /*
             * Do not wait and stop when duration is 0 or negative
             */
            if (sand_packet->stream.duration > 0)
            {
                sal_sleep(sand_packet->stream.duration);
                SHR_IF_ERR_EXIT(diag_sand_packet_stream_stop(unit, packet_h));
            }
            SHR_EXIT();
        }
    }
#endif

    SHR_IF_ERR_EXIT(pparse_compose(unit, sand_packet->proto_list, sand_packet->packet_size,
                                   DSIG_MAX_SIZE_BITS, data, &size));

    /** Non SAT transmit */
    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, data, data_string, size, PRINT_BIG_ENDIAN);

    SHR_IF_ERR_EXIT(diag_sand_packet_tx(unit, src_port, sand_packet->packet_size, data_string, flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_stream_send(
    int unit,
    rhhandle_t * packet_hs,
    int stream_nof)
{
    int time, total_time, loop_counter;
    int i_stream, cur_stream;
    sand_packet_t *sand_packet;

    SHR_FUNC_INIT_VARS(unit);

    for (i_stream = 0; i_stream < stream_nof; i_stream++)
    {
        SHR_NULL_CHECK(packet_hs[i_stream], _SHR_E_PARAM, "packet_h");
    }

    for (i_stream = 0; i_stream < stream_nof; i_stream++)
    {
        /*
         * Granularity can be set explicitly only for the first stream, all the others will need to use the
         * same granularity
         */
        if (i_stream != 0)
        {
            PACKET_GRANULARITY(packet_hs[i_stream]) = SAT_GRANULARITY_SET_BY_BCM;
        }
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_start(unit, packet_hs[i_stream]));
    }

    loop_counter = stream_nof;
    total_time = 0;
    do
    {
        time = SAND_STREAM_TIME_MAX;
        cur_stream = -1;
        for (i_stream = 0; i_stream < stream_nof; i_stream++)
        {
            sand_packet = (sand_packet_t *) packet_hs[i_stream];

            if (sand_packet->stream.running == FALSE)
                continue;

            /** Look for smallest time between the ones that have no expired already         */
            if (sand_packet->stream.duration < time)
            {
                time = sand_packet->stream.duration;
                cur_stream = i_stream;
            }
        }
        if ((time == SAND_STREAM_TIME_MAX) || (cur_stream == -1))
        {
            break;
        }
        else
        {
            sand_packet = (sand_packet_t *) packet_hs[cur_stream];
            sal_sleep(sand_packet->stream.duration - total_time);
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_stop(unit, packet_hs[cur_stream]));
            total_time += sand_packet->stream.duration;
        }
    }
    while (--loop_counter);

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
diag_sand_packet_last_show(
    int unit,
    int core)
{
    signal_output_t *packet_output = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** get also the pkt_header for the FWD layer FWD lookup terms - with various options in its signal appearance */
    if (sand_signal_output_find(unit, core, 0, "IRPP", "NIF", "", "header", &packet_output) != _SHR_E_NONE)
    {
        if (sand_signal_output_find(unit, core, 0, "IRPP", "", "", "header", &packet_output) != _SHR_E_NONE)
        {
            /** for special cases where instead of from "NIF" and "InNetworkAdaptor", used another block name as "PRT" */
            SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, 0, "IRPP", "PRT", "", "Pkt_Header",
                                                        &packet_output), "no signal record for header\n");
        }
    }

    packet_decode_info_print(unit, packet_output->field_list, NULL);

exit:
    sand_signal_output_free(packet_output);
    SHR_FUNC_EXIT;
}

#if !defined(NO_FILEIO)

static shr_error_e
diag_sand_packet_find_by_name(
    int unit,
    char *packet_n,
    xml_node * curPacket)
{
    xml_node curTop = NULL, curSubTop = NULL, curLine;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(curPacket, _SHR_E_PARAM, "curPacket");

    if (ISEMPTY(packet_n))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Empty packet name\n");
    }
    if (((curTop = dbx_pre_compiled_common_top_get(unit, "DNX-Devices.xml", "top", 0)) == NULL) ||
        ((curSubTop = dbx_xml_child_get_first(curTop, "case-objects")) == NULL))
    {
        /*
         * No parsing objects - just leave
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    RHDATA_ITERATOR(curLine, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(curLine, "type", type);
        if (!sal_strcasecmp(type, "tests"))
        {
            xml_node curFile, curSection, cur;
            int device_specific = 0;

            RHDATA_GET_INT_DEF(curLine, "device_specific", device_specific, 0);
            RHDATA_GET_STR_CONT(curLine, "file", filename);
            if (device_specific == TRUE)
            {
                if ((curFile = dbx_pre_compiled_devices_top_get(unit, filename, "top", CONF_OPEN_PER_DEVICE)) == NULL)
                    continue;
            }
            else
            {
                if ((curFile = dbx_pre_compiled_common_top_get(unit, filename, "top", 0)) == NULL)
                    continue;
            }

            /*
             * Go through packets and found the requested one
             */
            if ((curSection = dbx_xml_child_get_first(curFile, "packets")) != NULL)
            {
                RHDATA_ITERATOR(cur, curSection, "packet")
                {
                    char name[RHNAME_MAX_SIZE];
                    RHDATA_GET_STR_DEF_NULL(cur, "name", name);
                    if (!sal_strcasecmp(packet_n, name))
                    {
                        *curPacket = cur;
                        SHR_EXIT();
                    }
                }
            }
            dbx_xml_top_close(curFile);
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_load(
    int unit,
    char *packet_n,
    rhhandle_t packet_h,
    bcm_port_t * src_port_p)
{
    xml_node cur, curProto, curPacket = NULL;
    int rate = 0, burst = 0, duration = -1, packet_size = 0, rate_unit = BCM_SAT_GTF_RATE_IN_BYTES;
    int granularity = SAT_GRANULARITY_SET_BY_DIAG;
    int src_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_sand_packet_find_by_name(unit, packet_n, &curPacket));

    if (curPacket == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Packet case:'%s' was not found\n", packet_n);
    }

    RHDATA_GET_INT_DEF(curPacket, "length", packet_size, SAND_PACKET_DEFAULT_SIZE_BYTES);

    RHDATA_ITERATOR(curProto, curPacket, "proto")
    {
        char proto_name[RHNAME_MAX_SIZE];
        xml_node curField;
        RHDATA_GET_STR_STOP(curProto, "name", proto_name);
        /*
         * Header argument
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, proto_name));
        RHDATA_ITERATOR(curField, curProto, "field")
        {
            char field_name[RHNAME_MAX_SIZE], field_value[RHNAME_MAX_SIZE], full_name[RHNAME_MAX_SIZE];
            RHDATA_GET_STR_STOP(curField, "name", field_name);
            RHDATA_GET_STR_STOP(curField, "value", field_value);
            sal_snprintf(full_name, RHNAME_MAX_SIZE - 1, "%s.%s", proto_name, field_name);
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, full_name, field_value));
        }
    }
    RHCHDATA_GET_INT_DEF(curPacket, "source", "port", src_port, 201);
    /*
     * if there is stream entry acquire relevant parameters, keep existing values if parameter does not appear
     */
    if ((cur = dbx_xml_child_get_first(curPacket, "stream")) != NULL)
    {
        char rate_unit_name[RHNAME_MAX_SIZE];
        RHDATA_GET_INT_DEF(cur, "time", duration, -1);
        RHDATA_GET_INT_DEF(cur, "rate", rate, 0);
        RHDATA_GET_INT_DEF(cur, "burst", burst, 0);
        RHDATA_GET_INT_DEF(cur, "granularity", granularity, SAT_GRANULARITY_SET_BY_DIAG);
        RHDATA_GET_STR_DEF(cur, "unit", rate_unit_name, "kbites");
        if (!sal_strcasecmp(rate_unit_name, "kbites"))
        {
            rate_unit = BCM_SAT_GTF_RATE_IN_BYTES;
        }
        else if (sal_strcasecmp(rate_unit_name, "packets"))
        {
            rate_unit = BCM_SAT_GTF_RATE_IN_PACKETS;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Bad rate unit:%s in packet case:'%s'\n", rate_unit_name, packet_n);
        }
    }

    diag_sand_packet_stream_create(unit, -1, packet_h, packet_size, duration, rate_unit, rate, burst, granularity);
    if (src_port_p != NULL)
    {
        *src_port_p = src_port;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * See diag_send_packet.h
 */
shr_error_e
diag_sand_packet_proto_get_uint_field(
    int unit,
    char *packet_n,
    char *proto_n,
    char *field_n,
    uint32 *lenght_p,
    uint32 *uint32_field_val_p)
{
    shr_error_e shr_error;
    xml_node curProto, curPacket = NULL;
    int proto_found = FALSE;
    int in_proto_size;
    int packet_proto_size;
    int field_found = FALSE;
    int in_field_size;
    int packet_field_size;
    int packet_size;

    SHR_FUNC_INIT_VARS(unit);
    if (uint32_field_val_p != NULL)
    {
        *uint32_field_val_p = -1;
    }
    SHR_IF_ERR_EXIT(diag_sand_packet_find_by_name(unit, packet_n, &curPacket));
    if (curPacket == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "Packet case:'%s' was not found\n", packet_n);
    }
    if (lenght_p != NULL)
    {
        RHDATA_GET_INT_DEF(curPacket, "length", packet_size, SAND_PACKET_DEFAULT_SIZE_BYTES);
        *lenght_p = (uint32) packet_size;
    }
    if ((proto_n == NULL) || (field_n == NULL))
    {
        SHR_EXIT();
    }
    in_proto_size = sal_strnlen(proto_n, RHNAME_MAX_SIZE);
    in_field_size = sal_strnlen(field_n, RHNAME_MAX_SIZE);
    RHDATA_ITERATOR(curProto, curPacket, "proto")
    {
        char proto_name[RHNAME_MAX_SIZE];
        xml_node curField;
        char field_value[RHNAME_MAX_SIZE];

        proto_found = FALSE;
        RHDATA_GET_STR_STOP(curProto, "name", proto_name);
        packet_proto_size = sal_strnlen(proto_name, RHNAME_MAX_SIZE);
        if (packet_proto_size != in_proto_size)
        {
            continue;
        }
        if (sal_memcmp(proto_name, proto_n, in_proto_size) != 0)
        {
            continue;
        }
        proto_found = TRUE;
        field_found = FALSE;
        RHDATA_ITERATOR(curField, curProto, "field")
        {
            char field_name[RHNAME_MAX_SIZE];

            RHDATA_GET_STR_STOP(curField, "name", field_name);
            RHDATA_GET_STR_STOP(curField, "value", field_value);
            packet_field_size = sal_strnlen(field_name, RHNAME_MAX_SIZE);
            if (packet_field_size != in_field_size)
            {
                continue;
            }
            if (sal_memcmp(field_name, field_n, in_field_size) != 0)
            {
                continue;
            }
            field_found = TRUE;
            break;
        }
        if ((field_found == TRUE) && (proto_found == TRUE))
        {
            /*
             * Currently, we ignore potential errors in sal_strtoul()
             */
            *uint32_field_val_p = sal_strtoul(field_value, NULL, 0);
            SHR_EXIT();
        }
    }
exit:
    shr_error = SHR_GET_CURRENT_ERR();
    if (shr_error == _SHR_E_NONE)
    {
        if ((proto_n != NULL) && (field_n != NULL))
        {
            if ((field_found != TRUE) || (proto_found != TRUE))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
        }
    }
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_send_by_case(
    int unit,
    char *packet_n,
    rhhandle_t * packet_h_p,
    uint32 flags)
{
    rhhandle_t packet_h = NULL;
    bcm_port_t src_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));

    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_load(unit, packet_n, packet_h, &src_port), "");

    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, flags));

exit:
    /*
     * If pointer to handle provided - let the caller deal with it
     * If packet_h == NULL - both case have the same effect
     */
    if (packet_h_p != NULL)
    {
        *packet_h_p = packet_h;
    }
    else
    {
        diag_sand_packet_free(unit, packet_h);
    }
    SHR_FUNC_EXIT;
}
#endif /* !defined(NO_FILEIO) */

#if defined(INCLUDE_L3) && defined(BCM_DNX_SUPPORT)
/*
 * \brief add configuration for Bridge flow, matching the "simple" packet
 */
shr_error_e
diag_sand_bridge_simple_packet_add(
    int unit)
{
    uint32 vsi = 1121;
    bcm_mac_t dmac_addr = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    bcm_port_t in_port = 200;
    bcm_port_t out_port = 201;
    bcm_l2_addr_t l2addr;
    bcm_gport_t vlan_port_id = 0x1234;
    bcm_vlan_port_t vlan_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, in_port, bcmPortClassId, in_port));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, out_port, bcmPortClassId, out_port));

    /**Create VSI*/
    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vsi));

    /**Create InLif*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_WITH_ID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vsi;
    vlan_port.port = in_port;
    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    bcm_l2_addr_t_init(&l2addr, dmac_addr, vsi);
    l2addr.port = out_port;
    l2addr.flags = BCM_L2_STATIC;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief destroy configuration done by diag_sand_bridge_simple_packet_add
 */
shr_error_e
diag_sand_bridge_simple_packet_destroy(
    int unit)
{
    uint32 vsi = 1121;
    bcm_mac_t dmac_addr = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    bcm_gport_t vlan_port_id = 0x1234;

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port_id, vlan_port_id);

    SHR_IF_ERR_EXIT(bcm_l2_addr_delete(unit, dmac_addr, vsi));
    SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, vlan_port_id));
    SHR_IF_ERR_EXIT(bcm_vlan_destroy_all(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief add configuration for Route flow, matching the "default" packet
 */
shr_error_e
diag_sand_route_default_packet_add(
    int unit)
{
    bcm_switch_fec_property_config_t fec_config;
    bcm_vlan_port_t vlan_port;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t ingress_rif;
    bcm_port_t in_port = 201;
    bcm_port_t out_port = 202;
    bcm_l3_egress_t l3eg;
    bcm_l3_host_t host;
    int kaps_result;
    int fec_offset = 1234;
    int intf_in = 47;
    int intf_out = 144;
    int vrf = 77;
    int encap_id = 0x1456;
    bcm_gport_t vlan_port_id = 0x1212;

    bcm_mac_t intf_in_mac_address = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };

    uint32 host_addr = 0x0a0101de;      /* 10.01.01.222 */

    SHR_FUNC_INIT_VARS(unit);

    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
    kaps_result = fec_config.start + fec_offset;

    /** Set In-Port to In ETh-RIF*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT | BCM_VLAN_PORT_WITH_ID;
    vlan_port.vsi = intf_in;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, intf_in, in_port, 0));

    /*
     * Create ETH-RIF and set its properties
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_in_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_in;
    l3if.l3a_vid = intf_in;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_out_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_out;
    l3if.l3a_vid = intf_out;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    /** Set Incoming ETH-RIF properties*/
    bcm_l3_ingress_t_init(&ingress_rif);
    ingress_rif.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_rif.vrf = vrf;
    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &ingress_rif, &intf_in));

    /** Create ARP and set its properties*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, arp_next_hop_mac, sizeof(bcm_mac_t));
    l3eg.encap_id = encap_id;
    l3eg.vlan = intf_out;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, NULL));
    encap_id = l3eg.encap_id;

    /*
     * Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = intf_out;
    l3eg.encap_id = encap_id;
    l3eg.port = out_port;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &kaps_result));
    /**Add host entry*/
    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = host_addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = kaps_result;
    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief destroy configuration done by diag_sand_route_default_packet_add
 */
shr_error_e
diag_sand_route_default_packet_destroy(
    int unit)
{
    bcm_switch_fec_property_config_t fec_config;
    bcm_l3_host_t host;
    int kaps_result;
    int vrf = 77;
    int encap_id = 0x1456;
    uint32 host_addr = 0x0a0101de;      /* 10.01.01.222 */
    int fec_offset = 1234;

    SHR_FUNC_INIT_VARS(unit);

    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));
    kaps_result = fec_config.start + fec_offset;

    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = host_addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = kaps_result;

    BCM_L3_ITF_SET(kaps_result, BCM_L3_ITF_TYPE_FEC, kaps_result);
    BCM_L3_ITF_SET(encap_id, BCM_L3_ITF_TYPE_LIF, encap_id);
    SHR_IF_ERR_EXIT(bcm_l3_host_delete(unit, &host));
    SHR_IF_ERR_EXIT(bcm_l3_intf_delete_all(unit));
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, kaps_result));
    SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, encap_id));

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
diag_sand_packet_send_simple(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if defined(INCLUDE_L3) && defined(BCM_DNX_SUPPORT)
    SHR_IF_ERR_EXIT(diag_sand_bridge_simple_packet_add(unit));
    SHR_IF_ERR_EXIT(diag_sand_route_default_packet_add(unit));
#endif

#if !defined(NO_FILEIO)

    SHR_IF_ERR_EXIT(diag_sand_packet_send_by_case(unit, "simple", NULL, SAND_PACKET_RESUME));
#ifdef BCM_DNX_SUPPORT
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
#endif
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_send_by_case(unit, "default", NULL, 0));
    }

    /** re-send the packets, to prevent OLP packet*/
    SHR_IF_ERR_EXIT(diag_sand_packet_send_by_case(unit, "simple", NULL, SAND_PACKET_RESUME));
#ifdef BCM_DNX_SUPPORT
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
#endif
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_send_by_case(unit, "default", NULL, 0));
    }

#else
    {
        rhhandle_t packet_h = NULL;
        bcm_port_t from_port = 201;
        bcm_port_mapping_info_t mapping_info;
        bcm_port_interface_info_t interface_info;
        uint32 flags;
        SHR_IF_ERR_EXIT(bcm_port_get(unit, from_port, &flags, &interface_info, &mapping_info));
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        diag_sand_packet_proto_add(unit, packet_h, "PTCH_2");
        diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16);
        diag_sand_packet_proto_add(unit, packet_h, "ETH1");
        diag_sand_packet_send(unit, from_port, packet_h, FALSE);
        diag_sand_packet_free(unit, packet_h);
    }
#endif /* !defined(NO_FILEIO) */
#if defined(INCLUDE_L3) && defined(BCM_DNX_SUPPORT)
    SHR_IF_ERR_EXIT(diag_sand_bridge_simple_packet_destroy(unit));
    SHR_IF_ERR_EXIT(diag_sand_route_default_packet_destroy(unit));
#endif
exit:
    SHR_FUNC_EXIT;
}
