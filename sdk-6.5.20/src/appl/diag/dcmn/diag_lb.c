/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <soc/macipadr.h>
#include <soc/dpp/cosq.h>
#include <bcm/error.h>
#include <bcm/lb.h>
#include <bcm/types.h>
#include <bcm_int/dpp/lb.h>
#include <soc/dpp/TMC/tmc_api_link_bonding.h>
 
#ifdef BCM_LB_SUPPORT

#define LB_CONFIG_SHOW_STATUS_USAGE    \
    "LB Config Show\n\t"               \
    "direction=<both/rx/tx>\n\t"       \
    "obj=<all/global/lbg/modem>\n\t"    \
    "lbg=<lbg_id>\n\t"                 \
    "modem=<modem_id>\n"

#define LB_STATUS_SHOW_STATUS_USAGE    \
    "LB Status Show\n\t"               \
    "direction=<both/rx/tx>\n\t"       \
    "obj=<all/lbg/modem>\n\t"          \
    "lbg=<lbg_id>\n\t"                 \
    "modem=<modem_id>\n"

#define LB_COUNTER_SHOW_STATUS_USAGE    \
    "LB Counter Show\n\t"               \
    "direction=<both/rx/tx>\n\t"        \
    "obj=<all/global/lbg/modem>\n\t"    \
    "lbg=<lbg_id>\n\t"                  \
    "modem=<modem_id>\n\t"              \
    "nz - filter zero counters\n"

#ifndef COMPILER_STRING_CONST_LIMIT
#define LB_CONFIG_SHOW_USAGE \
    LB_CONFIG_SHOW_STATUS_USAGE "\n"

#define LB_STATUS_SHOW_USAGE \
    LB_STATUS_SHOW_STATUS_USAGE "\n"

#define LB_COUNTER_SHOW_USAGE \
    LB_COUNTER_SHOW_STATUS_USAGE "\n"

#else
#define LB_CONFIG_SHOW_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define LB_STATUS_SHOW_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define LB_COUNTER_SHOW_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#endif

#ifndef COMPILER_STRING_CONST_LIMIT
#define LB_CONFIG_USAGE \
    LB_CONFIG_SHOW_USAGE "\n\t"  \
    "       - Set/display LB Configuration"

#define LB_STATUS_USAGE \
    LB_STATUS_SHOW_USAGE "\n\t"  \
    "       - Display LB Status"

#define LB_COUNTER_USAGE \
    LB_COUNTER_SHOW_USAGE "\n\t"  \
    "       - Display LB Counter"

#else
#define LB_CONFIG_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define LB_STATUS_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#define LB_COUNTER_USAGE \
    "Full documentation cannot be displayed with -pedantic compiler\n\t"

#endif


char cmd_dpp_lb_usage[] =
    "Usages:\n"
    LB_CONFIG_USAGE  "\n\n"
    LB_STATUS_USAGE  "\n\n"
    LB_COUNTER_USAGE "\n\n"
    ;

#define PARSE_LB_DIRECTION     0
#define PARSE_LB_OBJ           1

#define LB_DIRECTION_STRs      {"rx", "tx", "both"}
#define LB_OBJ_STRs            {"all", "global", "lbg", "modem"}

STATIC cmd_result_t
  _lb_parse_str(int type, char *str_in, int *value)
{
    char **_strs = NULL;

    char *direction_strs[] = LB_DIRECTION_STRs;
    char *obj_strs[] = LB_OBJ_STRs;

    int i = 0, len = 0, found = 0;
    
    if ((NULL == str_in) || (NULL == value)) {
        return CMD_FAIL;
    }

    switch (type) {
    case PARSE_LB_DIRECTION:
        _strs = direction_strs;
        len = sizeof(direction_strs)/sizeof(char *);
        break;
    case PARSE_LB_OBJ:
        _strs = obj_strs;
        len = sizeof(obj_strs)/sizeof(char *);
        break;
    default:
        break;
    }

    for (i = 0; i < len; i++) {
         if (!sal_strncasecmp(str_in, _strs[i], strlen(str_in))) {
             (*value) = i;
             found = 1;
             break;
         }
    }

    if (found != 1) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

#define BLOCK_LINE_STR \
"+---------------------------------------------------------------------------------------------------------------------------+"
#define SPLIT_LINE_STR \
"|---------------------------------------------------------------------------------------------------------------------------|"
#define DECOLLATOR_STR "|"

#define DPP_LB_FORMAT_DEC      0
#define DPP_LB_FORMAT_HEX      1
#define DPP_LB_FORMAT_STR      2

#define DPP_LB_NOF_SUB_ITEM_MAX    15
#define DPP_LB_SUB_ITEM_LEN        30

#define DPP_LB_SUB_ITEM_VAR_INIT(_item_header_str_init)                           \
    char* item_header_str[] = _item_header_str_init;                              \
    char  item_str[DPP_LB_NOF_SUB_ITEM_MAX][DPP_LB_SUB_ITEM_LEN];                 \
    char* item_pstr[DPP_LB_NOF_SUB_ITEM_MAX];                                     \
    char  format_str[10];                                                         \
    int   sub_item_cnt = 0;                                                       \
    int   nof_sub_item = 0;                                                       \
    nof_sub_item = sizeof(item_header_str)/sizeof(char*);

/* DPP_LB_SUB_ITEM_VAR_INIT2 and DPP_LB_SUB_ITEM_VAR_END2 are used as a pair  */
#define DPP_LB_SUB_ITEM_VAR_INIT2(_item_header_str_qax, _item_header_str_qux)        \
    char** item_header_str = NULL;                                                   \
    char  item_str[DPP_LB_NOF_SUB_ITEM_MAX][DPP_LB_SUB_ITEM_LEN];                    \
    char* item_pstr[DPP_LB_NOF_SUB_ITEM_MAX];                                        \
    char  format_str[10];                                                            \
    int   sub_item_cnt = 0;                                                          \
    char* item_header_str_qax[] = _item_header_str_qax;                              \
    char* item_header_str_qux[] = _item_header_str_qux;                              \
    char** p_strs = NULL;                                                            \
    int   nof_sub_item = 0, alloc_size = 0;                                          \
    p_strs = SOC_IS_QUX(unit)? item_header_str_qux : item_header_str_qax;            \
    nof_sub_item = SOC_IS_QUX(unit)? (sizeof(item_header_str_qux)/sizeof(char*)) :   \
        (sizeof(item_header_str_qax)/sizeof(char*));                                 \
    alloc_size = nof_sub_item*sizeof(char*);                                         \
    item_header_str = sal_alloc(alloc_size, "item_header_str");                      \
    sal_memcpy(item_header_str, p_strs, alloc_size);

#define DPP_LB_SUB_ITEM_VAR_END2                                                     \
    sal_free(item_header_str);

#define DPP_LB_SUB_ITEM_DEC(_item) {                                                                                  \
    if ((sub_item_cnt < nof_sub_item) && (sub_item_cnt < DPP_LB_NOF_SUB_ITEM_MAX)) {         \
        sal_snprintf(format_str, sizeof(format_str), "%%-%dd", (int)sal_strlen(item_header_str[sub_item_cnt]));       \
        sal_snprintf(item_str[sub_item_cnt], sizeof(item_str[sub_item_cnt]), format_str, (int)_item);                 \
        item_pstr[sub_item_cnt] = &item_str[sub_item_cnt][0];                                                         \
        sub_item_cnt++;                                                                                               \
    }                                                                                                                 \
}

#define DPP_LB_SUB_ITEM_HEX(_item) {                                                                                   \
    if ((sub_item_cnt < nof_sub_item) && (sub_item_cnt < DPP_LB_NOF_SUB_ITEM_MAX)) {          \
        sal_snprintf(format_str, sizeof(format_str), "0x%%-%dx", ((int)sal_strlen(item_header_str[sub_item_cnt])-2));  \
        sal_snprintf(item_str[sub_item_cnt], sizeof(item_str[sub_item_cnt]), format_str, _item);                       \
        item_pstr[sub_item_cnt] = &item_str[sub_item_cnt][0];                                                          \
        sub_item_cnt++;                                                                                                \
    }                                                                                                                  \
}

#define DPP_LB_SUB_ITEM_STR(_item) {                                                                                  \
    if ((sub_item_cnt < nof_sub_item) && (sub_item_cnt < DPP_LB_NOF_SUB_ITEM_MAX)) {         \
        sal_snprintf(format_str, sizeof(format_str), "%%-%ds", (int)sal_strlen(item_header_str[sub_item_cnt]));       \
        sal_snprintf(item_str[sub_item_cnt], sizeof(item_str[sub_item_cnt]), format_str, _item);                      \
        item_pstr[sub_item_cnt] = &item_str[sub_item_cnt][0];                                                         \
        sub_item_cnt++;                                                                                               \
    }                                                                                                                 \
}

#define DPP_LB_SUB_ITEM_DEC2(_item, _invalid) {   \
    if (_invalid == _item) {                                    \
        DPP_LB_SUB_ITEM_STR("No");                              \
    }                                                           \
    else {                                                      \
        DPP_LB_SUB_ITEM_DEC(_item);                             \
    }                                                           \
}

#define DPP_LB_SUB_ITEM_HEX2(_item, _invalid) {   \
    if (_invalid == _item) {                                    \
        DPP_LB_SUB_ITEM_STR("No");                              \
    }                                                           \
    else {                                                      \
        DPP_LB_SUB_ITEM_HEX(_item);                             \
    }                                                           \
}

