/**
 * \file diag_sand_packet.c
 *
 * Implementation of diag command 'dnx packet'.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
/*
 * appl
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/sand/diag_sand_dsig.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*
 * bcm
 */
#include <bcm/rx.h>

/*
 * soc
 */
#include <soc/drv.h>
#ifdef SAL_THREAD_NAME_PRINT_DISABLE
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#endif
#endif
/*
 * }
 */
/*
 * PParse defines and macros
 * {
#define _PPARSE_DEBUG_
 */
#ifdef _PPARSE_DEBUG_
/**
 * \brief Easily disabled printing macro
 */
#define PPARSE_LOG(stuff_) LOG_CLI(stuff_)
#else
#define PPARSE_LOG(stuff_)
#endif

/****************************************************************************************
 * Packet Decode
 * {
 */
static shr_error_e
packet_decode_option_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    parsing_db_t pdb;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(pparse_init(unit, &pdb), "");

    if (NULL == utilex_rhlist_entry_get_by_name(pdb.obj_lst, keyword))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Object: %s does not exist\n", keyword);
    }

    *type_p = SAL_FIELD_TYPE_STR;

exit:
    pparse_destroy(&pdb);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Man page for 'packet decode' shell command
 */
static sh_sand_man_t packet_decode_man = {
    .brief = "Decode a packet given the 1st header name and raw data.",
    .full = "Use this command to parse a packet given in raw data (either hex string or binary string) by supplying"
        "it the first header hint.\n The raw data can be given in binary format with prefix '0b', "
        "for instance 0b01111000, or in hex format "
        "with prefix '0x', for instance 0x78. When no prefix is given, the format is assumed to be hex.",
    .synopsis = "[first header]=[data]",
    .examples = "ftmh_base=017c0064b322000640\n"
        "ETH=0x00010203040500060708090a81004001884700000000000001004500000000000000000000000a0b0c010a0b0c02"
};

/**
 * \brief
 *   hex nibble to integer value.
 * \param [in] nibble - hex nibble (0-9a-f)
 * \return int nibble value (0-15) or -1 if error
 */
static int
nibble_value(
    char nibble)
{
    if (nibble >= '0' && nibble <= '9')
    {
        return nibble - '0';
    }
    /*
     * "to lower"
     */
    nibble |= ('a' ^ 'A');
    if (nibble >= 'a' && nibble <= 'f')
    {
        return 10 + (nibble - 'a');
    }
    /*
     * unexpected nibble
     */
    return -1;
}

