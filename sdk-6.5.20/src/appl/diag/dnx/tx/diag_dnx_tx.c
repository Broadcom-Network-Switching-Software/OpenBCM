/** \file diag_dnx_tx.c
 *
 * Main diagnostics for tx applications
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

/*************
 * INCLUDES  *
 */
#include "diag_dnx_tx.h"
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/enet.h>

/*************
 * TYPEDEFS  *
 */

/*
 * CRC mode
 */
typedef enum
{
    CRC_NONE,
    CRC_RECOMPUTE,
    CRC_APPEND,
} sh_dnx_tx_crc_mode_e;

/*
 * ITMH destination encoding type
 */
typedef enum
{
    FWD_TYPE_NONE,
    FWD_TYPE_FEC,
    FWD_TYPE_SPA,
    FWD_TYPE_ACT_PROFILE,
    FWD_TYPE_TMFLOW,
    FWD_TYPE_MCID,
} sh_dnx_tx_fwd_type_e;

/*
 * TX description structure
 */
typedef struct xd_s
{
    int xd_unit;                /* Current Unit */
    int xd_tx_unit;             /* TX device */
    uint32 xd_tot_cnt;          /* # to send/receive */
    uint32 xd_cur_cnt;          /* # sent/received */
    uint32 xd_pkt_len;          /* Packet length */
    char *xd_file;              /* File name of packet data */
    char *xd_data;              /* Packet data */
    bcm_mac_t xd_mac_dst;       /* Destination mac address */
    bcm_mac_t xd_mac_src;       /* Source mac address */
    uint32 xd_mac_dst_inc;      /* Destination mac increment */
    uint32 xd_mac_src_inc;      /* Source mac increment */
    uint32 xd_pat;              /* XMIT pattern */
    uint32 xd_pat_inc;          /* Pattern Increment */
    int xd_pat_random;          /* Use Random Pattern */
    uint32 xd_tpid;             /* TPID */
    uint32 xd_vlan;             /* Vlan id (0 if untagged) */
    uint32 xd_prio;             /* Vlan prio */
    sh_dnx_tx_crc_mode_e xd_crc;        /* CRC mode */
    bcm_pbmp_t tx_pbmp;         /* Target ports. */
    bcm_pbmp_t tx_upbmp;        /* Untagged target ports. */
    uint32 xd_visibility;
    uint32 xd_in_tm_port;       /* Speified in TM port */
    uint32 xd_ptch_src_port;    /* Speified in PP port as PTCH src system port */
    uint32 xd_ptch_header_2;    /* Speified whole PTCH_2 Header */
    /*
     * Options for ITMH header
     */
    uint32 xd_itmh_present;
    uint32 itmh_fwd_action_strength;
    uint32 itmh_as_ext_present;
    uint32 itmh_pph_type;
    uint32 itmh_in_mirr_disable;
    uint32 itmh_dp;
    sh_dnx_tx_fwd_type_e itmh_fwd_type;
    uint32 itmh_fec_ptr;
    uint32 itmh_action_profile_idx;
    uint32 itmh_dest_sys_port;
    uint32 itmh_flow_id;
    uint32 itmh_multicast_id;
    uint32 itmh_snoop_profile;
    uint32 itmh_traffic_class;
    uint32 itmh_base_ext_present;
    uint32 itmh_base_ext_type;
    uint32 itmh_base_ext_value;
} xd_t;

/*
 * ITMH base header
 */
typedef union sh_dnx_tx_itmh_base_u
{
    struct
    {
        uint32 bytes[2];
    } raw;
    struct
    {
#if defined(LE_HOST)
        uint32 inbound_mirror_disable:1;
        uint32 pph_type:2;
        uint32 injected_as_extension_present:1;
        uint32 forward_action_strength:3;
        uint32 reserved:1;
        uint32 destination_hi:6;
        uint32 drop_precedence:2;
        uint32 destination_mi:8;
        uint32 snoop_profile_hi:1;
        uint32 destination_lo:7;
        uint32 itmh_base_ext_present:1;
        uint32 traffic_class:3;
        uint32 snoop_profile_lo:4;
#else
        uint32 reserved:1;
        uint32 forward_action_strength:3;
        uint32 injected_as_extension_present:1;
        uint32 pph_type:2;
        uint32 inbound_mirror_disable:1;
        uint32 drop_precedence:2;
        uint32 destination_hi:6;
        uint32 destination_mi:8;
        uint32 destination_lo:7;
        uint32 snoop_profile_hi:1;
        uint32 snoop_profile_lo:4;
        uint32 traffic_class:3;
        uint32 itmh_base_ext_present:1;
#endif
        uint32 __COMMON_FIELDS__:24;

    } jr2_common;

    struct
    {
        uint32 _rsvd_0:8;
#if defined(LE_HOST)
        uint32 destination_hi:6;
        uint32 _rsvd_1:2;
        uint32 destination_mi:8;
        uint32 _rsvd_2:1;
        uint32 destination_lo:7;
#else
        uint32 _rsvd_1:2;
        uint32 destination_hi:6;
        uint32 destination_mi:8;
        uint32 destination_lo:7;
        uint32 _rsvd_2:1;
#endif
        uint32 _rsvd_3:8;
        uint32 __COMMON_FIELDS__:24;
    } jr2_dest_info;

} sh_dnx_tx_itmh_base_t;

typedef union sh_dnx_tx_itmh_base_ext_u
{
    struct
    {
        uint32 bytes[1];
    } raw;

    struct
    {
#if defined(LE_HOST)
        uint32 value1_hi:6;
        uint32 type:1;
        uint32 reserved:1;
        uint32 value1_mi:8;
        uint32 value1_lo:8;
#else
        uint32 reserved:1;
        uint32 type:1;
        uint32 value1_hi:6;
        uint32 value1_mi:8;
        uint32 value1_lo:8;
#endif
        uint32 __COMMON_FIELDS__:8;
    } base_ext;
} sh_dnx_tx_itmh_base_ext_t;

typedef struct sh_dnx_tx_itmh_s
{
    /*
     * ITMH base header
     */
    sh_dnx_tx_itmh_base_t base;
    /*
     * ITMH base ext header
     */
    sh_dnx_tx_itmh_base_ext_t base_ext;
} sh_dnx_tx_itmh_t;

/*
 * ITMH base header on JR1 mode
 */
typedef union sh_dnx_tx_itmh_base_jr1_u
{
    struct
    {
        uint32 bytes[1];
    } raw;
    struct
    {
#if defined(LE_HOST)
        uint32 fwd_type_hi:3;
        uint32 dp:2;
        uint32 in_mirr_flag:1;
        uint32 pph_type:2;
        uint32 fwd_type_lo:16;
        uint32 itmh_base_ext:1;
        uint32 traffic_class_lo:2;
        uint32 traffic_class_hi:1;
        uint32 snoop_cmd:4;
#else
        uint32 pph_type:2;
        uint32 in_mirr_flag:1;
        uint32 dp:2;
        uint32 fwd_type_hi:3;
        uint32 fwd_type_lo:16;
        uint32 snoop_cmd:4;
        uint32 traffic_class_hi:1;
        uint32 traffic_class_lo:2;
        uint32 itmh_base_ext:1;
#endif
    } jer_common;

    struct
    {
#if defined(LE_HOST)
        uint32 dest_info_hi:3;
        uint32 _rsvd_0:5;
        uint32 dest_info_mi:8;
        uint32 dest_info_lo:8;
#else
        uint32 _rsvd_0:5;
        uint32 dest_info_hi:3;
        uint32 dest_info_mi:8;
        uint32 dest_info_lo:8;
#endif
        uint32 _rsvd_1:8;
    } jer_dest_info;
} sh_dnx_tx_itmh_base_jr1_t;

