/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <shared/util.h>

#include <soc/dcmn/error.h>
#include <soc/types.h>
#include <soc/mcm/memregs.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/ARAD/arad_nif_recover.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#define ARAD_NIF_RECOVER_REASSEMBLY_CTXT        (0x3f)
#define ARAD_NIF_RECOVER_NOF_INTERFACE          (32)
#define ARAD_NIF_RECOVER_INVALID_PORT           (-1)
#define ARAD_NIF_RECOVER_MAX_PORTS_PER_QUAD     (4)

typedef struct bist_flow_descriptor_s {
        uint32 num_of_bursts;
        uint32 data_type;
        uint32 Flow0CreateErr;
        uint32 Flow0Class;
        uint32 Flow0ConstantCh;
        uint32 Flow0MinBurstSize;
        uint32 Flow0BurstSizeMask;
        uint32 Flow0IncrementalBurstSize;             
} bist_flow_descriptor_t;

typedef struct arad_nif_recover_data_restore_s {
    uint32 interface_data[ARAD_NIF_RECOVER_NOF_INTERFACE];
    uint32 is_reassembly_ctxt_en;
    uint32 reassembly_ctxt;
} arad_nif_recover_data_restore_t;

STATIC int (*dynamic_port_recovery_sequence[SOC_MAX_NUM_DEVICES])(int unit, soc_pbmp_t *pbmp, int* rv) = {NULL};

STATIC int arad_nif_recover_bist_main(int unit, uint32 bist_mode, uint32 bist_ch_mode, uint32 bist_context_mask, int *rv);