static shr_error_e
packet_decode_data_from_hex(
    int unit,
    const char *data_str,
    uint32 *data,
    uint32 *data_size)
{
    int lsnibble;
    int word;

    SHR_FUNC_INIT_VARS(unit);

    *data_size = 0;
    lsnibble = sal_strlen(data_str);
    for (word = 0; word < PPARSE_MAX_DATA_WORDS; word++)
    {
        int msnibble = lsnibble - 8;
        if (msnibble < 0)
        {
            msnibble = 0;
        }
        for (; msnibble < lsnibble; msnibble++)
        {
            int nibval = nibble_value(data_str[msnibble]);
            if (nibval < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid data string format: %s\n", data_str);
            }
            data[word] = ((data[word]) << 4) | nibval;
            *data_size += 4;
        }
        lsnibble -= 8;
        if (lsnibble < 0)
        {
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
packet_decode_data_from_bin(
    int unit,
    const char *data_str,
    uint32 *data,
    uint32 *data_size)
{
    int lsbit;
    int word;

    SHR_FUNC_INIT_VARS(unit);

    *data_size = 0;
    lsbit = sal_strlen(data_str);
    for (word = 0; word < PPARSE_MAX_DATA_WORDS; word++)
    {
        int msbit = lsbit - 32;
        if (msbit < 0)
        {
            msbit = 0;
        }
        for (; msbit < lsbit; msbit++)
        {
            if ((data_str[msbit] != '0') && (data_str[msbit] != '1'))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid data string format: %s\n", data_str);
            }
            else
            {
                int bitval = data_str[msbit] - '0';
                data[word] = ((data[word]) << 1) | bitval;
                *data_size += 1;
            }
        }
        lsbit -= 32;
        if (lsbit < 0)
        {
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Translates a data string (packet in hex or binary format) to a uint32 array containing the data.
 * \param [in] unit - Unit #
 * \param [in] data_str - Data in hex format (may or may not include 0x prefix) or binary format
 *                          (must be preceded by 0b)
 * \param [out] data - Output array
 * \param [out] data_size - Actual data size in bits
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
packet_decode_data_get(
    int unit,
    const char *data_str,
    uint32 *data,
    uint32 *data_size)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_str, _SHR_E_PARAM, "data_str");
    SHR_NULL_CHECK(data, _SHR_E_PARAM, "data");
    SHR_NULL_CHECK(data_size, _SHR_E_PARAM, "data_size");

    if ((data_str[0] == '0' && data_str[1] == 'b'))
    {
        /*
         * Skip "0b" prefix
         */
        data_str += 2;
        SHR_CLI_EXIT_IF_ERR(packet_decode_data_from_bin(unit, data_str, data, data_size), "");
        SHR_EXIT();
    }
    else if (data_str[0] == '0' && data_str[1] == 'x')
    {
        /*
         * Skip "0x" prefix
         */
        data_str += 2;
    }
    /*
     * Default format is assumed to be hex string
     */
    SHR_CLI_EXIT_IF_ERR(packet_decode_data_from_hex(unit, data_str, data, data_size), "");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
packet_decode_fields_add_recursive(
    int unit,
    signal_output_t * obj,
    prt_control_t * prt_ctr,
    int shift)
{
    signal_output_t *field;

    SHR_FUNC_INIT_VARS(unit);

    RHITERATOR(field, obj->field_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET_SHIFT(shift, "%s", RHNAME(field));
        PRT_CELL_SET("%s", field->print_value);
        PRT_CELL_SET("% 3db", field->size);
        SHR_CLI_EXIT_IF_ERR(packet_decode_fields_add_recursive(unit, field, prt_ctr, 1 + shift), "");
    }

exit:
    SHR_FUNC_EXIT;
}

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
shr_error_e
packet_decode_info_print(
    int unit,
    rhlist_t * pit,
    sh_sand_control_t * sand_control)
{

    int obj_col_id = 0;
    signal_output_t *obj;

    uint32 size_acc = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI(("\n"));
    PRT_TITLE_SET("Parsing Results");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &obj_col_id, "Object");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_BINARY, PRT_XML_ATTRIBUTE, obj_col_id, NULL, "Value");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, obj_col_id, NULL, "Size");

    RHITERATOR(obj, pit)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        PRT_CELL_SET("%s", RHNAME(obj));
        PRT_CELL_SET("%s", obj->print_value);
        PRT_CELL_SET("%3db", obj->size);

        SHR_CLI_EXIT_IF_ERR(packet_decode_fields_add_recursive(unit, obj, prt_ctr, 1), "");

        size_acc += obj->size;
    }

    PRT_INFO_ADD("Total headers stack size: %dB", (size_acc >> 3));

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Diag command for parsing a packet using dynamic description found in XML database.
 *   The caller specifies the data to be parsed using the 'data=' option and the name of the
 *   first header to be parsed using the 'type=' option.
 *   Following headers will be derived from the available information as long as the data
 *   hasn't run out.
 * \param [in] unit - Unit #
 * \param [in] args - CLI arguments
 * \param [in] sand_control - Control structure for DNX shell framework
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
packet_decode_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int core;
    sh_sand_arg_t *sand_arg;

    parsing_db_t pdb;
    rhlist_t *pit = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize parsing database
     */
    SHR_CLI_EXIT_IF_ERR(pparse_init(unit, &pdb), "");

    SH_SAND_GET_INT32("Core", core);

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        char *type_name = SH_SAND_GET_NAME(sand_arg);
        char *data_str = sand_arg->param1.val_str;
        pparse_packet_info_t packet_info;

        PPARSE_LOG(("asked to parse: %s\nfirst header is: %s\n", data_str, type_name));

        /*
         * Translate data string to data buffer
         */
        SHR_CLI_EXIT_IF_ERR(packet_decode_data_get(unit, data_str, packet_info.data, &packet_info.data_size), "");

        PPARSE_LOG(("Data: 0x%08x%08x%08x%08x\nData Size: %u\n", packet_info.data[3], packet_info.data[2],
                    packet_info.data[1], packet_info.data[0], packet_info.data_size));

        /*
         * Set the data description and send everything to the parsing engine
         */
        packet_info.data_desc = type_name;
        SHR_CLI_EXIT_IF_ERR(pparse_packet_process(unit, core, &pdb, &packet_info, &pit), "");
    }
    /*
     * Print parsed info
     */
    SHR_CLI_EXIT_IF_ERR(packet_decode_info_print(unit, pit, sand_control), "");

exit:
    pparse_destroy(&pdb);
    pparse_parsed_info_tree_free(unit, pit);
    SHR_FUNC_EXIT;
}
/****************************************************************************************
 * Packet Decode
 * }
 */

/****************************************************************************************
 * Packet Compose
 * {
 */
static shr_error_e
packet_compose_option_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    parsing_db_t pdb;
    char obj_name[RHNAME_MAX_SIZE + 1];
    char *dot_pos;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(pparse_init(unit, &pdb), "");

    if ((dot_pos = sal_strstr(keyword, ".")) != NULL)
    {
        sal_strncpy(obj_name, keyword, dot_pos - keyword);
        obj_name[dot_pos - keyword] = '\0';
        *type_p = SAL_FIELD_TYPE_STR;
    }
    else
    {
        sal_strncpy(obj_name, keyword, RHNAME_MAX_SIZE);
        *type_p = SAL_FIELD_TYPE_BOOL;
    }
    if (NULL == utilex_rhlist_entry_get_by_name(pdb.obj_lst, obj_name))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Object: %s does not exist\n", obj_name);
    }

exit:
    pparse_destroy(&pdb);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Man page for 'packet compose' shell command
 */
