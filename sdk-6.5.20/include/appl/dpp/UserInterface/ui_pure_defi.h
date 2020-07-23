/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_INCLUDED
/* { */
#define UI_PURE_DEFI_INCLUDED


#include <appl/diag/dpp/ui_defx.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fmf.h>
#include <appl/dpp/UserInterface/ui_consts.h>
#include <appl/diag/dpp/utils_memory.h>


typedef struct
{
    /*
     * Type of value loaded in 'in_val_chars' (numeric or symbolic):
     * VAL_NUMERIC, VAL_SYMBOL, VAL_TEXT, VAL_IP, VAL_NONE
     * 'VAL_NONE' indicates no value has been entered
     * for this entry.
     */
  int val_type ;
    /*
     * Numeric value related to string_value. Only relevant
     * for val_type = VAL_SYMBOL
     */
  int numeric_equivalent ;
  union
  {
    unsigned long  ulong_value ;
    const char     *string_value ;
    char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
    unsigned long  ip_value ;
  } value ;
} PARAM_VAL ;
/*
 * Container for a value typed in by the user. These ASCII
 * characetrs may constitute a numeric value (hex or integer)
 * or a string (symbolic name).
 */
typedef struct
{
    /*
     * Identifier of the parameter for which this value was entered.
     */
  const char *par_name ;
    /*
     * Index, of the parameter for which this value was entered,
     * in the array 'allowed_params' of the corresponding
     * subject.
     */
  unsigned int param_match_index ;
    /*
     * Input representing numeric value (hex or integer) or
     * symbolic name (which must be null terminated!).
     * Only loaded after full field has been typed in and
     * accepted.
     * Also contains type of value loaded in 'in_val_chars'
     * (numeric or symbolic):
     * VAL_NUMERIC, VAL_SYMBOL, VAL_TEXT, VAL_IP, VAL_NONE
     * 'VAL_NONE' indicates no value has been entered
     * for this entry.
     */
  PARAM_VAL param_value ;
    /*
     * Index, of the value specified in 'param_value',
     * in the array 'allowed_vals' of the corresponding
     * parameter.
     */
  unsigned int val_match_index ;
    /*
     * Index of value for specified 'par_name'. This takes care
     * of the case where there may be more than one value following
     * one parameter name. Counting starts at '0'.
     */
  unsigned int in_val_index ;
} PARAM_VAL_PAIR ;
typedef struct
{
    /*
     * Numeric value related to this symbolic value.
     */
  int numeric_equivalent ;
    /*
     * Description of this parameter, in plain text (for 'help'
     * purposes).
     */
  char *plain_text_help ;
    /*
     * Pointer to a function to call after the symbol value
     * has been accepted.
     */
  VAL_PROC_PTR val_checker ;
} VAL_SYMB_DESCRIPTOR ;
typedef struct
{
    /*
     * Maximal number of characters.
     */
  unsigned int max_num_chars ;
    /*
     * Description of this parameter, in plain text (for 'help'
     * purposes).
     */
  char *plain_text_help ;
} VAL_TEXT_DESCRIPTOR ;
typedef struct
{
    /*
     * Currently nothing.
     */
  int TBD_CURRENTLY_NOTHING;
} VAL_IP_DESCRIPTOR ;

/*
 *  Flag for the use of SOC_SAND_FAP20V ATM in UI
 */
/*
 *  Flag for the end of ChipSim in SOC_SAND_FAP20V
 */


/*
 * Bits and fields related to 'val_numeric_attributes'
 * element.
 */
/*
 * If 'true' then 'val_max' is meaningful.
 */
#define HAS_MIN_VALUE      BIT(0)
#define HAS_NO_MIN_VALUE   0
/*
 * If 'true' then 'val_min' is meaningful.
 */
#define HAS_MAX_VALUE      BIT(1)
#define HAS_NO_MAX_VALUE   0
/*
 * If 'true' then 'val_checker' is meaningful (i.e., A validation
 * routine should be called for this param value.
 */
/*
 * If 'true' then value must be positive.
 */
#define POSITIVE_VALUE  BIT(3)
/*
 * If 'true' then value must be negative.
 */
#define NEGATIVE_VALUE  BIT(4)
typedef struct
{
    /*
     * Attribute bits and fields. See list above.
     */
  unsigned long val_numeric_attributes ;
    /*
     * Maximal allowed value.
     */
  long val_max ;
    /*
     * Minimal allowed value.
     */
  long val_min ;
    /*
     * Number of times this value may be repeated (i.e.,
     * value is an array). Must be at least 1!
     */
  unsigned long max_num_repeated ;
    /*
     * Pointer to a function to call after every digit
     * is received and after the full numeric value
     * has been accepted.
     */
  VAL_PROC_PTR val_checker ;
} VAL_NUM_DESCRIPTOR ;
typedef union
{
  VAL_NUM_DESCRIPTOR  val_num_descriptor ;
  VAL_SYMB_DESCRIPTOR val_symb_descriptor ;
  VAL_TEXT_DESCRIPTOR val_text_descriptor ;
  VAL_IP_DESCRIPTOR   val_ip_descriptor ;
} VAL_DESCRIPTOR ;
/*
 * Values for 'val_type' element in PARAM_VAL_RULES
 */
#define VAL_END_OF_LIST    0
#define VAL_NONE           VAL_END_OF_LIST
#define VAL_NUMERIC        BIT(1)
#define VAL_SYMBOL         BIT(2)
#define VAL_TEXT           BIT(3)
#define VAL_IP             BIT(4)
#define VAL_TYPES_MASK     (VAL_NUMERIC | VAL_SYMBOL | VAL_TEXT | VAL_IP)
#define LAST_VAL_ON_LINE   BIT(16)
#define VAL_INDEX_ALL      BIT(17)
#define VAL_SYMBOL_LAST    (VAL_SYMBOL | LAST_VAL_ON_LINE)
#define VAL_TEXT_QUOTATION (BIT(18) | VAL_TEXT)
/*
 * Symbolic name assigned to numeric values
 * on an array of PARAM_VAL_RULES describing
 * allowed values per parameter.
 */
#define SYMB_NAME_NUMERIC SPECIAL_DOLLAR_STRING"numeric"
/*
 * Symbolic name assigned to free text values
 * on an array of PARAM_VAL_RULES describing
 * allowed values per parameter.
 */
#define SYMB_NAME_TEXT    SPECIAL_DOLLAR_STRING"text"
/*
 * Symbolic name assigned to IP address values
 * on an array of PARAM_VAL_RULES describing
 * allowed values per parameter.
 */
#define SYMB_NAME_IP      SPECIAL_DOLLAR_STRING"ipaddr"
typedef struct
{
    /*
     * Indicator on what type of param value it is.
     */
  int val_type ;
    /*
     * ASCII presentation of the symbolic value.
     * Note: Must be small letters only!
     * For numeric variables:
     * This is a dummey variable. Enables searching
     * array of 'PARAM_VAL_RULES' for name (string)
     * matching without special case for numeric
     * variable.
     */
  const char *symb_val ;
  VAL_DESCRIPTOR val_descriptor ;
} PARAM_VAL_RULES ;
/*
 * List of identifiers for parameters:
 */
