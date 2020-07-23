#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FLOWCONTROL


#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>


#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_flow_control.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/port_sw_db.h>

#include <soc/mcm/allenum.h>






#define ARAD_ILKN_NDX_MIN                                        (ARAD_NIF_ILKN_ID_A)
#define ARAD_ILKN_NDX_MAX                                        (ARAD_NIF_ILKN_ID_B)
#define ARAD_FC_CLASS_MAX                                        7
#define ARAD_CAL_MODE_NDX_MAX                                    (SOC_TMC_FC_NOF_CAL_MODES-1)

#define ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS 3

#define ARAD_FC_REC_CAL_DEST_ARR_SIZE   7
#define ARAD_FC_GEN_CAL_SRC_ARR_SIZE    8

#define ARAD_FC_PFC_GENERIC_BITMAP_SIZE SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE
#define ARAD_FC_HCFC_BITMAPS 8


#define ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL 0x0 
#define ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL 0x2 


#define ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET 9



#define ARAD_FC_RX_MLF_MAX           ((1 << soc_reg_field_length(unit, NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr, RX_N_LLFC_THRESHOLD_SETf)) - 1)

#define ARAD_FC_RX_MLF_HRF_MAX       ((1 << soc_reg_field_length(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, HRF_RX_N_LLFC_THRESHOLD_SETf)) - 1)

#define ARAD_FC_RX_MLF_HRF_NOF_BITS  (soc_reg_field_length(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, HRF_RX_N_LLFC_THRESHOLD_SETf))

#if defined(BCM_88650_B0)
#define ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MIN 1
#define ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MAX 2
#endif


#define ARDON_FC_FCV_BIT_LEN 256


#define FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX               SOC_TMC_EGR_NOF_Q_PRIO_ARAD
#define FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD              4




#define ARAD_FC_CAL_MODE_IS_ILKN(c_mode) \
  SOC_SAND_NUM2BOOL((c_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) || (c_mode ==SOC_TMC_FC_CAL_MODE_ILKN_OOB))


#define SET_FC_ENABLE(__FC_ENA, __FIELD) {                                            \
    if (SHR_BITGET(cfc_enables->bmp, __FC_ENA)) {                                     \
        if (soc_reg_field_valid(unit, reg, __FIELD)) {                                \
            field_value = (SHR_BITGET(ena_info->bmp, __FC_ENA))? 1: 0;                \
            soc_reg_above_64_field32_set(unit, reg, reg_data, __FIELD, field_value);  \
        }                                                                             \
    }                                                                                 \
}

#define GET_FC_ENABLE(__FC_ENA, __FIELD) {                                            \
    if (SHR_BITGET(cfc_enables->bmp, __FC_ENA)) {                                     \
        if (soc_reg_field_valid(unit, reg, __FIELD)) {                                \
            field_value = soc_reg_above_64_field32_get(unit, reg, reg_data, __FIELD); \
            if (field_value) {                                                        \
                SHR_BITSET(ena_info->bmp, __FC_ENA);                                  \
            }                                                                         \
        }                                                                             \
        else {                                                                        \
            SHR_BITCLR(ena_info->bmp, __FC_ENA);                                      \
        }                                                                             \
    }                                                                                 \
}

#define FC_ENABLE_NOT_SUPPORT_ARAD {    \
    SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN,  \
    SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN,   \
    SOC_TMC_FC_NIF_TO_GEN_PFC_EN,       \
    SOC_TMC_FC_STAT_VSQ_TO_HCFC_EN      \
}









static SOC_TMC_FC_GEN_CAL_SRC 
  arad_fc_gen_cal_src_arr[ARAD_FC_GEN_CAL_SRC_ARR_SIZE] = 
    { SOC_TMC_FC_GEN_CAL_SRC_STE, 
      SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ,
      SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ,
      SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS,
      SOC_TMC_FC_GEN_CAL_SRC_HCFC,
      SOC_TMC_FC_GEN_CAL_SRC_LLFC,
      SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT,
      SOC_TMC_FC_GEN_CAL_SRC_CONST
    };

static SOC_TMC_FC_REC_CAL_DEST 
  arad_fc_rec_cal_dest_arr[ARAD_FC_REC_CAL_DEST_ARR_SIZE] = 
    { SOC_TMC_FC_REC_CAL_DEST_PFC, 
      SOC_TMC_FC_REC_CAL_DEST_NIF_LL,
      SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY,
      SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY,
      SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC,
      SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT,
      SOC_TMC_FC_REC_CAL_DEST_NONE
    };






static soc_error_t
arad_fc_cal_tbl_get(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN SOC_TMC_FC_CAL_MODE           cal_mode,
    SOC_SAND_IN ARAD_FC_CAL_TYPE              cal_type,
    SOC_SAND_IN SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_OUT soc_mem_t                    *cal_table
  )
{
    soc_mem_t 
	    cal_table_tmp = INVALIDm;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_table);
    if (cal_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx > SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn ndx is out of range")));
        }
    }
    else {
        if (if_ndx > SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("oob ndx is out of range")));
        }        
    }

    if (cal_type == ARAD_FC_CAL_TYPE_RX){
        if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode)) {
            cal_table_tmp = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_RX_0_CALm : CFC_ILKN_RX_1_CALm);
        }
        else {
            cal_table_tmp = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_RX_0_CALm : CFC_SPI_OOB_RX_1_CALm);
        }
    }
    else {
        if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode)) {
            cal_table_tmp = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_TX_0_CALm : CFC_ILKN_TX_1_CALm);
        }
        else {
            cal_table_tmp = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_TX_0_CALm : CFC_SPI_OOB_TX_1_CALm);
        }
    }
	
	(*cal_table) = cal_table_tmp;

exit:
    SOCDNX_FUNC_RETURN;    
}



STATIC soc_error_t
  arad_flow_control_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    SOC_TMC_FC_ILKN_MUB_GEN_CAL
        ilkn_cal;
    uint32
        i,
        ilkn_ndx,
        oob_ndx;
    SOC_TMC_FC_ENABLE_BITMAP
        cfc_enables; 
    soc_field_t
        fld = INVALIDf;
    soc_field_t ilkn_inb_tx_cal_len_flds[] = {
        ILKN_TX_0_CAL_LENf, ILKN_TX_1_CAL_LENf};
    uint32
        cal_length = 0;
    soc_reg_t
        reg = INVALIDr;
    soc_reg_t nbi_tx_ilkn_control[] = {
        NBI_TX_0_ILKN_CONTROLr, NBI_TX_1_ILKN_CONTROLr};
    soc_field_t nbi_inb_tx_fc_cal_len_flds[] = {
        TX_0_FC_CAL_LENf, TX_1_FC_CAL_LENf};

    SOCDNX_INIT_FUNC_DEFS;

    
    SHR_BITCLR_RANGE(cfc_enables.bmp, 0, SOC_TMC_FC_NOF_ENABLEs);

    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_ERP_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_IF_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_EGQ_TO_SCH_PFC_EN);

    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG);

    rv = arad_fc_enables_set(unit, &cfc_enables, &cfc_enables);
    SOCDNX_IF_ERR_EXIT(rv);

    
    for (ilkn_ndx = 0; ilkn_ndx < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; ilkn_ndx++) {
        rv = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_RX, ilkn_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_TX, ilkn_ndx);
        SOCDNX_IF_ERR_EXIT(rv);

        if ((SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_type[ilkn_ndx] == SOC_TMC_FC_CAL_INB_TYPE_ILKN) && 
            (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[ilkn_ndx] & SOC_DPP_FC_CAL_MODE_TX_ENABLE)) {
            fld = ilkn_inb_tx_cal_len_flds[ilkn_ndx]; 
            cal_length = SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[ilkn_ndx][SOC_TMC_CONNECTION_DIRECTION_TX] * 
	    	             SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[ilkn_ndx][SOC_TMC_CONNECTION_DIRECTION_TX];
            if ((cal_length >= SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)) || (cal_length == 0)){
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("in-band calendar length is out of range [1, %d]"), (SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)-1)));
            }
    
            rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld, cal_length-1);
            SOCDNX_IF_ERR_EXIT(rv);

            
            reg = nbi_tx_ilkn_control[ilkn_ndx];
            fld = nbi_inb_tx_fc_cal_len_flds[ilkn_ndx];
            
            rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld, (cal_length - 1) / 16);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }


    
    rv = soc_reg_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, ILKN_RX_1_SRC_SELf, 1);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_reg_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, ILKN_1_OOB_RX_LN_SRCf, 1);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_reg_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, ILKN_1_OOB_RX_IF_SRCf, 1);
    SOCDNX_IF_ERR_EXIT(rv);

    
    for (oob_ndx = 0; oob_ndx < SOC_DPP_CONFIG(unit)->tm.max_oob_ports; oob_ndx++) {
        rv = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_RX, oob_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_TX, oob_ndx);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    for (ilkn_ndx = 0; ilkn_ndx < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; ilkn_ndx++) {
        for(i = 0; i < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; i++) {
            ilkn_cal.entries[i].source = SOC_TMC_FC_GEN_CAL_SRC_CONST;
            ilkn_cal.entries[i].id = 0;
        }
        rv = arad_fc_ilkn_mub_gen_cal_set(unit, ilkn_ndx, &ilkn_cal);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    rv = soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, GFMC_FC_MAPf,    0x8);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_2_FC_MAPf,  0xc);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_1_FC_MAPf,  0xe);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_0_FC_MAPf,  0xf);
    SOCDNX_IF_ERR_EXIT(rv);

#if defined(BCM_88650_B0)
    
    
    
    if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
    {
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_0_RT_CAL_INDEX_0f,  0x0);
		SOCDNX_IF_ERR_EXIT(rv);
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_0_RT_CAL_INDEX_1f,  0x1);
		SOCDNX_IF_ERR_EXIT(rv);
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_1_RT_CAL_INDEX_0f,  0x1);
		SOCDNX_IF_ERR_EXIT(rv);
        rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_1_RT_CAL_INDEX_1f,  0x0);
		SOCDNX_IF_ERR_EXIT(rv);
    }


