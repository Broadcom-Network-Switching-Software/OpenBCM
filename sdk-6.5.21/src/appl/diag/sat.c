/*
 * 
 *
 * SAT specific commands.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>
#ifdef BCM_SAT_SUPPORT
#include <sal/types.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <bcm/sat.h>
#include <bcm/error.h>


#if defined(BCM_ESW_SUPPORT) && !defined(BCM_PETRA_SUPPORT)
#define SAT_GTF_PACKET_SET_USAGE \
    "sat Gtf Packetconfig Set <gtfId> [HeaderType=<val>] [PayloadType=<val>] [PayloadPattern=<val>]\n\t"  \
    "                         [CirNumofctfs=<val>] [CiraddendTlv=true|false] [CiraddCrc=true|false]\n\t"  \
    "                         [EirNumofctfs=<val>] [EiraddendTlv=true|false] [EiraddCrc=true|false]\n\t"  \
    "                         [Offsetofseqnum=<val>]"
#endif /* BCM_ESW_SUPPORT && !BCM_PETRA_SUPPORT */
#if defined(BCM_ESW_SUPPORT)
#define SAT_GTF_OLPHEADER_USAGE \
    "sat Gtf Packetconfig OlpHeader Set <gtfId> [rawData=<data>]\n\t"  \
    "                               L2  <gtfId> [DMac=<mac>] [SMac=<mac>] [Tpid=<val>] [Vlan=<vid>]\n\t"  \
    "                                   [vlanPri=<val>] [vlanCfi=<val>] [ETHertype=<val>]\n\t"  \
    "                               Tx  <gtfId> [isDown=true|false] [destModid=<val>] [destPort=<val>]\n\t"  \
    "                                   [isAddTs=true|false] [timestampFormat=<val>] [OffsetofTs=<val>]"
#ifndef COMPILER_STRING_CONST_LIMIT
#define SAT_EP_SHOW_USAGE \
    "sat Endpoint SHOW    [<epId>]"
#define SAT_EP_DESTROY_USAGE \
    "sat Endpoint Destroy [<epId>] [isDowN=true|false]"
#define SAT_EP_CREATE_USAGE \
    "sat Endpoint Create  [<epId>] [isDowN=true|false] [MatchOuterVlan=true|false] [MatchInnerVlan=true|false]\n\t"  \
    "                     [DMac=<mac>] [SMac=<mac>] [OuterVlan=<val>] [InnerVlan=<val>]\n\t"  \
    "                     [SrcPort=<port>] [ETHertype=<val>] [Drop=true|false] [Redirect=true|false]\n\t"  \
    "                     [LLF=true|false] [CopytoCpu=true|false] [timeStamp=true|false] [pktPri=<val>]\n\t"  \
    "                     [DestPort=<port>] [timeStampFormat=<val>] [sessionId=<val>]"
#define SAT_EP_REPLACE_USAGE \
    "             Replace <epId> [isDowN=true|false] [MatchOuterVlan=true|false] [MatchInnerVlan=true|false]\n\t"  \
    "                     [DMac=<mac>] [SMac=<mac>] [OuterVlan=<val>] [InnerVlan=<val>]\n\t"  \
    "                     [SrcPort=<port>] [ETHertype=<val>] [Drop=true|false] [Redirect=true|false]\n\t"  \
    "                     [LLF=true|false] [CopytoCpu=true|false] [timeStamp=true|false] [pktPri=<val>]\n\t"  \
    "                     [DestPort=<port>] [timeStampFormat=<val>] [sessionId=<val>]"
#else
#define SAT_EP_SHOW_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#define SAT_EP_DESTROY_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#define SAT_EP_CREATE_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#define SAT_EP_REPLACE_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#endif
#define SAT_ENDPOINT_CONFIGURE_USAGE \
    SAT_EP_SHOW_USAGE "\n\t"  \
    SAT_EP_DESTROY_USAGE "\n\t"  \
    SAT_EP_CREATE_USAGE "\n\t"  \
    SAT_EP_REPLACE_USAGE "\n\t"  \
    "            - Set/display SAT endpoint configuration"
#else
#define SAT_GTF_OLPHEADER_USAGE
#define SAT_ENDPOINT_CONFIGURE_USAGE
#endif /* BCM_ESW_SUPPORT */
#if defined(BCM_PETRA_SUPPORT)
#define SAT_GTF_PACKET_SET_USAGE \
    "sat Gtf Packetconfig Set <gtfId> [HeaderType=<val>] [PayloadType=<val>] [PayloadPattern=<val>]\n\t"  \
    "                         [CirNumofctfs=<val>] [CiraddendTlv=true|false] [CiraddCrc=true|false]\n\t"  \
    "                         [EirNumofctfs=<val>] [EiraddendTlv=true|false] [EiraddCrc=true|false]\n\t"  \
    "                         [pktContext=<val>] [Offsetofseqnum=<val>]"
#define SAT_GTF_DNXHEADER_USAGE \
    "sat Gtf Packetconfig DnxHeader Set  <gtfId> [rawData=<data>]\n\t"  \
    "                               Ptch <gtfId> [srcGport=<port>] [opaqueAttr=<val>]\n\t" \
    "                                    [isNextHeaderItmh=true|false]\n\t"  \
    "                               Itmh <gtfId> [dstGport=<port>] [Prio=<val>] [Color=<val>]\n\t"  \
    "                                    [inMirrorFlag=true|false] [snoopCMD=<val>] [pphType=<val>]\n\t"  \
    "                               Otsh <gtfId> [Type=<val>] [SubType=<val>] [isDowN=true|false]\n\t"  \
    "                                    [tsData=<val>] [tsOffset=<val>]"
#define SAT_CTF_DNX_TRAP_USAGE \
    "sat Ctf Trap Add    <trapId>\n\t"  \
    "             Remove [<trapId>]"
#else
#define SAT_GTF_DNXHEADER_USAGE
#define SAT_CTF_DNX_TRAP_USAGE
#endif /* BCM_PETRA_SUPPORT */
#define SAT_GTF_SHOW_USAGE \
    "sat Gtf SHOW    [<gtfId>]"
#define SAT_GTF_CREATE_USAGE \
    "sat Gtf Create  [<gtfId>]"
#define SAT_GTF_DESTROY_USAGE \
    "sat Gtf Destroy [<gtfId>]"
#define SAT_GTF_START_USAGE \
    "sat Gtf START   <gtfId> <priority>"
#define SAT_GTF_STOP_USAGE \
    "sat Gtf STOP    <gtfId> <priority>"
#define SAT_GTF_STAT_USAGE \
    "sat Gtf STat    <gtfId> <priority>"
#define SAT_GTF_BANDWIDTH_USAGE \
    "sat Gtf Bandwidth Set <gtfId> <priority> [Rate=<val>] [maxBurst=<val>]\n\t"  \
    "                  Get <gtfId> <priority>"
#define SAT_GTF_RATEPATTERN_USAGE \
    "sat Gtf Ratepattern Set <gtfId> <priority> [Mode=<val>] [HighThresh=<val>]\n\t"  \
    "                        [LowThresh=<val>] [stopITerCnt=<val>] [stopBurstCnt=<val>]\n\t"  \
    "                        [stopIntervalCnt=<val>] [BurstpktWeight=<val>]\n\t"  \
    "                        [IntervalpktWeight=<val>] [StopintervalEqburst=true|false]\n\t"  \
    "                    Get <gtfId> <priority>"
#ifndef COMPILER_STRING_CONST_LIMIT
#define SAT_GTF_PACKETCONFIG_USAGE \
    "sat Gtf Packetconfig Format <gtfId>\n\t"  \
    SAT_GTF_PACKET_SET_USAGE "\n\t" \
    SAT_GTF_OLPHEADER_USAGE "\n\t" \
    SAT_GTF_DNXHEADER_USAGE "\n\t" \
    "sat Gtf Packetconfig Packetdata <gtfId> [DMac=<mac>] [SMac=<mac>] [Tpid=<val>]\n\t"  \
    "                                [Vlan=<val>] [vlanPri=<val>] [extData=<data>]\n\t"  \
    "sat Gtf Packetconfig Length <gtfId> <priority> [pktLens=<val>]\n\t"  \
    "                            [pktlenPatterns=<val>] [PatternLens=<val>]\n\t"  \
    "sat Gtf Packetconfig STamp  <gtfId> <priority> [StampTypes=<val>] [stampFieldTypes=<val>]\n\t"  \
    "                            [stampIncSteps=<val>] [stampIncPeriods=<val>] [stampValues=<val>]\n\t"  \
    "                            [stampOffsets=<val>] [Numofstamps=<val>]"
#else
#define SAT_GTF_PACKETCONFIG_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#endif
#define SAT_CTF_CREATE_USAGE \
    "sat Ctf Create  [<ctfId>]"
#define SAT_CTF_DESTROY_USAGE \
    "sat Ctf Destroy [<ctfId>]"
#define SAT_CTF_PACKETCONFIG_USAGE \
    "sat Ctf Packetconfig <ctfId> [HeaderType=<val>] [PayloadType=<val>] [PayloadPattern=<val>]\n\t"  \
    "                     [PayloadOffset=<val>] [SeqnumberOffset=<val>] [TimestampOffset=<val>]\n\t"  \
    "                     [addendTlv=true|false] [addCrc=true|false]"
#define SAT_CTF_IDENTIFER_USAGE \
    "sat Ctf Identifier Add    <ctfId> [TC=<val>] [Color=<val>] [TrapId=<val>] [SessionId=<val>]\n\t"  \
    "                   Remove [TC=<val>] [Color=<val>] [TrapId=<val>] [SessionId=<val>]"
#define SAT_CTF_TRAP_USAGE \
    SAT_CTF_DNX_TRAP_USAGE "\n\t" \
    "sat Ctf Trap Data2Session Add    [trapData=<val>] [trapdataMask=<val>] [sessionId=<val>]\n\t"  \
    "                          Remove [trapData=<val>] [trapdataMask=<val>]"
#define SAT_CTF_BINLIMIT_USAGE \
    "sat Ctf Binlimit SHOW\n\t"  \
    "                 Set [Bins=<val>]"
#define SAT_CTF_AVAILABILITY_USAGE \
    "sat Ctf Availability <ctfId> [Noofslot=<val>] [Thresholdperslot=<val>]"
#define SAT_CTF_REPORT_USAGE \
    "sat Ctf Report <ctfId> [samplePercent=<val>] [addSeqnum=true|false] [addDelay=true|false]"
#define SAT_CTF_STAT_USAGE \
    "sat Ctf Stat Set  <ctfId> [useGlobalbin=true|false] [binMinDelay=<val>]\n\t"  \
    "                  [binStep=<val>] [updateCntInUnavail=true|false]\n\t"  \
    "             SHOW [<ctfId>] [Clronread=true|false]"

#define SAT_GLOBAL_CONFIGURE_USAGE \
    "sat CONFig Set [timestampFormat=<type>] [crcReversedData=true|false] [crcInvert=true|false]\n\t"  \
    "               [crcHighReset=true|false] [prbsUseNxor=true|false]\n\t"  \
    "sat CONFig SHOW\n\t"  \
    "          - Set/display global SAT configuration"

#ifndef COMPILER_STRING_CONST_LIMIT
#define SAT_GTF_CONFIGURE_USAGE \
    SAT_GTF_SHOW_USAGE "\n\t"  \
    SAT_GTF_CREATE_USAGE "\n\t"  \
    SAT_GTF_DESTROY_USAGE "\n\t"  \
    SAT_GTF_START_USAGE "\n\t"  \
    SAT_GTF_STOP_USAGE "\n\t"  \
    SAT_GTF_STAT_USAGE "\n\t"  \
    SAT_GTF_BANDWIDTH_USAGE "\n\t"  \
    SAT_GTF_RATEPATTERN_USAGE "\n\t"  \
    SAT_GTF_PACKETCONFIG_USAGE "\n\t"  \
    "       - Set/display SAT GTF configuration"

#define SAT_CTF_CONFIGURE_USAGE \
    SAT_CTF_CREATE_USAGE "\n\t"  \
    SAT_CTF_DESTROY_USAGE "\n\t"  \
    SAT_CTF_PACKETCONFIG_USAGE "\n\t"  \
    SAT_CTF_IDENTIFER_USAGE "\n\t"  \
    SAT_CTF_TRAP_USAGE "\n\t"  \
    SAT_CTF_BINLIMIT_USAGE "\n\t"  \
    SAT_CTF_AVAILABILITY_USAGE "\n\t"  \
    SAT_CTF_REPORT_USAGE "\n\t"  \
    SAT_CTF_STAT_USAGE "\n\t"  \
    "       - Set/display SAT CTF configuration"
#else
#define SAT_GTF_CONFIGURE_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define SAT_CTF_CONFIGURE_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"
#endif
char cmd_sat_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "sat sub_command <option> [args...]\n"
#else
    SAT_GLOBAL_CONFIGURE_USAGE "\n\n\t"
    SAT_GTF_CONFIGURE_USAGE "\n\n\t"
    SAT_CTF_CONFIGURE_USAGE "\n\n\t"
    SAT_ENDPOINT_CONFIGURE_USAGE "\n"
#endif
    ;

#define _SAT_CMD_GTF_PRI_COMMON (-1)    /* Priority value which means EIR together with CIR. */
#define _SAT_CMD_GTF_CNT_MAX    8
#define _SAT_CMD_CTF_CNT_MAX    32

typedef struct sat_subcommand_s {
    char *str;
    cmd_result_t (*func)(int unit, args_t *args);
    const char* help;
    const char* help_ext;  /* To prevent strings > 255 chars */
} sat_subcommand_t;

typedef struct _sat_gtf_pkt_info_s {
    bcm_sat_gtf_packet_config_t cfg;
    bcm_pkt_t *sat_header; /* release resource properly */
    int32 packet_length[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS];
    int32 dev_header_len; /* Internal header length. OLP or DNX */
    int32 ctrl_info_len;   /* From Internal to PDU TLV header */
    int32 dev_header_1_offset;    /* Internal headers' position */
    int32 dev_header_2_offset;
    int32 dev_header_3_offset;
    int32 eth_header_offset;
    int32 pdu_offset;
} _sat_gtf_pkt_info_t;

#define SAT_IF_ERROR_RETURN(op)                             \
    do {                                                    \
        int __rv__;                                         \
        if ((__rv__ = (op)) < 0) {                          \
            cli_out("Error: %s\n", bcm_errmsg(__rv__));     \
            return CMD_FAIL;                                \
        }                                                   \
    } while (0)

#define SAT_GTF_ID_CHECK_RETURN(id) \
    do { \
        if ((id < 0) || (id >= _SAT_CMD_GTF_CNT_MAX)) { \
            cli_out("Invalid GTF ID: %d\n", id); \
            return CMD_FAIL; \
        } \
    } while (0)

#define SAT_CTF_ID_CHECK_RETURN(id) \
    do { \
        if ((id < 0) || (id >= _SAT_CMD_CTF_CNT_MAX)) { \
            cli_out("Invalid CTF ID: %d\n", id); \
            return CMD_FAIL; \
        } \
    } while (0)

/*
 * Macro:
 *     SAT_ARG_NUMB_GET
 * Purpose:
 *     Get a nonnegative numerical value from stdin.
 */
#define SAT_ARG_NUMB_GET(args, str, numb) \
    do { \
        if (((str) = ARG_GET(args)) == NULL) { \
            return CMD_USAGE; \
        } \
        (numb) = _parse_integer(str); \
        if (numb < 0) { \
            return CMD_FAIL; \
        } \
    } while (0)

/*
 * Macro:
 *     SAT_ARG_INT_GET
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define SAT_ARG_INT_GET(args, str, numb) \
    do { \
        if (((str) = ARG_GET(args)) == NULL) { \
            return CMD_USAGE; \
        } \
        (numb) = _parse_integer(str); \
    } while (0)

STATIC uint32 _gtf_pkt_header_max_len = 128;    /* Internal + Eth + PDU */
STATIC _sat_gtf_pkt_info_t _gtf_pkt_cfg_default;
STATIC _sat_gtf_pkt_info_t _gtf_pkt_cfg[_SAT_CMD_GTF_CNT_MAX];
STATIC bcm_sat_ctf_packet_info_t _ctf_pkt_cfg[_SAT_CMD_CTF_CNT_MAX];