#define PARAM_END_OF_LIST              0
#define PARAM_MEM_READ_ID              1
#define PARAM_MEM_WRITE_ID             2
#define PARAM_HIST_LEN_ID              3
#define PARAM_MEM_DISP_ID              4
#define PARAM_MEM_FORMAT_ID            5
#define PARAM_AB_ID                    6
#define PARAM_ABB_ID                   7
#define PARAM_AA_ID                    8
#define PARAM_BB_ID                    9
#define PARAM_HIST_GET_ID             10
#define PARAM_MEM_TYPE_ID                 11
#define PARAM_MEM_DATA_ID                 12
#define PARAM_MEM_MUL_DATA_ID             13
#define PARAM_MEM_LEN_ID                  14
#define PARAM_MEM_OFFSET_ID               15
#define PARAM_MEM_REPEAT_ID               16
#define PARAM_MEM_OVERRIDE_ID             17
#define PARAM_NVRAM_SHOW_ID               18
#define PARAM_NVRAM_BLOCK_ID              19
#define PARAM_NVRAM_UPDATE_ID             20
#define PARAM_NVRAM_DATA_CACHE_ID         22
#define PARAM_NVRAM_INST_CACHE_ID         23
#define PARAM_NVRAM_AUTO_FLASH_ID         24
#define PARAM_NVRAM_L_DEFAULTS_ID         25
#define PARAM_NVRAM_LOAD_METHOD_ID        26
#define PARAM_NVRAM_KERNEL_STARTUP_ID     27
#define PARAM_NVRAM_APP_SOURCE_ID         28
#define PARAM_NVRAM_TEST_AT_STARTUP_ID    29
#define PARAM_NVRAM_ETH_ADDRESS_ID        30
#define PARAM_NVRAM_CONSOLE_BAUD_ID       31
#define PARAM_NVRAM_DLD_FILE_NAME_ID             32
#define PARAM_NVRAM_LOC_IP_ADDR_ID               33
#define PARAM_NVRAM_GATEWAY_ADDR_ID              34
#define PARAM_NVRAM_DLD_HOST_ADDR_ID             35
#define PARAM_NVRAM_LOC_IP_MASK_ID               36
#define PARAM_VXSHELL_REBOOT_ID                  37
#define PARAM_VXSHELL_ROUTE_SHOW_ID              38
#define PARAM_VXSHELL_DEVS_ID                    39
#define PARAM_VXSHELL_LOGOUT_ID                  40
#define PARAM_VXSHELL_I_ID                       41
#define PARAM_GENERAL_SHOW_ID                    42
#define PARAM_GENERAL_TEMPERATURE_ID             43
#define PARAM_GENERAL_FLASH_ID                   44
#define PARAM_GENERAL_ERASE_ID                   45
#define PARAM_GENERAL_APP_FILE_ID                46
#define PARAM_GENERAL_TIME_FROM_STARTUP_ID       47
#define PARAM_GENERAL_RUNTIME_LOG_ID             48
#define PARAM_GENERAL_CLEAR_ID                   49
#define PARAM_GENERAL_DISPLAY_ID                 50
#define PARAM_VXSHELL_IOSFDSHOW_ID               51
#define PARAM_NVRAM_BOARD_SERIAL_NUM_ID          52
#define PARAM_NVRAM_BOARD_HW_VERSION_ID          53
#define PARAM_NVRAM_BOARD_TYPE_ID                54
#define PARAM_NVRAM_BOARD_DESCRIPTION_ID         55
#define PARAM_NVRAM_LINE_MODE_ID                 56
#define PARAM_NVRAM_PAGE_MODE_ID                 57
#define PARAM_NVRAM_PAGE_LINES_ID                58
#define PARAM_NVRAM_CONF_FROM_MD_ID              59
#define PARAM_NVRAM_FE_CONFIGURATION_ID          60
#define PARAM_NVRAM_STARTUP_COUNTER_ID           106
#define PARAM_NVRAM_ACTIVATE_WATCHDOG_ID         107
#define PARAM_NVRAM_WATCHDOG_PERIOD_ID           108
#define PARAM_GENERAL_WATCHDOG_ID                109
#define PARAM_GENERAL_EXERCISES_ID               111
#define PARAM_NVRAM_TELNET_TIMEOUT_STD_ID        112
#define PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID       113
#define PARAM_NVRAM_EVENT_DISPLAY_LVL_ID         114
#define PARAM_NVRAM_EVENT_NVRAM_LVL_ID           115
#define PARAM_NVRAM_ACTIVATE_DPSS_ID             116
#define PARAM_NVRAM_BASE_REGISTER_04_ID          117
#define PARAM_NVRAM_BASE_REGISTER_05_ID          118
#define PARAM_NVRAM_OPTION_REGISTER_04_ID        119
#define PARAM_NVRAM_OPTION_REGISTER_05_ID        120
#define PARAM_SNMP_LOAD_CONFIG_FILE_ID           121
#define PARAM_SNMP_DISP_FILE_ID                  122
#define PARAM_SNMP_DIR_ID                        123
#define PARAM_SNMP_SET_CMD_LINE_ID               124
#define PARAM_SNMP_DOWNLOAD_MIBS_ID              125
#define PARAM_SNMP_INCLUDE_NETSNMP_ID            126
#define PARAM_SNMP_SHUTDOWN_AGENT_ID             127
#define PARAM_MEM_XOR_ID                         128
#define PARAM_MEM_AND_ID                         129
#define PARAM_MEM_OR_ID                          130
#define PARAM_VXSHELL_PING_ID                    131
#define PARAM_VXSHELL_HOST_IP_ID                 132
#define PARAM_VXSHELL_PING_NOF_PKTS_ID           133
#define PARAM_VXSHELL_PING_OPT_NOHOST_ID         134
#define PARAM_VXSHELL_PING_OPT_DONTROUTE_ID      135
#define PARAM_VXSHELL_PING_OPT_SILENT_ID         136
#define PARAM_VXSHELL_PING_OPT_DEBUG_ID          137
#define PARAM_GENERAL_DOWNLOAD_ID                138
#define PARAM_GENERAL_DOWNLOAD_APP_ID            139
#define PARAM_GENERAL_DOWNLOAD_HOST_IP_ID        140
#define PARAM_GENERAL_DOWNLOAD_BOOT_ID           141
#define PARAM_GENERAL_FLASH_SHOW_ID              142
#define PARAM_SIM_START_ID                       143
#define PARAM_SIM_END_ID                         144
#define PARAM_SIM_INDIRECT_DELAY_ID              146
#define PARAM_SIM_PRINT_CLEAR_LOG_ID             147
#define PARAM_SIM_SET_SPY_LOW_MEM_LOG_ID         148
#define PARAM_SIM_CELL_TX_DELAY_ID               149
#define PARAM_SIM_CELL_RX_CNT_ID                 150
#define PARAM_SIM_TASK_SLEEP_TIME_ID             151
#define PARAM_SIM_TASK_WAKE_UP_ID                152
#define PARAM_SIM_CNTR_ENABLE_ID                 153
#define PARAM_SIM_INT_ENABLE_ID                  154
#define PARAM_SIM_TIME_MONITOR_PRINT_ID          155
#define PARAM_SIM_INTERRUPT_ASSERT_ID            156
#define PARAM_SIM_INTERRUPT_CNT_ID               157
#define PARAM_SIM_INT_MASK_ALL_ID                158
#define PARAM_GENERAL_PERIODIC_SUSPEND_TEST_ID   159
#define PARAM_NVRAM_IP_SOURCE_MODE_ID            160
#define PARAM_MEM_SILENT_ID                      161
#define PARAM_NVRAM_TCP_TIMEOUT_ID               166
#define PARAM_NVRAM_TCP_RETRIES_ID               170
#define PARAM_NVRAM_HOST_BOARD_TYPE_ID           171
#define PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID        172
#define PARAM_NVRAM_HOST_BOARD_DESC_ID           173
#define PARAM_NVRAM_HOST_BOARD_VER_ID            174
#define PARAM_NVRAM_HOST_BOARD_SN_ID             175
#define PARAM_NVRAM_BOARD_PARAMS_ID              176
#define PARAM_VXSHELL_IF_SHOW_ID                 177
#define PARAM_VXSHELL_I_NET_STAT_SHOW_ID         178
#define PARAM_VXSHELL_ARP_SHOW_ID                179
#define PARAM_VXSHELL_ICMP_STAT_SHOW_ID          180
#define PARAM_VXSHELL_MEM_SHOW_ID                181
#define PARAM_VXSHELL_SHOW_FREE_LIST_ID          182
#define PARAM_VXSHELL_PRINT_ERRNO_ID             183
#define PARAM_VXSHELL_CHECK_STACK_SHOW_ID        184
#define PARAM_MEM_VAL_READ_ID                    185
#define PARAM_MEM_VAL_VAL_ID                     186
#define PARAM_MEM_VAL_MASK_ID                    187
#define PARAM_MEM_VAL_TIMEOUT_ID                 188
#define PARAM_MEM_VAL_OPERATION_ID               189
#define PARAM_MEM_COUNTERS_ID                    190
#define PARAM_MEM_COUNTERS_GET_ID                191
#define PARAM_MEM_COUNTERS_CLEAR_ID              192
#define PARAM_MEM_BUFF_OFFSET_ID                 193
#define PARAM_GENERAL_CLI_ERROR_COUNTER_ID       194
#define PARAM_GENERAL_CLI_ID                     195
#define PARAM_GENERAL_CLI_DOWNLOAD_ID            196
#define PARAM_GENERAL_CLI_FILENAME_ID            197
#define PARAM_GENERAL_CLI_DIR_ID                 198
#define PARAM_GENERAL_CLI_ERASE_ID               199
#define PARAM_GENERAL_CLI_DISPLAY_ID             200
#define PARAM_GENERAL_CLI_RUN_ID                 201
#define PARAM_GENERAL_CLI_EXEC_TIME_ID           202
#define PARAM_MEM_MUL_WRITE_ID                   203
#define PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID        204
#define PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID          205
#define PARAM_GENERAL_CLI_DOWNLOAD_HOST_IP_ID   206

#define PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID         207
#define PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID 208
#define PARAM_NVRAM_STARTUP_DEMO_MODE_ID             209
#define PARAM_GENERAL_VERSIONS_ID                    210
#define PARAM_GENERAL_TRACE_ID                       211
#define PARAM_GENERAL_TRACE_DISPLAY_ID               212
#define PARAM_GENERAL_TRACE_DISPLAY_N_CLEAR_ID       213
#define PARAM_GENERAL_REMARK_ID                      214
#define PARAM_VXSHELL_REBOOT_SOFT_ID                 215
#define PARAM_MEM_NUM_OF_TIMES_ID                    216
#define PARAM_MEM_PAUSE_AFTER_READ_ID                217
#define PARAM_MEM_REMOTE_OPERATION_ID                218
#define PARAM_NVRAM_FAP10M_RUN_MODE_ID               219
#define PARAM_GENERAL_HOST_BOARD_SN_ID               220
#define PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID      221
#define PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID        222
#define PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID 223
#define PARAM_GENERAL_DEMO_ID                        224
#define PARAM_GENERAL_DEMO_DISPLAY_NETWORK_ID        225
#define PARAM_GENERAL_DEMO_SHOW_FE_LINK_ID           226
#define PARAM_GENERAL_DEMO_CRATE_TYPE_ID             227
#define PARAM_GENERAL_DEMO_LINE_CARD_SLOT_ID         228
#define PARAM_GENERAL_DEMO_FABRIC_CARD_SLOT_ID       229
#define PARAM_GENERAL_DEMO_FAP_LINK_ID               230
#define PARAM_MEM_ADDRESS_IN_LONGS_ID                231
#define PARAM_GENERAL_AUX_INPUT_ID                   232
#define PARAM_GENERAL_PRINT_STATUS_ID                233
#define PARAM_GENERAL_AUX_INPUT_RESTART_ID           234
#define PARAM_GENERAL_FREE_UI_BITS_ID                235
#define PARAM_GENERAL_FPGA_INDIRECT_ID               236
#define PARAM_GENERAL_FPGA_IND_READ_ID               237
#define PARAM_GENERAL_FPGA_IND_WRITE_ID              238
#define PARAM_GENERAL_FPGA_IND_SUB_BLOCK_READ_ID     239
#define PARAM_GENERAL_FPGA_IND_SUB_BLOCK_WRITE_ID    240
#define PARAM_GENERAL_FPGA_IND_FROM_ELEMENT_ID       241
#define PARAM_GENERAL_FPGA_IND_TO_ELEMENT_ID         242
#define PARAM_GENERAL_FPGA_IND_ELEMENT_NUMBER_ID     243
#define PARAM_GENERAL_FPGA_IND_WRITE_DATA_ID         244
#define PARAM_GENERAL_FPGA_IND_AUTO_CLEAR_ID         245

