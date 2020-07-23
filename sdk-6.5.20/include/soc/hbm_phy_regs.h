/** \file include/soc/hbm_phy_regs.h
 * 
 * common HBM Phy register, fields and read write macros 
 * declaration. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** 
 *  The order of the members of both soc_hbm16_midstack_reg_e
 *  and soc_hbm16_channel_reg_e must be aligned to the arrays
 *  found in src/soc/common/hbm_phy_regs.c:
 *  soc_hbm16_midstack_reg_list and soc_hbm16_channel_reg_list
 */

#ifndef _SOC_HBMPHYREGS_H
#define _SOC_HBMPHYREGS_H

#include <soc/register.h>
#include <soc/types.h>

extern int soc_hbm16_midstack_read(int unit, int hbm_idx, soc_reg_t reg, int instance, uint32 *data);
extern int soc_hbm16_midstack_write(int unit, int hbm_idx, soc_reg_t reg, int instance, uint32 data);
extern int soc_hbm16_midstack_field_set(int unit, soc_reg_t reg, uint32 *regval, soc_field_t field, uint32 value);
extern int soc_hbm16_midstack_field_get(int unit, soc_reg_t reg, uint32 regval, soc_field_t field, uint32* field_val);
extern int soc_hbm16_midstack_field_size_get(int unit, soc_reg_t reg, soc_field_t field, uint32* field_size);

extern int soc_hbm16_channel_read(int unit, int hbm_idx, int channel, soc_reg_t reg, int instance, uint32 *data);
extern int soc_hbm16_channel_write(int unit, int hbm_idx, int channel, soc_reg_t reg, int instance, uint32 data);
extern int soc_hbm16_channel_field_set(int unit, soc_reg_t reg, uint32 *regval, soc_field_t field, uint32 value);
extern int soc_hbm16_channel_field_get(int unit, soc_reg_t reg, uint32 regval, soc_field_t field, uint32* field_val);
extern int soc_hbm16_channel_field_size_get(int unit, soc_reg_t reg, soc_field_t field, uint32* field_size);


extern soc_reg_info_t soc_hbm16_channel_reg_list[];

extern soc_reg_info_t soc_hbm16_midstack_reg_list[];

extern soc_field_info_t soc_hbm16_MIDSTACK_DAC_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_MIDSTACK_FREQ_CNTR_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_MIDSTACK_FREQ_CNTR_RST_CTRLr_fields[];
extern soc_field_info_t soc_hbm16_MIDSTACK_IO_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr_fields[];
extern soc_field_info_t soc_hbm16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr_fields[];

extern soc_field_info_t soc_hbm16_DWORD0_BV_GEN_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_IO_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_LANE_REMAPr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_LATENCY_SHIFTERr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_RDATA_FIFO_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_READ_MACHINE_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_READ_MIN_VDL_DATAr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_RXEN_RXBV_GEN_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERRORr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_LANE_REMAPr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_PTRr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_READ_MACHINEr_fields[];
extern soc_field_info_t soc_hbm16_DWORD0_STATUS_RXEN_RXBV_GENr_fields[];

extern soc_field_info_t soc_hbm16_AWORD_DAC_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_IO_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_LANE_REMAPr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_LATENCY_SHIFTERr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_MISR_CNTr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_MISR_CONFIGr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_MISR_INPUTr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_REG_RESERVEDr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_STATUS_RESERVEDr_fields[];
extern soc_field_info_t soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields[];

extern soc_field_info_t soc_hbm16_STATUS_AWORD_LANE_REMAPr_fields[];
extern soc_field_info_t soc_hbm16_STATUS_AWORD_MISRr_fields[];
extern soc_field_info_t soc_hbm16_STATUS_AWORD_MISR_CNTRr_fields[];

extern soc_field_info_t soc_hbm16_CONTROL_RESETSr_fields[];
extern soc_field_info_t soc_hbm16_CONTROL_WRITE_FIFO_CONFIGr_fields[];