static sh_sand_man_t packet_compose_man = {
    .brief = "Compose a packet according to given headers list.",
    .full = "Use this command to generate a raw packet string to use with tx command.\n"
        "Input to the command is in the form of a list of headers after each the list of fields with non-default values.\n"
        "The headers names are in accordance with the available headers shown in 'packet list'. Currently, only network"
        "headers are supported." "The fields should be supplied with a full \"path\" (see example)",
    .synopsis = "<list of headers>",
    .examples =
        "eth1 eth1.vlan.tpid=0x9100 eth1.vlan.vid=64 mpls mpls.label=3 mpls mpls.label=4 ipv4 ipv4.ttl=100 udp bfd\n"
        "PTCH_2 ETH1 MPLS MPLS IPv4"
};

/**
 * \brief - Options list for 'packet compose' shell command
 */
static sh_sand_option_t packet_compose_arguments[] = {
    /*
     * Name
     *//*
     * Type
     *//*
     * Description
     *//*
     * Default
     */
    {NULL}      /* End of options list - must be last. */
};

static shr_error_e
packet_compose_print(
    int unit,
    rhhandle_t packet_h,
    uint32 *data,
    uint32 size,
    sh_sand_control_t * sand_control)
{
    signal_output_t *header;
    int column_header;
    int column_field;
    char print_data[DSIG_MAX_SIZE_STR];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Packet final headers and fields");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &column_header, "Header");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, column_header, &column_field, "Field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, column_field, NULL, "Value");

    RHITERATOR(header, PACKET_PROTO_LIST(packet_h))
    {
        uint8 add_row = 0;
        signal_output_t *field;
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", RHNAME(header));
        RHITERATOR(field, header->field_list)
        {
            if (add_row)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
            else
            {
                add_row = 1;
            }
            PRT_CELL_SET("%s", RHNAME(field));
            if (!ISEMPTY(field->print_value))
            {
                PRT_CELL_SET("%s", field->print_value);
            }
            else
            {
                PRT_CELL_SET("0x%x", field->value[0]);
            }
        }
    }
    PRT_INFO_ADD("Size = %d bits", size);
    PRT_COMMITX;

    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, data, print_data, size, PRINT_BIG_ENDIAN);
    LOG_CLI(("\n String for tx: 0x%s\n", print_data));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
packet_compose_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sh_sand_arg_t *sand_arg;
    rhhandle_t packet_h;

    uint32 data[DSIG_MAX_SIZE_UINT32];
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_create(unit, &packet_h), "");

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        if (sal_strstr(SH_SAND_GET_NAME(sand_arg), ".") == NULL)
        {
            /*
             * Header argument
             */
            SHR_CLI_EXIT_IF_ERR(diag_sand_packet_proto_add(unit, packet_h, SH_SAND_GET_NAME(sand_arg)), "");
        }
        else
        {
            /*
             * Field argument
             */
            SHR_CLI_EXIT_IF_ERR(diag_sand_packet_field_add_str(unit, packet_h, SH_SAND_GET_NAME(sand_arg),
                                                               sand_arg->param1.val_str), "");
        }
    }

    SHR_CLI_EXIT_IF_ERR(pparse_compose(unit, PACKET_PROTO_LIST(packet_h), PACKET_SIZE(packet_h),
                                       DSIG_MAX_SIZE_BITS, data, &size), "");
    SHR_CLI_EXIT_IF_ERR(packet_compose_print(unit, packet_h, data, size, sand_control), "");

exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;
}
/****************************************************************************************
 * Packet Compose
 * }
 */

/****************************************************************************************
 * Packet Combine
 * {
 */
static shr_error_e
packet_combine_option_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sand_signal_parse_exists(unit, keyword), "");

    *type_p = SAL_FIELD_TYPE_BOOL;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Man page for 'packet combine' shell command
 */
static sh_sand_man_t packet_combine_man = {
    .brief = "Prints bit offsets from MSB of a specified packet structure",
    .full =
        "Use this command to generate a list of fields in a packet according to a list of given headers (protocols). The "
        "headers in the list must appear in NetworkStructures.xml or the device specific SignalStructures.xml. "
        "This command is mainly a code example for the usage of the underlying service - pparse_combine.",
    .synopsis = "name=<packet-name> <list of headers>",
    .examples = "name=BFDoTunnel ETH2 MPLS MPLS ETH0 IPV4 UDP BFD"
};

/**
 * \brief - Options list for 'packet compose' shell command
 */
static sh_sand_option_t packet_combine_arguments[] = {
    /*
     * Name
     *//*
     * Type
     *//*
     * Description
     *//*
     * Default
     */
    {"name", SAL_FIELD_TYPE_STR, "Name of the newly combined packet", NULL},
    {NULL}      /* End of options list - must be last. */
};