#endif

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
    arad_fc_shr_mapping(int unit, int fcv_bit, int cl_index, int select, int valid)
{
    soc_error_t 
        rv = SOC_E_NONE;
    uint32
        entry_offset,
        field_offset;
    uint32
        shr_data[2],
        index_field[4] = {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f},
        select_field[4] = {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f},
        valid_field[4] = {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
  
    SOCDNX_INIT_FUNC_DEFS;

    if (fcv_bit >= ARDON_FC_FCV_BIT_LEN) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fcv_bit=%d is out of range [0..%d]"), fcv_bit, ARDON_FC_FCV_BIT_LEN - 1)); 
    }

    entry_offset = fcv_bit / 4;
    field_offset = fcv_bit % 4;

    rv = soc_mem_read(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, index_field[field_offset], cl_index);
    soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, select_field[field_offset], select);
    soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, valid_field[field_offset], valid);

    rv = soc_mem_write(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
    arad_fc_init_shr_mapping(int unit)
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        i,
        entry_offset;
    uint32
        shr_data[2],
        index_field[4] = {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f},
        select_field[4] = {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f},
        valid_field[4] = {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
  
    SOCDNX_INIT_FUNC_DEFS;

    
    for(entry_offset = 0; entry_offset < ARDON_FC_FCV_BIT_LEN / 4; entry_offset++)   {

        rv = soc_mem_read(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        for(i = 0; i < 4; i++)  {
              soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, index_field[i], (entry_offset * 4) + i);
              soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, select_field[i], 0);
              soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, valid_field[i], 1);
        }

        rv = soc_mem_write(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_init_pfc_mapping(
      SOC_SAND_IN int  unit
    )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        nif_pfc_data[2],
        nif_pfc_data_2[2],
        *data_select,
        entry_offset,
        field_offset,
        base_queue_pair,
        nof_priorities,
        i,
        interface_i,
        valid;
    soc_port_if_t
        interface_type;
    uint32
        index_field[] = 
            {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f};
    uint32
        select_field[] = 
            {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f};
    uint32
        valid_field[] = 
            {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
    uint32
        phy_port;
    soc_pbmp_t
        pbmp;
    soc_port_t
        logical_port;
    uint32
    tm_port, 
    flags;
    int 
        core;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_port_sw_db_valid_ports_get(unit, 0, &pbmp);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_PBMP_ITER(pbmp, logical_port)
    {
        rv = soc_port_sw_db_flags_get(unit, logical_port, &flags);
        SOCDNX_IF_ERR_EXIT(rv);
        if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {
            rv = soc_port_sw_db_local_to_tm_port_get(unit, logical_port, &tm_port, &core);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_queue_pair);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
            SOCDNX_IF_ERR_EXIT(rv);

            rv = soc_port_sw_db_interface_type_get(unit, logical_port, &interface_type);
            SOCDNX_IF_ERR_EXIT(rv);

            if (interface_type == SOC_PORT_IF_NULL ||
                interface_type == SOC_PORT_IF_CPU  ||
                interface_type == SOC_PORT_IF_OLP  ||
                interface_type == SOC_PORT_IF_RCY  ||
                interface_type == SOC_PORT_IF_ERP  ||
                interface_type == SOC_PORT_IF_OAMP ||
                interface_type == SOC_PORT_IF_SAT  ||
                interface_type == SOC_PORT_IF_IPSEC) {
                continue;
            }

            rv = soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy_port);
            SOCDNX_IF_ERR_EXIT(rv);

            interface_i = phy_port - 1;

            if (SOC_IS_ARDON(unit)) {
                
                valid = SOC_DPP_CONFIG(unit)->arad->init.fc.cl_sch_enable ? 0x0 : 0x1;
            } else {
                valid = 0x1;
            }

            entry_offset = ((interface_i * SOC_TMC_EGR_NOF_Q_PRIO_ARAD) / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD);
            if(entry_offset > SOC_MEM_SIZE(unit, CFC_NIF_PFC_MAPm)) {
                
                continue;
            }

            rv = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
            SOCDNX_IF_ERR_EXIT(rv);

            if(nof_priorities > FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) {
                rv = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset + 1, nif_pfc_data_2);
                SOCDNX_IF_ERR_EXIT(rv);
            }

            for(i = 0; i < nof_priorities; i++) {
                field_offset = i % FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
                data_select = (i < FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) ? nif_pfc_data : nif_pfc_data_2;

                soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, index_field[field_offset], base_queue_pair + i);
                soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, select_field[field_offset], 0);
                soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, valid_field[field_offset], valid);
            }

            rv = soc_mem_write(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
            SOCDNX_IF_ERR_EXIT(rv);

            if(nof_priorities > FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) {
                rv = soc_mem_write(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset + 1, nif_pfc_data_2);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_flow_control_init_oob_rx(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        interface_number,
        fld_val,
        fld;
    uint32
        per_rep_idx,
        rep_idx,
        entry_idx,
        cal_length = 0,
        cal_reps = 0;
    uint64
        field64;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    SOC_TMC_FC_CAL_MODE
        cal_mode_ndx;
    soc_dpp_tm_config_t  *tm;

    SOCDNX_INIT_FUNC_DEFS;
 
    SOC_REG_ABOVE_64_CLEAR(cal_table_data);
    tm = &(SOC_DPP_CONFIG(unit)->tm);

    for(interface_number = 0; interface_number < SOC_TMC_FC_NOF_OOB_IDS; interface_number++)
    {
        if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_NONE)
            continue;

        
        if(tm->fc_oob_mode[interface_number] & SOC_DPP_FC_CAL_MODE_RX_ENABLE)
        {
            
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_ENf : ILKN_RX_1_ENf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
            COMPILER_64_SET(field64,0,fld_val);
            rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
            SOCDNX_IF_ERR_EXIT(rv);

            

            
            fld_val = 0x0;
            rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_OOB_RX_0_ERR_FC_STATUS_SELf,  fld_val);
			SOCDNX_IF_ERR_EXIT(rv);
            rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_OOB_RX_1_ERR_FC_STATUS_SELf,  fld_val);
			SOCDNX_IF_ERR_EXIT(rv);

            
            fld_val = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 0);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_MODEf : OOB_RX_1_MODEf);                            
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);

            
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_SELf : ILKN_RX_1_SELf);                                            
            rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            if(tm->fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld_val = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC ? 1 : 0);
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_0_OOB_MODEf : SPI_1_OOB_MODEf);                                            
                rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
                SOCDNX_IF_ERR_EXIT(rv);
            }

            
            cal_length = tm->fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
            {
                fld = (interface_number == ARAD_FC_ILKN_ID_A ? ILKN_OOB_RX_0_CAL_LENf : ILKN_OOB_RX_1_CAL_LENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_length - 1);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            else
            {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_LENf : SPI_OOB_RX_1_CAL_LENf);
                COMPILER_64_SET(field64, 0, cal_length);
                rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            } 

            
            cal_reps = tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 
                       tm->fc_oob_calender_rep_count[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_Mf : SPI_OOB_RX_1_CAL_Mf);
            COMPILER_64_SET(field64,0,cal_reps);
            rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
            SOCDNX_IF_ERR_EXIT(rv);

            
            cal_mode_ndx = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) ? SOC_TMC_FC_CAL_MODE_ILKN_OOB : SOC_TMC_FC_CAL_MODE_SPI_OOB;
            rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, interface_number, &cal_table);
            SOCDNX_IF_ERR_EXIT(rv);
            for (per_rep_idx = 0; per_rep_idx < cal_length; per_rep_idx++) {
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, 0);
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_DST_SELf, arad_fc_rec_cal_dest_type_to_val_internal(SOC_TMC_FC_REC_CAL_DEST_NONE));

                for (rep_idx = 0; rep_idx < cal_reps; rep_idx++) {
                    entry_idx = (rep_idx * cal_length) + per_rep_idx;
                    rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }

            

            
            fld_val = 0x1;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            
            fld_val = 0x1;
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_ENf : ILKN_RX_1_ENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_SPI || tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
                COMPILER_64_SET(field64,0,fld_val);
                rv = soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_flow_control_init_oob_tx(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        interface_number,
        fld_val,
        fld;
    uint32
        per_rep_idx,
        rep_idx,
        entry_idx,
        cal_length = 0,
        cal_reps = 0;
    uint64
        field64;
    uint32 
        interfcae_status_oob_ignore;
    soc_pbmp_t
        phy_ports;
    uint32 
        clp0_lanes,
        clp1_lanes,
        lanes_bitmap;
    int 
        lanes_bitmap_length;
    uint64 
        reg_val64;
    soc_field_t 
        intf_f[] = {FRC_ILKN_OOB_TX_0_INTFf, FRC_ILKN_OOB_TX_1_INTFf}, 
        lanes_f[] = {FRC_ILKN_OOB_TX_0_LANESf, FRC_ILKN_OOB_TX_1_LANESf};
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    SOC_TMC_FC_CAL_MODE
        cal_mode_ndx;
    soc_dpp_tm_config_t  *tm;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);
    tm = &(SOC_DPP_CONFIG(unit)->tm);
  
    for(interface_number = 0; interface_number < SOC_TMC_FC_NOF_OOB_IDS; interface_number++)
    {
        
        rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_0_OUT_SPEEDf, tm->fc_oob_tx_speed[interface_number]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        interfcae_status_oob_ignore = soc_property_port_get(unit, interface_number, spn_ILKN_INTERFACE_STATUS_OOB_IGNORE, 0);;
        soc_arad_ilkn_phy_ports_btmp_set(unit, interface_number, &phy_ports);

        
        rv = READ_CFC_ILKN_OOB_TX_FRCr(unit, &reg_val64);
		SOCDNX_IF_ERR_EXIT(rv);
        lanes_bitmap = 0;
        lanes_bitmap_length = soc_reg_field_length(unit, CFC_ILKN_OOB_TX_FRCr, FRC_ILKN_OOB_TX_1_LANESf);
        clp0_lanes = clp1_lanes = 0;
        SHR_BITSET_RANGE(&lanes_bitmap, 0, lanes_bitmap_length); 
      
        if(interfcae_status_oob_ignore) {
            
            soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, intf_f[interface_number], 1);
            soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, lanes_f[interface_number], lanes_bitmap );
        } else {
            
            SHR_BITCOPY_RANGE(&clp0_lanes, 0, &SOC_PBMP_WORD_GET(phy_ports, 0), ARAD_NIF_CLP0_PORT_MIN+1, ARAD_NIF_CLP_NOF_PORTS);
            SHR_BITCOPY_RANGE(&clp1_lanes, 0, &SOC_PBMP_WORD_GET(phy_ports, 0), ARAD_NIF_CLP1_PORT_MIN+1, ARAD_NIF_CLP_NOF_PORTS);
            lanes_bitmap = lanes_bitmap & ( ~ (clp0_lanes | (clp1_lanes << 12)));
            soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, intf_f[interface_number], 0);
            soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, lanes_f[interface_number], lanes_bitmap);
        }
        rv = WRITE_CFC_ILKN_OOB_TX_FRCr(unit, reg_val64);
		SOCDNX_IF_ERR_EXIT(rv);
        

        if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_NONE)
            continue;

        
        if(tm->fc_oob_mode[interface_number] & SOC_DPP_FC_CAL_MODE_TX_ENABLE)
        {
            
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_RSTNf : OOB_TX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            
            fld_val = 0x0;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_ENf : ILKN_TX_1_ENf);
            COMPILER_64_SET(field64,0,fld_val);
            rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
            SOCDNX_IF_ERR_EXIT(rv);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            

            
            fld_val = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 0);
            COMPILER_64_SET(field64,0,fld_val);
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_MODEf : OOB_TX_1_MODEf);        
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
			SOCDNX_IF_ERR_EXIT(rv);

                        
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_SELf : ILKN_TX_1_SELf);   
            rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
			SOCDNX_IF_ERR_EXIT(rv);

            
            cal_length = tm->fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_TX];
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld = (interface_number == ARAD_FC_ILKN_ID_A ? ILKN_TX_0_CAL_LENf : ILKN_TX_1_CAL_LENf);
                COMPILER_64_SET(field64,0,cal_length - 1);
                rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            else
            { 
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_LENf : SPI_OOB_TX_1_CAL_LENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_length);
                SOCDNX_IF_ERR_EXIT(rv);
            } 

            
            cal_reps = tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 
                           1 : 
                           tm->fc_oob_calender_rep_count[interface_number][SOC_TMC_CONNECTION_DIRECTION_TX];
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_Mf : SPI_OOB_TX_1_CAL_Mf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_reps);
            SOCDNX_IF_ERR_EXIT(rv);

            
            cal_mode_ndx = (tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) ? SOC_TMC_FC_CAL_MODE_ILKN_OOB : SOC_TMC_FC_CAL_MODE_SPI_OOB;
            rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, interface_number, &cal_table);
            SOCDNX_IF_ERR_EXIT(rv);
            for (per_rep_idx = 0; per_rep_idx < cal_length; per_rep_idx++) {
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, 0);
                soc_mem_field32_set(unit, cal_table, cal_table_data, FC_SRC_SELf, arad_fc_gen_cal_src_type_to_val_internal(unit, SOC_TMC_FC_GEN_CAL_SRC_CONST));

                for (rep_idx = 0; rep_idx < cal_reps; rep_idx++) {
                    entry_idx = (rep_idx * cal_length) + per_rep_idx;

                    rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
                    SOCDNX_IF_ERR_EXIT(rv);
                }
            }

            

            
            fld_val = 0x1;
            fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_RSTNf : OOB_TX_1_RSTNf);
            rv = soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
            SOCDNX_IF_ERR_EXIT(rv);

            
            fld_val = 0x1;
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_ENf : ILKN_TX_1_ENf);
                COMPILER_64_SET(field64,0,fld_val);
                rv = soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            if(tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_SPI || tm->fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC) {
                fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
                rv = soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val);
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int aradon_flow_control_init_cl_sch(int unit, int enable)
{
    uint32
        reg32_val,
        field32_val;
     
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, &reg32_val));
    field32_val = enable;
    soc_reg_field_set(unit, CFC_CFC_ENABLERSr, &reg32_val, NIF_TO_SCH_SHR_ENBf, field32_val);
    field32_val = 0x1; 
    soc_reg_field_set(unit, CFC_CFC_ENABLERSr, &reg32_val, FRC_NIF_LLVL_ENf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, reg32_val));


    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, SCH_REG_1BCr, REG_PORT_ANY, 0, &reg32_val)); 
    field32_val = enable;
    soc_reg_field_set(unit, SCH_REG_1BCr, &reg32_val, FIELD_0_0f, field32_val); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, SCH_REG_1BCr, REG_PORT_ANY, 0, reg32_val));

    field32_val = enable ? 255 : 0;
    soc_reg_field_set(unit, SCH_REG_1BCr, &reg32_val, FIELD_4_15f, field32_val); 

    field32_val = enable ? 0 : 1;
    soc_reg_field_set(unit, SCH_REG_1BCr, &reg32_val, FIELD_28_28f, field32_val); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, SCH_REG_1BCr, REG_PORT_ANY, 0, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBI_ENABLE_PKT_BOUNDARY_LLFCr, REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0x1;
    soc_reg_field_set(unit, NBI_ENABLE_PKT_BOUNDARY_LLFCr, &reg32_val, ENABLE_PKT_BOUNDARY_LLFCf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBI_ENABLE_PKT_BOUNDARY_LLFCr, REG_PORT_ANY, 0, reg32_val));
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_flow_control_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_dpp_tm_config_t  
        *tm;

    SOCDNX_INIT_FUNC_DEFS;

    tm = &(SOC_DPP_CONFIG(unit)->tm);

    rv = arad_flow_control_regs_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_flow_control_init_oob_rx(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_flow_control_init_oob_tx(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_ARDON(unit)) {
        rv = aradon_flow_control_init_cl_sch(unit, tm->fc_cl_sch_enable);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fc_inbnd_max_nof_priorities_get(
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
  switch(arad_nif_id2type(nif_ndx))
  {
  case ARAD_NIF_TYPE_SGMII:
    return 2;
  case ARAD_NIF_TYPE_XAUI:
    return 8;
  case ARAD_NIF_TYPE_RXAUI:
    return 8;
  default:
    return 0;
  }
}

soc_error_t
  arad_fc_enables_verify(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *ena_info
    )
{
    int i = 0;
    SOC_TMC_FC_ENABLE fc_enable_not_support[] = FC_ENABLE_NOT_SUPPORT_ARAD;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(cfc_enables);
    SOCDNX_NULL_CHECK(ena_info);

    for (i = 0; i < sizeof(fc_enable_not_support)/sizeof(SOC_TMC_FC_ENABLE); i++) {
        if (SHR_BITGET(cfc_enables->bmp, fc_enable_not_support[i])) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("cfc_enables[i] is not supported by this chip")));
        }
    }   

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
   arad_fc_enables_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *ena_info
    )
{
    soc_error_t rv = SOC_E_NONE;
    
    soc_reg_above_64_val_t reg_data;
    soc_reg_t reg = CFC_CFC_ENABLERSr;
    uint32 field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_fc_enables_verify(unit, cfc_enables, ena_info);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SET_FC_ENABLE(SOC_TMC_FC_EN, CFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_LLFC_EN, EGQ_TO_NIF_CNM_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_PFC_EN, EGQ_TO_NIF_CNM_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN, EGQ_TO_SCH_DEVICE_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_EN, EGQ_TO_SCH_ERP_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN, EGQ_TO_SCH_ERP_TC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_IF_EN, EGQ_TO_SCH_IF_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_PFC_EN, EGQ_TO_SCH_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN, GLB_RSC_TO_EGQ_RCL_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG, GLB_RSC_TO_HCFC_HP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG, GLB_RSC_TO_HCFC_LP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG, GLB_RSC_TO_RCL_PFC_HP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG, GLB_RSC_TO_RCL_PFC_LP_CFGf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN, ILKN_RX_TO_EGQ_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN, ILKN_RX_TO_EGQ_PORT_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN, ILKN_RX_TO_GEN_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN, ILKN_RX_TO_NIF_FAST_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_RET_REQ_EN, ILKN_RX_TO_RET_REQ_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN, LLFC_VSQ_TO_NIF_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_PFC_VSQ_TO_NIF_EN, PFC_VSQ_TO_NIF_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SCH_OOB_RX_EN, SCH_OOB_RX_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN, SPI_OOB_RX_TO_EGQ_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN, SPI_OOB_RX_TO_EGQ_PORT_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN, SPI_OOB_RX_TO_GEN_PFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN, SPI_OOB_RX_TO_NIF_FAST_LLFC_ENf);
    SET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_RET_REQ_EN, SPI_OOB_RX_TO_RET_REQ_ENf);

    rv = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
   arad_fc_enables_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN SOC_TMC_FC_ENABLE_BITMAP      *cfc_enables,
      SOC_SAND_OUT SOC_TMC_FC_ENABLE_BITMAP     *ena_info
    )
{
    soc_error_t rv = SOC_E_NONE;
    
    soc_reg_above_64_val_t reg_data;
    soc_reg_t reg = CFC_CFC_ENABLERSr;
    uint32 field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_fc_enables_verify(unit, cfc_enables, ena_info);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(reg_data);
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    GET_FC_ENABLE(SOC_TMC_FC_EN, CFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_LLFC_EN, EGQ_TO_NIF_CNM_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_NIF_CNM_PFC_EN, EGQ_TO_NIF_CNM_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN, EGQ_TO_SCH_DEVICE_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_EN, EGQ_TO_SCH_ERP_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN, EGQ_TO_SCH_ERP_TC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_IF_EN, EGQ_TO_SCH_IF_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_EGQ_TO_SCH_PFC_EN, EGQ_TO_SCH_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN, GLB_RSC_TO_EGQ_RCL_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG, GLB_RSC_TO_HCFC_HP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG, GLB_RSC_TO_HCFC_LP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG, GLB_RSC_TO_RCL_PFC_HP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG, GLB_RSC_TO_RCL_PFC_LP_CFGf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN, ILKN_RX_TO_EGQ_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN, ILKN_RX_TO_EGQ_PORT_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN, ILKN_RX_TO_GEN_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN, ILKN_RX_TO_NIF_FAST_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_ILKN_RX_TO_RET_REQ_EN, ILKN_RX_TO_RET_REQ_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN, LLFC_VSQ_TO_NIF_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_PFC_VSQ_TO_NIF_EN, PFC_VSQ_TO_NIF_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SCH_OOB_RX_EN, SCH_OOB_RX_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN, SPI_OOB_RX_TO_EGQ_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN, SPI_OOB_RX_TO_EGQ_PORT_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN, SPI_OOB_RX_TO_GEN_PFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN, SPI_OOB_RX_TO_NIF_FAST_LLFC_ENf);
    GET_FC_ENABLE(SOC_TMC_FC_SPI_OOB_RX_TO_RET_REQ_EN, SPI_OOB_RX_TO_RET_REQ_ENf);
  
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
  arad_fc_gen_inbnd_pfc_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_PFC      *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset,
        reg_offset,
        bit_offset, 
        fld_offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    soc_reg_above_64_val_t  
        pfc_map_reg;
    int
        core;
    soc_field_t 
        rx_mlf_pfc_fld[4] = {
            FC_TX_N_PORT_0_GEN_PFC_FROM_MLF_MASKf,
            FC_TX_N_PORT_1_GEN_PFC_FROM_MLF_MASKf, 
            FC_TX_N_PORT_2_GEN_PFC_FROM_MLF_MASKf, 
            FC_TX_N_PORT_3_GEN_PFC_FROM_MLF_MASKf};
    uint32
        pfc_map_rx_mlf_reg;
    soc_mem_t 
        mem = INVALIDm;
    soc_reg_above_64_val_t
        map_data;
    uint32
        entry_offset = 0; 
    soc_reg_above_64_val_t 
        cat2tc_map_ena;
    soc_reg_t 
        cat2tc_map_ena_reg = INVALIDr;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset );
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egq_ppct_tbl_get_unsafe(
          unit,
          core,
          base_q_pair,
          &data
          );
    SOCDNX_IF_ERR_EXIT(rv);

    data.cnm_intrcpt_fc_en = SOC_SAND_BOOL2NUM(info->cnm_intercept_enable);
    data.cnm_intrcpt_fc_vec_pfc = info->cnm_pfc_channel;

    rv = arad_egq_ppct_tbl_set_unsafe(
          unit,
          core,
          base_q_pair,
          &data
          );
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_LOW) { 
        SOC_REG_ABOVE_64_CLEAR(pfc_map_reg);
        rv = READ_CFC_LP_GLB_RSC_TO_NIF_PFC_MAPr(unit, pfc_map_reg);
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITCOPY_RANGE(pfc_map_reg, (offset*8), &(info->glbl_rcs_low), 0, 8);

        rv = WRITE_CFC_LP_GLB_RSC_TO_NIF_PFC_MAPr(unit, pfc_map_reg);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_NIF) {
        reg_offset = offset / ARAD_MAX_NIFS_PER_WC;
        fld_offset = offset % ARAD_MAX_NIFS_PER_WC;

        rv = READ_NBI_FC_PFC_MLF_MASKr(unit, reg_offset, &pfc_map_rx_mlf_reg);
        SOCDNX_IF_ERR_EXIT(rv);
  
        soc_reg_field_set(unit, NBI_FC_PFC_MLF_MASKr, &pfc_map_rx_mlf_reg, rx_mlf_pfc_fld[fld_offset], info->nif_cls_bitmap);

        rv = WRITE_NBI_FC_PFC_MLF_MASKr(unit, reg_offset, pfc_map_rx_mlf_reg);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_VSQ_CAT2TC) {
        mem = CFC_CAT_2_TC_MAP_NIFm;
        entry_offset = info->cat2_tc;

        cat2tc_map_ena_reg = CFC_CAT_2_TC_MAP_NIF_ENAr;
        bit_offset = info->cat2_tc;

        SOC_REG_ABOVE_64_CLEAR(map_data);
        
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITCOPY_RANGE(map_data, (offset*8), &(info->cat2_tc_bitmap), 0, 8);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        SOC_REG_ABOVE_64_CLEAR(cat2tc_map_ena);
        rv = soc_reg_above_64_get(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
        SOCDNX_IF_ERR_EXIT(rv);
        
        SHR_BITSET(cat2tc_map_ena, bit_offset);
        
        rv = soc_reg_above_64_set(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  arad_fc_gen_inbnd_ll_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_LL       *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    int
        core;
    uint32 
	    llfc_map;
 
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset );
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              base_q_pair,
              &data
              );
    SOCDNX_IF_ERR_EXIT(rv);

    data.cnm_intrcpt_fc_en = SOC_SAND_BOOL2NUM(info->cnm_enable);
    
    data.cnm_intrcpt_fc_vec_llfc = offset; 

    rv = arad_egq_ppct_tbl_set_unsafe(
              unit,
              core,
              base_q_pair,
              &data
              );
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_NIF) {

        rv = READ_NBI_FC_TX_GEN_LLFC_FROM_MLF_MASKr(unit, &llfc_map);
        SOCDNX_IF_ERR_EXIT(rv);
        if (info->nif_enable) {
            llfc_map |= (1<<offset);
        }
        else {
            llfc_map &= ~(1<<offset);
        }

        rv = WRITE_NBI_FC_TX_GEN_LLFC_FROM_MLF_MASKr(unit, llfc_map);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_gen_inbnd_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_port_t                         port,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO          *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    SOC_TMC_FC_GEN_INBND_LL
        ll_disabled;
    SOC_TMC_FC_GEN_INBND_PFC
        pfc_disabled;
    SOC_TMC_FC_ENABLE_BITMAP
        cfc_enables;
  
    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_FC_GEN_INBND_LL_clear(&ll_disabled);
    SOC_TMC_FC_GEN_INBND_PFC_clear(&pfc_disabled);
    pfc_disabled.inherit = SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED;

    
    SHR_BITCLR_RANGE(cfc_enables.bmp, 0, SOC_TMC_FC_NOF_ENABLEs);
    
    switch (info->mode) {
    case SOC_TMC_FC_INBND_MODE_LL:
        if (info->ll.gen_src == SOC_TMC_FC_GEN_SRC_VSQ_LLFC) {
            SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN);
        }
        break;
    case SOC_TMC_FC_INBND_MODE_PFC:
    case SOC_TMC_FC_INBND_MODE_SAFC:
        if (info->pfc.gen_src == SOC_TMC_FC_GEN_SRC_VSQ_PFC) {
            SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_PFC_VSQ_TO_NIF_EN);
        }
        break;
    case SOC_TMC_FC_INBND_MODE_DEVICE_DISABLED:
        SHR_BITSET(cfc_enables.bmp, (SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN | SOC_TMC_FC_PFC_VSQ_TO_NIF_EN));     
        break;
    default:
    case SOC_TMC_FC_INBND_MODE_DISABLED:
        break;
    }
    rv = arad_fc_enables_set(unit, &cfc_enables, &cfc_enables);
    SOCDNX_IF_ERR_EXIT(rv);

    

    
    rv = arad_fc_inbnd_mode_set(unit, port, 1, info->mode);
    SOCDNX_IF_ERR_EXIT(rv);
    

    
    switch (info->mode)
    {
    case SOC_TMC_FC_INBND_MODE_LL:
        
        rv = arad_fc_gen_inbnd_ll_set_unsafe(
            unit,
            port,
            &info->ll
          );
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_TMC_FC_INBND_MODE_PFC:
    case SOC_TMC_FC_INBND_MODE_SAFC:
        
        rv = arad_fc_gen_inbnd_pfc_set_unsafe(
            unit,
            port,
            &info->pfc
          );
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_TMC_FC_INBND_MODE_DISABLED:
    default:
        break;
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  arad_fc_gen_inbnd_pfc_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_PFC      *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset,
        reg_offset,
        bit_offset,
        fld_offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    soc_reg_above_64_val_t  
        pfc_map_reg;
    soc_field_t 
        rx_mlf_pfc_fld[4] = {
        FC_TX_N_PORT_0_GEN_PFC_FROM_MLF_MASKf,
        FC_TX_N_PORT_1_GEN_PFC_FROM_MLF_MASKf, 
        FC_TX_N_PORT_2_GEN_PFC_FROM_MLF_MASKf, 
        FC_TX_N_PORT_3_GEN_PFC_FROM_MLF_MASKf};
    uint32
        pfc_map_rx_mlf_reg;
    int
        core;
    soc_mem_t 
        mem= INVALIDm;
    soc_reg_above_64_val_t
        map_data;
    uint32
        entry_offset = 0; 
    soc_reg_above_64_val_t 
        cat2tc_map_ena_data;
    soc_reg_t 
        cat2tc_map_ena_reg = INVALIDr;
    int 
        ca2tc_map_ena = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset );
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    
    info->cnm_intercept_enable = 0;
     rv = arad_egq_ppct_tbl_get_unsafe(
                    unit,
                    core,
                    base_q_pair,
                    &data
                    );
    SOCDNX_IF_ERR_EXIT(rv);
    info->cnm_intercept_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_fc_en);
    

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_GLB_LOW) {
        SOC_REG_ABOVE_64_CLEAR(pfc_map_reg);
        rv = READ_CFC_LP_GLB_RSC_TO_NIF_PFC_MAPr(unit, pfc_map_reg);
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITCOPY_RANGE(&(info->glbl_rcs_low), 0, pfc_map_reg, (offset*8), 8);
    }
    

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_NIF) {
        reg_offset = offset / ARAD_MAX_NIFS_PER_WC;
        fld_offset = offset % ARAD_MAX_NIFS_PER_WC;

        rv = READ_NBI_FC_PFC_MLF_MASKr(unit, reg_offset, &pfc_map_rx_mlf_reg);
        SOCDNX_IF_ERR_EXIT(rv);

        info->nif_cls_bitmap = soc_reg_field_get(unit, NBI_FC_PFC_MLF_MASKr, pfc_map_rx_mlf_reg, rx_mlf_pfc_fld[fld_offset]);
    }
    

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_VSQ_CAT2TC) {
        mem = CFC_CAT_2_TC_MAP_NIFm;
        entry_offset = info->cat2_tc;
        
        cat2tc_map_ena_reg = CFC_CAT_2_TC_MAP_NIF_ENAr;
        bit_offset = info->cat2_tc;

        
        SOC_REG_ABOVE_64_CLEAR(cat2tc_map_ena_data);
        rv = soc_reg_above_64_get(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena_data);
        SOCDNX_IF_ERR_EXIT(rv);
        
        ca2tc_map_ena = SHR_BITGET(cat2tc_map_ena_data, bit_offset);

        
        if (ca2tc_map_ena) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
            SOCDNX_IF_ERR_EXIT(rv);

            SHR_BITCOPY_RANGE(&(info->cat2_tc_bitmap), 0, map_data, (offset*8), 8);
        }
        else {
            info->cat2_tc_bitmap = 0;
        }
    }
    

    info->inherit = SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  arad_fc_gen_inbnd_ll_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  soc_port_t                    port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_LL       *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        offset;
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        data;
    int
        core;
    uint32
	    llfc_map;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_core_get(unit, port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_first_phy_port_get(unit, port, &offset );
    SOCDNX_IF_ERR_EXIT(rv);
    offset--;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egq_ppct_tbl_get_unsafe(
                    unit,
                    core,
                    base_q_pair,
                    &data
                  );
    SOCDNX_IF_ERR_EXIT(rv);
    info->cnm_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_fc_en);

    
    if (info->gen_src == SOC_TMC_FC_GEN_SRC_NIF) {

        rv = READ_NBI_FC_TX_GEN_LLFC_FROM_MLF_MASKr(unit, &llfc_map);
        SOCDNX_IF_ERR_EXIT(rv);

        info->nif_enable = (llfc_map & (1<<offset)) ? 1 : 0;
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_gen_inbnd_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO  *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    
    
    rv = arad_fc_inbnd_mode_get(unit, port, 1, &(info->mode));
    SOCDNX_IF_ERR_EXIT(rv);
  
    
    if (info->mode == SOC_TMC_FC_INBND_MODE_LL)
    {
        rv = arad_fc_gen_inbnd_ll_get_unsafe(
                unit,
                port,
                &info->ll
              );
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    if (info->mode == SOC_TMC_FC_INBND_MODE_PFC || info->mode == SOC_TMC_FC_INBND_MODE_SAFC)
    {
        rv = arad_fc_gen_inbnd_pfc_get_unsafe(
                unit,
                port,
                &info->pfc
              );
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_fc_gen_inbnd_glb_hp_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;
   
  uint32
    ll_fld_val,
    pfc_fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_SET_UNSAFE);

  
  ll_fld_val = (fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL || fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_LLVL_ENf,  ll_fld_val));

  
  pfc_fld_val = (fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_PFC || fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_PFC_ENf,  pfc_fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_set_unsafe()", 0, 0);
}

uint32
  arad_fc_gen_inbnd_glb_hp_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fc_mode, (SOC_TMC_FC_NOF_INGR_GEN_GLB_HP_MODES-1), ARAD_FC_MODE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_set_verify()", 0, 0);
}