/*
 * Read an integer: return unsigned representation.
 * Number format explained below.
 *
 * Expects:
 * [-]0x[0-9|A-F|a-f]+   -hexadecimal if the string begins with "0x"
 * [-][0-9]+             -decimal integer
 * [-]0[0-7]+            -octal integer
 * [-]0b[0-1]+           -binary if the string begins with "0b"
 */
STATIC int
_parse_integer(char *str)
{
    if (!isint(str)) {
        cli_out("ERROR: malformed integer \"%s\"\n", str);
        return CMD_FAIL;
    }

    return _shr_ctoi(str);
}

/*
 * Process a comma seperated list and store elements in a data array.
 * Returns element count.
 */
STATIC int
_list_to_data_array(char *str, uint32 max_array_len, int32 *array)
{
    int index = 0;
    int num, tmp_size = 16;
    char *head = str, *rear = str;
    char tmp[16];

    if ((array == NULL) || (str == NULL)) {
        return -1;
    }
    while ((index < max_array_len) && (*head != '\0')) {
        rear = sal_strchr(head, ',');
        if (!rear) {
            rear = str + sal_strlen(str);
        }
        sal_memset(tmp, 0, tmp_size);
        sal_memcpy(tmp, head, rear - head);
        num = parse_integer(tmp);
        array[index] = num;
        index++;

        if (*rear == '\0') {
            break;
        }
        head = rear + 1;
    }

    return index;
}

/*
 * Parsing string given by user as packet data
 * NOTE: Must be used pair with "_parse_packet_data_end".
 */
STATIC int
_parse_packet_data(int unit, char *packet, int *length, uint8 **data)
{
    uint8 *p;
    char   tmp, data_iter;
    int    data_len, i, j, pkt_len, data_base;

    /* If string data starts with 0x or 0X, skip it */
    if ((packet[0] == '0')
        && (packet[1] == 'x' || packet[1] == 'X')) {
        data_base = 2;
    } else {
        data_base = 0;
    }

    data_len = strlen(packet) - data_base;
    pkt_len = (data_len + 1) / 2;

    p = sal_alloc(pkt_len, "sat_packet");
    if (p == NULL) {
        data = NULL;
        return -1;
    }
    sal_memset(p, 0, pkt_len);
    /* Convert char to value */
    i = j = 0;
    while (j < data_len) {
        data_iter = packet[data_base + j];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else if (data_iter == ' ') {
            ++j;
            continue;
        } else {
            sal_free(p);
            data = NULL;
            return -1;
        }

        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
        ++j;
    }

    *length = i / 2;
    *data = p;

    return 0;
}

/*
 * Release allocated resources in previous _parse_packet_data
 */
STATIC int
_parse_packet_data_end(int unit, char *packet, int *length, uint8 *data)
{
    if (data == NULL) {
        return -1;
    }
    sal_free(data);

    return 0;
}

STATIC void
_sat_gtf_pkt_info_t_init(_sat_gtf_pkt_info_t *info)
{
    if (info == NULL) {
        return;
    }
    sal_memset(info, 0, sizeof(_sat_gtf_pkt_info_t));
    info->packet_length[bcmSatGtfPriCir][0] = 64;
    info->packet_length[bcmSatGtfPriEir][0] = 64;
    info->dev_header_len = -1;
    info->ctrl_info_len = -1;
    info->dev_header_1_offset = -1;
    info->dev_header_2_offset = -1;
    info->dev_header_3_offset = -1;
    info->eth_header_offset = -1;
    info->pdu_offset = -1;
}

#if defined(BCM_PETRA_SUPPORT)
/* 
 *  PTCH-2 Header format:
 *    Field                      Size            Bit/s
 *    Parser-Program-Control     1               15
 *    Opaque-PT-Attributes       3               14:12
 *    Reserved                   4               11:8
 *    PP-SSP                     8               7:0
 */
STATIC int
_insert_dnx_ptch2(uint8* buffer, int byte_start_offset, 
    uint8 next_header_is_itmh, 
    uint8 opaque_value, 
    uint8 pp_ssp) 
{
    int offset = byte_start_offset;
    int parser_program_ctl = 0;

    parser_program_ctl = next_header_is_itmh ? 0 : 1;
    buffer[offset++] = (parser_program_ctl<<7) | (opaque_value<<4);
    buffer[offset++] = pp_ssp;
    
    return offset;
}

/* 
 *  Arad-style ITMH Header format:
 *    Field                      Size            Bit/s
 *    PPH_TYPE                   2               31:30
 *    IN_MIRR_DISABLE            1               29
 *    SNOOP_CMD                  4               28:25
 *    FWD_TRAFFIC_CLASS          3               24:22
 *    FWD_DP                     2               21:20
 *    FWD_DEST_INFO              20              19:0
 *
 *    Arad-style FWD_DEST_INFO(20bits)

 *    0         Multicast-ID(19)
 *    1 0 0     ISQ-Flow-ID(17)
 *    1 1 1        OUTLIT(17)
 *    1 1 0 0   Dest-System-Port-Agr(16)
 *    1 1 0 1    Multicast-Flow-ID(17)
 *    1 1 1        Flow-ID(17)
 */
STATIC int
_insert_dnx_itmh_arad(uint8* buffer, int byte_start_offset, 
    int dst_sys_port, 
    uint8 fwd_tc, 
    uint8 fwd_dp, 
    uint8 snoop_cmd, 
    uint8 in_mirr_flag, 
    uint8 pph_type)
{
    int offset = byte_start_offset;
    int fwd_dest_info = 0xc0000 | dst_sys_port;

    buffer[offset++] = ((pph_type&0x3)<<6)|((in_mirr_flag&0x1)<<5)|((snoop_cmd&0xf)<<1)|((fwd_tc&0x4)>>2);
    buffer[offset++] = ((fwd_tc&0x3)<<6) | ((fwd_dp&0x3)<<4) | ((fwd_dest_info&0xf0000)>>16);
    buffer[offset++] = (fwd_dest_info&0xff00)>>8;
    buffer[offset++] = (fwd_dest_info&0xff);

    return offset;
}

/* 
 *  OAM-TS Header format:
 *    Field                      Size            Bit/s
 *    Type                       2               47:46
 *    OAM-Sub-Type               3               45:43
 *    MEP-Type                   1               42
 *    OAM-TS-Data                34              41:8
 *    Offset                     8               7:0
 */
STATIC int
_insert_dnx_oam_ts(uint8* buffer, int byte_start_offset, 
    uint8 type, 
    uint8 sub_type,
    uint8 is_up_mep,
    uint64 ts_data,
    uint8 ts_offset) 
{
    int offset = byte_start_offset;
    int mep_type;
    int ts_data_hi = 0, ts_data_lo = 0;    

    mep_type = is_up_mep?1:0;
    ts_data_hi = COMPILER_64_HI(ts_data);
    ts_data_lo = COMPILER_64_LO(ts_data);
    buffer[offset++] = ((type&0x3)<<6)|((sub_type&0x7)<<3)|((mep_type&0x1)<<2)|(ts_data_hi&0x3);    
    buffer[offset++] = (ts_data_lo&0xff000000)>>24;
    buffer[offset++] = (ts_data_lo&0xff0000)>>16;
    buffer[offset++] = (ts_data_lo&0xff00)>>8;
    buffer[offset++] = (ts_data_lo&0xff);
    buffer[offset++] = ts_offset&0xff;

    return offset;
}
#endif
#if defined(BCM_ESW_SUPPORT)
/* 
 *  XGS OLP L2 Header
 *    Field                      Size
 *    OLP-DA                     48
 *    OLP-SA                     48
 *    OLP-VLAN                   32
 */
STATIC int
_insert_xgs_olp_l2(uint8* buffer, int byte_start_offset, 
    bcm_mac_t dest_addr, 
    bcm_mac_t src_addr, 
    uint16 tpid, 
    bcm_vlan_t vlan, 
    uint8 vlan_pri, 
    uint8 vlan_cfi, 
    uint16 ether_type) 
{
    int i = 0, offset = byte_start_offset; 
    for (i = 0; i < 6; i++) {
        buffer[offset++] = dest_addr[i];
    }
    for (i = 0; i < 6; i++) {
        buffer[offset++] = src_addr[i];
    }

    buffer[offset++] = (tpid>>8)&0xff;
    buffer[offset++] = tpid&0xff;
    buffer[offset++] = (vlan_pri<<5)|((vlan_cfi&&0x1)<<4)|((vlan>>8)&0xff);        
    buffer[offset++] = vlan&0xff;
    
    buffer[offset++] = (ether_type>>8)&0xff;
    buffer[offset++] = ether_type&0xff;

    return offset;
}

/* 
 *  XGS OLP TX Header
 *  From SB2_OLP_Architecture_Spec_0.94_Clean.docx
 *    Field                                     Size     Bit/s    Bytes    Description
 *    Version                                      4     127:124  15       Version =0 for Saber 2 & Katana 2
 *    HEADER_TYPE                                  8     123:116  15-14    00=OAM_TX_HEADER
 *    HEADER_SUBTYPE                               8     115:108  14-13    03 = SAT_DOWN_MEP_TX
 *                                                                         04 = SAT_UPMEP_TX
 *    INT_PRI                                      4     107:104  13       Internal Priority used for UP-MEP Tx. DP is assumed to be green.
 *    PORT_ID                                      16    103:88   12-11    [15-8] : MODID
 *                                                                         [7-0]  : PORT_NUM 
 *    COUNTER_1_LOCATION                           1     87       10       0 = in IP 
 *                                                                         1 = in EP
 *    COUNTER_1_ID                                 15    86:72    10-9     Counter ID to be incremented or sampled by this packet
 *    COUNTER_1_ACTION                             2     71:70    8        00: No-op 
 *                                                                         01: Increment 
 *                                                                         10: Sample
 *                                                                         11: reserved
 *    COUNTER_2_LOCATION                           1     69       8
 *    COUNTER_2_ID                                 15    68:54    8-6
 *    COUNTER_2_ACTION                             2     53:52    6
 *    TIMESTAMP_ACTION                             2     51:50    6        00: No-op 
 *                                                                         01: Sample 1588 Timestamp 
 *                                                                         10: Sample NTP Timestamp 
 *                                                                         11: Reserved
 *    OAM_REPLACEMENT_OFFSET                       8     49:42    6-5
 *    RELATIVE_LM_REPLACEMENT_OFFSET               4     41:38    5-4
 *    RELATIVE_LM_REPLACEMENT_OFFSET_ADD_OR_SUB    1     37       4
 *    COUNTER_3_LOCATION                           1     36       4
 *    COUNTER_3_ID                                 15    35:21    4-2
 *    COUNTER_3_ACTION                             2     20:19    2
 *    COUNTER1_OFFSET_TO_LM_PAYLOAD                6     18:13    2-1
 *    COUNTER2_OFFSET_TO_LM_PAYLOAD                6     12:7     1-0
 *    COUNTER3_OFFSET_TO_LM_PAYLOAD                6     6:1      0
 *    Reserved                                     1     0        0
 *    Total                      128
 */
STATIC int
_insert_xgs_olp_tx(uint8* buffer, int byte_start_offset, 
    int is_down_mep, 
    int modid, 
    int port, 
    int timestamp_action,
    int oam_offset) 
{
    int offset = byte_start_offset;
    int version = 0;
    int header_type = 0;
    int header_subtype = (is_down_mep) ? (2) : (3);
    int int_pri = 0;
    int oam_replacement_offset = 0;
    
    oam_replacement_offset = (oam_offset - 14) / 2;

    /* byte 15: Version    [3:0]->[7:4], 
     *          header_type[7:4]->[3:0]
     */
    buffer[offset++] = ((version&0xf)<<4)|((header_type&0xf0)>>4); 
    /* byte 14: header_type   [3:0]->[7:4], 
     *          header_subtype[7:4]->[3:0]
     */
    buffer[offset++] = ((header_type&0xf)<<4)|((header_subtype&0xf0)>>4);        
    /* byte 13: header_subtype[3:0]->[7:4], 
     *          int_pri       [3:0]->[3:0]
     */
    buffer[offset++] = ((header_subtype&0xf)<<4)|(int_pri&0xf);
    /* byte 12: MODID [7:0]->[7:0] */
    buffer[offset++] = (modid&0xff);
    /* byte 11: port  [7:0]->[7:0] */
    buffer[offset++] = (port&0xff);
    /* byte 10: COUNTER_1_LOCATION [0]->[7]
     *          COUNTER_1_ID       [14:8]->[6:0]
     */
    buffer[offset++] = 0;
    /* byte  9: COUNTER_1_ID       [7:0]->[7:0] */
    buffer[offset++] = 0;
    /* byte  8: COUNTER_1_ACTION   [1:0]->[7:6]
     *          COUNTER_2_LOCATION [0]->[5]
     *          COUNTER_2_ID       [14:10]->[4:0]
     */
    buffer[offset++] = 0;
    /* byte  7: COUNTER_2_ID       [9:2]->[7:0]
     */
    buffer[offset++] = 0;
    /* byte  6: COUNTER_2_ID           [1:0]->[7:6]
     *          COUNTER_2_ACTION       [1:0]->[5:4]
     *          TIMESTAMP_ACTION       [1:0]->[3:2]
     *          OAM_REPLACEMENT_OFFSET [7:6]->[1:0]
     */
    buffer[offset++] = ((timestamp_action&0x3)<<2)|((oam_replacement_offset&0xc0)>>6);
    /* byte  5: OAM_REPLACEMENT_OFFSET         [5:0]->[7:2]
     *          RELATIVE_LM_REPLACEMENT_OFFSET [3:2]->[1:0]
 */
    buffer[offset++] = (oam_replacement_offset&0x3f)<<2;
    /* byte  4: RELATIVE_LM_REPLACEMENT_OFFSET            [1:0]->[7:6]
     *          RELATIVE_LM_REPLACEMENT_OFFSET_ADD_OR_SUB [0]->[5]
     *          COUNTER_3_LOCATION                        [0]->[4]
     *          COUNTER_3_ID                              [14:11]->[3:0]
     */
    buffer[offset++] = 0;
    /* byte  3: COUNTER_3_ID                              [10:3]->[7:0] */
    buffer[offset++] = 0;
    /* byte  2: COUNTER_3_ID                              [2:0]->[7:5]
     *          COUNTER_3_ACTION                          [1:0]->[4:3]
     *            COUNTER1_OFFSET_TO_LM_PAYLOAD             [5:3]->[2:0]
     */
    buffer[offset++] = 0;
    /* byte  1: COUNTER1_OFFSET_TO_LM_PAYLOAD             [2:0]->[7:5]
     *          COUNTER2_OFFSET_TO_LM_PAYLOAD             [5:1]->[4:0]
     */
    buffer[offset++] = 0;
    /* byte  0: COUNTER2_OFFSET_TO_LM_PAYLOAD             [0]->[7]
     *          COUNTER2_OFFSET_TO_LM_PAYLOAD             [5:0]->[6:1]
     *          Reserved                                  [0]->[0]
     */
    buffer[offset++] = 0;

    return offset;
}
#endif

/* 
 *  Insert Ethernet Header
 */
STATIC int
_insert_eth(uint8* buffer, int byte_start_offset,
    bcm_mac_t dest_addr,
    bcm_mac_t src_addr, 
    uint16* tpids,
    bcm_vlan_t* vlans,
    uint8* vlan_pri,
    uint8* vlan_cfi,
    int no_tags,
    uint16 ether_type) 
{
    int i = 0, offset = byte_start_offset; 
    for (i = 0; i < 6; i++) {
        buffer[offset++] = dest_addr[i];
    }
    for (i = 0; i < 6; i++) {
        buffer[offset++] = src_addr[i];
    }
    i = 0;
    while (no_tags) {
        buffer[offset++] = (tpids[i]>>8)&0xff;
        buffer[offset++] = tpids[i]&0xff;
        buffer[offset++] = (vlan_pri[i]<<5)|((vlan_cfi[i]&0x1)<<4)|((vlans[i]>>8)&0xff);
        buffer[offset++] = vlans[i]&0xff;
        --no_tags;
        i++;
    }
    
    buffer[offset++] = (ether_type>>8)&0xff;
    buffer[offset++] = ether_type&0xff;
    
    return offset;
}

/* 
 *  FL PDU Header format: Refer to MEF Sat spec.
 */