typedef enum soc_hbm16_midstack_fields_e {
    HBM16_AUTO_UPDATEf,
    HBM16_AUTO_UPDATE_COUNTERf,
    HBM16_AUTO_UPDATE_READYf,
    HBM16_BV_00f,
    HBM16_BV_01f,
    HBM16_BV_02f,
    HBM16_BV_03f,
    HBM16_BV_04f,
    HBM16_BV_05f,
    HBM16_BV_06f,
    HBM16_BV_07f,
    HBM16_BV_08f,
    HBM16_BV_09f,
    HBM16_BV_10f,
    HBM16_BV_11f,
    HBM16_BV_12f,
    HBM16_BV_13f,
    HBM16_BV_14f,
    HBM16_BV_15f,
    HBM16_BYTE0f,
    HBM16_BYTE1f,
    HBM16_BYTE2f,
    HBM16_BYTE3f,
    HBM16_CAf,
    HBM16_CLEARf,
    HBM16_CNTf,
    HBM16_CONTINUOUS_MODEf,
    HBM16_CURRENT_ERRORf,
    HBM16_DAC_DATAf,
    HBM16_DAC_PDf,
    HBM16_DATA_PATH_ADDITIONAL_LATENCYf,
    HBM16_DATA_PATH_SHIFT_ENABLEf,
    HBM16_DATA_UI0f,
    HBM16_DATA_UI1f,
    HBM16_DBI0f,
    HBM16_DBI1f,
    HBM16_DBI2f,
    HBM16_DBI3f,
    HBM16_DBI_UI0f,
    HBM16_DBI_UI1f,
    HBM16_DCOUNTf,
    HBM16_DIFF_PORT_CK_DRIVE_STRENGTHf,
    HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf,
    HBM16_DM_UI0f,
    HBM16_DM_UI1f,
    HBM16_DONEf,
    HBM16_ENf,
    HBM16_ENABLEf,
    HBM16_EN_NLDL_CLKOUT_BARf,
    HBM16_ERROR_CLEARf,
    HBM16_FINITEf,
    HBM16_FORCE_RXENf,
    HBM16_FORCE_RX_BUSYf,
    HBM16_FORCE_RX_VALIDf,
    HBM16_FORCE_UPDATEf,
    HBM16_FREQ_CNTR_FC_RESET_Nf,
    HBM16_FREQ_CNTR_RO_RESET_Nf,
    HBM16_INITf,
    HBM16_MAX_VDL_STEPf,
    HBM16_MIN_VDL_STEPf,
    HBM16_MISR_RSTBf,
    HBM16_MODEf,
    HBM16_OBS_INTERVALf,
    HBM16_OBS_PTRf,
    HBM16_OUTf,
    HBM16_OVERFLOWf,
    HBM16_PAR_LATENCYf,
    HBM16_PAR_UI0f,
    HBM16_PAR_UI1f, 
    HBM16_PHASE_FIFO_MIN_DELAY_2f,
    HBM16_PHY_RSTB_1Gf,
    HBM16_RAf, 
    HBM16_RCMD_01f,
    HBM16_RCMD_02f,
    HBM16_RCMD_03f,
    HBM16_RCMD_04f,
    HBM16_RCMD_05f,
    HBM16_RCMD_06f,
    HBM16_RCMD_07f,
    HBM16_RCMD_08f,
    HBM16_RCMD_09f,
    HBM16_RCMD_10f,
    HBM16_RCMD_11f,
    HBM16_RCMD_12f,
    HBM16_RCMD_13f,
    HBM16_RCMD_14f,
    HBM16_RCMD_15f,
    HBM16_RCMD_16f,
    HBM16_RCMD_ADDITIONAL_LATENCYf,
    HBM16_RCMD_SHIFT_ENABLEf,
    HBM16_RD_DATA_DELAYf,
    HBM16_RD_UI0f,
    HBM16_RD_UI1f,
    HBM16_RDATA_FIFO_WR2RD_DELAYf,
    HBM16_RESERVEDf,
    HBM16_RESET_DRIVE_STRENGTHf,
    HBM16_RO_OVERFLOWf,
    HBM16_RO_UCOUNTf,
    HBM16_RO_VDL_STEPf,
    HBM16_RPTRf,
    HBM16_RPTR_WRT_RCLKf,
    HBM16_RXENBf,
    HBM16_RXENB_AERRf,
    HBM16_RXENB_CATTRIPf,
    HBM16_RXENB_DERRf,
    HBM16_RXENB_RDQSf,
    HBM16_RXENB_TEMPf,
    HBM16_RXEN_POST_CYCLESf,
    HBM16_RXEN_PRE_CYCLESf,
    HBM16_SEL0f,
    HBM16_SEL1f,
    HBM16_SEL2f,
    HBM16_SEL3f,
    HBM16_SEL_CAf,
    HBM16_SEL_FC_REFCLKf,
    HBM16_SEL_MIDSTACK_2G_WCLK_FOR_NLDL_CLKf,
    HBM16_SEL_RAf,
    HBM16_SINGLE_PORT_AQ_DRIVE_STRENGTHf,
    HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf,
    HBM16_STARTf,
    HBM16_START_OBSf,
    HBM16_STATE_00f,
    HBM16_STATE_01f,
    HBM16_STATE_02f,
    HBM16_STATE_03f,
    HBM16_STATE_04f,
    HBM16_STATE_05f,
    HBM16_STATE_06f,
    HBM16_STATE_07f,
    HBM16_STATE_08f,
    HBM16_STATE_09f,
    HBM16_STATE_10f,
    HBM16_STATE_11f,
    HBM16_STATE_12f,
    HBM16_STATE_13f,
    HBM16_STATE_14f,
    HBM16_STATE_15f,
    HBM16_STICKY_ERRORf,
    HBM16_TEST_PIN_DIV_SELf,
    HBM16_UI_SHIFTf, 
    HBM16_UNDERFLOWf,
    HBM16_UPDATE_PENDINGf,
    HBM16_WPTRf,
    HBM16_WPTR_WRT_RCLKf,
    HBM16_MIDSTACK_NUM_FIELDS
} soc_hbm16_midstack_fields_t;