uint32
  arad_fc_gen_inbnd_glb_hp_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_GET_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_get_verify()", 0, 0);
}


uint32
  arad_fc_gen_inbnd_glb_hp_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;
   
  uint32
    ll_fld_val,
    pfc_fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_GET_UNSAFE);
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_LLVL_ENf, &ll_fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_PFC_ENf, &pfc_fld_val));
  
  
  if(ll_fld_val && pfc_fld_val)
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL;
  }
  else if (pfc_fld_val)
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_PFC;
  }
  else if (ll_fld_val)
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL;
  }
  else
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_NONE;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_get_unsafe()", 0, 0);
}


uint32
  arad_fc_gen_inbnd_cnm_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tc_field,
    tc_reg,
    fld_val;
  uint32
    tc_regs[3] = {EGQ_CNM_CPID_TO_FC_TYPE_0r, EGQ_CNM_CPID_TO_FC_TYPE_1r, EGQ_CNM_CPID_TO_FC_TYPE_2r};
  uint32
    tc_fields[8] = {CNM_CPID_0_TO_FC_TYPEf, CNM_CPID_1_TO_FC_TYPEf, CNM_CPID_2_TO_FC_TYPEf, CNM_CPID_3_TO_FC_TYPEf,
                   CNM_CPID_4_TO_FC_TYPEf, CNM_CPID_5_TO_FC_TYPEf, CNM_CPID_6_TO_FC_TYPEf, CNM_CPID_7_TO_FC_TYPEf};
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_SET_UNSAFE);

  
  tc_reg = tc_regs[cpid_tc_ndx / ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS];
  tc_field = tc_fields[cpid_tc_ndx];
  
  
  
  fld_val = SOC_SAND_BOOL2NUM(enable_ll);
  fld_val = fld_val | (fc_class << 1);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, tc_reg, REG_PORT_ANY, 0, tc_field,  fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_set_unsafe()", cpid_tc_ndx, 0);
}

uint32
  arad_fc_gen_inbnd_cnm_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpid_tc_ndx, ARAD_FC_CLASS_MAX - 1, ARAD_CPID_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fc_class, ARAD_FC_CLASS_MAX - 1, ARAD_FC_CLASS_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_set_verify()", cpid_tc_ndx, 0);
}

uint32
  arad_fc_gen_inbnd_cnm_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpid_tc_ndx, ARAD_FC_CLASS_MAX - 1, ARAD_CPID_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_get_verify()", cpid_tc_ndx, 0);
}

uint32
  arad_fc_gen_inbnd_cnm_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tc_reg,
    tc_field,
    fld_val;
  uint32
    tc_regs[3] = {EGQ_CNM_CPID_TO_FC_TYPE_0r, EGQ_CNM_CPID_TO_FC_TYPE_1r, EGQ_CNM_CPID_TO_FC_TYPE_2r};
  uint32
    tc_fields[8] = {CNM_CPID_0_TO_FC_TYPEf, CNM_CPID_1_TO_FC_TYPEf, CNM_CPID_2_TO_FC_TYPEf, CNM_CPID_3_TO_FC_TYPEf,
                    CNM_CPID_4_TO_FC_TYPEf, CNM_CPID_5_TO_FC_TYPEf, CNM_CPID_6_TO_FC_TYPEf, CNM_CPID_7_TO_FC_TYPEf};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fc_class);
  SOC_SAND_CHECK_NULL_INPUT(enable_ll);

  
  tc_reg = tc_regs[cpid_tc_ndx / ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS];
  tc_field = tc_fields[cpid_tc_ndx];

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, tc_reg, REG_PORT_ANY, 0, tc_field, &fld_val));

  
  
  *enable_ll = fld_val & 0x1;
  *fc_class = fld_val >> 1;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_get_unsafe()", cpid_tc_ndx, 0);
}




soc_error_t
  arad_fc_rec_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
    uint32
        res = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_nif_id_verify(
          nif_ndx
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
    {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fc_rcy_hr_enable_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_RCY_HR_ENABLE_GET_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rcy_hr_enable_get_verify()", 0, 0);
}


soc_error_t
  arad_fc_gen_cal_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        source_val = 0;
    uint32
        rep_idx = 0,      
        per_rep_idx = 0,  
        entry_idx = 0;    
    ARAD_CFC_CALTX_TBL_DATA
        tx_cal;
    uint64
        field64;
    uint32
        field32;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t 
        cal_table_data;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t 
        fld = INVALIDf;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = arad_fc_gen_cal_set_verify(unit, cal_mode_ndx, if_ndx, cal_conf, cal_buff);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);

    rv = arad_fc_clear_calendar_unsafe(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    
    for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
    {
        
        source_val = arad_fc_gen_cal_src_type_to_val_internal(unit, cal_buff[per_rep_idx].source);

        if (source_val == -1)
        {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        else if (cal_buff[per_rep_idx].source == SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT)
        {
            
            tx_cal.fc_source_sel = source_val;
            tx_cal.fc_index = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL : ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL);
        }
        else
        {
            tx_cal.fc_source_sel = source_val;
            tx_cal.fc_index = cal_buff[per_rep_idx].id;
        }

        rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, if_ndx, &cal_table);
        SOCDNX_IF_ERR_EXIT(rv);
        for (rep_idx = 0; rep_idx < cal_conf->cal_reps; rep_idx++)
        {
            entry_idx = (rep_idx * cal_conf->cal_len) + per_rep_idx;

            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, tx_cal.fc_index);
            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_SRC_SELf, tx_cal.fc_source_sel);
            
            rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } 

    
    
    
    if ((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) || (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)) {
        reg = CFC_OOB_PAD_CONFIGURATIONr;
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_MODEf : OOB_TX_1_MODEf);
        field32 = SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB);       
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) {
        reg = CFC_SPI_OOB_TX_CONFIGURATIONr;        
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
        field32 = 1;
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld , field32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    reg = CFC_ILKN_TX_CONFIGURATIONr;        
    if ((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) || (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)) {
        
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_SELf : ILKN_TX_1_SELf);
        field32 = SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB);       
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
		SOCDNX_IF_ERR_EXIT(rv);

        
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_ENf : ILKN_TX_1_ENf);
        field32 = cal_conf->enable;  
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
		SOCDNX_IF_ERR_EXIT(rv);
    }

    COMPILER_64_SET(field64,0,0x1);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);
    COMPILER_64_SET(field64,0,0x0);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);
    

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_gen_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_gen_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  arad_fc_gen_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        entry_idx = 0;    
    ARAD_CFC_CALTX_TBL_DATA
        tx_cal;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    soc_reg_t
        sel_reg = INVALIDr,
        en_reg = INVALIDr,
        cal_len_reg = INVALIDr,
        cal_rep_reg = INVALIDr;
    soc_field_t 
        sel_fld = INVALIDf, 
        en_fld = INVALIDf, 
        cal_len_fld = INVALIDf, 
        cal_rep_fld = INVALIDf;
    uint32
        sel_fld_val = 0,
        en_fld_val = 0,
        cal_len_val = 0,
        cal_rep_val = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = arad_fc_gen_cal_get_verify(unit, cal_mode_ndx, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_TMC_FC_CAL_IF_INFO_clear(cal_conf);
    SOC_TMC_FC_GEN_CALENDAR_clear(cal_buff);
    SOC_REG_ABOVE_64_CLEAR(cal_table_data);

    
    
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB){
        en_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
        en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf;
        rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else { 
        en_reg = CFC_ILKN_TX_CONFIGURATIONr;
        sel_reg = CFC_ILKN_TX_CONFIGURATIONr;
        sel_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_TX_0_SELf : ILKN_TX_1_SELf;
        rv = soc_reg_above_64_field32_read(unit, sel_reg, REG_PORT_ANY, 0, sel_fld, &sel_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
        if ((sel_fld_val && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)) || 
            (!sel_fld_val && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB))) {
            en_fld_val = 0;
        }
        else {
            en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_TX_0_ENf : ILKN_TX_1_ENf;
            rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
			SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    cal_conf->enable = SOC_SAND_NUM2BOOL(en_fld_val);
    

    
    
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_len_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_LENf : SPI_OOB_TX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val;
    }
    else
    {
        cal_len_reg = CFC_ILKN_TX_CONFIGURATIONr;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_CAL_LENf : ILKN_TX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val + 1;
    }
    

    
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_rep_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
        cal_rep_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_Mf : SPI_OOB_TX_1_CAL_Mf);

        rv = soc_reg_above_64_field32_read(unit, cal_rep_reg, REG_PORT_ANY, 0, cal_rep_fld, &cal_rep_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else 
    {
        cal_rep_val = 1;
    }
    cal_conf->cal_reps = cal_rep_val;
    

    

    

    rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_TX, if_ndx, &cal_table);
    SOCDNX_IF_ERR_EXIT(rv);
    
    for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
    {
        rv = soc_mem_read(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
        SOCDNX_IF_ERR_EXIT(rv);

        tx_cal.fc_index = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_INDEXf);
        tx_cal.fc_source_sel = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_SRC_SELf);

        if(tx_cal.fc_source_sel >= ARAD_FC_GEN_CAL_SRC_ARR_SIZE)
        {
            cal_buff[entry_idx].source = SOC_TMC_FC_GEN_CAL_SRC_NONE;
            cal_buff[entry_idx].id = 0;
        }
        else if(arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel] == SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT)
        {
            
            cal_buff[entry_idx].source = arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel];
            cal_buff[entry_idx].id = (tx_cal.fc_index == ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL ? ARAD_FC_ILKN_ID_A : ARAD_FC_ILKN_ID_B);
        }
        else
        {
            cal_buff[entry_idx].source = arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel];
            cal_buff[entry_idx].id = tx_cal.fc_index;
        }
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_clear_calendar_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE              cal_mode_ndx,
    SOC_SAND_IN  ARAD_FC_CAL_TYPE                 cal_type,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             if_ndx
  )
{
    uint32
        res = SOC_SAND_OK;
    soc_error_t
        rv = SOC_E_NONE;
    uint32 
        cal_rx_entry[128] = {0};
    uint32 
        cal_tx_entry[128] = {0};
    soc_mem_t
        cal_table = INVALIDm;
  
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, cal_type, if_ndx, &cal_table);
    SOCDNX_IF_ERR_EXIT(rv);
    if(cal_type == ARAD_FC_CAL_TYPE_RX)
    {
        soc_mem_field32_set(unit, cal_table, cal_rx_entry, FC_DST_SELf, arad_fc_rec_cal_dest_type_to_val_internal(SOC_TMC_FC_REC_CAL_DEST_NONE));
        soc_mem_field32_set(unit, cal_table, cal_rx_entry, FC_INDEXf, 0x0);

        res = arad_fill_table_with_entry(unit, cal_table, MEM_BLOCK_ANY, cal_rx_entry); 
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
    
    if(cal_type == ARAD_FC_CAL_TYPE_TX)
    {
        soc_mem_field32_set(unit, cal_table, cal_tx_entry, FC_SRC_SELf, arad_fc_gen_cal_src_type_to_val_internal(unit, SOC_TMC_FC_GEN_CAL_SRC_CONST));
        soc_mem_field32_set(unit, cal_table, cal_tx_entry, FC_INDEXf, 0x0);

        res = arad_fill_table_with_entry(unit, cal_table, MEM_BLOCK_ANY, cal_tx_entry); 
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_rec_cal_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        dest_sel = 0;
    uint32
        reg_val = 0;
    uint32
        rep_idx = 0,      
        per_rep_idx = 0,  
        entry_idx = 0;    
    ARAD_CFC_CALRX_TBL_DATA
        rx_cal;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t 
        cal_table_data;
    uint64
        field64;
    SOC_TMC_FC_ENABLE_BITMAP
        cfc_enables; 
    uint32
        field32;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t 
        fld = INVALIDf;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = arad_fc_rec_cal_set_verify(unit, cal_mode_ndx, if_ndx, cal_conf, cal_buff);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);
    
    rv = arad_fc_clear_calendar_unsafe(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    
    for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
    {
        
        dest_sel = arad_fc_rec_cal_dest_type_to_val_internal(cal_buff[per_rep_idx].destination);

        if(dest_sel == -1)
        {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        else if (cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_NONE)
        {
            rx_cal.fc_dest_sel = dest_sel;
            rx_cal.fc_index = 0;
        }
        else if (cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT)
        {
            
            rx_cal.fc_dest_sel = dest_sel;
            rx_cal.fc_index = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL : ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL);
        }
        else
        {
            rx_cal.fc_dest_sel = dest_sel;
            rx_cal.fc_index = cal_buff[per_rep_idx].id;
        }

        if(cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_NIF_LL)
        {
            
            rv = READ_NBI_FC_LLFC_STOP_TX_FROM_CFC_MASKr(unit, &reg_val);
			SOCDNX_IF_ERR_EXIT(rv);
            if (cal_conf->enable) {
                SHR_BITSET(&reg_val,cal_buff[per_rep_idx].id);
            }
            else {
                SHR_BITCLR(&reg_val,cal_buff[per_rep_idx].id);
            }
            rv = WRITE_NBI_FC_LLFC_STOP_TX_FROM_CFC_MASKr(unit,  reg_val);
			SOCDNX_IF_ERR_EXIT(rv);
        }

        rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, if_ndx, &cal_table);
        SOCDNX_IF_ERR_EXIT(rv);
        for (rep_idx = 0; rep_idx < cal_conf->cal_reps; rep_idx++)
        {
            entry_idx = (rep_idx * cal_conf->cal_len) + per_rep_idx;

            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_INDEXf, rx_cal.fc_index);
            soc_mem_field32_set(unit, cal_table, cal_table_data, FC_DST_SELf, rx_cal.fc_dest_sel);
            
            rv = soc_mem_write(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    

    
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) {
        reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
        field32 = cal_conf->enable;
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld , field32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    reg = CFC_ILKN_RX_CONFIGURATIONr;
    if ((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) || (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)) {
        
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_SELf : ILKN_RX_1_SELf);
        field32 = SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB);       
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
        SOCDNX_IF_ERR_EXIT(rv);
        
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_ENf : ILKN_RX_1_ENf);
        field32 = cal_conf->enable;
        rv = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  field32);
		SOCDNX_IF_ERR_EXIT(rv);
    }

    
    SHR_BITCLR_RANGE(((cfc_enables).bmp), 0, SOC_TMC_FC_NOF_ENABLEs);

    
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_ILKN_RX_TO_RET_REQ_EN);

    
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN);
    SHR_BITSET(cfc_enables.bmp, SOC_TMC_FC_SPI_OOB_RX_TO_RET_REQ_EN);

    rv = arad_fc_enables_set(unit, &cfc_enables, &cfc_enables);
    SOCDNX_IF_ERR_EXIT(rv);
    

    COMPILER_64_SET(field64,0, 0x1);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);
    COMPILER_64_SET(field64,0, 0x0);
    rv = soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64);
    SOCDNX_IF_ERR_EXIT(rv);

    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_rec_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_rec_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID                if_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (cal_mode_ndx >= SOC_TMC_FC_NOF_CAL_MODES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND) {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }
    else {
        if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_rec_cal_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        entry_idx = 0;    
    ARAD_CFC_CALRX_TBL_DATA
        rx_cal;
    soc_mem_t
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    soc_reg_t
        sel_reg = INVALIDr,
        en_reg = INVALIDr,
        cal_len_reg = INVALIDr,
        cal_rep_reg = INVALIDr;        
    soc_field_t 
        sel_fld = INVALIDf, 
        en_fld = INVALIDf, 
        cal_len_fld = INVALIDf, 
        cal_rep_fld = INVALIDf;
    uint32
        sel_fld_val = 0,
        en_fld_val = 0,
        cal_len_val = 0,
        cal_rep_val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cal_conf);
    SOCDNX_NULL_CHECK(cal_buff);

    rv = arad_fc_rec_cal_get_verify(unit, cal_mode_ndx, if_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(cal_table_data);

    
    
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB){
        en_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf;
        rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else { 
        en_reg = CFC_ILKN_RX_CONFIGURATIONr;
        sel_reg = CFC_ILKN_RX_CONFIGURATIONr;
        sel_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_RX_0_SELf : ILKN_RX_1_SELf;
        rv = soc_reg_above_64_field32_read(unit, sel_reg, REG_PORT_ANY, 0, sel_fld, &sel_fld_val);
		SOCDNX_IF_ERR_EXIT(rv);

        if ((sel_fld_val && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)) || 
            (!sel_fld_val && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB))) {
            en_fld_val = 0;
        }
        else {
            en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? ILKN_RX_0_ENf : ILKN_RX_1_ENf;
            rv = soc_reg_above_64_field32_read(unit, en_reg, REG_PORT_ANY, 0, en_fld, &en_fld_val);
			SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    cal_conf->enable = SOC_SAND_NUM2BOOL(en_fld_val);
    

    
    
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_len_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_LENf : SPI_OOB_RX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val;
    }
    else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
    {
        cal_len_reg = CFC_ILKN_RX_CONFIGURATIONr;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_CAL_LENf : ILKN_OOB_RX_1_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val+1;
    }
    else if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)
    {
        cal_len_reg = NBI_FC_ILKN_RX_CHFC_CAL_LENr;
        cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? FC_ILKN_RX_0_CHFC_CAL_LENf : FC_ILKN_RX_1_CHFC_CAL_LENf);
        rv = soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &cal_len_val);
        SOCDNX_IF_ERR_EXIT(rv);
        cal_conf->cal_len = cal_len_val;
    }
    

    
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
        cal_rep_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
        cal_rep_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_Mf : SPI_OOB_RX_1_CAL_Mf);

        rv = soc_reg_above_64_field32_read(unit, cal_rep_reg, REG_PORT_ANY, 0, cal_rep_fld, &cal_rep_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else 
    {
        cal_rep_val = 1;
    }
    cal_conf->cal_reps = cal_rep_val;
    
  
    

    
    rv = arad_fc_cal_tbl_get(unit, cal_mode_ndx, ARAD_FC_CAL_TYPE_RX, if_ndx, &cal_table);
    SOCDNX_IF_ERR_EXIT(rv);
    for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
    {
        rv = soc_mem_read(unit, cal_table, MEM_BLOCK_ANY, entry_idx, cal_table_data);
        SOCDNX_IF_ERR_EXIT(rv);

        rx_cal.fc_index = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_INDEXf);
        rx_cal.fc_dest_sel = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_DST_SELf);

        if(rx_cal.fc_dest_sel >= ARAD_FC_REC_CAL_DEST_ARR_SIZE)
        {
            cal_buff[entry_idx].destination = SOC_TMC_FC_REC_CAL_DEST_NONE;
            cal_buff[entry_idx].id = 0;
        }
        else if(arad_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel] == SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT)
        {
            
            cal_buff[entry_idx].destination = arad_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel];
            cal_buff[entry_idx].id = (rx_cal.fc_index == ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL ? ARAD_FC_ILKN_ID_A : ARAD_FC_ILKN_ID_B);
        }
        else
        {
            cal_buff[entry_idx].destination = arad_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel];
            cal_buff[entry_idx].id = rx_cal.fc_index;
        }
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_ilkn_llfc_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID          ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO        *info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        ilkn_bit_offset;
    soc_reg_t
        ilkn_llfc_reg = INVALIDr,
        multiple_use_bits_reg = INVALIDr;
    soc_field_t 
        on_ch0_fld = INVALIDf, 
        every_16_chs_fld = INVALIDf,
        multiple_use_bits_fld = INVALIDf;
    uint32
        buffer;
    uint64
        field64;
    int reg_arr_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    rv = arad_fc_ilkn_llfc_set_verify(unit, ilkn_ndx, direction_ndx, info);
    SOCDNX_IF_ERR_EXIT(rv);

    ilkn_llfc_reg = NBI_FC_ILKNr;
    multiple_use_bits_reg = (ilkn_ndx == 0) ? NBI_ILKN_0_MULTIPLE_USE_BITSr : NBI_ILKN_1_MULTIPLE_USE_BITSr;

    reg_arr_index = 0;

    if (ARAD_IS_DIRECTION_REC(direction_ndx)) {
        on_ch0_fld                 = FC_ILKN_RX_LLFC_ON_CH_0f;
        every_16_chs_fld           = FC_ILKN_RX_LLFC_EVERY_16_CHSf;
        multiple_use_bits_fld = (ilkn_ndx == 0) ? ILKN_RX_0_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf : ILKN_RX_1_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf;
    }
    else{
        on_ch0_fld                 = FC_ILKN_TX_GEN_LLFC_ON_CH_0f;
        every_16_chs_fld           = FC_ILKN_TX_GEN_LLFC_EVERY_16_CHSf;
        multiple_use_bits_fld = (ilkn_ndx == 0) ? ILKN_TX_0_MULTIPLE_USE_BITS_FROM_LLFC_MASKf : ILKN_TX_1_MULTIPLE_USE_BITS_FROM_LLFC_MASKf;        
    }

    ilkn_bit_offset = ilkn_ndx;
    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, on_ch0_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
    buffer |= SOC_SAND_SET_FLD_IN_PLACE(
                  info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_0 ? 1 : 0,
                  ilkn_bit_offset,
                  SOC_SAND_BIT(ilkn_bit_offset)
                );
    rv = soc_reg_above_64_field32_modify(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, on_ch0_fld,  buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, every_16_chs_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
    buffer |= SOC_SAND_SET_FLD_IN_PLACE(
                  info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N ? 1 : 0,
                  ilkn_bit_offset,
                  SOC_SAND_BIT(ilkn_bit_offset)
                );
    rv = soc_reg_above_64_field32_modify(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, every_16_chs_fld,  buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    if (ARAD_IS_DIRECTION_GEN(direction_ndx)) {
        rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, FC_ILKN_MODEf, &buffer);
        SOCDNX_IF_ERR_EXIT(rv);
        buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
        buffer |= SOC_SAND_SET_FLD_IN_PLACE(
                          info->cal_channel != SOC_TMC_FC_ILKN_CAL_LLFC_NONE ? 1 : 0,
                          ilkn_bit_offset,
                          SOC_SAND_BIT(ilkn_bit_offset)
                    );
        rv = soc_reg_above_64_field32_modify(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, FC_ILKN_MODEf,  buffer);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    
    buffer = info->multi_use_mask;
    COMPILER_64_SET(field64,0, buffer);
    rv = soc_reg_above_64_field64_modify(unit, multiple_use_bits_reg, REG_PORT_ANY, reg_arr_index, multiple_use_bits_fld,  field64);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_llfc_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID          ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_llfc_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_fc_ilkn_llfc_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        ilkn_bit_offset;
    uint32
        buffer;
    soc_reg_t
        ilkn_llfc_reg = INVALIDr,
        multiple_use_bits_reg = INVALIDr;
    soc_field_t 
        rx_on_ch0_fld = INVALIDf, 
        rx_every_16_chs_fld = INVALIDf,
        rx_multiple_use_bits_fld = INVALIDf,
        tx_on_ch0_fld = INVALIDf, 
        tx_every_16_chs_fld = INVALIDf,
        tx_multiple_use_bits_fld = INVALIDf;
    int reg_arr_index;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(rec_info);
    SOCDNX_NULL_CHECK(gen_info);

    rv = arad_fc_ilkn_llfc_get_verify(unit, ilkn_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_TMC_FC_ILKN_LLFC_INFO_clear(rec_info);
    SOC_TMC_FC_ILKN_LLFC_INFO_clear(gen_info);

    ilkn_llfc_reg = NBI_FC_ILKNr;
    multiple_use_bits_reg = (ilkn_ndx == 0) ? NBI_ILKN_0_MULTIPLE_USE_BITSr : NBI_ILKN_1_MULTIPLE_USE_BITSr;
    rx_multiple_use_bits_fld = (ilkn_ndx == 0) ? ILKN_RX_0_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf : ILKN_RX_1_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf;
    tx_multiple_use_bits_fld = (ilkn_ndx == 0) ? ILKN_TX_0_MULTIPLE_USE_BITS_FROM_LLFC_MASKf : ILKN_TX_1_MULTIPLE_USE_BITS_FROM_LLFC_MASKf;        
    reg_arr_index = 0;

    rx_on_ch0_fld                 = FC_ILKN_RX_LLFC_ON_CH_0f;
    rx_every_16_chs_fld           = FC_ILKN_RX_LLFC_EVERY_16_CHSf;
    tx_on_ch0_fld                 = FC_ILKN_TX_GEN_LLFC_ON_CH_0f;
    tx_every_16_chs_fld           = FC_ILKN_TX_GEN_LLFC_EVERY_16_CHSf;

    ilkn_bit_offset = ilkn_ndx;

    
    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, rx_on_ch0_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
        rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
    }
    else
    {
        rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, rx_every_16_chs_fld, &buffer);
        SOCDNX_IF_ERR_EXIT(rv);
        if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
        {
            rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
        }
        else
        {
            rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
        }
    }

    
    rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, tx_on_ch0_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
        gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
    }
    else
    {
        rv = soc_reg_above_64_field32_read(unit, ilkn_llfc_reg, REG_PORT_ANY, 0, tx_every_16_chs_fld, &buffer);
        SOCDNX_IF_ERR_EXIT(rv);
        if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
        {
            gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
        }
        else
        {
            gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
        }
    }

    rv = soc_reg_above_64_field32_read(unit, multiple_use_bits_reg, REG_PORT_ANY, reg_arr_index, rx_multiple_use_bits_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    rec_info->multi_use_mask = (uint8) buffer;
    rv = soc_reg_above_64_field32_read(unit, multiple_use_bits_reg, REG_PORT_ANY, reg_arr_index, tx_multiple_use_bits_fld, &buffer);
    SOCDNX_IF_ERR_EXIT(rv);
    gen_info->multi_use_mask = (uint8) buffer;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fc_ilkn_retransmit_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_INFO     *ret_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    rx_polarity_field[2] = {ILKN_OOB_RX_0_RT_POLARITYf, ILKN_OOB_RX_1_RT_POLARITYf},
    tx_polarity_field[2] = {ILKN_OOB_TX_0_RT_POLARITYf, ILKN_OOB_TX_1_RT_POLARITYf},
    err_indication_field[2] = {ILKN_OOB_RX_0_ERR_RT_STATUS_SELf, ILKN_OOB_RX_1_ERR_RT_STATUS_SELf};
  uint32
    reg_index = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ret_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_RET_REQ_ENf,  ret_info->enable));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_RET_REQ_ENf,  ret_info->enable));

  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    reg_index = 0;
  }
  if(ilkn_ndx == ARAD_NIF_ILKN_ID_B)
  {
    reg_index = 1;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, rx_polarity_field[reg_index],  ret_info->rx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, tx_polarity_field[reg_index],  ret_info->tx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, err_indication_field[reg_index],  ret_info->error_indication));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_retransmit_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_INFO     *ret_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    rx_polarity_field[2] = {ILKN_OOB_RX_0_RT_POLARITYf, ILKN_OOB_RX_1_RT_POLARITYf},
    tx_polarity_field[2] = {ILKN_OOB_TX_0_RT_POLARITYf, ILKN_OOB_TX_1_RT_POLARITYf},
    err_indication_field[2] = {ILKN_OOB_RX_0_ERR_RT_STATUS_SELf, ILKN_OOB_RX_1_ERR_RT_STATUS_SELf};
  uint32
    reg_index = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ret_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_RET_REQ_ENf, &ret_info->enable));

  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    reg_index = 0;
  }
  if(ilkn_ndx == ARAD_NIF_ILKN_ID_B)
  {
    reg_index = 1;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, rx_polarity_field[reg_index], &ret_info->rx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, tx_polarity_field[reg_index], &ret_info->tx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, err_indication_field[reg_index], &ret_info->error_indication));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_mub_rec_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                            bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_MUB_REC_SET_UNSAFE);
  
  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_0_MUB_RX_ENAf,  bitmap));
  } else {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_1_MUB_RX_ENAf,  bitmap));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_mub_rec_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_mub_rec_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT uint8                            *bitmap
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_MUB_REC_GET_UNSAFE);

  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_0_MUB_RX_ENAf, &fld_val));
  } else {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_1_MUB_RX_ENAf, &fld_val));
  }


  *bitmap = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_mub_rec_get_unsafe()", ilkn_ndx, 0);
}


