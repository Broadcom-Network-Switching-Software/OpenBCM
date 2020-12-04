/** \file src/soc/common/hbm_phy_regs.c
 *  
 * common HBM Phy register and fields definitions.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** 
 *  The order of the members of both
 *  soc_hbm16_midstack_reg_list and
 *  soc_hbm16_channel_reg_list must be aligned to the enums
 *  found in include/soc/hbm_phy_regs.h:
 *  soc_hbm16_midstack_reg_e and soc_hbm16_channel_reg_e
 */

#include <soc/field.h>
#include <soc/types.h>
#include <soc/hbm_phy_regs.h>

soc_field_info_t soc_hbm16_MIDSTACK_DAC_CONFIGr_fields[] = {
    { HBM16_DAC_DATAf, 4, 0, SOCF_LE },
    { HBM16_DAC_PDf, 1, 4, 0 }
};

soc_field_info_t soc_hbm16_MIDSTACK_FREQ_CNTR_CONFIGr_fields[] = {
    { HBM16_EN_NLDL_CLKOUT_BARf, 1, 26, 0 },
    { HBM16_OBS_INTERVALf, 16, 0, SOCF_LE },
    { HBM16_RO_VDL_STEPf, 9, 16, SOCF_LE },
    { HBM16_SEL_FC_REFCLKf, 1, 25, 0 },
    { HBM16_SEL_MIDSTACK_2G_WCLK_FOR_NLDL_CLKf, 1, 31, 0 },
    { HBM16_START_OBSf, 1, 27, 0 },
    { HBM16_TEST_PIN_DIV_SELf, 3, 28, SOCF_LE }
};

soc_field_info_t soc_hbm16_MIDSTACK_FREQ_CNTR_RST_CTRLr_fields[] = {
    { HBM16_FREQ_CNTR_FC_RESET_Nf, 1, 0, 0 },
    { HBM16_FREQ_CNTR_RO_RESET_Nf, 1, 1, 0 }
};

soc_field_info_t soc_hbm16_MIDSTACK_IO_CONFIGr_fields[] = {
    { HBM16_RESET_DRIVE_STRENGTHf, 3, 0, SOCF_LE },
    { HBM16_RXENB_CATTRIPf, 1, 4, 0 },
    { HBM16_RXENB_TEMPf, 1, 3, 0 }
};