#define PARAM_GENERAL_FPGA_RATE_ID                   246
#define PARAM_GENERAL_FPGA_RATE_START_ID             247
#define PARAM_GENERAL_FPGA_RATE_STOP_ID              248
#define PARAM_GENERAL_FPGA_RATE_CONTINUOUS_ID        249
#define PARAM_GENERAL_FPGA_RATE_COLLECTION_RATE_ID   250
#define PARAM_GENERAL_FPGA_RATE_REPORT_RATE_ID       251
#define PARAM_GENERAL_FPGA_RATE_HOST_IP_ID           252
#define PARAM_GENERAL_FPGA_RATE_GET_CURRENT_ID       253

#define PARAM_GENERAL_DCL_ID                         254
#define PARAM_GENERAL_HOST_FILE_ID                   255
#define PARAM_GENERAL_HOST_FILE_SET_ID               256
#define PARAM_GENERAL_HOST_FILE_DELETE_ID            257
#define PARAM_GENERAL_HOST_FILE_HOST_IP_ID           258
#define PARAM_GENERAL_HOST_FILE_DEST_HOST_ID         259
#define PARAM_GENERAL_HOST_FILE_FIRST_FILE_ID        260
#define PARAM_GENERAL_HOST_FILE_LAST_FILE_ID         261

#define PARAM_GENERAL_MULTI_MEM_ID                   262
#define PARAM_GENERAL_MULTI_MEM_READ_ID              263
#define PARAM_GENERAL_MULTI_MEM_WRITE_ID             264
#define PARAM_GENERAL_MULTI_MEM_ADDRESS_ID           265
#define PARAM_GENERAL_MULTI_MEM_FORMAT_ID            266
#define PARAM_GENERAL_MULTI_MEM_REPEAT_ID            267
#define PARAM_GENERAL_MULTI_MEM_DATA_ID              268

#define PARAM_NVRAM_FRONT_BOARD_TYPE_ID              270
#define PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID           271
#define PARAM_NVRAM_FRONT_BOARD_DESC_ID              272
#define PARAM_NVRAM_FRONT_BOARD_VER_ID               273
#define PARAM_NVRAM_FRONT_BOARD_SN_ID                274

#define PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID            275
#define PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID         276
#define PARAM_NVRAM_HOST_DB_BOARD_DESC_ID            277
#define PARAM_NVRAM_HOST_DB_BOARD_VER_ID             278
#define PARAM_NVRAM_HOST_DB_BOARD_SN_ID              279

#define PARAM_NVRAM_EVENT_SYSLOG_LVL_ID              280
#define PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID   290
#define PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID          291
#define PARAM_NVRAM_USR_APP_FLAVOR_ID                292
#define PARAM_GENERAL_GET_BOARD_INFO                 293

#define PARAM_NVRAM_CLK_MIRROR_DATA_ID              304
#define PARAM_GENERAL_GENERIC_TEST_ID               305
#define PARAM_GENERAL_GENERIC_PARAM_0_ID            294
#define PARAM_GENERAL_GENERIC_PARAM_1_ID            295
#define PARAM_GENERAL_GENERIC_PARAM_2_ID            296
#define PARAM_GENERAL_GENERIC_PARAM_3_ID            297
#define PARAM_GENERAL_GENERIC_PARAM_4_ID            298
#define PARAM_GENERAL_GENERIC_PARAM_5_ID            299
#define PARAM_GENERAL_GENERIC_PARAM_6_ID            300
#define PARAM_GENERAL_GENERIC_PARAM_7_ID            301
#define PARAM_GENERAL_GENERIC_PARAM_8_ID            302
#define PARAM_GENERAL_GENERIC_PARAM_9_ID            303


#define PARAM_NVRAM_EEPROM_HOST_DB_CARD_TYPE_ID       311
#define PARAM_GENERAL_PETRA_TEST_CHIP_PROGRAM_LOAD_ID 312
#define PARAM_GENERAL_CHIP_ID_ID                      313
#define PARAM_GENERAL_DSDA_INIT                       314


#define PARAM_FE200_START_RANGE_ID                   320
#define PARAM_FE200_END_RANGE_ID                     599

#define PARAM_PSS_START_RANGE_ID                      600
#define PARAM_PSS_END_RANGE_ID                        799

#define PARAM_FAP10M_START_RANGE_ID                  800
#define PARAM_FAP10M_END_RANGE_ID                    1399

#define PARAM_LINE_CARD_START_RANGE_ID               1400
#define PARAM_LINE_CARD_END_RANGE_ID                 1799

#define PARAM_DEMO_START_RANGE_ID                    1800
#define PARAM_DEMO_END_RANGE_ID                      2099

#define PARAM_SWEEP_APP_START_RANGE_ID                2100
#define PARAM_SWEEP_APP_END_RANGE_ID                  2299

#define PARAM_FAP20V_START_RANGE_ID                   2300
#define PARAM_FAP20V_END_RANGE_ID                     4000

#define PARAM_LINE_TGS_START_RANGE_ID                 4100
#define PARAM_LINE_TGS_END_RANGE_ID                   4500

#define PARAM_FMF_START_RANGE_ID                      4600
#define PARAM_FMF_END_RANGE_ID                        4800

#define PARAM_SKT_START_RANGE_ID                      5001
#define PARAM_SKT_END_RANGE_ID                        5099

#define PARAM_CSYS_START_RANGE_ID                     5100
#define PARAM_CSYS_END_RANGE_ID                       7000

#define PARAM_LINE_GFA_START_RANGE_ID                 7100
#define PARAM_LINE_GFA_END_RANGE_ID                   7300

#define PARAM_DHRP_API_START_RANGE_ID                 8000
#define PARAM_DHRP_API_END_RANGE_ID                   8099

#define PARAM_VXSHELL_TASK_ID                        8310
#define PARAM_VXSHELL_TASK_OPT_DELETE_ID             8311
#define PARAM_VXSHELL_TASK_OPT_RESTART_ID            8312
#define PARAM_VXSHELL_TASK_OPT_RESUME_ID             8313
#define PARAM_VXSHELL_TASK_OPT_SUSPEND_ID            8314

#define PARAM_SAND_TRACE_PRINT_ID                    8720
#define PARAM_SAND_TRACE_CLEAR_ID                    8721
#define PARAM_SAND_TCM_PRINT_DELTA_LIST_ID           8722
#define PARAM_SAND_OS_RESOURCE_PRINT_ID              8723
#define PARAM_SAND_STATUS_ID                         8725
#define PARAM_SAND_TRANSLATE_ID                      8726
#define PARAM_SAND_TRANSLATE_TO_PROC_ID_ID           8727
#define PARAM_SAND_TCM_ID                            8728
#define PARAM_SAND_DEVICE_TYPE_ID                    8729
#define PARAM_SAND_PRINT_WHEN_WRITING_ID             8730
#define PARAM_SAND_PRINT_WHEN_WRITING_EN_ID          8731
#define PARAM_SAND_PRINT_WHEN_WRITING_DIS_ID         8732
#define PARAM_SAND_PRINT_WHEN_WRITING_IND_ID         8733
#define PARAM_SAND_PRINT_WHEN_WRITING_ASIC_STYLE_ID  8735

#define PARAM_SAND_CALL_SYSTEM_ID                    8740

#define PARAM_GENERAL_REGRESSION_TEST_ID             9000
#define PARAM_GENERAL_REGRESSION_ALL_ID              9001
#define PARAM_GENERAL_REGRESSION_AUTOLEARN_ID        9002
#define PARAM_GENERAL_REGRESSION_SHORT_ID            9003
#define PARAM_GENERAL_REGRESSION_FULL_ID             9004

#define PARAM_GENERAL_STOP_NETWORK_LEARNING          9007

#define PARAM_GENERAL_FLASH_DFFS_ID                  9010
#define PARAM_GENERAL_FLASH_DFFS_DIR_ID              9011
#define PARAM_GENERAL_FLASH_DFFS_DLD_ID              9012
#define PARAM_GENERAL_FLASH_DFFS_DEL_ID              9013
#define PARAM_GENERAL_FLASH_DFFS_PRINT_ID            9014
#define PARAM_GENERAL_FLASH_DFFS_ADD_COMMENT_ID      9015
#define PARAM_GENERAL_FLASH_DFFS_SHOW_COMMENT_ID     9016
#define PARAM_GENERAL_FLASH_DFFS_COMMENT_ID          9017
#define PARAM_GENERAL_FLASH_DFFS_ADD_ATTR_ID         9018
#define PARAM_GENERAL_FLASH_DFFS_DATE_ID             9019
#define PARAM_GENERAL_FLASH_DFFS_VER_ID              9020
#define PARAM_GENERAL_FLASH_DFFS_ATTR1_ID            9021
#define PARAM_GENERAL_FLASH_DFFS_ATTR2_ID            9022
#define PARAM_GENERAL_FLASH_DFFS_ATTR3_ID            9023
#define PARAM_GENERAL_FLASH_DFFS_NAME_ID             9024
#define PARAM_GENERAL_FLASH_DFFS_PRINT_SIZE_ID       9025
#define PARAM_GENERAL_FLASH_DFFS_DIR_FORMAT_ID       9026
#define PARAM_GENERAL_FLASH_DFFS_DIAGNOSTICS_ID      9027

#define PARAM_LINE_TEVB_START_RANGE_ID               9100
#define PARAM_LINE_TEVB_END_RANGE_ID                 9199
#define PARAM_DHRP_START_RANGE_ID                    9200
#define PARAM_DHRP_END_RANGE_ID                      9399
#define PARAM_DIAG_START_RANGE_ID                    9400
#define PARAM_DIAG_END_RANGE_ID                      9499

#define PARAM_ACCESS_TIMNA_START_RANGE_ID            9700
#define PARAM_ACCESS_TIMNA_END_RANGE_ID              9999

#define PARAM_TIMNA_API_START_RANGE_ID               10000
#define PARAM_TIMNA_API_END_RANGE_ID                 11999

#define PARAM_FAP21V_API_START_RANGE_ID              12000
#define PARAM_FAP21V_API_END_RANGE_ID                13999

#define PARAM_PETRA_PP_API_START_RANGE_ID            14000
#define PARAM_PETRA_PP_API_END_RANGE_ID              15999

#define PARAM_FAP21V_ACC_START_RANGE_ID              16000
#define PARAM_FAP21V_ACC_END_RANGE_ID                17899