static shr_error_e
packet_combine_print(
    int unit,
    signal_output_t * combined,
    sh_sand_control_t * sand_control)
{
    signal_output_t *field;
    int column_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Packet Field Offsets from MSB");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &column_id, "Header");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, column_id, NULL, "Start-bit");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, column_id, NULL, "End-bit");

    RHITERATOR(field, combined->field_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(field));
        PRT_CELL_SET("%d", field->start_bit);
        PRT_CELL_SET("%d", field->end_bit);
    }
    PRT_INFO_ADD("Combined packet name: '%s'", RHNAME(combined));
    PRT_INFO_ADD("Combined-size = %d bits", combined->size);
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
packet_combine_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sh_sand_arg_t *sand_arg;
    rhhandle_t packet_h;
    signal_output_t combined_pkt;
    char *name_arg_p;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all the headers (protocols) needed for the packet.
     */
    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_create(unit, &packet_h), "");
    SH_SAND_GET_ITERATOR(sand_arg)
    {
        if (sand_arg->type == SAL_FIELD_TYPE_BOOL)
        {
            /*
             * Header argument.
             * Add the header to the list
             */
            SHR_CLI_EXIT_IF_ERR(diag_sand_packet_proto_add(unit, packet_h, SH_SAND_GET_NAME(sand_arg)), "");
        }
    }
    /*
     * Create the pparse combine output structure
     */
    sal_memset(&combined_pkt, 0, sizeof(signal_output_t));
    /*
     * Set the combined signal name to the diag's name= argument.
     */
    SH_SAND_GET_STR("name", name_arg_p);
    sal_strncpy(RHNAME(&combined_pkt), name_arg_p, RHNAME_MAX_SIZE - 1);
    /*
     * Call pparse_combine. This will:
     * - Combine the created list to the combined_pkt.field_list
     * - Update combined_pkt.size.
     */
    SHR_CLI_EXIT_IF_ERR(pparse_combine(unit, PACKET_PROTO_LIST(packet_h), &combined_pkt, TRUE), "");
    /*
     * Print the result
     */
    SHR_CLI_EXIT_IF_ERR(packet_combine_print(unit, &combined_pkt, sand_control), "");
exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;
}
/****************************************************************************************
 * Packet Combine
 * }
 */

/****************************************************************************************
 * Packet List
 * {
 */
/**
 * \brief - Man page for 'export' shell command
 */
static sh_sand_man_t packet_list_man = {
    .brief = "List the available objects to parse.",
    .full = "List the available objects that can be used in the 'type' option of 'packet decode' command",
    .synopsis = NULL,   /* NULL means autogenerated */
    .examples = NULL
};

static shr_error_e
packet_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    parsing_db_t pdb;
    pparse_db_entry_t *entry = NULL;
    int header_col_id;
#ifdef _PPARSE_DEBUG_
    int ext_col_id;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(pparse_init(unit, &pdb), "");

    PRT_TITLE_SET("Available headers");
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &header_col_id, "Header");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, header_col_id, NULL, "Size");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_ATTRIBUTE, header_col_id, NULL, "Description");
#ifdef _PPARSE_DEBUG_
    PRT_COLUMN_ADDX(PRT_XML_CHILD, header_col_id, &ext_col_id, "Extensions");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, ext_col_id, NULL, "#ExtConditions");
#endif

    RHITERATOR(entry, pdb.obj_lst)
    {
#ifdef _PPARSE_DEBUG_
        int ext_i;
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
#else
        if (ISEMPTY(entry->desc_str))
        {
            /*
             * Don't print objects without description
             */
            continue;
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
#endif
        PRT_CELL_SET("%s", entry->entry.name);
        PRT_CELL_SET("%3db", entry->size);
        PRT_CELL_SET("%s", entry->desc_str);
#ifdef _PPARSE_DEBUG_
        for (ext_i = 0; ext_i < entry->extensions_nof; ext_i++)
        {
            packet_object_extension_t *ext = &entry->extensions[ext_i];
            if (ext_i > 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(2);
            }
            PRT_CELL_SET("%s", ext->object_name);
            PRT_CELL_SET("%d", ext->conditions_nof);
        }
#endif
    }

    PRT_COMMITX;

exit:
    pparse_destroy(&pdb);
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/****************************************************************************************
 * Packet List
 * }
 */

/****************************************************************************************
 * Packet Tx
 * {
 */
static shr_error_e
packet_tx_example_generate(
    int unit,
    int flags,
    struct sh_sand_cmd_s *sh_sand_cmd,
    rhlist_t * examples_list)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, "pkt=default"), "");
    SHR_CLI_EXIT_IF_ERR(sh_sand_example_add(unit, flags, sh_sand_cmd, examples_list, "pkt=mpls_ipv4"), "");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Man page for 'export' shell command
 */
