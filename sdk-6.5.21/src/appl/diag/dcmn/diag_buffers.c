
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_buffers.c
 * Purpose: Manages buffers diagnostics functions 
 */


#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/diag_buffers.h>
#include <appl/diag/diag.h>

#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/utils.h>

#include <soc/dpp/JER/jer_ocb_dram_buffers.h>

#define UINT64_HEXA_STR_MAX_SIZE 20

/*
 * Functions
 */
#ifdef BCM_JERICHO_SUPPORT
cmd_result_t
cmd_dpp_diag_buffers_static(int unit, args_t *a) {
    ARAD_MGMT_INIT  *arad_init;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs;
    int i;
    uint32 field32_val, reg32_val, field_start, field_end, buffer_size;
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above64_val;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;

    cli_out("Configuration parameters:\n");
    cli_out("\tPDM mode = %d\n", arad_init->dram.pdm_mode);
    cli_out("\tTotal Ram size = %d MB\n", arad_init->dram.dram_size_total_mbyte);
    cli_out("\tNumber of Drams = %d \n", arad_init->drc_info.dram_num);
    cli_out("\n");

    cli_out("SW DB calculation:\n");
    cli_out("\tOCB_0 size = %d\n", dbuffs->ocb_0.size);
    cli_out("\tOCB_1 size = %d\n", dbuffs->ocb_1.size);
    cli_out("\tFMC_0 size = %d\n", dbuffs->fmc_0.size);
    cli_out("\tFMC_1 size = %d\n", dbuffs->fmc_1.size);
    cli_out("\tFBC_FMC_0 size = %d\n", dbuffs->fbc_fmc_0.size);
    cli_out("\tFBC_FMC_1 size = %d\n", dbuffs->fbc_fmc_1.size);
    cli_out("\tMNMC_0 size = %d\n", dbuffs->mnmc_0.size);
    cli_out("\tMNMC_1 size = %d\n", dbuffs->mnmc_1.size);
    cli_out("\tFBC_MNMC_0 size = %d\n", dbuffs->fbc_mnmc_0.size);
    cli_out("\tFBC_MNMC_1 size = %d\n", dbuffs->fbc_mnmc_1.size);
    cli_out("\n");

    cli_out("Buffer Range Register value:\n");
    if(soc_reg64_get(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, REG_PORT_ANY, 0, &reg64_val) < 0) return CMD_FAIL;
    field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, reg64_val, OCB_0_BUFF_PTR_STARTf);
    field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_OCB_BUF_RANGE_0r, reg64_val, OCB_0_BUFF_PTR_ENDf);
    cli_out("\tECI_GLOBAL_OCB_BUF_RANGE_0. Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);

    if(soc_reg64_get(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, REG_PORT_ANY, 0, &reg64_val) < 0) return CMD_FAIL;
    field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, reg64_val, OCB_1_BUFF_PTR_STARTf);
    field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_OCB_BUF_RANGE_1r, reg64_val, OCB_1_BUFF_PTR_ENDf);
    cli_out("\tECI_GLOBAL_OCB_BUF_RANGE_1. Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);
  
    if(soc_reg64_get(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, &reg64_val) < 0) return CMD_FAIL;
    field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, reg64_val, FULL_MULTICAST_0_DB_PTR_STARTf);
    field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_0r, reg64_val, FULL_MULTICAST_0_DB_PTR_ENDf);
    cli_out("\tFULL_MULTICAST_0_DB_PTR_START. Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);

    if(soc_reg64_get(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, &reg64_val) < 0) return CMD_FAIL;
    field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, reg64_val, FULL_MULTICAST_1_DB_PTR_STARTf);
    field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_FULL_MC_DB_RANGE_1r, reg64_val, FULL_MULTICAST_1_DB_PTR_ENDf);
    cli_out("\tFULL_MULTICAST_1_DB_PTR_START. Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);

    if(soc_reg64_get(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, REG_PORT_ANY, 0, &reg64_val) < 0) return CMD_FAIL;
    field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, reg64_val, MINI_MULTICAST_0_DB_PTR_STARTf);
    field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_0r, reg64_val, MINI_MULTICAST_0_DB_PTR_ENDf);
    cli_out("\tECI_GLOBAL_MINI_MC_DB_RANGE_0. Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);

    if(soc_reg64_get(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, REG_PORT_ANY, 0, &reg64_val) < 0) return CMD_FAIL;
    field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, reg64_val, MINI_MULTICAST_1_DB_PTR_STARTf);
    field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_MINI_MC_DB_RANGE_1r, reg64_val, MINI_MULTICAST_1_DB_PTR_ENDf);
    cli_out("\tECI_GLOBAL_MINI_MC_DB_RANGE_1. Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);
    cli_out("\n");

    cli_out("FBC Range Register value:\n");
    for (i = 0; i < 2; ++i) {
        if (soc_reg_above_64_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, i, reg_above64_val) < 0) return CMD_FAIL; 
        field_start = soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf);
        field_end = soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf);
        cli_out("\tIDR_FBC_EXTERNAL_CONFIGURATION, FULL_MULTICAST.%d Low=0x%x, High=0x%x (size=%d)\n", i, field_start, field_end, field_end - field_start + 1);
        field_start = soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf);
        field_end = soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf);
        cli_out("\tIDR_FBC_EXTERNAL_CONFIGURATION, MINI_MULTICAST.%d Low=0x%x, High=0x%x (size=%d)\n", i, field_start, field_end, field_end - field_start + 1);
        cli_out("\n");
    }

    cli_out("BANK configuration Register value:\n");
    for (i = 0; i < 2; i++) {
        if(soc_reg32_get(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, REG_PORT_ANY, i, &reg32_val) < 0)
            return CMD_FAIL;
        field32_val = soc_reg_field_get(unit, IDR_FBC_BANK_IS_USED_BY_OCBr, reg32_val, FBC_BANK_IS_USED_BY_OCB_Nf);
        cli_out("\tFBC_BANK_IS_USED_BY_OCB_N.%d = 0x%x\n", i, field32_val);
    }
    for (i = 0; i < JER_OCB_BANK_NUM; i++) {
        if(soc_reg64_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, REG_PORT_ANY, i, &reg64_val) < 0)
            return CMD_FAIL;
        field_start = soc_reg64_field32_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, reg64_val, OCB_BUFF_START_BANK_Nf);
        field_end = soc_reg64_field32_get(unit, ECI_GLOBAL_OCB_BANK_RANGEr, reg64_val, OCB_BUFF_END_BANK_Nf);
        cli_out("\tOCB_BUFF_START_BANK_N.%d = 0x%x. OCB_BUFF_END_BANK_N.%d = 0x%x\n", i, field_start, i, field_end);
    }    

    if(soc_reg32_get(unit, IQMT_MEM_BANK_SELECTr, REG_PORT_ANY, 0, &reg32_val) < 0)
        return CMD_FAIL;
    field32_val = soc_reg_field_get(unit, IQMT_MEM_BANK_SELECTr, reg32_val, FLUSCNT_BANK_SELECTf);
    cli_out("\tFLUSCNT_BANK_SELECT = 0x%x\n", field32_val);

    field32_val = soc_reg_field_get(unit, IQMT_MEM_BANK_SELECTr, reg32_val, MNUSCNT_BANK_SELECTf);
    cli_out("\tMNUSCNT_BANK_SELECT = 0x%x\n", field32_val);

    if(soc_reg32_get(unit, IQMT_BANK_SELECTr, REG_PORT_ANY, 0, &reg32_val) < 0)
        return CMD_FAIL;
    field32_val = soc_reg_field_get(unit, IQMT_BANK_SELECTr, reg32_val, BDB_BANK_SELECTf);
    cli_out("\tBDB_BANK_SELECT = 0x%x\n", field32_val);

    field32_val = soc_reg_field_get(unit, IQMT_BANK_SELECTr, reg32_val, PDM_BANK_SELECTf);
    cli_out("\tPDM_BANK_SELECT = 0x%x\n", field32_val);
    cli_out("\n");

    cli_out("Buffer size Register value:\n");
    if(soc_reg32_get(unit, EGQ_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, &reg32_val) < 0)
        return CMD_FAIL;
    field32_val = soc_reg_field_get(unit, EGQ_GLOBAL_GENERAL_CFG_3r, reg32_val, DRAM_BUFF_SIZEf);
    switch(field32_val){
    case 0:
        buffer_size = SOC_TMC_ITM_DBUFF_SIZE_BYTES_256;
        break;
    case 1:
        buffer_size = SOC_TMC_ITM_DBUFF_SIZE_BYTES_512;
        break;
    case 2:
        buffer_size = SOC_TMC_ITM_DBUFF_SIZE_BYTES_1024;
        break;
    case 3:
        buffer_size = SOC_TMC_ITM_DBUFF_SIZE_BYTES_2048;
        break;
    default:
        cli_out("Error: DRAM_BUFF_SIZEf=%d\n", field32_val);
        return CMD_FAIL;
    }
    cli_out("\tDRAM_BUFF_SIZE = 0x%x. buffer size=%d\n", field32_val, buffer_size);

    field32_val = soc_reg_field_get(unit, EGQ_GLOBAL_GENERAL_CFG_3r, reg32_val, OCB_BUFF_SIZEf);
        switch(field32_val){
    case 0:
        buffer_size = 256;
        break;
    case 1:
        buffer_size = 512;
        break;
    default:
        cli_out("Error: OCB_BUFF_SIZEf=%d\n", field32_val);
        return CMD_FAIL;
    }
    cli_out("\tOCB_BUFF_SIZE = 0x%x. buffer size=%d\n", field32_val, buffer_size);
    cli_out("\n");

    cli_out("OCB multicast Register value:\n");
    for (i = 0; i < 2; i++) {
        if(soc_reg64_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, REG_PORT_ANY, i, &reg64_val) < 0)
            return CMD_FAIL;
        field_start = soc_reg64_field32_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, reg64_val, OCB_ELIGIBLE_MULTICAST_RANGE_N_LOWf);
        field_end = soc_reg64_field32_get(unit, IDR_OCB_ELIGIBLE_MULTICAST_RANGEr, reg64_val, OCB_ELIGIBLE_MULTICAST_RANGE_N_HIGHf);
        cli_out("\tIDR_OCB_ELIGIBLE_MULTICAST_RANGE.%d. Low=0x%x, High=0x%x\n", i, field_start, field_end);
    }
    for (i = 0; i < 2; i++) {
        if(soc_reg64_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, REG_PORT_ANY, i, &reg64_val) < 0)
            return CMD_FAIL;
        field_start = soc_reg64_field32_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_LOWf);
        field_end = soc_reg64_field32_get(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, reg64_val, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf);
        cli_out("\tIDR_OCB_COMMITTED_MULTICAST_RANGE.%d. Low=0x%x, High=0x%x\n", i, field_start, field_end);
    }
    cli_out("\n");

    return CMD_OK;
}
#endif /*  BCM_JERICHO_SUPPORT */