soc_error_t
  arad_fc_ilkn_mub_channel_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx >  SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_mub_channel_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_t 
        reg = CFC_ILKN_MUB_ENABLEr;
    soc_field_t rx_flds[6] = {
        ILKN_0_MUB_RX_ENAf,
        ILKN_1_MUB_RX_ENAf, 
        ILKN_2_MUB_RX_ENAf,
        ILKN_3_MUB_RX_ENAf,
        ILKN_4_MUB_RX_ENAf,
        ILKN_5_MUB_RX_ENAf};
    soc_field_t tx_flds[6] = {
        ILKN_0_MUB_TX_ENAf,
        ILKN_1_MUB_TX_ENAf, 
        ILKN_2_MUB_TX_ENAf,
        ILKN_3_MUB_TX_ENAf,
        ILKN_4_MUB_TX_ENAf,
        ILKN_5_MUB_TX_ENAf};
    soc_field_t fld;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_ilkn_mub_channel_verify(unit, ilkn_ndx);
    SOCDNX_IF_ERR_EXIT(res);

    
    fld = (direction_ndx == SOC_TMC_FC_DIRECTION_GEN) ? tx_flds[ilkn_ndx] : rx_flds[ilkn_ndx];

    res = soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, fld,  bitmap);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_mub_channel_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID           ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_DIRECTION           direction_ndx,
    SOC_SAND_OUT uint8                          *bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_t 
        reg = CFC_ILKN_MUB_ENABLEr;
    soc_field_t rx_flds[6] = {
        ILKN_0_MUB_RX_ENAf,
        ILKN_1_MUB_RX_ENAf, 
        ILKN_2_MUB_RX_ENAf,
        ILKN_3_MUB_RX_ENAf,
        ILKN_4_MUB_RX_ENAf,
        ILKN_5_MUB_RX_ENAf};
    soc_field_t tx_flds[6] = {
        ILKN_1_MUB_TX_ENAf,
        ILKN_1_MUB_TX_ENAf, 
        ILKN_2_MUB_TX_ENAf,
        ILKN_3_MUB_TX_ENAf,
        ILKN_4_MUB_TX_ENAf,
        ILKN_5_MUB_TX_ENAf};
    soc_field_t fld;
    soc_reg_above_64_val_t reg_data_64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(bitmap);

    res = arad_fc_ilkn_mub_channel_verify(unit, ilkn_ndx);
    SOCDNX_IF_ERR_EXIT(res);

    SOC_REG_ABOVE_64_CLEAR(reg_data_64);
    
    fld = (direction_ndx == SOC_TMC_FC_DIRECTION_GEN) ? tx_flds[ilkn_ndx] : rx_flds[ilkn_ndx];

    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data_64);
    SOCDNX_IF_ERR_EXIT(res);

    (*bitmap) = soc_reg_above_64_field32_get(unit, reg, reg_data_64, fld);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_mub_gen_cal_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ilkn_ndx > SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ilkn_ndx is out of range")));
    }

    SOCDNX_NULL_CHECK(cal_info);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_mub_gen_cal_set (
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
    soc_error_t
        res = SOC_E_NONE;
    uint32 
        calendar_regs[] = 
            {CFC_ILKN_0_MUB_TX_CALr, CFC_ILKN_1_MUB_TX_CALr, CFC_ILKN_2_MUB_TX_CALr, 
             CFC_ILKN_3_MUB_TX_CALr, CFC_ILKN_4_MUB_TX_CALr, CFC_ILKN_5_MUB_TX_CALr};
    uint32
        fields_0[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_0_MUB_TX_MAP_0f, ILKN_0_MUB_TX_MAP_1f, ILKN_0_MUB_TX_MAP_2f, ILKN_0_MUB_TX_MAP_3f,
            ILKN_0_MUB_TX_MAP_4f, ILKN_0_MUB_TX_MAP_5f, ILKN_0_MUB_TX_MAP_6f, ILKN_0_MUB_TX_MAP_7f};
    uint32
        fields_1[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_1_MUB_TX_MAP_0f, ILKN_1_MUB_TX_MAP_1f, ILKN_1_MUB_TX_MAP_2f, ILKN_1_MUB_TX_MAP_3f,
            ILKN_1_MUB_TX_MAP_4f, ILKN_1_MUB_TX_MAP_5f, ILKN_1_MUB_TX_MAP_6f, ILKN_1_MUB_TX_MAP_7f};
    soc_reg_above_64_val_t  
        cal_data, entry_data;
    uint32
        entry_idx;
    uint32
        ilkn_id_offset = ilkn_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_ilkn_mub_gen_cal_verify(unit, ilkn_ndx, cal_info);
    SOCDNX_IF_ERR_EXIT(res);
    
    SOC_REG_ABOVE_64_CLEAR(cal_data);

    
    for(entry_idx = 0; entry_idx < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; entry_idx++)
    {
        
        SOC_REG_ABOVE_64_CLEAR(entry_data);
        
        entry_data[0] = arad_fc_gen_cal_src_type_to_val_internal(unit, cal_info->entries[entry_idx].source) << ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET;
        
        entry_data[0] |= (cal_info->entries[entry_idx].id & SOC_SAND_BITS_MASK(ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET-1, 0));

        
        if(ilkn_ndx == 0) {
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_0[entry_idx], entry_data);
        } else {
            soc_reg_above_64_field_set(unit, calendar_regs[ilkn_id_offset], cal_data, fields_1[entry_idx], entry_data);
        }
    }
  
    
    res = soc_reg_above_64_set(unit, calendar_regs[ilkn_id_offset], REG_PORT_ANY, 0, cal_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_ilkn_mub_gen_cal_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_ID             ilkn_ndx,
    SOC_SAND_OUT SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
    soc_error_t
        res = SOC_E_NONE;
    uint32 
        calendar_regs[] = 
            {CFC_ILKN_0_MUB_TX_CALr, CFC_ILKN_1_MUB_TX_CALr};
    uint32
        fields_0[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_0_MUB_TX_MAP_0f, ILKN_0_MUB_TX_MAP_1f, ILKN_0_MUB_TX_MAP_2f, ILKN_0_MUB_TX_MAP_3f,
            ILKN_0_MUB_TX_MAP_4f, ILKN_0_MUB_TX_MAP_5f, ILKN_0_MUB_TX_MAP_6f, ILKN_0_MUB_TX_MAP_7f};
    uint32
        fields_1[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
            {ILKN_1_MUB_TX_MAP_0f, ILKN_1_MUB_TX_MAP_1f, ILKN_1_MUB_TX_MAP_2f, ILKN_1_MUB_TX_MAP_3f,
            ILKN_1_MUB_TX_MAP_4f, ILKN_1_MUB_TX_MAP_5f, ILKN_1_MUB_TX_MAP_6f, ILKN_1_MUB_TX_MAP_7f};
    soc_reg_above_64_val_t  
        cal_data, entry_data;
    uint32
        entry_idx;
    uint32
        ilkn_id_offset = ilkn_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_ilkn_mub_gen_cal_verify(unit, ilkn_ndx, cal_info);
    SOCDNX_IF_ERR_EXIT(res);

    SOC_REG_ABOVE_64_CLEAR(cal_data);

    
    res = soc_reg_above_64_get(unit, calendar_regs[ilkn_id_offset], REG_PORT_ANY, 0, cal_data);
    SOCDNX_IF_ERR_EXIT(res);

    
    for(entry_idx = 0; entry_idx < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; entry_idx++)
    {
        SOC_REG_ABOVE_64_CLEAR(entry_data);
        if(ilkn_ndx == 0) {
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_0[entry_idx], entry_data);
        } else {
            soc_reg_above_64_field_get(unit, calendar_regs[ilkn_id_offset], cal_data, fields_1[entry_idx], entry_data);
        }

        
        cal_info->entries[entry_idx].source = arad_fc_gen_cal_src_arr[entry_data[0] >> ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET];

        
        cal_info->entries[entry_idx].id = (entry_data[0] & SOC_SAND_BITS_MASK(ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET-1, 0));
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_pfc_generic_bitmap_verify(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if ((priority < 0) || (priority > SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if(pfc_bitmap_index >= SOC_DPP_DEFS_GET(unit, nof_fc_pfc_generic_bitmaps)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_pfc_generic_bitmap_set(
    SOC_SAND_IN   int                              unit,
    SOC_SAND_IN   int                              priority,
    SOC_SAND_IN   uint32                           pfc_bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        reg_offset;
    soc_reg_above_64_val_t  
        pfc_map_data;
    uint32
        pfc_map_reg[SOC_DPP_DEFS_MAX(NOF_FC_PFC_GENERIC_BITMAPS)] = {
        CFC_PFC_GENERIC_BITMAP_0r, CFC_PFC_GENERIC_BITMAP_1r, CFC_PFC_GENERIC_BITMAP_2r, CFC_PFC_GENERIC_BITMAP_3r,
        CFC_PFC_GENERIC_BITMAP_4r, CFC_PFC_GENERIC_BITMAP_5r, CFC_PFC_GENERIC_BITMAP_6r, CFC_PFC_GENERIC_BITMAP_7r,
        CFC_PFC_GENERIC_BITMAP_8r, CFC_PFC_GENERIC_BITMAP_9r, CFC_PFC_GENERIC_BITMAP_10r, CFC_PFC_GENERIC_BITMAP_11r,
        CFC_PFC_GENERIC_BITMAP_12r, CFC_PFC_GENERIC_BITMAP_13r, CFC_PFC_GENERIC_BITMAP_14r, CFC_PFC_GENERIC_BITMAP_15r
        };

    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_fc_pfc_generic_bitmap_verify(unit, priority, pfc_bitmap_index);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(pfc_map_data);

    
    rv = soc_reg_above_64_get(unit, pfc_map_reg[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    for(reg_offset = 0; reg_offset < ARAD_FC_PFC_GENERIC_BITMAP_SIZE / 32; reg_offset++)
    {
        pfc_map_data[reg_offset] = pfc_bitmap->bitmap[reg_offset];
    }

    
    rv = soc_reg_above_64_set(unit, pfc_map_reg[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_pfc_generic_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index,
    SOC_SAND_OUT   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        reg_offset;
    soc_reg_above_64_val_t  
        pfc_map_data;
    uint32
        pfc_map_reg[SOC_DPP_DEFS_MAX(NOF_FC_PFC_GENERIC_BITMAPS)] = 
        {
        CFC_PFC_GENERIC_BITMAP_0r, CFC_PFC_GENERIC_BITMAP_1r, CFC_PFC_GENERIC_BITMAP_2r, CFC_PFC_GENERIC_BITMAP_3r,
        CFC_PFC_GENERIC_BITMAP_4r, CFC_PFC_GENERIC_BITMAP_5r, CFC_PFC_GENERIC_BITMAP_6r, CFC_PFC_GENERIC_BITMAP_7r,
        CFC_PFC_GENERIC_BITMAP_8r, CFC_PFC_GENERIC_BITMAP_9r, CFC_PFC_GENERIC_BITMAP_10r, CFC_PFC_GENERIC_BITMAP_11r,
        CFC_PFC_GENERIC_BITMAP_12r, CFC_PFC_GENERIC_BITMAP_13r, CFC_PFC_GENERIC_BITMAP_14r, CFC_PFC_GENERIC_BITMAP_15r
        };

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pfc_bitmap);
    rv = arad_fc_pfc_generic_bitmap_verify(unit, priority, pfc_bitmap_index);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(pfc_map_data);

    
    rv = soc_reg_above_64_get(unit, pfc_map_reg[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);
  
    
    for(reg_offset = 0; reg_offset < ARAD_FC_PFC_GENERIC_BITMAP_SIZE / 32; reg_offset++)
    {
        pfc_bitmap->bitmap[reg_offset] = pfc_map_data[reg_offset];
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_fc_pfc_generic_bitmap_valid_update(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    SOC_SAND_IN   int                               priority,
    SOC_SAND_OUT  uint32                           *is_valid)
{
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(src_type);
    COMPILER_REFERENCE(priority);

    SOCDNX_NULL_CHECK(is_valid);

    
    *is_valid = 1;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_fc_pfc_generic_bitmap_used_update(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE   src_type,
    SOC_SAND_IN   int                               priority,
    SOC_SAND_IN   int                               pfc_bitmap_index,
    SOC_SAND_OUT  uint32                            is_set)
{
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(src_type);
    COMPILER_REFERENCE(priority);
    COMPILER_REFERENCE(pfc_bitmap_index);
    COMPILER_REFERENCE(is_set);

    

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_cat_2_tc_hcfc_bitmap_verify(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (core > MAX_NUM_OF_CORES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("core is out of range")));
    }

    if ((tc < 0) || (tc > SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("tc is out of range")));
    }

    SOCDNX_NULL_CHECK(hcfc_bitmap);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_cat_2_tc_hcfc_bitmap_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t 
        rv = SOC_E_NONE;
    soc_mem_t
        mem = INVALIDm;          
    soc_reg_above_64_val_t
        map_data;
    uint32 
        entry_offset = 0,
        entry_length = 0;
    soc_reg_above_64_val_t 
        cat2tc_map_ena;
    soc_reg_t 
        cat2tc_map_ena_reg = INVALIDr;
    uint32
        bit_offset;

    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_fc_cat_2_tc_hcfc_bitmap_verify(unit, core, tc, is_ocb_only, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(rv);

    mem = CFC_CAT_2_TC_MAP_HCFCm;
    entry_offset = tc;

    cat2tc_map_ena_reg = CFC_CAT_2_TC_MAP_HCFC_ENAr;
    bit_offset = tc;

    entry_length = soc_mem_field_length(unit, mem, BITMAPf);

    SOC_REG_ABOVE_64_CLEAR(map_data);
    
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(map_data, 0, hcfc_bitmap->bitmap, 0, entry_length);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOC_REG_ABOVE_64_CLEAR(cat2tc_map_ena);
    rv = soc_reg_above_64_get(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
    SOCDNX_IF_ERR_EXIT(rv);
        
    SHR_BITSET(cat2tc_map_ena, bit_offset);
        
    rv = soc_reg_above_64_set(unit, cat2tc_map_ena_reg, REG_PORT_ANY, 0, cat2tc_map_ena);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_cat_2_tc_hcfc_bitmap_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           tc,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t
        mem = INVALIDm;          
    soc_reg_above_64_val_t 
        map_data;
    uint32 
        entry_offset = 0,
        entry_length = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_fc_cat_2_tc_hcfc_bitmap_verify(unit, core, tc, is_ocb_only, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(rv);

    mem = CFC_CAT_2_TC_MAP_HCFCm;
    entry_offset = tc;

    entry_length = soc_mem_field_length(unit, mem, BITMAPf);

    SOC_REG_ABOVE_64_CLEAR(map_data);
    
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(hcfc_bitmap->bitmap, 0, map_data, 0, entry_length);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_glb_hcfc_bitmap_verify(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (core > MAX_NUM_OF_CORES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("core is out of range")));
    }

    if ((pool_id != 0) && (pool_id != 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pool_id is out of range")));
    }

    SOCDNX_NULL_CHECK(hcfc_bitmap);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_glb_hcfc_bitmap_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_map_data;
    soc_reg_t
        map_reg = INVALIDr;
    uint32
        field_length = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_glb_hcfc_bitmap_verify(unit, core, is_high_prio, is_ocb_only, pool_id, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(res);

    map_reg = (is_high_prio) ? CFC_GLB_RSC_TO_HCFC_HP_MAPr : CFC_GLB_RSC_TO_HCFC_LP_MAPr;
    field_length = soc_reg_field_length(unit, CFC_GLB_RSC_TO_HCFC_HP_MAPr, GLB_RSC_TO_HCFC_HP_MAPf);

    SOC_REG_ABOVE_64_CLEAR(reg_map_data);
    res = soc_reg_above_64_get(unit, map_reg, REG_PORT_ANY, 0, reg_map_data);
    SOCDNX_IF_ERR_EXIT(res);

    SHR_BITCOPY_RANGE(reg_map_data, 0, hcfc_bitmap->bitmap, 0, field_length);

    res = soc_reg_above_64_set(unit, map_reg, REG_PORT_ANY, 0, reg_map_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_glb_hcfc_bitmap_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   int                           core,
    SOC_SAND_IN   int                           is_high_prio,
    SOC_SAND_IN   int                           is_ocb_only,
    SOC_SAND_IN   int                           pool_id,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_BITMAP        *hcfc_bitmap
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_map_data;
    soc_reg_t
        map_reg = INVALIDr;
    uint32
        field_length = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_glb_hcfc_bitmap_verify(unit, core, is_high_prio, is_ocb_only, pool_id, hcfc_bitmap);
    SOCDNX_IF_ERR_EXIT(res);

    map_reg = (is_high_prio) ? CFC_GLB_RSC_TO_HCFC_HP_MAPr : CFC_GLB_RSC_TO_HCFC_LP_MAPr;
    field_length = soc_reg_field_length(unit, CFC_GLB_RSC_TO_HCFC_HP_MAPr, GLB_RSC_TO_HCFC_HP_MAPf);

    SOC_REG_ABOVE_64_CLEAR(reg_map_data);
    res = soc_reg_above_64_get(unit, map_reg, REG_PORT_ANY, 0, reg_map_data);
    SOCDNX_IF_ERR_EXIT(res);

    SHR_BITCOPY_RANGE(hcfc_bitmap->bitmap, 0, reg_map_data, 0, field_length);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_hcfc_watchdog_set_verify(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_IN   SOC_TMC_FC_HCFC_WATCHDOG      *hcfc_watchdog
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
    }

    SOCDNX_NULL_CHECK(hcfc_watchdog);

    if (hcfc_watchdog->period * SOC_INFO(unit).frequency > SOC_DPP_DEFS_GET(unit, max_oob_wd_period)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("watchdog period is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_hcfc_watchdog_set(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_IN   SOC_TMC_FC_HCFC_WATCHDOG      *hcfc_watchdog
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg64_data;
    uint32
        reg32_data = 0,
        fld_val = 0;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t
        fld = INVALIDf;
    soc_field_t
        period_0_fld[] = {
            HCFC_OOB_RX_0_WD_PERIOD_0f, 
            HCFC_OOB_RX_0_WD_PERIOD_1f, 
            HCFC_OOB_RX_0_WD_PERIOD_2f,
            HCFC_OOB_RX_0_WD_PERIOD_3f,
            HCFC_OOB_RX_0_WD_PERIOD_4f}, 
        period_1_fld[] = {
            HCFC_OOB_RX_1_WD_PERIOD_0f, 
            HCFC_OOB_RX_1_WD_PERIOD_1f, 
            HCFC_OOB_RX_1_WD_PERIOD_2f,
            HCFC_OOB_RX_1_WD_PERIOD_3f,
            HCFC_OOB_RX_1_WD_PERIOD_4f};
    uint32
        hcfc_channel = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_hcfc_watchdog_set_verify(unit, if_ndx, hcfc_watchdog);
    SOCDNX_IF_ERR_EXIT(res);

    SOCDNX_NULL_CHECK(hcfc_watchdog);

    
    res = READ_CFC_HCFC_OOB_RX_ERR_CFGr(unit, &reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    fld_val = 0;
    fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? HCFC_OOB_RX_0_ERR_CFGf : HCFC_OOB_RX_1_ERR_CFGf;
    soc_reg_field_set(unit, CFC_HCFC_OOB_RX_ERR_CFGr, &reg32_data, fld, fld_val);

    res = WRITE_CFC_HCFC_OOB_RX_ERR_CFGr(unit, reg32_data);
    SOCDNX_IF_ERR_EXIT(res);;

    
    res = READ_CFC_HCFC_OOB_RX_WD_EN_CFGr(unit, &reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    fld_val = hcfc_watchdog->enable ? 1 : 0;
    fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? HCFC_OOB_RX_0_WD_ENf : HCFC_OOB_RX_1_WD_ENf;
    soc_reg_field_set(unit, CFC_HCFC_OOB_RX_WD_EN_CFGr, &reg32_data, fld, fld_val);

    res = WRITE_CFC_HCFC_OOB_RX_WD_EN_CFGr(unit, reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = READ_CFC_HCFC_OOB_RX_WD_ERR_STATUSr(unit, &reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    fld_val = hcfc_watchdog->error_status;
    fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? HCFC_OOB_RX_0_WD_ERR_STATUSf : HCFC_OOB_RX_1_WD_ERR_STATUSf;
    soc_reg_field_set(unit, CFC_HCFC_OOB_RX_WD_ERR_STATUSr, &reg32_data, fld, fld_val);

    res = WRITE_CFC_HCFC_OOB_RX_WD_ERR_STATUSr(unit, reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    
    SOC_REG_ABOVE_64_CLEAR(reg64_data);
    reg = (if_ndx == SOC_TMC_FC_OOB_ID_A)? CFC_HCFC_OOB_RX_0_WD_PERIOD_CFGr : CFC_HCFC_OOB_RX_1_WD_PERIOD_CFGr;
    
    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg64_data);
    SOCDNX_IF_ERR_EXIT(res);

    for (hcfc_channel = 0; hcfc_channel < SOC_TMC_FC_HCFC_CHANNEL_NUM; hcfc_channel++) {
        fld_val = hcfc_watchdog->period * SOC_INFO(unit).frequency;
        fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? period_0_fld[hcfc_channel] : period_1_fld[hcfc_channel];
        soc_reg_above_64_field32_set(unit, reg, reg64_data, fld, fld_val);
    }

    res = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg64_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_hcfc_watchdog_get_verify(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_WATCHDOG      *hcfc_watchdog
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (if_ndx >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("if_ndx is out of range")));
    }

    SOCDNX_NULL_CHECK(hcfc_watchdog);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_hcfc_watchdog_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_TMC_FC_CAL_IF_ID          if_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_HCFC_WATCHDOG      *hcfc_watchdog
  )
{
    soc_error_t
        res = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg64_data;
    uint32
        reg32_data = 0;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t
        fld = INVALIDf;
    soc_field_t
        period_0_fld[] = {
            HCFC_OOB_RX_0_WD_PERIOD_0f, 
            HCFC_OOB_RX_0_WD_PERIOD_1f, 
            HCFC_OOB_RX_0_WD_PERIOD_2f,
            HCFC_OOB_RX_0_WD_PERIOD_3f,
            HCFC_OOB_RX_0_WD_PERIOD_4f}, 
        period_1_fld[] = {
            HCFC_OOB_RX_1_WD_PERIOD_0f, 
            HCFC_OOB_RX_1_WD_PERIOD_1f, 
            HCFC_OOB_RX_1_WD_PERIOD_2f,
            HCFC_OOB_RX_1_WD_PERIOD_3f,
            HCFC_OOB_RX_1_WD_PERIOD_4f};

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_hcfc_watchdog_get_verify(unit, if_ndx, hcfc_watchdog);
    SOCDNX_IF_ERR_EXIT(res);

    SOCDNX_NULL_CHECK(hcfc_watchdog);

    
    res = READ_CFC_HCFC_OOB_RX_WD_EN_CFGr(unit, &reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? HCFC_OOB_RX_0_WD_ENf : HCFC_OOB_RX_1_WD_ENf;
    hcfc_watchdog->enable = soc_reg_field_get(unit, CFC_HCFC_OOB_RX_WD_EN_CFGr, reg32_data, fld);

    
    res = READ_CFC_HCFC_OOB_RX_WD_ERR_STATUSr(unit, &reg32_data);
    SOCDNX_IF_ERR_EXIT(res);

    fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? HCFC_OOB_RX_0_WD_ERR_STATUSf : HCFC_OOB_RX_1_WD_ERR_STATUSf;
    hcfc_watchdog->error_status = soc_reg_field_get(unit, CFC_HCFC_OOB_RX_WD_ERR_STATUSr, reg32_data, fld);

    
    SOC_REG_ABOVE_64_CLEAR(reg64_data);
    reg = (if_ndx == SOC_TMC_FC_OOB_ID_A)? CFC_HCFC_OOB_RX_0_WD_PERIOD_CFGr : CFC_HCFC_OOB_RX_1_WD_PERIOD_CFGr;
    
    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg64_data);
    SOCDNX_IF_ERR_EXIT(res);

    fld = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? period_0_fld[0] : period_1_fld[0];
    hcfc_watchdog->period = soc_reg_above_64_field32_get(unit, reg, reg64_data, fld) / SOC_INFO(unit).frequency;

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(BCM_88650_B0)

uint32
  arad_fc_ilkn_retransmit_cal_set_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    if (ARAD_IS_DIRECTION_REC(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_ENf : ILKN_OOB_RX_1_RT_CAL_ENf,  cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_LENf : ILKN_OOB_RX_1_RT_CAL_LENf,  cal_cfg->length - 1));
    }
    if (ARAD_IS_DIRECTION_GEN(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_ENf : ILKN_OOB_TX_1_RT_CAL_ENf,  cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_LENf : ILKN_OOB_TX_1_RT_CAL_LENf,  cal_cfg->length - 1));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_set_unsafe()", ilkn_ndx, direction_ndx);
}

uint32
  arad_fc_ilkn_retransmit_cal_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    if (ARAD_IS_DIRECTION_REC(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_ENf : ILKN_OOB_RX_1_RT_CAL_ENf, &cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_LENf : ILKN_OOB_RX_1_RT_CAL_LENf, &fld_val));
      cal_cfg->length = fld_val + 1;
    }
    if (ARAD_IS_DIRECTION_GEN(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_ENf : ILKN_OOB_TX_1_RT_CAL_ENf, &cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_LENf : ILKN_OOB_TX_1_RT_CAL_LENf, &fld_val));
      cal_cfg->length = fld_val + 1;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_get_unsafe()", ilkn_ndx, direction_ndx);
}

uint32
  arad_fc_ilkn_retransmit_cal_set_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_SET_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    if(cal_cfg->length < ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MIN || cal_cfg->length > ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MAX)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_set_verify()", ilkn_ndx, direction_ndx);
}

#endif

STATIC int
  arad_fc_port_fifo_threshold_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_if_t              if_type,
    SOC_SAND_IN  SOC_TMC_FC_PORT_FIFO_TH    *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    if (if_type == SOC_PORT_IF_CAUI || if_type == SOC_PORT_IF_ILKN) {  
        
        if (info->llfc.clear > ARAD_FC_RX_MLF_HRF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("llfc.clear is out of range")));
        }

        if (info->llfc.set > ARAD_FC_RX_MLF_HRF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("llfc.set is out of range")));
        }

        
        if (info->pfc.clear > ARAD_FC_RX_MLF_HRF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pfc.clear is out of range")));
        }

        if (info->pfc.set > ARAD_FC_RX_MLF_HRF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pfc.set is out of range")));
        }
    }
    else {
        
        if (info->llfc.clear > ARAD_FC_RX_MLF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("llfc.clear is out of range")));
        }

        if (info->llfc.set > ARAD_FC_RX_MLF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("llfc.set is out of range")));
        }

        
        if (info->pfc.clear > ARAD_FC_RX_MLF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pfc.clear is out of range")));
        }

        if (info->pfc.set > ARAD_FC_RX_MLF_MAX) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("pfc.set is out of range")));
        }
    }  

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_fc_port_fifo_threshold_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  SOC_TMC_FC_PORT_FIFO_TH    *info
  )
{
    int res = SOC_E_NONE;
    uint32 wc_ndx;
    uint32 nif_intern_id;
    soc_port_if_t interface_type;
    uint32 reg_offset;
    uint32 reg_val32, phy_port;
    soc_reg_above_64_val_t field_val, reg_value;  

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    nif_intern_id = phy_port - 1;
    wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);

    res = arad_fc_port_fifo_threshold_verify(unit, interface_type, info);
    SOCDNX_IF_ERR_EXIT(res);

    
    if (interface_type == SOC_PORT_IF_CAUI || interface_type == SOC_PORT_IF_ILKN) {
        reg_offset = ARAD_NIF_CLP_NDX(wc_ndx);

        SOC_REG_ABOVE_64_CLEAR(reg_value);
        res = READ_NBI_HRF_RX_CONTROLLER_CONFIG_1r(unit, reg_offset, reg_value);
        SOCDNX_IF_ERR_EXIT(res);

        SOC_REG_ABOVE_64_CLEAR(field_val);
        SHR_BITCOPY_RANGE(field_val, 0, &(info->llfc.set), 0, ARAD_FC_RX_MLF_HRF_NOF_BITS);
        soc_reg_above_64_field_set(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_LLFC_THRESHOLD_SETf,field_val);

        SOC_REG_ABOVE_64_CLEAR(field_val);
        SHR_BITCOPY_RANGE(field_val, 0, &(info->llfc.clear), 0, ARAD_FC_RX_MLF_HRF_NOF_BITS);
        soc_reg_above_64_field_set(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_LLFC_THRESHOLD_CLRf,field_val);

        SOC_REG_ABOVE_64_CLEAR(field_val);
        SHR_BITCOPY_RANGE(field_val, 0, &(info->pfc.set), 0, ARAD_FC_RX_MLF_HRF_NOF_BITS);
        soc_reg_above_64_field_set(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_PFC_THRESHOLD_SETf,field_val);

        SOC_REG_ABOVE_64_CLEAR(field_val);
        SHR_BITCOPY_RANGE(field_val, 0, &(info->pfc.clear), 0, ARAD_FC_RX_MLF_HRF_NOF_BITS);
        soc_reg_above_64_field_set(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_PFC_THRESHOLD_CLRf,field_val); 

        res = WRITE_NBI_HRF_RX_CONTROLLER_CONFIG_1r(unit, reg_offset, reg_value);
        SOCDNX_IF_ERR_EXIT(res);
    }
    else {
        reg_offset = wc_ndx;

        res = READ_NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr(unit, reg_offset, &reg_val32);
        SOCDNX_IF_ERR_EXIT(res);
        soc_reg_field_set(unit, NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr, &reg_val32, RX_N_LLFC_THRESHOLD_SETf, info->llfc.set);
        soc_reg_field_set(unit, NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr, &reg_val32, RX_N_LLFC_THRESHOLD_CLRf, info->llfc.clear);
        res = WRITE_NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr(unit, reg_offset, reg_val32);
        SOCDNX_IF_ERR_EXIT(res);

        res = READ_NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr(unit, reg_offset, &reg_val32);
        SOCDNX_IF_ERR_EXIT(res);
        soc_reg_field_set(unit, NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr, &reg_val32, RX_N_PFC_THRESHOLD_SETf, info->pfc.set);    
        soc_reg_field_set(unit, NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr, &reg_val32, RX_N_PFC_THRESHOLD_CLRf, info->pfc.clear);
        res = WRITE_NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr(unit, reg_offset, reg_val32);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_fc_port_fifo_threshold_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_OUT SOC_TMC_FC_PORT_FIFO_TH  *info
  )
{
    int res = SOC_E_NONE;
    uint32 wc_ndx;
    uint32 nif_intern_id;
    soc_port_if_t interface_type;
    uint32 reg_offset;  
    uint32 reg_val32, phy_port;
    soc_reg_above_64_val_t field_val, reg_value;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    nif_intern_id = phy_port - 1;
    wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);

    
    if (interface_type == SOC_PORT_IF_CAUI || interface_type == SOC_PORT_IF_ILKN) {
        reg_offset = ARAD_NIF_CLP_NDX(wc_ndx);

        SOC_REG_ABOVE_64_CLEAR(reg_value);
        res = READ_NBI_HRF_RX_CONTROLLER_CONFIG_1r(unit, reg_offset, reg_value);
        SOCDNX_IF_ERR_EXIT(res);
        soc_reg_above_64_field_get(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_LLFC_THRESHOLD_SETf, field_val);
        info->llfc.set = field_val[0];
        soc_reg_above_64_field_get(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_LLFC_THRESHOLD_CLRf, field_val);
        info->llfc.clear = field_val[0];
        soc_reg_above_64_field_get(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_PFC_THRESHOLD_SETf, field_val);
        info->pfc.set = field_val[0];
        soc_reg_above_64_field_get(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, reg_value, HRF_RX_N_PFC_THRESHOLD_CLRf, field_val);
        info->pfc.clear = field_val[0];
    }
    else {
        reg_offset = wc_ndx;

        res = READ_NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr(unit, reg_offset, &reg_val32);
        SOCDNX_IF_ERR_EXIT(res);
        info->llfc.set = soc_reg_field_get(unit, NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr, reg_val32, RX_N_LLFC_THRESHOLD_SETf);
        info->llfc.clear = soc_reg_field_get(unit, NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr, reg_val32, RX_N_LLFC_THRESHOLD_CLRf);

        res = READ_NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr(unit, reg_offset, &reg_val32);
        SOCDNX_IF_ERR_EXIT(res);
        info->pfc.set = soc_reg_field_get(unit, NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr, reg_val32, RX_N_PFC_THRESHOLD_SETf);
        info->pfc.clear = soc_reg_field_get(unit, NBI_RX_MLF_PFC_THRESHOLDS_CONFIGr, reg_val32, RX_N_PFC_THRESHOLD_CLRf);
    }


exit:
    SOCDNX_FUNC_RETURN;
}



uint32
arad_fc_rec_cal_dest_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_REC_CAL_DEST dest_type
  )
{
    uint32 ret;

    for(ret = 0; ret < ARAD_FC_REC_CAL_DEST_ARR_SIZE; ret++)
    {
        if(arad_fc_rec_cal_dest_arr[ret] == dest_type)
        {
            return ret;
        }
    }

    return -1;
}

uint32
arad_fc_gen_cal_src_type_to_val_internal(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CAL_SRC     src_type
  )
{
    uint32 ret;

    for(ret = 0; ret < ARAD_FC_GEN_CAL_SRC_ARR_SIZE; ret++) {
        if(arad_fc_gen_cal_src_arr[ret] == src_type) {
            return ret;
        }
    }

    return -1;
}


soc_error_t
  arad_fc_pfc_mapping_verify(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                       nif_id,
      SOC_SAND_IN uint32                       src_pfc_ndx,
      SOC_SAND_IN SOC_TMC_FC_PFC_MAP           *pfc_map
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (src_pfc_ndx > (SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("src pfc index is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_pfc_mapping_set(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                        nif_id,
      SOC_SAND_IN uint32                        src_pfc_ndx,
      SOC_SAND_IN SOC_TMC_FC_PFC_MAP            *pfc_map
    )
{
    soc_error_t res = SOC_E_NONE;
    soc_reg_above_64_val_t nif_pfc_data;
    uint32 entry_offset, field_offset;
    uint32 index_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f};
    uint32 select_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f};
    uint32 valid_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
    uint32 select_data = 0, valid = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_pfc_mapping_verify(unit, nif_id, src_pfc_ndx, pfc_map);
    SOCDNX_IF_ERR_EXIT(res);

    entry_offset = ((nif_id * SOC_TMC_EGR_NOF_Q_PRIO_ARAD) / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) + 
        src_pfc_ndx / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
    field_offset = src_pfc_ndx % FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;

    if (pfc_map->mode == SOC_TMC_FC_PFC_MAP_EGQ) {
        select_data = 0;
    }
    else {
        select_data = 1;
    }
    valid = pfc_map->valid;

    SOC_REG_ABOVE_64_CLEAR(nif_pfc_data);
    res = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, index_field[field_offset], pfc_map->index);
    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, select_field[field_offset], select_data);
    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, valid_field[field_offset], valid);

    res = soc_mem_write(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_pfc_mapping_get(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                        nif_id,
      SOC_SAND_IN uint32                        src_pfc_ndx,
      SOC_SAND_OUT SOC_TMC_FC_PFC_MAP           *pfc_map
    )
{
    soc_error_t res = SOC_E_NONE;
    soc_reg_above_64_val_t nif_pfc_data;
    uint32 entry_offset, field_offset;
    uint32 index_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f};
    uint32 select_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f};
    uint32 valid_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
    uint32 select_data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_pfc_mapping_verify(unit, nif_id, src_pfc_ndx, pfc_map);
    SOCDNX_IF_ERR_EXIT(res);

    entry_offset = ((nif_id * SOC_TMC_EGR_NOF_Q_PRIO_ARAD) / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) + 
        src_pfc_ndx / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
    field_offset = src_pfc_ndx % FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;

    SOC_REG_ABOVE_64_CLEAR(nif_pfc_data);
    res = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);

    pfc_map->index = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, index_field[field_offset]);
    select_data = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, select_field[field_offset]);
    pfc_map->valid = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, valid_field[field_offset]);

    if (select_data == 0) {
        pfc_map->mode = SOC_TMC_FC_PFC_MAP_EGQ;
    }
    else {
        pfc_map->mode = SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP;
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_inbnd_mode_verify(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN ARAD_INTERFACE_ID             nif_ndx,
      SOC_SAND_IN int                           is_generate
  )
{
    uint32 res = SOC_SAND_OK; 
    
    SOCDNX_INIT_FUNC_DEFS;

    res = arad_nif_id_verify(
            nif_ndx
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("API is not applicable for ilkn")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_inbnd_mode_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN int                           is_generate,
      SOC_SAND_IN SOC_TMC_FC_INBND_MODE         mode
      )
{
    soc_error_t 
        rv = SOC_E_NONE;    
    uint32 
        fld_val;
    int
        pfc_en,
        llfc_en,
        pause_tx,
        pause_rx;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_pm_pause_get(unit, port, &pause_tx, &pause_rx));
    if(is_generate) {
        pause_tx = (mode == SOC_TMC_FC_INBND_MODE_LL ? 1 : 0);
    } else {
        pause_rx = (mode == SOC_TMC_FC_INBND_MODE_LL ? 1 : 0);
    }
    SOCDNX_IF_ERR_EXIT(soc_pm_pause_set(unit, port, pause_tx, pause_rx));

    
    pfc_en = (mode == SOC_TMC_FC_INBND_MODE_PFC ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(soc_pm_pfc_set(unit, port, is_generate ? 0 : 1, pfc_en));

    
    llfc_en = (mode == SOC_TMC_FC_INBND_MODE_SAFC ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(soc_pm_llfc_set(unit, port, is_generate ? 0 : 1, llfc_en));

    
    fld_val = SOC_SAND_BOOL2NUM(mode == SOC_TMC_FC_INBND_MODE_LL);
    rv = soc_reg_field32_modify(unit, PORT_CONFIGr, port, LLFC_ENf, fld_val);
    SOCDNX_IF_ERR_EXIT(rv);

    fld_val = SOC_SAND_BOOL2NUM(mode != SOC_TMC_FC_INBND_MODE_LL);
    rv = soc_reg_field32_modify(unit, PORT_CONFIGr, port, PFC_ENABLEf, fld_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_inbnd_mode_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN int                           is_generate,
      SOC_SAND_OUT SOC_TMC_FC_INBND_MODE        *mode
    )
{
    soc_error_t 
        rv = SOC_E_NONE;    
    int 
        ll_enable = 0, 
        pfc_enable = 0, 
        safc_enable = 0;
    int
        pause_tx,
        pause_rx;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = soc_pm_pause_get(unit, port, &pause_tx, &pause_rx);
    SOCDNX_IF_ERR_EXIT(rv);
    ll_enable = (is_generate ? pause_tx : pause_rx);

    
    rv = soc_pm_pfc_get(unit, port, (is_generate ? 0 :1), &pfc_enable);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_pm_llfc_get(unit, port, (is_generate ? 0 :1), &safc_enable);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (ll_enable)
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_LL;
    }
    else if(pfc_enable)
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_PFC;
    }
    else if(safc_enable)
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_SAFC;
    }
    else
    {
        (*mode) = SOC_TMC_FC_INBND_MODE_DISABLED;
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_vsq_index_group2global(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP vsq_group,
    SOC_SAND_IN  uint32                vsq_in_group_ndx,
    SOC_SAND_IN  int                   cosq,
    SOC_SAND_IN  uint8                 is_ocb_only,
    SOC_SAND_IN  uint32                src_port,
    SOC_SAND_OUT uint32                *vsq_fc_ndx
  )
{
    SOC_TMC_ITM_VSQ_NDX
        vsq_ndx_tmp = 0;

    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(cosq);
	COMPILER_REFERENCE(is_ocb_only);
	COMPILER_REFERENCE(src_port);

    SOCDNX_NULL_CHECK(vsq_fc_ndx);

    switch(vsq_group)
    {
    case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
        vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS_SIZE;
    case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
        vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS_SIZE;
        
        
    case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
        vsq_ndx_tmp += ARAD_ITM_VSQ_GROUP_CTGRY_SIZE;
    case ARAD_ITM_VSQ_GROUP_LLFC:
    case ARAD_ITM_VSQ_GROUP_PFC:
    case ARAD_ITM_VSQ_GROUP_CTGRY:
        vsq_ndx_tmp += vsq_in_group_ndx;
    default:
        break;
    }
    *vsq_fc_ndx = vsq_ndx_tmp;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_cmic_rx_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_IN uint32                        priority_bmp,
      SOC_SAND_IN int                           is_ena
  )
{
    soc_error_t 
        rv = SOC_E_NONE;    
    uint32 
        tm_port,
        base_queue_pair,
        nof_priorities, 
        val32_arr[2] = {0};
    int
        core, 
        cosq = 0, 
        i = 0; 
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        priority_bmp_lcl[1];

    SOCDNX_INIT_FUNC_DEFS;

    
    if(!IS_CPU_PORT(unit, port)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("port is not CPU port!\n")));
    }

    
    rv = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_queue_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, CFC_CMIC_RX_FC_CFGr, REG_PORT_ANY, 0, &reg_val64));
    val32_arr[1] = COMPILER_64_HI(reg_val64);
    val32_arr[0] = COMPILER_64_LO(reg_val64);

    *priority_bmp_lcl = priority_bmp;
    for (i = 0; i < nof_priorities; i++) 
    {
        if (SHR_BITGET(priority_bmp_lcl, i)) 
        {
            
            cosq = base_queue_pair - ARAD_PS_CPU_FIRST_VALID_QPAIR + i;
            if (cosq >= 32) {
                (is_ena) ? SHR_BITSET(&val32_arr[1], cosq - 32) : SHR_BITCLR(&val32_arr[1], cosq - 32);
            }
            else {
                (is_ena) ? SHR_BITSET(&val32_arr[0], cosq) : SHR_BITCLR(&val32_arr[0], cosq);
            }
        }
    }

    
    COMPILER_64_SET(reg_val64, val32_arr[1], val32_arr[0]);        
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, CFC_CMIC_RX_FC_CFGr, REG_PORT_ANY, 0, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_cmic_rx_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN soc_port_t                    port,
      SOC_SAND_OUT uint32                       *priority_bmp,
      SOC_SAND_OUT int                          *is_ena
  )
{
    soc_error_t 
        rv = SOC_E_NONE;    
    uint32 
        tm_port,
        base_queue_pair,
        nof_priorities, 
        val32_arr[2] = {0};
    int
        core, 
        cosq = 0, 
        i = 0, 
        cosq_ena = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(priority_bmp);
    SOCDNX_NULL_CHECK(is_ena);

    
    if(!IS_CPU_PORT(unit, port)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("port is not CPU port!\n")));
    }

    *priority_bmp = 0;
    *is_ena = 0;

    
    rv = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_queue_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, CFC_CMIC_RX_FC_CFGr, REG_PORT_ANY, 0, &reg_val64));
    val32_arr[1] = COMPILER_64_HI(reg_val64);
    val32_arr[0] = COMPILER_64_LO(reg_val64);

    for (i = 0; i < nof_priorities; i++) 
    {
        cosq = base_queue_pair - ARAD_PS_CPU_FIRST_VALID_QPAIR + i;
        if (cosq >= 32) {
            cosq_ena = SHR_BITGET(&val32_arr[1], cosq - 32);
        }
        else {
            cosq_ena = SHR_BITGET(&val32_arr[0], cosq);
        }

        if (cosq_ena != 0) {
            SHR_BITSET(priority_bmp, i);
            *is_ena = 1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_parse_tx_cal_tbl(
    int                      unit,
    uint32                   intf_type,
    uint32                   is_oob,
    SOC_TMC_FC_DIRECTION     direction,
    int                      port, 
    int                      calendar_id,
    uint32                   *cal_tx_src_type,
    SOC_TMC_ITM_VSQ_GROUP    *vsq_group,
    uint32                   *fc_index
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    SOC_TMC_FC_CAL_MODE
        cal_mode;
    ARAD_FC_CAL_TYPE
        cal_type;
    ARAD_CFC_CALTX_TBL_DATA
        tx_cal;
    uint8
        is_ocb_only;
    SOC_TMC_FC_GEN_CAL_SRC
        fc_src;

    SOCDNX_INIT_FUNC_DEFS;

    if (intf_type == SOC_TMC_FC_INTF_TYPE_ILKN) {
        cal_mode = is_oob ? SOC_TMC_FC_CAL_MODE_ILKN_OOB : SOC_TMC_FC_CAL_MODE_ILKN_INBND;
    }
    else {
        cal_mode = SOC_TMC_FC_CAL_MODE_SPI_OOB;
    }
    cal_type = ((direction == SOC_TMC_FC_DIRECTION_GEN) ? ARAD_FC_CAL_TYPE_TX : ARAD_FC_CAL_TYPE_RX);
    SOCDNX_IF_ERR_EXIT(arad_fc_cal_tbl_get(unit, cal_mode, cal_type, port, &cal_table));

    rv = soc_mem_read(unit, cal_table, MEM_BLOCK_ANY, calendar_id, cal_table_data);
    SOCDNX_IF_ERR_EXIT(rv);
    tx_cal.fc_index = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_INDEXf);
    tx_cal.fc_source_sel = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_SRC_SELf);

    fc_src = arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel];
    *fc_index = tx_cal.fc_index;
 
    if (fc_src == SOC_TMC_FC_GEN_CAL_SRC_STE) {
        
        if (SOC_SAND_FAILURE(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_vsq_index_global2group,(unit, tx_cal.fc_index, vsq_group, fc_index, &is_ocb_only)))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unit %d, error in vsq id retrieve %d"), unit, tx_cal.fc_index));
        }
        *cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_VSQ;
    }
    else if (fc_src == SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ) {
        *vsq_group = SOC_TMC_ITM_VSQ_GROUP_LLFC;    
        *cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_VSQ;
    }
    else if (fc_src == SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ) {
        *vsq_group = SOC_TMC_ITM_VSQ_GROUP_PFC;     
        *cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_VSQ;
    }
    else if (fc_src == SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS) {
        *cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_GLB; 
    }
    else if (fc_src == SOC_TMC_FC_GEN_CAL_SRC_LLFC) {
        *cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_NIF; 
    }
    else if (fc_src == SOC_TMC_FC_GEN_CAL_SRC_HCFC) {
        *cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_HCFC; 
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support fc_src(%d) fc_source_sel(%d)\n"), fc_src, tx_cal.fc_source_sel));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_parse_rx_cal_tbl(
    int                      unit,
    uint32                   intf_type,
    uint32                   is_oob,
    SOC_TMC_FC_DIRECTION     direction,
    int                      port, 
    int                      calendar_id,
    uint32                   *cal_rx_dst_type,
    uint32                   *fc_index
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
        cal_table = INVALIDm;
    soc_reg_above_64_val_t
        cal_table_data;
    SOC_TMC_FC_CAL_MODE
        cal_mode;
    ARAD_FC_CAL_TYPE
        cal_type;
    ARAD_CFC_CALRX_TBL_DATA
        rx_cal;
    uint32
        fc_dst_sel_2_dst_type[] = {    
           SOC_TMC_FC_REC_CAL_DEST_PFC,
           SOC_TMC_FC_REC_CAL_DEST_NIF_LL,
           SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY,
           SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY,
           SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC,
           SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT,
           SOC_TMC_FC_REC_CAL_DEST_NONE
           };

    SOCDNX_INIT_FUNC_DEFS;

    if (intf_type == SOC_TMC_FC_INTF_TYPE_ILKN) {
        cal_mode = is_oob ? SOC_TMC_FC_CAL_MODE_ILKN_OOB : SOC_TMC_FC_CAL_MODE_ILKN_INBND;
    }
    else {
        cal_mode = SOC_TMC_FC_CAL_MODE_SPI_OOB;
    }
    cal_type = ((direction == SOC_TMC_FC_DIRECTION_GEN) ? ARAD_FC_CAL_TYPE_TX : ARAD_FC_CAL_TYPE_RX);
    SOCDNX_IF_ERR_EXIT(arad_fc_cal_tbl_get(unit, cal_mode, cal_type, port, &cal_table));

    rv = soc_mem_read(unit, cal_table, MEM_BLOCK_ANY, calendar_id, cal_table_data);
    SOCDNX_IF_ERR_EXIT(rv);
    rx_cal.fc_index = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_INDEXf);
    rx_cal.fc_dest_sel = soc_mem_field32_get(unit, cal_table, cal_table_data, FC_DST_SELf);

    *cal_rx_dst_type = fc_dst_sel_2_dst_type[rx_cal.fc_dest_sel];
    *fc_index = rx_cal.fc_index;