STATIC int arad_nif_recover_control_tx(int unit, soc_pbmp_t* pbmp, uint32 is_stop)
{
    uint64 reg;
    soc_port_t port;

    SOCDNX_INIT_FUNC_DEFS;

    PBMP_ITER(*pbmp, port) {
        if (SOC_BLK_XLP == SOC_PORT_TYPE(unit, port)) {
            SOCDNX_IF_ERR_EXIT(READ_XLP_REG_062500r(unit, port, &reg));
            soc_reg64_field32_set(unit, XLP_REG_062500r, &reg, FIELD_0_0f, is_stop);
            SOCDNX_IF_ERR_EXIT(WRITE_XLP_REG_062500r(unit, port, reg));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_port_loopbacks_set(int unit, soc_pbmp_t* pbmp, int lb)
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_pm_mac_loopback_set(unit, port, lb)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_reset_xlp_block(int unit)
{
    soc_reg_above_64_val_t reg;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg, XLP_0_INITf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg));

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg));
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg, XLP_0_INITf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg));
		
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int arad_nif_recover_port_mask_create(int unit, soc_pbmp_t* pbmp, uint32* mask)
{
    soc_port_t port_i;
    uint32 phy_port;
    SOCDNX_INIT_FUNC_DEFS;

    *mask = 0;

    SOC_PBMP_ITER(*pbmp, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port ));
        --phy_port;
        SHR_BITSET(mask, phy_port);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_send_packets_bist(int unit, soc_pbmp_t* pbmp, int *rv)
{
    uint32 flags, mask = 0;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (SOC_PORT_IS_STAT_INTERFACE(flags)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Cannot send BIST on statistic interface port\n")));
        }
    }

    SOCDNX_IF_ERR_EXIT(arad_nif_recover_port_mask_create(unit, pbmp, &mask));

    SOCDNX_IF_ERR_EXIT(arad_nif_recover_bist_main(unit, 0, 0, mask, rv));

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int arad_nif_recover_check_counters(int unit, soc_pbmp_t* pbmp, int *counters_match, soc_port_t* fail_port){
    uint32 val1, val2, val3, val4;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;
   
    *counters_match = 1;
    *fail_port = ARAD_NIF_RECOVER_INVALID_PORT;

    PBMP_ITER(*pbmp, port) {
        if (SOC_BLK_XLP == SOC_PORT_TYPE(unit, port)) {
            SOCDNX_IF_ERR_EXIT(READ_PORT_TXFIFO_CELL_REQ_CNTr(unit, port, &val1)); 
            SOCDNX_IF_ERR_EXIT(READ_PORT_TXFIFO_CELL_CNTr(unit, port, &val2));
            SOCDNX_IF_ERR_EXIT(READ_PORT_CDC_TXFIFO_CELL_REQ_CNTr(unit, port, &val3));
            SOCDNX_IF_ERR_EXIT(READ_PORT_CDC_TXFIFO_CELL_CNTr(unit, port, &val4));
        
            if( (val1 != val3) || (val2 != val4) )
            {
                LOG_VERBOSE( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "PORT_TXFIFO_CELL_REQ_CNT %d\n"), val1));
                LOG_VERBOSE( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "PORT_TXFIFO_CELL_CNT %d\n"), val2));
                LOG_VERBOSE( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "PORT_CDC_TXFIFO_CELL_REQ_CNT %d\n"), val3));
                LOG_VERBOSE( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "PORT_CDC_TXFIFO_CELL_CNT %d\n"), val4));
                *counters_match = 0;
                *fail_port = port;
                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int arad_nif_recover_bist_run_test(int unit, uint32 bist_mode, uint32 bist_ch_mode, uint32 bist_context_mask, uint64* bist_tx_burst_th,
              bist_flow_descriptor_t bist_flow_descriptor_0,
              bist_flow_descriptor_t bist_flow_descriptor_1,
              bist_flow_descriptor_t bist_flow_descriptor_2,
              bist_flow_descriptor_t bist_flow_descriptor_3,
              int* status)
{
    int counter;
    uint32 bist_tx_done = 0;
    uint32 bist_rx_done = 0;
    uint32 bist_status;
    uint64 bist_rx_ok_bursts_cnt;
    uint64 bist_tx_bursts_cnt;
    uint32 bist_rx_counters_1, bist_rx_counters_2, bist_rx_counters_3;
    uint32 bist_tx_crc_err_cnt;
    uint32 reg;
    uint64 reg64;
    char uint64_hexa_string_1[32];
    char uint64_hexa_string_2[32];

    SOCDNX_INIT_FUNC_DEFS;

    if (status){
        *status = 1;
        _shr_format_uint64_hexa_string(*bist_tx_burst_th, uint64_hexa_string_1); 
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test start configuration bist_mode = %d, bist_ch_mode = %d, bist_context_mask = %x, bist_tx_burst_th = %s\n"),
                  bist_mode, bist_ch_mode, bist_context_mask, uint64_hexa_string_1));
    }

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_CONTEXTS_MASKr(unit, &reg););
    soc_reg_field_set(unit, NBI_BIST_CONTEXTS_MASKr, &reg, BIST_CONTEXTS_MASKf, bist_context_mask);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_CONTEXTS_MASKr(unit, reg));
    
    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_TX_BURSTS_THRESHOLDr(unit, &reg64));
    soc_reg64_field_set(unit, NBI_BIST_TX_BURSTS_THRESHOLDr, &reg64, BIST_TX_BURST_THf, *bist_tx_burst_th);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_TX_BURSTS_THRESHOLDr(unit, reg64));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 0, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_0.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_0.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_0.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_0.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_0.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 0, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 0, &reg)); 
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_0.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_0.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_0.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 0, reg));


    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 1, &reg)); 
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_1.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_1.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_1.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_1.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_1.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 1, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 1, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_1.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_1.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_1.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 1, reg));


    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 2, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_2.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_2.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_2.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_2.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_2.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 2, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 2, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_2.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_2.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_2.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 2, reg));

 
    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 3, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_NUM_OF_BURSTSf, bist_flow_descriptor_3.num_of_bursts);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_DATA_TYPEf, bist_flow_descriptor_3.data_type);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CREATE_ERRf, bist_flow_descriptor_3.Flow0CreateErr);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CLASSf, bist_flow_descriptor_3.Flow0Class);
    soc_reg_field_set(unit, NBI_BIST_FLOW_FIRST_DESCRIPTORr, &reg, FLOW_N_CONSTANT_CHf, bist_flow_descriptor_3.Flow0ConstantCh);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW_FIRST_DESCRIPTORr(unit, 3, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 3, &reg));    
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_MIN_BURST_SIZEf, bist_flow_descriptor_3.Flow0MinBurstSize);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_BURST_SIZE_MASKf, bist_flow_descriptor_3.Flow0BurstSizeMask);
    soc_reg_field_set(unit, NBI_BIST_FLOW__SECOND_DESCRIPTORr, &reg, FLOW_N_INCREMENTAL_BURST_SIZEf, bist_flow_descriptor_3.Flow0IncrementalBurstSize);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_FLOW__SECOND_DESCRIPTORr(unit, 3, reg));

    SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_GENERAL_CONFIGURATIONr(unit, &reg));
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_GENERATOR_RSTNf, 1);
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_MODEf, bist_mode); 
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, BIST_CH_CHOSING_METHODf, bist_ch_mode);
    soc_reg_field_set(unit, NBI_BIST_GENERAL_CONFIGURATIONr, &reg, TAKE_BIST_NOT_EGQf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_GENERAL_CONFIGURATIONr(unit, reg));

     
    if (status != NULL) {

        LOG_INFO( BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test start generator & checker\n")));
        LOG_INFO( BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test wait for tx_done\n")));
        counter = 0;
        while ((bist_tx_done != 1) && (counter < 400)) 
        {      
            READ_NBI_BIST_STATUSr(unit, &bist_status);
            bist_tx_done = bist_status & 0x00000001;
            ++counter;
        }

        if (counter < 200) {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist_test TX_DONE\n")));
        }
        else {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR TX FAIL\n")));
        }

        LOG_INFO( BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "bist_test wait for rx_done\n")));
        counter = 0;
        while ((bist_rx_done != 1) && (counter < 400))
        {
            READ_NBI_BIST_STATUSr(unit, &bist_status);
            bist_rx_done = (bist_status >> 4) & 0x00000001;
            counter++;
        }
        if (counter < 200) {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist_test RX_DONE\n")));
        }
        else {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR RX FAIL\n")));
        }
   
        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_OK_BURSTS_COUNTERr(unit, &bist_rx_ok_bursts_cnt)); 
        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_TX_BURSTS_COUNTERr(unit, &bist_tx_bursts_cnt));

        _shr_format_uint64_hexa_string(bist_rx_ok_bursts_cnt, uint64_hexa_string_1);
        _shr_format_uint64_hexa_string(bist_tx_bursts_cnt, uint64_hexa_string_2);

        if (COMPILER_64_NE(bist_rx_ok_bursts_cnt , bist_tx_bursts_cnt)) 
        {

            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR number_of_ok_packets received %s does not match number_of_packets_transmitted %s\n"), 
                                 uint64_hexa_string_1, uint64_hexa_string_2));
            *status = 0;
        }
        else
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist number_of_ok_packets received %s match number_of_packets_transmitted %s\n"),
                                 uint64_hexa_string_1, uint64_hexa_string_2));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_COUNTERS_1r(unit, &bist_rx_counters_1));
        if (bist_rx_counters_1 > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_rx_counters_1 %x reports on errors\n"),bist_rx_counters_1));
            *status = 0;     
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_COUNTERS_2r(unit, &bist_rx_counters_2));
        if (bist_rx_counters_2 > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_rx_counters_2 %x reports on errors\n"),bist_rx_counters_2));
            *status = 0; 
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_RX_COUNTERS_3r(unit, &bist_rx_counters_3));
        if (bist_rx_counters_3 > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_rx_counters_3 %x reports on errors\n"),bist_rx_counters_3));
            *status = 0;   
        }

        SOCDNX_IF_ERR_EXIT(READ_NBI_BIST_TX_ADDITIONAL_COUNTERr(unit, &bist_tx_crc_err_cnt));
        bist_tx_crc_err_cnt &= 0x0000FFFF; 
        if (bist_tx_crc_err_cnt > 0) 
        {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist ERROR bist_tx_crc_err_cnt %x reports on errors\n"), bist_tx_crc_err_cnt));
            *status = 0;    
        }

        if (*status == 1) {
            LOG_INFO( BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "bist_test: PASS\n")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_bist_main(int unit, uint32 bist_mode, uint32 bist_ch_mode, uint32 bist_context_mask, int *rv){

    uint64 bist_tx_burst_th;
    bist_flow_descriptor_t bist_flow_descriptor_tmp;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_SET(bist_tx_burst_th, 0, 100);
    
    
    bist_flow_descriptor_tmp.num_of_bursts = 1;
    bist_flow_descriptor_tmp.data_type = 0;             
    bist_flow_descriptor_tmp.Flow0CreateErr = 0;        
    bist_flow_descriptor_tmp.Flow0Class = 0xF;          
    bist_flow_descriptor_tmp.Flow0ConstantCh = 0;        
    bist_flow_descriptor_tmp.Flow0MinBurstSize = 0x40;  
    bist_flow_descriptor_tmp.Flow0BurstSizeMask = 0;    
    bist_flow_descriptor_tmp.Flow0IncrementalBurstSize = 0; 
    
    SOCDNX_IF_ERR_EXIT(arad_nif_recover_bist_run_test(unit, bist_mode, bist_ch_mode, bist_context_mask, &bist_tx_burst_th, bist_flow_descriptor_tmp, bist_flow_descriptor_tmp,
            bist_flow_descriptor_tmp, bist_flow_descriptor_tmp, rv));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_clean_up(int unit, soc_pbmp_t* pbmp, int is_init_sequence){

    soc_port_t port;
    uint32 reg;
    uint32 phy_port;
    int bit;
    uint64 reg64;
    soc_reg_above_64_val_t reg_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    sal_usleep(10);

    SOC_PBMP_ITER(*pbmp, port) {

       SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
       bit = SOC_PORT_INFO(unit, phy_port).bindex;

       SOCDNX_IF_ERR_EXIT(READ_PORT_MIB_RESETr(unit, port, &reg));
       soc_reg_field_set(unit, PORT_MIB_RESETr, &reg, CLR_CNTf, (1 << bit));
       SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, reg));

       SOCDNX_IF_ERR_EXIT(READ_PORT_MIB_RESETr(unit, port, &reg));
       soc_reg_field_set(unit, PORT_MIB_RESETr, &reg, CLR_CNTf, 0);
       SOCDNX_IF_ERR_EXIT(WRITE_PORT_MIB_RESETr(unit, port, reg));
    }

    
    if (is_init_sequence) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64)); 
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64, NBI_INITf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));

        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64, NBI_INITf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));

        
        SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PACKET_COUNTERr(unit, 0, &reg64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

		
STATIC int arad_nif_recover_reset_fail_ports(int unit, soc_pbmp_t* pbmp, int is_init_sequence, soc_port_t fail_port){

    uint32  rv;
    int counter_interval;
    uint32 counter_flags;
    soc_pbmp_t counter_pbmp, ports_pbmp;
    soc_port_t port;
    soc_pbmp_t probe_bm, probe_ok_bm;
    uint32 reg, phy_port;
    soc_port_t port_i;
    int speed, quad, tmp_quad, i;
    int ports_enable[ARAD_NIF_RECOVER_MAX_PORTS_PER_QUAD] = {0};
    SOCDNX_INIT_FUNC_DEFS;
    
    SOC_PBMP_ITER(*pbmp, port) {
        if (SOC_BLK_XLP == SOC_PORT_TYPE(unit, port)) {
            SOCDNX_IF_ERR_EXIT(READ_XLP_XMAC_CTRLr(unit, port, &reg)); 
            soc_reg_field_set(unit, XLP_XMAC_CTRLr, &reg, SOFT_RESETf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_XLP_XMAC_CTRLr(unit, port, reg));
            
            SOCDNX_IF_ERR_EXIT(READ_XLP_XMAC_CTRLr(unit, port, &reg));
            soc_reg_field_set(unit, XLP_XMAC_CTRLr, &reg, SOFT_RESETf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_XLP_XMAC_CTRLr(unit, port, reg));
        }
    }

    if (fail_port != ARAD_NIF_RECOVER_INVALID_PORT) {
        soc_pbmp_t failed_ports;

        SOC_PBMP_CLEAR(failed_ports);
        SOC_PBMP_PORT_ADD(failed_ports, fail_port);

                
        rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_speed_get,(unit, fail_port, &speed)));
        SOCDNX_IF_ERR_EXIT(rv);
      
        
        if (speed != 48000) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, fail_port, &phy_port)); 
            quad = (phy_port - 1) / 4;
            SOC_PBMP_ITER(*pbmp, port) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
                tmp_quad = (phy_port - 1) /4;
                if (tmp_quad == quad) {
                    SOC_PBMP_PORT_ADD(failed_ports, port);
                }
            }
        }

        
        i = 0;
        SOC_PBMP_ITER(failed_ports, port){
            rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_enable_get,(unit, port, 0, &ports_enable[i])));
            SOCDNX_IF_ERR_EXIT(rv);
            ++i;
        }
        
        
        SOC_PBMP_ITER(failed_ports, port){

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_pbmp));
            SOC_PBMP_ITER(ports_pbmp, port_i) {
                 rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_enable_set,(unit, port_i, 0, 0)));
                 SOCDNX_IF_ERR_EXIT(rv);
            }

            if (SOC_PBMP_MEMBER(SOC_INFO(unit).cmic_bitmap, port)) {
                
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));
            } else {
                
                SOCDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
                SOCDNX_IF_ERR_EXIT(soc_counter_stop(unit));
                
                
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_initialized_set(unit, port, 0));

                
                rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_detach, (unit, port));
                SOCDNX_IF_ERR_EXIT(rv);

                
                SOC_PBMP_PORT_REMOVE(counter_pbmp, port);
                SOCDNX_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
            }

        }
        
        SOC_PBMP_ITER(failed_ports, port){

            SOC_PBMP_CLEAR(probe_bm);
            SOC_PBMP_CLEAR(probe_ok_bm);   

            SOC_PBMP_PORT_ADD(probe_bm, port);
            rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_probe, (unit, probe_bm, &probe_ok_bm, is_init_sequence));
            SOCDNX_IF_ERR_EXIT(rv);
        }

        
        SOC_PBMP_ITER(failed_ports, port){
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_pbmp));
            
            SOC_PBMP_ITER(ports_pbmp, port_i) {
                rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_speed_set,(unit, port_i, speed));
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }

        
        i = 0;
        SOC_PBMP_ITER(failed_ports, port){
            if (ports_enable[i]) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_pbmp)); 

                SOC_PBMP_ITER(ports_pbmp, port_i) {
                     rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_enable_set,(unit, port_i, 0, 1)));
                     SOCDNX_IF_ERR_EXIT(rv);
                }
            }
            ++i;
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int arad_nif_recover_ire_config_set(int unit, soc_pbmp_t* pbmp, int enable, arad_nif_recover_data_restore_t* data)
{
    int i;
    soc_port_t port;
    uint32 mask[1], reg_val, entry, base_addr, ras_ctx, flags;
    SOCDNX_INIT_FUNC_DEFS;

    ras_ctx = ARAD_NIF_RECOVER_REASSEMBLY_CTXT;

    SOC_PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (SOC_PORT_IS_STAT_INTERFACE(flags)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Cannot configure IRE on statistic interface port\n")));
        }
    }

    SOCDNX_IF_ERR_EXIT(arad_nif_recover_port_mask_create(unit, pbmp, mask));

    for(i = 0; i < ARAD_NIF_RECOVER_NOF_INTERFACE; ++i) {
        if (SHR_BITGET(mask, i)) {
            SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, i, &entry)); 
            soc_mem_field_get(unit, IRE_NIF_PORT_MAPm, &entry, CTXT_MAP_BASE_ADDRESSf, &base_addr);

            SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_CTXT_MAPm(unit, MEM_BLOCK_ANY, base_addr, &entry));
            if (enable) {
                
                soc_mem_field_get(unit, IRE_NIF_CTXT_MAPm, &entry, REASSEMBLY_CONTEXTf, &data->interface_data[i]); 
            }
            soc_mem_field_set(unit, IRE_NIF_CTXT_MAPm, &entry, REASSEMBLY_CONTEXTf, enable ? &ras_ctx : &data->interface_data[i]); 
            SOCDNX_IF_ERR_EXIT(WRITE_IRE_NIF_CTXT_MAPm(unit, MEM_BLOCK_ANY, base_addr, &entry));
        }
    }

    SOCDNX_IF_ERR_EXIT(READ_IRE_STATIC_CONFIGURATIONr(unit, &reg_val));
    if (enable) {
        data->is_reassembly_ctxt_en = soc_reg_field_get(unit, IRE_STATIC_CONFIGURATIONr, reg_val, BAD_REASSEMBLY_CONTEXT_VALIDf);
        data->reassembly_ctxt = soc_reg_field_get(unit, IRE_STATIC_CONFIGURATIONr, reg_val, BAD_REASSEMBLY_CONTEXTf);

        soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &reg_val, BAD_REASSEMBLY_CONTEXT_VALIDf, enable); 
        soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &reg_val, BAD_REASSEMBLY_CONTEXTf, ras_ctx);
        soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &reg_val, ENABLE_DATA_PATHf, enable);
    } else {
        soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &reg_val, BAD_REASSEMBLY_CONTEXT_VALIDf, data->is_reassembly_ctxt_en);
        soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &reg_val, BAD_REASSEMBLY_CONTEXTf, data->reassembly_ctxt);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_STATIC_CONFIGURATIONr(unit, reg_val));



exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int arad_nif_recover_check_rx_and_ire(int unit, soc_pbmp_t* pbmp, int *pass, soc_port_t* fail_port)
{
    uint32 reg32_val, flags;
    uint64 reg64;
    int reasmbl_ctx, pbmp_count, stat_count = 0;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    *pass = 1;
    *fail_port = ARAD_NIF_RECOVER_INVALID_PORT;
    COMPILER_64_ZERO(reg64);
    SOC_PBMP_COUNT(*pbmp, pbmp_count);

    SOC_PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (SOC_PORT_IS_STAT_INTERFACE(flags)){
            ++stat_count;
            continue;
        }
        SOCDNX_IF_ERR_EXIT(READ_RPKTr(unit, port, &reg64));
        if (COMPILER_64_IS_ZERO(reg64)) {
            LOG_VERBOSE( BSL_LS_SOC_PORT, (BSL_META_U(unit, "port %d init failed\n"), port));
            *pass = 0;
            *fail_port = port;
        }
    }

    
    if (stat_count != pbmp_count) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_INTERRUPT_REGISTERr(unit, &reg32_val)); 
        reasmbl_ctx = soc_reg_field_get(unit, IRE_INTERRUPT_REGISTERr, reg32_val, ERROR_BAD_REASSEMBLY_CONTEXTf);
        if (reasmbl_ctx == 0) {
            *pass = 0;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int arad_nif_recover_run_sequence(int unit, soc_pbmp_t *pbmp, uint32 flags, int iterations, int is_init_sequence)
{
    int i = 0, tx_counters_match = 1, rx_traffic_rcv = 1;
    char* fail_type;
    soc_port_t fail_port_1 = -1, fail_port_2 = -1;
    arad_nif_recover_data_restore_t interface_data;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i <= iterations; ++i) {

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_port_loopbacks_set(unit, pbmp, 1)); 

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_control_tx(unit, pbmp, 1)); 

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_ire_config_set(unit, pbmp, 1, &interface_data));

        if (is_init_sequence) {
            SOCDNX_IF_ERR_EXIT(arad_nif_recover_send_packets_bist(unit, pbmp, NULL)); 
        } else {
            SOCDNX_IF_ERR_EXIT(dynamic_port_recovery_sequence[unit](unit, pbmp, NULL));
        }
        sal_usleep(100);

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_check_counters(unit, pbmp, &tx_counters_match, &fail_port_1));

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_control_tx(unit, pbmp, 0));

	    if ((flags & ARAD_NIF_RECOVER_F_DONT_RUN_RX_TRAFFIC) == 0) {
	        SOCDNX_IF_ERR_EXIT(arad_nif_recover_check_rx_and_ire(unit, pbmp, &rx_traffic_rcv, &fail_port_2));
	    }

        SOCDNX_IF_ERR_EXIT(WRITE_NBI_BIST_GENERAL_CONFIGURATIONr(unit, 0));

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_reset_fail_ports(unit, pbmp, is_init_sequence, (fail_port_1 == -1) ? fail_port_2 : fail_port_1));

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_ire_config_set(unit, pbmp, 0, &interface_data));

        SOCDNX_IF_ERR_EXIT(arad_nif_recover_port_loopbacks_set(unit, pbmp, 0));

        if (tx_counters_match && rx_traffic_rcv) {
            if (i > 0) {
                LOG_INFO( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "Recovery succeeded within %d iterations.\n"), i)); 
            }
            break;
        } else {
            fail_type = tx_counters_match ? "Err #02" : "Err #01";
            if (i < iterations) {
                LOG_INFO( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "Recovery iteration %d/%d failed(%s), trying again.\n"), i + 1, iterations, fail_type)); 
                SOCDNX_IF_ERR_EXIT(arad_nif_recover_reset_xlp_block(unit));
                SOCDNX_IF_ERR_EXIT(arad_nif_recover_clean_up(unit, pbmp, is_init_sequence));
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,(_BSL_SOC_MSG("Recovery failed(%s).\n"), fail_type ));
            }
        } 
    }
    SOCDNX_IF_ERR_EXIT(arad_nif_recover_clean_up(unit, pbmp, is_init_sequence));

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_nif_recover_run_recovery_test(int unit, soc_pbmp_t* pbmp, uint32 flags, int iterations, int is_init_sequence)
{
    soc_port_t port;
    soc_pbmp_t xlp_pbmp, clp_pbmp;
    int temp_flags, nof_ports;
    SOCDNX_INIT_FUNC_DEFS;

    if (!is_init_sequence && dynamic_port_recovery_sequence[unit] == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("Cannot perform recovery on dynamic port without callback function.\n")));
    }

    SOC_PBMP_CLEAR(xlp_pbmp);
    SOC_PBMP_ASSIGN(clp_pbmp, *pbmp);
    
    SOC_PBMP_ITER(*pbmp, port){
        if (SOC_BLK_XLP == SOC_PORT_TYPE(unit, port)) {
            SOC_PBMP_PORT_ADD(xlp_pbmp, port);
            SOC_PBMP_PORT_REMOVE(clp_pbmp, port);
        }
    }

    SOC_PBMP_COUNT(xlp_pbmp, nof_ports);
    if (nof_ports) {
        temp_flags = ARAD_NIF_RECOVER_FLAGS_XLP_GET(flags) ? flags : 0; 
        SOCDNX_IF_ERR_EXIT(arad_nif_recover_run_sequence(unit, &xlp_pbmp, temp_flags, iterations, is_init_sequence));
    }

    SOC_PBMP_COUNT(clp_pbmp, nof_ports);
    if (nof_ports) {
        temp_flags = ARAD_NIF_RECOVER_FLAGS_CLP_GET(flags) ? flags : 0; 
        SOCDNX_IF_ERR_EXIT(arad_nif_recover_run_sequence(unit, &clp_pbmp, temp_flags, iterations, is_init_sequence));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