cmd_result_t
cmd_dpp_diag_buffers_dynamic(int unit, args_t *a) {

    int field_iter, nof_fields;
    uint32 field32_val, reg32_val;
    uint64 field64_val, reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    char uint64_hexa_string[UINT64_HEXA_STR_MAX_SIZE];

    soc_field_t IDR_BUFFER_SELECTION_DECISIONr_fields[] = { DBG_PACKET_DESTINATIONf, DBG_PACKET_IS_UNICASTf, DBG_PACKET_IS_INGRESS_MULTICASTf, DBG_PACKET_IS_EGRESS_MULTICASTf, DBG_QUEUE_IS_OCB_ONLYf, 
                                                            DBG_MCID_IS_OCB_ONLYf, DBG_REASSEMBLY_CONTEXT_IS_OCB_ONLYf, DBG_TRAFFIC_CLASS_IS_OCB_ONLYf, DBG_WANTED_BUFFER_TYPE_IS_OCB_ONLYf, 
                                                            DBG_OCB_ONLY_BUFFER_IS_AVAILABLEf, DBG_BUFFER_TYPE_IS_OCB_ONLYf, DBG_QUEUE_IS_OCB_ELIGIBLEf, DBG_MCID_IS_OCB_ELIGIBLEf, DBG_PACKET_SNOOP_CMDf, 
                                                            DBG_PACKET_MIRROR_CMDf, DBG_SNOOP_IS_OCB_ELIGIBLEf, DBG_MIRROR_IS_OCB_ELIGIBLEf, DBG_OCB_SHAPER_IS_ENBALEDf, 
                                                            DBG_WANTED_BUFFER_TYPE_IS_OCB_MIXf, DBG_OCB_MIX_BUFFER_IS_AVAILABLEf, DBG_BUFFER_TYPE_IS_OCB_MIXf, DBG_DRAM_BUFFER_TYPE_TABLE_KEYf, 
                                                            DBG_WANTED_BUFFER_TYPE_IS_FMCf, DBG_WANTED_BUFFER_TYPE_IS_MMCf, DBG_FMC_BUFFER_IS_AVAILABLEf, DBG_MMC_BUFFER_IS_AVAILABLEf, 
                                                            DBG_BUFFER_TYPE_IS_FMCf, DBG_BUFFER_TYPE_IS_MMCf, DBG_QUEUE_IS_FULL_DRAM_REJECTf, DBG_QUEUE_IS_PARTIAL_DRAM_REJECTf, 
                                                            DBG_MCID_IS_DRAM_REJECTf, DBG_MMU_IS_BLOCKEDf };

    soc_field_t IRR_IQM_ENQUEUE_COMMANDr_fields[] = { ENQ_CMD_2_ND_DESTf, ENQ_CMD_DBf, ENQ_CMD_DRAM_REJECTf, ENQ_CMD_DROPPf, ENQ_CMD_ECN_CAPf, ENQ_CMD_FWD_ACTf, ENQ_CMD_FWD_TYPEf, 
                                                      ENQ_CMD_HDR_APPEND_SIZE_PTRf, ENQ_CMD_HDR_DELTAf, ENQ_CMD_HDR_TRUNCATE_SIZEf, ENQ_CMD_IGNORE_CPf, ENQ_CMD_IRR_MCR_ENG_IDf, ENQ_CMD_IRR_Qf, 
                                                      ENQ_CMD_LAGHASHf, ENQ_CMD_LSTf, ENQ_CMD_MC_ERRf, ENQ_CMD_NUM_DBf, ENQ_CMD_OCB_ONLYf, ENQ_CMD_OUTLIFf, ENQ_CMD_PP_PORTf, ENQ_CMD_QNUMf, 
                                                      ENQ_CMD_Q_VALIDf, ENQ_CMD_RESOURCE_ERRf, ENQ_CMD_RSTUSRf, ENQ_CMD_SCH_CMP_PTRf, ENQ_CMD_SOPCf, ENQ_CMD_ST_VSQ_PTRf, ENQ_CMD_TCf, 
                                                      ENQ_CMD_VALID_BYTESf };

    if(soc_reg32_get(unit, OCB_OCB_WR_CTRr, REG_PORT_ANY, 0, &reg32_val) < 0)
        return CMD_FAIL;
    field32_val = soc_reg_field_get(unit, OCB_OCB_WR_CTRr, reg32_val, OCB_WR_CTRf);
    cli_out("OCB_WR_CTR = 0x%x\n", field32_val);

    if(soc_reg32_get(unit, OCB_OCB_RD_CTRr, REG_PORT_ANY, 0, &reg32_val) < 0)
        return CMD_FAIL;
    field32_val = soc_reg_field_get(unit, OCB_OCB_RD_CTRr, reg32_val, OCB_RD_CTRf);
    cli_out("OCB_RD_CTR = 0x%x\n", field32_val);

    if(soc_reg32_get(unit, OCB_OCB_CRC_ERRr, REG_PORT_ANY, 0, &reg32_val) < 0)
        return CMD_FAIL;
    field32_val = soc_reg_field_get(unit, OCB_OCB_CRC_ERRr, reg32_val, OCB_CRC_ERRf);
    cli_out("OCB_CRC_ERR = 0x%x\n", field32_val);

    /* IDR_BUFFER_SELECTION_DECISIONr */
    cli_out("IDR_BUFFER_SELECTION_DECISION:\n");
    if(soc_reg64_get(unit, IDR_BUFFER_SELECTION_DECISIONr, REG_PORT_ANY, 0, &reg64_val) < 0)
        return CMD_FAIL;
    
    nof_fields = sizeof(IDR_BUFFER_SELECTION_DECISIONr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg64_field32_get(unit, IDR_BUFFER_SELECTION_DECISIONr, reg64_val, IDR_BUFFER_SELECTION_DECISIONr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, IDR_BUFFER_SELECTION_DECISIONr_fields[field_iter]), field32_val);
    }


    /* IRR_IQM_ENQUEUE_COMMANDr */
    cli_out("\nIRR_IQM_ENQUEUE_COMMAND:\n");
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    if(soc_reg_above_64_get(unit, IDR_BUFFER_SELECTION_DECISIONr, REG_PORT_ANY, 0, reg_above_64_val) < 0)
    return CMD_FAIL;

    nof_fields = sizeof(IRR_IQM_ENQUEUE_COMMANDr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg_above_64_field32_get(unit, IRR_IQM_ENQUEUE_COMMANDr, reg_above_64_val, IRR_IQM_ENQUEUE_COMMANDr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, IRR_IQM_ENQUEUE_COMMANDr_fields[field_iter]), field32_val);
    }

    field64_val = soc_reg_above_64_field64_get(unit, IRR_IQM_ENQUEUE_COMMANDr, reg_above_64_val, ENQ_CMD_COUNTER_ACTIONf);
    _shr_format_uint64_hexa_string( field64_val, uint64_hexa_string);
    cli_out("\t%s = %s\n", SOC_FIELD_NAME(unit, ENQ_CMD_COUNTER_ACTIONf), uint64_hexa_string);

    return 0;

}