static sh_sand_man_t packet_tx_man = {
    .brief = "Send packet to the pipeline",
    .full = "Send packet to the pipeline, providing list of protocols, that should compose it."
        " There are default values for each field in the header."
        " Assign values for the ones that need to be different right after the protocol name",
    .synopsis = NULL,   /* NULL means autogenerated */
    .examples = "PTCH_2 ETH1 IPv4 show resume=NO\n"
        "packet=simple rx\n"
        "packet=default sat port=1\n"
        "packet=default port=218 size=256 rate=10000 burst=10000 dur=1 unit=kbits\n"
        "pkt=sat\n"
        "packet=default sat port=1 duration=0 grnl=1000\n"
        "port=200 PTCH_2 PTCH_2.PP_SSP=27 ETH1 MPLS BIER IPv4\n"
        "PTCH_2 ETH1 ETH1.da=00:FF:EE:DD:DD:BB MPLS MPLS.Label=256 IPv6\n"
        "raw DATA=0x80c90000000000010000000000028100000108004500002e0000000040ff79d20000000000000000000102030405060708090a0b0c0d0e0f1011",
    .example_generate_cb = packet_tx_example_generate
};

/**
 * \brief - Options list for 'packet list' shell command
 */
/* *INDENT-OFF* */
static sh_sand_enum_t unit_enum_table[] = {
    {"kbits", BCM_SAT_GTF_RATE_IN_BYTES, "If set gtf rate is configured in kilobits per second"},
    {"packets", BCM_SAT_GTF_RATE_IN_PACKETS, "If set gtf rate is configured in packets per second"},
    {NULL}
};

static sh_sand_option_t packet_tx_arguments[] = {
   /* Name | Type               | Description                            | Default      */
    {"DATA",    SAL_FIELD_TYPE_STR,    "String representing packet to be send", ""},
    {"raw",     SAL_FIELD_TYPE_BOOL,   "Transmit composed packet from DATA",    "No"},
    {"show",    SAL_FIELD_TYPE_BOOL,   "Show full packet content",              "No"},
    {"port",    SAL_FIELD_TYPE_INT32,  "Use specific source logical port",      "201"},
    {"resume",  SAL_FIELD_TYPE_BOOL,   "Triggers signals cache clearance",      "Yes"},
    {"packet",  SAL_FIELD_TYPE_STR,    "Allows sand packet from xml case",      ""},
    {"rx",      SAL_FIELD_TYPE_BOOL,   "Record received packets",               "No"},
    {"size",    SAL_FIELD_TYPE_UINT32, "Packet Size(bytes)",                    "0"},
    {"duration",SAL_FIELD_TYPE_UINT32, "Stream Duration(seconds)",              "-1"},
    {"unit",    SAL_FIELD_TYPE_ENUM,   "Rate units",                            "kbits", unit_enum_table},
    {"rate",    SAL_FIELD_TYPE_UINT32, "Stream Rate(kbits or packets per sec)", "0"},
    {"burst",   SAL_FIELD_TYPE_UINT32, "Stream Burst(bytes)",                   "0"},
    {"granularity", SAL_FIELD_TYPE_UINT32, "Clocks per cycle - set globally",   "-1"},
    {"sat",     SAL_FIELD_TYPE_BOOL,   "Generate SAT stream from the core of src_port", "No"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */

/**
 * \brief
 *   Diag command for printing packet that was just sent. All headers and fields will be presented, including defaults
 * \param [in] unit - Unit #
 * \param [in] pit - original header list
 * \param [in] sand_control - Control structure for DNX shell framework
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
packet_tx_cmd_show(
    int unit,
    rhlist_t * pit,
    sh_sand_control_t * sand_control)
{
    int core;

    signal_output_t *obj;
    parsing_db_t pdb;
    rhlist_t *result_pit = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize parsing database
     */
    SHR_CLI_EXIT_IF_ERR(pparse_init(unit, &pdb), "");

    SH_SAND_GET_INT32("Core", core);

    RHITERATOR(obj, pit)
    {
        char *type_name = RHNAME(obj);
        char *data_str = obj->print_value;
        pparse_packet_info_t packet_info;

        /*
         * Translate data string to data buffer
         */
        SHR_CLI_EXIT_IF_ERR(packet_decode_data_get(unit, data_str, packet_info.data, &packet_info.data_size), "");

        /*
         * Set the data description and send everything to the parsing engine
         */
        packet_info.data_desc = type_name;
        SHR_CLI_EXIT_IF_ERR(pparse_packet_process(unit, core, &pdb, &packet_info, &result_pit), "");
    }
    /*
     * Print parsed info
     */
    SHR_CLI_EXIT_IF_ERR(packet_decode_info_print(unit, result_pit, sand_control), "");

exit:
    pparse_destroy(&pdb);
    pparse_parsed_info_tree_free(unit, result_pit);
    SHR_FUNC_EXIT;
}

static rhlist_t *stream_list = NULL;

typedef struct
{
    rhentry_t entry;
    rhhandle_t packet_h;
} stream_info_t;

static shr_error_e
packet_tx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int raw_flag, resume_flag, rx_flag, sat_flag;
#if !defined(NO_FILEIO)
    char *packet_n;
#endif
    int show_flag;
    int packet_free_flag = TRUE;
    int rate, burst, duration, packet_size, rate_unit, granularity;
    uint32 flags = 0;

    bcm_port_t src_port;
    rhhandle_t packet_h = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("raw", raw_flag);
    SH_SAND_GET_BOOL("resume", resume_flag);
    SH_SAND_GET_BOOL("rx", rx_flag);
    SH_SAND_GET_BOOL("sat", sat_flag);
    SH_SAND_GET_UINT32("size", packet_size);
    SH_SAND_GET_UINT32("rate", rate);
    SH_SAND_GET_UINT32("duration", duration);
    SH_SAND_GET_UINT32("burst", burst);
    SH_SAND_GET_UINT32("granularity", granularity);
    SH_SAND_GET_ENUM("unit", rate_unit);
    SH_SAND_GET_INT32("port", src_port);

    if (resume_flag == TRUE)
    {
        flags |= SAND_PACKET_RESUME;
    }
    if (rx_flag == TRUE)
    {
        flags |= SAND_PACKET_RX;
    }
    if (sat_flag == TRUE)
    {
        flags |= SAND_PACKET_SAT;
    }
    if (raw_flag == TRUE)
    {
        char *data_string;
        SH_SAND_GET_STR("data", data_string);

        if (ISEMPTY(data_string))
        {
            data_string = example_data_string;
        }
        /*
         * Leave after it anyway
         */
        SHR_CLI_EXIT(diag_sand_packet_tx(unit, src_port, packet_size, data_string, flags), "");
    }

    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_create(unit, &packet_h), "");