exit:
    SOCDNX_FUNC_RETURN;
}


static soc_error_t
  _arad_fc_trigger_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr; 
    uint32
        reg_val32 = 0, 
        fld_val32 = 0, 
        val32_arr[2] = {0};
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        first_phy;
    uint32
        start_bit = 0;
    int
        res_type = 0,
        vsq_type = 0,
        src_id = 0;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_key);
    SOCDNX_NULL_CHECK(fc_status_info);

    if (fc_status_key->intf_type == SOC_TMC_FC_INTF_TYPE_NIF) {
        res_type = fc_status_key->src_type;
        vsq_type = fc_status_key->vsq_type;
        src_id = fc_status_key->vsq_id;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, fc_status_key->port, &first_phy ));
        first_phy--;

        if (fc_status_key->src_type == SOC_TMC_FC_SRC_TYPE_NIF){
            src_id = first_phy;
        }
        else if (fc_status_key->src_type == SOC_TMC_FC_SRC_TYPE_VSQ){
            if (fc_status_key->vsq_type == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
                src_id = first_phy;
                fc_status_info->nif_tx_src_id = src_id;
            }
            else if (fc_status_key->vsq_type == SOC_TMC_ITM_VSQ_GROUP_PG) {
                src_id = first_phy*8;
                fc_status_info->nif_tx_src_id = src_id;
            }
        }
    }
    else {
        res_type = fc_status_info->cal_tx_src_type;
        vsq_type = fc_status_info->cal_tx_src_vsq_type;
        src_id = fc_status_info->cal_tx_src_id;
    }

    switch(res_type) {
    case SOC_TMC_FC_SRC_TYPE_GLB:  
        
        reg = IQM_GLOBAL_FLOW_CONTROL_STATEr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, 0, 0, &reg_val32)); 

        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, GLB_FC_FRDB_UCf);
        fc_status_info->iqm_glb_mnmc_db_hi_fc_state[0] = SHR_BITGET(&fld_val32, 1) ? 1 : 0;
        fc_status_info->iqm_glb_mnmc_db_lo_fc_state[0] = SHR_BITGET(&fld_val32, 0) ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, GLB_FC_FRDB_FLMCf);
        fc_status_info->iqm_glb_flmc_db_hi_fc_state[0] = SHR_BITGET(&fld_val32, 1) ? 1 : 0;
        fc_status_info->iqm_glb_flmc_db_lo_fc_state[0] = SHR_BITGET(&fld_val32, 0) ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, GLB_FC_BDBf);
        fc_status_info->iqm_glb_bdb_hi_fc_state[0] = SHR_BITGET(&fld_val32, 1) ? 1 : 0;
        fc_status_info->iqm_glb_bdb_lo_fc_state[0] = SHR_BITGET(&fld_val32, 0) ? 1 : 0;

        
        reg = CFC_IQM_GLBL_FC_STATUSr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, 0, 0, &reg_val32)); 

        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FR_UC_DB_FC_HPf);
        fc_status_info->cfc_iqm_glb_mnmc_db_hi_fc_state[0] = fld_val32 ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FR_UC_DB_FC_LPf);
        fc_status_info->cfc_iqm_glb_mnmc_db_lo_fc_state[0] = fld_val32 ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FR_FL_MC_DB_FC_HPf);
        fc_status_info->cfc_iqm_glb_flmc_db_hi_fc_state[0] = fld_val32 ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FR_FL_MC_DB_FC_LPf);
        fc_status_info->cfc_iqm_glb_flmc_db_lo_fc_state[0] = fld_val32 ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FR_BDB_FC_HPf);
        fc_status_info->cfc_iqm_glb_bdb_hi_fc_state[0] = fld_val32 ? 1 : 0;
        fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FR_BDB_FC_LPf);
        fc_status_info->cfc_iqm_glb_bdb_lo_fc_state[0] = fld_val32 ? 1 : 0;

        break;
    case SOC_TMC_FC_SRC_TYPE_VSQ:  
        
        if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_CTGRY || vsq_type == SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS) {
            fld_val32 = 0;
        }
        else if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS) {
            fld_val32 = 1;
        }
        else if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG) {
            fld_val32 = src_id/64;
        }
        reg = CFC_IQM_VSQ_FC_STATUS_SELr;
        soc_reg_field_set(unit, reg, &reg_val32, IQM_VSQ_GRPS_ABCD_FC_STATUS_SELf, fld_val32);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

        reg = CFC_IQM_VSQ_GRPS_ABCD_FC_STATUSr;
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        val32_arr[1] = COMPILER_64_HI(reg_val64);
        val32_arr[0] = COMPILER_64_LO(reg_val64);
        if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
            fc_status_info->cfc_iqm_vsq_fc_state[0] = SHR_BITGET(val32_arr, src_id) ? 1 : 0;
        }
        else if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS) {
            fc_status_info->cfc_iqm_vsq_fc_state[0] = SHR_BITGET(val32_arr, (src_id+4)) ? 1 : 0;
        }
        else {
            fc_status_info->cfc_iqm_vsq_fc_state[0] = SHR_BITGET(val32_arr, src_id%64) ? 1 : 0;
        }

        if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_LLFC) {
            reg = CFC_IQM_VSQ_LLFC_STATUSr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, 0, 0, &reg_val32)); 
            fc_status_info->cfc_iqm_vsq_fc_state[0] = SHR_BITGET(&reg_val32, src_id) ? 1 : 0;
        }
        else if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_PFC) {
            reg = CFC_IQM_VSQ_FC_STATUS_SELr;
            fld_val32 = src_id/64;
            soc_reg_field_set(unit, reg, &reg_val32, IQM_VSQ_PFC_STATUS_SELf, fld_val32);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

            reg = CFC_IQM_VSQ_PFC_STATUSr;
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
            val32_arr[1] = COMPILER_64_HI(reg_val64);
            val32_arr[0] = COMPILER_64_LO(reg_val64);
            start_bit = src_id%64;
            SHR_BITCOPY_RANGE(&(fc_status_info->cfc_iqm_vsq_fc_state[0]), 0, val32_arr, start_bit, 8);
        }

        break;
    case SOC_TMC_FC_SRC_TYPE_NIF:  
        reg = CFC_NIF_AF_FC_STATUSr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, 0, 0, &reg_val32)); 
        fc_status_info->cfc_nif_af_fc_status = SHR_BITGET(&reg_val32, src_id) ? 1 : 0;

        if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_LLFC) {
            reg = NBI_FC_RX_GEN_LLFC_FROM_MLFr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, 0, 0, &reg_val32)); 
            fc_status_info->nbi_llfc_status_from_mlf = SHR_BITGET(&reg_val32, src_id) ? 1 : 0;
        }
        else {
            reg = NBI_FC_RX_GEN_PFC_FROM_MLFr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, 0, 0, &reg_val32)); 
            fc_status_info->nbi_pfc_status_from_mlf = SHR_BITGET(&reg_val32, src_id) ? 1 : 0;
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_llfc_stop_tx_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   first_phy,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO  *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_info);

    
    reg = NBI_FC_TX_LLFC_STOP_TX_FROM_CFCr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_TX_LLFC_STOP_TX_FROM_CFCf);
    fc_status_info->nbi_llfc_stop_tx_from_cfc = SHR_BITGET(fld_val32, first_phy) ? 1 : 0;

    
    reg = NBI_FC_TX_LLFC_STOP_TX_TO_XMALr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_TX_LLFC_STOP_TX_TO_XMALf);
    fc_status_info->nbi_llfc_stop_tx_to_mac = SHR_BITGET(fld_val32, first_phy) ? 1 : 0;

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_queue_pair_to_egr_if_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   queue_pair,
    SOC_SAND_OUT uint32                  *egr_if
  )
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32     base_q_pair_i;
    uint32     tm_port;
    int        core;
    int        is_found = 0, nof_priorities = 0;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair_i);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.priority_mode.get(unit, port_i, &nof_priorities);
        SOCDNX_IF_ERR_EXIT(rv);

        if ((queue_pair >= base_q_pair_i) && (queue_pair < (base_q_pair_i + nof_priorities))) {
            is_found = 1;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));

            SOCDNX_SAND_IF_ERR_EXIT(arad_port2egress_offset(unit, core, tm_port, egr_if));

            break;
        }
    }

    if (!is_found) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Egress interface is not found for queue_pair(%d)\n"), queue_pair));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_egq_pfc_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN int                      is_full,
    SOC_SAND_IN uint32                   base_queue_pair,
    SOC_SAND_IN int                      nof_priorities,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO  *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32 = 0, 
        reg_val32_hi = 0,
        reg_val32_lo = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0), 
        fld_val64 = COMPILER_64_INIT(0, 0);
    soc_reg_above_64_val_t
        reg_val_abv64;
    int 
        i = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_info);

    if (is_full) {
        
        for (i = 0; i < 256; i+=64) {
            reg = CFC_EGQ_STATUS_SELr;
            fld_val32 = i/64;
            soc_reg_field_set(unit, reg, &reg_val32, EGQ_PFC_STATUS_SELf, fld_val32);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

            reg = CFC_EGQ_PFC_STATUSr;
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
            fld_val64 = soc_reg64_field_get(unit, reg, reg_val64, EGQ_PFC_STATUSf);
            reg_val32_hi = COMPILER_64_HI(fld_val64);
            reg_val32_lo = COMPILER_64_LO(fld_val64);
            SHR_BITCOPY_RANGE(fc_status_info->cfc_egq_pfc_status_full[0], i + 32, &reg_val32_hi, 0, 32);
            SHR_BITCOPY_RANGE(fc_status_info->cfc_egq_pfc_status_full[0], i, &reg_val32_lo, 0, 32);
        }

        
        reg = EGQ_CFC_FLOW_CONTROLr;
        SOC_REG_ABOVE_64_CLEAR(reg_val_abv64);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, 0, 0, reg_val_abv64));
        SHR_BITCOPY_RANGE(fc_status_info->egq_cfc_fc_status_full[0], 0, reg_val_abv64, 0, 256);
    }
    else {
        
        reg = CFC_EGQ_STATUS_SELr;
        fld_val32 = base_queue_pair/64;
        soc_reg_field_set(unit, reg, &reg_val32, EGQ_PFC_STATUS_SELf, fld_val32);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

        reg = CFC_EGQ_PFC_STATUSr;
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        fld_val64 = soc_reg64_field_get(unit, reg, reg_val64, EGQ_PFC_STATUSf);
        reg_val32_hi = COMPILER_64_HI(fld_val64);
        reg_val32_lo = COMPILER_64_LO(fld_val64);
        if (base_queue_pair%64 >= 32){
            SHR_BITCOPY_RANGE(&fld_val32, 0, &reg_val32_hi, base_queue_pair%32, nof_priorities);
        }
        else {
            SHR_BITCOPY_RANGE(&fld_val32, 0, &reg_val32_lo, base_queue_pair%32, nof_priorities);
        }
        fc_status_info->cfc_egq_pfc_status[0] = fld_val32;

        
        reg = EGQ_CFC_FLOW_CONTROLr;
        SOC_REG_ABOVE_64_CLEAR(reg_val_abv64);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, 0, 0, reg_val_abv64));
        SHR_BITCOPY_RANGE(&fld_val32, 0, reg_val_abv64, base_queue_pair, nof_priorities);
        fc_status_info->egq_cfc_fc_status[0] = fld_val32;

        
        for (i = 0; i < 8; i++) {
            reg = SCH_DVS_FC_COUNTERS_CONFIGURATION_REGISTERr;
            fld_val32 = base_queue_pair + i;
            soc_reg_field_set(unit, reg, &reg_val32, FC_CNT_PORTf, fld_val32);
            soc_reg_field_set(unit, reg, &reg_val32, CNT_PORT_FCf, 1);
            soc_reg_field_set(unit, reg, &reg_val32, CNT_INTERFACE_FCf, 0);
            soc_reg_field_set(unit, reg, &reg_val32, CNT_DEVICE_FCf, 0);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

            reg = SCH_DVS_FLOW_CONTROL_COUNTERr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            fc_status_info->sch_fc_port_cnt[i] = reg_val32;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_egq_inf_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   egr_if,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO  *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0}, 
        entry_val32;
    uint32
        sch_if = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_info);

    
    reg = CFC_EGQ_IF_FC_STATUSr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, EGQ_IF_FC_STATUSf);
    fc_status_info->cfc_egq_inf_fc = SHR_BITGET(fld_val32, egr_if) ? 1 : 0; 

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, 0), egr_if, &entry_val32));
    sch_if = soc_SCH_FC_MAP_FCMm_field32_get(unit,&entry_val32,FC_MAP_FCMf);

    reg = SCH_DVS_FC_COUNTERS_CONFIGURATION_REGISTERr;
    *fld_val32 = sch_if;
    soc_reg_field_set(unit, reg, &reg_val32, FC_INTERFACEf, *fld_val32);
    soc_reg_field_set(unit, reg, &reg_val32, CNT_PORT_FCf, 0);
    soc_reg_field_set(unit, reg, &reg_val32, CNT_INTERFACE_FCf, 1);
    soc_reg_field_set(unit, reg, &reg_val32, CNT_DEVICE_FCf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

    reg = SCH_DVS_FLOW_CONTROL_COUNTERr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    fc_status_info->sch_fc_inf_cnt = reg_val32;

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_egq_dev_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO  *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_info);

    
    reg = CFC_EGQ_FC_STATUSr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    fld_val32 = soc_reg_field_get(unit, reg, reg_val32, EGQ_DEV_FCf);
    fc_status_info->cfc_egq_dev_fc = fld_val32; 

    
    reg = SCH_DVS_FC_COUNTERS_CONFIGURATION_REGISTERr;
    soc_reg_field_set(unit, reg, &reg_val32, CNT_PORT_FCf, 0);
    soc_reg_field_set(unit, reg, &reg_val32, CNT_INTERFACE_FCf, 0);
    soc_reg_field_set(unit, reg, &reg_val32, CNT_DEVICE_FCf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

    reg = SCH_DVS_FLOW_CONTROL_COUNTERr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    fc_status_info->sch_fc_dev_cnt = reg_val32;

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_nif_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        first_phy = 0;
    soc_reg_t
        reg = INVALIDr,
        reg_tx_pfc_arr[] = {TPFC_0r, TPFC_1r, TPFC_2r, TPFC_3r, TPFC_4r, TPFC_5r, TPFC_6r, TPFC_7r},
        reg_rx_pfc_arr[] = {RPFC_0r, RPFC_1r, RPFC_2r, RPFC_3r, RPFC_4r, RPFC_5r, RPFC_6r, RPFC_7r};
    uint32
        reg_val32 = 0,
        fld_val32[1] = {0},
        fld_val32_arr[2] = {0};
    uint64
        reg_val64,
        fld_val64;
    uint32
        port = 0,
        base_q_pair,
        egr_if;
    int 
        nof_priorities,
        i = 0,
        is_generic_map_used = 0;
    SOC_TMC_FC_PFC_MAP
        pfc_map;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_key);
    SOCDNX_NULL_CHECK(fc_status_info);

    port = fc_status_key->port;
    rv = soc_port_sw_db_first_phy_port_get(unit, port, &first_phy );
    SOCDNX_IF_ERR_EXIT(rv);
    first_phy = (first_phy > 0) ? (first_phy - 1) : first_phy;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.priority_mode.get(unit, port, &nof_priorities);
    SOCDNX_IF_ERR_EXIT(rv);

    if (fc_status_key->direction == SOC_TMC_FC_DIRECTION_GEN) {
        
        SOCDNX_IF_ERR_EXIT(_arad_fc_trigger_status_info_get(unit, fc_status_key, fc_status_info));

        
        switch(fc_status_key->nif_fc_type) 
        {
        case SOC_TMC_FC_NIF_TYPE_LLFC:
            
            reg = NBI_FC_TX_GEN_LLFC_FROM_CFCr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_TX_GEN_LLFC_FROM_CFCf);
            fc_status_info->nbi_tx_llfc_from_cfc = SHR_BITGET(fld_val32, first_phy) ? 1 : 0;

            reg = NBI_FC_TX_GEN_LLFC_TO_XMALr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_TX_GEN_LLFC_TO_XMALf);
            fc_status_info->nbi_tx_llfc_to_mac = SHR_BITGET(fld_val32, first_phy) ? 1 : 0;  

            
            reg = TXPFr;
            SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
            COMPILER_64_ZERO(fc_status_info->mib_tx_pause_cnt);
            COMPILER_64_ADD_64(fc_status_info->mib_tx_pause_cnt, reg_val64);

            break;
        case SOC_TMC_FC_NIF_TYPE_PFC:
        case SOC_TMC_FC_NIF_TYPE_SAFC:
            
            reg = NBI_FC_CHOOSE_PORT_FOR_PFCr;
            *fld_val32 = first_phy;
            soc_reg_field_set(unit, reg, &reg_val32, FC_CHOOSE_PORT_FOR_PFCf, *fld_val32);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

            reg = NBI_FC_PFC_DEBUG_INDICATIONSr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_TX_GEN_PFC_FROM_CFCf);
            fc_status_info->nbi_tx_pfc_from_cfc = *fld_val32; 

            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_TX_GEN_PFC_TO_XMALf);
            fc_status_info->nbi_tx_pfc_to_mac = *fld_val32; 

            
            if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC) {
                reg = TXPPr;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                COMPILER_64_ZERO(fc_status_info->mib_tx_pfc_cnt);
                COMPILER_64_ADD_64(fc_status_info->mib_tx_pfc_cnt, reg_val64);

                for (i = 0; i < 8; i++) {
                    reg = reg_tx_pfc_arr[i];
                    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                    COMPILER_64_ZERO(fc_status_info->mib_tx_pfc_x_cnt[i]);
                    COMPILER_64_ADD_64(fc_status_info->mib_tx_pfc_x_cnt[i], reg_val64);
                }
            }
            else {
                reg = TX_LLFC_LOG_COUNTERr;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                COMPILER_64_ZERO(fc_status_info->mib_tx_safc_log_cnt);
                COMPILER_64_ADD_64(fc_status_info->mib_tx_safc_log_cnt, reg_val64);
            }
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support nif_fc_type(%d)\n"), fc_status_key->nif_fc_type));
            break;
        }
    }
    else {
        switch(fc_status_key->nif_fc_type) 
        {
        case SOC_TMC_FC_NIF_TYPE_LLFC:
            
            reg = RXPFr;
            SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
            COMPILER_64_ZERO(fc_status_info->mib_rx_pause_cnt);
            COMPILER_64_ADD_64(fc_status_info->mib_rx_pause_cnt, reg_val64);

            
            reg =  EGQ_NIF_FLOW_CONTROLr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            fc_status_info->egq_nif_fc_status[0] = SHR_BITGET(&reg_val32, first_phy) ? 1 : 0;
            break;
        case SOC_TMC_FC_NIF_TYPE_PFC:
        case SOC_TMC_FC_NIF_TYPE_SAFC:
            
            if (fc_status_key->nif_fc_type == SOC_TMC_FC_NIF_TYPE_PFC) {
                reg = RXPPr;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                COMPILER_64_ZERO(fc_status_info->mib_rx_pfc_cnt);
                COMPILER_64_ADD_64(fc_status_info->mib_rx_pfc_cnt, reg_val64);

                for (i = 0; i < 8; i++) {
                    reg = reg_rx_pfc_arr[i];
                    SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                    COMPILER_64_ZERO(fc_status_info->mib_rx_pfc_x_cnt[i]);
                    COMPILER_64_ADD_64(fc_status_info->mib_rx_pfc_x_cnt[i], reg_val64);
                }
            }
            else {
                reg = RX_LLFC_LOG_COUNTERr;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                COMPILER_64_ZERO(fc_status_info->mib_rx_safc_log_cnt);
                COMPILER_64_ADD_64(fc_status_info->mib_rx_safc_log_cnt, reg_val64);

                reg = RX_LLFC_PHY_COUNTERr;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                COMPILER_64_ZERO(fc_status_info->mib_rx_safc_phy_cnt);
                COMPILER_64_ADD_64(fc_status_info->mib_rx_safc_phy_cnt, reg_val64);

                reg = RX_LLFC_CRC_COUNTERr;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg, port, 0, &reg_val64));
                COMPILER_64_ZERO(fc_status_info->mib_rx_safc_crc_cnt);
                COMPILER_64_ADD_64(fc_status_info->mib_rx_safc_crc_cnt, reg_val64);
            }

            
            reg = NBI_FC_CHOOSE_PORT_FOR_PFCr;
            *fld_val32 = first_phy;
            soc_reg_field_set(unit, reg, &reg_val32, FC_CHOOSE_PORT_FOR_PFCf, *fld_val32);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

            reg = NBI_FC_PFC_DEBUG_INDICATIONSr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_RX_PFC_FROM_XMALf);
            fc_status_info->nbi_rx_pfc_from_mac = *fld_val32; 

            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FC_RX_PFC_TO_CFCf);
            fc_status_info->nbi_rx_pfc_to_cfc = *fld_val32; 

            
            reg = CFC_NIF_PFC_STATUS_SELr;
            *fld_val32 = (first_phy*8)/64;
            soc_reg_field_set(unit, reg, &reg_val32, NIF_PFC_STATUS_SELf, *fld_val32);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32)); 

            reg = CFC_NIF_PFC_STATUSr;
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
            fld_val64 = soc_reg64_field_get(unit, reg, reg_val64, NIF_PFC_STATUSf);
            fld_val32_arr[1] = COMPILER_64_HI(fld_val64);
            fld_val32_arr[0] = COMPILER_64_LO(fld_val64); 
            SHR_BITCOPY_RANGE(fld_val32, 0, fld_val32_arr, (first_phy*8)%64, 8);
            fc_status_info->cfc_nif_pfc_status = *fld_val32;

            
            for (i = 0; i < 8; i++) {
                SOCDNX_IF_ERR_EXIT(arad_fc_pfc_mapping_get(unit, first_phy, i, &pfc_map));

                if (pfc_map.valid == 0) {
                    fc_status_info->nif_rx_dst_type[i] = SOC_TMC_FC_NOF_PFC_MAP_MODE;
                    continue;
                }
                fc_status_info->nif_rx_dst_type[i] = pfc_map.mode;
                fc_status_info->nif_rx_dst_id[i] = pfc_map.index;
                if (pfc_map.mode == SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP) {
                    is_generic_map_used = 1;
                }
            }

            if (is_generic_map_used) {
                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, TRUE, 0, 0, fc_status_info));
            }
            else {
                SOCDNX_IF_ERR_EXIT(_arad_fc_queue_pair_to_egr_if_get(unit, base_q_pair, &egr_if));

                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, FALSE, base_q_pair, nof_priorities, fc_status_info));
                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_inf_status_info_get(unit, egr_if, fc_status_info));
                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_dev_status_info_get(unit, fc_status_info));
            }
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support nif_fc_type(%d)\n"), fc_status_key->nif_fc_type));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_ilkn_inb_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0,
	reg_val32_hi[1] = {0},
	reg_val32_lo = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    soc_reg_above_64_val_t
        reg_val_abv64;
    uint32
        calendar_id = 0,
        port = 0, 
        intf_type = 0, 
        egr_if;
    SOC_TMC_FC_DIRECTION
        direction;
    int 
        nof_priorities = 0;
    uint32
        fc_index = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_key);
    SOCDNX_NULL_CHECK(fc_status_info);

    
    if (fc_status_key->port >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("port(%d) is out of range [0, %d]"), fc_status_key->port, (SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports-1)));
    }

    if (fc_status_key->calendar_id >= SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("calendar_id(%d) is out of range [0, %d]"), fc_status_key->calendar_id, (SOC_DPP_DEFS_GET(unit, fc_inb_cal_len_max)-1)));
    }

    calendar_id = fc_status_key->calendar_id;
    port = fc_status_key->port;
    intf_type = fc_status_key->intf_type;
    direction = fc_status_key->direction;

    if (fc_status_key->direction == SOC_TMC_FC_DIRECTION_GEN) {
        
        SOCDNX_IF_ERR_EXIT(_arad_fc_parse_tx_cal_tbl(unit, intf_type, FALSE, direction, port, calendar_id,
            &fc_status_info->cal_tx_src_type,
            &fc_status_info->cal_tx_src_vsq_type,
            &fc_status_info->cal_tx_src_id));

        
        if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_HCFC) {
            SOCDNX_IF_ERR_EXIT(_arad_fc_trigger_status_info_get(unit, fc_status_key, fc_status_info));
        }

        
        reg = ((port == 0) ? NBI_FC_ILKN_RX_0_CHFC_FROM_CFC_RAWr : NBI_FC_ILKN_RX_1_CHFC_FROM_CFC_RAWr);
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *reg_val32_hi = COMPILER_64_HI(reg_val64);
        reg_val32_lo = COMPILER_64_LO(reg_val64);
        fc_status_info->nbi_ilkn_rx_chfc_from_cfc_raw = (calendar_id >= 32) ? 
            (SHR_BITGET(reg_val32_hi, calendar_id-32) ? 1 : 0) : 
            (SHR_BITGET(&reg_val32_lo, calendar_id) ? 1 : 0);

        reg = ((port == 0) ? NBI_FC_ILKN_TX_0_GEN_CHFC_ROCr : NBI_FC_ILKN_TX_1_GEN_CHFC_ROCr);
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *reg_val32_hi = COMPILER_64_HI(reg_val64);
        reg_val32_lo = COMPILER_64_LO(reg_val64);
        fc_status_info->nbi_ilkn_tx_chfc_roc = (calendar_id >= 32) ? 
            (SHR_BITGET(reg_val32_hi, calendar_id-32) ? 1 : 0) : 
            (SHR_BITGET(&reg_val32_lo, calendar_id) ? 1 : 0);
    }
    else
    {
        if (fc_status_key->calendar_fc_type == SOC_TMC_FC_CALENDAR_TYPE_LLFC) 
        {
            reg = ((port == 0) ? NBI_FC_ILKN_RX_0_LLFC_FROM_RX_CNTr : NBI_FC_ILKN_RX_1_LLFC_FROM_RX_CNTr);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            COMPILER_64_SET(fc_status_info->nbi_ilkn_rx_llfc_cnt, 0, reg_val32);

            reg = ((port == 0) ? NBI_FC_ILKN_TX_0_LLFC_STOP_TX_CNTr : NBI_FC_ILKN_TX_1_LLFC_STOP_TX_CNTr);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
            COMPILER_64_SET(fc_status_info->nbi_ilkn_llfc_stop_tx_cnt, 0, reg_val32);
        }
        else if (fc_status_key->calendar_fc_type == SOC_TMC_FC_CALENDAR_TYPE_CHFC) 
        {
            
            SOCDNX_IF_ERR_EXIT(_arad_fc_parse_rx_cal_tbl(unit, intf_type, FALSE, direction, port, calendar_id,
                &fc_status_info->cal_rx_dst_type,
                &fc_status_info->cal_rx_dst_id));
            fc_index = fc_status_info->cal_rx_dst_id;

            
            reg = ((port == 0) ? NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_RAWr : NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_RAWr);
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
            *reg_val32_hi = COMPILER_64_HI(reg_val64);
            reg_val32_lo = COMPILER_64_LO(reg_val64);
            fc_status_info->nbi_ilkn_rx_chfc_from_port_raw = (calendar_id >= 32) ? 
                (SHR_BITGET(reg_val32_hi, calendar_id-32) ? 1 : 0) : 
                (SHR_BITGET(&reg_val32_lo, calendar_id) ? 1 : 0); 

            reg = ((port == 0) ? NBI_FC_ILKN_RX_0_CHFC_FROM_PORT_ROCr : NBI_FC_ILKN_RX_1_CHFC_FROM_PORT_ROCr);
            SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
            *reg_val32_hi = COMPILER_64_HI(reg_val64);
            reg_val32_lo = COMPILER_64_LO(reg_val64);
            fc_status_info->nbi_ilkn_rx_chfc_from_port_roc = (calendar_id >= 32) ?
                (SHR_BITGET(reg_val32_hi, calendar_id-32) ? 1 : 0) :
                (SHR_BITGET(&reg_val32_lo, calendar_id) ? 1 : 0);

            
            reg = ((port == 0) ? CFC_ILKN_RX_0_FC_STATUSr : CFC_ILKN_RX_1_FC_STATUSr);
            SOC_REG_ABOVE_64_CLEAR(reg_val_abv64);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, 0, 0, reg_val_abv64));
            fc_status_info->cfc_ilkn_fc_status = SHR_BITGET(reg_val_abv64, calendar_id) ? 1 : 0;

            switch(fc_status_info->cal_rx_dst_type)
            {
            case SOC_TMC_FC_REC_CAL_DEST_NIF_LL:
                SOCDNX_IF_ERR_EXIT(_arad_fc_llfc_stop_tx_status_info_get(unit, fc_index, fc_status_info));
                break;
            case SOC_TMC_FC_REC_CAL_DEST_PFC:
                if (nof_priorities == 0) nof_priorities = 1;
                
                
            case SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY:
                if (nof_priorities == 0) nof_priorities = 2;
                
                
            case SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY:
                if (nof_priorities == 0) nof_priorities = 8;
                SOCDNX_IF_ERR_EXIT(_arad_fc_queue_pair_to_egr_if_get(unit, fc_index, &egr_if));

                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, FALSE, fc_index, nof_priorities, fc_status_info));
                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_inf_status_info_get(unit, egr_if, fc_status_info));
                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_dev_status_info_get(unit, fc_status_info));
                break;
            case SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC:
                SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, TRUE, 0, 0, fc_status_info));
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support")));
                break;
            }
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support calendar_fc_type(%d)\n"), fc_status_key->calendar_fc_type));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_mub_inb_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    soc_field_t
        fld = INVALIDf;
    uint32
        reg_val32 = 0,
        fld_val32[1] = {0};
    uint32
        calendar_id = 0, 
        port = 0;
    uint32
        vsq_group = SOC_TMC_ITM_VSQ_GROUP_CTGRY,
        vsq_id = 0,
        fc_index = 0;
    uint8
        is_ocb_only;
    SOC_TMC_FC_ILKN_MUB_GEN_CAL
        cal_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_key);
    SOCDNX_NULL_CHECK(fc_status_info);

    
    if (fc_status_key->port >= SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("port(%d) is out of range [0, %d]"), fc_status_key->port, (SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports-1)));
    }

    if (fc_status_key->calendar_id >= SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("calendar_id(%d) is out of range [0, %d]"), fc_status_key->calendar_id, (SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN-1)));
    }

    calendar_id = fc_status_key->calendar_id;
    port = fc_status_key->port;

    if (fc_status_key->direction == SOC_TMC_FC_DIRECTION_GEN) {
        
        SOCDNX_IF_ERR_EXIT(arad_fc_ilkn_mub_gen_cal_get(unit, port, &cal_info));

        fc_index = cal_info.entries[calendar_id].id;

        if (cal_info.entries[calendar_id].source == SOC_TMC_FC_GEN_CAL_SRC_STE) {
            if (SOC_SAND_FAILURE(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_vsq_index_global2group,(unit, fc_index, &vsq_group, &vsq_id, &is_ocb_only)))) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unit %d, error in vsq id retrieve %d"), unit, fc_index));
            }
            if(unit >= SOC_MAX_NUM_DEVICES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("ERROR: invalid unit")));
            }
            fc_status_info->cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_VSQ;
            fc_index = vsq_id;
        }
        else if (cal_info.entries[calendar_id].source == SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ) {
            fc_status_info->cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_VSQ;
            vsq_group = SOC_TMC_ITM_VSQ_GROUP_LLFC;    
        }
        else if (cal_info.entries[calendar_id].source == SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ) {
            fc_status_info->cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_VSQ;
            vsq_group = SOC_TMC_ITM_VSQ_GROUP_PFC;    
        }
        else if (cal_info.entries[calendar_id].source == SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS) {
            fc_status_info->cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_GLB;
        }
        else if (cal_info.entries[calendar_id].source == SOC_TMC_FC_GEN_CAL_SRC_LLFC) {
            fc_status_info->cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_NIF;
        }
        else if (cal_info.entries[calendar_id].source == SOC_TMC_FC_GEN_CAL_SRC_HCFC) {
            fc_status_info->cal_tx_src_type = SOC_TMC_FC_SRC_TYPE_HCFC;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support fc_src(%d)\n"), cal_info.entries[calendar_id].source));
        }

        fc_status_info->cal_tx_src_vsq_type = vsq_group;
        fc_status_info->cal_tx_src_id = fc_index;

        
        if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_HCFC) {
            SOCDNX_IF_ERR_EXIT(_arad_fc_trigger_status_info_get(unit, fc_status_key, fc_status_info));
        }

        
        reg = NBI_FC_TX_MUBITS_FROM_CFCr;
        fld = ((port == 0) ? FC_TX_0_MUBITS_FROM_CFCf : FC_TX_1_MUBITS_FROM_CFCf);
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, fld);
        fc_status_info->nbi_mub_tx_from_cfc = SHR_BITGET(fld_val32, calendar_id) ? 1 : 0;

        reg = NBI_ILKN_MULTIPLE_USE_BITS_STATUSr;
        fld = ((port == 0) ? ILKN_TX_0_MULTIPLE_USE_BITS_VALUEf : ILKN_TX_1_MULTIPLE_USE_BITS_VALUEf);
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, fld);
        fc_status_info->nib_mub_tx_value = SHR_BITGET(fld_val32, calendar_id) ? 1 : 0;
    }
    else {
        if (fc_status_key->calendar_fc_type == SOC_TMC_FC_CALENDAR_TYPE_LLFC) 
        {
            
            reg = NBI_ILKN_MULTIPLE_USE_BITS_STATUSr;
            fld = ((port == 0) ? ILKN_RX_0_MULTIPLE_USE_BITS_VALUEf : ILKN_RX_1_MULTIPLE_USE_BITS_VALUEf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, fld);
            fc_status_info->nbi_mub_rx_value = SHR_BITGET(fld_val32, calendar_id) ? 1 : 0;

            reg = NBI_FC_RX_LLFC_STOP_TX_FROM_MUBITSr;
            fld = ((port == 0) ? FC_RX_0_LLFC_STOP_TX_FROM_MUBITSf : FC_RX_1_LLFC_STOP_TX_FROM_MUBITSf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, fld);
            fc_status_info->nbi_mub_llfc_stop_tx_from_mub = *fld_val32;
        }
        else if (fc_status_key->calendar_fc_type == SOC_TMC_FC_CALENDAR_TYPE_CHFC) 
        {
            
            reg = NBI_FC_RX_MUBITS_TO_CFCr;
            fld = ((port == 0) ? FC_RX_0_MUBITS_TO_CFCf : FC_RX_1_MUBITS_TO_CFCf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, fld);
            fc_status_info->nbi_mub_rx_to_cfc = SHR_BITGET(fld_val32, calendar_id) ? 1 : 0;

            
            reg = CFC_NIF_MUB_STATUSr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_mub_fc_status = SHR_BITGET(fld_val32, (calendar_id + ((port==1)?8:0)) ) ? 1 : 0;

            
            SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, TRUE, fc_index, 0, fc_status_info));
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support calendar_fc_type(%d)\n"), fc_status_key->calendar_fc_type));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t
  _arad_fc_oob_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  )
{
    soc_reg_t
        reg = INVALIDr;
    soc_field_t
        fld = INVALIDf, 
        fld_hcfc_crc_err_arr[SOC_TMC_FC_NOF_OOB_IDS][SOC_TMC_FC_HCFC_CHANNEL_NUM] = 
            {{HCFC_OOB_RX_0_CRC_ERR_CTR_0f, 
              HCFC_OOB_RX_0_CRC_ERR_CTR_1f, 
              HCFC_OOB_RX_0_CRC_ERR_CTR_2f,
              HCFC_OOB_RX_0_CRC_ERR_CTR_3f,
              HCFC_OOB_RX_0_CRC_ERR_CTR_4f}, 
             {HCFC_OOB_RX_1_CRC_ERR_CTR_0f, 
              HCFC_OOB_RX_1_CRC_ERR_CTR_1f, 
              HCFC_OOB_RX_1_CRC_ERR_CTR_2f,
              HCFC_OOB_RX_1_CRC_ERR_CTR_3f,
              HCFC_OOB_RX_1_CRC_ERR_CTR_4f}};
    uint32
        reg_val32 = 0,
        fld_val32 = 0,
        reg_val32_hi = 0,
        reg_val32_lo = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    soc_reg_above_64_val_t
        reg_val_abv64;
    uint32
        calendar_id = 0, 
        port = 0, 
        intf_type = 0,
        egr_if = 0;
    SOC_TMC_FC_DIRECTION
        direction;
    int 
        i = 0,
        nof_priorities = 0;
    uint32
        fc_index = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_key);
    SOCDNX_NULL_CHECK(fc_status_info);

    
    if (fc_status_key->port >= SOC_DPP_CONFIG(unit)->tm.max_oob_ports) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("port(%d) is out of range [0, %d]"), fc_status_key->port, (SOC_DPP_CONFIG(unit)->tm.max_oob_ports-1)));
    }

    if (fc_status_key->calendar_id >= SOC_TMC_FC_OOB_CAL_LEN_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("calendar_id(%d) is out of range [0, %d]"), fc_status_key->calendar_id, (SOC_TMC_FC_OOB_CAL_LEN_MAX-1)));
    }

    calendar_id = fc_status_key->calendar_id;
    port = fc_status_key->port;
    intf_type = fc_status_key->intf_type;
    direction = fc_status_key->direction;

    if (fc_status_key->direction == SOC_TMC_FC_DIRECTION_GEN) {
        
        SOCDNX_IF_ERR_EXIT(_arad_fc_parse_tx_cal_tbl(unit, intf_type, TRUE, direction, port, calendar_id,
            &fc_status_info->cal_tx_src_type,
            &fc_status_info->cal_tx_src_vsq_type,
            &fc_status_info->cal_tx_src_id));

        
        if (fc_status_info->cal_tx_src_type != SOC_TMC_FC_SRC_TYPE_HCFC) {
            SOCDNX_IF_ERR_EXIT(_arad_fc_trigger_status_info_get(unit, fc_status_key, fc_status_info));
        }
    }
    else {
        
        SOCDNX_IF_ERR_EXIT(_arad_fc_parse_rx_cal_tbl(unit, intf_type, TRUE, direction, port, calendar_id,
            &fc_status_info->cal_rx_dst_type,
            &fc_status_info->cal_rx_dst_id));
        fc_index = fc_status_info->cal_rx_dst_id;

        
        if (intf_type == SOC_TMC_FC_INTF_TYPE_ILKN) {
            reg = ((port == 0) ? CFC_ILKN_RX_0_FC_STATUSr : CFC_ILKN_RX_1_FC_STATUSr);
            SOC_REG_ABOVE_64_CLEAR(reg_val_abv64);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, 0, 0, reg_val_abv64));
            fc_status_info->cfc_ilkn_fc_status = SHR_BITGET(reg_val_abv64, calendar_id) ? 1 : 0;
        }

        switch(fc_status_info->cal_rx_dst_type)
        {
        case SOC_TMC_FC_REC_CAL_DEST_NIF_LL:
            if (intf_type == SOC_TMC_FC_INTF_TYPE_SPI) {
                reg = ((port == 0) ?  CFC_SPI_OOB_RX_0_LLFC_STATUSr : CFC_SPI_OOB_RX_1_LLFC_STATUSr);
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
                fc_status_info->cfc_spi_rx_llfc_status = SHR_BITGET(&fld_val32, fc_index) ? 1 : 0;
            }

            SOCDNX_IF_ERR_EXIT(_arad_fc_llfc_stop_tx_status_info_get(unit, fc_index, fc_status_info));
            break;
        case SOC_TMC_FC_REC_CAL_DEST_PFC:
            if (nof_priorities == 0) nof_priorities = 1;
            
            
        case SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY:
            if (nof_priorities == 0) nof_priorities = 2;
            
            
        case SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY:
            if (nof_priorities == 0) nof_priorities = 8;

            if (intf_type == SOC_TMC_FC_INTF_TYPE_SPI) {
                reg = CFC_SPI_OOB_RX_PFC_SELr;
                fld_val32 = (fc_status_info->cal_rx_dst_type == 0) ? 0 : 1;
                fld = ((port == 0) ? SPI_OOB_RX_0_PFC_SRC_SELf : SPI_OOB_RX_1_PFC_SRC_SELf);
                fld_val32 = fc_index/64;
                fld = ((port == 0) ? SPI_OOB_RX_0_PFC_SELf : SPI_OOB_RX_1_PFC_SELf);
                soc_reg_field_set(unit, reg, &reg_val32, fld, fld_val32);
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

                reg = ((port == 0) ?  CFC_SPI_OOB_RX_0_PFC_STATUSr : CFC_SPI_OOB_RX_1_PFC_STATUSr);
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
                reg_val32_hi = COMPILER_64_HI(reg_val64);
                reg_val32_lo = COMPILER_64_LO(reg_val64);
                if (fc_index%64 >= 32){
                    SHR_BITCOPY_RANGE(&fc_status_info->cfc_spi_rx_pfc_status, 0, &reg_val32_hi, fc_index%32, nof_priorities);
                }
                else {
                    SHR_BITCOPY_RANGE(&fc_status_info->cfc_spi_rx_pfc_status, 0, &reg_val32_lo, fc_index%32, nof_priorities);
                }
            }

            SOCDNX_IF_ERR_EXIT(_arad_fc_queue_pair_to_egr_if_get(unit, fc_index, &egr_if));

            SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, FALSE, fc_index, nof_priorities, fc_status_info));
            SOCDNX_IF_ERR_EXIT(_arad_fc_egq_inf_status_info_get(unit, egr_if, fc_status_info));
            SOCDNX_IF_ERR_EXIT(_arad_fc_egq_dev_status_info_get(unit, fc_status_info));
            break;
        case SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC:
            if (intf_type == SOC_TMC_FC_INTF_TYPE_SPI) {
                reg = ((port == 0) ?   CFC_SPI_OOB_RX_0_GEN_PFC_STATUSr : CFC_SPI_OOB_RX_1_GEN_PFC_STATUSr);
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
                fc_status_info->cfc_spi_rx_gen_pfc_status = SHR_BITGET(&fld_val32, fc_index) ? 1 : 0;
            }

            SOCDNX_IF_ERR_EXIT(_arad_fc_egq_pfc_status_info_get(unit, TRUE, 0, 0, fc_status_info));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support cal_rx_dst_type(%d)\n"), fc_status_info->cal_rx_dst_type));
            break;
        }

        
        switch(intf_type)
        {
        case SOC_TMC_FC_INTF_TYPE_ILKN: 
            reg = CFC_OOB_RX_ERRr;
            fld = ((port == 0) ? ILKN_OOB_RX_0_ERRf : ILKN_OOB_RX_1_ERRf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_ilkn_rx_crc_err_status = soc_reg_field_get(unit, reg, reg_val32, fld);

            reg = ((port == 0) ? CFC_ILKN_0_OOB_RX_LANES_STATUSr : CFC_ILKN_1_OOB_RX_LANES_STATUSr);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_oob_rx_lanes_status = reg_val32;

            reg = ((port == 0) ? CFC_ILKN_0_OOB_RX_CRC_ERR_CNTr : CFC_ILKN_1_OOB_RX_CRC_ERR_CNTr);
            fld = ((port == 0) ? ILKN_0_OOB_RX_CRC_ERR_CNTRf : ILKN_1_OOB_RX_CRC_ERR_CNTRf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_ilkn_rx_crc_err_cnt = soc_reg_field_get(unit, reg, reg_val32, fld);
            break;
        case SOC_TMC_FC_INTF_TYPE_SPI: 
            reg = CFC_OOB_RX_ERRr;
            fld = ((port == 0) ? SPI_OOB_RX_0_ERRf : SPI_OOB_RX_1_ERRf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_spi_rx_crc_err_status = soc_reg_field_get(unit, reg, reg_val32, fld);

            reg = ((port == 0) ? CFC_SPI_OOB_RX_0_ERROR_COUNTERr : CFC_SPI_OOB_RX_1_ERROR_COUNTERr);
            fld = ((port == 0) ? SPI_OOB_RX_0_FRM_ERR_CNTf : SPI_OOB_RX_1_FRM_ERR_CNTf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_spi_rx_frame_err_cnt = soc_reg_field_get(unit, reg, reg_val32, fld);

            reg = ((port == 0) ? CFC_SPI_OOB_RX_0_ERROR_COUNTERr : CFC_SPI_OOB_RX_1_ERROR_COUNTERr);
            fld = ((port == 0) ? SPI_OOB_RX_0_DIP_2_ERR_CNTf : SPI_OOB_RX_1_DIP_2_ERR_CNTf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_spi_rx_dip_2_err_cnt = soc_reg_field_get(unit, reg, reg_val32, fld);
            break;
        case SOC_TMC_FC_INTF_TYPE_HCFC: 
            reg = CFC_OOB_RX_ERRr;
            fld = ((port == 0) ? HCFC_OOB_RX_0_ERRf : HCFC_OOB_RX_1_ERRf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_hcfc_rx_crc_err_status = soc_reg_field_get(unit, reg, reg_val32, fld);

            reg = ((port == 0) ? CFC_HCFC_OOB_RX_0_CRC_ERR_CTRr : CFC_HCFC_OOB_RX_1_CRC_ERR_CTRr);
            for (i = 0; i < 5; i++) {
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
                fc_status_info->cfc_hcfc_rx_crc_err_cnt[i] = soc_reg64_field32_get(unit, reg, reg_val64, fld_hcfc_crc_err_arr[port][i]);
            }

            reg = CFC_HCFC_OOB_RX_WD_ERR_STATUSr;
            fld = ((port == 0) ? HCFC_OOB_RX_0_WD_ERR_STATUSf : HCFC_OOB_RX_1_WD_ERR_STATUSf);
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            fc_status_info->cfc_hcfc_rx_wd_err_status = soc_reg_field_get(unit, reg, reg_val32, fld);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support")));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_fc_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(fc_status_key);
    SOCDNX_NULL_CHECK(fc_status_info);

    switch(fc_status_key->intf_type)
    {
    case SOC_TMC_FC_INTF_TYPE_NIF:
        SOCDNX_IF_ERR_EXIT(_arad_fc_nif_status_info_get(unit, fc_status_key, fc_status_info));
        break;
    case SOC_TMC_FC_INTF_TYPE_ILKN:
        if (fc_status_key->is_oob) {
            SOCDNX_IF_ERR_EXIT(_arad_fc_oob_status_info_get(unit, fc_status_key, fc_status_info));
        }
        else {
            SOCDNX_IF_ERR_EXIT(_arad_fc_ilkn_inb_status_info_get(unit, fc_status_key, fc_status_info));
        }
        break;
    case SOC_TMC_FC_INTF_TYPE_MUB:
        SOCDNX_IF_ERR_EXIT(_arad_fc_mub_inb_status_info_get(unit, fc_status_key, fc_status_info));
        break;
    case SOC_TMC_FC_INTF_TYPE_SPI:
    case SOC_TMC_FC_INTF_TYPE_HCFC:
        SOCDNX_IF_ERR_EXIT(_arad_fc_oob_status_info_get(unit, fc_status_key, fc_status_info));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Not support intf_type(%d)\n"), fc_status_key->intf_type));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