#ifdef BCM_QAX_SUPPORT
cmd_result_t
cmd_dpp_qax_diag_buffers_static(int unit, args_t *a) {
    ARAD_MGMT_INIT  *arad_init;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs;
    uint32 field_start, field_end;
    soc_reg_above_64_val_t reg_above64_val;

    arad_init = &SOC_DPP_CONFIG(unit)->arad->init;
    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;

    cli_out("Configuration parameters:\n");
    cli_out("\tTotal Dram size = %d MB\n", arad_init->dram.dram_size_total_mbyte);
    cli_out("\tNumber of Drams = %d \n", arad_init->drc_info.dram_num);
    cli_out("\n");

    cli_out("SW DB calculation:\n");
    cli_out("\tDram buffer size = %d\n", dbuffs->mnmc_0.size);
    cli_out("\tFBM size = %d\n", dbuffs->fbc_mnmc_0.size);
    cli_out("\n");

    cli_out("Buffer Range Register value:\n");

    if (soc_reg_above_64_get(unit, DDP_FBC_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above64_val) < 0) return CMD_FAIL; 
    field_start = soc_reg_above_64_field32_get(unit, DDP_FBC_CONFIGURATIONr, reg_above64_val, DATA_FIRST_POINTERf);
    field_end = soc_reg_above_64_field32_get(unit, DDP_FBC_CONFIGURATIONr, reg_above64_val, DATA_LAST_POINTERf);
    cli_out("\tDDP_FBC_CONFIGURATION, DATA Start=0x%x, End=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);
    cli_out("\n");

    cli_out("FBC Range Register value:\n");

    if (soc_reg_above_64_get(unit, DDP_FBC_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above64_val) < 0) return CMD_FAIL; 
    field_start = soc_reg_above_64_field32_get(unit, DDP_FBC_CONFIGURATIONr, reg_above64_val, FREE_BUFF_START_POINTERf);
    field_end = soc_reg_above_64_field32_get(unit, DDP_FBC_CONFIGURATIONr, reg_above64_val, FREE_BUFF_LAST_POINTERf);
    cli_out("\tDDP_FBC_CONFIGURATION, FREE_BUFF Low=0x%x, High=0x%x (size=%d)\n", field_start, field_end, field_end - field_start + 1);
    cli_out("\n");

    cli_out("Buffer size Register value:\n");

    cli_out("\tDRAM_BUFF_SIZE buffer size= %d\n", arad_init->dram.dbuff_size);
    cli_out("\tOCB_BUFF_SIZE buffer size= %d\n", arad_init->ocb.databuffer_size);

    cli_out("\n");

    return CMD_OK;
}

cmd_result_t
cmd_dpp_qax_diag_buffers_dynamic(int unit, args_t *a) {

    int field_iter, nof_fields;
    uint32 field32_val;
    uint64 field64_val, reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    char uint64_hexa_string[UINT64_HEXA_STR_MAX_SIZE];

    soc_field_t DDP_PACKET_ERROR_COUNTERSr_fields[] = {
        SPB_PACKET_ERROR_CNTf, SPB_GOOD_PACKET_CNTf, DRAM_PACKET_CRC_FLIPPED_ERROR_CNTf, DRAM_PACKET_CRC_ERROR_CNTf,
        DRAM_PACKET_MLM_DISCARD_CNTf, DRAM_PACKET_PKUP_DISCARD_CNTf, DRAM_GOOD_PACKET_CNTf, DRAM_PACKET_CPY_DAT_CRC_ERR_CNTf,
        DRAM_PACKET_CPY_DAT_ECC_1B_ERR_CNTf, DRAM_PACKET_CPY_DAT_ECC_2B_ERR_CNTf, PKUP_2_CGM_PKT_TX_RPRT_CNTf};

    soc_field_t SQM_LAST_ENQ_CMDr_fields[] = {
        LAST_ENQ_CMD_DPf, LAST_ENQ_CMD_ACTION_TYPEf, LAST_ENQ_CMD_ST_VSQ_PTRf, LAST_ENQ_CMD_IN_PP_PORTf,
        LAST_ENQ_CMD_ITPP_DELTAf, LAST_ENQ_CMD_OMAC_DELTAf, LAST_ENQ_CMD_CROPf, 
        LAST_ENQ_CMD_PKT_SIZEf, LAST_ENQ_CMD_START_BUFFERf, LAST_ENQ_CMD_QNUMf, LAST_ENQ_CMD_USR_CNT_DECf};

    soc_field_t SQM_LAST_DEQ_CMDr_fields[] = {
        LAST_DEQ_CMD_HPf, LAST_DEQ_CMD_FMCf, LAST_DEQ_CMD_DST_PORTf, LAST_DEQ_CMD_DST_DEVICEf,
        LAST_DEQ_CMD_SRAM_TO_DRAMf, LAST_DEQ_CMD_DELETEf, LAST_DEQ_CMD_CREDITSf, LAST_DEQ_CMD_QNUMf, 
        LAST_DEQ_CMD_QSIZEf, LAST_DEQ_CMD_QTYPEf, LAST_DEQ_CMD_UPDATEf};

    soc_field_t DQM_LAST_ENQ_CMDr_fields[] = {
        LAST_ENQ_CMD_QNUMf, LAST_ENQ_CMD_BUNDLE_DRAM_SIZEf, LAST_ENQ_CMD_BUNDLE_OMAC_SIZEf, LAST_ENQ_CMD_BUNDLE_SRAM_SIZEf,
        LAST_ENQ_CMD_BUFFER_Af, LAST_ENQ_CMD_BUFFER_Bf, LAST_ENQ_CMD_BUFFER_Cf};

    soc_field_t DQM_LAST_DEQ_CMDr_fields[] = {
        LAST_DEQ_CMD_FMCf, LAST_DEQ_CMD_HPf, LAST_DEQ_CMD_DST_DEVICEf, LAST_DEQ_CMD_DELETEf,
        LAST_DEQ_CMD_QNUMf, LAST_DEQ_CMD_QTYPEf};

    /* DDP_PACKET_ERROR_COUNTERSr */
    cli_out("\nDDP_PACKET_ERROR_COUNTERS:\n");
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    if(soc_reg_above_64_get(unit, DDP_PACKET_ERROR_COUNTERSr, REG_PORT_ANY, 0, reg_above_64_val) < 0)
    return CMD_FAIL;

    nof_fields = sizeof(DDP_PACKET_ERROR_COUNTERSr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg_above_64_field32_get(unit, DDP_PACKET_ERROR_COUNTERSr, reg_above_64_val, DDP_PACKET_ERROR_COUNTERSr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, DDP_PACKET_ERROR_COUNTERSr_fields[field_iter]), field32_val);
    }

    /* SQM_LAST_ENQ_CMDr */
    cli_out("\nSQM_LAST_ENQ_CMD:\n");
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    if(soc_reg_above_64_get(unit, SQM_LAST_ENQ_CMDr, REG_PORT_ANY, 0, reg_above_64_val) < 0)
    return CMD_FAIL;

    nof_fields = sizeof(SQM_LAST_ENQ_CMDr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg_above_64_field32_get(unit, SQM_LAST_ENQ_CMDr, reg_above_64_val, SQM_LAST_ENQ_CMDr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, SQM_LAST_ENQ_CMDr_fields[field_iter]), field32_val);
    }

    field64_val = soc_reg_above_64_field64_get(unit, SQM_LAST_ENQ_CMDr, reg_above_64_val, LAST_ENQ_CMD_MC_SNP_MRR_CUDf);
    _shr_format_uint64_hexa_string( field64_val, uint64_hexa_string);
    cli_out("\t%s = %s\n", SOC_FIELD_NAME(unit, LAST_ENQ_CMD_MC_SNP_MRR_CUDf), uint64_hexa_string);

    /* SQM_LAST_DEQ_CMDr */
    cli_out("\nSQM_LAST_DEQ_CMD:\n");
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    if(soc_reg_above_64_get(unit, SQM_LAST_DEQ_CMDr, REG_PORT_ANY, 0, reg_above_64_val) < 0)
    return CMD_FAIL;

    nof_fields = sizeof(SQM_LAST_DEQ_CMDr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg_above_64_field32_get(unit, SQM_LAST_DEQ_CMDr, reg_above_64_val, SQM_LAST_DEQ_CMDr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, SQM_LAST_DEQ_CMDr_fields[field_iter]), field32_val);
    }

    /* DQM_LAST_ENQ_CMDr */
    cli_out("\nDQM_LAST_ENQ_CMD:\n");
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    if(soc_reg_above_64_get(unit, DQM_LAST_ENQ_CMDr, REG_PORT_ANY, 0, reg_above_64_val) < 0)
    return CMD_FAIL;

    nof_fields = sizeof(DQM_LAST_ENQ_CMDr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg_above_64_field32_get(unit, DQM_LAST_ENQ_CMDr, reg_above_64_val, DQM_LAST_ENQ_CMDr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, DQM_LAST_ENQ_CMDr_fields[field_iter]), field32_val);
    }

    /* DQM_LAST_DEQ_CMDr */
    cli_out("\nDQM_LAST_DEQ_CMD:\n");
    if(soc_reg64_get(unit, DQM_LAST_DEQ_CMDr, REG_PORT_ANY, 0, &reg64_val) < 0)
        return CMD_FAIL;
    
    nof_fields = sizeof(DQM_LAST_DEQ_CMDr_fields)/sizeof(soc_field_t) ;
    for (field_iter = 0; field_iter < nof_fields; ++field_iter) {
        field32_val = soc_reg64_field32_get(unit, DQM_LAST_DEQ_CMDr, reg64_val, DQM_LAST_DEQ_CMDr_fields[field_iter]);
        cli_out("\t%s = 0x%x\n", SOC_FIELD_NAME(unit, DQM_LAST_DEQ_CMDr_fields[field_iter]), field32_val);
    }

    return 0;
}