STATIC int
_insert_fl_pdu(uint8* buffer, int byte_start_offset, int header_len, int pkt_len,
    uint8 mel, 
    uint8 version,
    uint8 opcode,
    uint8 flags,
    uint8 tlv_offset,
    uint16 source_mep_id,
    uint32 session_id,
    uint8 add_data_tlv)
{
    int tmp = 0, offset = byte_start_offset;
    uint16 data_tlv_len;
    uint32 data_tlv_type = 3;
    
    buffer[offset++] = ((mel&0x7)<<5)|(version&0x1f);
    buffer[offset++] = opcode;
    buffer[offset++] = flags;
    buffer[offset++] = tlv_offset;                    /* TLV Offset: 16 */
    buffer[offset++] = source_mep_id>>8;
    buffer[offset++] = source_mep_id&0xff;
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = (session_id>>24)&0xff;
    buffer[offset++] = (session_id>>16)&0xff;
    buffer[offset++] = (session_id>>8)&0xff;
    buffer[offset++] = (session_id&0xff);
    offset += 4;                                      /* Sequence Number */
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = 0;                             /* Reserved */
    buffer[offset++] = 0;                             /* Reserved */ 
    buffer[offset++] = 0;                             /* Reserved */

    if (add_data_tlv) {
        tmp = offset - header_len;
        data_tlv_len = pkt_len - tmp - 4 - 3;     /* 4bytes CRC for whole packet + 3bytes Data TLV Header */
    
        buffer[offset++] = data_tlv_type;             /* Data TLV Type */
        buffer[offset++] = (data_tlv_len>>8)&0xff;    /* Data TLV Length */
        buffer[offset++] = (data_tlv_len&0xff);       /* Data TLV Length */
    }
    
    return offset;
}

/* 
 *  FL Packet format will be :
 *    PTCH-2-|-ITMH-|-EthernetHeader-|-FL PDU for Down MEPs
 */
STATIC int
_sat_gtf_packet_header_config_fl_down(int unit, uint8* pkt_data)
{
    int offset = 0;
    bcm_mac_t pkt_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t pkt_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    uint16 pkt_tpid = 0x8100;
    bcm_vlan_t pkt_vid = 0x100;
    uint8 pkt_pri = 0, pkt_cfi = 0;
    uint16 pkt_eth_type = 0x8902;
    uint8 pdu_mel = 0x3;
    uint8 pdu_version = 0x0;
    uint8 pdu_fl_opcode = 43;
    uint8 pdu_flags = 0;
    uint16 pdu_source_mep_id = 0;
    uint32 pdu_session_id = 0x12345678;
    uint8 pdu_add_data_tlv = 0;

    /* 1. Prepare internal header */
#if defined(BCM_ESW_SUPPORT)
    bcm_mac_t olp_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t xgs_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    uint16 olp_tpid = 0xa96d;
    uint16 olp_vlan = 0xdd4;
    uint8  olp_pri = 7;
    uint16 olp_ether_type = 0x5843;
    int is_down = 1;
    int dst_port = 1;
    int stamp_action = 0;   /* add no TS */
    int oam_offset = 22;
    {
        /* 1.1 Prepare XGS OLP L2 Header */
        _gtf_pkt_cfg_default.dev_header_1_offset = offset;
        offset = _insert_xgs_olp_l2(pkt_data, offset, 
            /* dest_addr */ xgs_mac, 
            /* src_addr */ olp_mac, 
            /* tpid */ olp_tpid, 
            /* vlan */ olp_vlan, 
            /* vlan_pri */ olp_pri, 
            /* vlan_cfi */ 0, 
            /* ether_type */ olp_ether_type);

        /* 1.2 Prepare XGS OLP TX Header */
        _gtf_pkt_cfg_default.dev_header_2_offset = offset;
        offset = _insert_xgs_olp_tx(pkt_data, offset, 
            /* is_down_mep */ is_down, 
            /* modid */ 0, 
            /* port */ dst_port,
            /* timestamp_action */ stamp_action,
            /* oam_offset */ oam_offset);
    }
#elif defined(BCM_PETRA_SUPPORT)
    uint8 next_is_itmh = 1;
    uint8 opaque_value = 7;
    uint8 src_gport = 13;
    int dst_gport = 14;
    uint8 pri = 3;
    {
        /* 1.1 Prepare internal DUNE header: PTCH */
        _gtf_pkt_cfg_default.dev_header_1_offset = offset;
        offset = _insert_dnx_ptch2(pkt_data, offset, 
            /* next_header_is_itmh */ next_is_itmh, 
            /* opaque_value */ opaque_value, 
            /* pp_ssp */ src_gport);

        /* 1.2 Prepare internal DUNE header: ITMH */
        _gtf_pkt_cfg_default.dev_header_2_offset = offset;
        offset = _insert_dnx_itmh_arad(pkt_data, offset, 
            /* dst_sys_port */ dst_gport, 
            /* fwd_tc */ pri, 
            /* fwd_dp */ 0, 
            /* snoop_cmd */ 0, 
            /* in_mirr_flag */ 0, 
            /* pph_type */ 0);
    }
#endif
    _gtf_pkt_cfg_default.dev_header_len = offset;

    /* 2. Prepare Ethernet header */
    _gtf_pkt_cfg_default.eth_header_offset = offset;
    offset = _insert_eth(pkt_data, offset, 
        /* dmac */ pkt_dmac, 
        /* smac */ pkt_smac, 
        /* tpid */ &pkt_tpid,
        /* vid */ &pkt_vid, 
        /* pri */ &pkt_pri, 
        /* cfi */ &pkt_cfi, 
        /* num_of_tags */ 1, 
        /* ether_type */ pkt_eth_type);
    
    /* 3. Prepare FL PDU header */
    _gtf_pkt_cfg_default.pdu_offset = offset;
    offset = _insert_fl_pdu(pkt_data, offset,
        _gtf_pkt_cfg_default.dev_header_len,
        _gtf_pkt_cfg_default.packet_length[0][0],
        pdu_mel, 
        pdu_version, 
        pdu_fl_opcode, 
        pdu_flags, 
        /* tlv_offset */ 16,
        pdu_source_mep_id, 
        pdu_session_id, 
        pdu_add_data_tlv);

    _gtf_pkt_cfg_default.ctrl_info_len = offset;
    
    return offset;
}

STATIC cmd_result_t
sat_gtf_packet_default_load(int unit, bcm_sat_gtf_t id)
{
    bcm_pkt_t *sat_header;
    
    SAT_GTF_ID_CHECK_RETURN(id);
    if (_gtf_pkt_cfg[id].sat_header) {
        bcm_pkt_free(unit, _gtf_pkt_cfg[id].sat_header);
        _gtf_pkt_cfg[id].sat_header = NULL;
    }
    
    sal_memcpy(&_gtf_pkt_cfg[id], &_gtf_pkt_cfg_default,
               sizeof(_gtf_pkt_cfg_default));
    SAT_IF_ERROR_RETURN(bcm_pkt_alloc(unit, _gtf_pkt_header_max_len, 0,
                                      &sat_header));
    sal_memcpy(sat_header->pkt_data->data,
               _gtf_pkt_cfg_default.cfg.header_info.pkt_data->data,
               _gtf_pkt_cfg_default.cfg.header_info.pkt_data->len);
    sat_header->pkt_data->len = _gtf_pkt_cfg_default.cfg.header_info.pkt_data->len;
    _gtf_pkt_cfg[id].cfg.header_info.pkt_data = sat_header->pkt_data;
    _gtf_pkt_cfg[id].sat_header = sat_header;

    return CMD_OK;
}

STATIC cmd_result_t
sat_gtf_packet_default_init(int unit)
{
    static int flag = 0;
    int id, pri, index;
    int pkt_size = 0;
    bcm_pkt_t *sat_header;

    if (flag) {
        return CMD_OK;
    }
    flag = 1;

    _sat_gtf_pkt_info_t_init(&_gtf_pkt_cfg_default);
    /* Fill in default GTF packet config. */
    /* 1. Configure header type*/
    _gtf_pkt_cfg_default.cfg.sat_header_type = bcmSatHeaderUserDefined;

    /* 2. Configure header info */
    SAT_IF_ERROR_RETURN(bcm_pkt_alloc(unit, _gtf_pkt_header_max_len, 0,
                                      &sat_header));
    pkt_size = _sat_gtf_packet_header_config_fl_down(
                    unit, sat_header->pkt_data->data);
    sat_header->pkt_data->len = pkt_size;
    _gtf_pkt_cfg_default.cfg.header_info.pkt_data = sat_header->pkt_data;    
    _gtf_pkt_cfg_default.cfg.header_info.blk_count = 1;
    _gtf_pkt_cfg_default.sat_header = sat_header;

    /* 3. Configure payload */
    _gtf_pkt_cfg_default.cfg.payload.payload_type = bcmSatPayloadPRBS;
    for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
        _gtf_pkt_cfg_default.cfg.payload.payload_pattern[index] = 0x1;
    }

    /* 4. Configure packet edit */
    for (pri = 0; pri < BCM_SAT_GTF_NUM_OF_PRIORITIES; pri++) {
        _gtf_pkt_cfg_default.cfg.packet_edit[pri].packet_length[0] = 64;
        _gtf_pkt_cfg_default.cfg.packet_edit[pri].pattern_length = 1;
        for (index = 0; index < BCM_SAT_GTF_MAX_STAMPS; index++) {
            _gtf_pkt_cfg_default.cfg.packet_edit[pri].stamps[index].stamp_type =
                bcmSatStampInvalid;
            _gtf_pkt_cfg_default.cfg.packet_edit[pri].stamps[index].field_type =
                bcmSatStampFieldUserDefined;
            _gtf_pkt_cfg_default.cfg.packet_edit[pri].stamps[index].inc_step = 1;
            _gtf_pkt_cfg_default.cfg.packet_edit[pri].stamps[index].inc_period_packets = 1;
        }
        _gtf_pkt_cfg_default.cfg.packet_edit[pri].number_of_ctfs = 1;
    }

    /* 5. Configure packet offset */
    _gtf_pkt_cfg_default.cfg.offsets.seq_number_offset = -1;

    /* Init GTF packet config DB. */
    for (id = 0; id < _SAT_CMD_GTF_CNT_MAX; id++) {
        _sat_gtf_pkt_info_t_init(&_gtf_pkt_cfg[id]);
        SAT_IF_ERROR_RETURN(sat_gtf_packet_default_load(unit, id));
    }

    return CMD_OK;
}

STATIC cmd_result_t
sat_subcommand_exec(int unit, args_t *a, sat_subcommand_t *command_list)
{
    cmd_result_t rv;
    char *arg = NULL;
    sat_subcommand_t *list = command_list;
    
    arg = ARG_GET(a);

    /* Match & exec subcommand. */
    if (arg) {
        while (list->str) {
            if (parse_cmp(list->str, arg, 0)) {
                rv = (*list->func)(unit, a);
                if ((rv == CMD_USAGE) || (rv == CMD_NFND)) {
                    cli_out("\t%s\n", list->help);
                    rv = CMD_OK;
                }
                return rv;
            }
            list++;
        }
    }

    /* Provide help when no match. */
    list = command_list;
    while (list->str) {
        cli_out("\t%s\n", list->help);
        list++;
    }

    return CMD_OK;
}

STATIC cmd_result_t
sat_gtf_show(int unit, bcm_sat_gtf_t id, void *cookie)
{
    int pri;
    uint64 stat;
    bcm_sat_gtf_bandwidth_t bw;
    bcm_sat_gtf_rate_pattern_t pattern;

    cli_out("GTF %d:\n", id);
    for (pri = 0; pri < BCM_SAT_GTF_NUM_OF_PRIORITIES; pri++) {
        bcm_sat_gtf_bandwidth_t_init(&bw);
        bcm_sat_gtf_rate_pattern_t_init(&pattern);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_bandwidth_get(unit, id, pri, &bw));
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_rate_pattern_get(unit, id, pri,
                                                         &pattern));
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_stat_get(unit, id, pri, 0,
                                bcmSatGtfStatPacketCount, &stat));
        cli_out("\tPriority %d - bandwidth: rate=%d kbps, burst=%d kbit\n",
                pri, bw.rate, bw.max_burst);
        cli_out("\t             pattern  : mode=%d, flag=0x%08x\n",
                pattern.rate_pattern_mode, pattern.flags);
        cli_out("\t                        thresh_low=%d, thresh_high=%d \n",
                pattern.low_threshold, pattern.high_threshold);
        cli_out("\t                        " \
                "cnt_iter=%d, cnt_burst=%d, cnt_interval=%d\n",
                pattern.stop_interval_count, pattern.stop_burst_count,
                pattern.stop_iter_count);
        cli_out("\t                        weight_burst=%d, weight_interval=%d\n",
                pattern.burst_packet_weight, pattern.interval_packet_weight);
        cli_out("\t             TX       : 0x%08x%08x packets\n",
                COMPILER_64_HI(stat), COMPILER_64_LO(stat));
    }

    return CMD_OK;
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
STATIC cmd_result_t
sat_gtf_header_raw_set(int unit, args_t *a)
{
    char *arg = NULL;
    int index, header_len;
    int cnt, pos, input_len;
    char *arg_data = NULL;
    uint8 *input_data = NULL;
    parse_table_t pt;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_packet_config_t *config;
    
    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_GTF_ID_CHECK_RETURN(id);

    config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                       "bcm_sat_gtf_packet_config");
    if (!config) {
        return CMD_FAIL;
    }

    header_len = (2*_gtf_pkt_cfg[id].dev_header_len + 5);
    arg_data = sal_alloc(header_len, "sat_header");
    if (!arg_data) {
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_data, 0, header_len);
    sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
               sizeof(bcm_sat_gtf_packet_config_t));
    pos = 0;
    for (index = 0; index < _gtf_pkt_cfg[id].dev_header_len; index++) {
        cnt = sal_sprintf(arg_data + pos, "%02x",
                      *(config->header_info.pkt_data->data + index));
        pos += cnt;
    }
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "rawData", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_data, NULL);
    if (!ARG_CNT(a)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_OK;
    }
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_FAIL;
    }
    (void)_parse_packet_data(unit, arg_data, &input_len, &input_data);
    parse_arg_eq_done(&pt);/* Do not do this before arg_data is parsed. */
    if (!input_data) {
        sal_free(config);
        return CMD_FAIL;
    }
    
    sal_memset(config->header_info.pkt_data->data, 0,
               _gtf_pkt_cfg[id].dev_header_len);
    sal_memcpy(config->header_info.pkt_data->data, input_data, input_len);
    (void)_parse_packet_data_end(unit, arg_data, &input_len, input_data);
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
    sal_memcpy(&_gtf_pkt_cfg[id].cfg, config,
               sizeof(bcm_sat_gtf_packet_config_t));
    sal_free(config);

    return CMD_OK;
}
#endif