#if !defined(NO_FILEIO)
    SH_SAND_GET_STR("packet", packet_n);
    /*
     * If packet name name was set - load the default parameters from case,
     * Command input will take precedence over case
     */
    if (!ISEMPTY(packet_n))
    {
        int is_present;
        bcm_port_t *src_port_p;
        SH_SAND_IS_PRESENT("port", is_present);
        /*
         * If caller specified src_port explicitly it takes precedence over the one from case,
         * If not keep the one from case
         */
        if (is_present == TRUE)
        {
            src_port_p = NULL;
        }
        else
        {
            src_port_p = &src_port;
        }
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_load(unit, packet_n, packet_h, src_port_p), "");
    }
#endif

    /*
     * Additional arguments may overwrite the default from case if existed, else just build regular packet
     */
    {
        sh_sand_arg_t *sand_arg;

        SH_SAND_GET_ITERATOR(sand_arg)
        {
            if (sal_strstr(SH_SAND_GET_NAME(sand_arg), ".") == NULL)
            {
                /*
                 * Header argument
                 */
                SHR_CLI_EXIT_IF_ERR(diag_sand_packet_proto_add(unit, packet_h, SH_SAND_GET_NAME(sand_arg)), "");
            }
            else
            {
                SHR_CLI_EXIT_IF_ERR(diag_sand_packet_field_add_str(unit, packet_h, SH_SAND_GET_NAME(sand_arg),
                                                                   SH_SAND_ARG_STR(sand_arg)), "");
            }
        }
    }
    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_stream_create(unit, -1, packet_h, packet_size,
                                                       duration, rate_unit, rate, burst, granularity), "");

    if (PACKET_DURATION(packet_h) == 0)
    {
        rhhandle_t stream_h;
        if (stream_list == NULL)
        {
            if ((stream_list = utilex_rhlist_create("Streams", sizeof(stream_info_t), 1)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create SAT stream list\n");
            }
        }
        SHR_CLI_EXIT_IF_ERR(utilex_rhlist_entry_add_tail(stream_list, NULL, RHID_TO_BE_GENERATED, &stream_h),
                            "Unable to add stream with ID:%d to the list\n", PACKET_GTF_ID(packet_h));
        ((stream_info_t *) stream_h)->packet_h = packet_h;
        LOG_CLI(("Stream:%d started to run. To stop 'pkt stop id=%d'\n", RHID(packet_h), RHID(packet_h)));
        /*
         * Do not free packets for non-stopped streams
         */
        packet_free_flag = FALSE;
    }
    SHR_CLI_EXIT_IF_ERR(diag_sand_packet_send(unit, src_port, packet_h, flags), "");
    SH_SAND_GET_BOOL("show", show_flag);
    if (show_flag == TRUE)
    {
        SHR_CLI_EXIT_IF_ERR(packet_tx_cmd_show(unit, PACKET_PROTO_LIST(packet_h), sand_control), "");
    }
exit:
    if (packet_free_flag == TRUE)
    {
        diag_sand_packet_free(unit, packet_h);
    }
    SHR_FUNC_EXIT;
}

/****************************************************************************************
 * Packet Tx
 * }
 */

/****************************************************************************************
 * Packet Rx
 * {
 */
/**
 * \brief - Man page for 'export' shell command
 */
static sh_sand_man_t packet_rx_man = {
    .brief = "Get number of matching packet in rx queue",
    .examples = "ETH1 IPv4 dump\n" "ETH1 MPLS BIER IPv4\n" "ETH1 ETH1.da=00:FF:EE:DD:DD:BB MPLS MPLS.Label=256 IPv6"
};

/**
 * \brief - Options list for 'packet list' shell command
 */