typedef union sh_dnx_tx_itmh_base_ext_jr1_u
{
    struct
    {
        uint32 bytes[1];
    } raw;

    struct
    {
#if defined(LE_HOST)
        uint32 value1_hi:4;
        uint32 reserved:1;
        uint32 type:3;
        uint32 value1_mi:8;
        uint32 value1_lo:8;
#else
        uint32 type:3;
        uint32 reserved:1;
        uint32 value1_hi:4;
        uint32 value1_mi:8;
        uint32 value1_lo:8;
#endif
        uint32 __COMMON_FIELDS__:8;
    } base_ext;
} sh_dnx_tx_itmh_base_ext_jr1_t;

typedef struct sh_dnx_tx_itmh_jr1_s
{
    /*
     * ITMH base header on JR1 mode
     */
    sh_dnx_tx_itmh_base_jr1_t base;
    /*
     * ITMH base ext header on JR1 mode
     */
    sh_dnx_tx_itmh_base_ext_jr1_t base_ext;
} sh_dnx_tx_itmh_jr1_t;

static xd_t *xd_units[SOC_MAX_NUM_DEVICES];

#define TAGGED_PACKET_LENGTH           68
#define UNTAGGED_PACKET_LENGTH         64

#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
/** Size of module header on DNX devices, it should be written at the start of the packet */
#define DNX_MODULE_HEADER_SIZE         16
#else
/** No Module Header on ADAPTER and non DNX*/
#define DNX_MODULE_HEADER_SIZE         0
#endif

#define SH_DNX_TX_PTCH_TYPE2_LEN                   2
#define SH_DNX_TX_ITMH_BASE_LEN                    5
#define SH_DNX_TX_ITMH_BASE_EXT_LEN                3
#define SH_DNX_TX_ITMH_BASE_LEN_JR1                4
#define SH_DNX_TX_ITMH_BASE_EXT_LEN_JR1            3

/** ITMH Destination Encoding(21)  */
/** FEC pointer(20)*/
#define SH_DNX_TX_ITMH_FWD_TYPE_FEC_POINTER        1
/** System Port Agg(16)*/
#define SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_DIRECT     2
/** SNS(3), FWS(4), Action-Priofile-IDX(9) */
#define SH_DNX_TX_ITMH_FWD_TYPE_ACTION_PROFILE_IDX 3
/** TM-Flow(19) */
#define SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_FLOW       4
/** MC-ID(19) */
#define SH_DNX_TX_ITMH_FWD_TYPE_SYSTEM_MULTICAST   5

/* *INDENT-OFF* */
static sh_sand_enum_t sh_dnx_tx_crc_mode[] = {
    {"None", CRC_NONE, "None"},
    {"Recompute", CRC_RECOMPUTE, "Recompute CRC at the last 4 bytes of packet data"},
    {"Append", CRC_APPEND, "Append 4 bytes CRC to the end of packet data"},
    {NULL}
};

static sh_sand_enum_t sh_dnx_tx_fwd_type[] = {
    {"None", FWD_TYPE_NONE, "None"},
    {"Fec", FWD_TYPE_FEC, "Fec pointer in destination encoding"},
    {"SPA", FWD_TYPE_SPA, "Source system port in destination encoding"},
    {"Action", FWD_TYPE_ACT_PROFILE, "Action profile index in destination encoding"},
    {"TMFlow", FWD_TYPE_TMFLOW, "TM flow in destination encoding"},
    {"MCID", FWD_TYPE_MCID, "Mulitcast ID in destination encoding"},
    {NULL}
};

sh_sand_option_t sh_dnx_tx_options[] = {
    {"CouNT", SAL_FIELD_TYPE_UINT32, "Number", "1", NULL, NULL, SH_SAND_ARG_FREE},
    {"UnTagged", SAL_FIELD_TYPE_BOOL, "Specify packet should be sent untagged", "No"},
    {"TXUnit", SAL_FIELD_TYPE_INT32, "Transmit unit number", "-1"},
    {"PortBitMap", SAL_FIELD_TYPE_PORT, "Specify port bitmap packet is sent to", "none"},
    {"UntagBitMap", SAL_FIELD_TYPE_PORT, "Specify untag bitmap used for DMA", "none"},
    {"FILENaMe", SAL_FIELD_TYPE_STR, "Load hex packet data from file", ""},
    {"DaTa", SAL_FIELD_TYPE_STR, "Packet value (Hex)", ""},
    {"LENgth", SAL_FIELD_TYPE_UINT32, "Specify the total length of the packet", "68"},
    {"TagProtocolIdentifier", SAL_FIELD_TYPE_UINT32, "Specify TPID", "0x8100"},
    {"VLan", SAL_FIELD_TYPE_UINT32, "Specify the VLAN tag used", "1"},
    {"VlanPrio", SAL_FIELD_TYPE_UINT32, "VLAN Priority", "0"},
    {"PatTeRN", SAL_FIELD_TYPE_UINT32, "Specify 32-bit pattern data", "0x12345678"},
    {"PatternINC", SAL_FIELD_TYPE_UINT32, "Specify increment of pattern data", "1"},
    {"PatternRandom", SAL_FIELD_TYPE_BOOL, "Specify packet shoudl use Random data pattern", "No"},
    {"SourceMac", SAL_FIELD_TYPE_MAC, "Specify source MAC address", "00:00:00:07:00:00"},
    {"SourceMacInc", SAL_FIELD_TYPE_UINT32, "Specify increment of source MAC", "0"},
    {"DestMac", SAL_FIELD_TYPE_MAC, "Specify destination MAC address", "00:00:00:e3:00:00"},
    {"DestMacInc", SAL_FIELD_TYPE_UINT32, "Specify increment of destination MAC", "0"},
    {"CRC", SAL_FIELD_TYPE_ENUM, "Specify CRC mode", "Recompute", sh_dnx_tx_crc_mode},
    {"PORT", SAL_FIELD_TYPE_UINT32, "Specify source TM PORT", "0"},
    {"VISibility", SAL_FIELD_TYPE_BOOL, "Specify visibility resume, default is enabled", "Yes"},
    {"PtchHeader", SAL_FIELD_TYPE_UINT32, "Specify the whole PTCH header", "0"},
    {"PtchSRCport", SAL_FIELD_TYPE_UINT32, "Specify source PP PORT", "0"},
    {"ItmhPRESent", SAL_FIELD_TYPE_BOOL, "Indicate ITMH header is present", "No"},
    {"FwdActStrength", SAL_FIELD_TYPE_UINT32, "Specify forward action strength", "0"},
    {"AsExtPresent", SAL_FIELD_TYPE_BOOL, "Indicate FTMH Application Specific Extension", "No"},
    {"PphType", SAL_FIELD_TYPE_UINT32, "Specify type of PPH", "0"},
    {"InMirrDisable", SAL_FIELD_TYPE_BOOL, "Indicate to disable in-bound port mirroring", "No"},
    {"DropPrecedence", SAL_FIELD_TYPE_UINT32, "Specify drop precedence", "0"},
    {"ForWarD", SAL_FIELD_TYPE_ENUM, "Specify Forward_Type in ITMH destination", "None", sh_dnx_tx_fwd_type},
    {"FEC", SAL_FIELD_TYPE_UINT32, "Specify Fec pointer id", "0"},
    {"ACTion", SAL_FIELD_TYPE_UINT32, "Specify action profile idx including SNS,FWS", "0"},
    {"DeSTination", SAL_FIELD_TYPE_UINT32, "Specify destination system port", "0"},
    {"FlowId", SAL_FIELD_TYPE_UINT32, "Specify unicast flow id ", "0"},
    {"MultiCast", SAL_FIELD_TYPE_UINT32, "Specify multicast ID identifying group", "0"},
    {"SnoopProfile", SAL_FIELD_TYPE_UINT32, "Specify snoop cmds", "0"},
    {"TrafficClass", SAL_FIELD_TYPE_UINT32, "Specify TM traffic class", "0"},
    {"ItmhBaseExtPresent", SAL_FIELD_TYPE_BOOL, "Indicate ITMH Base Extension header is present", "No"},
    {"ItmhBaseExtType", SAL_FIELD_TYPE_UINT32, "Specify ITMH base extention type", "0"},
    {"ItmhBaseExtValue", SAL_FIELD_TYPE_UINT32, "ITMH base extention value", "0"},
    {NULL}
};