#if defined(BCM_ESW_SUPPORT)
STATIC cmd_result_t
sat_gtf_olpheader(int unit, args_t *a)
{
    char *arg = NULL;
    cmd_result_t retCode;
    sal_mac_addr_t arg_dmac, arg_smac;
    uint32 arg_tpid, arg_vid, arg_pri, arg_cfi, arg_eth_type;
    uint32 arg_down, arg_mod, arg_port, arg_ts, arg_ts_format, arg_ts_offset;
    uint32 sub_type, ts_action, oam_replacement_offset;
    uint8 *data;
    parse_table_t pt;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_packet_config_t *config;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Set", arg, 0)) {
        return sat_gtf_header_raw_set(unit, a);
    } else if (parse_cmp("L2", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);

        config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                           "bcm_sat_gtf_packet_config");
        if (!config) {
            return CMD_FAIL;
        }
        sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
                   sizeof(bcm_sat_gtf_packet_config_t));
        data = config->header_info.pkt_data->data +
                _gtf_pkt_cfg[id].dev_header_1_offset;
        sal_memcpy(arg_dmac, data, sizeof(arg_dmac));
        sal_memcpy(arg_smac, data + 6, sizeof(arg_smac));
        arg_tpid = (*(data + 12) << 8) + *(data + 13);
        arg_vid = ((*(data + 14) & 0xf) << 8) + *(data + 15);
        arg_pri = (*(data + 14) & 0xe0) >> 5;
        arg_cfi = (*(data + 14) & 0x10) >> 4;
        arg_eth_type = (*(data + 16) << 8) + *(data + 17);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "DMac", PQ_DFL|PQ_MAC, NULL,
                        &arg_dmac, NULL);
        parse_table_add(&pt, "SMac", PQ_DFL|PQ_MAC, NULL,
                        &arg_smac, NULL);
        parse_table_add(&pt, "Tpid", PQ_DFL|PQ_HEX, NULL,
                        &arg_tpid, NULL);
        parse_table_add(&pt, "Vlan", PQ_DFL|PQ_INT, NULL,
                        &arg_vid, NULL);
        parse_table_add(&pt, "vlanPri", PQ_DFL|PQ_INT, NULL,
                        &arg_pri, NULL);
        parse_table_add(&pt, "vlanCfi", PQ_DFL|PQ_INT, NULL,
                        &arg_cfi, NULL);
        parse_table_add(&pt, "ETHertype", PQ_DFL|PQ_HEX, NULL,
                        &arg_eth_type, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            sal_free(config);
            return retCode;
        }
        (void)_insert_xgs_olp_l2(data, 0, arg_dmac, arg_smac, arg_tpid,
                                 arg_vid, arg_pri, arg_cfi, arg_eth_type);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
        sal_memcpy(&_gtf_pkt_cfg[id].cfg, config,
                   sizeof(bcm_sat_gtf_packet_config_t));
        sal_free(config);
    } else if (parse_cmp("Tx", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);
        config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                           "bcm_sat_gtf_packet_config");
        if (!config) {
            return CMD_FAIL;
        }

        sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
                   sizeof(bcm_sat_gtf_packet_config_t));
        data = config->header_info.pkt_data->data +
                _gtf_pkt_cfg[id].dev_header_2_offset;
        sub_type = ((*(data + 1) & 0xf) << 4) + ((*(data + 2) & 0xf0) >> 4);
        if (sub_type == 2) {
            arg_down = 1;
        }else {
            arg_down = 0;
        }
        arg_mod = *(data + 3);
        arg_port = *(data + 4);
        ts_action = (*(data + 9) >> 2) & 0x3;
        if (ts_action == 0) {
            arg_ts = 0;
            arg_ts_format = bcmSATTimestampFormatIEEE1588v1;
        } else {
            arg_ts = 1;
            arg_ts_format = (ts_action == 2)? bcmSATTimestampFormatNTP:\
                                              bcmSATTimestampFormatIEEE1588v1;
        }
        
        oam_replacement_offset = ((*(data + 9) & 0x3) << 6) +
                                 ((*(data + 10) & 0xfc) >> 2);
        arg_ts_offset = (oam_replacement_offset * 2) + 14;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "isDown", PQ_DFL|PQ_BOOL, NULL,
                        &arg_down, NULL);
        parse_table_add(&pt, "destModid", PQ_DFL|PQ_INT, NULL,
                        &arg_mod, NULL);
        parse_table_add(&pt, "destPort", PQ_DFL|PQ_INT, NULL,
                        &arg_port, NULL);
        parse_table_add(&pt, "isAddTs", PQ_DFL|PQ_BOOL, NULL,
                        &arg_ts, NULL);
        parse_table_add(&pt, "timestampFormat", PQ_DFL|PQ_INT, NULL,
                        &arg_ts_format, NULL);
        parse_table_add(&pt, "OffsetofTs", PQ_DFL|PQ_INT, NULL,
                        &arg_ts_offset, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            sal_free(config);
            return retCode;
        }
        if (!arg_ts) {
            ts_action = 0;
        } else {
            ts_action = (arg_ts_format == bcmSATTimestampFormatNTP)? 2:1;
        }
        (void)_insert_xgs_olp_tx(data, 0, arg_down, arg_mod, arg_port,
                                 ts_action, arg_ts_offset);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
        sal_memcpy(&_gtf_pkt_cfg[id].cfg, config,
                   sizeof(bcm_sat_gtf_packet_config_t));
        sal_free(config);
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}
#endif
#if defined(BCM_PETRA_SUPPORT)
STATIC cmd_result_t
sat_gtf_dnxheader(int unit, args_t *a)
{
    char *arg = NULL;
    cmd_result_t retCode;
    uint32 arg_port, arg_attr, arg_next;
    uint32 arg_pri, arg_color, arg_flag, arg_cmd, arg_type;
    uint32 arg_subtype, arg_down, arg_offset;
    uint64 arg_data;
    uint32 fwd_dest_info, ts_data_hi, ts_data_lo, pos;
    uint8 *data = NULL;
    parse_table_t pt;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_packet_config_t config;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Set", arg, 0)) {
        return sat_gtf_header_raw_set(unit, a);
    } else if (parse_cmp("Ptch", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);
        sal_memcpy(&config, &_gtf_pkt_cfg[id].cfg, sizeof(config));
        data = config.header_info.pkt_data->data +
                _gtf_pkt_cfg[id].dev_header_1_offset;
        arg_port = *(data + 1);
        arg_attr = (*data & 0x70) >> 4;
        arg_next = ((*data & 0x80) >> 7) ^ 0x1;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "srcGport", PQ_DFL|PQ_INT, NULL,
                        &arg_port, NULL);
        parse_table_add(&pt, "opaqueAttr", PQ_DFL|PQ_INT, NULL,
                        &arg_attr, NULL);
        parse_table_add(&pt, "isNextHeaderItmh", PQ_DFL|PQ_BOOL, NULL,
                        &arg_next, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        (void)_insert_dnx_ptch2(data, 0, arg_next, arg_attr, arg_port);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, &config));
        sal_memcpy(&_gtf_pkt_cfg[id].cfg, &config, sizeof(config));
    } else if (parse_cmp("Itmh", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);
        sal_memcpy(&config, &_gtf_pkt_cfg[id].cfg, sizeof(config));
        data = config.header_info.pkt_data->data +
                _gtf_pkt_cfg[id].dev_header_2_offset;
        fwd_dest_info = ((*(data + 1) & 0x0f) << 16) +
                        (*(data + 2) << 8) + *(data + 3);
        arg_port = fwd_dest_info & (~0xc0000);
        arg_pri = ((*(data + 1) & 0xc0) >> 6) + ((*data & 0x1) << 2);
        arg_color = (*(data + 1) & 0x30) >> 4;
        arg_flag = (*data & 0x20) >> 5;
        arg_cmd = (*data & 0x1e) >> 1;
        arg_type = (*data & 0xc0) >> 6;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "dstGport", PQ_DFL|PQ_INT, NULL,
                        &arg_port, NULL);
        parse_table_add(&pt, "Prio", PQ_DFL|PQ_INT, NULL,
                        &arg_pri, NULL);
        parse_table_add(&pt, "Color", PQ_DFL|PQ_INT, NULL,
                        &arg_color, NULL);
        parse_table_add(&pt, "inMirrorFlag", PQ_DFL|PQ_BOOL, NULL,
                        &arg_flag, NULL);
        parse_table_add(&pt, "snoopCMD", PQ_DFL|PQ_INT, NULL,
                        &arg_cmd, NULL);
        parse_table_add(&pt, "pphType", PQ_DFL|PQ_INT, NULL,
                        &arg_type, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        (void)_insert_dnx_itmh_arad(data, 0, arg_port, arg_pri, arg_color,
                                    arg_cmd, arg_flag, arg_type);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, &config));
        sal_memcpy(&_gtf_pkt_cfg[id].cfg, &config, sizeof(config));
    } else if (parse_cmp("Otsh", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);
        sal_memcpy(&config, &_gtf_pkt_cfg[id].cfg, sizeof(config));
        if (_gtf_pkt_cfg[id].dev_header_3_offset > 0) { /* OAM header included */
            data = config.header_info.pkt_data->data +
                   _gtf_pkt_cfg[id].dev_header_3_offset;
            arg_type = (*data & 0xc0) >> 6;
            arg_subtype = (*data & 0x38) >> 3;
            arg_down = ((*data & 0x4) >> 2) ^ 1;
            ts_data_hi = (*data & 0x3);
            ts_data_lo = (*(data + 1) << 24) + (*(data + 2) << 16) +
                         (*(data + 3) << 8) + *(data + 4);
            COMPILER_64_SET(arg_data, ts_data_hi, ts_data_lo);
            arg_offset = *(data + 5);
        } else {    /* Currently no OAM header */
            arg_type = 0;
            arg_subtype = 0;
            arg_down = 0;
            COMPILER_64_SET(arg_data, 0, 0);
            arg_offset = 0;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Type", PQ_DFL|PQ_INT, NULL,
                        &arg_type, NULL);
        parse_table_add(&pt, "SubType", PQ_DFL|PQ_INT, NULL,
                        &arg_subtype, NULL);
        parse_table_add(&pt, "isDowN", PQ_DFL|PQ_BOOL, NULL,
                        &arg_down, NULL);
        parse_table_add(&pt, "tsData", PQ_DFL|PQ_INT64, NULL,
                        &arg_data, NULL);
        parse_table_add(&pt, "tsOffset", PQ_DFL|PQ_INT, NULL,
                        &arg_offset, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        if (_gtf_pkt_cfg[id].dev_header_3_offset < 0) { /* newly add OAM header */
            pos = _gtf_pkt_cfg[id].ctrl_info_len;
            while (pos >= _gtf_pkt_cfg[id].eth_header_offset) {
                *(config.header_info.pkt_data->data + pos + 6) =
                *(config.header_info.pkt_data->data + pos);
                pos--;
            }
            _gtf_pkt_cfg[id].dev_header_3_offset =
                    _gtf_pkt_cfg[id].eth_header_offset;
            _gtf_pkt_cfg[id].eth_header_offset += 6; /* OAM header takes 6Bytes */
            _gtf_pkt_cfg[id].pdu_offset += 6;
            _gtf_pkt_cfg[id].dev_header_len += 6;
            _gtf_pkt_cfg[id].ctrl_info_len += 6;
            config.header_info.pkt_data->len += 6;
            data = config.header_info.pkt_data->data +
                   _gtf_pkt_cfg[id].dev_header_3_offset;
            sal_memset(data, 0, 6);
        }
        (void)_insert_dnx_oam_ts(data, 0, arg_type, arg_subtype, arg_down? 0:1,
                                 arg_data, arg_offset);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, &config));
        sal_memcpy(&_gtf_pkt_cfg[id].cfg, &config, sizeof(config));
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}
#endif

STATIC cmd_result_t
sat_gtf_packet_data(int unit, args_t *a)
{
    char *arg = NULL;
    sal_mac_addr_t arg_dmac, arg_smac;
    uint32 arg_tpid;
    int index, header_len, pkt_header_len;
    int cnt, pos, input_len, eth_type, ext_data_offset;
    char *arg_data = NULL;
    uint8 *input_data = NULL, *data, arg_pri, cfi;
    uint16 arg_vid, tpid;
    parse_table_t pt;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_packet_config_t *config;
    
    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_GTF_ID_CHECK_RETURN(id);

    config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                       "bcm_sat_gtf_packet_config");
    if (!config) {
        return CMD_FAIL;
    }

    pkt_header_len = _gtf_pkt_cfg[id].ctrl_info_len -
                     _gtf_pkt_cfg[id].eth_header_offset;
    header_len = (2*(pkt_header_len) + 5);
    arg_data = sal_alloc(header_len, "sat_header");
    if (!arg_data) {
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_data, 0, header_len);
    sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
               sizeof(bcm_sat_gtf_packet_config_t));
    pos = 0;
    ext_data_offset = _gtf_pkt_cfg[id].eth_header_offset + 12 + 4;
    for (index = ext_data_offset;
         index < _gtf_pkt_cfg[id].ctrl_info_len; index++) {
        cnt = sal_sprintf(arg_data + pos, "%02x",
                      *(config->header_info.pkt_data->data + index));
        pos += cnt;
    }
    data = config->header_info.pkt_data->data +
                _gtf_pkt_cfg[id].eth_header_offset;
    sal_memcpy(arg_dmac, data, sizeof(arg_dmac));
    sal_memcpy(arg_smac, data + 6, sizeof(arg_smac));
    arg_tpid = (*(data + 12) << 8) + *(data + 13);
    arg_vid = ((*(data + 14) & 0xf) << 8) + *(data + 15);
    arg_pri = (*(data + 14) & 0xe0) >> 5;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DMac", PQ_DFL|PQ_MAC, NULL,
                    &arg_dmac, NULL);
    parse_table_add(&pt, "SMac", PQ_DFL|PQ_MAC, NULL,
                    &arg_smac, NULL);
    parse_table_add(&pt, "Tpid", PQ_DFL|PQ_HEX, NULL,
                    &arg_tpid, NULL);
    parse_table_add(&pt, "Vlan", PQ_DFL|PQ_INT16, NULL,
                    &arg_vid, NULL);
    parse_table_add(&pt, "vlanPri", PQ_DFL|PQ_INT8, NULL,
                    &arg_pri, NULL);
    parse_table_add(&pt, "extData", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_data, NULL);
    if (!ARG_CNT(a)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_OK;
    }
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_FAIL;
    }
    (void)_parse_packet_data(unit, arg_data, &input_len, &input_data);
    parse_arg_eq_done(&pt);/* Do not do this before arg_data is parsed. */
    if (!input_data) {
        sal_free(config);
        return CMD_FAIL;
    }

    if (input_len > (_gtf_pkt_header_max_len - ext_data_offset)) {
        (void)_parse_packet_data_end(unit, arg_data, &input_len, input_data);
        cli_out("Data length exceeds limit.\n");
        sal_free(config);
        return CMD_FAIL;
    }
    
    cfi = (*(data + 14) & 0x10) >> 4;
    eth_type = (*(data + 16) << 8) + *(data + 17);
    tpid = arg_tpid & 0xffff;
    (void)_insert_eth(data, 0, arg_dmac, arg_smac, &tpid, &arg_vid, &arg_pri,
                      &cfi, 1, eth_type);
    
    sal_memset(config->header_info.pkt_data->data + ext_data_offset,
               0, _gtf_pkt_cfg[id].ctrl_info_len - ext_data_offset);
    sal_memcpy(config->header_info.pkt_data->data + ext_data_offset,
               input_data, input_len);
    (void)_parse_packet_data_end(unit, arg_data, &input_len, input_data);
    _gtf_pkt_cfg[id].ctrl_info_len = ext_data_offset + input_len;
    
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
    sal_memcpy(&_gtf_pkt_cfg[id].cfg, config, sizeof(bcm_sat_gtf_packet_config_t));
    sal_free(config);

    return CMD_OK;
}

STATIC cmd_result_t
sat_gtf_packet_length(int unit, args_t *a)
{
    char *arg = NULL;
    char *arg_length, *arg_pattern;
    uint32 arg_pattern_len;
    int32 length[BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS];
    int32 pattern[BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH];
    int pri, index, cnt, pos;
    int pattern_len = BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH*4;
    int length_len = BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS*5;
    parse_table_t pt;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_packet_config_t *config;
    
    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_GTF_ID_CHECK_RETURN(id);
    SAT_ARG_NUMB_GET(a, arg, pri);

    config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                       "bcm_sat_gtf_packet_config");
    if (!config) {
        return CMD_FAIL;
    }

    sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
               sizeof(bcm_sat_gtf_packet_config_t));
    arg_length = sal_alloc(length_len, "sat_length");
    if (!arg_length) {
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_length, 0, length_len);
    pos = 0;
    for (index = 0;
         (index < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS) &&
         (config->packet_edit[pri].packet_length[index] > 0);
         index++) {
        cnt = sal_sprintf(arg_length + pos, "%u,",
                      config->packet_edit[pri].packet_length[index]);
        pos += cnt;
    }
    arg_pattern = sal_alloc(pattern_len, "sat_pattern");
    if (!arg_pattern) {
        sal_free(arg_length);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_pattern, 0, pattern_len);
    pos = 0;
    for (index = 0; index < config->packet_edit[pri].pattern_length; index++) {
        cnt = sal_sprintf(arg_pattern + pos, "%u,",
                      config->packet_edit[pri].packet_length_pattern[index]);
        pos += cnt;
    }
    arg_pattern_len = config->packet_edit[pri].pattern_length;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "pktLens", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_length, NULL);
    parse_table_add(&pt, "pktlenPatterns", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_pattern, NULL);
    parse_table_add(&pt, "PatternLens", PQ_DFL|PQ_INT, NULL,
                    &arg_pattern_len, NULL);
    if (!ARG_CNT(a)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_OK;
    }
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(length, 0, sizeof(length));
    (void)_list_to_data_array(arg_length,
                              BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS,
                              length);
    sal_memset(pattern, 0, sizeof(pattern));
    (void)_list_to_data_array(arg_pattern,
                              BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH, pattern);
    parse_arg_eq_done(&pt);/* Do not do this before arg_pattern is parsed. */

    for (index = 0;
         index < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; index++) {
        config->packet_edit[pri].packet_length[index] = length[index];
    }
    for (index = 0;
         index < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; index++) {
        config->packet_edit[pri].packet_length_pattern[index] = pattern[index];
    }
    config->packet_edit[pri].pattern_length = arg_pattern_len;
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
    sal_memcpy(&_gtf_pkt_cfg[id].cfg, config,
               sizeof(bcm_sat_gtf_packet_config_t));
    sal_free(config);
    return CMD_OK;
}