#define DPP_LB_SUB_ITEM_STR2(_item, _invalid) {   \
    if (_invalid == _item) {                                    \
        DPP_LB_SUB_ITEM_STR("No");                              \
    }                                                           \
    else {                                                      \
        DPP_LB_SUB_ITEM_STR(_item);                             \
    }                                                           \
}

#define _DPP_LB_CFG_RX_GLOBAL_ITEM_STR_QAX_INIT  {   \
        "BufferSize(M)",                             \
        "SeqNumWidth"                                \
        }
#define _DPP_LB_CFG_RX_GLOBAL_ITEM_STR_QUX_INIT  {   \
        "BufferSize(M)",                             \
        "SeqNumWidth",                               \
        "NofTotShareBuf",                            \
        "BufferMode"                                 \
        }
#define _DPP_LB_CFG_RX_LBG_ITEM_STR_QAX_INIT     {   \
        "Lbg",                                       \
        "Modems",                                    \
        "LogicPort",                                 \
        "Ptc",                                       \
        "Timeout",                                   \
        "MaxoutOfOrder",                             \
        "MaxBuffer"                                  \
        }
#define _DPP_LB_CFG_RX_LBG_ITEM_STR_QUX_INIT     {   \
        "Lbg",                                       \
        "Modems",                                    \
        "LogicPort",                                 \
        "Ptc",                                       \
        "Timeout",                                   \
        "MaxoutOfOrder",                             \
        "MaxBuffer",                                 \
        "NofGuarBuf"                                 \
        }
#define _DPP_LB_CFG_RX_MODEM_ITEM_STR_QAX_INIT   {   \
        "Modem",                                     \
        "Lbg",                                       \
        "Port",                                      \
        "VLAN   ",                                   \
        "FifoStart",                                 \
        "FifoSize"                                   \
        }
#define _DPP_LB_CFG_RX_MODEM_ITEM_STR_QUX_INIT   {   \
        "Modem",                                     \
        "Lbg",                                       \
        "Port",                                      \
        "VLAN   ",                                   \
        "FifoSize"                                   \
        }
#define _DPP_LB_CFG_TX_GLOBAL_ITEM_STR_QAX1_INIT {  \
        "SegmentMode ",                             \
        "VlanTpid",                                 \
        "LbgTpid",                                  \
        "PktCrcEna"                                 \
        }
#define _DPP_LB_CFG_TX_GLOBAL_ITEM_STR_QUX1_INIT {  \
        "VlanTpid",                                 \
        "LbgTpid",                                  \
        }
#define _DPP_LB_CFG_TX_GLOBAL_ITEM_STR_2_INIT {  \
        "TC",                                    \
        "DP",                                    \
        "Pkt-Pri",                               \
        "Pkt-Cfi",                               \
        "Pkt-Dp"                                 \
        }
#define _DPP_LB_CFG_TX_LBG_ITEM_STR_QAX_INIT {   \
        "Lbg",                                   \
        "Enable",                                \
        "Modem ",                                \
        "LogicPort",                             \
        "FifoStart",                             \
        "FifoEnd",                               \
        "SeqNumWidth",                           \
        "LbgSchedule"                            \
        }
#define _DPP_LB_CFG_TX_LBG_ITEM_STR_QUX_INIT {   \
        "Lbg",                                   \
        "Enable",                                \
        "Modem ",                                \
        "LogicPort",                             \
        "FifoStart",                             \
        "FifoEnd",                               \
        "SeqNumWidth",                           \
        "LbgSchedule",                           \
        "SegmentMode",                           \
        "PktCrcEna"                              \
        }
#define _DPP_LB_CFG_TX_MODEM_ITEM_STR_INIT   {   \
        "Modem",                                 \
        "Lbg",                                   \
        "Port",                                  \
        "Chan",                                  \
        "DstMac           ",                     \
        "SrtMac           ",                     \
        "VLAN",                                  \
        "Shaper",                                \
        "Rate   ",                               \
        "Burst",                                 \
        "HdrCom",                                \
        "EgqInf"                                 \
        }
#define _DPP_LB_STATUS_RX_LBG_ITEM_STR_INIT  {   \
        "Lbg",                                   \
        "IsInSync",                              \
        "ExpSeqNum"                              \
        }
#define _DPP_LB_STATUS_RX_MODEM_ITEM_STR_INIT {  \
        "Modem",                                \
        "IsEmptyFifo"                            \
        }
#define _DPP_LB_STATUS_TX_LBG_ITEM_STR_INIT  {   \
        "Lbg",                                   \
        "IsEmptyFifo"                            \
        }

typedef struct _dpp_lb_status_rx_lbg_info_s {

    uint32 is_in_sync;

    uint32 exp_seq_num;
} _dpp_lb_status_rx_lbg_info_t;

typedef struct _dpp_lb_status_rx_modem_info_s {

    uint32 is_empty_fifo;
} _dpp_lb_status_rx_modem_info_t;

typedef struct _dpp_lb_status_tx_lbg_info_s {

    uint32 is_empty_fifo;
} _dpp_lb_status_tx_lbg_info_t;

typedef struct _dpp_lb_status_rx_info_s {

    _dpp_lb_status_rx_lbg_info_t   rx_lbg_status[SOC_TMC_LB_NOF_LBG];

    _dpp_lb_status_rx_modem_info_t rx_modem_status[SOC_TMC_LB_NOF_MODEM];
} _dpp_lb_status_rx_info_t;

typedef struct _dpp_lb_status_tx_info_s {

    _dpp_lb_status_tx_lbg_info_t   tx_lbg_status[SOC_TMC_LB_NOF_LBG];
} _dpp_lb_status_tx_info_t;

typedef struct _dpp_lb_status_info_s {

    _dpp_lb_status_rx_info_t rx_status;

    _dpp_lb_status_tx_info_t tx_status;
} _dpp_lb_status_info_t;

typedef struct _dpp_lb_counter_rx_global_info_s {

    uint64  discard_fragments;
} _dpp_lb_counter_rx_global_info_t;

typedef struct _dpp_lb_counter_tx_global_info_s {

    uint64  error_pkts;
} _dpp_lb_counter_tx_global_info_t;


typedef struct _dpp_lb_counter_rx_lbg_info_s {

    uint64  discard_fragments;
} _dpp_lb_counter_rx_lbg_info_t;

typedef struct _dpp_lb_counter_tx_lbg_info_s {

    uint64  unicast_pkts;

    uint64  multicast_pkts;

    uint64  broadcast_pkts;

    uint64  pkts_64octets;

    uint64  pkts_65to127octets;

    uint64  pkts_128to255octets;

    uint64  Pkts_256to511octets;

    uint64  Pkts_512to1023octets;

    uint64  Pkts_1024to1518octets;

    uint64  Pkts_1519octets;

    uint64  pkts;

    uint64  octets;
} _dpp_lb_counter_tx_lbg_info_t;

typedef struct _dpp_lb_counter_rx_modem_info_s {

    uint64  pkts;

    uint64  octets;
} _dpp_lb_counter_rx_modem_info_t;

typedef struct _dpp_lb_counter_tx_modem_info_s {

    uint64  pkts;

    uint64  octets;
} _dpp_lb_counter_tx_modem_info_t;

typedef struct _dpp_lb_counter_rx_info_s {

    _dpp_lb_counter_rx_global_info_t  rx_global_counter;

    _dpp_lb_counter_rx_lbg_info_t     rx_lbg_counter[SOC_TMC_LB_NOF_LBG];

    _dpp_lb_counter_rx_modem_info_t   rx_modem_counter[SOC_TMC_LB_NOF_MODEM];
} _dpp_lb_counter_rx_info_t;

typedef struct _dpp_lb_counter_tx_info_s {

    _dpp_lb_counter_tx_global_info_t  tx_global_counter;

    _dpp_lb_counter_tx_lbg_info_t     tx_lbg_counter[SOC_TMC_LB_NOF_LBG];

    _dpp_lb_counter_tx_modem_info_t   tx_modem_counter[SOC_TMC_LB_NOF_MODEM];
} _dpp_lb_counter_tx_info_t;

typedef struct _dpp_lb_counter_info_s {

    _dpp_lb_counter_rx_info_t rx_counter;

    _dpp_lb_counter_tx_info_t tx_counter;
} _dpp_lb_counter_info_t;

STATIC cmd_result_t
  _dpp_lb_block_header_print(
    char *hdr_str
  )
{
    char *block_line = BLOCK_LINE_STR;
    char decollator_char = '|';
    char blank_char = ' ';
    char str_print[1024] = {0};
    int line_size = sal_strlen(block_line);
    int str_size = sal_strlen(hdr_str);

    int blank_size = 0;
    int written = 0;
    int i = 0;

    blank_size = (line_size - str_size) - 2;

    written += sal_snprintf(str_print + written, sizeof(str_print), "%c", decollator_char);
    for (i = 0; i < (blank_size/2); i++) {
        written += sal_snprintf(str_print + written, sizeof(str_print), "%c", blank_char);
    }

    written += sal_snprintf(str_print + written, sizeof(str_print), "%s", hdr_str);

    for (i = 0; i < (blank_size/2 + blank_size%2); i++) {
        written += sal_snprintf(str_print + written, sizeof(str_print), "%c", blank_char);
    }

    written += sal_snprintf(str_print + written, sizeof(str_print), "%c", decollator_char);

    cli_out("%s\n", block_line);
    cli_out("%s\n", str_print);

    return CMD_OK;
}