#ifndef SOC_RESET_VAL_DEC
#    if !defined(SOC_NO_RESET_VALS)
#        define SOC_RESET_VAL_DEC(_lo, _hi) _lo, _hi,
#        define SOC_RESET_MASK_DEC(_lo, _hi) _lo, _hi,
#    else
#        define SOC_RESET_VAL_DEC(_lo, _hi)
#        define SOC_RESET_MASK_DEC(_lo, _hi)
#    endif
#endif

typedef enum soc_hbm16_midstack_reg_e {
    HBM16_MIDSTACK_DAC_CONFIGr,
    HBM16_MIDSTACK_FREQ_CNTR_CONFIGr,
    HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr,
    HBM16_MIDSTACK_IO_CONFIGr,
    HBM16_MIDSTACK_REG_RESERVEDr,
    HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr,
    HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr,
    HBM16_MIDSTACK_STATUS_RESERVEDr,
    HBM16_MIDSTACK_NUM_REGS
} soc_hbm16_midstack_reg_t;

typedef enum soc_hbm16_channel_reg_e {
    HBM16_DWORD0_BV_GEN_CONFIGr,
    HBM16_DWORD0_DAC_CONFIGr,
    HBM16_DWORD0_IO_CONFIGr,
    HBM16_DWORD0_LANE_REMAPr,
    HBM16_DWORD0_LATENCY_SHIFTERr,
    HBM16_DWORD0_RDATA_FIFO_CONFIGr,
    HBM16_DWORD0_READ_MACHINE_CONFIGr,
    HBM16_DWORD0_READ_MAX_VDL_DQS_Nr,
    HBM16_DWORD0_READ_MAX_VDL_DQS_Pr,
    HBM16_DWORD0_READ_MAX_VDL_FSMr,
    HBM16_DWORD0_READ_MIN_VDL_DATAr,
    HBM16_DWORD0_REG_RESERVEDr,
    HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr,
    HBM16_DWORD0_STATUS_BV_GEN_ERRORr,
    HBM16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr,
    HBM16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr,
    HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr,
    HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr,
    HBM16_DWORD0_STATUS_BV_GEN_LAST_BUSY_VALIDSr,
    HBM16_DWORD0_STATUS_BV_GEN_LAST_RCMDSr,
    HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_LSBr,
    HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_MSBr,
    HBM16_DWORD0_STATUS_LANE_REMAPr,
    HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r,
    HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r,
    HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r,
    HBM16_DWORD0_STATUS_RDATA_FIFO_PTRr,
    HBM16_DWORD0_STATUS_READ_MACHINEr,
    HBM16_DWORD0_STATUS_RESERVEDr,
    HBM16_DWORD0_STATUS_RXEN_RXBV_GENr,
    HBM16_DWORD0_WRITE_MAX_VDL_DATAr,
    HBM16_DWORD0_WRITE_MAX_VDL_DQSr,
    HBM16_DWORD1_BV_GEN_CONFIGr,
    HBM16_DWORD1_DAC_CONFIGr,
    HBM16_DWORD1_IO_CONFIGr,
    HBM16_DWORD1_LANE_REMAPr,
    HBM16_DWORD1_LATENCY_SHIFTERr,
    HBM16_DWORD1_RDATA_FIFO_CONFIGr,
    HBM16_DWORD1_READ_MACHINE_CONFIGr,
    HBM16_DWORD1_READ_MAX_VDL_DQS_Nr,
    HBM16_DWORD1_READ_MAX_VDL_DQS_Pr,
    HBM16_DWORD1_READ_MAX_VDL_FSMr,
    HBM16_DWORD1_READ_MIN_VDL_DATAr,
    HBM16_DWORD1_REG_RESERVEDr,
    HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr,
    HBM16_DWORD1_STATUS_BV_GEN_ERRORr,
    HBM16_DWORD1_STATUS_BV_GEN_ERROR_BUSY_VALIDSr,
    HBM16_DWORD1_STATUS_BV_GEN_ERROR_RCMDSr,
    HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_LSBr,
    HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_MSBr,
    HBM16_DWORD1_STATUS_BV_GEN_LAST_BUSY_VALIDSr,
    HBM16_DWORD1_STATUS_BV_GEN_LAST_RCMDSr,
    HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_LSBr,
    HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_MSBr,
    HBM16_DWORD1_STATUS_LANE_REMAPr,
    HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_0r,
    HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_1r,
    HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_2r,
    HBM16_DWORD1_STATUS_RDATA_FIFO_PTRr,
    HBM16_DWORD1_STATUS_READ_MACHINEr,
    HBM16_DWORD1_STATUS_RESERVEDr,
    HBM16_DWORD1_STATUS_RXEN_RXBV_GENr,
    HBM16_DWORD1_WRITE_MAX_VDL_DATAr,
    HBM16_DWORD1_WRITE_MAX_VDL_DQSr,
    HBM16_DWORD2_BV_GEN_CONFIGr,
    HBM16_DWORD2_DAC_CONFIGr,
    HBM16_DWORD2_IO_CONFIGr,
    HBM16_DWORD2_LANE_REMAPr,
    HBM16_DWORD2_LATENCY_SHIFTERr,
    HBM16_DWORD2_RDATA_FIFO_CONFIGr,
    HBM16_DWORD2_READ_MACHINE_CONFIGr,
    HBM16_DWORD2_READ_MAX_VDL_DQS_Nr,
    HBM16_DWORD2_READ_MAX_VDL_DQS_Pr,
    HBM16_DWORD2_READ_MAX_VDL_FSMr,
    HBM16_DWORD2_READ_MIN_VDL_DATAr,
    HBM16_DWORD2_REG_RESERVEDr,
    HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr,
    HBM16_DWORD2_STATUS_BV_GEN_ERRORr,
    HBM16_DWORD2_STATUS_BV_GEN_ERROR_BUSY_VALIDSr,
    HBM16_DWORD2_STATUS_BV_GEN_ERROR_RCMDSr,
    HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_LSBr,
    HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_MSBr,
    HBM16_DWORD2_STATUS_BV_GEN_LAST_BUSY_VALIDSr,
    HBM16_DWORD2_STATUS_BV_GEN_LAST_RCMDSr,
    HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_LSBr,
    HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_MSBr,
    HBM16_DWORD2_STATUS_LANE_REMAPr,
    HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_0r,
    HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_1r,
    HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_2r,
    HBM16_DWORD2_STATUS_RDATA_FIFO_PTRr,
    HBM16_DWORD2_STATUS_READ_MACHINEr,
    HBM16_DWORD2_STATUS_RESERVEDr,
    HBM16_DWORD2_STATUS_RXEN_RXBV_GENr,
    HBM16_DWORD2_WRITE_MAX_VDL_DATAr,
    HBM16_DWORD2_WRITE_MAX_VDL_DQSr,
    HBM16_DWORD3_BV_GEN_CONFIGr,
    HBM16_DWORD3_DAC_CONFIGr,
    HBM16_DWORD3_IO_CONFIGr,
    HBM16_DWORD3_LANE_REMAPr,
    HBM16_DWORD3_LATENCY_SHIFTERr,
    HBM16_DWORD3_RDATA_FIFO_CONFIGr,
    HBM16_DWORD3_READ_MACHINE_CONFIGr,
    HBM16_DWORD3_READ_MAX_VDL_DQS_Nr,
    HBM16_DWORD3_READ_MAX_VDL_DQS_Pr,
    HBM16_DWORD3_READ_MAX_VDL_FSMr,
    HBM16_DWORD3_READ_MIN_VDL_DATAr,
    HBM16_DWORD3_REG_RESERVEDr,
    HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr,
    HBM16_DWORD3_STATUS_BV_GEN_ERRORr,
    HBM16_DWORD3_STATUS_BV_GEN_ERROR_BUSY_VALIDSr,
    HBM16_DWORD3_STATUS_BV_GEN_ERROR_RCMDSr,
    HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_LSBr,
    HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_MSBr,
    HBM16_DWORD3_STATUS_BV_GEN_LAST_BUSY_VALIDSr,
    HBM16_DWORD3_STATUS_BV_GEN_LAST_RCMDSr,
    HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_LSBr,
    HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_MSBr,
    HBM16_DWORD3_STATUS_LANE_REMAPr,
    HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_0r,
    HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_1r,
    HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_2r,
    HBM16_DWORD3_STATUS_RDATA_FIFO_PTRr,
    HBM16_DWORD3_STATUS_READ_MACHINEr,
    HBM16_DWORD3_STATUS_RESERVEDr,
    HBM16_DWORD3_STATUS_RXEN_RXBV_GENr,
    HBM16_DWORD3_WRITE_MAX_VDL_DATAr,
    HBM16_DWORD3_WRITE_MAX_VDL_DQSr,
    HBM16_AWORD_DAC_CONFIGr,
    HBM16_AWORD_IO_CONFIGr,
    HBM16_AWORD_LANE_REMAPr,
    HBM16_AWORD_LATENCY_SHIFTERr,
    HBM16_AWORD_MISR_CNTr,
    HBM16_AWORD_MISR_CONFIGr,
    HBM16_AWORD_MISR_INPUTr,
    HBM16_AWORD_REG_RESERVEDr,
    HBM16_AWORD_STATUS_RESERVEDr,
    HBM16_AWORD_WRITE_MAX_VDL_ADDRr,
    HBM16_AWORD_WRITE_MAX_VDL_CKr,
    HBM16_STATUS_AWORD_LANE_REMAPr,
    HBM16_STATUS_AWORD_MISRr,
    HBM16_STATUS_AWORD_MISR_CNTRr,
    HBM16_CONTROL_REG_RESERVEDr,
    HBM16_CONTROL_RESETSr,
    HBM16_CONTROL_STATUS_RESERVEDr,
    HBM16_CONTROL_WRITE_FIFO_CONFIGr,
    HBM16_CHANNEL_NUM_REGS
} soc_hbm16_channel_reg_t;