STATIC cmd_result_t
sat_gtf_packet_stamp(int unit, args_t *a)
{
    char *arg = NULL;
    char *arg_type, *arg_field, *arg_step, *arg_period, *arg_value, *arg_offset;
    uint32 arg_stamp_cnt;
    int32 type[BCM_SAT_GTF_MAX_STAMPS], field[BCM_SAT_GTF_MAX_STAMPS];
    int32 step[BCM_SAT_GTF_MAX_STAMPS], period[BCM_SAT_GTF_MAX_STAMPS];
    int32 value[BCM_SAT_GTF_MAX_STAMPS], offset[BCM_SAT_GTF_MAX_STAMPS];
    int pri, index, cnt;
    int type_pos = 0, field_pos = 0, step_pos = 0;
    int period_pos = 0, value_pos = 0, offset_pos = 0;
    int list_len = BCM_SAT_GTF_MAX_STAMPS*4;
    parse_table_t pt;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_packet_config_t *config;
    
    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_GTF_ID_CHECK_RETURN(id);
    SAT_ARG_NUMB_GET(a, arg, pri);

    config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                       "bcm_sat_gtf_packet_config");
    if (!config) {
        return CMD_FAIL;
    }

    sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
               sizeof(bcm_sat_gtf_packet_config_t));
    if (!(arg_type = sal_alloc(list_len, "sat_type"))) {
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_type, 0, list_len);
    if (!(arg_field = sal_alloc(list_len, "sat_field"))) {
        sal_free(arg_type);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_field, 0, list_len);
    if (!(arg_step = sal_alloc(list_len, "sat_step"))) {
        sal_free(arg_type);
        sal_free(arg_field);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_step, 0, list_len);
    if (!(arg_period = sal_alloc(list_len, "sat_period"))) {
        sal_free(arg_type);
        sal_free(arg_field);
        sal_free(arg_step);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_period, 0, list_len);
    if (!(arg_value = sal_alloc(list_len, "sat_value"))) {
        sal_free(arg_type);
        sal_free(arg_field);
        sal_free(arg_step);
        sal_free(arg_period);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_value, 0, list_len);
    if (!(arg_offset = sal_alloc(list_len, "sat_offset"))) {
        sal_free(arg_type);
        sal_free(arg_field);
        sal_free(arg_step);
        sal_free(arg_period);
        sal_free(arg_value);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(arg_offset, 0, list_len);
    for (index = 0; index < BCM_SAT_GTF_MAX_STAMPS; index++) {
        cnt = sal_sprintf(arg_type + type_pos, "%u,",
                      config->packet_edit[pri].stamps[index].stamp_type);
        type_pos += cnt;
        cnt = sal_sprintf(arg_field + field_pos, "%u,",
                      config->packet_edit[pri].stamps[index].field_type);
        field_pos += cnt;
        cnt = sal_sprintf(arg_step + step_pos, "%u,",
                      config->packet_edit[pri].stamps[index].inc_step);
        step_pos += cnt;
        cnt = sal_sprintf(arg_period + period_pos, "%u,",
                      config->packet_edit[pri].stamps[index].inc_period_packets);
        period_pos += cnt;
        cnt = sal_sprintf(arg_value + value_pos, "%u,",
                      config->packet_edit[pri].stamps[index].value);
        value_pos += cnt;
        cnt = sal_sprintf(arg_offset + offset_pos, "%u,",
                      config->packet_edit[pri].stamps[index].offset);
        offset_pos += cnt;
    }
    arg_stamp_cnt = config->packet_edit[pri].number_of_stamps;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "StampTypes", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_type, NULL);
    parse_table_add(&pt, "stampFieldTypes", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_field, NULL);
    parse_table_add(&pt, "stampIncSteps", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_step, NULL);
    parse_table_add(&pt, "stampIncPeriods", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_period, NULL);
    parse_table_add(&pt, "stampValues", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_value, NULL);
    parse_table_add(&pt, "stampOffsets", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_offset, NULL);
    parse_table_add(&pt, "Numofstamps", PQ_DFL|PQ_INT, NULL,
                    &arg_stamp_cnt, NULL);
    if (!ARG_CNT(a)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_OK;
    }
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        sal_free(config);
        return CMD_FAIL;
    }
    sal_memset(type, 0, sizeof(type));
    (void)_list_to_data_array(arg_type, BCM_SAT_GTF_MAX_STAMPS, type);
    sal_memset(field, 0, sizeof(field));
    (void)_list_to_data_array(arg_field, BCM_SAT_GTF_MAX_STAMPS, field);
    sal_memset(step, 0, sizeof(step));
    (void)_list_to_data_array(arg_step, BCM_SAT_GTF_MAX_STAMPS, step);
    sal_memset(period, 0, sizeof(period));
    (void)_list_to_data_array(arg_period, BCM_SAT_GTF_MAX_STAMPS, period);
    sal_memset(value, 0, sizeof(value));
    (void)_list_to_data_array(arg_value, BCM_SAT_GTF_MAX_STAMPS, value);
    sal_memset(offset, 0, sizeof(offset));
    (void)_list_to_data_array(arg_offset, BCM_SAT_GTF_MAX_STAMPS, offset);
    parse_arg_eq_done(&pt);/* Do not do this before arg_offset is parsed. */
    for (index = 0; index < BCM_SAT_GTF_MAX_STAMPS; index++) {
        config->packet_edit[pri].stamps[index].stamp_type = type[index];
        config->packet_edit[pri].stamps[index].field_type = field[index];
        config->packet_edit[pri].stamps[index].inc_step = step[index];
        config->packet_edit[pri].stamps[index].inc_period_packets = period[index];
        config->packet_edit[pri].stamps[index].value = value[index];
        config->packet_edit[pri].stamps[index].offset = offset[index];
    }
    config->packet_edit[pri].number_of_stamps = arg_stamp_cnt;
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
    sal_memcpy(&_gtf_pkt_cfg[id].cfg, config,
               sizeof(bcm_sat_gtf_packet_config_t));
    sal_free(config);

    return CMD_OK;
}

STATIC cmd_result_t
sat_ctf_packet_default_load(int unit, bcm_sat_ctf_t id)
{
    int index;

    SAT_CTF_ID_CHECK_RETURN(id);
    sal_memset(&_ctf_pkt_cfg[id], 0, sizeof(_ctf_pkt_cfg[id]));
    _ctf_pkt_cfg[id].sat_header_type = bcmSatHeaderUserDefined;
    _ctf_pkt_cfg[id].payload.payload_type = bcmSatPayloadPRBS;
    for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
        _ctf_pkt_cfg[id].payload.payload_pattern[index] = 1;
    }
    _ctf_pkt_cfg[id].offsets.payload_offset = 32;
    _ctf_pkt_cfg[id].flags |= BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;

    return CMD_OK;
}