STATIC cmd_result_t
    _dpp_lb_item_print(
    char **item_str,
    int num_of_item
  )
{
    char *split_line = SPLIT_LINE_STR;
    char decollator_char = '|';
    char blank_char = ' ';
    char str_print[1024] = {0};
    int written = 0;
    int i = 0;
    int reminder_size = 0;

    for (i = 0; i < num_of_item; i++) {
        written += sal_snprintf(str_print + written, sizeof(str_print), "%c", decollator_char);
        written += sal_snprintf(str_print + written, sizeof(str_print), " %s ", item_str[i]);
    }

    reminder_size = strlen(split_line) - sal_strlen(str_print); 
    for (i = 0; i < (reminder_size - 1); i++) {
        written += sal_snprintf(str_print + written, sizeof(str_print), "%c", blank_char);
    }
    written +=  sal_snprintf(str_print + written, sizeof(str_print), "%c", decollator_char);

    cli_out("%s\n", str_print);

    return CMD_OK;
}

STATIC cmd_result_t
    _dpp_lb_counter_item_print(
    char   *counter_name,
    uint32  print_none_zero,
    uint64  cnt_value
  )
{
    char *split_line = SPLIT_LINE_STR;
    char str_print[1024] = {0};
    char buf_cnt_val[32] = {0}; 
    int commachr = ','; 
    int written = 0;
    int i = 0;
    int reminder_size = 0;
#define DIAG_LB_COUNTER_NAME_LEN_MAX 22 

    if (print_none_zero && COMPILER_64_IS_ZERO(cnt_value)) {
        return CMD_OK;
    }

    written += sal_snprintf(str_print, sizeof(str_print), "| %s", counter_name);
    reminder_size = DIAG_LB_COUNTER_NAME_LEN_MAX - written; 

    for (i = 0; i < (reminder_size - 1); i++) {
        written += sal_snprintf(str_print + written, sizeof(str_print), " ");
    }
    written += sal_snprintf(str_print + written, sizeof(str_print), ":");

    format_uint64_decimal(buf_cnt_val, cnt_value, commachr);
    written += sal_snprintf(str_print + written, sizeof(str_print), "%22s", buf_cnt_val);

    reminder_size = strlen(split_line) - sal_strlen(str_print); 
    for (i = 0; i < (reminder_size - 1); i++) {
        written += sal_snprintf(str_print + written, sizeof(str_print), " ");
    }
    written +=  sal_snprintf(str_print + written, sizeof(str_print), "|");

    cli_out("%s\n", str_print);

    return CMD_OK;
}

/* 
   Purpose: Display modem configuration in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                      RX Global Config                                      |
   |--------------------------------------------------------------------------------------------|
   | BufferSize(M) | SeqNumWidth                                                                |
   |--------------------------------------------------------------------------------------------|
   | 2             | 14                                                                         |
   +--------------------------------------------------------------------------------------------|
 */
STATIC cmd_result_t
_dpp_lb_config_info_rx_global_print(
    int                         unit,
    soc_lb_cfg_rx_glb_info_t   *rx_glb_info
  )
{
    cmd_result_t cmd_rv = CMD_OK;
    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT2(_DPP_LB_CFG_RX_GLOBAL_ITEM_STR_QAX_INIT, _DPP_LB_CFG_RX_GLOBAL_ITEM_STR_QUX_INIT)

    if (NULL == rx_glb_info) {
        cli_out("Error: %s is NULL\n", "rx_glb_info");
        cmd_rv = CMD_FAIL;
        goto exit;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Global Config");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    sub_item_cnt = 0;
    DPP_LB_SUB_ITEM_DEC(rx_glb_info->buffer_size);  
    DPP_LB_SUB_ITEM_HEX(rx_glb_info->seq_num_width);
    if (SOC_IS_QUX(unit)) {
        DPP_LB_SUB_ITEM_HEX(rx_glb_info->nof_total_shared_buffer);
        DPP_LB_SUB_ITEM_DEC(rx_glb_info->single_buffer_size);
    }

    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_pstr, sub_item_cnt);

    cli_out("%s\n", block_line);

exit:
    DPP_LB_SUB_ITEM_VAR_END2

    return cmd_rv;
}

/* 
   Purpose: Display lbg configuration in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                          RX Lbg Config                                     |
   |--------------------------------------------------------------------------------------------|
   |   Lbg  | Modems | Logic port |  Ptc | Timeout | MaxoutOfOrder | MaxBuffer                  |
   |--------------------------------------------------------------------------------------------|
   |   0    | 0xffff |     20     |      | 16bits  | 13bits        | 12bits                     |
   |   1    | 0xf0f0 |     21     |      | 0xffff  | 0x1fff        | 0xfff                      |
   +--------------------------------------------------------------------------------------------+
 */ 
STATIC cmd_result_t
_dpp_lb_config_info_rx_lbg_print(
    int                         unit,
    soc_lbg_t                   lbg_id,
    soc_lb_cfg_rx_lbg_info_t   *rx_lbg_info
  )
{
    cmd_result_t cmd_rv = CMD_OK;
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    soc_lb_cfg_rx_lbg_info_t   *rx_lbg_info_tmp = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT2(_DPP_LB_CFG_RX_LBG_ITEM_STR_QAX_INIT, _DPP_LB_CFG_RX_LBG_ITEM_STR_QUX_INIT)

    if (NULL == rx_lbg_info) {
        cli_out("Error: %s is NULL\n", "rx_lbg_info");
        cmd_rv = CMD_FAIL;
        goto exit;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Lbg Config");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (lbg_id == -1) {
        lbg_start = 0;
        lbg_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        lbg_start = lbg_end = lbg_id;
    }

    for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
        rx_lbg_info_tmp = rx_lbg_info + lbg_tmp;

        sub_item_cnt = 0;
        DPP_LB_SUB_ITEM_DEC(lbg_tmp);    
        DPP_LB_SUB_ITEM_HEX(rx_lbg_info_tmp->modem_bmp);
        DPP_LB_SUB_ITEM_DEC2(rx_lbg_info_tmp->logic_port, BCM_LB_PORT_INVALID);
        DPP_LB_SUB_ITEM_DEC(rx_lbg_info_tmp->ptc);
        DPP_LB_SUB_ITEM_HEX(rx_lbg_info_tmp->timeout);
        DPP_LB_SUB_ITEM_HEX(rx_lbg_info_tmp->max_out_of_order);
        DPP_LB_SUB_ITEM_HEX(rx_lbg_info_tmp->max_buffer);
        if (SOC_IS_QUX(unit)) {
            DPP_LB_SUB_ITEM_HEX(rx_lbg_info_tmp->nof_guaranteed_buffer);
        }

        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

exit:
    DPP_LB_SUB_ITEM_VAR_END2

    return cmd_rv;
}

