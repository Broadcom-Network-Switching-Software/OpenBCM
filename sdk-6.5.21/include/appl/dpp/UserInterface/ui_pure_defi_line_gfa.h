/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_LINE_GFA_INCLUDED
/* { */
#define UI_PURE_DEFI_LINE_GFA_INCLUDED
/*
 * Note:
 * the following definitions must range between PARAM_LINE_GFA_START_RANGE_ID (7100)
 * and PARAM_LINE_GFA_END_RANGE_ID (7300).
 * See ui_pure_defi.h
 */
#define PARAM_GFA_FAP20V_1_ID                  7100
#define PARAM_GFA_RESET_ID                     7101
#define PARAM_GFA_STAY_DOWN_ID                 7102
#define PARAM_GFA_MILISEC_ID                   7103
#define PARAM_GFA_FAP20V_1_CLR_CNT_INT_ID      7105
#define PARAM_GFA_FAP20V_1_IDDR_TEST_RW_ID     7106
#define PARAM_GFA_FAP20V_1_IDDR_OFFSET_ID      7107
#define PARAM_GFA_NOF_ITERATION_ID             7108
#define PARAM_GFA_PAD_DATA_ID                  7109
#define PARAM_GFA_USE_COEXIST_ID               7110
#define PARAM_GFA_READ_AND_CLEAR_CPU_PKT_ID    7111
#define PARAM_GFA_FAP20V_1_ZBT_TEST_RW_ID      7113
#define PARAM_GFA_USE_ZBT_ID                   7114
#define PARAM_GFA_USE_OFC_ID                   7115
#define PARAM_GFA_FAP20V_1_CPU_TEST_RW_ID      7116
#define PARAM_GFA_MAX_NOF_FAPS_ID              7117
#define PARAM_GFA_MAX_NOF_PORTS_ID             7118
#define PARAM_GFA_ZBT_NOF_BUFF_MEM_FIELD_ID    7119
#define PARAM_GFA_DRAM_BUFF_SIZE_FIELD_ID      7120
#define PARAM_GFA_FAP20V_1_INQ_TEST_RW_ID      7121
#define PARAM_GFA_SEARCH_DRAM_DELAY_LINE_ID    7122
#define PARAM_GFA_START_TAPS_ID                7123
#define PARAM_GFA_NOF_TAP_TO_START_ID          7124
#define PARAM_GFA_SEARCH_DRAM_DELAY_LINE_AUTO_ID 7125
#define PARAM_GFA_LBG_ID                       7126
#define PARAM_GFA_LBG_START_CONF_ID            7127
#define PARAM_GFA_LBG_SEND_PKT_ID              7128
#define PARAM_GFA_PKT_BYTE_SIZE_ID             7129
#define PARAM_GFA_LBG_RESTORE_CONF_ID          7130
#define PARAM_GFA_GET_RATE_ID                  7131
#define PARAM_GFA_NOF_PKTS_ID                  7132
#define PARAM_GFA_VERBOSE_ID                   7133
#define PARAM_GFA_SCAN_DRAM_DELAY_LINE_ID      7134
#define PARAM_GFA_START_DDR_ID                 7135
#define PARAM_GFA_END_DDR_ID                   7136
#define PARAM_GFA_START_SCAN_ID                7137
#define PARAM_GFA_END_SCAN_ID                  7138
#define PARAM_GFA_SCAN_DRAM_DELAY_AUTO_ID      7139
#define PARAM_GFA_FIX_FACTOR_ID                7140
#define PARAM_GFA_SCAN_DRAM_DELAY_AUTO_B_ID    7141
#define PARAM_GFA_SAMPLE_NUM_ID                7142
#define PARAM_GFA_SAMPLE_ADD_ID                7143
#define PARAM_GFA_PACKET_SCHEME_ID             7144
#define PARAM_GFA_BW_RATE_ID                   7145
#define PARAM_LBG_ON_ID                        7146
#define PARAM_INVERSE_SSO_ID                   7147
#define PARAM_GFA_SEARCH_DRAM_DELAY_LINE_AUTO_FULL_ID 7148
#define PARAM_GFA_CALIBRATE_DDR_ID             7149
#define PARAM_GFA_CALIBRATE_DDR_FULL_ID        7150
#define PARAM_GFA_NOF_DDR_ID                   7151
#define PARAM_GFA_OFFSET_CORRECT_ID            7152
#define PARAM_GFA_DO_EXHAUSTIVE_SEARCH_ID      7153
#define PARAM_GFA_USE_DUNE_FTG_ID              7154
#define PARAM_GFA_CALIBRATE_LOAD_ID            7155
#define PARAM_GFA_SAMPLE_DELAY_REG_ID          7156
#define PARAM_GFA_END_WIN_ID                   7157
#define PARAM_GFA_AVRG_ID                      7158
#define PARAM_GFA_CALIBRATE_READ_FROM_NV_ID    7159
#define PARAM_GFA_CALIBRATE_CLEAR_NV_ID        7160
#define PARAM_GFA_MAP_CPU_PORT_ID              7161
#define PARAM_GFA_DO_STATIC_SPI_CALIB          7162
#define PARAM_GFA_PRINT_STATUS_ID              7163
#define PARAM_GFA_MB_ID                        7164
#define PARAM_GFA_FLASH_ID                     7165
#define PARAM_GFA_DOWNLOAD_ID                  7166
#define PARAM_GFA_BURN_FPGA_ID                 7167
#define PARAM_GFA_ERASE_ID                     7168
#define PARAM_GFA_QUEUE_SCAN_ID                7169
#define PARAM_GFA_QUEUE_SCAN_EN_ID             7170
#define PARAM_GFA_QUEUE_SCAN_DIS_ID            7172
#define PARAM_GFA_QUEUE_SCAN_PRINT_ID          7173
#define PARAM_GFA_QUEUE_SCAN_SET_PARAM_ID      7174
#define PARAM_GFA_QUEUE_SCAN_NOF_REWRITE_ID    7175
#define PARAM_GFA_QUEUE_SCAN_READ_SIZE_ID      7176
#define PARAM_GFA_QUEUE_SCAN_Q_SIZE_ID         7177
#define PARAM_GFA_CALIB_WRITE_DQDLY_STATE      7178
#define PARAM_GFA_CALIB_WRITE_DQDLY_STATE_DDR  7179
#define PARAM_GFA_CALIB_WRITE_DQDLY_STATE_POS  7180