/* *INDENT-OFF* */
static sh_sand_option_t packet_rx_arguments[] = {
   /* Name | Type               | Description                            | Default      */
    {"dump",  SAL_FIELD_TYPE_BOOL, "Dump all packets from rx queue",    "No"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */

static shr_error_e
packet_rx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int show_flag;
    int match_count, rx_count;
    sh_sand_arg_t *sand_arg;

    rhhandle_t packet_h = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("dump", show_flag);

    if ((rx_count = diag_sand_rx_nof(unit)) == 0)
    {
        /*
         * Not an error - there are simply no packets in RX queue - nothing to work with
         */
        SHR_CLI_EXIT(_SHR_E_NONE, "RX queue is empty\n");
    }

    if (show_flag == TRUE)
    {
        SHR_CLI_EXIT_IF_ERR(diag_sand_rx_dump(unit, sand_control), "");
    }

    if (SH_SAND_GET_DYN_ARGS_NUM() != 0)
    {
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_create(unit, &packet_h), "");

        SH_SAND_GET_ITERATOR(sand_arg)
        {
            if (sal_strstr(SH_SAND_GET_NAME(sand_arg), ".") == NULL)
            {
                /*
                 * Header argument
                 */
                SHR_CLI_EXIT_IF_ERR(diag_sand_packet_proto_add(unit, packet_h, SH_SAND_GET_NAME(sand_arg)), "");
            }
            else
            {
                SHR_CLI_EXIT_IF_ERR(diag_sand_packet_field_add_str(unit, packet_h, SH_SAND_GET_NAME(sand_arg),
                                                                   SH_SAND_ARG_STR(sand_arg)), "");
            }
        }

        SHR_CLI_EXIT_IF_ERR(diag_sand_rx_compare(unit, packet_h, &match_count), "");

        LOG_CLI(("%d matching packets found in RX queue out of:%d\n", match_count, rx_count));
    }

exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;
}

/****************************************************************************************
 * Packet Rx
 * }
 */
/****************************************************************************************
 * Packet Show
 * {
 */
/**
 * \brief - Man page for 'export' shell command
 */
static sh_sand_man_t packet_show_man = {
    .brief = "Show active SAT Streams"
};

static shr_error_e
packet_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    stream_info_t *stream;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Active SAT Streams");
    PRT_COLUMN_ADD("Stream ID");
    PRT_COLUMN_ADD("GTF ID");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Burst");
    RHITERATOR(stream, stream_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", RHID(stream));
        PRT_CELL_SET("%d", PACKET_GTF_ID(stream->packet_h));
        PRT_CELL_SET("%d", PACKET_CORE(stream->packet_h));
        PRT_CELL_SET("%d", PACKET_RATE(stream->packet_h));
        PRT_CELL_SET("%d", PACKET_BURST(stream->packet_h));
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/****************************************************************************************
 * Packet Show
 * }
 */

/****************************************************************************************
 * Packet Stop
 * {
 */
/**
 * \brief - Man page for 'export' shell command
 */
static sh_sand_option_t packet_stop_arguments[] = {
    /*
     * Name | Type | Description | Default
     */
    {"id", SAL_FIELD_TYPE_INT32, "Stop specified SAT stream", "-1"},
    {NULL}      /* End of options list - must be last. */
};

static sh_sand_man_t packet_stop_man = {
    .brief = "Stop active SAT Stream",
    .examples = "id=-1\n"
};

static shr_error_e
packet_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int stream_id;
    stream_info_t *stream_info;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", stream_id);

    if (stream_id != -1)
    {
        if ((stream_info = (stream_info_t *) utilex_rhlist_entry_get_by_id(stream_list, stream_id)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "No active stream with ID:%d\n", stream_id);
        }
        SHR_CLI_EXIT_IF_ERR(utilex_rhlist_entry_remove(stream_list, stream_info),
                            "Failed to remove stream:%d from the list", stream_id);
        SHR_CLI_EXIT_IF_ERR(diag_sand_packet_send(unit, 0, stream_info->packet_h, SAND_PACKET_STOP), "");
        diag_sand_packet_free(unit, stream_info->packet_h);
    }
    else
    {
        /*
         * Stop and remove all streams
         */
        RHITERATOR(stream_info, stream_list)
        {
            SHR_CLI_EXIT_IF_ERR(diag_sand_packet_send(unit, 0, stream_info->packet_h, SAND_PACKET_STOP), "");
            diag_sand_packet_free(unit, stream_info->packet_h);
        }
        utilex_rhlist_clean(stream_list);
    }
exit:
    SHR_FUNC_EXIT;
}

/****************************************************************************************
 * Packet Stop
 * }
 */

#ifdef SAL_THREAD_NAME_PRINT_DISABLE
/****************************************************************************************
 * Packet Parse
 * {
 */