/* 
   Purpose: Display modem configuration in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                        RX Modem Config                                     |
   |--------------------------------------------------------------------------------------------|
   | Modem  | Port | VLAN | Lbg | FifoStart | FifoEnd | Timeout | MaxoutOfOrder | MaxBuffer     |
   |--------------------------------------------------------------------------------------------|
   |   2    | N    | N    | N   |           | 14bits  | 16bits  | 13bits        | 12bits        |
   |   3    | 128  | 4096 | N   | 14bits    | 0x3fff  | 0xffff  | 0x1fff        | 0xfff         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
  _dpp_lb_config_info_rx_modem_print(
    int                         unit,
    soc_modem_t                 modem_id,
    soc_lb_cfg_rx_modem_info_t *rx_modem_info
  )
{
    cmd_result_t cmd_rv = CMD_OK;
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    soc_lb_cfg_rx_modem_info_t   *rx_modem_info_tmp = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT2(_DPP_LB_CFG_RX_MODEM_ITEM_STR_QAX_INIT, _DPP_LB_CFG_RX_MODEM_ITEM_STR_QUX_INIT)

    if (NULL == rx_modem_info) {
        cli_out("Error: %s is NULL\n", "rx_modem_info");
        cmd_rv = CMD_FAIL;
        goto exit;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Modem Config");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (modem_id == -1) {
        modem_start = 0;
        modem_end = SOC_TMC_LB_MODEM_MAX;
    }
    else {
        modem_start = modem_end = modem_id;
    }

    for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
        rx_modem_info_tmp = rx_modem_info + modem_tmp;

        sub_item_cnt = 0;
        DPP_LB_SUB_ITEM_DEC(modem_tmp);      
        DPP_LB_SUB_ITEM_DEC2(rx_modem_info_tmp->lbg_id, SOC_TMC_LB_LBG_INVALID);
        DPP_LB_SUB_ITEM_DEC2(rx_modem_info_tmp->logic_port, BCM_LB_PORT_INVALID);
        DPP_LB_SUB_ITEM_HEX(rx_modem_info_tmp->vlan_bmp);
        if (!SOC_IS_QUX(unit)) {
            DPP_LB_SUB_ITEM_HEX(rx_modem_info_tmp->fifo_start);
        }
        DPP_LB_SUB_ITEM_HEX(rx_modem_info_tmp->fifo_size);

        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

exit:
    DPP_LB_SUB_ITEM_VAR_END2

    return cmd_rv;
}

/* 
   Purpose: Display global configuration in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                      TX Global Config                                      |
   |--------------------------------------------------------------------------------------------|
   | SegmentMode  | VlanTpid | LbgTpid | PktCrcEna                                              |
   |--------------------------------------------------------------------------------------------|
   | 192 enhanced | 0x8100   | 0x88a8  | Y                                                      |
   |--------------------------------------------------------------------------------------------|
   | TC | Dp  | Pkt-Pri | Pkt-Cfi | Pkt-Dp                                                      |
   |--------------------------------------------------------------------------------------------|
   | 7  | 3   |         |         |                                                             |
   | 0  | 1   |         |         |                                                             |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_config_info_tx_global_pri_map_print(
    soc_lb_cfg_tx_glb_info_t   *tx_glb_info
  )
{
    uint32 tc = 0, dp = 0;
    soc_lb_pkt_pri_t *pkt_pri;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT(_DPP_LB_CFG_TX_GLOBAL_ITEM_STR_2_INIT)

    if (NULL == tx_glb_info) {
        cli_out("Error: %s is NULL\n", "tx_glb_info");
        return CMD_FAIL;
    }

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    for (tc = 0; tc < SOC_TMC_NOF_TRAFFIC_CLASSES; tc++) {
        for (dp = 0; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++) {
            pkt_pri = &(tx_glb_info->pkt_pri[tc][dp]);

            sub_item_cnt = 0;
            DPP_LB_SUB_ITEM_DEC(tc);    
            DPP_LB_SUB_ITEM_DEC(dp);
            DPP_LB_SUB_ITEM_DEC(pkt_pri->pkt_pri);
            DPP_LB_SUB_ITEM_DEC(pkt_pri->pkt_cfi);
            DPP_LB_SUB_ITEM_DEC(pkt_pri->pkt_dp);

            cli_out("%s\n", split_line);
            _dpp_lb_item_print(item_pstr, sub_item_cnt);
        }
    } 

    cli_out("%s\n", block_line);

    return CMD_OK;
}


/* 
   Purpose: Display global configuration in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                      TX Global Config                                      |
   |--------------------------------------------------------------------------------------------|
   | SegmentMode  | VlanTpid | LbgTpid | PktCrcEna                                              |
   |--------------------------------------------------------------------------------------------|
   | 192 enhanced | 0x8100   | 0x88a8  | Y                                                      |
   |--------------------------------------------------------------------------------------------|
   | TC | Dp  | Pkt-Pri | Pkt-Cfi | Pkt-Dp                                                      |
   |--------------------------------------------------------------------------------------------|
   | 7  | 3   |         |         |                                                             |
   | 0  | 1   |         |         |                                                             |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_config_info_tx_global_print(
    int                         unit,
    soc_lb_cfg_tx_glb_info_t   *tx_glb_info
  )
{
    cmd_result_t cmd_rv = CMD_OK;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT2(_DPP_LB_CFG_TX_GLOBAL_ITEM_STR_QAX1_INIT, _DPP_LB_CFG_TX_GLOBAL_ITEM_STR_QUX1_INIT)

    if (NULL == tx_glb_info) {
        cli_out("Error: %s is NULL\n", "tx_glb_info");
        cmd_rv = CMD_FAIL;
        goto exit;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Global Config");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    sub_item_cnt = 0;
    if (!SOC_IS_QUX(unit)) {
        DPP_LB_SUB_ITEM_DEC(tx_glb_info->segment_mode);
    }
    DPP_LB_SUB_ITEM_HEX(tx_glb_info->vlan_tpid);
    DPP_LB_SUB_ITEM_HEX(tx_glb_info->lbg_tpid);
    if (!SOC_IS_QUX(unit)) {
        DPP_LB_SUB_ITEM_DEC(tx_glb_info->is_pkt_crc_ena);
    }

    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_pstr, sub_item_cnt);
    cli_out("%s\n", split_line);

    cmd_rv = _dpp_lb_config_info_tx_global_pri_map_print(tx_glb_info);

exit:
    DPP_LB_SUB_ITEM_VAR_END2;

    return cmd_rv;
}

/* 
   Purpose: Display lbg configuration in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                          TX Lbg Config                                     |
   |--------------------------------------------------------------------------------------------|
   |   lbg  | enable | Modem  | logic port | FifoStart | FifoEnd | SeqNumWidth | LbgSchedule    |
   |--------------------------------------------------------------------------------------------|
   |   0    |   Y    | 0xffff |    20      | 9bits     | 9bits   |             |                |
   |   1    |   N    | 0xffff |    21      | 0x1ff     | 0x1ff   |             |                |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_config_info_tx_lbg_print(
    int                         unit,
    soc_lbg_t                   lbg_id,
    soc_lb_cfg_tx_lbg_info_t   *tx_lbg_info
  )
{
    cmd_result_t cmd_rv = CMD_OK;
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    soc_lb_cfg_tx_lbg_info_t   *tx_lbg_info_tmp = NULL;

    char *segment_mode_strs[] = _SOC_LB_SEGMENT_MODE_MSG_INIT;
    char *segment_mode_str = NULL;
    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT2(_DPP_LB_CFG_TX_LBG_ITEM_STR_QAX_INIT, _DPP_LB_CFG_TX_LBG_ITEM_STR_QUX_INIT)

    if (NULL == tx_lbg_info) {
        cli_out("Error: %s is NULL\n", "tx_lbg_info");
        cmd_rv = CMD_FAIL;
        goto exit;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Lbg Config");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (lbg_id == -1) {
        lbg_start = 0;
        lbg_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        lbg_start = lbg_end = lbg_id;
    }

    for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
        tx_lbg_info_tmp = tx_lbg_info + lbg_tmp;

        sub_item_cnt = 0;
        DPP_LB_SUB_ITEM_DEC(lbg_tmp);    
        DPP_LB_SUB_ITEM_STR(tx_lbg_info_tmp->is_enable ? "Y" : "N"); 
        DPP_LB_SUB_ITEM_HEX(tx_lbg_info_tmp->modem_bmp);
        DPP_LB_SUB_ITEM_DEC2(tx_lbg_info_tmp->logic_port, BCM_LB_PORT_INVALID);
        DPP_LB_SUB_ITEM_HEX(tx_lbg_info_tmp->fifo_start);
        DPP_LB_SUB_ITEM_HEX(tx_lbg_info_tmp->fifo_end);
        DPP_LB_SUB_ITEM_DEC(tx_lbg_info_tmp->seq_num_width);
        DPP_LB_SUB_ITEM_DEC(tx_lbg_info_tmp->lbg_weight);
        if (SOC_IS_QUX(unit)) {
            segment_mode_str = segment_mode_strs[(tx_lbg_info_tmp->segment_mode > socLbSegmentModeCount) ? 
                socLbSegmentModeCount : tx_lbg_info_tmp->segment_mode];
            DPP_LB_SUB_ITEM_STR(segment_mode_str);
            DPP_LB_SUB_ITEM_STR(tx_lbg_info_tmp->is_pkt_crc_ena ? "Y" : "N");
        }

        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

exit:
    DPP_LB_SUB_ITEM_VAR_END2

    return cmd_rv;
}

/* 
   Purpose: Display modem configuration in TX side in below format
   +--------------------------------------------------------------------------------------------------------------+
   |                                                  TX Modem Config                                             |
   |--------------------------------------------------------------------------------------------------------------|
   | modem  | Port | Chan |     DstMac     |     SrcMac     | VLAN | Shaper  | Rate    | Burst | HdrCom  | EgqInf | 
   |-----------------------------------------------------------------------------------------------------|--------|
   |   0    | 16   | Y    | 0x000000000001 | 0x000000000001 | fff  | enable  | 1048575 | 8191  | -63     | 128    |
   |   1    | 15   | N    | 0x000000000001 | 0x000000000001 | fff  | disable | 1048575 | 8191  | -63     |        |
   |   2    | no   | N    | 0xffffffffffff | 0x000000000001 | fff  | disable | 1048575 | 8191  | -63     |        |
   +--------------------------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_config_info_tx_modem_print(
    soc_modem_t                 modem_id,
    soc_lb_cfg_tx_modem_info_t *tx_modem_info
  )
{
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    soc_lb_cfg_tx_modem_info_t   *tx_modem_info_tmp = NULL;
    char dstMacStr[MACADDR_STR_LEN];
    char srcMacStr[MACADDR_STR_LEN];

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT(_DPP_LB_CFG_TX_MODEM_ITEM_STR_INIT)


    if (NULL == tx_modem_info) {
        cli_out("Error: %s is NULL\n", "tx_modem_info");
        return CMD_FAIL;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Modem Config");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (modem_id == -1) {
        modem_start = 0;
        modem_end = SOC_TMC_LB_MODEM_MAX;
    }
    else {
        modem_start = modem_end = modem_id;
    }

    for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
        tx_modem_info_tmp = tx_modem_info + modem_tmp;

        sub_item_cnt = 0;
        format_macaddr(dstMacStr, tx_modem_info_tmp->modem_packet.dst_mac);
        format_macaddr(srcMacStr, tx_modem_info_tmp->modem_packet.src_mac);
        DPP_LB_SUB_ITEM_DEC(modem_tmp);
        DPP_LB_SUB_ITEM_DEC2(tx_modem_info_tmp->lbg_id, SOC_TMC_LB_LBG_INVALID);
        DPP_LB_SUB_ITEM_DEC2(tx_modem_info_tmp->logic_port, BCM_LB_PORT_INVALID);
        DPP_LB_SUB_ITEM_STR(
            (tx_modem_info_tmp->modem_packet.pkt_format==socLbgFormatTypeChannelize)? "Y" : "N");
        DPP_LB_SUB_ITEM_STR(dstMacStr);
        DPP_LB_SUB_ITEM_STR(srcMacStr);
        DPP_LB_SUB_ITEM_DEC2(tx_modem_info_tmp->modem_packet.vlan, BCM_VLAN_INVALID);
        DPP_LB_SUB_ITEM_STR(tx_modem_info_tmp->modem_shaper.enable ? "Y" : "N");
        DPP_LB_SUB_ITEM_DEC(tx_modem_info_tmp->modem_shaper.rate);
        DPP_LB_SUB_ITEM_DEC(tx_modem_info_tmp->modem_shaper.max_burst);
        DPP_LB_SUB_ITEM_DEC(tx_modem_info_tmp->modem_shaper.hdr_compensation);
        DPP_LB_SUB_ITEM_DEC(tx_modem_info_tmp->egr_intf);

        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

STATIC cmd_result_t
  _dpp_lb_config_info_print(
    int                     unit,
    soc_lb_info_key_t      *lb_key,
    soc_lb_cfg_info_t      *lb_cfg
  )
{  
    if ((NULL == lb_key) || (NULL == lb_cfg)) {
        cli_out("Error: parameter is NULL\n");
        return CMD_FAIL;
    }

    if ((socLbDirectionRx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_config_info_rx_global_print(unit, &(lb_cfg->rx_cfg_info.rx_glb_info));
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_config_info_rx_lbg_print(unit, lb_key->lbg_id, lb_cfg->rx_cfg_info.rx_lbg_info);
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_config_info_rx_modem_print(unit, lb_key->modem_id, lb_cfg->rx_cfg_info.rx_modem_info);
        }
    }

    if ((socLbDirectionTx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_config_info_tx_global_print(unit, &(lb_cfg->tx_cfg_info.tx_glb_info));
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_config_info_tx_lbg_print(unit, lb_key->lbg_id, lb_cfg->tx_cfg_info.tx_lbg_info);
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_config_info_tx_modem_print(lb_key->modem_id, lb_cfg->tx_cfg_info.tx_modem_info);
        }
    }

    return CMD_OK;
}

/* 
   Purpose: Display lbg Status in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                          RX Lbg Status                                     |
   |--------------------------------------------------------------------------------------------|
   |   lbg  | IsInSync | ExpSeqNum                                                              |
   |--------------------------------------------------------------------------------------------|
   |   0    |   Y      | 1048575                                                                |
   |   1    |   N      | 1048575                                                                |
   +--------------------------------------------------------------------------------------------+
 */ 