sh_sand_man_t sh_dnx_tx_man = {
    .brief =    "Transmit packets",
    .full =     "Transmit the specified number of packets with specified content.",
    .synopsis = NULL,   /* NULL means autogenerated */
    .examples = "1 PtchSRCport=200\n"
                "10 PtchSRCport=201 LENgth=80\n"
                "1 PtchSRCport=202 DaTa=0x11223344abcd000000000001810000010899111111111111111111112222222222222222222233333333333333333333444444444444444444445555555555555555555566666666666666666666aabbccddeeffaabbccdd"
};
/* *INDENT-ON* */

/*************
* FUNCTIONS *
 */

/**
 * \brief - Init TX description structure
 * \param [in] unit - Unit #
 * \return
 *      Standard error handling.
 */
static shr_error_e
sh_dnx_xd_init(
    int unit)
{
    xd_t *xd;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Allocate xd struct at the first call
     */
    xd = xd_units[unit];
    if (xd == NULL)
    {
        xd = sal_alloc(sizeof(xd_t), "xd");
        if (xd == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for XD structure\n");
        }
        xd_units[unit] = xd;
    }

    sal_memset(xd, 0, sizeof(xd_t));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Parse TX command from BCM shell
 * \param [in] unit - Unit #
 * \param [in] args - Command line arguments, may be NULL if called from outside the shell
 * \param [in] sand_control - Shell command control structure
 * \param [out] xd - transmit description from arguments
 * \return
 *      Standard error handling.
 */
static shr_error_e
sh_dnx_tx_parse(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    xd_t * xd)
{
    int untagged = 0;
    char *xfile = NULL;
    char *xdata = NULL;

    SHR_FUNC_INIT_VARS(unit);

    xd->xd_unit = unit;
    SH_SAND_GET_UINT32("CouNT", xd->xd_tot_cnt);
    SH_SAND_GET_BOOL("UnTagged", untagged);
    SH_SAND_GET_INT32("TXUnit", xd->xd_tx_unit);
    SH_SAND_GET_PORT("PortBitMap", xd->tx_pbmp);
    SH_SAND_GET_PORT("UntagBitMap", xd->tx_upbmp);
    SH_SAND_GET_STR("FILENaMe", xd->xd_file);
    SH_SAND_GET_STR("DaTa", xd->xd_data);
    SH_SAND_GET_UINT32("LENgth", xd->xd_pkt_len);
    SH_SAND_GET_UINT32("TagProtocolIdentifier", xd->xd_tpid);
    SH_SAND_GET_UINT32("VLan", xd->xd_vlan);
    SH_SAND_GET_UINT32("VlanPrio", xd->xd_prio);
    SH_SAND_GET_UINT32("PatTeRN", xd->xd_pat);
    SH_SAND_GET_UINT32("PatternINC", xd->xd_pat_inc);
    SH_SAND_GET_BOOL("PatternRandom", xd->xd_pat_random);
    SH_SAND_GET_MAC("SourceMac", xd->xd_mac_src);
    SH_SAND_GET_UINT32("SourceMacInc", xd->xd_mac_src_inc);
    SH_SAND_GET_MAC("DestMac", xd->xd_mac_dst);
    SH_SAND_GET_UINT32("DestMacInc", xd->xd_mac_dst_inc);
    SH_SAND_GET_ENUM("CRC", xd->xd_crc);
    SH_SAND_GET_UINT32("PORT", xd->xd_in_tm_port);
    SH_SAND_GET_BOOL("VISibility", xd->xd_visibility);
    /*
     * options for PTCH header
     */
    SH_SAND_GET_UINT32("PtchHeader", xd->xd_ptch_header_2);
    SH_SAND_GET_UINT32("PtchSRCport", xd->xd_ptch_src_port);
    /*
     * options for ITMH header
     */
    SH_SAND_GET_BOOL("ItmhPRESent", xd->xd_itmh_present);
    SH_SAND_GET_UINT32("FwdActStrength", xd->itmh_fwd_action_strength);
    SH_SAND_GET_BOOL("AsExtPresent", xd->itmh_as_ext_present);
    SH_SAND_GET_UINT32("PphType", xd->itmh_pph_type);
    SH_SAND_GET_BOOL("InMirrDisable", xd->itmh_in_mirr_disable);
    SH_SAND_GET_UINT32("DropPrecedence", xd->itmh_dp);
    SH_SAND_GET_ENUM("ForWarD", xd->itmh_fwd_type);
    SH_SAND_GET_UINT32("FEC", xd->itmh_fec_ptr);
    SH_SAND_GET_UINT32("DeSTination", xd->itmh_dest_sys_port);
    SH_SAND_GET_UINT32("ACTion", xd->itmh_action_profile_idx);
    SH_SAND_GET_UINT32("FlowId", xd->itmh_flow_id);
    SH_SAND_GET_UINT32("MultiCast", xd->itmh_multicast_id);
    SH_SAND_GET_UINT32("SnoopProfile", xd->itmh_snoop_profile);
    SH_SAND_GET_UINT32("TrafficClass", xd->itmh_traffic_class);
    SH_SAND_GET_BOOL("ItmhBaseExtPresent", xd->itmh_base_ext_present);
    SH_SAND_GET_UINT32("ItmhBaseExtType", xd->itmh_base_ext_type);
    SH_SAND_GET_UINT32("ItmhBaseExtValue", xd->itmh_base_ext_value);

    if (xd->xd_tx_unit == -1)
    {
        xd->xd_tx_unit = unit;
    }
    if (!ISEMPTY(xd->xd_file))
    {
        xfile = sal_strdup(xd->xd_file);
    }
    else
    {
        xfile = NULL;
    }
    if (!ISEMPTY(xd->xd_data))
    {
        xdata = sal_strdup(xd->xd_data);
    }
    else
    {
        xdata = NULL;
    }
    xd->xd_file = xfile;
    xd->xd_data = xdata;
    /*
     * Force to untagged if option "UnTagged" is specified
     */
    if (untagged)
    {
        xd->xd_vlan = 0;
        xd->xd_prio = 0;
    }

    /*
     * Specify destination as unicast then update Destination via sh_dnx_tx_itmh_dest_port_set() later
     */
    if (BCM_PBMP_NOT_NULL(xd->tx_pbmp))
    {
        xd->itmh_fwd_type = SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_DIRECT;
    }

    /*
     * Verify ITMH parameters
     */
    switch (xd->itmh_fwd_type)
    {
        case SH_DNX_TX_ITMH_FWD_TYPE_FEC_POINTER:
            if (xd->itmh_fec_ptr == 0)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "FEC is not given when ForWarD is Fec\n");
            }
            if ((xd->itmh_fec_ptr & 0xc0000) == 0xc0000)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "FEC is out of range when Forward_Type is Fec\n");
            }
            break;
        case SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_DIRECT:
            break;
        case SH_DNX_TX_ITMH_FWD_TYPE_ACTION_PROFILE_IDX:
            if (xd->itmh_action_profile_idx == 0)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "ACTion is not given when ForWarD is Action\n");
            }
            break;
        case SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_FLOW:
            if (xd->itmh_flow_id == 0)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "FlowId is not given when ForWarD is TMFlow\n");
            }
            break;
        case SH_DNX_TX_ITMH_FWD_TYPE_SYSTEM_MULTICAST:
            if (xd->itmh_multicast_id == 0)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "MultiCast is not given when ForWarD is MCID\n");
            }
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