#define PARAM_FE600_BSP_START_RANGE_ID               17900
#define PARAM_FE600_BSP_END_RANGE_ID                 17950

#define PARAM_FAP21V_GFA_START_RANGE_ID              18000

#define PARAM_FAP21V_APP_START_RANGE_ID              19000

#define PARAM_SWEEPT20_API_START_RANGE_ID            20000
#define PARAM_SWEEPT20_API_END_RANGE_ID              21999

#define PARAM_PETRA_API_START_RANGE_ID               22000
#define PARAM_PETRA_API_END_RANGE_ID                 23999

#define PARAM_PTG_START_RANGE_ID                     24000

#define PARAM_PETRA_PP_ACC_START_RANGE_ID            25000

#define PARAM_PETRA_ACC_START_RANGE_ID               26000
#define PARAM_PETRA_ACC_END_RANGE_ID                 27999

#define PARAM_PETRA_PP_SWP_START_RANGE_ID            28000
#define PARAM_PETRA_PP_SWP_END_RANGE_ID              28999

#define PARAM_PETRA_GFA_START_RANGE_ID               29000

#define PARAM_FE600_API_START_RANGE_ID               30000
#define PARAM_FE600_API_END_RANGE_ID                 30499

#define PARAM_SWP_P_START_RANGE_ID                   31000
#define PARAM_SWP_P_END_RANGE_ID                     31999

#define PARAM_GSA_START_RANGE_ID                     32000

#define PARAM_GSA_PETRA_START_RANGE_ID               33000

#define PARAM_T20E_ACC_START_RANGE_ID                34000
#define PARAM_T20E_ACC_END_RANGE_ID                  35999

#define SOC_PARAM_PPD_API_START_RANGE_ID                 36000
#define SOC_PARAM_PPD_API_END_RANGE_ID                   37999

#define PARAM_OAM_ACC_START_RANGE_ID                 38000
#define PARAM_OAM_ACC_END_RANGE_ID                   39999

#define PARAM_OAM_API_START_RANGE_ID                 40000
#define PARAM_OAM_API_END_RANGE_ID                   41999

#define PARAM_PPA_API_START_RANGE_ID                 42000
#define PARAM_PPA_API_END_RANGE_ID                   42999

#define PARAM_TMD_API_START_RANGE_ID                 43000
#define PARAM_TMD_API_END_RANGE_ID                   44999

#define PARAM_PB_API_START_RANGE_ID                  45000
#define PARAM_PB_API_END_RANGE_ID                    45999

#define PARAM_PB_PP_ACC_START_RANGE_ID               46000
#define PARAM_PB_PP_ACC_END_RANGE_ID                 47999

#define PARAM_PB_PP_ACC2_START_RANGE_ID              48000
#define PARAM_PB_PP_ACC2_END_RANGE_ID                48999

#define PARAM_PCP_API_START_RANGE_ID                 49000


/*
 * Values for parameter 'default_care':
 */
#define HAS_DEFAULT       BIT(0)
#define HAS_NO_DEFAULT        0
#define HAS_DEFAULT_MASK  BIT(0)
#define MUST_APPEAR       BIT(1)
#define MAY_NOT_APPEAR        0
#define MUST_APPEAR_MASK  BIT(1)
/*
 * Maximal value allowed for ordinal numbers on parameter description
 * within 'PARAM' structures in ui_rom_defi.h.
 * Make sure to give it a sensible value and NOT use bits 31,30,29
 * Parameters marked as LAST_ORDINAL are treated differently from other
 * parameters marked with some positive ordinal: They may all be entered
 * without interfering with each other, on the same line. (For other
 * parameters, with smaller 'ordinal', only one may be selected and
 * appear on the line.
 */
#define LAST_ORDINAL               1000
/*
 * Number of elements in the 'mutex_control' array which is used
 * for defining various 'paths' along one CLI line.
 */
#define NUM_ELEMENTS_MUTEX_CONTROL 16
typedef   struct
{
  int          param_id ;
  const char   *par_name ;
  PARAM_VAL_RULES *allowed_vals ;
    /*
     * Number of elements in 'allowed_vals' array.
     */
  unsigned int num_allowed_vals ;
    /*
     * 2 Control flags.
     *  1. Says if the parameter has/hasn't a default value.
     *     Only if it has - then 'default_val' is meaningful.
     *     The possible values for this are 'HAS_DEFAULT' or 'HAS_NO_DEFAULT' (defined above)
     *  2. Says if the parameter must apear or not.
     *     This refers to the subtree defined by 'mutex_control' (see below),
     *      Ex: If the parameter is defined for BIT(3), and here you specify
     *          that it MUST apear, it means that only for the 'subdirectory'/'subtree'/group
     *          of parameters that are defined for BIT(3) - this parameter MUST appear.
     *          In another group like BIT(4) - it may not appear.
     *     The possible values for this are 'MUST_APPEAR' or 'MAY_NOT_APPEAR' (defined above)
     * So you put in this flag bitwise OR between the 2 controls.
     * Exapmle: HAS_DEFAULT | MAY_NOT_APPEAR
     */
  int          default_care ;
  PARAM_VAL    *default_val ;
    /*
     * Number of numeric values in array 'allowed_vals'.
     */
  unsigned int num_numeric_vals ;
    /*
     * Number of symbolic values in array 'allowed_vals'.
     */
  unsigned int num_symbolic_vals ;
    /*
     * Number of free text values in array 'allowed_vals'.
     */
  unsigned int num_text_vals ;
    /*
     * Number of IP address values in array 'allowed_vals'.
     */
  unsigned int num_ip_vals ;
    /*
     * Index of numeric value (if there is any) in
     * array 'allowed_vals'.
     */
  unsigned int numeric_index ;
    /*
     * Index of free text value (if there is any) in
     * array 'allowed_vals'.
     */
  unsigned int text_index ;
    /*
     * Index of IP address value (if there is any) in
     * array 'allowed_vals'.
     */
  unsigned int ip_index ;
  const char   *param_short_help ;
  const char   *param_detailed_help ;
    /*
     * Help for variables attached to this parameter.
     */
  const char   *vals_detailed_help ;
    /*
     * Mutual exclusion and grouping syntax control.
     * Up to 32 groups specified by bits.
     * If a parameter has specific
     * bits set then other parameters, with none of
     * these bits set, may not be on the same line.
     * Also, if specific bits are set for all
     * parameters on a specific command line then
     * all other allowed parameters with any of
     * these bits set are also required (provided
     * they do not have default values).
     */
  unsigned long mutex_control[NUM_ELEMENTS_MUTEX_CONTROL] ;
    /*
     * Order control for parameters on command line.
     * If this value is 'zero' then this parameter
     * string is not in the "ordinal" game and may
     * be found anywhere on the line.
     * See remark concerning LAST_ORDINAL above!
     * Otherwise, parameters with a smaller "ordinal"
     * must come before parameters with a larger
     * "ordinal" (unless, of course, they do not have
     * to be on the line at all).
     */
  unsigned int  ordinal ;
    /*
     * Pointer to a function to call after full
     * symbolic parameter has been accepted.
     */
  VAL_PROC_PTR val_checker ;
} PARAM ;

typedef struct
{
    /*
     * Buffer containing whatever has been input
     * so far plus spare.
     */
  char          line_buf[MAX_CHARS_ON_LINE + 2] ;
    /*
     * Maximal number of charachters allowed on
     * prompt line (without spare).
     */
  unsigned int  max_chars_on_line ;
    /*
 */
  unsigned int  max_chars_on_screen_line ;
    /*
     * Total size of currently allocated 'line_buf'.
     */
  unsigned long line_buf_size ;
    /*
     * Index of next input ASCII character to go into 'line_buf'.
     */
  unsigned long line_buf_index ;
    /*
     * Number of ASCII character already loaded into 'line_buf'.
     * If there were no 'insert_mode' then this would be the
     * same number as 'line_buf_index'.
     */
  unsigned long num_loaded ;
    /*
     * Currently used prompt.
     */
  char          *prompt_string ;
    /*
     * Index, in 'line_buf', where currently entered field has started
     * (for 'subject', this is alwasy zero).
     */
  unsigned int  start_index_this_field ;
    /*
     * Number of characters already entered in currently
     * handled field
     * (for 'subject', this is alwasy equal to 'num_loaded').
     */
  unsigned int  num_loaded_this_field ;
    /*
     * Processing state for current line.
     */
  int           proc_state ;
    /*
     * Processed line section. Filled on line and
     * checked after CR has been hit.
     */
    /*
     * Identifiers of subject on current line.
     */
  char                *line_subject_name ;
  struct subject_str  *line_subject ;
    /*
     * Indicator: Subject already entered or not.
     */
  int           subject_entered ;
    /*
     * List of parameter values on current line.
     * Note that if a single parameter gets more than one value
     * then there will be a few entries with the same 'par_name'.
     */
  PARAM_VAL_PAIR  param_val_pair[MAX_PARAMS_VAL_PAIRS_ON_LINE] ;
    /*
     * Index of currently entered param_val_pair entry (which contains
     * the pair: param ID and value).
     */
  unsigned int  param_index ;
    /*
     * Indicator: Parameter name string already entered or not.
     */
  int           param_string_entered ;
    /*
     * Index of the parameter 'param_string',
     * in the array 'allowed_params' of the corresponding
     * subject.
     */
  unsigned int  current_param_match_index ;
    /*
     * Indicator: Number of parameter values already entered
     * for the last entered parameter name.
     */
  unsigned int  num_param_values ;
    /*
     * List of parameters (identified by 'par_name', char *)
     * on current line.
     */
  PARAM         *parameters[MAX_PARAM_NAMES_ON_LINE] ;
    /*
     * Indicator: Number of parameter names already entered
     * for this subject on this line.
     */
  unsigned int   num_param_names ;
    /*
     * Indicator: Ordinal number of input character
     * at which a special action is scheduled. Related
     * to the static variable 'num_chars_handled'
     * in procedure 'handle_next_char'.
     */
  unsigned int   scheduled_num_char ;
    /*
     * Indicator:
     *   Quotation mark status - User is entering text with
     *   embedded spaces. This state ends when a second quotation
     *   mark is entered.
     */
  int           quotation_mark_entered ;
} CURRENT_LINE ;
/*
 * List of identifiers for subjects:
 */