STATIC cmd_result_t
sat_ctf_trap_data2session(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_data = 0, arg_mask = 0xff, arg_id = 0;
    parse_table_t pt;
    cmd_result_t retCode;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Add", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "trapData", PQ_DFL|PQ_INT, NULL,
                        &arg_data, NULL);
        parse_table_add(&pt, "trapdataMask", PQ_DFL|PQ_HEX, NULL,
                        &arg_mask, NULL);
        parse_table_add(&pt, "sessionId", PQ_DFL|PQ_INT, NULL,
                        &arg_id, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_trap_data_to_session_map(
                                unit, arg_data, arg_mask, arg_id));
    } else if (parse_cmp("Remove", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "trapData", PQ_DFL|PQ_INT, NULL,
                        &arg_data, NULL);
        parse_table_add(&pt, "trapdataMask", PQ_DFL|PQ_HEX, NULL,
                        &arg_mask, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_trap_data_to_session_unmap(
                                unit, arg_data, arg_mask));
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
sat_ctf_show(int unit, bcm_sat_ctf_t id, void *cookie)
{
    int index;
    uint32 flags = *(uint32 *)cookie;
    bcm_sat_ctf_stat_t stat;

    bcm_sat_ctf_stat_t_init(&stat);
    cli_out("CTF %d:\n", id);
    SAT_IF_ERROR_RETURN(bcm_sat_ctf_stat_get(unit, id, flags, &stat));
    cli_out("\tReceived Packet    : 0x%08x%08x",
        COMPILER_64_HI(stat.received_packet_cnt),
        COMPILER_64_LO(stat.received_packet_cnt));
    cli_out("\t\t\tReceived Octet    : 0x%08x%08x\n",
        COMPILER_64_HI(stat.received_octet_cnt),
        COMPILER_64_LO(stat.received_octet_cnt));
    cli_out("\tOut-of-Order Packet: 0x%08x%08x",
        COMPILER_64_HI(stat.out_of_order_packet_cnt),
        COMPILER_64_LO(stat.out_of_order_packet_cnt));
    cli_out("\t\t\tOut-of-Order Octet: 0x%08x%08x\n",
        COMPILER_64_HI(stat.out_of_order_octet_cnt),
        COMPILER_64_LO(stat.out_of_order_octet_cnt));
    cli_out("\tError Packet       : 0x%08x%08x",
        COMPILER_64_HI(stat.err_packet_cnt),
        COMPILER_64_LO(stat.err_packet_cnt));
    cli_out("\t\t\tError Octet       : 0x%08x%08x\n",
        COMPILER_64_HI(stat.err_octet_cnt),
        COMPILER_64_LO(stat.err_octet_cnt));
    cli_out("\tLast Packet Delay  : 0x%08x%08x\n",
        COMPILER_64_HI(stat.last_packet_delay),
        COMPILER_64_LO(stat.last_packet_delay));
    cli_out("\tBin Delay Counter  : ");
    for (index = 0; index < BCM_SAT_CTF_STAT_MAX_NUM_OF_BINS; index++) {
        cli_out("0x%08x%08x,", COMPILER_64_HI(stat.bin_delay_counters[index]),
            COMPILER_64_LO(stat.bin_delay_counters[index]));
    }
    cli_out("\n\tAcc Delay Count    : 0x%08x%08x\n",
        COMPILER_64_HI(stat.accumulated_delay_count),
        COMPILER_64_LO(stat.accumulated_delay_count));
    cli_out("\tMin Packet Delay   : 0x%08x%08x\n",
        COMPILER_64_HI(stat.min_packet_delay),
        COMPILER_64_LO(stat.min_packet_delay));
    cli_out("\tMax Packet Delay   : 0x%08x%08x\n",
        COMPILER_64_HI(stat.max_packet_delay),
        COMPILER_64_LO(stat.max_packet_delay));
    cli_out("\tTime-of-Day Stamp  : 0x%08x%08x\n",
        COMPILER_64_HI(stat.time_of_day_stamp),
        COMPILER_64_LO(stat.time_of_day_stamp));
    cli_out("\tSec-in-Avail Count : 0x%08x%08x\n",
        COMPILER_64_HI(stat.sec_in_avail_state_counter),
        COMPILER_64_LO(stat.sec_in_avail_state_counter));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_show(int unit, args_t *a)
{
    char *arg = NULL;
    int rv = 0, foo;
    bcm_sat_gtf_t id;

    arg = ARG_GET(a);
    if (arg) {
        id = _parse_integer(arg);
        SAT_GTF_ID_CHECK_RETURN(id);
        rv = sat_gtf_show(unit, id, NULL);
    } else {
        (void)bcm_sat_gtf_traverse(unit, sat_gtf_show, (void *)&foo);
    }

    return (rv? CMD_FAIL:CMD_OK);
}

STATIC cmd_result_t
cmd_sat_gtf_create(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 flag = 0;
    bcm_sat_gtf_t id;

    SAT_IF_ERROR_RETURN(sat_gtf_packet_default_init(unit));
    arg = ARG_GET(a);
    if (arg) {
        id = _parse_integer(arg);
        SAT_GTF_ID_CHECK_RETURN(id);
        flag |= BCM_SAT_GTF_WITH_ID;
    }
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_create(unit, flag, &id));
    if (!(flag & BCM_SAT_GTF_WITH_ID)) {
        cli_out("GTF ID: %d\n", id);
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_destroy(int unit, args_t *a)
{
    char *arg = NULL;
    bcm_sat_gtf_t id;

    arg = ARG_GET(a);
    if (arg) {
        id = _parse_integer(arg);
        SAT_GTF_ID_CHECK_RETURN(id);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_destroy(unit, id));
    } else {
        cli_out("Destroy all GTF...\n");
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_destroy_all(unit));
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_start(int unit, args_t *a)
{
    char *arg = NULL;
    int pri;
    bcm_sat_gtf_t id;

    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_ARG_INT_GET(a, arg, pri);
    if (pri < _SAT_CMD_GTF_PRI_COMMON) {
        return CMD_FAIL;
    }
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_start(unit, id, pri));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_stop(int unit, args_t *a)
{
    char *arg = NULL;
    int pri;
    bcm_sat_gtf_t id;

    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_ARG_INT_GET(a, arg, pri);
    if (pri < _SAT_CMD_GTF_PRI_COMMON) {
        return CMD_FAIL;
    }
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_stop(unit, id, pri));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_stat(int unit, args_t *a)
{
    char *arg = NULL;
    uint64 stat;
    int pri;
    bcm_sat_gtf_t id;

    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_ARG_NUMB_GET(a, arg, pri);
    SAT_IF_ERROR_RETURN(bcm_sat_gtf_stat_get(unit, id, pri, 0,
                                bcmSatGtfStatPacketCount, &stat));
    cli_out("\tTransmitted : 0x%08x%08x packets\n",
        COMPILER_64_HI(stat), COMPILER_64_LO(stat));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_bandwidth(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_rate = 100, arg_burst = 1000;
    int pri;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_bandwidth_t bw;
    parse_table_t pt;
    cmd_result_t retCode;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Set", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_ARG_INT_GET(a, arg, pri);
        if (pri < _SAT_CMD_GTF_PRI_COMMON) {
            return CMD_FAIL;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Rate", PQ_DFL|PQ_INT, NULL,
                        &arg_rate, NULL);
        parse_table_add(&pt, "maxBurst", PQ_DFL|PQ_INT, NULL,
                        &arg_burst, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        bcm_sat_gtf_bandwidth_t_init(&bw);
        bw.rate= arg_rate;
        bw.max_burst = arg_burst;
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_bandwidth_set(unit, id, pri, &bw));
    } else if (parse_cmp("Get", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_ARG_INT_GET(a, arg, pri);
        if (pri < _SAT_CMD_GTF_PRI_COMMON) {
            return CMD_FAIL;
        }
        bcm_sat_gtf_bandwidth_t_init(&bw);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_bandwidth_get(unit, id, pri, &bw));
        cli_out("\tbandwidth: rate=%d kbps, burst=%d kbit\n",
                bw.rate, bw.max_burst);
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_ratepattern(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_mode = bcmSatGtfRatePatternContinues;
    uint32 arg_high = 10, arg_low = 10;
    uint32 arg_cnt_iter = 100, arg_cnt_burst = 100, arg_cnt_interval = 1000;
    uint32 arg_wt_burst = 1, arg_wt_interval = 1, arg_stop_eq = 0;
    int pri;
    bcm_sat_gtf_t id;
    bcm_sat_gtf_rate_pattern_t pattern;
    parse_table_t pt;
    cmd_result_t retCode;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Set", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_ARG_NUMB_GET(a, arg, pri);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Mode", PQ_DFL|PQ_INT, NULL,
                        &arg_mode, NULL);
        parse_table_add(&pt, "HighThresh", PQ_DFL|PQ_INT, NULL,
                        &arg_high, NULL);
        parse_table_add(&pt, "LowThresh", PQ_DFL|PQ_INT, NULL,
                        &arg_low, NULL);
        parse_table_add(&pt, "stopITerCnt", PQ_DFL|PQ_INT, NULL,
                        &arg_cnt_iter, NULL);
        parse_table_add(&pt, "stopBurstCnt", PQ_DFL|PQ_INT, NULL,
                        &arg_cnt_burst, NULL);
        parse_table_add(&pt, "stopIntervalCnt", PQ_DFL|PQ_INT, NULL,
                        &arg_cnt_interval, NULL);
        parse_table_add(&pt, "BurstpktWeight", PQ_DFL|PQ_INT, NULL,
                        &arg_wt_burst, NULL);
        parse_table_add(&pt, "IntervalpktWeight", PQ_DFL|PQ_INT, NULL,
                        &arg_wt_interval, NULL);
        parse_table_add(&pt, "StopintervalEqburst", PQ_DFL|PQ_BOOL, NULL,
                        &arg_stop_eq, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        bcm_sat_gtf_rate_pattern_t_init(&pattern);
        pattern.rate_pattern_mode = arg_mode;
        pattern.high_threshold = arg_high;
        pattern.low_threshold = arg_low;
        pattern.stop_iter_count = arg_cnt_iter;
        pattern.stop_burst_count = arg_cnt_burst;
        pattern.stop_interval_count = arg_cnt_interval;
        pattern.burst_packet_weight = arg_wt_burst;
        pattern.interval_packet_weight = arg_wt_interval;
        if (arg_stop_eq) {
            pattern.flags |= BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
        }
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_rate_pattern_set(unit, id, pri,
                                                         &pattern));
    } else if (parse_cmp("Get", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_ARG_NUMB_GET(a, arg, pri);
        
        bcm_sat_gtf_rate_pattern_t_init(&pattern);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_rate_pattern_get(unit, id, pri,
                                                         &pattern));
        cli_out("\tpattern  : mode=%d, flag=0x%08x\n",
                pattern.rate_pattern_mode, pattern.flags);
        cli_out("\t           thresh_low=%d, thresh_high=%d \n",
                pattern.low_threshold, pattern.high_threshold);
        cli_out("\t           cnt_iter=%d, cnt_burst=%d, cnt_interval=%d\n",
                pattern.stop_interval_count, pattern.stop_burst_count,
                pattern.stop_iter_count);
        cli_out("\t           weight_burst=%d, weight_interval=%d\n",
                pattern.burst_packet_weight, pattern.interval_packet_weight);
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_gtf_packet(int unit, args_t *a)
{
    char *arg = NULL;
    int index, cnt, pos;
    int arg_pattern_len = BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE * 6;
    int32 pattern[BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE];
    uint32 arg_ht, arg_pt;
    char *arg_pattern;
    uint32 arg_cir_num, arg_cir_tlv, arg_cir_crc;
    uint32 arg_eir_num, arg_eir_tlv, arg_eir_crc;
    uint32 arg_context, arg_offset;
    bcm_sat_gtf_t id;
    parse_table_t pt;
    bcm_sat_gtf_packet_config_t *config;

    SAT_IF_ERROR_RETURN(sat_gtf_packet_default_init(unit));
    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Format", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id,
                                &_gtf_pkt_cfg_default.cfg));
        SAT_IF_ERROR_RETURN(sat_gtf_packet_default_load(unit, id));
    } else if (parse_cmp("Set", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_GTF_ID_CHECK_RETURN(id);

        config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t),
                           "bcm_sat_gtf_packet_config");
        if (!config) {
            return CMD_FAIL;
        }
        sal_memcpy(config, &_gtf_pkt_cfg[id].cfg,
                   sizeof(bcm_sat_gtf_packet_config_t));
        arg_ht = config->sat_header_type;
        arg_pt = config->payload.payload_type;
        if (!(arg_pattern = sal_alloc(arg_pattern_len, "sat_arg"))) {
            sal_free(config);
            return CMD_FAIL;
        }
        sal_memset(arg_pattern, 0, arg_pattern_len);
        pos = 0;
        for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
            cnt = sal_sprintf(arg_pattern + pos, "0x%02x,",
                          config->payload.payload_pattern[index]);
            pos += cnt;
        }
        arg_cir_num = config->packet_edit[bcmSatGtfPriCir].number_of_ctfs;
        arg_cir_tlv = (config->packet_edit[bcmSatGtfPriCir].flags &
                       BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV);
        arg_cir_crc = (config->packet_edit[bcmSatGtfPriCir].flags &
                       BCM_SAT_GTF_PACKET_EDIT_ADD_CRC);
        arg_eir_num = config->packet_edit[bcmSatGtfPriEir].number_of_ctfs;
        arg_eir_tlv = (config->packet_edit[bcmSatGtfPriEir].flags &
                       BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV);
        arg_eir_crc = (config->packet_edit[bcmSatGtfPriEir].flags &
                       BCM_SAT_GTF_PACKET_EDIT_ADD_CRC);
        arg_context = config->packet_context_id;
        arg_offset = config->offsets.seq_number_offset;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "HeaderType", PQ_DFL|PQ_INT, NULL,
                        &arg_ht, NULL);
        parse_table_add(&pt, "PayloadType", PQ_DFL|PQ_INT, NULL,
                        &arg_pt, NULL);
        parse_table_add(&pt, "PayloadPattern", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                        &arg_pattern, NULL);
        parse_table_add(&pt, "CirNumofctfs", PQ_DFL|PQ_INT, NULL,
                        &arg_cir_num, NULL);
        parse_table_add(&pt, "CiraddendTlv", PQ_DFL|PQ_BOOL, NULL,
                        &arg_cir_tlv, NULL);
        parse_table_add(&pt, "CiraddCrc", PQ_DFL|PQ_BOOL, NULL,
                        &arg_cir_crc, NULL);
        parse_table_add(&pt, "EirNumofctfs", PQ_DFL|PQ_INT, NULL,
                        &arg_eir_num, NULL);
        parse_table_add(&pt, "EiraddendTlv", PQ_DFL|PQ_BOOL, NULL,
                        &arg_eir_tlv, NULL);
        parse_table_add(&pt, "EiraddCrc", PQ_DFL|PQ_BOOL, NULL,
                        &arg_eir_crc, NULL);
#if defined(BCM_PETRA_SUPPORT)
        parse_table_add(&pt, "pktContext", PQ_DFL|PQ_INT, NULL,
                        &arg_context, NULL);
#endif
        parse_table_add(&pt, "Offsetofseqnum", PQ_DFL|PQ_INT, NULL,
                        &arg_offset, NULL);
        if (!ARG_CNT(a)) {  /* Display settings */
            cli_out("Current settings:\n");
            parse_eq_format(&pt);
            parse_arg_eq_done(&pt);
            sal_free(config);
            return CMD_OK;
        }
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            sal_free(config);
            return CMD_FAIL;
        }
        sal_memset(pattern, 0, sizeof(pattern));
        (void)_list_to_data_array(arg_pattern, BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE,
                                  pattern);
        parse_arg_eq_done(&pt);/* Do not do this before arg_pattern is parsed. */
        
        config->sat_header_type = arg_ht;
        config->payload.payload_type = arg_pt;
        for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
            config->payload.payload_pattern[index] = (uint8)pattern[index];
        }
        config->packet_edit[bcmSatGtfPriCir].number_of_ctfs = arg_cir_num;
        if (arg_cir_tlv) {
            config->packet_edit[bcmSatGtfPriCir].flags |=
                BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        } else {
            config->packet_edit[bcmSatGtfPriCir].flags &=
                ~BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        }
        if (arg_cir_crc) {
            config->packet_edit[bcmSatGtfPriCir].flags |=
                BCM_SAT_GTF_PACKET_EDIT_ADD_CRC;
        } else {
            config->packet_edit[bcmSatGtfPriCir].flags &=
                ~BCM_SAT_GTF_PACKET_EDIT_ADD_CRC;
        }
        config->packet_edit[bcmSatGtfPriEir].number_of_ctfs = arg_eir_num;
        if (arg_eir_tlv) {
            config->packet_edit[bcmSatGtfPriEir].flags |=
                BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        } else {
            config->packet_edit[bcmSatGtfPriEir].flags &=
                ~BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        }
        if (arg_eir_crc) {
            config->packet_edit[bcmSatGtfPriEir].flags |=
                BCM_SAT_GTF_PACKET_EDIT_ADD_CRC;
        } else {
            config->packet_edit[bcmSatGtfPriEir].flags &=
                ~BCM_SAT_GTF_PACKET_EDIT_ADD_CRC;
        }
        config->packet_context_id = arg_context;
        config->offsets.seq_number_offset = arg_offset;
        SAT_IF_ERROR_RETURN(bcm_sat_gtf_packet_config(unit, id, config));
        sal_memcpy(&_gtf_pkt_cfg[id].cfg, config,
                   sizeof(bcm_sat_gtf_packet_config_t));
        sal_free(config);
    }
#if defined(BCM_ESW_SUPPORT)
    else if (parse_cmp("OlpHeader", arg, 0)) {
        return sat_gtf_olpheader(unit, a);
    }
#endif
#if defined(BCM_PETRA_SUPPORT)
    else if (parse_cmp("DnxHeader", arg, 0)) {
        return sat_gtf_dnxheader(unit, a);
    }