#ifndef NO_FILEIO
#define TX_LOAD_MAX             4096

/**
 * \brief
 *   Discard args single line from input file
 * \param [in] fp - Input file pointer
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
sh_dnx_discard_line(
    FILE * fp)
{
    int c;
    do
    {
        if ((c = getc(fp)) == EOF)
        {
            return c;
        }
    }
    while (c != '\n');
    return c;
}

/**
 * \brief
 *   Load a single byte from input file
 * \param [in] fp - Input file pointer
 * \param [out] byte - One byte of data
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
sh_dnx_tx_load_byte(
    FILE * fp,
    uint8 *byte)
{
    int c, d;

    do
    {
        if ((c = getc(fp)) == EOF)
        {
            return -1;
        }
        else if (c == '#')
        {
            if ((c = sh_dnx_discard_line(fp)) == EOF)
            {
                return -1;
            }
        }
    }
    while (!isxdigit(c));

    do
    {
        if ((d = getc(fp)) == EOF)
        {
            return -1;
        }
        else if (d == '#')
        {
            if ((d = sh_dnx_discard_line(fp)) == EOF)
            {
                return -1;
            }
        }
    }
    while (!isxdigit(d));

    *byte = (xdigit2i(c) << 4) | xdigit2i(d);

    return 0;
}

/**
 * \brief
 *   Load full packet from file and calc data length.
 * \param [in] unit - Unit #
 * \param [in] fname - Input file name
 * \param [in,out] length - Packet data length
 * \param [out] packet_data - Packet data
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_tx_load_packet(
    int unit,
    char *fname,
    uint32 *length,
    uint8 *packet_data)
{
    uint8 *p = packet_data;
    FILE *fp;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fname, _SHR_E_PARAM, "fname");
    SHR_NULL_CHECK(length, _SHR_E_PARAM, "length");
    SHR_NULL_CHECK(packet_data, _SHR_E_MEMORY, "packet_data");

    if ((fp = sal_fopen(fname, "r")) == NULL)
    {
        sal_free(p);
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Fail to open file %s\n", fname);
    }

    for (i = 0; i < TX_LOAD_MAX; i++)
    {
        if (sh_dnx_tx_load_byte(fp, &p[i]) < 0)
            break;
    }

    *length = i;
    sal_fclose(fp);

exit:
    SHR_FUNC_EXIT;
}
#endif /* NO_FILEIO */

/**
 * \brief
 *   Translates input data string (in hex) to a uint8 array containing the data.
 *   Calc data length
 * \param [in] unit - Unit #
 * \param [in] packet_string - Input packet string
 * \param [in,out] length - Packet data length
 * \param [in] packet_data - Output Packet data
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_parse_user_packet_payload(
    int unit,
    char *packet_string,
    uint32 *length,
    uint8 *packet_data)
{
    uint8 *p = packet_data;
    char tmp, data_iter;
    int data_len, byte_len, i, pkt_len, data_base;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_string, _SHR_E_PARAM, "packet_string");
    SHR_NULL_CHECK(length, _SHR_E_PARAM, "length");
    SHR_NULL_CHECK(packet_data, _SHR_E_MEMORY, "packet_data");
    /*
     * If string data starts with 0x or 0X, skip it
     */
    if ((packet_string[0] == '0') && ((packet_string[1] == 'x') || (packet_string[1] == 'X')))
    {
        data_base = 2;
    }
    else
    {
        data_base = 0;
    }

    data_len = strlen(packet_string) - data_base;
    byte_len = (data_len + 1) / 2;
    pkt_len = byte_len > *length ? byte_len : *length;

    /*
     * Convert char to value
     */
    i = 0;
    while (i < data_len)
    {
        data_iter = packet_string[data_base + i];
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
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unexpected char: %c\n", data_iter);
        }

        /*
         * String input is in 4b unit. Below we're filling in 8b:
         * offset is /2, and we shift by 4b if the input char is odd
         */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
    }

    *length = pkt_len;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check Eth header in packet data
 * \param [in,out] xd - Transmit description structure
 * \param [in,out] packet_data - Packet data
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
sh_dnx_check_pkt_fields(
    xd_t * xd,
    uint8 *packet_data)
{
    enet_hdr_t *ep;
    uint16 tpid;

    ep = (enet_hdr_t *) packet_data;
    if (!ISEMPTY(xd->xd_file) || !ISEMPTY(xd->xd_data))
    {
        /*
         * Reset Src MAC, Dst MAC, VLAN ID, VLAN priority per file or string input
         */
        ENET_COPY_MACADDR(&ep->en_dhost, xd->xd_mac_dst);
        ENET_COPY_MACADDR(&ep->en_shost, xd->xd_mac_src);

        tpid = soc_ntohs(ep->en_tag_tpid);
        if (tpid != xd->xd_tpid)
        {
            xd->xd_vlan = VLAN_ID_NONE;
            xd->xd_prio = 0;
        }
        else
        {
            xd->xd_vlan = VLAN_CTRL_ID(bcm_ntohs(ep->en_tag_ctrl));
            xd->xd_prio = VLAN_CTRL_PRIO(bcm_ntohs(ep->en_tag_ctrl));
        }
    }
    else
    {
        if (xd->xd_vlan)
        {
            ep->en_tag_tpid = bcm_htons(xd->xd_tpid);
            ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(xd->xd_prio, 0, xd->xd_vlan));
            /*
             * Ethernet type is set to length of Ethernet packet
             */
            if (xd->xd_crc == 1)
            {
                ep->en_tag_len = bcm_htons(xd->xd_pkt_len) - ENET_TAGGED_HDR_LEN - ENET_CHECKSUM_LEN;
            }
            else
            {
                ep->en_tag_len = bcm_htons(xd->xd_pkt_len) - ENET_TAGGED_HDR_LEN;
            }
        }
        else
        {
            if (xd->xd_crc == 1)
            {
                ep->en_untagged_len = bcm_htons(xd->xd_pkt_len) - ENET_UNTAGGED_HDR_LEN - ENET_CHECKSUM_LEN;
            }
            else
            {
                ep->en_untagged_len = bcm_htons(xd->xd_pkt_len) - ENET_UNTAGGED_HDR_LEN;
            }
        }
        ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
        ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);
    }

    return;
}