static bcm_rx_t
dnx_sand_packet_receive_parser(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
    int i;
    pparse_packet_info_t packet_info;
    rhlist_t *pit = NULL;
    parsing_db_t pdb;
    int up32_size;
    uint32 *src;
    int header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
    int system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    LOG_CLI(("Packet with size:%d received with dest GPORT:0x%08x\n", pkt->tot_len, pkt->dst_gport));

    up32_size = (pkt->pkt_len + 3) / 4;
    src = (uint32 *) pkt->_pkt_data.data;
    for (i = 0; i < up32_size; i++)
    {
        packet_info.data[up32_size - (i + 1)] = bcm_ntohl(src[i]);
    }
    packet_info.data_size = up32_size * 32;
    packet_info.gport = pkt->dst_gport;

    if (BCM_GPORT_IS_LOCAL(pkt->dst_gport))
    {
        header_type_out =
            dnx_data_port.static_add.header_type_get(unit, BCM_GPORT_LOCAL_GET(pkt->dst_gport))->header_type_out;
    }
    if (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_CPU)
    {
        /*
         *Jericho mode
         */
        if (system_headers_mode == 0)
        {
            packet_info.data_desc = "FTMH_J1";
        }
        else
        {
            packet_info.data_desc = "FTMH_Base";
        }
    }
    else
    {
        packet_info.data_desc = "ETH";
    }

    if (pparse_init(unit, &pdb) != _SHR_E_NONE)
    {
        LOG_CLI(("Failed to init parser on rx packet\n"));
    }
    else
    {
        pparse_packet_process(unit, 0, &pdb, &packet_info, &pit);
    }

    packet_decode_info_print(unit, pit, NULL);

    return BCM_RX_NOT_HANDLED;
}

static shr_error_e
packet_parser_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_register
                    (unit, "packet parser", dnx_sand_packet_receive_parser, BCM_RX_PRIO_MAX, (void *) NULL,
                     BCM_RCO_F_ALL_COS));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
packet_parser_stop(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Wait a little for other threads to process
     */
    sal_usleep(50000);
    SHR_IF_ERR_EXIT(bcm_rx_unregister(unit, dnx_sand_packet_receive_parser, BCM_RX_PRIO_MAX));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sh_sand_packet_parser_on_man = {
    .brief = "Packet Parsing on.",
    .examples = "\n"
};

sh_sand_man_t sh_sand_packet_parser_off_man = {
    .brief = "Packet Parsing off.",
    .examples = "\n"
};

sh_sand_man_t sh_sand_packet_parser_man = {
    .brief = "Packet Parse tool.",
    .full  = "This command parses currect received packet using an external database for objects in the packet.",
    .examples = "on\n" "off\n"
};

sh_sand_cmd_t sh_sand_packet_parse_cmds[] = {
  /* Name     | Leaf Action       | Node | Options for Leaf       | Usage              | option resolution callback
     */
    {"on",   packet_parser_start,    NULL,   NULL,                   &sh_sand_packet_parser_on_man},
    {"off",  packet_parser_stop,     NULL,   NULL,                   &sh_sand_packet_parser_off_man},
    {NULL}
};
/* *INDENT-ON* */
/****************************************************************************************
 * Packet Parse
 * }
 */
#endif

/**
 * \brief - Man page for 'packet' shell command
 */
/* *INDENT-OFF* */
sh_sand_man_t sh_sand_packet_man = {
    .brief = "Packet management tool.",
    .full  = "This command parses/composes/sends raw packet data, using an external database for objects in the packet.",
};

sh_sand_cmd_t sh_sand_packet_cmds[] = {
  /* Name     | Leaf Action       | Node | Options for Leaf       | Usage              | option resolution callback
     */
    {"decode",  packet_decode_cmd,  NULL, NULL,                     &packet_decode_man,  packet_decode_option_get},
    {"compose", packet_compose_cmd, NULL, packet_compose_arguments, &packet_compose_man, packet_compose_option_get},
    {"combine", packet_combine_cmd, NULL, packet_combine_arguments, &packet_combine_man, packet_combine_option_get},
    {"list",    packet_list_cmd,    NULL, NULL,                     &packet_list_man},
    {"tx",      packet_tx_cmd,      NULL, packet_tx_arguments,      &packet_tx_man,      packet_compose_option_get},
    {"rx",      packet_rx_cmd,      NULL, packet_rx_arguments,      &packet_rx_man,      packet_compose_option_get},
    {"show",    packet_show_cmd,    NULL, NULL,                     &packet_show_man},
    {"stop",    packet_stop_cmd,    NULL, packet_stop_arguments,    &packet_stop_man},
#ifdef SAL_THREAD_NAME_PRINT_DISABLE
    {"parser",  NULL,   sh_sand_packet_parse_cmds,  NULL,           &sh_sand_packet_parser_man},
#endif
    {NULL}
};
/* *INDENT-ON* */

/*
 * This routine is for DPP/DFE only
 */
cmd_result_t
cmd_sand_packet(
    int unit,
    args_t * args)
{
    cmd_result_t result;

    SH_SAND_VERIFY("PacKeT", sh_sand_packet_cmds, result);

    sh_sand_act(unit, args, sh_sand_packet_cmds, sh_sand_sys_cmds, NULL);
    ARG_DISCARD(args);
    /*
     * Always return OK - we provide all help & usage from inside framework
     */
exit:
    return result;
}

/*
 * General shell style usage
 */
const char cmd_sand_packet_usage[] = "Please use \"packet dcd usage\" for help\n";