#endif


/*
 * Print diag buffers usage
*/
void cmd_dpp_diag_buffers_usage(int unit) {
    char cmd_dpp_diag_buffers_usage[] =
       "Usage (DIAG buffers):"
       "\n\tDIAGnotsics buffers commands\n\t"
       "Usages:\n\t"
       "DIAG buffers <OPTION>"
#ifdef __PEDANTIC__
       "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else 
       "OPTION can be:"
       "\nstatic - \tDisplay static config"
       "\ndynamic - \tDisplay dynamic config"
       "\n";
#endif
          
    cli_out(cmd_dpp_diag_buffers_usage);
}

/*
 * "Driver" for the diag buffers functionalities.
*/
cmd_result_t
cmd_dpp_diag_buffers(int unit, args_t *a) {
    char      *function;
    int strnlen_function;

    if (!SOC_IS_JERICHO(unit)) {
        cli_out("Error: buffers Diag is not supported for this unit=%d\n", unit);
        return CMD_USAGE;
    }
    function = ARG_GET(a); 
    if (function)
    {
        strnlen_function = sal_strnlen(function,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if ((!function) || (!sal_strncasecmp(function, "static", strnlen_function))) {
#ifdef BCM_JERICHO_SUPPORT
#ifdef BCM_QAX_SUPPORT
        if (SOC_IS_QAX(unit)) {
            return cmd_dpp_qax_diag_buffers_static(unit, a);
        }
        else 
#endif /* BCM_QAX_SUPPORT */
        { 
            return cmd_dpp_diag_buffers_static(unit, a);
        }
#endif /* BCM_JERICHO_SUPPORT */
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (!sal_strncasecmp(function, "dynamic", strnlen_function)) {
#ifdef BCM_QAX_SUPPORT
        if (SOC_IS_QAX(unit)) {
            return cmd_dpp_qax_diag_buffers_dynamic(unit, a);
        }
        else 
#endif /* BCM_QAX_SUPPORT */
        {
            return cmd_dpp_diag_buffers_dynamic(unit, a);
        }
    } else {
        return CMD_USAGE;
    }
}
#endif /* BCM_PETRA_SUPPORT */