#endif
    else if (parse_cmp("Packetdata", arg, 0)) {
        return sat_gtf_packet_data(unit, a);
    } else if (parse_cmp("Length", arg, 0)) {
        return sat_gtf_packet_length(unit, a);
    } else if (parse_cmp("STamp", arg, 0)) {
        return sat_gtf_packet_stamp(unit, a);
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_create(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 flag = 0;
    bcm_sat_ctf_t id;

    arg = ARG_GET(a);
    if (arg) {
        id = _parse_integer(arg);
        SAT_CTF_ID_CHECK_RETURN(id);
        flag |= BCM_SAT_CTF_WITH_ID;
    }
    SAT_IF_ERROR_RETURN(bcm_sat_ctf_create(unit, flag, &id));
    if (!(flag & BCM_SAT_CTF_WITH_ID)) {
        cli_out("CTF ID: %d\n", id);
    }
    SAT_IF_ERROR_RETURN(sat_ctf_packet_default_load(unit, id));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_destroy(int unit, args_t *a)
{
    char *arg = NULL;
    bcm_sat_ctf_t id;

    arg = ARG_GET(a);
    if (arg) {
        id = _parse_integer(arg);
        SAT_CTF_ID_CHECK_RETURN(id);
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_destroy(unit, id));
    } else {
        cli_out("Destroy all CTF...\n");
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_destroy_all(unit));
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_packet(int unit, args_t *a)
{
    char *arg = NULL;
    int index, cnt, pos = 0;
    int arg_pattern_len = BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE*6;
    int32 pattern[BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE];
    char *arg_pattern;
    uint32 arg_ht, arg_pt, arg_p_off, arg_seq_off, arg_ts_off;
    uint32 arg_tlv, arg_crc;
    parse_table_t pt;
    bcm_sat_ctf_t id;
    bcm_sat_ctf_packet_info_t config;

    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_CTF_ID_CHECK_RETURN(id);
    sal_memcpy(&config, &_ctf_pkt_cfg[id], sizeof(config));
    arg_ht = config.sat_header_type;
    arg_pt = config.payload.payload_type;
    if (!(arg_pattern = sal_alloc(arg_pattern_len, "sat_pattern"))) {
        return CMD_FAIL;
    }
    sal_memset(arg_pattern, 0, arg_pattern_len);
    for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
        cnt = sal_sprintf(arg_pattern + pos, "0x%02x,",
                      config.payload.payload_pattern[index]);
        pos += cnt;
    }
    arg_p_off = config.offsets.payload_offset;
    arg_seq_off = config.offsets.seq_number_offset;
    arg_ts_off = config.offsets.timestamp_offset;
    arg_tlv = (config.flags & BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV);
    arg_crc = (config.flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "HeaderType", PQ_DFL|PQ_INT, NULL,
                    &arg_ht, NULL);
    parse_table_add(&pt, "PayloadType", PQ_DFL|PQ_INT, NULL,
                    &arg_pt, NULL);
    parse_table_add(&pt, "PayloadPattern", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                    &arg_pattern, NULL);
    parse_table_add(&pt, "PayloadOffset", PQ_DFL|PQ_INT, NULL,
                    &arg_p_off, NULL);
    parse_table_add(&pt, "SeqnumberOffset", PQ_DFL|PQ_INT, NULL,
                    &arg_seq_off, NULL);
    parse_table_add(&pt, "TimestampOffset", PQ_DFL|PQ_INT, NULL,
                    &arg_ts_off, NULL);
    parse_table_add(&pt, "addendTlv", PQ_DFL|PQ_BOOL, NULL,
                    &arg_tlv, NULL);
    parse_table_add(&pt, "addCrc", PQ_DFL|PQ_BOOL, NULL,
                    &arg_crc, NULL);
    if (!ARG_CNT(a)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    sal_memset(pattern, 0, sizeof(pattern));
    (void)_list_to_data_array(arg_pattern, BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE,
                              pattern);
    parse_arg_eq_done(&pt);/* Do not do this before arg_pattern is parsed. */
    
    config.sat_header_type = arg_ht;
    config.payload.payload_type = arg_pt;
    for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
        config.payload.payload_pattern[index] = (uint8)pattern[index];
    }
    config.offsets.payload_offset = arg_p_off;
    config.offsets.seq_number_offset = arg_seq_off;
    config.offsets.timestamp_offset = arg_ts_off;
    if (arg_tlv) {
        config.flags |= BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
    } else {
        config.flags &= ~BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
    }
    if (arg_crc) {
        config.flags |= BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
    } else {
        config.flags &= ~BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
    }
    SAT_IF_ERROR_RETURN(bcm_sat_ctf_packet_config(unit, id, &config));
    sal_memcpy(&_ctf_pkt_cfg[id], &config, sizeof(config));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_identifer(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_tc = 0, arg_dp = 0, arg_trap_id = 0, arg_session_id = 0;
    parse_table_t pt;
    bcm_sat_ctf_t id;
    bcm_sat_ctf_identifier_t config;
    cmd_result_t retCode;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Add", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_CTF_ID_CHECK_RETURN(id);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "TC", PQ_DFL|PQ_INT, NULL,
                        &arg_tc, NULL);
        parse_table_add(&pt, "Color", PQ_DFL|PQ_INT, NULL,
                        &arg_dp, NULL);
        parse_table_add(&pt, "TrapId", PQ_DFL|PQ_INT, NULL,
                        &arg_trap_id, NULL);
        parse_table_add(&pt, "SessionId", PQ_DFL|PQ_INT, NULL,
                        &arg_session_id, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        bcm_sat_ctf_identifier_t_init(&config);
        config.tc = arg_tc;
        config.color = arg_dp;
        config.trap_id = arg_trap_id;
        config.session_id = arg_session_id;
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_identifier_map(unit, &config, id));
    } else if (parse_cmp("Remove", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "TC", PQ_DFL|PQ_INT, NULL,
                        &arg_tc, NULL);
        parse_table_add(&pt, "Color", PQ_DFL|PQ_INT, NULL,
                        &arg_dp, NULL);
        parse_table_add(&pt, "TrapId", PQ_DFL|PQ_INT, NULL,
                        &arg_trap_id, NULL);
        parse_table_add(&pt, "SessionId", PQ_DFL|PQ_INT, NULL,
                        &arg_session_id, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        bcm_sat_ctf_identifier_t_init(&config);
        config.tc = arg_tc;
        config.color = arg_dp;
        config.trap_id = arg_trap_id;
        config.session_id = arg_session_id;
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_identifier_unmap(unit, &config));
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_trap(int unit, args_t *a)
{
    char *arg = NULL;
#if defined(BCM_PETRA_SUPPORT)
    uint32 id;
#endif

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    }
#if defined(BCM_PETRA_SUPPORT)
    else if (parse_cmp("Add", arg, 0)) {
        /* coverity[unsigned_compare] */
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_trap_add(unit, id));
    } else if (parse_cmp("Remove", arg, 0)) {
        arg = ARG_GET(a);
        if (arg) {
            id = _parse_integer(arg);
            /* coverity[unsigned_compare] */
            SAT_CTF_ID_CHECK_RETURN(id);
            SAT_IF_ERROR_RETURN(bcm_sat_ctf_trap_remove(unit, id));
        } else {
            cli_out("Remove all trap...\n");
            SAT_IF_ERROR_RETURN(bcm_sat_ctf_trap_remove_all(unit));
        }
    }
#endif
    else if (parse_cmp("Data2Session", arg, 0)) {
        return sat_ctf_trap_data2session(unit, a);
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_binlimit(int unit, args_t *a)
{
    char *arg = NULL;
    int max_cnt = 9;
    int cnt, index, pos = 0, arg_bin_len = max_cnt*6;
    char *arg_bins = NULL;
    int32 bins[9];
    parse_table_t pt;
    bcm_sat_ctf_bin_limit_t config[9];

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("SHOW", arg, 0)) {
        for (index = 0; index < max_cnt; index++) {
            bcm_sat_ctf_bin_limit_t_init(&config[index]);
        }
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_bin_limit_get(unit, max_cnt,
                                                      config, &cnt));
        cli_out("Hardware bins:\n\t");
        for (index = 0; index < cnt; index++) {
            cli_out("%d, ", config[index].bin_limit);
        }
        cli_out("\n");
    } else if (parse_cmp("Set", arg, 0)) {
        if (!(arg_bins = sal_alloc(arg_bin_len, "sat_bin"))) {
            return CMD_FAIL;
        }
        sal_memset(arg_bins, 0, arg_bin_len);
        for (index = 0; index < max_cnt; index++) {
            cnt = sal_sprintf(arg_bins + pos, "%d,", (index + 1) * 100);
            pos += cnt;
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Bins", PQ_DFL|PQ_MALLOC|PQ_STRING, NULL,
                        &arg_bins, NULL);
        if (!ARG_CNT(a)) {  /* Display settings */
            cli_out("Current settings:\n");
            parse_eq_format(&pt);
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        sal_memset(bins, 0, sizeof(bins));
        cnt = _list_to_data_array(arg_bins, max_cnt, bins);
        parse_arg_eq_done(&pt);/* Do not do this before arg_bins is parsed. */
        for (index = 0; index < max_cnt; index++) {
            bcm_sat_ctf_bin_limit_t_init(&config[index]);
        }
        for (index = 0; index < cnt; index++) {
            config[index].bin_select = index;
            config[index].bin_limit = bins[index];
        }
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_bin_limit_set(unit, cnt, config));
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_avail(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_cnt = 10, arg_thresh = 100;
    parse_table_t pt;
    bcm_sat_ctf_t id;
    cmd_result_t retCode;
    bcm_sat_ctf_availability_config_t config;

    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_CTF_ID_CHECK_RETURN(id);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Noofslot", PQ_DFL|PQ_INT, NULL,
                    &arg_cnt, NULL);
    parse_table_add(&pt, "Thresholdperslot", PQ_DFL|PQ_INT, NULL,
                    &arg_thresh, NULL);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }
    bcm_sat_ctf_availability_config_t_init(&config);
    config.switch_state_num_of_slots = arg_cnt;
    config.switch_state_threshold_per_slot = arg_thresh;
    SAT_IF_ERROR_RETURN(bcm_sat_ctf_availability_config_set(unit, id, &config));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_report(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_ratio = 1, arg_seq = 0, arg_delay = 0;
    parse_table_t pt;
    bcm_sat_ctf_t id;
    cmd_result_t retCode;
    bcm_sat_ctf_report_config_t config;

    SAT_ARG_NUMB_GET(a, arg, id);
    SAT_CTF_ID_CHECK_RETURN(id);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "samplePercent", PQ_DFL|PQ_INT, NULL,
                    &arg_ratio, NULL);
    parse_table_add(&pt, "addSeqnum", PQ_DFL|PQ_BOOL, NULL,
                    &arg_seq, NULL);
    parse_table_add(&pt, "addDelay", PQ_DFL|PQ_BOOL, NULL,
                    &arg_delay, NULL);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }
    bcm_sat_ctf_report_config_t_init(&config);
    config.report_sampling_percent = arg_ratio;
    if (arg_seq) {
        config.flags |= BCM_SAT_CTF_REPORT_ADD_SEQ_NUM;
    } else {
        config.flags &= ~BCM_SAT_CTF_REPORT_ADD_SEQ_NUM;
    }
    if (arg_delay) {
        config.flags |= BCM_SAT_CTF_REPORT_ADD_DELAY;
    } else {
        config.flags &= ~BCM_SAT_CTF_REPORT_ADD_DELAY;
    }
    SAT_IF_ERROR_RETURN(bcm_sat_ctf_reports_config_set(unit, id, &config));

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_ctf_stat(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_global = 0, arg_delay = 50, arg_step = 10, arg_update = 0;
    uint32 arg_clear = 0, flags = 0, traverse = 0;
    parse_table_t pt;
    bcm_sat_ctf_t id = 0;
    cmd_result_t retCode;
    bcm_sat_ctf_stat_config_t config;
    bcm_sat_ctf_stat_t stat;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Set", arg, 0)) {
        SAT_ARG_NUMB_GET(a, arg, id);
        SAT_CTF_ID_CHECK_RETURN(id);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "useGlobalbin", PQ_DFL|PQ_BOOL, NULL,
                        &arg_global, NULL);
        parse_table_add(&pt, "binMinDelay", PQ_DFL|PQ_INT, NULL,
                        &arg_delay, NULL);
        parse_table_add(&pt, "binStep", PQ_DFL|PQ_INT, NULL,
                        &arg_step, NULL);
        parse_table_add(&pt, "updateCntInUnavail", PQ_DFL|PQ_BOOL, NULL,
                        &arg_update, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        bcm_sat_ctf_stat_config_t_init(&config);
        config.use_global_bin_config = arg_global;
        config.bin_min_delay = arg_delay;
        config.bin_step = arg_step;
        config.update_counters_in_unvavail_state = arg_update;
        SAT_IF_ERROR_RETURN(bcm_sat_ctf_stat_config_set(unit, id, &config));
    } else if (parse_cmp("SHOW", arg, 0)) {
        arg = ARG_GET(a);
        if (arg == NULL) {  /* w/o ID, w/o CLR */
            traverse = 1;
        } else {
            if (sal_strchr(arg, '=')) {  /* w/o ID, with CLR */
                traverse = 1;
                ARG_PREV(a);
            } else {    /* with ID */
                id = _parse_integer(arg);
                SAT_CTF_ID_CHECK_RETURN(id);
                traverse = 0;
            }
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Clronread", PQ_DFL|PQ_BOOL, NULL,
                            &arg_clear, NULL);
            if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
                cli_out("%s: Error: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            parse_arg_eq_done(&pt);
        }
        bcm_sat_ctf_stat_t_init(&stat);
        if (!arg_clear) {
            flags |= BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ;
        } else {
            flags &= ~BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ;
        }
        if (traverse) {
            return bcm_sat_ctf_traverse(unit, sat_ctf_show, (void *)&flags);
        } else {
            return sat_ctf_show(unit, id, (void *)&flags);
        }
    } else {
        return CMD_NFND;
    }

    return CMD_OK;
}

#if defined(BCM_ESW_SUPPORT)
#define SAT_ENDPOINT_ID_CHECK_RETURN(id) \
    do { \
        if (id < 0) { \
            cli_out("Invalid ENDPOINT ID\n"); \
            return CMD_FAIL; \
        } \
    } while (0)

STATIC void
sat_endpoint_info_t_init(bcm_sat_endpoint_info_t *info)
{
    bcm_mac_t dmac_def = {0, 0, 0, 0, 0, 0x1};
    bcm_mac_t smac_def = {0, 0, 0, 0, 0, 0x2};
    uint32 src_port_def = 1, dst_port_def = 5;
    bcm_gport_t src_gport_def, dst_gport_def;
    
    if (!info) {
        return;
    }
    BCM_GPORT_LOCAL_SET(src_gport_def, src_port_def);
    BCM_GPORT_LOCAL_SET(dst_gport_def, dst_port_def);
    sal_memset(info, 0, sizeof(bcm_sat_endpoint_info_t));
    info->flags |= BCM_SAT_ENDPOINT_DOWNMEP;
    sal_memcpy(info->dst_mac_address, dmac_def, sizeof(dmac_def));
    sal_memcpy(info->src_mac_address, smac_def, sizeof(smac_def));
    info->outer_vlan = 1;
    info->inner_vlan = 1;
    info->src_gport = src_gport_def;
    info->ether_type = 0x8902;
    info->action_flags |= BCM_SAT_ACTION_FWD_ACTION_LLF;
    info->dest_gport = dst_gport_def;
    info->timestamp_format = bcmSATTimestampFormatIEEE1588v1;
    info->session_id = 1;
}