/**
 * \brief
 *   Build PTCH header
 * \param [in] xd - Transmit description structure
 * \param [out] ptch_header - Buffer pointer to PTCH header
 * \param [in] ptch_ssp - PTCH source system port
 * \param [in] header_type_incoming - incoming tm port header type, use JR1 format if INJECTED_2_JR1_MODE, INJECTED_2_PP_JR1_MODE
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
sh_dnx_tx_setup_ptch(
    xd_t * xd,
    uint8 *ptch_header,
    bcm_port_t ptch_ssp,
    int header_type_incoming)
{
    if (xd->xd_ptch_header_2)
    {
        ptch_header[0] = (xd->xd_ptch_header_2 >> 8) & 0xff;
        ptch_header[1] = xd->xd_ptch_header_2 & 0xff;
    }
    else
    {
        if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)
            || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
        {
            ptch_header[0] = 0x0;
            ptch_header[1] = ptch_ssp & 0xff;
        }
        else
        {
            ptch_header[0] = (ptch_ssp >> 8) & 0x3;
            ptch_header[1] = ptch_ssp & 0xff;
        }
    }

    if (BCM_PBMP_IS_NULL(xd->tx_pbmp) && !xd->xd_itmh_present)
    {
        ptch_header[0] |= 0x80;
    }

    return;
}

/**
 * \brief
 *   Build ITMH header
 * \param [in] xd - Transmit description structure
 * \param [out] itmh_header - Buffer pointer to ITMH header
 * \param [in] header_type_incoming - incoming tm port header type, use JR1 format if INJECTED_2_JR1_MODE, INJECTED_2_PP_JR1_MODE
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
sh_dnx_tx_setup_itmh(
    xd_t * xd,
    uint8 *itmh_header,
    int header_type_incoming)
{
    if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
    {
        sh_dnx_tx_itmh_jr1_t itmh;

        sal_memset(&itmh, 0, sizeof(sh_dnx_tx_itmh_jr1_t));
        itmh.base.jer_common.pph_type = xd->itmh_pph_type;
        itmh.base.jer_common.in_mirr_flag = xd->itmh_in_mirr_disable;
        itmh.base.jer_common.dp = xd->itmh_dp;

        if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_DIRECT)
        {
            itmh.base.jer_dest_info.dest_info_hi = 0x1;
            itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_dest_sys_port >> 8) & 0xff;
            itmh.base.jer_dest_info.dest_info_lo = xd->itmh_dest_sys_port & 0xff;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_FLOW)
        {
            itmh.base.jer_dest_info.dest_info_hi = 0x6 | ((xd->itmh_flow_id >> 16) & 0x1);
            itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_flow_id >> 8) & 0xff;
            itmh.base.jer_dest_info.dest_info_lo = xd->itmh_flow_id & 0xff;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_FEC_POINTER)
        {
            itmh.base.jer_dest_info.dest_info_hi = 0x2 | ((xd->itmh_fec_ptr >> 16) & 0x1);
            itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_fec_ptr >> 8) & 0xff;
            itmh.base.jer_dest_info.dest_info_lo = xd->itmh_fec_ptr & 0xff;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_SYSTEM_MULTICAST)
        {
            itmh.base.jer_dest_info.dest_info_hi = 0x4 | ((xd->itmh_multicast_id >> 16) & 0x1);
            itmh.base.jer_dest_info.dest_info_mi = (xd->itmh_multicast_id >> 8) & 0xff;
            itmh.base.jer_dest_info.dest_info_lo = xd->itmh_multicast_id & 0xff;
        }

        itmh.base.jer_common.snoop_cmd = xd->itmh_snoop_profile;
        itmh.base.jer_common.traffic_class_hi = (xd->itmh_traffic_class >> 2) & 0x1;
        itmh.base.jer_common.traffic_class_lo = xd->itmh_traffic_class & 0x3;
        itmh.base.jer_common.itmh_base_ext = xd->itmh_base_ext_present;

        sal_memcpy(itmh_header, (uint8 *) itmh.base.raw.bytes, SH_DNX_TX_ITMH_BASE_LEN_JR1);

        if (xd->itmh_base_ext_present)
        {
            itmh.base_ext.base_ext.type = xd->itmh_base_ext_type;
            itmh.base_ext.base_ext.value1_hi = (xd->itmh_base_ext_value >> 16) & 0x3f;
            itmh.base_ext.base_ext.value1_mi = (xd->itmh_base_ext_value >> 8) & 0xff;
            itmh.base_ext.base_ext.value1_lo = xd->itmh_base_ext_value & 0xff;
            sal_memcpy(itmh_header + SH_DNX_TX_ITMH_BASE_LEN_JR1, (uint8 *) itmh.base_ext.raw.bytes,
                       SH_DNX_TX_ITMH_BASE_EXT_LEN_JR1);
        }
    }
    else
    {
        sh_dnx_tx_itmh_t itmh;

        sal_memset(&itmh, 0, sizeof(sh_dnx_tx_itmh_t));
        itmh.base.jr2_common.forward_action_strength = xd->itmh_fwd_action_strength;
        itmh.base.jr2_common.injected_as_extension_present = xd->itmh_as_ext_present;
        itmh.base.jr2_common.pph_type = xd->itmh_pph_type;
        itmh.base.jr2_common.inbound_mirror_disable = xd->itmh_in_mirr_disable;
        itmh.base.jr2_common.snoop_profile_hi = xd->itmh_snoop_profile >> 4 & 0x1;
        itmh.base.jr2_common.snoop_profile_lo = xd->itmh_snoop_profile & 0xf;
        itmh.base.jr2_common.traffic_class = xd->itmh_traffic_class;
        itmh.base.jr2_common.drop_precedence = xd->itmh_dp;
        itmh.base.jr2_common.itmh_base_ext_present = xd->itmh_base_ext_present;

        if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_FEC_POINTER)
        {
            /*
             * 0(1) + FEC-PTR(20)
             */
            if ((xd->itmh_fec_ptr & 0xc0000) == 0xc0000)
            {
                /*
                 * 0(1) + 11(2)+ FEC-PTR -> 0(1) + 10(2)+ FEC-PTR
                 */
                xd->itmh_fec_ptr &= 0xfffbffff;
            }
            itmh.base.jr2_dest_info.destination_hi = (xd->itmh_fec_ptr >> 15) & 0x1f;
            itmh.base.jr2_dest_info.destination_mi = (xd->itmh_fec_ptr >> 7) & 0xff;
            itmh.base.jr2_dest_info.destination_lo = xd->itmh_fec_ptr & 0x7f;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_ACTION_PROFILE_IDX)
        {
            /*
             * 01111(5) + SNS(3) + FWS(4) + Action-Profile-Idx(9)
             */
            itmh.base.jr2_dest_info.destination_hi = 0x1e | ((xd->itmh_action_profile_idx >> 15) & 0x1);
            itmh.base.jr2_dest_info.destination_mi = xd->itmh_action_profile_idx >> 7 & 0xff;
            itmh.base.jr2_dest_info.destination_lo = xd->itmh_action_profile_idx & 0x7f;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_DIRECT)
        {
            /*
             * 01100(5) + System_Port_Agg(16)
             */
            itmh.base.jr2_dest_info.destination_hi = 0x18 | ((xd->itmh_dest_sys_port >> 15) & 0x1);
            itmh.base.jr2_dest_info.destination_mi = xd->itmh_dest_sys_port >> 7 & 0xff;
            itmh.base.jr2_dest_info.destination_lo = xd->itmh_dest_sys_port & 0x7f;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_UNICAST_FLOW)
        {
            /*
             * 10(2) + TM-Flow(19)
             */
            itmh.base.jr2_dest_info.destination_hi = 0x20 | ((xd->itmh_flow_id >> 15) & 0xf);
            itmh.base.jr2_dest_info.destination_mi = xd->itmh_flow_id >> 7 & 0xff;
            itmh.base.jr2_dest_info.destination_lo = xd->itmh_flow_id & 0x7f;
        }
        else if (xd->itmh_fwd_type == SH_DNX_TX_ITMH_FWD_TYPE_SYSTEM_MULTICAST)
        {
            /*
             * 11(2) + MC-ID(19)
             */
            itmh.base.jr2_dest_info.destination_hi = 0x30 | ((xd->itmh_multicast_id >> 15) & 0xf);
            itmh.base.jr2_dest_info.destination_mi = xd->itmh_multicast_id >> 7 & 0xff;
            itmh.base.jr2_dest_info.destination_lo = xd->itmh_multicast_id & 0x7f;
        }

        sal_memcpy(itmh_header, (uint8 *) itmh.base.raw.bytes, SH_DNX_TX_ITMH_BASE_LEN);
        if (xd->itmh_base_ext_present)
        {
            itmh.base_ext.base_ext.reserved = 0;
            itmh.base_ext.base_ext.type = xd->itmh_base_ext_type;
            itmh.base_ext.base_ext.value1_hi = (xd->itmh_base_ext_value >> 16) & 0x3f;
            itmh.base_ext.base_ext.value1_mi = (xd->itmh_base_ext_value >> 8) & 0xff;
            itmh.base_ext.base_ext.value1_lo = xd->itmh_base_ext_value & 0xff;
            sal_memcpy(itmh_header + SH_DNX_TX_ITMH_BASE_LEN, (uint8 *) itmh.base_ext.raw.bytes,
                       SH_DNX_TX_ITMH_BASE_EXT_LEN);
        }
    }

    return;
}