STATIC cmd_result_t
_dpp_lb_status_info_rx_lbg_print (
    int unit,
    soc_lbg_t              lbg_id,
    _dpp_lb_status_info_t *lb_status
  )
{
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    _dpp_lb_status_rx_lbg_info_t   *rx_lbg_status = NULL;
    uint8 bit_val = 0;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT(_DPP_LB_STATUS_RX_LBG_ITEM_STR_INIT)

    if (NULL == lb_status) {
        cli_out("Error: %s is NULL\n", "lb_status");
        return CMD_FAIL;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Lbg Status");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (lbg_id == -1) {
        lbg_start = 0;
        lbg_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        lbg_start = lbg_end = lbg_id;
    }

    for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
        if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
            continue;
        }
 
        rx_lbg_status = &(lb_status->rx_status.rx_lbg_status[lbg_tmp]);
        sub_item_cnt = 0;
        DPP_LB_SUB_ITEM_DEC(lbg_tmp);    
        DPP_LB_SUB_ITEM_STR((rx_lbg_status->is_in_sync ? "Y" : "N"));
        DPP_LB_SUB_ITEM_DEC(rx_lbg_status->exp_seq_num);
        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display modem Status in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                        RX Modem Status                                     |
   |--------------------------------------------------------------------------------------------|
   | modem  | IsEmptyFifo                                                                       | 
   |--------------------------------------------------------------------------------------------|
   |   0    | Y                                                                                 |
   |   1    | N                                                                                 |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
  _dpp_lb_status_info_rx_modem_print(
    soc_modem_t            modem_id,
    _dpp_lb_status_info_t *lb_status
  )
{
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    _dpp_lb_status_rx_modem_info_t   *rx_modem_status = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT(_DPP_LB_STATUS_RX_MODEM_ITEM_STR_INIT)

    if (NULL == lb_status) {
        cli_out("Error: %s is NULL\n", "lb_status");
        return CMD_FAIL;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Modem Status");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (modem_id == -1) {
        modem_start = 0;
        modem_end = SOC_TMC_LB_MODEM_MAX;
    }
    else {
        modem_start = modem_end = modem_id;
    }

    for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
        rx_modem_status = &(lb_status->rx_status.rx_modem_status[modem_tmp]);

        sub_item_cnt = 0;
        DPP_LB_SUB_ITEM_DEC(modem_tmp);    
        DPP_LB_SUB_ITEM_STR((rx_modem_status->is_empty_fifo ? "Y" : "N"));
        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display lbg Status in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                          RX Lbg Status                                     |
   |--------------------------------------------------------------------------------------------|
   | Lbg    | IsEmptyFifo                                                                       | 
   |--------------------------------------------------------------------------------------------|
   |   0    | Y                                                                                 |
   |   1    | N                                                                                 |
   +--------------------------------------------------------------------------------------------+
 */ 
STATIC cmd_result_t
  _dpp_lb_status_info_tx_lbg_print(
    int unit,
    soc_lbg_t              lbg_id,
    _dpp_lb_status_info_t *lb_status
  )
{
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    _dpp_lb_status_tx_lbg_info_t   *tx_lbg_status = NULL;
    uint8 bit_val = 0;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    DPP_LB_SUB_ITEM_VAR_INIT(_DPP_LB_STATUS_TX_LBG_ITEM_STR_INIT)

    if (NULL == lb_status) {
        cli_out("Error: %s is NULL\n", "lb_status");
        return CMD_FAIL;
    }

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Lbg Status");

    /*** 2. print item header ***/
    cli_out("%s\n", split_line);
    _dpp_lb_item_print(item_header_str, nof_sub_item);

    /*** 3. print item ***/
    cli_out("%s\n", split_line);

    if (lbg_id == -1) {
        lbg_start = 0;
        lbg_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        lbg_start = lbg_end = lbg_id;
    }

    for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
        if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
            continue;
        }

        tx_lbg_status = &(lb_status->tx_status.tx_lbg_status[lbg_tmp]);
        sub_item_cnt = 0;
        DPP_LB_SUB_ITEM_DEC(lbg_tmp);    
        DPP_LB_SUB_ITEM_STR((tx_lbg_status->is_empty_fifo ? "Y" : "N"));
        _dpp_lb_item_print(item_pstr, sub_item_cnt);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

STATIC cmd_result_t
  _dpp_lb_status_info_get(
    int unit,
    soc_lb_info_key_t     *lb_key,
    _dpp_lb_status_info_t *lb_status
  )
{
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    uint32 flags = 0;
    int status_val = 0;
    uint8 bit_val = 0;
    int rv = 0;

    if ((NULL == lb_key) || (NULL == lb_status)) {
        cli_out("Error: parameter is NULL\n");
        return CMD_FAIL;
    }
    if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
        if ((lb_key->lbg_id != -1) && (lb_key->lbg_id > SOC_TMC_LB_LBG_MAX)) {
            cli_out("Fail parameter(lbg_id=%d) should be less than %d\n", lb_key->lbg_id, SOC_TMC_LB_LBG_MAX);
            return CMD_FAIL;
        }
    }
    if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
        if ((lb_key->lbg_id != -1) && (lb_key->lbg_id > SOC_TMC_LB_MODEM_MAX)) {
            cli_out("Fail parameter(modem_id=%d) should be less than %d\n", lb_key->modem_id, SOC_TMC_LB_MODEM_MAX);
            return CMD_FAIL;
        }
    }

    if ((socLbDirectionRx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) 
    {
        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            if (lb_key->lbg_id == -1) {
                lbg_start = 0;
                lbg_end = SOC_TMC_LB_LBG_MAX;
            }
            else {
                lbg_start = lbg_end = lb_key->lbg_id;
            }

            for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
                if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
                    continue;
                }

                rv = bcm_lb_status_get(unit, lbg_tmp, bcmLbDirectionRx, flags, bcmLbStatusIsInSync, &status_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                lb_status->rx_status.rx_lbg_status[lbg_tmp].is_in_sync = status_val;

                rv = bcm_lb_control_get(unit, lbg_tmp, bcmLbDirectionRx, flags, bcmLbControlExpectedSequenceNumber, &status_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                lb_status->rx_status.rx_lbg_status[lbg_tmp].exp_seq_num = status_val;                
            }
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            if (lb_key->modem_id == -1) {
                modem_start = 0;
                modem_end = SOC_TMC_LB_MODEM_MAX;
            }
            else {
                modem_start = modem_end = lb_key->modem_id;
            }

            for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
                rv = bcm_lb_modem_status_get(unit, modem_tmp, bcmLbDirectionRx, flags, bcmLbModemStatusIsEmptyFifo, &status_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                lb_status->rx_status.rx_modem_status[modem_tmp].is_empty_fifo = status_val;
            }
        }
    }

    if ((socLbDirectionTx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) 
    {
        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            if (lb_key->lbg_id == -1) {
                lbg_start = 0;
                lbg_end = SOC_TMC_LB_LBG_MAX;
            }
            else {
                lbg_start = lbg_end = lb_key->lbg_id;
            }

            for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
                if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
                    continue;
                }
 
                rv = bcm_lb_status_get(unit, lbg_tmp, bcmLbDirectionTx, flags, bcmLbStatusIsEmptyFifo, &status_val);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                }

                lb_status->tx_status.tx_lbg_status[lbg_tmp].is_empty_fifo = status_val;
            }
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
  _dpp_lb_status_info_print(
    int unit,
    soc_lb_info_key_t     *lb_key,
    _dpp_lb_status_info_t *lb_status
  )
{
    if ((NULL == lb_key) || (NULL == lb_status)) {
        cli_out("Error: parameter is NULL\n");
        return CMD_FAIL;
    }

    if ((socLbDirectionRx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) 
    {
        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_status_info_rx_lbg_print(unit, lb_key->lbg_id, lb_status);
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_status_info_rx_modem_print(lb_key->modem_id, lb_status);
        }
    }

    if ((socLbDirectionTx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) 
    {
        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_status_info_tx_lbg_print(unit, lb_key->lbg_id, lb_status);
        }
    }

    return CMD_OK;
}