STATIC cmd_result_t
sat_endpoint_show(int unit, bcm_sat_endpoint_info_t *info, void *cookie)
{
    if (!info) {
        return CMD_FAIL;
    }

    cli_out("ENDPOINT %d:\n", info->ep_id);
    cli_out("\tSource Port     : %d", BCM_GPORT_LOCAL_GET(info->src_gport));
    cli_out("\t\t\tDest Port     : %d\n", BCM_GPORT_LOCAL_GET(info->dest_gport));
    cli_out("\tSource MAC      : %02x:%02x:%02x:%02x:%02x:%02x",
        info->src_mac_address[0], info->src_mac_address[1], 
        info->src_mac_address[2], info->src_mac_address[3], 
        info->src_mac_address[4], info->src_mac_address[5]);
    cli_out("\tDest MAC      : %02x:%02x:%02x:%02x:%02x:%02x\n",
        info->dst_mac_address[0], info->dst_mac_address[1], 
        info->dst_mac_address[2], info->dst_mac_address[3], 
        info->dst_mac_address[4], info->dst_mac_address[5]);
    cli_out("\tOuter VLAN      : %u", info->outer_vlan);
    cli_out("\t\t\tInner VLAN    : %u\n", info->inner_vlan);
    cli_out("\tEthertype       : 0x%04x\n", info->ether_type);
    cli_out("\tPacket Pri      : %u\n", info->pkt_pri);
    cli_out("\tTimestamp Format: %s\n",
        info->timestamp_format? "NTP":"IEEE1588v1");
    cli_out("\tSession ID      : %u\n", info->session_id);
    cli_out("\tFlags           : ");
    cli_out("%s", (info->flags & BCM_SAT_ENDPOINT_UPMEP)? "UPMEP, ":"");
    cli_out("%s", (info->flags & BCM_SAT_ENDPOINT_DOWNMEP)? "DOWNMEP, ":"");
    cli_out("%s", (info->flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN)? \
                  "MATCH_INNER_VLAN, ":"");
    cli_out("%s", (info->flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN)? \
                  "MATCH_OUTER_VLAN, ":"");
    cli_out("\n\tActions         : ");
    cli_out("%s", (info->action_flags & BCM_SAT_ACTION_FWD_ACTION_DROP)? \
                  "DROP, ":"");
    cli_out("%s", (info->action_flags & BCM_SAT_ACTION_FWD_ACTION_REDIRECT)? \
                  "REDIRECT, ":"");
    cli_out("%s", (info->action_flags & BCM_SAT_ACTION_FWD_ACTION_LLF)? \
                  "LLF, ":"");
    cli_out("%s", (info->action_flags & BCM_SAT_ACTION_COPY_TO_CPU)? \
                  "Copy-to-CPU, ":"");
    cli_out("%s", (info->action_flags & BCM_SAT_ACTION_SAMPLE_TIMESTAMP)? \
                  "TIMESTAMP, ":"");
    cli_out("\n");

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_endpoint_show(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 index;
    uint32 flag[] = {BCM_SAT_ENDPOINT_UPMEP, BCM_SAT_ENDPOINT_DOWNMEP};
    bcm_sat_endpoint_t id;
    bcm_sat_endpoint_info_t info;
    int rv;

    arg = ARG_GET(a);
    if (arg) {
        id = _parse_integer(arg);
        SAT_ENDPOINT_ID_CHECK_RETURN(id);
        for (index = 0; index < COUNTOF(flag); index++) {
            sal_memset(&info, 0, sizeof(bcm_sat_endpoint_info_t));
            rv = bcm_sat_endpoint_get(unit, id, flag[index], &info);
            if (rv != BCM_E_NOT_FOUND) {
                (void)sat_endpoint_show(unit, &info, NULL);
            }
        }
    } else {
        for (index = 0; index < COUNTOF(flag); index++) {
            (void)bcm_sat_endpoint_traverse(unit, flag[index], 
                                            sat_endpoint_show, NULL);
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_endpoint_destroy(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_down = 1;
    uint32 flag = BCM_SAT_ENDPOINT_DOWNMEP, destroy_all = 0;
    parse_table_t pt;
    bcm_sat_endpoint_t id = 0;

    arg = ARG_GET(a);
    if (arg == NULL) {  /* All downMEP */
        destroy_all = 1;
    } else {
        if (sal_strchr(arg, '=')) {  /* w/o ID, with option */
            destroy_all = 1;
            ARG_PREV(a);
        } else {    /* with ID */
            destroy_all = 0;
            id = _parse_integer(arg);
            SAT_ENDPOINT_ID_CHECK_RETURN(id);
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "isDowN", PQ_DFL|PQ_BOOL, NULL,
                        &arg_down, NULL);        
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
    }
    flag = (arg_down)? BCM_SAT_ENDPOINT_DOWNMEP:BCM_SAT_ENDPOINT_UPMEP;

    if (destroy_all) {
        cli_out("Destroy all ENDPOINT (%s)...\n", (arg_down)? "DOWN":"UP");
        SAT_IF_ERROR_RETURN(bcm_sat_endpoint_destroy_all(unit, flag));
    } else {
        cli_out("Destroy ENDPOINT %u (%s)...\n", id, (arg_down)? "DOWN":"UP");
        SAT_IF_ERROR_RETURN(bcm_sat_endpoint_destroy(unit, id, flag));
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_endpoint_create(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_down, arg_match_ov, arg_match_iv;
    uint32 arg_drop, arg_redirect, arg_llf, arg_to_cpu;
    uint32 arg_stamp;
    soc_port_t arg_src_port, arg_dst_port;
    parse_table_t pt;
    bcm_sat_endpoint_info_t config;
    cmd_result_t retCode;

    sat_endpoint_info_t_init(&config);
    arg_down = config.flags & BCM_SAT_ENDPOINT_DOWNMEP;
    arg_match_ov = config.flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;
    arg_match_iv = config.flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN;
    arg_drop = config.action_flags & BCM_SAT_ACTION_FWD_ACTION_DROP;
    arg_redirect = config.action_flags & BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
    arg_llf = config.action_flags & BCM_SAT_ACTION_FWD_ACTION_LLF;
    arg_to_cpu = config.action_flags & BCM_SAT_ACTION_COPY_TO_CPU;
    arg_stamp = config.action_flags & BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
    arg_src_port = BCM_GPORT_LOCAL_GET(config.src_gport);
    arg_dst_port = BCM_GPORT_LOCAL_GET(config.dest_gport);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "isDowN", PQ_DFL|PQ_BOOL, NULL,
                    &arg_down, NULL);
    parse_table_add(&pt, "MatchOuterVlan", PQ_DFL|PQ_BOOL, NULL,
                    &arg_match_ov, NULL);
    parse_table_add(&pt, "MatchInnerVlan", PQ_DFL|PQ_BOOL, NULL,
                    &arg_match_iv, NULL);
    parse_table_add(&pt, "DMac", PQ_DFL|PQ_MAC, NULL,
                    &config.dst_mac_address, NULL);
    parse_table_add(&pt, "SMac", PQ_DFL|PQ_MAC, NULL,
                    &config.src_mac_address, NULL);
    parse_table_add(&pt, "OuterVlan", PQ_DFL|PQ_INT16, NULL,
                    &config.outer_vlan, NULL);
    parse_table_add(&pt, "InnerVlan", PQ_DFL|PQ_INT16, NULL,
                    &config.inner_vlan, NULL);
    parse_table_add(&pt, "SrcPort", PQ_DFL|PQ_PORT, NULL,
                    &arg_src_port, NULL);
    parse_table_add(&pt, "ETHertype", PQ_DFL|PQ_HEX, NULL,
                    &config.ether_type, NULL);
    parse_table_add(&pt, "Drop", PQ_DFL|PQ_BOOL, NULL,
                    &arg_drop, NULL);
    parse_table_add(&pt, "Redirect", PQ_DFL|PQ_BOOL, NULL,
                    &arg_redirect, NULL);
    parse_table_add(&pt, "LLF", PQ_DFL|PQ_BOOL, NULL,
                    &arg_llf, NULL);
    parse_table_add(&pt, "CopytoCpu", PQ_DFL|PQ_BOOL, NULL,
                    &arg_to_cpu, NULL);
    parse_table_add(&pt, "timeStamp", PQ_DFL|PQ_BOOL, NULL,
                    &arg_stamp, NULL);
    parse_table_add(&pt, "pktPri", PQ_DFL|PQ_INT8, NULL,
                    &config.pkt_pri, NULL);
    parse_table_add(&pt, "DestPort", PQ_DFL|PQ_PORT, NULL,
                    &arg_dst_port, NULL);
    parse_table_add(&pt, "timeStampFormat", PQ_DFL|PQ_INT, NULL,
                    &config.timestamp_format, NULL);
    parse_table_add(&pt, "sessionId", PQ_DFL|PQ_INT16, NULL,
                    &config.session_id, NULL);
    arg = ARG_GET(a);
    if (arg == NULL) {  /* w/o ID, w/o option */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    } else {
        if (sal_strchr(arg, '=')) {  /* w/o ID, with option */
            config.flags &= ~BCM_SAT_ENDPOINT_WITH_ID;
            ARG_PREV(a);
        } else {    /* with ID */
            config.ep_id = _parse_integer(arg);
            config.flags |= BCM_SAT_ENDPOINT_WITH_ID;
        }
        
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        SAT_ENDPOINT_ID_CHECK_RETURN(config.ep_id);
    }

    if (arg_down) {
        config.flags &= ~BCM_SAT_ENDPOINT_UPMEP;
        config.flags |= BCM_SAT_ENDPOINT_DOWNMEP;
    } else {
        config.flags |= BCM_SAT_ENDPOINT_UPMEP;
        config.flags &= ~BCM_SAT_ENDPOINT_DOWNMEP;
    }
    if (arg_match_ov) {
        config.flags |= BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;
    } else {
        config.flags &= ~BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;
    }
    if (arg_match_iv) {
        config.flags |= BCM_SAT_ENDPOINT_MATCH_INNER_VLAN;
    } else {
        config.flags &= ~BCM_SAT_ENDPOINT_MATCH_INNER_VLAN;
    }
    if (arg_drop) {
        config.action_flags |= BCM_SAT_ACTION_FWD_ACTION_DROP;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_FWD_ACTION_DROP;
    }
    if (arg_redirect) {
        config.action_flags |= BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
    }
    if (arg_llf) {
        config.action_flags |= BCM_SAT_ACTION_FWD_ACTION_LLF;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_FWD_ACTION_LLF;
    }
    if (arg_to_cpu) {
        config.action_flags |= BCM_SAT_ACTION_COPY_TO_CPU;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_COPY_TO_CPU;
    }
    if (arg_stamp) {
        config.action_flags |= BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
    }
    BCM_GPORT_LOCAL_SET(config.src_gport, arg_src_port);
    BCM_GPORT_LOCAL_SET(config.dest_gport, arg_dst_port);
    SAT_IF_ERROR_RETURN(bcm_sat_endpoint_create(unit, &config));
    if (!(config.flags & BCM_SAT_ENDPOINT_WITH_ID)) {
        cli_out("ENDPOINT ID: %d\n", config.ep_id);
    }

    return CMD_OK;
}

STATIC cmd_result_t
cmd_sat_endpoint_replace(int unit, args_t *a)
{
    char *arg = NULL;
    uint32 arg_down, arg_match_ov, arg_match_iv;
    uint32 arg_drop, arg_redirect, arg_llf, arg_to_cpu;
    uint32 arg_stamp;
    soc_port_t arg_src_port, arg_dst_port;
    parse_table_t pt;
    bcm_sat_endpoint_info_t config;
    cmd_result_t retCode;

    sat_endpoint_info_t_init(&config);
    SAT_ARG_NUMB_GET(a, arg, config.ep_id);
    config.flags |= BCM_SAT_ENDPOINT_REPLACE;
    config.flags |= BCM_SAT_ENDPOINT_WITH_ID;

    arg_down = config.flags & BCM_SAT_ENDPOINT_DOWNMEP;
    arg_match_ov = config.flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;
    arg_match_iv = config.flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN;
    arg_drop = config.action_flags & BCM_SAT_ACTION_FWD_ACTION_DROP;
    arg_redirect = config.action_flags & BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
    arg_llf = config.action_flags & BCM_SAT_ACTION_FWD_ACTION_LLF;
    arg_to_cpu = config.action_flags & BCM_SAT_ACTION_COPY_TO_CPU;
    arg_stamp = config.action_flags & BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
    arg_src_port = BCM_GPORT_LOCAL_GET(config.src_gport);
    arg_dst_port = BCM_GPORT_LOCAL_GET(config.dest_gport);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "isDowN", PQ_DFL|PQ_BOOL, NULL,
                    &arg_down, NULL);
    parse_table_add(&pt, "MatchOuterVlan", PQ_DFL|PQ_BOOL, NULL,
                    &arg_match_ov, NULL);
    parse_table_add(&pt, "MatchInnerVlan", PQ_DFL|PQ_BOOL, NULL,
                    &arg_match_iv, NULL);
    parse_table_add(&pt, "DMac", PQ_DFL|PQ_MAC, NULL,
                    &config.dst_mac_address, NULL);
    parse_table_add(&pt, "SMac", PQ_DFL|PQ_MAC, NULL,
                    &config.src_mac_address, NULL);
    parse_table_add(&pt, "OuterVlan", PQ_DFL|PQ_INT16, NULL,
                    &config.outer_vlan, NULL);
    parse_table_add(&pt, "InnerVlan", PQ_DFL|PQ_INT16, NULL,
                    &config.inner_vlan, NULL);
    parse_table_add(&pt, "SrcPort", PQ_DFL|PQ_PORT, NULL,
                    &arg_src_port, NULL);
    parse_table_add(&pt, "ETHertype", PQ_DFL|PQ_HEX, NULL,
                    &config.ether_type, NULL);
    parse_table_add(&pt, "Drop", PQ_DFL|PQ_BOOL, NULL,
                    &arg_drop, NULL);
    parse_table_add(&pt, "Redirect", PQ_DFL|PQ_BOOL, NULL,
                    &arg_redirect, NULL);
    parse_table_add(&pt, "LLF", PQ_DFL|PQ_BOOL, NULL,
                    &arg_llf, NULL);
    parse_table_add(&pt, "CopytoCpu", PQ_DFL|PQ_BOOL, NULL,
                    &arg_to_cpu, NULL);
    parse_table_add(&pt, "timeStamp", PQ_DFL|PQ_BOOL, NULL,
                    &arg_stamp, NULL);
    parse_table_add(&pt, "pktPri", PQ_DFL|PQ_INT8, NULL,
                    &config.pkt_pri, NULL);
    parse_table_add(&pt, "DestPort", PQ_DFL|PQ_PORT, NULL,
                    &arg_dst_port, NULL);
    parse_table_add(&pt, "timeStampFormat", PQ_DFL|PQ_INT, NULL,
                    &config.timestamp_format, NULL);
    parse_table_add(&pt, "sessionId", PQ_DFL|PQ_INT16, NULL,
                    &config.session_id, NULL);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (arg_down) {
        config.flags &= ~BCM_SAT_ENDPOINT_UPMEP;
        config.flags |= BCM_SAT_ENDPOINT_DOWNMEP;
    } else {
        config.flags |= BCM_SAT_ENDPOINT_UPMEP;
        config.flags &= ~BCM_SAT_ENDPOINT_DOWNMEP;
    }
    if (arg_match_ov) {
        config.flags |= BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;
    } else {
        config.flags &= ~BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN;
    }
    if (arg_match_iv) {
        config.flags |= BCM_SAT_ENDPOINT_MATCH_INNER_VLAN;
    } else {
        config.flags &= ~BCM_SAT_ENDPOINT_MATCH_INNER_VLAN;
    }
    if (arg_drop) {
        config.action_flags |= BCM_SAT_ACTION_FWD_ACTION_DROP;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_FWD_ACTION_DROP;
    }
    if (arg_redirect) {
        config.action_flags |= BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
    }
    if (arg_llf) {
        config.action_flags |= BCM_SAT_ACTION_FWD_ACTION_LLF;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_FWD_ACTION_LLF;
    }
    if (arg_to_cpu) {
        config.action_flags |= BCM_SAT_ACTION_COPY_TO_CPU;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_COPY_TO_CPU;
    }
    if (arg_stamp) {
        config.action_flags |= BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
    } else {
        config.action_flags &= ~BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
    }
    BCM_GPORT_LOCAL_SET(config.src_gport, arg_src_port);
    BCM_GPORT_LOCAL_SET(config.dest_gport, arg_dst_port);
    SAT_IF_ERROR_RETURN(bcm_sat_endpoint_create(unit, &config));

    return CMD_OK;
}
#endif /* BCM_ESW_SUPPORT */

STATIC cmd_result_t
cmd_sat_global(int unit, args_t *a)
{
    cmd_result_t retCode;
    char *arg = NULL;
    parse_table_t pt;
    bcm_sat_timestamp_format_t arg_format = bcmSATTimestampFormatIEEE1588v1;
    int arg_reverse = 0, arg_invert = 0, arg_reset = 0, arg_nxor = 0;
    bcm_sat_config_t conf;

    arg = ARG_GET(a);
    if (!arg) {
        return CMD_USAGE;
    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "timestampFormat", PQ_DFL|PQ_INT, NULL,
                        &arg_format, NULL);
        parse_table_add(&pt, "crcReversedData", PQ_DFL|PQ_BOOL, NULL,
                        &arg_reverse, NULL);
        parse_table_add(&pt, "crcInvert", PQ_DFL|PQ_BOOL, NULL,
                        &arg_invert, NULL);
        parse_table_add(&pt, "crcHighReset", PQ_DFL|PQ_BOOL, NULL,
                        &arg_reset, NULL);
        parse_table_add(&pt, "prbsUseNxor", PQ_DFL|PQ_BOOL, NULL,
                        &arg_nxor, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
        
        bcm_sat_config_t_init(&conf);
        conf.timestamp_format = arg_format;
        conf.config_flags |= (arg_reverse)? BCM_SAT_CONFIG_CRC_REVERSED_DATA:0;
        conf.config_flags |= (arg_invert)? BCM_SAT_CONFIG_CRC_INVERT:0;
        conf.config_flags |= (arg_reset)? BCM_SAT_CONFIG_CRC_HIGH_RESET:0;
        conf.config_flags |= (arg_nxor)? BCM_SAT_CONFIG_PRBS_USE_NXOR:0;
        SAT_IF_ERROR_RETURN(bcm_sat_config_set(unit, &conf));
    } else if (parse_cmp("SHOW", arg, 0)) {
        bcm_sat_config_t_init(&conf);
        SAT_IF_ERROR_RETURN(bcm_sat_config_get(unit, &conf));
        cli_out("Hardware settings:\n\t");
        cli_out("stamp format: %s\n\t",
                (conf.timestamp_format == bcmSATTimestampFormatIEEE1588v1)? \
                "IEEE1588v1":"NTP");
        cli_out("flags       : %s%s%s%s\n",
                (conf.config_flags & BCM_SAT_CONFIG_CRC_REVERSED_DATA)? \
                "CRC_REVERSED, ":"",
                (conf.config_flags & BCM_SAT_CONFIG_CRC_INVERT)? \
                "CRC_INVERT, ":"",
                (conf.config_flags & BCM_SAT_CONFIG_CRC_HIGH_RESET)? \
                "CRC_HIGH_RESET, ":"",
                (conf.config_flags & BCM_SAT_CONFIG_PRBS_USE_NXOR)? \
                "PRBS_USE_NXOR, ":"");
    } else {
        return CMD_NFND;
    }
    
    return CMD_OK;
}

STATIC sat_subcommand_t _sat_gtf_subcommands[] = {
        {"SHOW", cmd_sat_gtf_show, SAT_GTF_SHOW_USAGE},
        {"Create", cmd_sat_gtf_create, SAT_GTF_CREATE_USAGE},
        {"Destroy", cmd_sat_gtf_destroy, SAT_GTF_DESTROY_USAGE},
        {"START", cmd_sat_gtf_start, SAT_GTF_START_USAGE},
        {"STOP", cmd_sat_gtf_stop, SAT_GTF_STOP_USAGE},
        {"STat", cmd_sat_gtf_stat, SAT_GTF_STAT_USAGE},
        {"Bandwidth", cmd_sat_gtf_bandwidth, SAT_GTF_BANDWIDTH_USAGE},
        {"Ratepattern", cmd_sat_gtf_ratepattern, SAT_GTF_RATEPATTERN_USAGE},
        {"Packetconfig", cmd_sat_gtf_packet, SAT_GTF_PACKETCONFIG_USAGE},
        {0},
    };
STATIC sat_subcommand_t _sat_ctf_subcommands[] = {
        {"Create", cmd_sat_ctf_create, SAT_CTF_CREATE_USAGE},
        {"Destroy", cmd_sat_ctf_destroy, SAT_CTF_DESTROY_USAGE},
        {"Packetconfig", cmd_sat_ctf_packet, SAT_CTF_PACKETCONFIG_USAGE},
        {"Identifier", cmd_sat_ctf_identifer, SAT_CTF_IDENTIFER_USAGE},
        {"Trap", cmd_sat_ctf_trap, SAT_CTF_TRAP_USAGE},
        {"Binlimit", cmd_sat_ctf_binlimit, SAT_CTF_BINLIMIT_USAGE},
        {"Availability", cmd_sat_ctf_avail, SAT_CTF_AVAILABILITY_USAGE},
        {"Report", cmd_sat_ctf_report, SAT_CTF_REPORT_USAGE},
        {"Stat", cmd_sat_ctf_stat, SAT_CTF_STAT_USAGE},
        {0},
    };

STATIC cmd_result_t
cmd_sat_gtf(int unit, args_t *a)
{
    return sat_subcommand_exec(unit, a, _sat_gtf_subcommands);
}

STATIC cmd_result_t
cmd_sat_ctf(int unit, args_t *a)
{
    return sat_subcommand_exec(unit, a, _sat_ctf_subcommands);
}

#if defined(BCM_ESW_SUPPORT)
STATIC sat_subcommand_t _sat_ep_subcommands[] = {
        {"SHOW", cmd_sat_endpoint_show, SAT_EP_SHOW_USAGE},
        {"Destroy", cmd_sat_endpoint_destroy, SAT_EP_DESTROY_USAGE},
        {"Create", cmd_sat_endpoint_create, SAT_EP_CREATE_USAGE},
        {"Replace", cmd_sat_endpoint_replace, SAT_EP_REPLACE_USAGE},
        {0},
    };

STATIC cmd_result_t
cmd_sat_endpoint(int unit, args_t *a)
{
    return sat_subcommand_exec(unit, a, _sat_ep_subcommands);
}
#endif /* BCM_ESW_SUPPORT */

STATIC sat_subcommand_t _sat_subcommands[] = {
        {"CONFig", cmd_sat_global, SAT_GLOBAL_CONFIGURE_USAGE},
        {"Gtf", cmd_sat_gtf, SAT_GTF_CONFIGURE_USAGE},
        {"Ctf", cmd_sat_ctf, SAT_CTF_CONFIGURE_USAGE},
#if defined(BCM_ESW_SUPPORT)
        {"Endpoint", cmd_sat_endpoint, SAT_ENDPOINT_CONFIGURE_USAGE},
#endif /* BCM_ESW_SUPPORT */
        {0},
    };

cmd_result_t
cmd_sat(int unit, args_t *a)
{
    return sat_subcommand_exec(unit, a, _sat_subcommands);
}

#endif /* BCM_SAT_SUPPORT */