typedef enum
{
  SUBJECT_END_OF_LIST = 0,
  SUBJECT_MEM_ID,
  SUBJECT_HISTORY_ID,
  SUBJECT_NEW_01_ID,
  SUBJECT_NEW_02_ID,
  SUBJECT_NVRAM_ID,
  SUBJECT_HELP_ID,
  SUBJECT_GENERAL_ID,
  SUBJECT_VX_SHELL_ID,
  SUBJECT_FREE_PLACE_1_ID,
  SUBJECT_FREE_PLACE_2_ID,
  SUBJECT_SNMP_ID,
  SUBJECT_FAP10M_SERVICES_ID,
  SUBJECT_SIM_SERVICES_ID,
  SUBJECT_FE200_SERVICES_ID,
  SUBJECT_PSS_SERVICES_ID,
  SUBJECT_SAND_SERVICES_ID,
  SUBJECT_LINE_CARD_SERVICES_ID,
  SUBJECT_NEGEV_DEMO_SERVICES_ID,
  SUBJECT_GOBI_DEMO_SERVICES_ID,
  SUBJECT_DEMO_SERVICES_ID,
  SUBJECT_FMF_SERVICES_ID,
  SUBJECT_GFA_SERVICES_ID,
  SUBJECT_FAP20V_SERVICES_ID,
  SUBJECT_FAP20V_SWEEP_APP_SERVICES_ID,
  SUBJECT_TGS_SERVICES_ID,
  SUBJECT_CSYS_SERVICES_ID,
  SUBJECT_SKT_ID,
  SUBJECT_FAP20V_B_SERVICES_ID,
  SUBJECT_FAP20V_SWEEP_APP_B_SERVICES_ID,
  SUBJECT_TEVB_SERVICES_ID,
  SUBJECT_DHRP_SERVICES_ID,
  SUBJECT_DIAG_SERVICES_ID,
  SUBJECT_DHRP_API_SERVICES_ID,
  SUBJECT_TIMNA_ACCESS_SERVICES_ID,
  SUBJECT_TIMNA_API_SERVICES_ID,
  SUBJECT_FAP21V_API_SERVICES_ID,
  SUBJECT_PETRA_API_SERVICES_ID,
  SUBJECT_SWP_PETRA_SERVICES_ID,
  SUBJECT_PETRA_PP_API_SERVICES_ID,
  SUBJECT_FAP21V_ACC_SERVICES_ID,
  SUBJECT_FAP21V_GFA_SERVICES_ID,
  SUBJECT_FAP21V_APP_SERVICES_ID,
  SUBJECT_TIMNA_APP_SERVICES_ID,
  SUBJECT_PTG_SERVICES_ID,
  SUBJECT_PETRA_ACC_SERVICES_ID,
  SUBJECT_PETRA_PP_ACC_SERVICES_ID,
  SUBJECT_PETRA_PP_SWP_SERVICES_ID,
  SUBJECT_PETRA_PP_GSA_SERVICES_ID,
  SUBJECT_PETRA_GSA_SERVICES_ID,
  SUBJECT_PETRA_APP_SERVICES_ID,
  SUBJECT_PETRA_PP_APP_SERVICES_ID,
  SUBJECT_PETRA_GFA_SERVICES_ID,
  SUBJECT_PB_API_SERVICES_ID,
  SUBJECT_PB_PP_ACC_SERVICES_ID,
  SUBJECT_PB_PP_ACC2_SERVICES_ID,
  SUBJECT_PCP_API_SERVICES_ID,
  SUBJECT_FE600_BSP_SERVICES_ID,
  SUBJECT_FE600_API_SERVICES_ID,
  SUBJECT_T20E_ACC_SERVICES_ID,
  SUBJECT_PPD_API_SERVICES_ID,
  SUBJECT_OAM_ACC_SERVICES_ID,
  SUBJECT_OAM_API_SERVICES_ID,
  SUBJECT_TMD_API_SERVICES_ID
} SUBJECT_SERVICES_IDS;

/*
 * Pointer to a procedure to activate one a line
 * conaining related subject has been checked
 * and accepted.
 */
typedef int (*SUBJECT_PROC)(CURRENT_LINE *,CURRENT_LINE **) ;
typedef struct subject_str
{
  int          subject_id ;
  char         *subj_name ;
  PARAM        *allowed_params ;
  unsigned int num_allowed_params ;
    /*
     * Pointer to ascii array (null terminated)containing
     * the names of all parameters, for this subject,
     * separated by CR/LF.
     */
  char         *params_ascii_list ;
  char         *subj_short_help ;
  char         *subj_detailed_help ;
    /*
     * Number of parameters, within the allowed list, which have
     * a default value.
     */
  unsigned int num_params_with_default ;
    /*
     * Number of parameters, within the allowed list,
     * which do not have a default value.
     */
  unsigned int num_params_without_default ;
    /*
     * Pointer to a procedure to call after the full
     * line has been accepted (to do the action related
     * to this subject).
     */
  SUBJECT_PROC subject_handler ;
    /*
     * Flag. If 'true' then subject may be the only text
     * on the command line for meaningful processing to be
     * carried out.
     */
  unsigned int may_stand_alone ;
} SUBJECT ;
/*
 * Security levels.
 */
#define UNDEFINED_LEVEL -1
/*
 * Security level to start with. On final version, this
 * should be UNDEFINED_LEVEL.
 */
typedef struct
{
  int level ;
} SECURITY ;
/*
 * UI processing state of current line.
 */
#define EXPECTING_PASSWORD      BIT(0)
#define RECEIVING_PASSWORD      BIT(1)
#define EXPECTING_SUBJECT       BIT(2)
#define RECEIVING_SUBJECT       BIT(3)
#define EXPECTING_PARAM_STRING  BIT(4)
#define RECEIVING_PARAM_STRING  BIT(5)
#define EXPECTING_PARAM_VAL     BIT(6)
#define RECEIVING_PARAM_VAL     BIT(7)
#define INSERT_MODE             BIT(8)
#define EXPECTING_ENTER         BIT(9)
/*
 * State to start with. On final version, this
 * should be ECPECTING_PASSWORD.
 */
#define STARTER_STATE         EXPECTING_SUBJECT
/*
 * Prompt to display at the beginning of a line.
 */
#define PROMPT_STRING "Dune >"
/*
 * Definitions related to 'select_learning_mode' parameter
 * in subject "pss" (under "vlan").
 * {
 */
#define VLAN_INDIVIDUAL_EQUIVALENT 1
#define VLAN_SHARED_EQUIVALENT     2
/*
 * }
 */
/*
 * Definitions related to 'memory_type' parameter
 * in subject "pss" (under "diagnostics test_mem").
 * {
 */
#define BUFFER_DRAM_EQUIVALENT   1
#define WIDE_SRAM_EQUIVALENT     2
#define NARROW_SRAM_EQUIVALENT   3
#define FLOW_DRAM_EQUIVALENT     4
#define MAC_TBL_MEM_EQUIVALENT   5
#define VLAN_TBL_MEM_EQUIVALENT  6
/*
 * }
 */
/*
 * Definitions related to 'test_profile' parameter
 * in subject "pss" (under "diagnostics test_mem").
 * {
 */
#define INCREMENTAL_EQUIVALENT   1
#define BIT_TOGGLE_EQUIVALENT    2
#define AA_55_EQUIVALENT         3
/*
 * }
 */
/*
 * Definitions related to 'cmd_type' parameter
 * in subject "pss" (under "user_tx_table add").
 * {
 */
#define GT_CMD_TX_BY_VLAN_EQUIVALENT  1
#define GT_CMD_TX_BY_IF_EQUIVALENT    2
#define GT_CMD_TX_BY_LPORT_EQUIVALENT 3
/*
 * }
 */
/*
 * Definitions related to 'encapsulation' parameter
 * in subject "pss" (under "user_tx_table add").
 * {
 */
#define GT_REGULAR_ENCAPSULATION_EQUIVALENT  1
#define GT_CONTROL_ENCAPSULATION_EQUIVALENT  2
/*
 * }
 */
/*
 * Definitions related to 'num_loop_cycles' parameter
 * in subject "pss" (under "user_tx_table set_tx_mode").
 * {
 */
#define GT_INFINITE_CYCLES_EQUIVALENT        -1
/*
 * }
 */
/*
 * Definitions related to 'buffer_mode' parameter
 * in subject "pss" (under "user_rx_table set_rx_mode").
 * {
 */
#define GT_CYCLIC_BUFFER_EQUIVALENT                1
#define GT_USE_ONCE_BUFFER_EQUIVALENT              2
/*
 * }
 */
/*
 * Definitions related to 'start_tx' parameter
 * in subject "pss" (under "user_tx_table tx_action").
 * {
 */
#define GT_START_TX_EQUIVALENT               1
#define GT_STOP_TX_EQUIVALENT                2
/*
 * }
 */
/*
 * Definitions related to 'start_rx' parameter
 * in subject "pss" (under "user_rx_table rx_action").
 * {
 */
#define GT_START_RX_EQUIVALENT               1
#define GT_STOP_RX_EQUIVALENT                2
/*
 * }
 */
/*
 * Definitions related to 'tot_num_packets' parameter
 * in subject "pss" (under "user_tx_table set_tx_mode").
 * {
 */
#define GT_AS_SPECIFIED_PER_ENTRY_EQUIVALENT -1
/*
 * }
 */
/*
 * Definitions related to 'data' parameter
 * in subject "pss" (under "user_tx_table add").
 * {
 */
/*
 * Maximal number of data items allowed for the user to
 * specify for a message to transmit.
 */
#define PSS_DATA_NUM_VALS  30
/*
 * }
 */
/*
 * Definitions related to 'port_type' parameter
 * in subject "pss" (under "vlan").
 * {
 */
#define PORT_STANDARD_EQUIVALENT 1
#define PORT_TRUNK_EQUIVALENT    2
/*
 * }
 */
/*
 * Definitions related to 'exercises' parameter
 * ('general' subject).
 * {
 */