#define PARAM_GFA_SCRN_ID                      7183
#define PARAM_GFA_SCRN_FTG_TEST_ID             7184
#define PARAM_GFA_SCRN_IF_TEST_ID              7185
#define PARAM_GFA_SCRN_PRNT_USR_MD_ID          7186

#define PARAM_GFA_FPGA_ID                      7190
#define PARAM_GFA_FPGA_DOWNLOAD_ID             7191

#define PARAM_GFA_LBG_VALIDATE_RATE_ID         7195
#define PARAM_GFA_LBG_VALIDATE_RATE_RATE_ID    7196



#define PARAM_GFA_SWEEP_APP_ID                 7200
#define PARAM_GFA_CREDIT_SIZE_ID               7201
#define PARAM_GFA_SPI4_SRC_TSCLK_SEL_ID        7202
#define PARAM_GFA_SPI4_SNK_RSCLK_SEL_ID        7203
#define PARAM_GFA_SPI4_SRC_DATA_MAX_T_ID       7204
#define PARAM_GFA_SPI4_SRC_INIT_TRAIN_SEQ_ID   7205
#define PARAM_GFA_FAP_ID                       7206
#define PARAM_GFA_SPI4_SRC_CAL_LEN_ID          7207
#define PARAM_GFA_SPI4_SRC_CAL_M_ID            7208
#define PARAM_GFA_SPI4_SNK_CAL_LEN_ID          7209
#define PARAM_GFA_SPI4_SNK_CAL_M_ID            7210


/* } */
#endif