#define READ_HBM16_MIDSTACK_DAC_CONFIGr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_DAC_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_DAC_CONFIGr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_DAC_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_FREQ_CNTR_CONFIGr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_FREQ_CNTR_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_FREQ_CNTR_RST_CTRLr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_IO_CONFIGr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_IO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_IO_CONFIGr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_IO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_REG_RESERVEDr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_REG_RESERVEDr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr, REG_PORT_ANY, rv)

#define READ_HBM16_MIDSTACK_STATUS_RESERVEDr(unit, hbc_idx, rvp) \
        soc_hbm16_midstack_read(unit, hbc_idx, HBM16_MIDSTACK_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_MIDSTACK_STATUS_RESERVEDr(unit, hbc_idx, rv) \
        soc_hbm16_midstack_write(unit, hbc_idx, HBM16_MIDSTACK_STATUS_RESERVEDr, REG_PORT_ANY, rv)





#define READ_HBM16_DWORD0_BV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_BV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_BV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_BV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_DAC_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_DAC_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_DAC_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_DAC_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_IO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_IO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_IO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_IO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_LATENCY_SHIFTERr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_LATENCY_SHIFTERr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_LATENCY_SHIFTERr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_LATENCY_SHIFTERr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_READ_MACHINE_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_READ_MACHINE_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_READ_MAX_VDL_FSMr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_READ_MAX_VDL_FSMr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_READ_MIN_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_READ_MIN_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_REG_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_REG_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_READ_MACHINEr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_READ_MACHINEr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD0_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD0_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD0_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD0_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_BV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_BV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_BV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_BV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_DAC_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_DAC_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_DAC_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_DAC_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_IO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_IO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_IO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_IO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_LATENCY_SHIFTERr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_LATENCY_SHIFTERr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_LATENCY_SHIFTERr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_LATENCY_SHIFTERr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_READ_MACHINE_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_READ_MACHINE_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_READ_MAX_VDL_FSMr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_READ_MAX_VDL_FSMr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_READ_MIN_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_READ_MIN_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_REG_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_REG_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_READ_MACHINEr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_READ_MACHINEr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD1_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD1_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD1_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD1_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_BV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_BV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_BV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_BV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_DAC_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_DAC_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_DAC_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_DAC_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_IO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_IO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_IO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_IO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_LATENCY_SHIFTERr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_LATENCY_SHIFTERr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_LATENCY_SHIFTERr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_LATENCY_SHIFTERr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_READ_MACHINE_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_READ_MACHINE_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_READ_MAX_VDL_FSMr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_READ_MAX_VDL_FSMr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_READ_MIN_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_READ_MIN_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_REG_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_REG_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_READ_MACHINEr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_READ_MACHINEr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD2_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD2_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD2_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD2_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_BV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_BV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_BV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_BV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_DAC_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_DAC_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_DAC_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_DAC_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_IO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_IO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_IO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_IO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_LATENCY_SHIFTERr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_LATENCY_SHIFTERr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_LATENCY_SHIFTERr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_LATENCY_SHIFTERr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_RDATA_FIFO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_RDATA_FIFO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_READ_MACHINE_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_READ_MACHINE_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_READ_MACHINE_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Nr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_READ_MAX_VDL_DQS_Nr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_READ_MAX_VDL_DQS_Pr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_READ_MAX_VDL_DQS_Pr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_READ_MAX_VDL_FSMr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_READ_MAX_VDL_FSMr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_READ_MAX_VDL_FSMr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_READ_MIN_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_READ_MIN_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_READ_MIN_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_REG_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_REG_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_RXEN_RXBV_GEN_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_ERRORr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERRORr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_ERROR_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_ERROR_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_ERROR_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_LAST_BUSY_VALIDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_BUSY_VALIDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_LAST_RCMDSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_RCMDSr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_LSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_LSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_MSBr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_BV_GEN_LAST_STATES_MSBr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_0r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_0r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_1r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_1r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_2r(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_2r, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_RDATA_FIFO_PTRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RDATA_FIFO_PTRr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_READ_MACHINEr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_READ_MACHINEr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_READ_MACHINEr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_STATUS_RXEN_RXBV_GENr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_STATUS_RXEN_RXBV_GENr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DATAr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_WRITE_MAX_VDL_DATAr, REG_PORT_ANY, rv)

#define READ_HBM16_DWORD3_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_DWORD3_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_DWORD3_WRITE_MAX_VDL_DQSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_DWORD3_WRITE_MAX_VDL_DQSr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_DAC_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_DAC_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_DAC_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_DAC_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_IO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_IO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_IO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_IO_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_LATENCY_SHIFTERr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_LATENCY_SHIFTERr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_LATENCY_SHIFTERr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_LATENCY_SHIFTERr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_MISR_CNTr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_MISR_CNTr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_MISR_CNTr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_MISR_CNTr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_MISR_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_MISR_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_MISR_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_MISR_CONFIGr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_MISR_INPUTr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_MISR_INPUTr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_MISR_INPUTr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_MISR_INPUTr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_REG_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_REG_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_STATUS_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_STATUS_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_STATUS_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_WRITE_MAX_VDL_ADDRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_WRITE_MAX_VDL_ADDRr, REG_PORT_ANY, rv)

#define READ_HBM16_AWORD_WRITE_MAX_VDL_CKr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_AWORD_WRITE_MAX_VDL_CKr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_AWORD_WRITE_MAX_VDL_CKr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_AWORD_WRITE_MAX_VDL_CKr, REG_PORT_ANY, rv)

#define READ_HBM16_STATUS_AWORD_LANE_REMAPr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_STATUS_AWORD_LANE_REMAPr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_STATUS_AWORD_LANE_REMAPr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_STATUS_AWORD_LANE_REMAPr, REG_PORT_ANY, rv)

#define READ_HBM16_STATUS_AWORD_MISRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_STATUS_AWORD_MISRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_STATUS_AWORD_MISRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_STATUS_AWORD_MISRr, REG_PORT_ANY, rv)

#define READ_HBM16_STATUS_AWORD_MISR_CNTRr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_STATUS_AWORD_MISR_CNTRr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_STATUS_AWORD_MISR_CNTRr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_STATUS_AWORD_MISR_CNTRr, REG_PORT_ANY, rv)

#define READ_HBM16_CONTROL_REG_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_CONTROL_REG_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_CONTROL_REG_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_CONTROL_REG_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_CONTROL_RESETSr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_CONTROL_RESETSr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_CONTROL_RESETSr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_CONTROL_RESETSr, REG_PORT_ANY, rv)

#define READ_HBM16_CONTROL_STATUS_RESERVEDr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_CONTROL_STATUS_RESERVEDr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_CONTROL_STATUS_RESERVEDr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_CONTROL_STATUS_RESERVEDr, REG_PORT_ANY, rv)

#define READ_HBM16_CONTROL_WRITE_FIFO_CONFIGr(unit, hbc_idx, channel, rvp) \
        soc_hbm16_channel_read(unit, hbc_idx, channel, HBM16_CONTROL_WRITE_FIFO_CONFIGr, REG_PORT_ANY, rvp)
#define WRITE_HBM16_CONTROL_WRITE_FIFO_CONFIGr(unit, hbc_idx, channel, rv) \
        soc_hbm16_channel_write(unit, hbc_idx, channel, HBM16_CONTROL_WRITE_FIFO_CONFIGr, REG_PORT_ANY, rv)

#endif /* _SOC_HBMPHYREGS_H */