#define EVENT_STORM_EQUIVALENT                               1
#define ERR_WITHIN_ERR_EQUIVALENT                            2
#define EVENT_AGING_EQUIVALENT                               3
#define FATAL_ERROR_EQUIVALENT                               4
#define EVENT_DOUBLE_EQUIVALENT                              5
#define EVENT_ALARM_ON_EQUIVALENT                            6
#define EVENT_ALARM_OFF_EQUIVALENT                           7
#define EVENT_PRINT_TASK_ON_EQUIVALENT                       8
#define EVENT_PRINT_TASK_OFF_EQUIVALENT                      9
#define LOAD_REAL_TIME_TRACE_EQUIVALENT                      10
#define CALL_MAIN_DPSS_INTERRUPT_EQUIVALENT                  11
#define CAUSE_SOFTWARE_EMULATION_EXCEPTION_EQUIVALENT        12
#define CALL_SOFTWARE_EMULATION_EXCEPTION_HANDLER_EQUIVALENT 13
#define CALL_ALIGNMENT_EXCEPTION_HANDLER_EQUIVALENT          14
#define CALL_DSI_EXCEPTION_HANDLER_EQUIVALENT                15
#define CALL_ISI_EXCEPTION_HANDLER_EQUIVALENT                16
#define CALL_CHAR_BUFF_MODULE_TEST                           17
#define INIT_FPGA_REAL_TIME_COLLETION                        18
#define CALL_FPGA_COLLETION_ISR                              19
#define START_FPGA_A_COLLETION_PROCESS                       20
#define STOP_FPGA_B_COLLETION_PROCESS                        21
#define START_FPGA_B_COLLETION_PROCESS                       22
#define STOP_FPGA_A_COLLETION_PROCESS                        23
#define STOP_FPGA_FULL_COL_SCENARIO                          24
/*
 * }
 */
/*
 * Definitions related to 'watchdog' parameter
 * ('general' subject).
 * {
 */
#define STOP_RESET_EQUIVALENT       1
#define INVOKE_HANDLER_EQUIVALENT   2
/*
 * }
 */
/*
 * Definitions related to 'app_source_file' parameter
 * {
 */
#define HOST_SOURCE_EQUIVALENT   0
/*
 * }
 */
/*
 * Definitions related to 'console_baud_rate' parameter.
 * {
 */
#define B_4800_EQUIVALENT      4800
#define B_9600_EQUIVALENT      9600
#define B_38400_EQUIVALENT    38400
#define B_19200_EQUIVALENT    19200
/*
 * }
 */
/*
 * Definitions related to 'block' parameter
 * {
 */
#define BLOCK_1_EQUIVALENT       1
#define BLOCK_2_EQUIVALENT       2
#define BLOCK_3_EQUIVALENT       5
#define BLOCK_4_EQUIVALENT       6
#define BLOCK_5_EQUIVALENT       7
#define BLOCK_6_EQUIVALENT       8
#define BLOCK_7_EQUIVALENT       11
#define BLOCK_100_EQUIVALENT     3
#define BLOCK_101_EQUIVALENT     4
#define BLOCK_200_EQUIVALENT     9
#define BLOCK_201_EQUIVALENT     10
#define BLOCK_300_EQUIVALENT     12
#define BLOCK_301_EQUIVALENT     13
#define BLOCK_400_EQUIVALENT     14
#define BLOCK_401_EQUIVALENT     15
/*
 * }
 */
/*
 * Definitions related to 'load_defaults' parameter
 * {
 */
#define DEFAULTS_ALL_EQUIVALENT        0
#define DEFAULTS_BLOCK_2_EQUIVALENT    2
#define DEFAULTS_BLOCK_3_EQUIVALENT    5
#define DEFAULTS_BLOCK_4_EQUIVALENT    6
#define DEFAULTS_BLOCK_5_EQUIVALENT    7
#define DEFAULTS_BLOCK_6_EQUIVALENT    8
#define DEFAULTS_BLOCK_7_EQUIVALENT    11
#define DEFAULTS_BLOCK_101_EQUIVALENT  4
#define DEFAULTS_BLOCK_200_EQUIVALENT  9
#define DEFAULTS_BLOCK_201_EQUIVALENT  10
/*
 * }
 */
/*
 * Definitions related to 'update' parameter
 * {
 */
#define RUN_TIME_EQUIVALENT      1
#define NEXT_STARTUP_EQUIVALENT  2
#define BOTH_EQUIVALENT          3
/*
 * }
 */
/*
 * Definitions related to 'format' parameter
 * {
 */
#define CHAR_EQUIVALENT  1
#define SHORT_EQUIVALENT 2
#define LONG_EQUIVALENT  4
/*
 * }
 */
/*
 * Definitions related to 'disp' parameter
 * {
 */
#define HEX_EQUIVALENT              1
#define UNSIGNED_INTEGER_EQUIVALENT 2
#define SIGNED_INTEGER_EQUIVALENT   3
/*
 * }
 */
/*
 * Definitions related to 'type' parameter
 * {
 */
#define FE_1_REGS_EQUIVALENT       1
#define FE_2_REGS_EQUIVALENT       2
#define SDRAM_EQUIVALENT           3
#define FLASH_BANK_1_EQUIVALENT    4
#define E2PROM_EQUIVALENT          5
#define EPLD_EQUIVALENT            6
#define GP2_EQUIVALENT             7
#define GP3_EQUIVALENT             8
#define INTERNAL_DPRAM_EQUIVALENT  9
#define FAP10M_1_REGS_EQUIVALENT   11
#define FAP10M_2_REGS_EQUIVALENT   12
#define GP1_FE1_EQUIVALENT         13
#define GP1_FE2_EQUIVALENT         14
#define MEM_BUFFER_EQUIVALENT      15
#define GT64131_SDRAM_EQUIVALENT   16
#define GT64131_REGS_EQUIVALENT    17
#define PCI_IO_EQUIVALENT          18
#define PCI_MEM1_EQUIVALENT        19
#define PCI_MEM2_EQUIVALENT        20
#define GP1_FAP10M_EQUIVALENT      21
#define GP1_FAP20V_1_EQUIVALENT    22
#define GP1_FTG_A_EQUIVALENT       23
#define GP1_FTG_B_EQUIVALENT       24
#define GP1_FAP20V_FSC_1_EQUIVALENT     25
#define GP1_FAP20V_EPLD_1_EQUIVALENT    26
#define GP1_FAP20V_FLASH_1_EQUIVALENT   27
#define GP1_FAP20V_STRM_IF_1_EQUIVALENT 28
#define TEVB_TIMNA_EQUIVALENT 29
#define TEVB_EPLD_EQUIVALENT 30
#define TEVB_FLASH_EQUIVALENT 31

#define GP1_FAP21V_1_EQUIVALENT    22
#define GP1_FAP21V_FSC_1_EQUIVALENT     25
#define GP1_FAP21V_EPLD_1_EQUIVALENT    26
#define GP1_FAP21V_FLASH_1_EQUIVALENT   27
#define GP1_FAP21V_STRM_IF_1_EQUIVALENT 28

#define GFA_PETRA_PETRA_EQUIVALENT                            29
#define GFA_PETRA_STATISTICS_EQUIVALENT                       30
#define GFA_PETRA_IO_FPGA_EQUIVALENT                          31
#define GFA_PETRA_FILE_SYSTEM_FLASH_EQUIVALENT                32
#define UTILS_PTG_FRONT_END_IO_FPGA_EQUIVALENT                33
#define UTILS_PTG_FRONT_END_FILE_SYSTEM_FLASH_EQUIVALENT      34
#define UTILS_PTG_FRONT_END_DEVICE_0_EQUIVALENT               35
#define UTILS_PTG_FRONT_END_DEVICE_1_EQUIVALENT               36
#define GFA_BI_PETRA_EQUIVALENT                               37
#define GFA_BI_PCP_EQUIVALENT                                 38
#define GFA_BI_IO_FPGA_EQUIVALENT                             39

/*
 * }
 */
/*
 * Definitions related to 'offset' parameter
 * and also for general use.
 * {
 */
#define FALSE_EQUIVALENT         0
#define ON_EQUIVALENT            1
#define OFF_EQUIVALENT           0
/*
 * }
 */
/*
 * Definitions related to 'general_parameters' parameter
 * ('pss bridge')
 * {
 */
#define SET_MAC_DA_SA_TRAP_EQUIVALENT 1
#define HOST_COUNTERS_EQUIVALENT      2
/*
 * }
 */
/*
 * Definitions related to 'num_packets' parameters
 * {
 */
#define NUM_PACKETS_INFINITE   (unsigned long)(-1)
/*
 * }
 */
/*
 * Definitions related to 'telnet_timeout_*' parameters
 * {
 */
#define TELNET_INACTIVITY_NONE  255
/*
 * }
 */
/*
 * Definitions related to 'show' parameter
 * in the context of 'general' subject and
 * also for general use.
 * {
 */
#define METHOD_REGULAR_DISPLAY   0
#define METHOD_PERIODIC_DISPLAY  2
/*
 * }
 */
/*
 * Definitions related to 'read' parameter
 * in the context of 'general fpga_indirect'
 * subject and also for general use.
 * {
 */
#define GENERATOR_ACCESS     0
#define VALIDATOR_ACCESS     1
#define FLOW_CONTROL_ACCESS  2
/*
 * }
 */
/*
 * Definitions related to 'sub_block' parameter
 * in the context of 'general fpga_indirect'
 * subject and also for general use.
 * {
 */