/**
 * \brief
 *   Calculate size of system headers to append
 * \param [in] xd - Transmit description structure
 * \param [in] header_type_incoming - incoming tm port header type, use JR1 format if INJECTED_2_JR1_MODE, INJECTED_2_PP_JR1_MODE
 * \return
 *   System header size
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
sh_dnx_tx_calc_header_size(
    xd_t * xd,
    int header_type_incoming)
{
    int header_size = 0;

    header_size += DNX_MODULE_HEADER_SIZE;
    header_size += SH_DNX_TX_PTCH_TYPE2_LEN;
    if (BCM_PBMP_NOT_NULL(xd->tx_pbmp) || xd->xd_itmh_present)
    {
        if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)
            || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
        {
            if (xd->itmh_base_ext_present)
            {
                header_size += (SH_DNX_TX_ITMH_BASE_LEN_JR1 + SH_DNX_TX_ITMH_BASE_EXT_LEN_JR1);
            }
            else
            {
                header_size += SH_DNX_TX_ITMH_BASE_LEN_JR1;
            }
        }
        else
        {
            if (xd->itmh_base_ext_present)
            {
                header_size += (SH_DNX_TX_ITMH_BASE_LEN + SH_DNX_TX_ITMH_BASE_EXT_LEN);
            }
            else
            {
                header_size += SH_DNX_TX_ITMH_BASE_LEN;
            }
        }
    }

    return header_size;
}

/**
 * \brief
 *   Overrite ITMH Destination of SPA type
 * \param [in] unit - Unit #
 * \param [in] itmh_header - Buffer pointer to ITMH header
 * \param [in] dest_port - SPA
 * \param [in] header_type_incoming - incoming tm port header type, use JR1 format if INJECTED_2_JR1_MODE, INJECTED_2_PP_JR1_MODE
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
sh_dnx_tx_itmh_dest_port_set(
    int unit,
    uint8 *itmh_header,
    uint32 dest_port,
    int header_type_incoming)
{
    if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
    {
        sh_dnx_tx_itmh_jr1_t *itmh_jr1 = (sh_dnx_tx_itmh_jr1_t *) itmh_header;

        /*
         * 001(3) + System_Port_Agg(16)
         */
        itmh_jr1->base.jer_dest_info.dest_info_hi = 0x1;
        itmh_jr1->base.jer_dest_info.dest_info_mi = (dest_port >> 8) & 0xff;
        itmh_jr1->base.jer_dest_info.dest_info_lo = dest_port & 0xff;
    }
    else
    {
        sh_dnx_tx_itmh_t *itmh_jr2 = (sh_dnx_tx_itmh_t *) itmh_header;

        /*
         * 01100(5) + System_Port_Agg(16)
         */
        itmh_jr2->base.jr2_dest_info.destination_hi = 0x18 | ((dest_port >> 15) & 0x1);
        itmh_jr2->base.jr2_dest_info.destination_mi = (dest_port >> 7) & 0xff;
        itmh_jr2->base.jr2_dest_info.destination_lo = dest_port & 0x7f;
    }

    return;
}

/**
 * \brief
 *   Strip VLAN tag from packet marked by parameter "data"
 * \param [in] unit - Unit #
 * \param [out] data - Raw packet data
 * \param [out] pkt_info - Packet information structure
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
sh_dnx_tx_tag_strip(
    int unit,
    uint8 *data,
    bcm_pkt_t * pkt_info)
{
    xd_t *xd = xd_units[unit];
    enet_hdr_t *ep;

    ep = (enet_hdr_t *) data;

    if (!ENET_TAGGED(ep))
    {
        return;
    }
    sal_memcpy(data + 2 * ENET_MAC_SIZE, data + 2 * ENET_MAC_SIZE + ENET_TAG_SIZE, xd->xd_pkt_len - 16);
    xd->xd_pkt_len -= ENET_TAG_SIZE;
    pkt_info->pkt_data[0].len -= ENET_TAG_SIZE;

    return;
}

/**
 * \brief
 *   Recover VLAN tag from packet marked by parameter "data"
 * \param [in] unit - Unit #
 * \param [out] data - Raw packet data
 * \param [out] pkt_info - Packet information structure
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
sh_dnx_tx_tag_recover(
    int unit,
    uint8 *data,
    bcm_pkt_t * pkt_info)
{
    xd_t *xd = xd_units[unit];
    enet_hdr_t *ep;

    ep = (enet_hdr_t *) data;
    if (!xd->xd_vlan)
    {
        return;
    }
    sal_memcpy(data + 2 * ENET_MAC_SIZE + ENET_TAG_SIZE, data + 2 * ENET_MAC_SIZE, xd->xd_pkt_len - 2 * ENET_MAC_SIZE);
    ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(xd->xd_prio, 0, xd->xd_vlan));
    ep->en_tag_tpid = bcm_htons(ENET_DEFAULT_TPID);
    xd->xd_pkt_len += ENET_TAG_SIZE;
    pkt_info->pkt_data[0].len += ENET_TAG_SIZE;

    return;
}

/**
 * \brief
 *   Transmit packet from BCM shell using bcm_tx API
 * \param [in] xd - transmit description from arguments
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_do_tx(
    xd_t * xd)
{
    uint8 *pkt_data = NULL;
    uint8 *pkt_data_with_header = NULL;
    uint8 *payload = NULL;
    int payload_len;
    enet_hdr_t *ep = NULL;
    bcm_pkt_t *pkt_info = NULL;
    uint32 port_ndx;
    bcm_gport_t sysport_gport;
    int header_size = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t ptch_ssp;
    bcm_port_t ptch_ssp_tm;
    bcm_core_t ptch_ssp_core;
    bcm_port_t in_tm_port;
    bcm_port_t in_tm_port_pp;
    bcm_core_t in_tm_port_core;
    bcm_port_t in_logical_port;
    uint8 is_ptch_ssp_cpu = FALSE;
    uint8 has_cpu_port = FALSE;
    int header_type_incoming;
    int cpu_channel;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_gport_t gport;
    bcm_gport_t trunk_gport;
    bcm_trunk_t tid_find;
    bcm_trunk_pp_port_allocation_info_t allocation_info;

    SHR_FUNC_INIT_VARS(xd->xd_tx_unit);

    if (!ISEMPTY(xd->xd_file))
    {
        /*
         * Reading from file
         */