/* 
   Purpose: Display Global counter in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                     RX Global Counter                                      |
   |--------------------------------------------------------------------------------------------|
   | RDrop            :                                                                         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_counter_info_rx_global_print(
    _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    _dpp_lb_counter_rx_global_info_t   *rx_global_counter = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    char counter_name[30] = {0};

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Global Counter");

    /*** 2. print item ***/
    cli_out("%s\n", split_line);

    rx_global_counter = &(lb_counter->rx_counter.rx_global_counter);
    sal_snprintf(counter_name, sizeof(counter_name), "RDrop");
    _dpp_lb_counter_item_print(counter_name, print_none_zero, rx_global_counter->discard_fragments);

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display lbg counter in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                       RX Lbg Counter                                       |
   |--------------------------------------------------------------------------------------------|
   | RDrop            :                                                                         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_counter_info_rx_lbg_print(
    int unit,
    soc_lbg_t              lbg_id,
    _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    _dpp_lb_counter_rx_lbg_info_t   *rx_lbg_counter = NULL;
    uint8 bit_val = 0;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    char counter_name[30] = {0};

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Lbg Counter");

    /*** 2. print item ***/
    cli_out("%s\n", split_line);

    if (lbg_id == -1) {
        lbg_start = 0;
        lbg_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        lbg_start = lbg_end = lbg_id;
    }

    for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
        if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
            continue;
        }

        rx_lbg_counter = &(lb_counter->rx_counter.rx_lbg_counter[lbg_tmp]);

        sal_snprintf(counter_name, sizeof(counter_name), "RDrop.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, rx_lbg_counter->discard_fragments);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display modem Counter in RX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                      RX Modem Counter                                      |
   |--------------------------------------------------------------------------------------------|
   | RPKT.modem0      :                                                                         |
   | RBYT.modem0      :                                                                         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
  _dpp_lb_counter_info_rx_modem_print(
    soc_modem_t             modem_id,
    _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    _dpp_lb_counter_rx_modem_info_t   *rx_modem_counter = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    char counter_name[30] = {0};

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("RX Modem Counter");

    /*** 2. print item ***/
    cli_out("%s\n", split_line);

    if (modem_id == -1) {
        modem_start = 0;
        modem_end = SOC_TMC_LB_MODEM_MAX;
    }
    else {
        modem_start = modem_end = modem_id;
    }

    for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
        rx_modem_counter = &(lb_counter->rx_counter.rx_modem_counter[modem_tmp]);

        sal_snprintf(counter_name, sizeof(counter_name), "RPKT.modem%d", modem_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, rx_modem_counter->pkts);

        sal_snprintf(counter_name, sizeof(counter_name), "RBYT.modem%d", modem_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, rx_modem_counter->octets);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display Global counter in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                     TX Global Counter                                      |
   |--------------------------------------------------------------------------------------------|
   | TErr             :                                                                         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_counter_info_tx_global_print(
   _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    _dpp_lb_counter_tx_global_info_t   *tx_global_counter = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    char counter_name[30] = {0};

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Global Counter");

    /*** 2. print item ***/
    cli_out("%s\n", split_line);

    tx_global_counter = &(lb_counter->tx_counter.tx_global_counter);
    sal_snprintf(counter_name, sizeof(counter_name), "TErr");
    _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_global_counter->error_pkts);

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display lbg counter in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                       TX Lbg Counter                                       |
   |--------------------------------------------------------------------------------------------|
   | TUCA.lbg0        :                                                                         |
   | TMCA.lbg0        :                                                                         |
   | TBCA.lbg0        :                                                                         |
   | T64.lbg0         :                                                                         |
   | T65to127.lbg0    :                                                                         |
   | T128to255.lbg0   :                                                                         |
   | T256to511.lbg0   :                                                                         |
   | T512to1023.lbg0  :                                                                         |
   | T1024to1518.lbg0 :                                                                         |
   | T1519.lbg0       :                                                                         |
   | TPKT.lbg0        :                                                                         |
   | TBYT.lbg0        :                                                                         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_counter_info_tx_lbg_print(
    int unit,
    soc_lbg_t              lbg_id,
    _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    _dpp_lb_counter_tx_lbg_info_t   *tx_lbg_counter = NULL;
    uint8 bit_val = 0;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    char counter_name[30] = {0};

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Lbg Counter");

    /*** 2. print item ***/
    cli_out("%s\n", split_line);

    if (lbg_id == -1) {
        lbg_start = 0;
        lbg_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        lbg_start = lbg_end = lbg_id;
    }

    for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
        if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
            continue;
        }

        tx_lbg_counter = &(lb_counter->tx_counter.tx_lbg_counter[lbg_tmp]);

        sal_snprintf(counter_name, sizeof(counter_name), "TUCA.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->unicast_pkts);
        sal_snprintf(counter_name, sizeof(counter_name), "TMCA.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->multicast_pkts);
        sal_snprintf(counter_name, sizeof(counter_name), "TBCA.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->broadcast_pkts);
        sal_snprintf(counter_name, sizeof(counter_name), "T64.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->pkts_64octets);
        sal_snprintf(counter_name, sizeof(counter_name), "T65to127.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->pkts_65to127octets);
        sal_snprintf(counter_name, sizeof(counter_name), "T128to255.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->pkts_128to255octets);
        sal_snprintf(counter_name, sizeof(counter_name), "T256to511.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->Pkts_256to511octets);
        sal_snprintf(counter_name, sizeof(counter_name), "T512to1023.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->Pkts_512to1023octets);
        sal_snprintf(counter_name, sizeof(counter_name), "T1024to1518.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->Pkts_1024to1518octets);
        sal_snprintf(counter_name, sizeof(counter_name), "T1519.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->Pkts_1519octets);
        sal_snprintf(counter_name, sizeof(counter_name), "TPKT.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->pkts);
        sal_snprintf(counter_name, sizeof(counter_name), "TBYT.lbg%d", lbg_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_lbg_counter->octets);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

/* 
   Purpose: Display modem counter in TX side in below format
   +--------------------------------------------------------------------------------------------+
   |                                      TX Modem Counter                                      |
   |--------------------------------------------------------------------------------------------|
   | TPKT.modem0      :                                                                         |
   | TBYT.modem0      :                                                                         |
   +--------------------------------------------------------------------------------------------+
 */
STATIC cmd_result_t
_dpp_lb_counter_info_tx_modem_print(
    soc_modem_t             modem_id,
    _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    _dpp_lb_counter_tx_modem_info_t   *tx_modem_counter = NULL;

    char *block_line = BLOCK_LINE_STR;
    char *split_line = SPLIT_LINE_STR;
    char counter_name[30] = {0};

    /*** 1. print block header ***/
    _dpp_lb_block_header_print("TX Modem Counter");

    /*** 2. print item ***/
    cli_out("%s\n", split_line);

    if (modem_id == -1) {
        modem_start = 0;
        modem_end = SOC_TMC_LB_MODEM_MAX;
    }
    else {
        modem_start = modem_end = modem_id;
    }

    for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
        tx_modem_counter = &(lb_counter->tx_counter.tx_modem_counter[modem_tmp]);
        
        sal_snprintf(counter_name, sizeof(counter_name), "TPKT.modem%d", modem_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_modem_counter->pkts);

        sal_snprintf(counter_name, sizeof(counter_name), "TBYT.modem%d", modem_tmp);
        _dpp_lb_counter_item_print(counter_name, print_none_zero, tx_modem_counter->octets);
    }

    cli_out("%s\n", block_line);

    return CMD_OK;
}

#define DPP_LB_STAT_LBG_GET(_direction, _stat_type, _stat_val) {                            \
    rv = bcm_lb_stat_get(unit, lbg_tmp, _direction, flags, _stat_type, &_stat_val);         \
    if (rv != BCM_E_NONE) {                                                                 \
        cli_out("fail in bcm_lb_status_get stat(%d)\n", _stat_type);                        \
        return CMD_FAIL;                                                                    \
    }                                                                                       \
}

#define DPP_LB_STAT_MODEM_GET(_direction, _stat_type, _stat_val) {                                 \
    rv = bcm_lb_modem_stat_get(unit, modem_tmp, _direction, flags, _stat_type, &_stat_val);        \
    if (rv != BCM_E_NONE) {                                                                        \
        cli_out("fail in bcm_lb_modem_stat_get stat(%d)\n", _stat_type);                           \
        return CMD_FAIL;                                                                           \
    }                                                                                              \
}

STATIC cmd_result_t
  _dpp_lb_counter_info_get(
    int unit,
    soc_lb_info_key_t      *lb_key,
    _dpp_lb_counter_info_t *lb_counter
  )
{
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;
    _dpp_lb_counter_rx_lbg_info_t     *rx_lbg_counter = NULL;
    _dpp_lb_counter_rx_modem_info_t   *rx_modem_counter = NULL;
    _dpp_lb_counter_tx_lbg_info_t     *tx_lbg_counter = NULL;
    _dpp_lb_counter_tx_modem_info_t   *tx_modem_counter = NULL;
    uint32 flags = 0;
    uint64 stat_val = COMPILER_64_INIT(0, 0);
    uint8 bit_val = 0;
    int rv = 0;

    if ((NULL == lb_key) || (NULL == lb_counter)) {
        cli_out("Error: parameter is NULL\n");
        return CMD_FAIL;
    }
    if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
        if ((lb_key->lbg_id != -1) && (lb_key->lbg_id > SOC_TMC_LB_LBG_MAX)) {
            cli_out("Fail parameter(lbg_id=%d) should be less than %d\n", lb_key->lbg_id, SOC_TMC_LB_LBG_MAX);
            return CMD_FAIL;
        }
    }
    if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
        if ((lb_key->lbg_id != -1) && (lb_key->lbg_id > SOC_TMC_LB_MODEM_MAX)) {
            cli_out("Fail parameter(modem_id=%d) should be less than %d\n", lb_key->modem_id, SOC_TMC_LB_MODEM_MAX);
            return CMD_FAIL;
        }
    }

    if ((socLbDirectionRx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) 
    {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            flags = BCM_LB_FLAG_GLOBAL;
            rv = bcm_lb_stat_get(unit, 0, bcmLbDirectionRx, flags, bcmLbStatsDiscardFragments, &stat_val);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }
            lb_counter->rx_counter.rx_global_counter.discard_fragments = stat_val;
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            flags = 0;
            if (lb_key->lbg_id == -1) {
                lbg_start = 0;
                lbg_end = SOC_TMC_LB_LBG_MAX;
            }
            else {
                lbg_start = lbg_end = lb_key->lbg_id;
            }

            for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
                if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
                    continue;
                }

                rx_lbg_counter = &(lb_counter->rx_counter.rx_lbg_counter[lbg_tmp]);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionRx, bcmLbStatsDiscardFragments, rx_lbg_counter->discard_fragments);
            }
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            flags = 0;
            if (lb_key->modem_id == -1) {
                modem_start = 0;
                modem_end = SOC_TMC_LB_MODEM_MAX;
            }
            else {
                modem_start = modem_end = lb_key->modem_id;
            }

            for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
                rx_modem_counter = &(lb_counter->rx_counter.rx_modem_counter[modem_tmp]);
                DPP_LB_STAT_MODEM_GET(bcmLbDirectionRx, bcmLbModemStatsPkts, rx_modem_counter->pkts);
                DPP_LB_STAT_MODEM_GET(bcmLbDirectionRx, bcmLbModemStatsOctets, rx_modem_counter->octets);
            }
        }
    }

    if ((socLbDirectionTx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) 
    {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            flags = BCM_LB_FLAG_GLOBAL;
            rv = bcm_lb_stat_get(unit, 0, bcmLbDirectionTx, flags, bcmLbStatsErrorPkts, &stat_val);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }
            lb_counter->tx_counter.tx_global_counter.error_pkts = stat_val;
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            flags = 0;
            if (lb_key->lbg_id == -1) {
                lbg_start = 0;
                lbg_end = SOC_TMC_LB_LBG_MAX;
            }
            else {
                lbg_start = lbg_end = lb_key->lbg_id;
            }

            for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
                if ((LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_tmp, &bit_val) != BCM_E_NONE) || !bit_val) {
                    continue;
                }

                tx_lbg_counter = &(lb_counter->tx_counter.tx_lbg_counter[lbg_tmp]);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsUnicastPkts, tx_lbg_counter->unicast_pkts);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsMulticastPkts, tx_lbg_counter->multicast_pkts);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsBroadcastPkts, tx_lbg_counter->broadcast_pkts);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts64Octets, tx_lbg_counter->pkts_64octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts65to127Octets, tx_lbg_counter->pkts_65to127octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts128to255Octets, tx_lbg_counter->pkts_128to255octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts256to511Octets, tx_lbg_counter->Pkts_256to511octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts512to1023Octets, tx_lbg_counter->Pkts_512to1023octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts1024to1518Octets, tx_lbg_counter->Pkts_1024to1518octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts1519Octets, tx_lbg_counter->Pkts_1519octets);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsPkts, tx_lbg_counter->pkts);
                DPP_LB_STAT_LBG_GET(bcmLbDirectionTx, bcmLbStatsOctets, tx_lbg_counter->octets);
            }
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            flags = 0;
            if (lb_key->modem_id == -1) {
                modem_start = 0;
                modem_end = SOC_TMC_LB_MODEM_MAX;
            }
            else {
                modem_start = modem_end = lb_key->modem_id;
            }

            for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
                tx_modem_counter = &(lb_counter->tx_counter.tx_modem_counter[modem_tmp]);
                DPP_LB_STAT_MODEM_GET(bcmLbDirectionTx, bcmLbModemStatsPkts, tx_modem_counter->pkts);
                DPP_LB_STAT_MODEM_GET(bcmLbDirectionTx, bcmLbModemStatsOctets, tx_modem_counter->octets);
            }
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
  _dpp_lb_counter_info_print(
    int unit,
    soc_lb_info_key_t      *lb_key,
    _dpp_lb_counter_info_t *lb_counter, 
    uint32                  print_none_zero
  )
{
    if ((NULL == lb_key) || (NULL == lb_counter)) {
        cli_out("Error: parameter is NULL\n");
        return CMD_FAIL;
    }

    if ((socLbDirectionRx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_counter_info_rx_global_print(lb_counter, print_none_zero);
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_counter_info_rx_lbg_print(unit, lb_key->lbg_id, lb_counter, print_none_zero);
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_counter_info_rx_modem_print(lb_key->modem_id, lb_counter, print_none_zero);
        }
    }

    if ((socLbDirectionTx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_counter_info_tx_global_print(lb_counter, print_none_zero);
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_counter_info_tx_lbg_print(unit, lb_key->lbg_id, lb_counter, print_none_zero);
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            _dpp_lb_counter_info_tx_modem_print(lb_key->modem_id, lb_counter, print_none_zero);
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
    cmd_dpp_lb_config_show(int unit, args_t *a)
{
    cmd_result_t rv = CMD_OK;
    parse_table_t pt;
    char *arg_direction_str = NULL, *arg_obj_str = NULL;
    int arg_lbg_id = -1;
    int arg_modem_id = -1;
    int direction = 0;
    int lb_obj = 0;

    soc_lb_info_key_t     lb_key;
    soc_lb_cfg_info_t    *lb_cfg_info = NULL;

    if (!SOC_IS_QAX(unit)) {
        cli_out("%s: Link Bonding isn't support on this chip %d\n", FUNCTION_NAME(), unit);
        return CMD_FAIL;
    }
    if (0 == SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
        cli_out("%s: Link Bonding is not enabled on unit %d\n", FUNCTION_NAME(), unit);
        return CMD_FAIL;
    } 

    lb_cfg_info = sal_alloc(sizeof(soc_lb_cfg_info_t), "cmd_dpp_lb_config_show.soc_lb_cfg_info_t");
    if(lb_cfg_info == NULL) {
        cli_out("%s: fail when alloc memory for lb_cfg_info\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "direction", PQ_STRING, "both", &arg_direction_str, NULL);
    parse_table_add(&pt, "obj", PQ_STRING, "all", &arg_obj_str, NULL);
    parse_table_add(&pt, "lbg_id", PQ_DFL|PQ_INT, (void *)-1, (void *)&arg_lbg_id, 0);
    parse_table_add(&pt, "modem_id", PQ_DFL|PQ_INT, (void *)-1, (void *)&arg_modem_id, 0);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        rv = CMD_USAGE;
        goto done;
    }

    if (CMD_OK != _lb_parse_str(PARSE_LB_DIRECTION, arg_direction_str, &direction)) {
        cli_out("%s: fail parse lb direction(%s)\n", FUNCTION_NAME(), arg_direction_str);
        rv = CMD_USAGE;
        goto done;
    }

    if (CMD_OK != _lb_parse_str(PARSE_LB_OBJ, arg_obj_str, &lb_obj)) {
        cli_out("%s: fail parse lb obj(%s)\n", FUNCTION_NAME(), arg_obj_str);
        rv = CMD_USAGE;
        goto done;
    }

    sal_memset(&lb_key, 0, sizeof(lb_key));
    lb_key.direction = (soc_lb_direction_type_t)direction;
    lb_key.obj_type = lb_obj;
    lb_key.lbg_id = arg_lbg_id;
    lb_key.modem_id = arg_modem_id;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(lb_cfg_info, 0, sizeof(soc_lb_cfg_info_t));
    if (SOC_E_NONE != soc_dpp_lb_config_info_get(unit, &lb_key, lb_cfg_info)) {
        cli_out("fail in soc_dpp_lb_config_info_get\n");
        rv = CMD_FAIL;
        goto done;
    }

    if (CMD_OK != _dpp_lb_config_info_print(unit, &lb_key, lb_cfg_info)) {
        cli_out("fail in _dpp_lb_config_info_print\n");
        rv = CMD_FAIL;
        goto done;
    }

done:
    if (lb_cfg_info != NULL) {
        sal_free(lb_cfg_info);
    }

    return rv;
}

STATIC cmd_result_t
    cmd_dpp_lb_status_show(int unit, args_t *a)
{
    cmd_result_t rv = CMD_OK;
    parse_table_t pt;
    char *arg_direction_str = NULL, *arg_obj_str = NULL;
    int arg_lbg_id = -1;
    int arg_modem_id = -1;
    int direction = 0;
    int lb_obj = 0;

    soc_lb_info_key_t     lb_key;
    _dpp_lb_status_info_t *lb_status = NULL;

    if (!SOC_IS_QAX(unit)) {
        cli_out("%s: Link Bonding isn't support on this chip %d\n", FUNCTION_NAME(), unit);
        return CMD_FAIL;
    }
    if (0 == SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
        cli_out("%s: Link Bonding is not enabled on unit %d\n", FUNCTION_NAME(), unit);
        return CMD_FAIL;
    } 

    lb_status = sal_alloc(sizeof(_dpp_lb_status_info_t), "cmd_dpp_lb_status_show._dpp_lb_status_info_t");
    if(lb_status == NULL) {
        cli_out("%s: fail when alloc memory for lb_status\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "direction", PQ_STRING, "both", &arg_direction_str, NULL);
    parse_table_add(&pt, "obj", PQ_STRING, "all", &arg_obj_str, NULL);
    parse_table_add(&pt, "lbg_id", PQ_DFL|PQ_INT, (void *)-1, (void *)&arg_lbg_id, 0);
    parse_table_add(&pt, "modem_id", PQ_DFL|PQ_INT, (void *)-1, (void *)&arg_modem_id, 0);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        rv = CMD_USAGE;
        goto done;
    }

    if (CMD_OK != _lb_parse_str(PARSE_LB_DIRECTION, arg_direction_str, &direction)) {
        cli_out("%s: fail parse lb direction(%s)\n", FUNCTION_NAME(), arg_direction_str);
        rv = CMD_USAGE;
        goto done;
    }
    if (CMD_OK != _lb_parse_str(PARSE_LB_OBJ, arg_obj_str, &lb_obj)) {
        cli_out("%s: fail parse lb obj(%s)\n", FUNCTION_NAME(), arg_obj_str);
        rv = CMD_USAGE;
        goto done;
    }
    sal_memset(&lb_key, 0, sizeof(lb_key));
    lb_key.direction = (soc_lb_direction_type_t)direction;
    lb_key.obj_type = lb_obj;
    lb_key.lbg_id = arg_lbg_id;
    lb_key.modem_id = arg_modem_id;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(lb_status, 0, sizeof(_dpp_lb_status_info_t));
    if (CMD_OK != _dpp_lb_status_info_get(unit, &lb_key, lb_status)) {
        cli_out("fail in _dpp_lb_status_info_get\n");
        rv = CMD_FAIL;
        goto done;
    }

    if (CMD_OK != _dpp_lb_status_info_print(unit, &lb_key, lb_status)) {
        cli_out("fail in _dpp_lb_status_info_print\n");
        rv = CMD_FAIL;
        goto done;
    }

done:
    if (lb_status != NULL) {
        sal_free(lb_status);
    }

    return rv;
}

STATIC cmd_result_t
    cmd_dpp_lb_counter_show(int unit, args_t *a)
{
    cmd_result_t rv = CMD_OK;
    parse_table_t pt;
    char *arg_direction_str = NULL, *arg_obj_str = NULL;
    int arg_lbg_id = -1;
    int arg_modem_id = -1;
    int direction = 0;
    int lb_obj = 0;
    uint32 ignore_zero = 0;

    int arg_index = 0;
    char *option;      

    soc_lb_info_key_t       lb_key;
    _dpp_lb_counter_info_t *lb_counter = NULL;

    if (!SOC_IS_QAX(unit)) {
        cli_out("%s: Link Bonding isn't support on this chip %d\n", FUNCTION_NAME(), unit);
        return CMD_FAIL;
    }
    if (0 == SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
        cli_out("%s: Link Bonding is not enabled on unit %d\n", FUNCTION_NAME(), unit);
        return CMD_FAIL;
    }

    lb_counter = sal_alloc(sizeof(_dpp_lb_counter_info_t), "cmd_dpp_lb_counter_show._dpp_lb_counter_info_t");
    if(lb_counter == NULL) {
        cli_out("%s: fail when alloc memory for lb_counter\n", FUNCTION_NAME());
        return CMD_FAIL;
    }
    arg_index = ARG_CUR_INDEX(a);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "direction", PQ_STRING, "both", &arg_direction_str, NULL);
    parse_table_add(&pt, "obj", PQ_STRING, "all", &arg_obj_str, NULL);
    parse_table_add(&pt, "lbg_id", PQ_DFL|PQ_INT, (void *)-1, (void *)&arg_lbg_id, 0);
    parse_table_add(&pt, "modem_id", PQ_DFL|PQ_INT, (void *)-1, (void *)&arg_modem_id, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        rv = CMD_USAGE;
        goto done;
    }
    a->a_arg = arg_index;
    option = ARG_GET(a);

    while (NULL != option) {
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        if (!sal_strncasecmp(option, "nz", sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))) {
            ignore_zero = 1;
        }
        option = ARG_GET(a);
    }

    if (CMD_OK != _lb_parse_str(PARSE_LB_DIRECTION, arg_direction_str, &direction)) {
        cli_out("%s: fail parse lb direction(%s)\n", FUNCTION_NAME(), arg_direction_str);
        rv = CMD_USAGE;
        goto done;
    }

    if (CMD_OK != _lb_parse_str(PARSE_LB_OBJ, arg_obj_str, &lb_obj)) {
        cli_out("%s: fail parse lb obj(%s)\n", FUNCTION_NAME(), arg_obj_str);
        rv = CMD_USAGE;
        goto done;
    }

    sal_memset(&lb_key, 0, sizeof(lb_key));
    lb_key.direction = (soc_lb_direction_type_t)direction;
    lb_key.obj_type = lb_obj;
    lb_key.lbg_id = arg_lbg_id;
    lb_key.modem_id = arg_modem_id;

    parse_arg_eq_done(&pt); /* Do not do this before arg_data is parsed. */

    sal_memset(lb_counter, 0, sizeof(_dpp_lb_counter_info_t));
    if (CMD_OK != _dpp_lb_counter_info_get(unit, &lb_key, lb_counter)) {
        cli_out("fail in _dpp_lb_counter_info_get\n");
        rv = CMD_FAIL;
        goto done;
    }

    if (CMD_OK != _dpp_lb_counter_info_print(unit, &lb_key, lb_counter, ignore_zero)) {
        cli_out("fail in _dpp_lb_counter_info_print\n");
        rv = CMD_FAIL;
        goto done;
    }

done:
    if (lb_counter != NULL) {
        sal_free(lb_counter);
    }

    return rv;
}

STATIC cmd_t _dpp_lb_config_subcmds[] = {
        {"Show", cmd_dpp_lb_config_show, LB_CONFIG_SHOW_USAGE, NULL}
    };

STATIC cmd_t _dpp_lb_status_subcmds[] = {
        {"Show", cmd_dpp_lb_status_show, LB_STATUS_SHOW_USAGE, NULL}
    };

STATIC cmd_t _dpp_lb_counter_subcmds[] = {
        {"Show", cmd_dpp_lb_counter_show, LB_COUNTER_SHOW_USAGE, NULL}
    };

STATIC cmd_result_t
cmd_dpp_lb_config(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_lb_config_subcmds, COUNTOF(_dpp_lb_config_subcmds));
}

STATIC cmd_result_t
cmd_dpp_lb_status(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_lb_status_subcmds, COUNTOF(_dpp_lb_status_subcmds));
}

STATIC cmd_result_t
cmd_dpp_lb_counter(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_lb_counter_subcmds, COUNTOF(_dpp_lb_counter_subcmds));
}

STATIC cmd_t _dpp_lb_subcmds[] = {
        {"Config",  cmd_dpp_lb_config,  LB_CONFIG_USAGE, NULL},
        {"Status",  cmd_dpp_lb_status,  LB_STATUS_USAGE, NULL},
        {"Counter", cmd_dpp_lb_counter, LB_COUNTER_USAGE, NULL}
    };

cmd_result_t
cmd_dpp_lb(int unit, args_t *a)
{
    return subcommand_execute(unit, a, _dpp_lb_subcmds, COUNTOF(_dpp_lb_subcmds));
}

#endif