#define SUB_BLOCK_CALENDAR_TABLE                  0
#define SUB_BLOCK_EGRESS_STREAM_TABLE             1
#define SUB_BLOCK_EGRESS_PATTERN_TABLE            2
#define SUB_BLOCK_LOG_MEMORY_TABLE                3
#define SUB_BLOCK_CHANNEL_TO_PORT_TABLE           4
#define SUB_BLOCK_INGRESS_STREAM_TABLE            5
#define SUB_BLOCK_INGRESS_PATTERN_TABLE           6
#define SUB_BLOCK_LOGICAL_MULTICAST_TABLE         7
#define SUB_BLOCK_INGRESS_MULTICAST_STREAM_TABLE  8
#define SUB_BLOCK_STREAM_SELECTION_TABLE          9
#define SUB_BLOCK_STREAM_RATE_TABLE               10
#define SUB_BLOCK_PORT_RATE_TABLE                 11
#define SUB_BLOCK_M_DELAY_TABLE                   12
#define SUB_BLOCK_LAST_DELAY_TABLE                13
#define SUB_BLOCK_DELAY_HISTOGRAM_TABLE           14
#define SUB_BLOCK_LAST_TIMESTAMP_TABLE            15
#define SUB_BLOCK_INTER_PACKET_DELAY_HISTOGRAM    16
#define SUB_BLOCK_JITTER_HISTOGRAM_TABLE          17
#define SUB_BLOCK_PORT_STATISTICS_TABLE           18
#define SUB_BLOCK_CHANNEL_WEIGHT_TABLE            19
#define SUB_BLOCK_CHANNEL_FIFO_SIZE_TABLE         20
#define SUB_BLOCK_SPILL_OVER_TABLE                21
/*
 * }
 */
/*
 * Definitions related to general use of
 * 'action' parameter (read or write) in
 * the context of 'general' subject
 * {
 */
#define FPGA_READ_ACTION     0
#define FPGA_WRITE_ACTION    1
/*
 * }
 */
/*
 * Definitions related to 'fpga_indirect'
 * parameter in the context of 'general'
 * subject and also for general use.
 * {
 */
#define FPGA_A_IDENTIFIER     0
#define FPGA_B_IDENTIFIER     1
/*
 * }
 */
/*
 * Definitions related to 'operation' parameter
 * in the context of 'mem' subject (val_read).
 * {
 */
#define EQUAL_EQUIVALENT      0
#define NOT_EQUAL_EQUIVALENT  1
/*
 * }
 */
/*
 * Definitions related to 'counter get' or 'counter clear'
 * parameters in the context of 'mem' subject ('counter').
 * {
 */
#define VAL_READ_FAILURES_EQUIVALENT  1
/*
 * }
 */
/*
 * Definitions related to 'fe_indirect' subject
 * {
 */
/*
 * }
 */

/*
 * Definitions related to 'regression' subject
 * {
 */
/*
 * Numerical value equivalent for symbolic
 * value 'all'.
 */
typedef enum{
  SHORT_RGR_EQUIVALENT=0,
  FULL_RGR_EQUIVALENT
} UI_RGR_SCOPE_TYPE;
/*
 * }
 */
/*
 * Definitions related to 'history' subject
 * {
 */
/*
 * Numerical value equivalent for symbolic
 * value 'all'.
 */
#define ALL_EQUIVALENT 1
/*
 * Number of elements in history fifo.
 * Should be 20 or so
 */
#define NUM_HISTORY_ELEMENTS 25
/*
 * Values for 'display_direction' element.
 */
#define DIRECTION_NONE 0
#define DIRECTION_UP   1
#define DIRECTION_DOWN 2
typedef struct
{
    /*
     * Next index to load in '*history_array'.
     * Range: 0 -> (elements_in_array - 1)
     */
  unsigned int index ;
    /*
     * Index last used to display a line on
     * the screen. It is made equal to 'index'
     * every time an <enter> key is hit.
     * Range: 0 -> (elements_in_array - 1)
     */
  unsigned int display_index ;
    /*
     * Direction of history display path (backward
     * or forward, equivalent to up and down).
     */
  unsigned int display_direction ;
    /*
     * Number of meaningful entries in '*history_array'.
     * Can be up to 'elements_in_array'. Newest entry
     * is pointed by (index - 1) [modulo the size of
     * the array].
     */
  unsigned int num_loaded ;
    /*
     * Number of entries in 'history_array'.
     */
  unsigned int elements_in_array ;
    /*
     * Array of pointers to structures of
     * type CURRENT_LINE.
     */
  CURRENT_LINE **history_array ;
    /*
     * Pointer to structure of
     * type CURRENT_LINE. Used as a trash can.
     */
  CURRENT_LINE *trash_line_ptr ;
} HISTORY_FIFO ;
/*
 * }
 */
/*
 * Definitions related to 'mem' subject
 * {
 */
/*
 * Maximal number of memory blocks allowed for this
 * system.
 */
/*
 * Various attributes for memory blocks.
 */
/*
 * Access block using only long access, aligned
 * on long boundary.
 */
#define LONG_ALIGNED_WRITE_ONLY BIT(0)
/*
 * Access block using byte access only.
 */
#define BYTE_ACCESS_ONLY        BIT(1)
/*
 * Block is an EEPROM and requires special access
 * routines for writing.
 */
#define EEPROM_ACCESS           BIT(2)
/*
 * Block may only be accessed for 'read' ('write'
 * using 'override' only)
 */
#define BLOCK_READ_ONLY         BIT(3)

/*
 * }
 */

/*
 * Parameters related to 'handle_nv_item' procedure:
 */
#define PRINT_ITEMS      BIT(0)
#define CHANGE_NV        BIT(1)
#define CHANGE_RUN_TIME  BIT(2)
/*
 * Prototypes.
 * {
 */
char
  sys_getch(
    void
  ) ;
void
  sys_flush_getch(
    void
  ) ;
/*
 * Definitions related to 'ui_proc_signal'
 */
/*
 * Indications on telnet channels
 */
#define BOTH_DEAD  1
#define BOTH_ALIVE 2
/*
 * Number of times timer procedure detects a change in
 * status before it believes it.
 */
#define MAX_TRIGGER_DELAY 1
void
  ui_proc_signal(
    int signal_id
  ) ;
int
  search_subjects(
    char          *subj_string,
    unsigned int  size_subj_string,
    int           search_all,
    unsigned int  num_match,
    unsigned int  *match_index,
    unsigned int  *full_match,
    char          **subject_name_ptr,
    unsigned int  *common_len
  ) ;
#if DEBUG_AID
/* { */
int
  aa_value_checker(
    unsigned long  source_id,
    char *err_msg
  ) ;
/* } */
#endif
/*
 * }
 */
/*
 * Definitions related to 'ui_utils'
 * {
 */
void
  init_current_line(
    CURRENT_LINE *current_line
  ) ;
int
  display_line_info(
    CURRENT_LINE *current_line
  ) ;
int
  space_while_receiving_subject(
    CURRENT_LINE *current_line,
    int          enter_case
  ) ;
int
  space_receiving_param_string(
    CURRENT_LINE *current_line,
    int          enter_case
  ) ;
int
  space_receiving_param_val(
    CURRENT_LINE *current_line,
    int          enter_case
  ) ;
int
  is_field_legitimate_ip(
    CURRENT_LINE  *current_line,
    char          **err_msg,
    unsigned long partial,
    unsigned long *ip_addr
  ) ;
int
  param_val_pair_to_ascii(
    CURRENT_LINE *current_line,
    unsigned int index,
    char         **presentation
  ) ;
const char
  *proc_state_to_ascii(
    CURRENT_LINE *current_line
  ) ;
unsigned int
  index_of_last_space(
    CURRENT_LINE *current_line
  ) ;
int
  convert_numeric_field(
    CURRENT_LINE  *current_line,
    char          **err_mag,
    unsigned long *value,
    unsigned long partial
  ) ;
int
  is_char_numeric_starter(
    char in_char
  ) ;
int
  is_last_field_numeric(
    CURRENT_LINE *current_line
  ) ;
unsigned
  int
    get_current_ordinal(
      CURRENT_LINE  *current_line,
      unsigned long *current_mutex,
      unsigned int  ignore_multi_val
    ) ;
int
  is_char_legitimate_text(
    char in_char
  ) ;
int
  is_field_legitimate_text(
    CURRENT_LINE  *current_line,
    char          **err_msg,
    unsigned long partial
  ) ;
int
  is_field_numeric_starter(
    CURRENT_LINE *current_line,
    char         **err_mag
  ) ;
int
  get_current_mutex_control(
    CURRENT_LINE  *current_line,
    unsigned long *current_mutex
  ) ;
int
  get_blank_proc_state(
    CURRENT_LINE *current_line
  ) ;
int
  search_params(
    const char    *reference_string,
    unsigned int  size_reference_string,
    int           search_all,
    unsigned int  num_match,
    unsigned int  *match_index,
    unsigned int  *full_match,
    char    **parameter_name_ptr,
    unsigned int  *common_len,
    char          *params_list,
    unsigned int  num_parameters,
    unsigned int  element_size,
    unsigned int  name_offest,
    char          *found_names_list,
    unsigned int  extra_search_on,
    unsigned long *reference_mask,
    unsigned int  mask_offest,
    char          **excluded_params_base,
    unsigned int  num_exc_parameters,
    unsigned long reference_ordinal,
    unsigned int  ordinal_offest
  ) ;
int
  compare_param_vals_by_name(
    const void *parameter_val_1,
    const void *parameter_val_2
  ) ;
int
  compare_params_by_name(
    const void *parameter_1,
    const void *parameter_2
  ) ;
int
  compare_subjs_by_name(
    const void *subject_1,
    const void *subject_2
  ) ;
void
  print_detailed_help(
    CURRENT_LINE *current_line,
    int          for_subjects
  ) ;
void
  all_subjs_short_help(
    CURRENT_LINE *current_line,
    int          for_subjects
  ) ;
int
  complete_last_field(
    const char         *field_string,
    CURRENT_LINE *current_line,
    int          partial_in_string,
    unsigned int c_len
  ) ;
int
  get_num_subjs(
    void
  ) ;
int
  get_subj_list_len(
    void
  ) ;
char
  *get_subj_list_ptr(
    void
  ) ;
void
  clear_n_redisplay_line(
    CURRENT_LINE *current_line
  ) ;
int
  is_params_on_line(
    const char   *param_name,
    CURRENT_LINE *current_line
  ) ;
void
  back_space_line(
    CURRENT_LINE *current_line
  ) ;
void
  redisplay_line(
    CURRENT_LINE *current_line
  ) ;
void
  msg_and_redisplay(
    const char *msg_string,
    CURRENT_LINE *current_line
  ) ;
int
  send_char_to_screen(
    char    out_char
  ) ;
int
  low_level_to_screen(
    char *out_string,
    int  num_to_send
  ) ;