soc_field_info_t soc_hbm16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr_fields[] = {
    { HBM16_DCOUNTf, 16, 0, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr_fields[] = {
    { HBM16_RO_OVERFLOWf, 3, 24, SOCF_LE|SOCF_RO },
    { HBM16_RO_UCOUNTf, 24, 0, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_BV_GEN_CONFIGr_fields[] = {
    { HBM16_ENABLEf, 1, 0, 0 },
    { HBM16_ERROR_CLEARf, 1, 1, 0 }
};

soc_field_info_t soc_hbm16_DWORD0_IO_CONFIGr_fields[] = {
    { HBM16_DIFF_PORT_DQS_DRIVE_STRENGTHf, 3, 0, SOCF_LE },
    { HBM16_RXENB_DERRf, 1, 7, 0 },
    { HBM16_RXENB_RDQSf, 1, 6, 0 },
    { HBM16_SINGLE_PORT_DQ_DRIVE_STRENGTHf, 3, 3, SOCF_LE }
};

soc_field_info_t soc_hbm16_DWORD0_LANE_REMAPr_fields[] = {
    { HBM16_BYTE0f, 4, 0, SOCF_LE },
    { HBM16_BYTE1f, 4, 4, SOCF_LE },
    { HBM16_BYTE2f, 4, 8, SOCF_LE },
    { HBM16_BYTE3f, 4, 12, SOCF_LE },
    { HBM16_SEL0f, 1, 16, 0 },
    { HBM16_SEL1f, 1, 17, 0 },
    { HBM16_SEL2f, 1, 18, 0 },
    { HBM16_SEL3f, 1, 19, 0 }
};

soc_field_info_t soc_hbm16_DWORD0_LATENCY_SHIFTERr_fields[] = {
    { HBM16_DATA_PATH_ADDITIONAL_LATENCYf, 3, 6, SOCF_LE },
    { HBM16_DATA_PATH_SHIFT_ENABLEf, 1, 9, 0 },
    { HBM16_RCMD_ADDITIONAL_LATENCYf, 5, 0, SOCF_LE },
    { HBM16_RCMD_SHIFT_ENABLEf, 1, 5, 0 }
};

soc_field_info_t soc_hbm16_DWORD0_RDATA_FIFO_CONFIGr_fields[] = {
    { HBM16_CLEARf, 1, 4, 0 },
    { HBM16_OBS_PTRf, 4, 0, SOCF_LE }
};

soc_field_info_t soc_hbm16_DWORD0_READ_MACHINE_CONFIGr_fields[] = {
    { HBM16_CONTINUOUS_MODEf, 1, 1, 0 },
    { HBM16_ENABLEf, 1, 0, 0 },
    { HBM16_PHASE_FIFO_MIN_DELAY_2f, 1, 2, 0 },
    { HBM16_RDATA_FIFO_WR2RD_DELAYf, 3, 4, SOCF_LE }
};

soc_field_info_t soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields[] = {
    { HBM16_MAX_VDL_STEPf, 9, 0, SOCF_LE }
};

soc_field_info_t soc_hbm16_DWORD0_READ_MIN_VDL_DATAr_fields[] = {
    { HBM16_MIN_VDL_STEPf, 5, 0, SOCF_LE }
};

soc_field_info_t soc_hbm16_DWORD0_RXEN_RXBV_GEN_CONFIGr_fields[] = {
    { HBM16_AUTO_UPDATEf, 1, 1, 0 },
    { HBM16_ENABLEf, 1, 0, 0 },
    { HBM16_FORCE_RXENf, 1, 5, 0 },
    { HBM16_FORCE_RX_BUSYf, 1, 3, 0 },
    { HBM16_FORCE_RX_VALIDf, 1, 4, 0 },
    { HBM16_FORCE_UPDATEf, 1, 2, 0 },
    { HBM16_PAR_LATENCYf, 3, 17, SOCF_LE },
    { HBM16_RD_DATA_DELAYf, 5, 12, SOCF_LE },
    { HBM16_RXEN_POST_CYCLESf, 5, 25, SOCF_LE },
    { HBM16_RXEN_PRE_CYCLESf, 5, 20, SOCF_LE }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERRORr_fields[] = {
    { HBM16_CURRENT_ERRORf, 1, 0, SOCF_RO },
    { HBM16_STICKY_ERRORf, 1, 4, SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields[] = {
    { HBM16_BV_00f, 2, 0, SOCF_LE|SOCF_RO },
    { HBM16_BV_01f, 2, 2, SOCF_LE|SOCF_RO },
    { HBM16_BV_02f, 2, 4, SOCF_LE|SOCF_RO },
    { HBM16_BV_03f, 2, 6, SOCF_LE|SOCF_RO },
    { HBM16_BV_04f, 2, 8, SOCF_LE|SOCF_RO },
    { HBM16_BV_05f, 2, 10, SOCF_LE|SOCF_RO },
    { HBM16_BV_06f, 2, 12, SOCF_LE|SOCF_RO },
    { HBM16_BV_07f, 2, 14, SOCF_LE|SOCF_RO },
    { HBM16_BV_08f, 2, 16, SOCF_LE|SOCF_RO },
    { HBM16_BV_09f, 2, 18, SOCF_LE|SOCF_RO },
    { HBM16_BV_10f, 2, 20, SOCF_LE|SOCF_RO },
    { HBM16_BV_11f, 2, 22, SOCF_LE|SOCF_RO },
    { HBM16_BV_12f, 2, 24, SOCF_LE|SOCF_RO },
    { HBM16_BV_13f, 2, 26, SOCF_LE|SOCF_RO },
    { HBM16_BV_14f, 2, 28, SOCF_LE|SOCF_RO },
    { HBM16_BV_15f, 2, 30, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields[] = {
    { HBM16_RCMD_01f, 2, 0, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_02f, 2, 2, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_03f, 2, 4, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_04f, 2, 6, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_05f, 2, 8, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_06f, 2, 10, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_07f, 2, 12, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_08f, 2, 14, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_09f, 2, 16, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_10f, 2, 18, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_11f, 2, 20, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_12f, 2, 22, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_13f, 2, 24, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_14f, 2, 26, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_15f, 2, 28, SOCF_LE|SOCF_RO },
    { HBM16_RCMD_16f, 2, 30, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields[] = {
    { HBM16_STATE_00f, 4, 0, SOCF_LE|SOCF_RO },
    { HBM16_STATE_01f, 4, 4, SOCF_LE|SOCF_RO },
    { HBM16_STATE_02f, 4, 8, SOCF_LE|SOCF_RO },
    { HBM16_STATE_03f, 4, 12, SOCF_LE|SOCF_RO },
    { HBM16_STATE_04f, 4, 16, SOCF_LE|SOCF_RO },
    { HBM16_STATE_05f, 4, 20, SOCF_LE|SOCF_RO },
    { HBM16_STATE_06f, 4, 24, SOCF_LE|SOCF_RO },
    { HBM16_STATE_07f, 4, 28, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields[] = {
    { HBM16_STATE_08f, 4, 0, SOCF_LE|SOCF_RO },
    { HBM16_STATE_09f, 4, 4, SOCF_LE|SOCF_RO },
    { HBM16_STATE_10f, 4, 8, SOCF_LE|SOCF_RO },
    { HBM16_STATE_11f, 4, 12, SOCF_LE|SOCF_RO },
    { HBM16_STATE_12f, 4, 16, SOCF_LE|SOCF_RO },
    { HBM16_STATE_13f, 4, 20, SOCF_LE|SOCF_RO },
    { HBM16_STATE_14f, 4, 24, SOCF_LE|SOCF_RO },
    { HBM16_STATE_15f, 4, 28, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_LANE_REMAPr_fields[] = {
    { HBM16_BYTE0f, 4, 0, SOCF_LE|SOCF_RO },
    { HBM16_BYTE1f, 4, 4, SOCF_LE|SOCF_RO },
    { HBM16_BYTE2f, 4, 8, SOCF_LE|SOCF_RO },
    { HBM16_BYTE3f, 4, 12, SOCF_LE|SOCF_RO },
    { HBM16_DBI0f, 1, 16, SOCF_RO },
    { HBM16_DBI1f, 1, 17, SOCF_RO },
    { HBM16_DBI2f, 1, 18, SOCF_RO },
    { HBM16_DBI3f, 1, 19, SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r_fields[] = {
    { HBM16_DATA_UI0f, 32, 0, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r_fields[] = {
    { HBM16_DATA_UI1f, 32, 0, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r_fields[] = {
    { HBM16_DBI_UI0f, 4, 0, SOCF_LE|SOCF_RO },
    { HBM16_DBI_UI1f, 4, 4, SOCF_LE|SOCF_RO },
    { HBM16_DM_UI0f, 4, 8, SOCF_LE|SOCF_RO },
    { HBM16_DM_UI1f, 4, 12, SOCF_LE|SOCF_RO },
    { HBM16_PAR_UI0f, 1, 24, SOCF_RO },
    { HBM16_PAR_UI1f, 1, 28, SOCF_RO },
    { HBM16_RD_UI0f, 2, 16, SOCF_LE|SOCF_RO },
    { HBM16_RD_UI1f, 2, 20, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_RDATA_FIFO_PTRr_fields[] = {
    { HBM16_OVERFLOWf, 1, 20, SOCF_RO },
    { HBM16_RPTRf, 4, 4, SOCF_LE|SOCF_RO },
    { HBM16_RPTR_WRT_RCLKf, 4, 12, SOCF_LE|SOCF_RO },
    { HBM16_UNDERFLOWf, 1, 16, SOCF_RO },
    { HBM16_WPTRf, 4, 0, SOCF_LE|SOCF_RO },
    { HBM16_WPTR_WRT_RCLKf, 4, 8, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_READ_MACHINEr_fields[] = {
    { HBM16_RDATA_FIFO_WR2RD_DELAYf, 3, 4, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_DWORD0_STATUS_RXEN_RXBV_GENr_fields[] = {
    { HBM16_AUTO_UPDATE_COUNTERf, 7, 4, SOCF_LE|SOCF_RO },
    { HBM16_AUTO_UPDATE_READYf, 1, 1, SOCF_RO },
    { HBM16_PAR_LATENCYf, 3, 17, SOCF_LE|SOCF_RO },
    { HBM16_RD_DATA_DELAYf, 5, 12, SOCF_LE|SOCF_RO },
    { HBM16_RXEN_POST_CYCLESf, 5, 25, SOCF_LE|SOCF_RO },
    { HBM16_RXEN_PRE_CYCLESf, 5, 20, SOCF_LE|SOCF_RO },
    { HBM16_UPDATE_PENDINGf, 1, 0, SOCF_RO }
};

soc_field_info_t soc_hbm16_AWORD_DAC_CONFIGr_fields[] = {
    { HBM16_DAC_DATAf, 4, 0, SOCF_LE },
    { HBM16_DAC_PDf, 1, 4, 0 }
};

soc_field_info_t soc_hbm16_AWORD_IO_CONFIGr_fields[] = {
    { HBM16_DIFF_PORT_CK_DRIVE_STRENGTHf, 3, 0, SOCF_LE },
    { HBM16_RXENBf, 1, 6, 0 },
    { HBM16_RXENB_AERRf, 1, 7, 0 },
    { HBM16_SINGLE_PORT_AQ_DRIVE_STRENGTHf, 3, 3, SOCF_LE }
};

soc_field_info_t soc_hbm16_AWORD_LANE_REMAPr_fields[] = {
    { HBM16_CAf, 4, 0, SOCF_LE },
    { HBM16_RAf, 4, 4, SOCF_LE },
    { HBM16_SEL_CAf, 1, 8, 0 },
    { HBM16_SEL_RAf, 1, 9, 0 }
};

soc_field_info_t soc_hbm16_AWORD_LATENCY_SHIFTERr_fields[] = {
    { HBM16_DATA_PATH_ADDITIONAL_LATENCYf, 3, 0, SOCF_LE },
    { HBM16_DATA_PATH_SHIFT_ENABLEf, 1, 3, 0 }
};

soc_field_info_t soc_hbm16_AWORD_MISR_CNTr_fields[] = {
    { HBM16_INITf, 32, 0, SOCF_LE }
};

soc_field_info_t soc_hbm16_AWORD_MISR_CONFIGr_fields[] = {
    { HBM16_ENf, 1, 3, 0 },
    { HBM16_FINITEf, 1, 0, 0 },
    { HBM16_MODEf, 2, 1, SOCF_LE },
    { HBM16_STARTf, 1, 4, 0 }
};

soc_field_info_t soc_hbm16_AWORD_MISR_INPUTr_fields[] = {
    { HBM16_INITf, 30, 0, SOCF_LE }
};

soc_field_info_t soc_hbm16_AWORD_REG_RESERVEDr_fields[] = {
    { HBM16_RESERVEDf, 32, 0, SOCF_LE|SOCF_RES }
};

soc_field_info_t soc_hbm16_AWORD_STATUS_RESERVEDr_fields[] = {
    { HBM16_RESERVEDf, 32, 0, SOCF_LE|SOCF_RO|SOCF_RES }
};

soc_field_info_t soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields[] = {
    { HBM16_MAX_VDL_STEPf, 9, 0, SOCF_LE },
    { HBM16_UI_SHIFTf, 1, 12, 0 }
};

soc_field_info_t soc_hbm16_CONTROL_RESETSr_fields[] = {
    { HBM16_MISR_RSTBf, 1, 1, 0 },
    { HBM16_PHY_RSTB_1Gf, 1, 0, 0 }
};

soc_field_info_t soc_hbm16_CONTROL_WRITE_FIFO_CONFIGr_fields[] = {
    { HBM16_PHASE_FIFO_MIN_DELAY_2f, 1, 2, 0 }
};

soc_field_info_t soc_hbm16_STATUS_AWORD_LANE_REMAPr_fields[] = {
    { HBM16_CAf, 4, 0, SOCF_LE|SOCF_RO },
    { HBM16_RAf, 4, 4, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_STATUS_AWORD_MISRr_fields[] = {
    { HBM16_DONEf, 1, 0, SOCF_RO },
    { HBM16_OUTf, 30, 1, SOCF_LE|SOCF_RO }
};

soc_field_info_t soc_hbm16_STATUS_AWORD_MISR_CNTRr_fields[] = {
    { HBM16_CNTf, 32, 0, SOCF_LE|SOCF_RO }
};


soc_reg_info_t soc_hbm16_midstack_reg_list[] = {
    { /* SOC_REG_INT_MIDSTACK_DAC_CONFIGr */
        0,
        0,
        1,
        0,
        0x81002,
        0,
        0,
        2,
        soc_hbm16_MIDSTACK_DAC_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000008, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_FREQ_CNTR_CONFIGr */
        0,
        0,
        1,
        0,
        0x81001,
        0,
        0,
        7,
        soc_hbm16_MIDSTACK_FREQ_CNTR_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_FREQ_CNTR_RST_CTRLr */
        0,
        0,
        1,
        0,
        0x81000,
        0,
        0,
        2,
        soc_hbm16_MIDSTACK_FREQ_CNTR_RST_CTRLr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000003, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_IO_CONFIGr */
        0,
        0,
        1,
        0,
        0x81003,
        0,
        0,
        3,
        soc_hbm16_MIDSTACK_IO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x81004,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr */
        0,
        0,
        1,
        0,
        0x81005,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_MIDSTACK_STATUS_FREQ_CNTR_DCOUNTr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr */
        0,
        0,
        1,
        0,
        0x81006,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_MIDSTACK_STATUS_FREQ_CNTR_UCOUNTr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x07ffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_MIDSTACK_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x81007,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
};

soc_reg_info_t soc_hbm16_channel_reg_list[] = {
    { /* SOC_REG_INT_DWORD0_BV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80006,
        0,
        0,
        2,
        soc_hbm16_DWORD0_BV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000003, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_DAC_CONFIGr */
        0,
        0,
        1,
        0,
        0x8000d,
        0,
        0,
        2,
        soc_hbm16_AWORD_DAC_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000018, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_IO_CONFIGr */
        0,
        0,
        1,
        0,
        0x8000a,
        0,
        0,
        4,
        soc_hbm16_DWORD0_IO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x000000c0, 0x00000000)
        SOC_RESET_MASK_DEC(0x000000ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8000b,
        0,
        0,
        8,
        soc_hbm16_DWORD0_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x0000ffff, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_LATENCY_SHIFTERr */
        0,
        0,
        1,
        0,
        0x8000c,
        0,
        0,
        4,
        soc_hbm16_DWORD0_LATENCY_SHIFTERr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000003ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_RDATA_FIFO_CONFIGr */
        0,
        0,
        1,
        0,
        0x80009,
        0,
        0,
        2,
        soc_hbm16_DWORD0_RDATA_FIFO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_READ_MACHINE_CONFIGr */
        0,
        0,
        1,
        0,
        0x80008,
        0,
        0,
        4,
        soc_hbm16_DWORD0_READ_MACHINE_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000077, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_READ_MAX_VDL_DQS_Nr */
        0,
        0,
        1,
        0,
        0x80004,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_READ_MAX_VDL_DQS_Pr */
        0,
        0,
        1,
        0,
        0x80003,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_READ_MAX_VDL_FSMr */
        0,
        0,
        1,
        0,
        0x80005,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_READ_MIN_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80002,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MIN_VDL_DATAr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8001e,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_RXEN_RXBV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80007,
        0,
        0,
        10,
        soc_hbm16_DWORD0_RXEN_RXBV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff03f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_ERRORr */
        0,
        0,
        1,
        0,
        0x80013,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERRORr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000011, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80017,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr */
        0,
        0,
        1,
        0,
        0x80016,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x80014,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80015,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_LAST_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80012,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_LAST_RCMDSr */
        0,
        0,
        1,
        0,
        0x80011,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_LAST_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x8000f,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_BV_GEN_LAST_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80010,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8000e,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r */
        0,
        0,
        1,
        0,
        0x8001b,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r */
        0,
        0,
        1,
        0,
        0x8001c,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r */
        0,
        0,
        1,
        0,
        0x8001d,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x1133ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_RDATA_FIFO_PTRr */
        0,
        0,
        1,
        0,
        0x8001a,
        SOC_REG_FLAG_RO,
        0,
        6,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_PTRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0011ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_READ_MACHINEr */
        0,
        0,
        1,
        0,
        0x80019,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_READ_MACHINEr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000070, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8001f,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_STATUS_RXEN_RXBV_GENr */
        0,
        0,
        1,
        0,
        0x80018,
        SOC_REG_FLAG_RO,
        0,
        7,
        soc_hbm16_DWORD0_STATUS_RXEN_RXBV_GENr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff7f3, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_WRITE_MAX_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80000,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD0_WRITE_MAX_VDL_DQSr */
        0,
        0,
        1,
        0,
        0x80001,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_BV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80106,
        0,
        0,
        2,
        soc_hbm16_DWORD0_BV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000003, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_DAC_CONFIGr */
        0,
        0,
        1,
        0,
        0x8010d,
        0,
        0,
        2,
        soc_hbm16_AWORD_DAC_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000018, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_IO_CONFIGr */
        0,
        0,
        1,
        0,
        0x8010a,
        0,
        0,
        4,
        soc_hbm16_DWORD0_IO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x000000c0, 0x00000000)
        SOC_RESET_MASK_DEC(0x000000ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8010b,
        0,
        0,
        8,
        soc_hbm16_DWORD0_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x0000ffff, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_LATENCY_SHIFTERr */
        0,
        0,
        1,
        0,
        0x8010c,
        0,
        0,
        4,
        soc_hbm16_DWORD0_LATENCY_SHIFTERr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000003ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_RDATA_FIFO_CONFIGr */
        0,
        0,
        1,
        0,
        0x80109,
        0,
        0,
        2,
        soc_hbm16_DWORD0_RDATA_FIFO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_READ_MACHINE_CONFIGr */
        0,
        0,
        1,
        0,
        0x80108,
        0,
        0,
        4,
        soc_hbm16_DWORD0_READ_MACHINE_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000077, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_READ_MAX_VDL_DQS_Nr */
        0,
        0,
        1,
        0,
        0x80104,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_READ_MAX_VDL_DQS_Pr */
        0,
        0,
        1,
        0,
        0x80103,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_READ_MAX_VDL_FSMr */
        0,
        0,
        1,
        0,
        0x80105,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_READ_MIN_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80102,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MIN_VDL_DATAr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8011e,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_RXEN_RXBV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80107,
        0,
        0,
        10,
        soc_hbm16_DWORD0_RXEN_RXBV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff03f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_ERRORr */
        0,
        0,
        1,
        0,
        0x80113,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERRORr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000011, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_ERROR_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80117,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_ERROR_RCMDSr */
        0,
        0,
        1,
        0,
        0x80116,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_ERROR_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x80114,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_ERROR_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80115,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_LAST_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80112,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_LAST_RCMDSr */
        0,
        0,
        1,
        0,
        0x80111,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_LAST_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x8010f,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_BV_GEN_LAST_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80110,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8010e,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_0r */
        0,
        0,
        1,
        0,
        0x8011b,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_1r */
        0,
        0,
        1,
        0,
        0x8011c,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_RDATA_FIFO_OBS_CONTENTS_2r */
        0,
        0,
        1,
        0,
        0x8011d,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x1133ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_RDATA_FIFO_PTRr */
        0,
        0,
        1,
        0,
        0x8011a,
        SOC_REG_FLAG_RO,
        0,
        6,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_PTRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0011ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_READ_MACHINEr */
        0,
        0,
        1,
        0,
        0x80119,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_READ_MACHINEr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000070, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8011f,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_STATUS_RXEN_RXBV_GENr */
        0,
        0,
        1,
        0,
        0x80118,
        SOC_REG_FLAG_RO,
        0,
        7,
        soc_hbm16_DWORD0_STATUS_RXEN_RXBV_GENr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff7f3, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_WRITE_MAX_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80100,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD1_WRITE_MAX_VDL_DQSr */
        0,
        0,
        1,
        0,
        0x80101,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_BV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80206,
        0,
        0,
        2,
        soc_hbm16_DWORD0_BV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000003, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_DAC_CONFIGr */
        0,
        0,
        1,
        0,
        0x8020d,
        0,
        0,
        2,
        soc_hbm16_AWORD_DAC_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000018, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_IO_CONFIGr */
        0,
        0,
        1,
        0,
        0x8020a,
        0,
        0,
        4,
        soc_hbm16_DWORD0_IO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x000000c0, 0x00000000)
        SOC_RESET_MASK_DEC(0x000000ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8020b,
        0,
        0,
        8,
        soc_hbm16_DWORD0_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x0000ffff, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_LATENCY_SHIFTERr */
        0,
        0,
        1,
        0,
        0x8020c,
        0,
        0,
        4,
        soc_hbm16_DWORD0_LATENCY_SHIFTERr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000003ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_RDATA_FIFO_CONFIGr */
        0,
        0,
        1,
        0,
        0x80209,
        0,
        0,
        2,
        soc_hbm16_DWORD0_RDATA_FIFO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_READ_MACHINE_CONFIGr */
        0,
        0,
        1,
        0,
        0x80208,
        0,
        0,
        4,
        soc_hbm16_DWORD0_READ_MACHINE_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000077, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_READ_MAX_VDL_DQS_Nr */
        0,
        0,
        1,
        0,
        0x80204,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_READ_MAX_VDL_DQS_Pr */
        0,
        0,
        1,
        0,
        0x80203,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_READ_MAX_VDL_FSMr */
        0,
        0,
        1,
        0,
        0x80205,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_READ_MIN_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80202,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MIN_VDL_DATAr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8021e,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_RXEN_RXBV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80207,
        0,
        0,
        10,
        soc_hbm16_DWORD0_RXEN_RXBV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff03f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_ERRORr */
        0,
        0,
        1,
        0,
        0x80213,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERRORr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000011, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_ERROR_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80217,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_ERROR_RCMDSr */
        0,
        0,
        1,
        0,
        0x80216,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_ERROR_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x80214,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_ERROR_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80215,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_LAST_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80212,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_LAST_RCMDSr */
        0,
        0,
        1,
        0,
        0x80211,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_LAST_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x8020f,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_BV_GEN_LAST_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80210,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8020e,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_0r */
        0,
        0,
        1,
        0,
        0x8021b,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_1r */
        0,
        0,
        1,
        0,
        0x8021c,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_RDATA_FIFO_OBS_CONTENTS_2r */
        0,
        0,
        1,
        0,
        0x8021d,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x1133ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_RDATA_FIFO_PTRr */
        0,
        0,
        1,
        0,
        0x8021a,
        SOC_REG_FLAG_RO,
        0,
        6,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_PTRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0011ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_READ_MACHINEr */
        0,
        0,
        1,
        0,
        0x80219,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_READ_MACHINEr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000070, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8021f,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_STATUS_RXEN_RXBV_GENr */
        0,
        0,
        1,
        0,
        0x80218,
        SOC_REG_FLAG_RO,
        0,
        7,
        soc_hbm16_DWORD0_STATUS_RXEN_RXBV_GENr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff7f3, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_WRITE_MAX_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80200,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD2_WRITE_MAX_VDL_DQSr */
        0,
        0,
        1,
        0,
        0x80201,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_BV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80306,
        0,
        0,
        2,
        soc_hbm16_DWORD0_BV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000003, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_DAC_CONFIGr */
        0,
        0,
        1,
        0,
        0x8030d,
        0,
        0,
        2,
        soc_hbm16_AWORD_DAC_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000018, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_IO_CONFIGr */
        0,
        0,
        1,
        0,
        0x8030a,
        0,
        0,
        4,
        soc_hbm16_DWORD0_IO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x000000c0, 0x00000000)
        SOC_RESET_MASK_DEC(0x000000ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8030b,
        0,
        0,
        8,
        soc_hbm16_DWORD0_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x0000ffff, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_LATENCY_SHIFTERr */
        0,
        0,
        1,
        0,
        0x8030c,
        0,
        0,
        4,
        soc_hbm16_DWORD0_LATENCY_SHIFTERr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000003ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_RDATA_FIFO_CONFIGr */
        0,
        0,
        1,
        0,
        0x80309,
        0,
        0,
        2,
        soc_hbm16_DWORD0_RDATA_FIFO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_READ_MACHINE_CONFIGr */
        0,
        0,
        1,
        0,
        0x80308,
        0,
        0,
        4,
        soc_hbm16_DWORD0_READ_MACHINE_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000077, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_READ_MAX_VDL_DQS_Nr */
        0,
        0,
        1,
        0,
        0x80304,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_READ_MAX_VDL_DQS_Pr */
        0,
        0,
        1,
        0,
        0x80303,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_READ_MAX_VDL_FSMr */
        0,
        0,
        1,
        0,
        0x80305,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MAX_VDL_DQS_Nr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000001ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_READ_MIN_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80302,
        0,
        0,
        1,
        soc_hbm16_DWORD0_READ_MIN_VDL_DATAr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8031e,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_RXEN_RXBV_GEN_CONFIGr */
        0,
        0,
        1,
        0,
        0x80307,
        0,
        0,
        10,
        soc_hbm16_DWORD0_RXEN_RXBV_GEN_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff03f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_ERRORr */
        0,
        0,
        1,
        0,
        0x80313,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERRORr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000011, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_ERROR_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80317,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_ERROR_RCMDSr */
        0,
        0,
        1,
        0,
        0x80316,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_ERROR_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x80314,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_ERROR_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80315,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_LAST_BUSY_VALIDSr */
        0,
        0,
        1,
        0,
        0x80312,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_BUSY_VALIDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_LAST_RCMDSr */
        0,
        0,
        1,
        0,
        0x80311,
        SOC_REG_FLAG_RO,
        0,
        16,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_RCMDSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_LAST_STATES_LSBr */
        0,
        0,
        1,
        0,
        0x8030f,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_LSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_BV_GEN_LAST_STATES_MSBr */
        0,
        0,
        1,
        0,
        0x80310,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_BV_GEN_ERROR_STATES_MSBr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x8030e,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000fffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_0r */
        0,
        0,
        1,
        0,
        0x8031b,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_0r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_1r */
        0,
        0,
        1,
        0,
        0x8031c,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_1r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_RDATA_FIFO_OBS_CONTENTS_2r */
        0,
        0,
        1,
        0,
        0x8031d,
        SOC_REG_FLAG_RO,
        0,
        8,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_OBS_CONTENTS_2r_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x1133ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_RDATA_FIFO_PTRr */
        0,
        0,
        1,
        0,
        0x8031a,
        SOC_REG_FLAG_RO,
        0,
        6,
        soc_hbm16_DWORD0_STATUS_RDATA_FIFO_PTRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0011ffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_READ_MACHINEr */
        0,
        0,
        1,
        0,
        0x80319,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_DWORD0_STATUS_READ_MACHINEr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000070, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x8031f,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_STATUS_RXEN_RXBV_GENr */
        0,
        0,
        1,
        0,
        0x80318,
        SOC_REG_FLAG_RO,
        0,
        7,
        soc_hbm16_DWORD0_STATUS_RXEN_RXBV_GENr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3ffff7f3, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_WRITE_MAX_VDL_DATAr */
        0,
        0,
        1,
        0,
        0x80300,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_DWORD3_WRITE_MAX_VDL_DQSr */
        0,
        0,
        1,
        0,
        0x80301,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_DAC_CONFIGr */
        0,
        0,
        1,
        0,
        0x80a04,
        0,
        0,
        2,
        soc_hbm16_AWORD_DAC_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000018, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_IO_CONFIGr */
        0,
        0,
        1,
        0,
        0x80a02,
        0,
        0,
        4,
        soc_hbm16_AWORD_IO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x000000c0, 0x00000000)
        SOC_RESET_MASK_DEC(0x000000ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x80a03,
        0,
        0,
        4,
        soc_hbm16_AWORD_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x000000ff, 0x00000000)
        SOC_RESET_MASK_DEC(0x000003ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_LATENCY_SHIFTERr */
        0,
        0,
        1,
        0,
        0x80a05,
        0,
        0,
        2,
        soc_hbm16_AWORD_LATENCY_SHIFTERr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000000f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_MISR_CNTr */
        0,
        0,
        1,
        0,
        0x80a08,
        0,
        0,
        1,
        soc_hbm16_AWORD_MISR_CNTr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_MISR_CONFIGr */
        0,
        0,
        1,
        0,
        0x80a06,
        0,
        0,
        4,
        soc_hbm16_AWORD_MISR_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x0000001f, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_MISR_INPUTr */
        0,
        0,
        1,
        0,
        0x80a07,
        0,
        0,
        1,
        soc_hbm16_AWORD_MISR_INPUTr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x3fffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x80a09,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x80a0d,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_WRITE_MAX_VDL_ADDRr */
        0,
        0,
        1,
        0,
        0x80a01,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_AWORD_WRITE_MAX_VDL_CKr */
        0,
        0,
        1,
        0,
        0x80a00,
        0,
        0,
        2,
        soc_hbm16_AWORD_WRITE_MAX_VDL_ADDRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x000011ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_STATUS_AWORD_LANE_REMAPr */
        0,
        0,
        1,
        0,
        0x80a0a,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_STATUS_AWORD_LANE_REMAPr_fields,
        SOC_RESET_VAL_DEC(0x000000ff, 0x00000000)
        SOC_RESET_MASK_DEC(0x000000ff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_STATUS_AWORD_MISRr */
        0,
        0,
        1,
        0,
        0x80a0b,
        SOC_REG_FLAG_RO,
        0,
        2,
        soc_hbm16_STATUS_AWORD_MISRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x7fffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_STATUS_AWORD_MISR_CNTRr */
        0,
        0,
        1,
        0,
        0x80a0c,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_STATUS_AWORD_MISR_CNTRr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_CONTROL_REG_RESERVEDr */
        0,
        0,
        1,
        0,
        0x80c02,
        0,
        0,
        1,
        soc_hbm16_AWORD_REG_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_CONTROL_RESETSr */
        0,
        0,
        1,
        0,
        0x80c00,
        0,
        0,
        2,
        soc_hbm16_CONTROL_RESETSr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000003, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_CONTROL_STATUS_RESERVEDr */
        0,
        0,
        1,
        0,
        0x80c03,
        SOC_REG_FLAG_RO,
        0,
        1,
        soc_hbm16_AWORD_STATUS_RESERVEDr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0xffffffff, 0x00000000)
        -1,
        -1,
    },
    { /* SOC_REG_INT_CONTROL_WRITE_FIFO_CONFIGr */
        0,
        0,
        1,
        0,
        0x80c01,
        0,
        0,
        1,
        soc_hbm16_CONTROL_WRITE_FIFO_CONFIGr_fields,
        SOC_RESET_VAL_DEC(0x00000000, 0x00000000)
        SOC_RESET_MASK_DEC(0x00000004, 0x00000000)
        -1,
        -1,
    },
};