#ifdef NO_FILEIO
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "No filesystem\n");
#else
        if ((pkt_data = sal_alloc(TX_LOAD_MAX, "tx_packet")) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for packet data\n");
        }
        sal_memset(pkt_data, 0, TX_LOAD_MAX);
        SHR_CLI_EXIT_IF_ERR(sh_dnx_tx_load_packet(xd->xd_tx_unit, xd->xd_file, &xd->xd_pkt_len, pkt_data),
                            "Fail to load packet from file %s\n", xd->xd_file);
#endif
    }
    else if (!ISEMPTY(xd->xd_data))
    {
        /*
         * Read from hex string
         */
        if ((pkt_data = sal_alloc(TX_LOAD_MAX, "tx_packet")) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for packet data\n");
        }
        sal_memset(pkt_data, 0, TX_LOAD_MAX);
        SHR_CLI_EXIT_IF_ERR(sh_dnx_parse_user_packet_payload(xd->xd_tx_unit, xd->xd_data, &xd->xd_pkt_len, pkt_data),
                            "Fail to load packet from hex string\n");
    }
    else
    {
        if ((pkt_data = sal_alloc(xd->xd_pkt_len, "packet data")) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "No memory for packet data\n");
        }
    }

    sh_dnx_check_pkt_fields(xd, pkt_data);

    /*
     * Get in PP Port and corresponding core
     */
    SHR_IF_ERR_EXIT(bcm_port_get(xd->xd_tx_unit, xd->xd_ptch_src_port, &flags, &interface_info, &mapping_info));
    ptch_ssp = mapping_info.pp_port;
    ptch_ssp_tm = mapping_info.tm_port;
    ptch_ssp_core = mapping_info.core;
    if (interface_info.interface == _SHR_PORT_IF_CPU)
    {
        is_ptch_ssp_cpu = TRUE;
    }

    /*
     * Get in TM Port and corresponding core
     */
    SHR_IF_ERR_EXIT(bcm_port_get(xd->xd_tx_unit, xd->xd_in_tm_port, &flags, &interface_info, &mapping_info));
    in_tm_port_pp = mapping_info.pp_port;
    in_tm_port_core = mapping_info.core;
    in_tm_port = mapping_info.tm_port;
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(xd->xd_tx_unit, in_tm_port_core, in_tm_port, &in_logical_port));
    if (interface_info.interface != _SHR_PORT_IF_CPU)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "TX failed because source TM port(%d,%d) is not CPU port\n",
                     xd->xd_in_tm_port, in_tm_port_core);
    }

    if (xd->xd_in_tm_port)
    {
        if (xd->xd_ptch_src_port)
        {
            if (ptch_ssp_core != in_tm_port_core)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "TX failed because source TM port(%d,%d) and SSP(%d,%d) in PTCH are not in the same core\n",
                             xd->xd_in_tm_port, in_tm_port_core, xd->xd_ptch_src_port, ptch_ssp_core);
            }
        }
        else
        {
            /*
             * Get in pp port from PORT when PtchSRCport is not specified,
             */
            xd->xd_ptch_src_port = xd->xd_in_tm_port;
            ptch_ssp = in_tm_port_pp;
            ptch_ssp_tm = in_tm_port;
            ptch_ssp_core = in_tm_port_core;
        }
    }
    else
    {
        if (xd->xd_ptch_src_port)
        {
            /*
             * Get in tm port from PtchSRCport when PORT is not specified,
             */
            if (is_ptch_ssp_cpu)
            {
                xd->xd_in_tm_port = xd->xd_ptch_src_port;
                in_tm_port = ptch_ssp_tm;
                in_tm_port_pp = ptch_ssp;
                in_tm_port_core = ptch_ssp_core;
                has_cpu_port = TRUE;
            }
            else
            {
                if (ptch_ssp_core != in_tm_port_core)
                {
                    bcm_port_config_t config;

                    SHR_IF_ERR_EXIT(bcm_port_config_get(xd->xd_tx_unit, &config));
                    BCM_PBMP_ITER(config.cpu, port_ndx)
                    {
                        SHR_IF_ERR_EXIT(bcm_port_get(xd->xd_tx_unit, port_ndx, &flags, &interface_info, &mapping_info));
                        if (mapping_info.core == ptch_ssp_core)
                        {
                            /*
                             * Set source tm port as first CPU port which is on the same core with SSP
                             */
                            xd->xd_in_tm_port = port_ndx;
                            in_tm_port = mapping_info.tm_port;
                            in_tm_port_pp = mapping_info.pp_port;
                            in_tm_port_core = mapping_info.core;
                            has_cpu_port = TRUE;
                            break;
                        }
                    }
                    if (!has_cpu_port)
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "No cpu port\n");
                    }
                }
            }
        }
    }

    /*
     * Check if psrc is trunk
     */
    if (ptch_ssp == 0xFFFFFFFF)
    {
        /*
         * Switch local to sysport
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (xd->xd_tx_unit, xd->xd_ptch_src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                         &gport_info));
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, gport_info.sys_port);

        /*
         * Get trunk id from sysport Gport
         */
        SHR_IF_ERR_EXIT(bcm_trunk_find(xd->xd_tx_unit, -1, gport, &tid_find));

        /*
         * Get the pp port allocation info of a given trunk
         */
        SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(xd->xd_tx_unit, tid_find, 0, &allocation_info));

        /*
         * Check that trunk has a core on this member port
         */
        if ((SAL_BIT(ptch_ssp_core) & allocation_info.core_bitmap) == 0)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "PtchSRCport %d is not a valid member of Trunk %d\n", xd->xd_ptch_src_port,
                         tid_find);
        }
        else
        {
            ptch_ssp = allocation_info.pp_port_per_core_array[ptch_ssp_core];
        }
    }
    /*
     * Check if in tm port member of trunk
     */
    if (in_tm_port_pp == 0xFFFFFFFF)
    {
        /*
         * Switch local to sysport
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (xd->xd_tx_unit, xd->xd_in_tm_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                         &gport_info));
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, gport_info.sys_port);

        /*
         * Get trunk id from sysport Gport
         */
        SHR_IF_ERR_EXIT(bcm_trunk_find(xd->xd_tx_unit, -1, gport, &tid_find));
        BCM_GPORT_TRUNK_SET(trunk_gport, tid_find);
        key.index = 1;
        key.type = bcmSwitchPortHeaderType;
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(xd->xd_tx_unit, trunk_gport, key, &value));
        header_type_incoming = value.value;
    }
    else
    {
        key.index = 1;
        key.type = bcmSwitchPortHeaderType;
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(xd->xd_tx_unit, in_tm_port_core, in_tm_port, &in_logical_port));
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(xd->xd_tx_unit, in_logical_port, key, &value));
        header_type_incoming = value.value;
    }

    if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE)
        || (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE))
    {
        header_size = sh_dnx_tx_calc_header_size(xd, header_type_incoming);
        pkt_data_with_header = (uint8 *) sal_alloc(xd->xd_pkt_len + header_size, "TX packet with ptch");
        if (pkt_data_with_header == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Fail to allocate memory for packet data with header\n");
        }
        sal_memset(pkt_data_with_header, 0, DNX_MODULE_HEADER_SIZE);
        sh_dnx_tx_setup_ptch(xd, pkt_data_with_header + DNX_MODULE_HEADER_SIZE, ptch_ssp, header_type_incoming);
        if (BCM_PBMP_NOT_NULL(xd->tx_pbmp) || xd->xd_itmh_present)
        {
            sh_dnx_tx_setup_itmh(xd, pkt_data_with_header + DNX_MODULE_HEADER_SIZE + SH_DNX_TX_PTCH_TYPE2_LEN,
                                 header_type_incoming);
        }

        /*
         * Copy packet payload to new packet and delete old packet
         */
        sal_memcpy(&pkt_data_with_header[header_size], pkt_data, xd->xd_pkt_len);
        sal_free(pkt_data);
        pkt_data = NULL;

        SHR_IF_ERR_EXIT(bcm_pkt_alloc(xd->xd_tx_unit, header_size + xd->xd_pkt_len, 0, &pkt_info));
        pkt_info->flags &= ~BCM_TX_CRC_FLD;
        pkt_info->flags |=
            (xd->xd_crc == CRC_RECOMPUTE ? BCM_TX_CRC_REGEN : 0) | (xd->xd_crc == CRC_APPEND ? BCM_TX_CRC_APPEND : 0);
        pkt_info->flags &= ~BCM_TX_NO_PAD;
        if (xd->xd_pkt_len < (xd->xd_vlan != 0 ? TAGGED_PACKET_LENGTH : UNTAGGED_PACKET_LENGTH))
        {
            pkt_info->flags |= BCM_TX_NO_PAD;
        }
        if (!xd->xd_visibility)
        {
            pkt_info->flags |= BCM_TX_NO_VISIBILITY_RESUME;
        }

        SHR_IF_ERR_EXIT(bcm_pkt_memcpy(pkt_info, 0, pkt_data_with_header, header_size + xd->xd_pkt_len));
        pkt_info->pkt_data[0].len = header_size + xd->xd_pkt_len;

        /*
         * Set higig flag to indicate Module Header is present
         * Set the incoming CPU channel into dpp_header
         */
        pkt_info->flags |= BCM_PKT_F_HGHDR;
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(xd->xd_tx_unit, in_tm_port_core, in_tm_port, &in_logical_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(xd->xd_tx_unit, in_logical_port, &cpu_channel));
        pkt_info->_dpp_hdr[0] = cpu_channel;
#ifdef ADAPTER_SERVER_MODE
        /*
         * PTC is got from pkt->src_gport on adapter 
         * Given PTC value is equal to value of source TM port
         * If source TM port is not specified, it will be set as PtchSRCport
         */
        pkt_info->src_gport = in_tm_port;
#endif
        sal_free(pkt_data_with_header);
        pkt_data_with_header = NULL;
        ep = (enet_hdr_t *) (&pkt_info->pkt_data[0].data[header_size]);

        payload = &pkt_info->pkt_data[0].data[header_size + sizeof(enet_hdr_t)];
        payload_len = pkt_info->pkt_data[0].len - header_size - sizeof(enet_hdr_t);

        /*
         * XMIT all the required packets
         */
        for (xd->xd_cur_cnt = 0; xd->xd_cur_cnt < xd->xd_tot_cnt; xd->xd_cur_cnt++)
        {
            /*
             * Generate pattern on first time through, or every time if
             * pattern is incrementing.
             */

            if (xd->xd_mac_dst_inc != 0)
            {
                ENET_SET_MACADDR(ep->en_dhost, xd->xd_mac_dst);
            }
            if (xd->xd_mac_src_inc != 0)
            {
                ENET_SET_MACADDR(ep->en_shost, xd->xd_mac_src);
            }

            /*
             * Store pattern
             */
            if (ISEMPTY(xd->xd_file) && ISEMPTY(xd->xd_data) && xd->xd_pat_random)
            {
                packet_random_store(payload, payload_len);
            }
            else if (ISEMPTY(xd->xd_file) && ISEMPTY(xd->xd_data) && (xd->xd_cur_cnt == 0 || xd->xd_pat_inc != 0))
            {
                xd->xd_pat = packet_store(payload, payload_len, xd->xd_pat, xd->xd_pat_inc);
            }

            if (BCM_PBMP_NOT_NULL(xd->tx_pbmp))
            {
                /*
                 * Put packet to member port of tx_pbmp one by one via ITMH header
                 */
                BCM_PBMP_ITER(xd->tx_pbmp, port_ndx)
                {
                    /*
                     * Bypass ILKN ports, SFI ports
                     */
                    SHR_IF_ERR_EXIT(bcm_port_get(xd->xd_tx_unit, port_ndx, &flags, &interface_info, &mapping_info));
                    if ((interface_info.interface == _SHR_PORT_IF_ILKN)
                        || (interface_info.interface == _SHR_PORT_IF_SFI))
                    {
                        continue;
                    }

                    /*
                     * Get the system port from logical port as destination port
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                    (xd->xd_tx_unit, port_ndx, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                     &gport_info));
                    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, gport_info.sys_port);

                    /*
                     * Set destination port into ITMH header
                     */
                    sh_dnx_tx_itmh_dest_port_set(xd->xd_tx_unit,
                                                 pkt_info->pkt_data[0].data + DNX_MODULE_HEADER_SIZE +
                                                 SH_DNX_TX_PTCH_TYPE2_LEN, BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport),
                                                 header_type_incoming);

                    if (BCM_PBMP_MEMBER(xd->tx_upbmp, port_ndx))
                    {
                        /*
                         * Strip VLAN tag for member of tx_upbmp
                         */
                        sh_dnx_tx_tag_strip(xd->xd_tx_unit, pkt_info->pkt_data[0].data + header_size, pkt_info);
                    }
                    SHR_IF_ERR_EXIT(bcm_tx(xd->xd_tx_unit, pkt_info, NULL));
                    if (BCM_PBMP_MEMBER(xd->tx_upbmp, port_ndx))
                    {
                        /*
                         * add VLAN tag back after packet transmisstion for member of tx_upbmp
                         */
                        sh_dnx_tx_tag_recover(xd->xd_tx_unit, pkt_info->pkt_data[0].data + header_size, pkt_info);
                    }
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_tx(xd->xd_tx_unit, pkt_info, NULL));
            }

            increment_macaddr(xd->xd_mac_dst, xd->xd_mac_dst_inc);
            increment_macaddr(xd->xd_mac_src, xd->xd_mac_src_inc);
        }
        SHR_IF_ERR_EXIT(bcm_pkt_free(xd->xd_tx_unit, pkt_info));
    }
    else
    {
        if (pkt_data)
        {
            sal_free(pkt_data);
            pkt_data = NULL;
        }
        SHR_CLI_EXIT(_SHR_E_PARAM, "Can't inject packet without PTCH\n");
    }
exit:
    if (pkt_data)
    {
        sal_free(pkt_data);
        pkt_data = NULL;
    }
    if (pkt_data_with_header)
    {
        sal_free(pkt_data_with_header);
        pkt_data_with_header = NULL;
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_tx(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    xd_t *xd;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init the xd struct
     */
    SHR_IF_ERR_EXIT(sh_dnx_xd_init(unit));
    xd = xd_units[unit];

    /*
     * Parse the argument line
     */
    SHR_IF_ERR_EXIT(sh_dnx_tx_parse(unit, args, sand_control, xd));

    /*
     * Execute tx command
     */
    sh_dnx_do_tx(xd);

exit:
    SHR_FUNC_EXIT;
}