int
  send_array_to_screen(
    const char *const_out_string,
    int  num_to_send
  ) ;
int
  send_string_to_screen(
    const char *out_string,
    int  add_cr_lf
  ) ;
int
  params_fully_loaded(
    CURRENT_LINE *current_line,
    char         *err_msg
  ) ;
int
  is_params_mutex(
    unsigned int  match_index,
    CURRENT_LINE *current_line,
    char         *err_msg
  ) ;
int
  help_for_match(
    char         *field_string,
    unsigned int num_loaded,
    PARAM        *parameter_ptr
  );

char
  *get_blank_line(
    void
    ) ;
char
  get_rotating_stick(
    void
    ) ;
void
  send_to_shell(
    char         *string_to_send,
    unsigned int len
  ) ;
/*
 * Object: Input file (for CLI interpreter)
 * This object is related to the special mode where a local
 * file is made (temporarily) the input channel of the main
 * CLI interpreter (handle_next_char())
 * {
 */
void
  clear_file_input_mode(
    void
  ) ;
void
  set_file_input_mode(
    void
  ) ;
unsigned
  int
    is_file_input_mode(
      void
    ) ;
void
  set_cli_input_fd(
    int file_descriptor
  ) ;
int
  get_cli_input_fd(
    void
  ) ;
void
  close_cli_input_fd(
    void
  ) ;
void
  set_cli_times_available(
    void
  ) ;
void
  clear_cli_times_available(
    void
  ) ;
int
  are_cli_times_available(
    void
  ) ;
void
  set_cli_start_time(
    unsigned long time_from_startup
  ) ;
void
  set_cli_end_time(
    unsigned long time_from_startup
  ) ;
int
  get_cli_elapsed_time(
    unsigned long *elapsed_time,
    char          **file_name
  ) ;
/*
 * }
 */
/*
 * Object: page_print
 * {
 */
void
  clear_abort_print_flag(
    void
  ) ;
void
  set_abort_print_flag(
    void
  ) ;
unsigned
  int
    is_abort_print_flag_set(
      void
    ) ;
void
  clear_line_mode(
    void
  ) ;
void
  set_line_mode(
    void
  ) ;
unsigned
  int
    is_line_mode_set(
      void
    ) ;
int
  line_mode_from_nv(
    unsigned char *line_mode
  ) ;
int
  base_and_option_registers_from_nv(
    unsigned long *long_vals
  );
void
  clear_page_mode(
    void
  ) ;
void
  set_page_mode(
    void
  ) ;

void
  ui_triger_page_mode(
    void
  ) ;

unsigned
  int
    is_page_mode_set(
      void
    ) ;
int
  page_lines_from_nv(
    unsigned char *page_lines
  ) ;
int
  page_mode_from_nv(
    unsigned char *page_mode
  ) ;
void
  init_page_print(
    unsigned int lines_per_page
  ) ;
void
  set_lines_per_page(
    unsigned int lines_per_page
  ) ;
void
  clear_num_lines_printed(
    void
  ) ;
void
  inc_num_lines_printed(
    unsigned int add_lines
  ) ;
/*
 * Indicate by how many lines there will be page
 * overflow if input parameter lines are added.
 */
unsigned int
  get_page_overflow(
    unsigned int add_lines
  ) ;
/*
 * Indicate how many lines current page can take
 * before overflow.
 */
unsigned int
  get_page_left(
    void
  ) ;
/*
 * }
 */
int
  get_array_num_lines(
    char         *in_arr,
    unsigned int size_arr
  ) ;
char
  *get_ptr_of_lf(
    char         *in_str,
    unsigned int lf_number,
    unsigned int size_arr
  ) ;
/*
 * Definitions related to ask_get()
 */
/*
 * {
 */
/*
 * Maximal number of characters on line presenting question
 * to the user in the 'one_line' mode.
 */
#define MAX_ONE_LINE_ASK 60
/*
 * Wait for the user to hit 'y' or 'n'
 * and return with '0' for 'no' and
 * any other number for 'yes'.
 */
#define EXPECT_YES_NO     0
/*
 * Wait for the user to hit 'space' or any
 * other key (standing for 'continue' and
 * 'abort', respectively) and return with '0'
 * for 'abort' and any other number for
 * 'continue'.
 */
#define EXPECT_CONT_ABORT 1
/*
 * Wait for the user to hit any character
 * and return with the ASCII value of that
 * character.
 */
#define EXPECT_ANY        2
/*
 * Wait for the user to hit 'carriage return'
 * and return with the ASCII value of that
 * character.
 */
#define EXPECT_CR         3
/*
 * }
 */
int
  ask_get(
    char         *ask_text,
    unsigned int expect,
    unsigned int one_line_mode,
    unsigned int temporary_unsafe
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_process'
 * {
 */
int
  paste_text_params_quotation_marks(
    CURRENT_LINE *current_line
  ) ;
int
  cut_text_params_quotation_marks(
    CURRENT_LINE *current_line
  ) ;
int
  search_param_val_pairs(
    CURRENT_LINE *current_line,
    unsigned int *match_index,
    int          param_id,
    unsigned int ocr_num
  ) ;
int
  get_val_of(
    CURRENT_LINE  *current_line,
    int           *match_indicator,
    int           param_id,
    unsigned int  ocr_num,
    PARAM_VAL     **param_val,
    unsigned long param_type,
    char          *err_msg
  ) ;
int
  process_line(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_snmp'
 * {
 */
#if INCLUDE_NET_SNMP
/* { */
int
  subject_snmp(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/* } INCLUDE_NET_SNMP */
#endif


/*
 * }
 */
/*
 * Definitions related to 'ui_fe200'
 * {
 */
int
  subject_fe200(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_sand'
 * {
 */
int
  subject_sand(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
int
  subject_dhrp(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );

/*
 * Definitions related to 'ui_line_card'
 * {
 */
int
  subject_line_card(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_line_gfa'
 * {
 */
int
  subject_line_gfa(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */

/*
 * Definitions related to 'ui_line_tgs'
 * {
 */
int
  subject_line_tgs(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * Definitions related to 'ui_line_tevb'
 * {
 */
int
  subject_line_tevb(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );

/*
 * Definitions related to 'ui_diag'
 * {
 */
int
  subject_diag(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
/*
 * }
 */

/*
 * Definitions related to 'ui_negev_demo'
 * {
 */
int
  subject_demo(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_gobi_demo(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_negev_demo(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_fap20v_sweep_app(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_fap20v_sweep_app_b(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_csys(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_skt(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;

/*
 * }
 */
/*
 * Definitions related to 'ui_history'
 * {
 */
int
  subject_history(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  init_history_fifo(
    void
  ) ;
int
  put_history_fifo(
    CURRENT_LINE **current_line_ptr
  ) ;
void
  dec_display_index(
    void
  ) ;
void
  inc_display_index(
    void
  ) ;
void
  show_prev_history_line(
    CURRENT_LINE **current_line_ptre
  ) ;
void
  show_next_history_line(
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_fe_indirect'
 * {
 */
/*
 * Numerical value equivalent for symbolic
 * value 'all' in the context of 'index'.
 */
#define ALL_INDEX_EQUIVALENT   -1
int
  subject_fe_indirect(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  fe_ind_rr_index_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  ) ;
int
  fe_ind_data_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
    ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_vx_shell'
 * {
 */
int
  subject_vx_shell(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_general'
 * {
 */

void
  ui_printing_policy_set(
    unsigned int short_prints
  );

unsigned int
  ui_printing_policy_get(
    void
  );

int
  subject_general(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  app_file_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  ) ;
int
  line_card_slot_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  ) ;
int
  fabric_card_slot_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_nvram'
 * {
 */
int
  app_source_checker(
    void           *current_line_ptr,
    unsigned long  source_id,
    char           *err_msg,
    unsigned long  partial
    ) ;
int
  subject_nvram(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  handle_nv_item(
    void         **val_ptrs,
    unsigned int param_id,
    unsigned int action
  ) ;
int
  update_nvram_param(
    unsigned int update,
    unsigned int param_id,
    unsigned int param_val
  );
int
  get_run_downloading_host_addr(
    unsigned long *ip_addr_ptr
  ) ;
int
  get_run_ip_addr(
    unsigned long *ip_addr_ptr
    ) ;
int
  set_run_vals(
    char *string,
    int  strLen,
    int  offset
    );
int
  load_run_vals(
    void
    ) ;
int
  get_block_info(
    unsigned int    block_id,
    char            **block_base,
    unsigned long   *block_size,
    unsigned short  **block_crc_ptr
    ) ;
int
  load_defaults_conditional(
    unsigned int  block_id
    ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_pss'
 * {
 */
int
  is_legit_mac_address(
    char       *c_ptr
  ) ;
int
  subject_pss(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
/*
 * Definitions related to 'ui_fmf'
 * {
 */
int
  subject_fmf(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
/*
 * }
 */
int
  subject_fap20v(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_fap20v_b(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_fap10m(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_sim(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
int
  subject_fap21v_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_fap21v_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_petra_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_tmd_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_pb_pp_acc2(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_pcp_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_swp_p_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_petra_gfa(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_petra_pp_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_swp_p_pp_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_gsa_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_gsa_petra_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_ptg(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_petra_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_petra_pp_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );

int
  subject_fe600_bsp(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_fe600_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_t20e_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_ppd_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_oam_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_oam_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );

/*
 * Definitions related to 'ui_memory'
 * {
 */
int
  subject_mem(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  ) ;
void
  fill_blanks(
    char          *in_str,
    unsigned int  num,
    unsigned long options
  ) ;
int
  display_all_mac_counters(
    unsigned int logical_port_is_trunk,
    unsigned int logical_port_unit,
    unsigned int logical_port_device,
    unsigned int logical_port_port,
    unsigned int display_format,
    unsigned int clear_after_read
  );
/*
 * }
 */

 /*
 * Definitions related to 'ui_dhrp_api'
 * {
*/

int
  subject_dhrp_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_access_timna(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_timna_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_sweept20_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_fap21v_gfa(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
int
  subject_fap21v_app(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  );
void
  handle_next_char(
    CURRENT_LINE **current_line_ptr,
    char         in_char
  );
/*
 * }
 */
/* } */
#endif
