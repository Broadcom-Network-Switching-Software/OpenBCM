/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_nif.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_port.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/phy/phymod_sim.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/QAX/qax_nif.h>
#include <soc/dpp/QAX/qax_init.h>

#define SOC_QAX_IS_ILKN24(offset) ((offset == 0) || (offset == 2))
#define SOC_QAX_IS_ILKN12(offset) ((offset == 1) || (offset == 3))

#define SOC_QAX_NOF_QMLFS               12
#define SOC_QAX_NOF_LANES_PER_NBI       16
#define SOC_QAX_NOF_QUADS_PER_NBI       4
#define SOC_QAX_NOF_PORTS_NBIH          16
#define SOC_QAX_NOF_PORTS_EACH_NBIL     52
#define SOC_QAX_NIF_ILKN_MAX_NOF_LANES  24
#define SOC_QAX_SUB_PHYS_IN_QSGMII      4
#define SOC_QAX_NOF_ILKN_WRAP            2
#define SOC_QAX_MAX_PMS_PER_ILKN_PORT    6
#define SOC_QAX_NOF_NBIL                2
#define SOC_QAX_NOF_NBIH                1
#define SOC_QAX_DEF_CORE_CLOCK          600000
#define SOC_QAX_MAX_NOF_ILKN_PORTS      4
#define SOC_QAX_NOF_LANES ((SOC_QAX_NOF_NBIH + SOC_QAX_NOF_NBIL) * SOC_QAX_NOF_LANES_PER_NBI)

#define SOC_QAX_NIF_ILKN0_ILKN2_WM_HIGH      (10)
#define SOC_QAX_NIF_ILKN0_ILKN2_WM_LOW       (11)
#define SOC_QAX_NIF_ILKN1_ILKN3_WM_HIGH      (9)
#define SOC_QAX_NIF_ILKN1_ILKN3_WM_LOW       (10)


#define QAX_SOC_PRD_MODE_VLAN   (0)
#define QAX_SOC_PRD_MODE_ITMH   (1)
#define QAX_SOC_PRD_MODE_HIGIG  (2)

#define SOC_QUX_NOF_QMLFS               9

#define QAX_SOC_PRD_MAP_BITS_PER_PRIORITY  (2)

#define SOC_QAX_PRD_TM_MAP_MAX_VAL     (0x3F)
#define SOC_QAX_PRD_ETH_MAP_MAX_VAL    (0xF)

#define SOC_QAX_FIRST_ILKN_PHY          -1

static soc_dpp_pm_entry_t soc_qax_pml_table[] = {
     
        {1,         1,          0,            0x00},
        {1,         1,          0,            0x04},
        {1,         1,          0,            0x08},
        {0,         1,          0,            0x0c},

        {1,         1,          1,            0x00},
        {1,         1,          1,            0x04},
        {1,         1,          1,            0x08},
        {0,         1,          1,            0x0c},
};

static soc_dpp_pm_entry_t soc_qux_pml_table[] = {
     
        {0,         1,          0,            0x00},
        {0,         1,          0,            0x04},
};

static portmod_pm_instances_t qax_pm_instances[] = {
    {portmodDispatchTypePm4x25, 4},
    {portmodDispatchTypeDnx_fabric_o_nif, 4},
    {portmodDispatchTypePm4x10, 2},
    {portmodDispatchTypePm4x10Q, 6},
    {portmodDispatchTypePmOsILKN, 2}
};

static portmod_pm_instances_t qux_pm_instances[] = {
    {portmodDispatchTypePm4x10, 2},
#ifdef PORTMOD_PM4X2P5_SUPPORT
    {portmodDispatchTypePm4x2p5, 7},
#endif 
};

static portmod_pm_identifier_t qax_ilkn_pm_table[SOC_QAX_NOF_ILKN_WRAP][SOC_QAX_MAX_PMS_PER_ILKN_PORT] =
{
        {
          {portmodDispatchTypePm4x25, 1},
          {portmodDispatchTypePm4x25, 5},
          {portmodDispatchTypePm4x25, 9},
          {portmodDispatchTypePm4x25, 13},
          {portmodDispatchTypePm4x10, 29},
          {portmodDispatchTypePm4x10, 45}
        },
        {
          {portmodDispatchTypePm4x10, 17},
          {portmodDispatchTypePm4x10, 21},
          {portmodDispatchTypePm4x10, 25},
          {portmodDispatchTypePm4x10, 33},
          {portmodDispatchTypePm4x10, 37},
          {portmodDispatchTypePm4x10, 41}
        },
};


STATIC uint32 qax_hrf_sch_index[] = {12, 17, 22, 27};

STATIC int soc_qax_port_sch_hrf_config_set(int unit, soc_port_t port, int enable);

STATIC int soc_qax_nif_priority_ilkn_tdm_set(int unit, int core, uint32 ilkn_id);
STATIC int soc_qax_nif_priority_ilkn_high_low_set(int unit, int core, int is_high, uint32 ilkn_id);
STATIC int soc_qax_nif_priority_quad_tdm_set(int unit, int core, uint32 quad);
STATIC int soc_qax_nif_priority_quad_high_low_set(int unit, int core, int is_high, uint32 quad);

int
soc_qax_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit))
    {
        *soc_pml_table = &soc_qux_pml_table[0];
    }
    else {
        *soc_pml_table = &soc_qax_pml_table[0];
    }

    SOCDNX_FUNC_RETURN;
}


int
soc_qax_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit))
    {
          *pm_instances = &qux_pm_instances[0];
#ifdef PORTMOD_PM4X2P5_SUPPORT
          *pms_instances_arr_len = 2;
#else
          *pms_instances_arr_len = 1;
#endif 
    }
    else {
        *pm_instances = &qax_pm_instances[0];
        *pms_instances_arr_len = 5;
    }
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy)
{
    int qsgmii_count, skip;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
         skip = 0;
    } else {
        if(phy < 17) {
            skip = 0;
        } else {
            qsgmii_count = phy - 17;
            if (qsgmii_count > 12) {
                qsgmii_count = 12;
            }
            if (phy > 8*4) {
               qsgmii_count += phy - 33;
               if (qsgmii_count > 24) {
                   qsgmii_count = 24;
               }
            }

            skip = qsgmii_count * (SOC_QAX_SUB_PHYS_IN_QSGMII - 1);
        }
    }

    *new_phy = phy + skip;

    SOCDNX_FUNC_RETURN;
}

int
soc_qax_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy)
{
    int qsgmii_count;
    int reduce = 0;
    SOCDNX_INIT_FUNC_DEFS;

    reduce = 0;

    if (!SOC_IS_QUX(unit) && (phy >= 17)) {
        qsgmii_count = phy - 17;
        if (qsgmii_count > 48) {
            qsgmii_count = 48;
        }

        if (phy >= 69) {
           qsgmii_count += phy - 69;
        }
        if (qsgmii_count > 96) {
            qsgmii_count = 96;
        }
        reduce = qsgmii_count - (qsgmii_count / SOC_QAX_SUB_PHYS_IN_QSGMII);
    }

    *new_phy = phy - reduce;

    SOCDNX_FUNC_RETURN;
}

STATIC int qax_enable_ilkn_fields[] = {
    ENABLE_PORT_0f,
    ENABLE_PORT_1f,
    ENABLE_PORT_2f,
    ENABLE_PORT_3f,
};


int
soc_qax_port_open_fab_o_nif_path(int unit, int port) {
    uint32 reg_val, otp_bits;
    int phy = 0;
    int quad, idx, reg_port;
    soc_reg_t reg;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;


    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy = SOC_INFO(unit).port_l2p_mapping[port];
    } else{
        phy = port;
    }

    quad = (phy - 1) / 4;
    reg_port = REG_PORT_ANY;

    
    if (SOC_IS_QUX(unit)) {
        reg = NIF_NIF_PM_CFGr;
        field = PM_N_OTP_PORT_BOND_OPTIONf;
        idx = quad % 9;
    } else {
        idx = quad % 4;
        reg = NBIH_NIF_PM_CFGr;
        field = PMH_N_OTP_PORT_BOND_OPTIONf;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, idx, &reg_val));
    otp_bits = soc_reg_field_get(unit, reg, reg_val, field);
    otp_bits |= 0x100; 
    otp_bits &= ~0xf0;
    soc_reg_field_set(unit, reg, &reg_val, field, otp_bits);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, idx, reg_val));
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC

int
soc_qax_port_fabric_o_nif_fgs_enable(int unit, int port, int enable) {


    uint32 reg_val;
    int lane;
    SOCDNX_INIT_FUNC_DEFS;

    lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_MACT_PM_CREDIT_FILTERr(unit, lane / 4, lane % 4, 0x2));

    
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_FGS_RSTNr(unit, lane, enable));

    
    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PMH_FGS_SRDS_SIG_DET_CFGr(unit, lane, &reg_val));
    soc_reg_field_set(unit, NBIH_NIF_PMH_FGS_SRDS_SIG_DET_CFGr, &reg_val, SRDS_SIG_DET_CFG_LANE_Nf, enable);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_FGS_SRDS_SIG_DET_CFGr(unit, lane, reg_val));

    if (enable) {
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_MACT_PM_CREDIT_FILTERr(unit, lane / 4, lane % 4, 0x0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_fabric_o_nif_bypass_interface_enable(int unit, int port, int enable) {


    uint32 reg_val;
    uint32  rst_lanes = 0;
    int lane;
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_IF_ERR_EXIT(soc_qax_port_fabric_o_nif_fgs_enable(unit, port, 0));


    lane = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    rst_lanes |= 1 << lane;

    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, &reg_val));
    if (enable) {
        reg_val |= rst_lanes;
    } else {
        reg_val &= ~rst_lanes;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, &reg_val));
    if (enable) {
        reg_val |= rst_lanes;
    } else {
        reg_val &= ~rst_lanes;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, reg_val));

    if (enable) {
        SOCDNX_IF_ERR_EXIT(soc_qax_port_fabric_o_nif_fgs_enable(unit, port, 1));
    }



exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_ilkn_pmh_clk_data_mux_set(int unit, int is_mesh_hybrid)
{
    int port, start_port = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (is_mesh_hybrid)
    {
        start_port = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
    }

    for (port=start_port ; port < SOC_QAX_NOF_LANES_PER_NBI ; port++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_CLK_BYPASS_MUXr(unit, port, 1));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_DATA_BYPASS_MUXr(unit, port, 1));
    }

    if (SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency == SOC_QAX_DEF_CORE_CLOCK) {
        
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_CLK_DATA_PATH_MUXr(unit, 0x0));
    } else {
        
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_CLK_DATA_PATH_MUXr(unit, 0x1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_ilkn_pml_clk_data_mux_set(int unit)
{
    int port;
    SOCDNX_INIT_FUNC_DEFS;

    for (port=0 ;  port < SOC_QAX_NOF_NBIL ; port++)
    {
        if (SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency == SOC_QAX_DEF_CORE_CLOCK) {
            
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PML_CLK_DATA_PATH_MUXr(unit, port, 0x0));
        } else {
            
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PML_CLK_DATA_PATH_MUXr(unit, port, 0xf));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_ilkn_pml_override_clk_data_mux(int unit, pbmp_t *pbmp)
{
    int nbil_reg_port, port, pm_index, reg_port_idx;
    int nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    uint32 first_phy_port, first_lane;
    uint32 reg_val_ori[SOC_QAX_NOF_NBIL], reg_val[SOC_QAX_NOF_NBIL];
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency == SOC_QAX_DEF_CORE_CLOCK) {
        SOCDNX_FUNC_RETURN;
    }

    
    for (reg_port_idx = 0; reg_port_idx < SOC_QAX_NOF_NBIL; reg_port_idx ++) {
        SOCDNX_IF_ERR_EXIT(
                READ_NBIL_NIF_PML_CLK_DATA_PATH_MUXr(unit, reg_port_idx, &reg_val_ori[reg_port_idx]));
        reg_val[reg_port_idx] = reg_val_ori[reg_port_idx];
    }

    SOC_PBMP_ITER(*pbmp, port) {
        SOCDNX_IF_ERR_EXIT(
            soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));
        SOCDNX_IF_ERR_EXIT(
            MBCM_DPP_SOC_DRIVER_CALL(unit,
                                     mbcm_dpp_qsgmii_offsets_remove,
                                     (unit, first_phy_port, &first_lane)));

        --first_lane; 
        if (first_lane < SOC_DPP_DEFS_GET(unit, pml0_base_lane)) {
            continue;
        }
        nbil_reg_port = (first_lane / nof_lanes_nbi) - 1;
        pm_index = (first_lane % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

        if (IS_QSGMII_PORT(unit,port)) {
            SHR_BITCLR(&reg_val[nbil_reg_port], pm_index);
        } else {
            SHR_BITSET(&reg_val[nbil_reg_port], pm_index);
        }
    }

    for (reg_port_idx = 0; reg_port_idx < SOC_QAX_NOF_NBIL; reg_port_idx ++) {
        if (reg_val[reg_port_idx] != reg_val_ori[reg_port_idx]) {
            SOCDNX_IF_ERR_EXIT(
                    WRITE_NBIL_NIF_PML_CLK_DATA_PATH_MUXr(unit, reg_port_idx, reg_val[reg_port_idx]));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_protocol_offset_verify(int unit, soc_port_t port, uint32 protocol_offset) {
    uint32 phy_port;
    uint32 lane_id, phy_is_valid = 0;
    portmod_pm_identifier_t *ilkn_table;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    if (protocol_offset >= SOC_QAX_MAX_NOF_ILKN_PORTS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Protocol offset %d is out of range for the given interface"), protocol_offset));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &lane_id)));

    ilkn_table = qax_ilkn_pm_table[protocol_offset / ARAD_NIF_NUM_OF_OFFSETS_IN_PROTOCOL_GROUP];

    for (i = 0; i < SOC_QAX_MAX_PMS_PER_ILKN_PORT; ++i ) {
        if (lane_id >= ilkn_table[i].phy && lane_id < ilkn_table[i].phy + NUM_OF_LANES_IN_PM) {
            phy_is_valid = 1;
            break;
        }
    }
    if (phy_is_valid == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Protocol offset %d is out of range for the given interface"), protocol_offset));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int soc_qax_port_ilkn_nbi_enable_set(int unit, soc_port_t port, int enable)
{
    soc_reg_t reg=0;
    soc_field_t field;
    soc_port_t second_il_port;
    uint32 reg_val, offset, nof_segments;
    int nbil_reg_port, speed;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));
    SOCDNX_IF_ERR_EXIT(soc_jer_nif_ilkn_nof_segments_calc(unit, port, &nof_segments));
    SOC_DPP_CONFIG(unit)->jer->nif.ilkn_nof_segments[offset] = nof_segments;

    nbil_reg_port = offset >> 1;

    if (enable) {
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, &reg_val));
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset], 1);
        if ( SOC_QAX_IS_ILKN24(offset) ) {
            if (nof_segments == 2) {
                soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset + 1], 1);
            }
            field = (offset == 0) ? HRF_0_TAKES_TWO_SEGMENTSf : HRF_2_TAKES_TWO_SEGMENTSf;
            soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, field, (nof_segments == 2) ? 1 : 0);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, reg_val));

        if( SOC_QAX_IS_ILKN12(offset) ) {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ENABLE_INTERLAKENr(unit, nbil_reg_port, &reg_val));
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, ENABLE_PORT_0f, 1);
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, HRF_0_ACTIVEf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ENABLE_INTERLAKENr(unit, nbil_reg_port, reg_val));

            reg = (offset < 2 ) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit, reg, &reg_val, SELECT_TX_WORDS_23_FROM_NBIL_NOT_NBIHf, 1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));
        }

    } else { 
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, &reg_val));
        if ( SOC_QAX_IS_ILKN24(offset) ) { 
            soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset], 0);
            
            if (nof_segments == 2) {
                SOCDNX_IF_ERR_EXIT(soc_jer_nif_is_ilkn_port_exist(unit, offset + 1 , &second_il_port));
                if (second_il_port == SOC_JER_INVALID_PORT) {
                    soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset + 1], 0);
                }
            }
            field = (offset == 0) ? HRF_0_TAKES_TWO_SEGMENTSf : HRF_2_TAKES_TWO_SEGMENTSf;
            soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, field, 1); 
        } else { 
            SOCDNX_IF_ERR_EXIT(soc_jer_nif_is_ilkn_port_exist(unit, offset - 1  , &second_il_port));
            if (second_il_port == SOC_JER_INVALID_PORT) {
                soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset], 0);
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, reg_val));

        if( SOC_QAX_IS_ILKN12(offset) ) { 
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ENABLE_INTERLAKENr(unit, nbil_reg_port, &reg_val));
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, ENABLE_PORT_0f, 0);
            soc_reg_field_set(unit, NBIL_ENABLE_INTERLAKENr, &reg_val, HRF_0_ACTIVEf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ENABLE_INTERLAKENr(unit, nbil_reg_port, reg_val));

            reg = (offset < 2 ) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
            soc_reg_field_set(unit, reg, &reg_val, SELECT_TX_WORDS_23_FROM_NBIL_NOT_NBIHf, 0);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_nif_ilkn_nof_segments_set(int unit, int port, uint32 nof_segments) {

    soc_field_t field;
    uint32 reg_val, offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));

    
    SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKENr(unit, &reg_val));
    if ( SOC_QAX_IS_ILKN24(offset) ) {
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, qax_enable_ilkn_fields[offset + 1], (nof_segments == 2) ? 1 : 0);
        field = (offset == 0) ? HRF_0_TAKES_TWO_SEGMENTSf : HRF_2_TAKES_TWO_SEGMENTSf;
        soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKENr, &reg_val, field, (nof_segments == 2) ? 1 : 0);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKENr(unit, reg_val));

    
    SOCDNX_IF_ERR_EXIT(portmod_port_ilkn_nof_segments_set(unit, port, nof_segments));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_open_ilkn_path(int unit, int port) {

    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    soc_pbmp_t phys, phy_lanes;
    soc_reg_t reg=0;
    soc_field_t field;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    uint32 reg_val, offset;
    uint32 is_master, tdm_index, egr_if, fld_val[1], credit_val;
    int mubits_tx_polarity, mubits_rx_polarity, fc_tx_polarity, fc_rx_polarity, core, nbil_reg_port, nbih_reg_offset;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phys);
    SOC_PBMP_CLEAR(phy_lanes);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];

    nbih_reg_offset = nbil_reg_port = offset >> 1;

    if (is_master) {

        SOC_PBMP_PORT_ADD(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
        SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));

        
        field = (offset & 1) ? ILKN_1_PORT_RSTNf : ILKN_0_PORT_RSTNf;
        reg = (offset < 2 ) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, field, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));

        
        SOCDNX_IF_ERR_EXIT(soc_qax_port_ilkn_nbi_enable_set(unit, port, 1));
    }

    
    credit_val = ilkn_config->retransmit.enable_tx ? 1024 : 512;
    if ( SOC_QAX_IS_ILKN24(offset) ) {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_HRF_TX_CONFIG_HRFr(unit, nbih_reg_offset, &reg_val));
        soc_reg_field_set(unit, NBIH_HRF_TX_CONFIG_HRFr, &reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, credit_val);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_HRF_TX_CONFIG_HRFr(unit, nbih_reg_offset, reg_val));
    } else { 
        SOCDNX_IF_ERR_EXIT(READ_NBIL_HRF_TX_CONFIG_HRFr(unit, nbil_reg_port, 0, &reg_val));
        soc_reg_field_set(unit, NBIL_HRF_TX_CONFIG_HRFr, &reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, credit_val);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_HRF_TX_CONFIG_HRFr(unit, nbil_reg_port, 0, reg_val));
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);

    if ( SOC_QAX_IS_ILKN24(offset) )
    {
        tdm_index = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? 2 : 0;
        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, &reg64_val));
        soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, reg64_val));
    }
    else 
    {
        tdm_index = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? 1 : 0;
        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, &reg64_val));
        soc_reg64_field32_set(unit, NBIL_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, reg64_val));
    }

    
    
    if ( SOC_QAX_IS_ILKN24(offset) )
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_HRF_RESETr, REG_PORT_ANY, 0, &reg_val));
        field = (offset == 0) ? HRF_TX_0_CONTROLLER_RSTNf : HRF_TX_1_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, NBIH_HRF_RESETr, &reg_val, field, 1);
        field = (offset == 0) ? HRF_RX_0_CONTROLLER_RSTNf : HRF_RX_2_CONTROLLER_RSTNf;
        soc_reg_field_set(unit, NBIH_HRF_RESETr, &reg_val, field, 1);
        if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
            field = (offset == 0) ? HRF_RX_1_CONTROLLER_RSTNf : HRF_RX_3_CONTROLLER_RSTNf;
            soc_reg_field_set(unit, NBIH_HRF_RESETr, &reg_val, field, 1);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_HRF_RESETr, REG_PORT_ANY, 0, reg_val));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIL_HRF_RESETr, nbil_reg_port, 0, &reg_val));
        soc_reg_field_set(unit, NBIL_HRF_RESETr, &reg_val, HRF_RX_0_CONTROLLER_RSTNf, 1);
        soc_reg_field_set(unit, NBIL_HRF_RESETr, &reg_val, HRF_TX_0_CONTROLLER_RSTNf, 1);
        if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
            soc_reg_field_set(unit, NBIL_HRF_RESETr, &reg_val, HRF_RX_1_CONTROLLER_RSTNf, 1);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIL_HRF_RESETr, nbil_reg_port, 0, reg_val));
    }

    
    
    switch (offset) {
    case 0:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_2f : RX_HRF_ENABLE_HRF_0f;
        break;
    case 1:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_3f : RX_HRF_ENABLE_HRF_1f;
        break;
    case 2:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_6f : RX_HRF_ENABLE_HRF_4f;
        break;
    case 3:
        field = (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) ? RX_HRF_ENABLE_HRF_7f : RX_HRF_ENABLE_HRF_5f;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid ILKN offset %d"), offset));
    }
    SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKEN_HRFr(unit, &reg_val));
    soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKEN_HRFr, &reg_val, field, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKEN_HRFr(unit, reg_val));

    
    reg = (offset < 2) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
    field = (offset & 1) ? ENABLE_PORT_1f : ENABLE_PORT_0f;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, reg, &reg_val, field, 1);
    soc_reg_field_set(unit, reg, &reg_val, ENABLE_CORE_CLOCKf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val));

    
    mubits_tx_polarity = ilkn_config->mubits_tx_polarity;
    mubits_rx_polarity = ilkn_config->mubits_rx_polarity;
    fc_tx_polarity = ilkn_config->fc_tx_polarity;
    fc_rx_polarity = ilkn_config->fc_tx_polarity;
    if (mubits_tx_polarity || mubits_rx_polarity || fc_tx_polarity || fc_rx_polarity) {
        reg = (offset < 2) ? ILKN_PMH_ILKN_INVERT_POLARITY_SIGNALSr : ILKN_PML_ILKN_INVERT_POLARITY_SIGNALSr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, (offset & 1), &reg_val));
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_RX_MUBITS_POLARITYf, mubits_rx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_TX_MUBITS_POLARITYf, mubits_tx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_RECEIVED_FC_POLARITYf, fc_rx_polarity);
        soc_reg_field_set(unit, reg, &reg_val, ILKN_N_INVERT_TX_FC_POLARITYf, fc_tx_polarity);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, (offset & 1), reg_val));
    }

    
    reg = (offset < 2) ? ILKN_PMH_ILKN_TX_CONFr : ILKN_PML_ILKN_TX_CONFr;
    field = (offset & 1) ? TX_1_RETRANS_ENf : TX_0_RETRANS_ENf;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, reg, &reg_val, field, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));

    
    if (is_master) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);
        
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON)
        {
            SOC_SAND_SET_BIT(egr_if, 0, 0); 
        }

        SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);
        SHR_BITCLR(fld_val, egr_if);
        soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON)
        {
            egr_if++;
            SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
            *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);
            SHR_BITCLR(fld_val, egr_if);
            soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_qax_port_sch_hrf_config_set(unit, port, 1));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_qax_port_close_ilkn_path(int unit, int port) {

    ARAD_PORTS_ILKN_CONFIG *ilkn_config;
    soc_reg_t reg=0;
    soc_field_t field, tdm_field, data_field;
    soc_port_t channel_i;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    uint32 reg_val[1], offset, nof_channels;
    uint32 tdm_index, egr_if, fld_val[1], credit_val;
    int core, nbil_reg_port, nbih_reg_offset, tdm_channels = 0;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_error_t rv;
    soc_pbmp_t channels;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(channels);

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &nof_channels));

    nbih_reg_offset = nbil_reg_port = offset >> 1;

    if (SOC_PBMP_MEMBER(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port)) {
        SOC_PBMP_PORT_REMOVE(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port);
    }

    if (nof_channels > 1) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &channels));
        SOC_PBMP_ITER(channels, channel_i){
            if (IS_TDM_PORT(unit, channel_i)) {
                ++tdm_channels;
            }
        }
        if (tdm_channels != 1 || !IS_TDM_PORT(unit, port)) {
            
            SOC_EXIT;
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_qax_port_sch_hrf_config_set(unit, port, 0));

    
    if (nof_channels == 1) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);
        
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON)
        {
            SOC_SAND_SET_BIT(egr_if, 0, 0); 
        }

        SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);
        SHR_BITSET(fld_val, egr_if);
        soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));

        
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON)
        {
            egr_if++;
            SOCDNX_IF_ERR_EXIT(READ_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
            *fld_val = soc_reg_above_64_field32_get(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, reg_above_64_val, NIF_NO_FRAG_Lf);
            SHR_BITSET(fld_val, egr_if);
            soc_reg_above_64_field32_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_above_64_val,NIF_NO_FRAG_Lf, *fld_val);
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_above_64_val));
        }

        
        reg = (offset < 2) ? ILKN_PMH_ILKN_TX_CONFr : ILKN_PML_ILKN_TX_CONFr;
        field = (offset & 1) ? TX_1_RETRANS_ENf : TX_0_RETRANS_ENf;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, reg, reg_val, field, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_above_64_val));

        
        reg = (offset < 2) ? ILKN_PMH_ILKN_INVERT_POLARITY_SIGNALSr : ILKN_PML_ILKN_INVERT_POLARITY_SIGNALSr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, (offset & 1), reg_val));
        soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_RX_MUBITS_POLARITYf, 0x0);
        soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_TX_MUBITS_POLARITYf, 0x0);
        soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_RECEIVED_FC_POLARITYf, 0x0);
        soc_reg_field_set(unit, reg, reg_val, ILKN_N_INVERT_TX_FC_POLARITYf, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, (offset & 1), *reg_val));

        
        reg = (offset < 2) ? ILKN_PMH_ENABLE_INTERLAKENr : ILKN_PML_ENABLE_INTERLAKENr;
        field = (offset & 1) ? ENABLE_PORT_1f : ENABLE_PORT_0f;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, reg_val));
        soc_reg_field_set(unit, reg, reg_val, field, 0x0);
        soc_reg_field_set(unit, reg, reg_val, ENABLE_CORE_CLOCKf, 0x1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, *reg_val));
    }

    if (nof_channels == 1 || IS_TDM_PORT(unit, port)) {
        
        
        switch (offset) {
        case 0:
            tdm_field = RX_HRF_ENABLE_HRF_2f;
            data_field = RX_HRF_ENABLE_HRF_0f;
            break;
        case 1:
            tdm_field = RX_HRF_ENABLE_HRF_3f;
            data_field = RX_HRF_ENABLE_HRF_1f;
            break;
        case 2:
            tdm_field = RX_HRF_ENABLE_HRF_6f;
            data_field = RX_HRF_ENABLE_HRF_4f;
            break;
        case 3:
            tdm_field = RX_HRF_ENABLE_HRF_7f;
            data_field = RX_HRF_ENABLE_HRF_5f;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid ILKN offset %d"), offset));
        }
        SOCDNX_IF_ERR_EXIT(READ_NBIH_ENABLE_INTERLAKEN_HRFr(unit, reg_val));
        if (IS_TDM_PORT(unit, port)) {
            soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKEN_HRFr, reg_val, tdm_field, 0);
        }
        if (nof_channels == 1) {
            soc_reg_field_set(unit, NBIH_ENABLE_INTERLAKEN_HRFr, reg_val, data_field, 0);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ENABLE_INTERLAKEN_HRFr(unit, *reg_val));

        
        if ( SOC_QAX_IS_ILKN24(offset))
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_HRF_RESETr, REG_PORT_ANY, 0, reg_val));
            if (nof_channels == 1) {
                field = (offset == 0) ? HRF_TX_0_CONTROLLER_RSTNf : HRF_TX_1_CONTROLLER_RSTNf;
                soc_reg_field_set(unit, NBIH_HRF_RESETr, reg_val, field, 0x0);
                field = (offset == 0) ? HRF_RX_0_CONTROLLER_RSTNf : HRF_RX_2_CONTROLLER_RSTNf;
                soc_reg_field_set(unit, NBIH_HRF_RESETr, reg_val, field, 0x0);
            }
            if (IS_TDM_PORT(unit, port)) {
                field = (offset == 0) ? HRF_RX_1_CONTROLLER_RSTNf : HRF_RX_3_CONTROLLER_RSTNf;
                soc_reg_field_set(unit, NBIH_HRF_RESETr, reg_val, field, 0x0);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_HRF_RESETr, REG_PORT_ANY, 0, *reg_val));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIL_HRF_RESETr, nbil_reg_port, 0, reg_val));
            if (nof_channels == 1) {
                soc_reg_field_set(unit, NBIL_HRF_RESETr, reg_val, HRF_RX_0_CONTROLLER_RSTNf, 0x0);
                soc_reg_field_set(unit, NBIL_HRF_RESETr, reg_val, HRF_TX_0_CONTROLLER_RSTNf, 0x0);
            }
            if (IS_TDM_PORT(unit, port)) {
                soc_reg_field_set(unit, NBIL_HRF_RESETr, reg_val, HRF_RX_1_CONTROLLER_RSTNf, 0x0);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIL_HRF_RESETr, nbil_reg_port, 0, *reg_val));
        }

        if ( SOC_QAX_IS_ILKN24(offset) )
        {
            if (IS_TDM_PORT(unit, port)) {
                tdm_index = 2;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, &reg64_val));
                soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, 0x0);
                SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, reg64_val));
            }
            if (nof_channels == 1) {
                tdm_index = 0;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, &reg64_val));
                soc_reg64_field32_set(unit, NBIH_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, 0x0);
                SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIH_HRF_RX_CONFIG_HRFr, REG_PORT_ANY, tdm_index + nbih_reg_offset, reg64_val));
            }
        }
        else 
        {
            if (IS_TDM_PORT(unit, port)) {
                tdm_index = 1;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, &reg64_val));
                soc_reg64_field32_set(unit, NBIL_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
                SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, reg64_val));
            }
            if (nof_channels == 1) {
                tdm_index = 0;
                SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, &reg64_val));
                soc_reg64_field32_set(unit, NBIL_HRF_RX_CONFIG_HRFr, &reg64_val, HRF_RX_BURST_MERGE_FORCE_HRF_Nf, ilkn_config->interleaved ? 0 : 1);
                SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, NBIL_HRF_RX_CONFIG_HRFr, nbil_reg_port, tdm_index, reg64_val));
            }
        }
    }

    if (nof_channels == 1) {
        
        credit_val = 0x200; 
        if ( SOC_QAX_IS_ILKN24(offset) ) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_HRF_TX_CONFIG_HRFr(unit, nbih_reg_offset, reg_val));
            soc_reg_field_set(unit, NBIH_HRF_TX_CONFIG_HRFr, reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, credit_val);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_HRF_TX_CONFIG_HRFr(unit, nbih_reg_offset, *reg_val));
        } else { 
            SOCDNX_IF_ERR_EXIT(READ_NBIL_HRF_TX_CONFIG_HRFr(unit, nbil_reg_port, 0, reg_val));
            soc_reg_field_set(unit, NBIL_HRF_TX_CONFIG_HRFr, reg_val, HRF_TX_NUM_CREDITS_TO_EGQ_HRF_Nf, credit_val);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_HRF_TX_CONFIG_HRFr(unit, nbil_reg_port, 0, *reg_val));
        }

        
        SOCDNX_IF_ERR_EXIT(soc_qax_port_ilkn_nbi_enable_set(unit, port, 0));

        
        field = (offset & 1) ? ILKN_1_PORT_RSTNf : ILKN_0_PORT_RSTNf;
        reg = (offset < 2 ) ? ILKN_PMH_ILKN_RESETr : ILKN_PML_ILKN_RESETr;

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, reg_val));
        soc_reg_field_set(unit, reg, reg_val, field, 0x0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, *reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_ilkn_init(int unit)
{
    int pm_index;
    portmod_pbmp_t ilkn_phys;
    portmod_pm_create_info_t pm_info;
    portmod_pm_identifier_t ilkn_pms[MAX_NUM_OF_PMS_IN_ILKN];
    portmod_pm_identifier_t *ilkn_table;
    uint32 core_freq = 0x0;
    int is_mesh_hybrid = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_arad_core_frequency_config_get(unit, SOC_JER_CORE_FREQ_KHZ_DEFAULT, &core_freq));

    if (SOC_IS_QUX(unit)) {
        SOC_EXIT;
    }
    
    PORTMOD_PBMP_CLEAR(ilkn_phys);
    for(pm_index = 0 ; pm_index < MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm_index]);
    }

    ilkn_table = qax_ilkn_pm_table[0];

    for(pm_index=0 ; pm_index<MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        ilkn_pms[pm_index].type = ilkn_table[pm_index].type;
        ilkn_pms[pm_index].phy = ilkn_table[pm_index].phy;

        PORTMOD_PBMP_PORTS_RANGE_ADD(ilkn_phys, ilkn_pms[pm_index].phy, NUM_OF_LANES_IN_PM);
    }

    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = MAX_NUM_OF_PMS_IN_ILKN;
    pm_info.pm_specific_info.os_ilkn.core_clock_khz = core_freq;
    pm_info.pm_specific_info.os_ilkn.wm_high[0] = SOC_QAX_NIF_ILKN0_ILKN2_WM_HIGH;
    pm_info.pm_specific_info.os_ilkn.wm_high[1] = SOC_QAX_NIF_ILKN1_ILKN3_WM_HIGH;
    pm_info.pm_specific_info.os_ilkn.wm_low[0] = SOC_QAX_NIF_ILKN0_ILKN2_WM_LOW;
    pm_info.pm_specific_info.os_ilkn.wm_low[1] = SOC_QAX_NIF_ILKN1_ILKN3_WM_LOW;
    PORTMOD_PBMP_ASSIGN(pm_info.phys, ilkn_phys);
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

    
    PORTMOD_PBMP_CLEAR(ilkn_phys);
    for(pm_index = 0 ; pm_index < MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        portmod_pm_identifier_t_init(unit, &ilkn_pms[pm_index]);
    }

    ilkn_table = qax_ilkn_pm_table[1];

    for(pm_index=0 ; pm_index<MAX_NUM_OF_PMS_IN_ILKN ; pm_index++)
    {
        ilkn_pms[pm_index].type = ilkn_table[pm_index].type;
        ilkn_pms[pm_index].phy = ilkn_table[pm_index].phy;

        PORTMOD_PBMP_PORTS_RANGE_ADD(ilkn_phys, ilkn_pms[pm_index].phy, NUM_OF_LANES_IN_PM);
    }

    SOCDNX_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN;
    pm_info.pm_specific_info.os_ilkn.controlled_pms = ilkn_pms;
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = MAX_NUM_OF_PMS_IN_ILKN;
    pm_info.pm_specific_info.os_ilkn.core_clock_khz = core_freq;
    PORTMOD_PBMP_ASSIGN(pm_info.phys, ilkn_phys);
    pm_info.pm_specific_info.os_ilkn.wm_high[0] = SOC_QAX_NIF_ILKN0_ILKN2_WM_HIGH;
    pm_info.pm_specific_info.os_ilkn.wm_high[1] = SOC_QAX_NIF_ILKN1_ILKN3_WM_HIGH;
    pm_info.pm_specific_info.os_ilkn.wm_low[0] = SOC_QAX_NIF_ILKN0_ILKN2_WM_LOW;
    pm_info.pm_specific_info.os_ilkn.wm_low[1] = SOC_QAX_NIF_ILKN1_ILKN3_WM_LOW;
    SOCDNX_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

    if (!SOC_WARM_BOOT(unit)) {
        is_mesh_hybrid = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_enable", 0);
        if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit) || is_mesh_hybrid || soc_feature(unit, soc_feature_packet_tdm_marking))
        {
            SOCDNX_IF_ERR_EXIT(soc_qax_ilkn_pmh_clk_data_mux_set(unit, is_mesh_hybrid));
        }
        SOCDNX_IF_ERR_EXIT(soc_qax_ilkn_pml_clk_data_mux_set(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_sch_config(int unit, soc_port_t port)
{
    uint32 reg_val;
    soc_pbmp_t quad_bmp;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    uint32 rx_req_high_en[1] = {0}, rx_req_tdm_en[1] = {0}, rx_req_quads_en[1] = {0};
    soc_error_t rv;
    int qmlf_index;
    soc_reg_t reg;
    uint64 reg_val_64, rx_req_low_en_64;
    int is_ilkn_over_fabric;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_portmod_port_quad_get(unit, port, &quad_bmp, &is_ilkn_over_fabric));

    
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
        if (soc_feature(unit, soc_feature_no_tdm)) {
            SOC_EXIT;
        }
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg_val));
        *rx_req_tdm_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg_val, RX_REQ_PIPE_0_TDM_ENf);
        SOC_PBMP_ITER(quad_bmp, qmlf_index) {
            if (qmlf_index >= SOC_QAX_NOF_QMLFS)
                break;

            SHR_BITSET(rx_req_tdm_en, qmlf_index);
        }
        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg_val, RX_REQ_PIPE_0_TDM_ENf, *rx_req_tdm_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg_val));
    } else {

        if (SOC_IS_QUX(unit)) {

            
            SOCDNX_IF_ERR_EXIT(READ_NIF_RX_REQ_PIPE_0_LOW_ENr(unit, &reg_val_64));
            rx_req_low_en_64 = soc_reg64_field_get(unit, NIF_RX_REQ_PIPE_0_LOW_ENr, reg_val_64, RX_REQ_PIPE_0_LOW_ENf);

            SOC_PBMP_ITER(quad_bmp, qmlf_index) {
                if (qmlf_index >= SOC_QUX_NOF_QMLFS)
                    break;
                SHR_BITSET(rx_req_quads_en, qmlf_index);
                COMPILER_64_BITCLR(rx_req_low_en_64, qmlf_index);
                COMPILER_64_BITCLR(rx_req_low_en_64, qmlf_index + 16);
                COMPILER_64_BITCLR(rx_req_low_en_64, qmlf_index + 32);
                COMPILER_64_BITCLR(rx_req_low_en_64, qmlf_index + 48);
            }
            soc_reg64_field_set(unit, NIF_RX_REQ_PIPE_0_LOW_ENr, &reg_val_64, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en_64);
            SOCDNX_IF_ERR_EXIT(WRITE_NIF_RX_REQ_PIPE_0_LOW_ENr(unit, reg_val_64));

            

            SOCDNX_IF_ERR_EXIT(READ_NIF_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg_val));
            reg = NIF_RX_REQ_PIPE_0_HIGH_ENr;
            *rx_req_high_en = soc_reg_field_get(unit, reg, reg_val, RX_REQ_PIPE_0_HIGH_ENf);

            SHR_BITOR_RANGE(rx_req_high_en, rx_req_quads_en, 0, 16, rx_req_high_en);

            soc_reg_field_set(unit, reg, &reg_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_high_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NIF_RX_REQ_PIPE_0_HIGH_ENr(unit, reg_val));

        } else {

            
            SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg_val));
            reg = NBIH_RX_REQ_PIPE_0_HIGH_ENr;
            *rx_req_high_en = soc_reg_field_get(unit, reg, reg_val, RX_REQ_PIPE_0_HIGH_ENf);

            SOC_PBMP_ITER(quad_bmp, qmlf_index) {
            if (qmlf_index >= SOC_QAX_NOF_QMLFS)
                break;

                SHR_BITSET(rx_req_high_en, qmlf_index);
            }

            soc_reg_field_set(unit, reg, &reg_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_high_en);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_qax_port_sch_hrf_config_set(int unit, soc_port_t port, int enable)
{
    uint32 offset, sch_index;
    soc_error_t rv;
    uint32 reg_val;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    uint32 rx_req_quads_en[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_QUX(unit)) {
         SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    sch_index =  qax_hrf_sch_index[offset];

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
    SOCDNX_IF_ERR_EXIT(rv);
    if (hdr_type == SOC_TMC_PORT_HEADER_TYPE_TDM)
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg_val));
        *rx_req_quads_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg_val, RX_REQ_PIPE_0_TDM_ENf);
        if (enable) {
            shr_bitop_range_set(rx_req_quads_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        } else {
            shr_bitop_range_clear(rx_req_quads_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        }
        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg_val, RX_REQ_PIPE_0_TDM_ENf, *rx_req_quads_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg_val));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg_val));
        *rx_req_quads_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg_val, RX_REQ_PIPE_0_HIGH_ENf);

        if (enable) {
            SHR_BITSET_RANGE(rx_req_quads_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        } else {
            SHR_BITCLR_RANGE(rx_req_quads_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        }
        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_quads_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_nif_priority_set(        int     unit,
                                     int     core,
                                     uint32  quad_ilkn,
                                     uint32  is_ilkn,
                                     uint32  flags,
                                     uint32  allow_tdm,
                                     int     priority_level)
{
    uint32 is_high_set, is_low_set, is_tdm_set;
    uint32 is_set_required=1;
    SOCDNX_INIT_FUNC_DEFS;

    if((priority_level < JER_NIF_PRIO_LOW_LEVEL) || (priority_level > JER_NIF_PRIO_HIGH_LEVEL))
    {
        if (priority_level != JER_NIF_PRIO_TDM_LEVEL)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("priority level %d is invalid for device"), priority_level));
        } else if ((priority_level == JER_NIF_PRIO_TDM_LEVEL) && (allow_tdm == 0)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("priority level %d can't be configured manually with this API"), priority_level));
        }
    }

    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_ilkn_tdm_get(unit, core, quad_ilkn, is_ilkn, &is_tdm_set));
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_ilkn_high_get(unit, core, quad_ilkn, is_ilkn, &is_high_set));
    SOCDNX_IF_ERR_EXIT( soc_jer_nif_priority_quad_ilkn_low_get(unit, core, quad_ilkn, is_ilkn, &is_low_set));

    
    if ((priority_level == JER_NIF_PRIO_HIGH_LEVEL && !is_high_set) ||
        (priority_level == JER_NIF_PRIO_LOW_LEVEL && !is_low_set) ||
        (priority_level == JER_NIF_PRIO_TDM_LEVEL && !is_tdm_set))
    {
        is_set_required = 1;
    } else {
        is_set_required = 0;
    }

    if (is_set_required) {
        if (is_ilkn)
        {
            
            if (priority_level == JER_NIF_PRIO_TDM_LEVEL)
            {
                
                SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_ilkn_tdm_clear(unit, quad_ilkn));
                
                SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_ilkn_tdm_set(unit, core, quad_ilkn));
            } else {
                if (priority_level == JER_NIF_PRIO_HIGH_LEVEL)
                {
                    
                    SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_ilkn_high_low_set(unit, core, 1, quad_ilkn));
                } else  {
                    
                    SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_ilkn_high_low_set(unit, core, 0, quad_ilkn));
                }
            }
        } else {
            
            if (priority_level == JER_NIF_PRIO_TDM_LEVEL)
            {
                
                SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_quad_tdm_high_low_clear(unit, quad_ilkn, 1, 0));
                
                SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_quad_tdm_set(unit, core, quad_ilkn));
            } else {
                if (priority_level == JER_NIF_PRIO_HIGH_LEVEL)
                {
                    
                    SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_quad_high_low_set(unit, core, 1, quad_ilkn));
                } else  {
                    
                    SOCDNX_IF_ERR_EXIT( soc_qax_nif_priority_quad_high_low_set(unit, core, 0, quad_ilkn));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_nif_priority_ilkn_high_low_clear(int unit, uint32 ilkn_id) {

    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 sch_index;
    uint32 rx_req_high_clear[1] = {0};
    soc_reg_above_64_val_t rx_req_low_clear;

    SOCDNX_INIT_FUNC_DEFS;

    sch_index =  qax_hrf_sch_index[ilkn_id];

    
    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg32_val));
    *rx_req_high_clear = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg32_val, RX_REQ_PIPE_0_HIGH_ENf);

    shr_bitop_range_clear(rx_req_high_clear, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

    soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg32_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_high_clear);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
    soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_clear);

    SHR_BITCLR_RANGE(rx_req_low_clear, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
    SHR_BITCLR_RANGE(rx_req_low_clear, sch_index + 32, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
    SHR_BITCLR_RANGE(rx_req_low_clear, sch_index + 64, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
    SHR_BITCLR_RANGE(rx_req_low_clear, sch_index + 96, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

    soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_clear);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;

}


int soc_qax_nif_priority_ilkn_tdm_clear(int unit, uint32 ilkn_id) {

    uint32 reg32_val;
    uint32 sch_index;
    uint32 rx_req_tdm_clear[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    sch_index =  qax_hrf_sch_index[ilkn_id];

    
    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg32_val));

    *rx_req_tdm_clear = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg32_val, RX_REQ_PIPE_0_TDM_ENf);

    shr_bitop_range_clear(rx_req_tdm_clear, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
    soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg32_val, RX_REQ_PIPE_0_TDM_ENf, *rx_req_tdm_clear);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;

}

STATIC int soc_qax_nif_priority_ilkn_tdm_set(int unit, int core, uint32 ilkn_id) {

    uint32 reg32_val;
    uint32 sch_index;
    uint32 rx_req_tdm_en[1] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    sch_index =  qax_hrf_sch_index[ilkn_id];

    SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg32_val));
    *rx_req_tdm_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, reg32_val, RX_REQ_PIPE_0_TDM_ENf);

    shr_bitop_range_set(rx_req_tdm_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

    soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, &reg32_val, RX_REQ_PIPE_0_TDM_ENf, *rx_req_tdm_en);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_qax_nif_priority_ilkn_high_low_set(int unit, int core, int is_high, uint32 ilkn_id) {

    uint32 share_quad_ilkn_id;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    int is_share_quad, share_quad_port_core, priority;
    soc_port_t share_quad_port;

    uint32 sch_index;
    uint32 rx_req_high_en[1] = {0};
    uint32 rx_req_high_clear[1] = {0};
    soc_reg_above_64_val_t rx_req_low_en;
    soc_reg_above_64_val_t rx_req_low_clear;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_nif_is_ilkn_share_quad(unit, ilkn_id, &is_share_quad, &share_quad_port));
    if (is_share_quad) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, share_quad_port, &share_quad_port_core));
        share_quad_ilkn_id = (ilkn_id & 1) ? ilkn_id - 1 : ilkn_id + 1;
        SOCDNX_IF_ERR_EXIT(soc_jer_nif_priority_get(unit, share_quad_port_core, share_quad_ilkn_id, 1, 0, 0, &priority));
        if (priority != (is_high ? 1 : 0)) {
            LOG_WARN(BSL_LS_BCM_PORT,(BSL_META_U(unit, "Detecting priority change for ILKN port sharing serdes quad/s with another ILKN port. "
                                                       "Note that all ports on the same serdes quad must have the same priority!\n")));
        }
    }
    
    
    sch_index =  qax_hrf_sch_index[ilkn_id];

    if (is_high) {

        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg32_val));
        *rx_req_high_en = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg32_val, RX_REQ_PIPE_0_HIGH_ENf);

        SHR_BITSET_RANGE(rx_req_high_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg32_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_high_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg32_val));
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_clear);

        SHR_BITCLR_RANGE(rx_req_low_clear, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        SHR_BITCLR_RANGE(rx_req_low_clear, sch_index + 32, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        SHR_BITCLR_RANGE(rx_req_low_clear, sch_index + 64, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        SHR_BITCLR_RANGE(rx_req_low_clear, sch_index + 96, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

        soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_clear);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
    } else {

        

        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        soc_reg_above_64_field_get(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);

        SHR_BITSET_RANGE(rx_req_low_en, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        SHR_BITSET_RANGE(rx_req_low_en, sch_index + 32, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        SHR_BITSET_RANGE(rx_req_low_en, sch_index + 64, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);
        SHR_BITSET_RANGE(rx_req_low_en, sch_index + 96, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

        soc_reg_above_64_field_set(unit, NBIH_RX_REQ_PIPE_0_LOW_ENr, reg_above_64_val, RX_REQ_PIPE_0_LOW_ENf, rx_req_low_en);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg32_val));
        *rx_req_high_clear = soc_reg_field_get(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, reg32_val, RX_REQ_PIPE_0_HIGH_ENf);

        shr_bitop_range_clear(rx_req_high_clear, sch_index, SOC_QAX_HRF_SCH_PRIO_BIT_NUM);

        soc_reg_field_set(unit, NBIH_RX_REQ_PIPE_0_HIGH_ENr, &reg32_val, RX_REQ_PIPE_0_HIGH_ENf, *rx_req_high_clear);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_nif_priority_quad_tdm_high_low_clear(int unit, uint32 quad, int clear_tdm, int clear_high_low) {

    uint32 mask;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t above64_mask;
#ifdef BCM_QUX_SUPPORT
    uint64 reg64_val, mask_64;
    uint32 mask_low = 0;
    uint32 mask_high = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    
    mask = ~(1 << quad);

#ifdef BCM_QUX_SUPPORT
    COMPILER_64_ZERO(mask_64);
    COMPILER_64_ZERO(reg64_val);
    if (SOC_IS_QUX(unit)) {
        if (quad >= SAL_UINT32_NOF_BITS) {
            mask_high = 1 << (quad - SAL_UINT32_NOF_BITS);
        } else {
            mask_low = 1 << quad;
        }
        mask_high = ~mask_high;
        mask_low = ~mask_low;

        COMPILER_64_SET(mask_64, mask_high, mask_low);
    } else
#endif
    {
        SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);
    }

    
    if (clear_tdm && !SOC_IS_QUX(unit)) {
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, &reg32_val));
        reg32_val &= mask;
        SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_0_TDM_ENr(unit, reg32_val));

    }

    
    if (clear_high_low) {

        if (SOC_IS_QUX(unit)) {
#ifdef BCM_QUX_SUPPORT

            
            SOCDNX_IF_ERR_EXIT( READ_NIF_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg32_val));
            reg32_val &= mask;
            SOCDNX_IF_ERR_EXIT( WRITE_NIF_RX_REQ_PIPE_0_HIGH_ENr(unit, reg32_val));

           
            

            

            SOCDNX_IF_ERR_EXIT( READ_NIF_RX_REQ_PIPE_0_LOW_ENr(unit, &reg64_val));
            COMPILER_64_AND(reg64_val, mask_64);
            SOCDNX_IF_ERR_EXIT( WRITE_NIF_RX_REQ_PIPE_0_LOW_ENr(unit, reg64_val));

            
            
#endif
        } else {
            
            SOCDNX_IF_ERR_EXIT( READ_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, &reg32_val));
            reg32_val &= mask;
            SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_0_HIGH_ENr(unit, reg32_val));

            
            SOCDNX_IF_ERR_EXIT( READ_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
            SOC_REG_ABOVE_64_AND(reg_above_64_val, above64_mask);
            SOCDNX_IF_ERR_EXIT( WRITE_NBIH_RX_REQ_PIPE_0_LOW_ENr(unit, reg_above_64_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}


STATIC int soc_qax_nif_priority_quad_tdm_set(int unit, int core, uint32 quad) {
    uint32 mask;
    uint32 reg32_val;

    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_QUX(unit)) {
        
        SOCDNX_FUNC_RETURN;
    }

    
    mask = (1 << quad);

    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, REG_PORT_ANY, 0, &reg32_val));
    reg32_val |= mask;
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, REG_PORT_ANY, 0, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_qax_nif_priority_quad_high_low_set(int unit, int core, int is_high, uint32 quad) {

    uint32 mask;
    uint32 clear_mask;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t above64_clear_mask;
    soc_reg_above_64_val_t above64_mask;
    uint32 reg32_val;
    soc_reg_t prio_reg;
    soc_reg_t prio_reg1;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_QUX(unit)) {
        prio_reg = (is_high == 1) ? NIF_RX_REQ_PIPE_0_HIGH_ENr : NIF_RX_REQ_PIPE_0_LOW_ENr;
        prio_reg1 = (is_high == 1) ? NIF_RX_REQ_PIPE_0_LOW_ENr : NIF_RX_REQ_PIPE_0_HIGH_ENr;
    } else {
        prio_reg = (is_high == 1) ? NBIH_RX_REQ_PIPE_0_HIGH_ENr : NBIH_RX_REQ_PIPE_0_LOW_ENr;
        prio_reg1 = (is_high == 1) ? NBIH_RX_REQ_PIPE_0_LOW_ENr : NBIH_RX_REQ_PIPE_0_HIGH_ENr;
    }

    
    mask = (1 << quad);
    clear_mask = ~mask;

    if (is_high) {
        
        if (SOC_IS_QUX(unit)) {
            clear_mask &= ~(1 << (quad + 16));
        }
        
        SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_clear_mask, clear_mask);

        
        SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, prio_reg, REG_PORT_ANY, 0, &reg32_val));
        reg32_val |= mask;
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, prio_reg, REG_PORT_ANY, 0, reg32_val));

        
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, prio_reg1, REG_PORT_ANY, 0, reg_above_64_val));
        SOC_REG_ABOVE_64_AND(reg_above_64_val, above64_clear_mask);
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, prio_reg1, REG_PORT_ANY, 0, reg_above_64_val));

    } else {
        
        if (SOC_IS_QUX(unit)) {
            mask |= 1 << (quad + 16);
        }
        
        SOC_REG_ABOVE_64_SET_WORD_PATTERN(above64_mask, mask);

        
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, prio_reg, REG_PORT_ANY, 0, reg_above_64_val));
        SOC_REG_ABOVE_64_OR(reg_above_64_val, above64_mask);
        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_set(unit, prio_reg, REG_PORT_ANY, 0, reg_above_64_val));

        
        SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, prio_reg1, REG_PORT_ANY, 0, &reg32_val));
        reg32_val &= clear_mask;
        SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, prio_reg1, REG_PORT_ANY, 0, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




int
soc_qax_wait_gtimer_trigger(int unit)
{
    int rv, counter;
    uint64 reg_val_64;
    uint32 gtimer_trigger_f;
    soc_reg_t reg;

    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_QUX(unit)) {
        reg = NIF_GTIMER_CONFIGURATIONr;
    } else {
        reg = NBIH_GTIMER_CONFIGURATIONr;
    }
    rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_reg64_field32_set(unit, reg, &reg_val_64, GTIMER_TRIGGERf, 0x0);
    rv = soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    sal_usleep(500000);

    soc_reg64_field32_set(unit, reg, &reg_val_64, GTIMER_TRIGGERf, 0x1);
    rv = soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);
    gtimer_trigger_f = soc_reg64_field32_get(unit, reg, reg_val_64, GTIMER_TRIGGERf);

    counter = 0;
    while (gtimer_trigger_f == 0x1) {
        sal_usleep(500000);
        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val_64);
        SOCDNX_IF_ERR_EXIT(rv);
        gtimer_trigger_f = soc_reg64_field32_get(unit, reg, reg_val_64, GTIMER_TRIGGERf);

        if(10 == counter){
            SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("soc_qax_wait_gtimer_trigger timeout")));
        }

        counter++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_qax_nif_ilkn_phys_aligned_pbmp_get(int unit, soc_port_t port, soc_pbmp_t *phys_aligned, int is_format_adjust)
{
    int *ilkn_lanes, lane_index;
    uint32 offset;
    soc_pbmp_t phy_pbmp, phy_ports;
    int ilkn_pmhl_lanes[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,29,30,31,32,45,46,47,48};
    int ilkn_pmlq_lanes[] = {17,18,19,20,21,22,23,24,25,26,27,28,33,34,35,36,37,38,39,40,41,42,43,44};
    SOCDNX_INIT_FUNC_DEFS;


    SOC_PBMP_CLEAR(*phys_aligned);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phy_ports, &phy_pbmp));

    ilkn_lanes = (offset < 2) ? ilkn_pmhl_lanes : ilkn_pmlq_lanes;
    for (lane_index = 0; lane_index < SOC_QAX_NIF_ILKN_MAX_NOF_LANES; ++lane_index) {
        if (SOC_PBMP_MEMBER(phy_pbmp, ilkn_lanes[lane_index])) {
            
            SOC_PBMP_PORT_ADD(*phys_aligned, (((offset & 1) && is_format_adjust) ? JER_NIF_ILKN_MAX_NOF_LANES - 1 - lane_index : lane_index));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nif_ilkn_pbmp_from_num_lanes_get(int unit ,uint32 num_lanes, uint32 first_phy, uint32 ilkn_id, soc_pbmp_t* pbmp)
{
    int lanes_to_add, ilkn_wrap_index, ilkn_wrap_pm=-1, ii;
    int first_phy_in_first_active_quad, num_lanes_first_quad;
    portmod_pm_identifier_t *ilkn_wrap;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*pbmp);

    if ((ilkn_id & 1) && num_lanes > 12) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d can't have more than 12 lanes"), ilkn_id));
    }

    ilkn_wrap_index = ilkn_id>>1; 
    ilkn_wrap = qax_ilkn_pm_table[ilkn_wrap_index];

    
    if (first_phy == SOC_QAX_FIRST_ILKN_PHY) {
        ilkn_wrap_pm = ((ilkn_id & 1) == 0) ? 0 : SOC_QAX_MAX_PMS_PER_ILKN_PORT/2; 
    } else {
        
        for (ii = 0; ii <= SOC_QAX_MAX_PMS_PER_ILKN_PORT; ii++) {
            if (first_phy >= ilkn_wrap[ii].phy && first_phy < ilkn_wrap[ii].phy + NUM_OF_LANES_IN_PM) {
                ilkn_wrap_pm = ii;
                break;
            }
        }

        if (ilkn_wrap_pm == -1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Can't fint relevant ilkn PM")));
        }

        
        first_phy_in_first_active_quad = (first_phy - 1) % NUM_OF_LANES_IN_PM;
        num_lanes_first_quad = NUM_OF_LANES_IN_PM - first_phy_in_first_active_quad;
        
        lanes_to_add = (num_lanes < num_lanes_first_quad) ? num_lanes : num_lanes_first_quad;
        SOC_PBMP_PORTS_RANGE_ADD(*pbmp, (ilkn_wrap[ilkn_wrap_pm].phy+first_phy_in_first_active_quad), lanes_to_add);
        ++ilkn_wrap_pm;
        num_lanes -= lanes_to_add;
    }

    while (num_lanes > 0)
    {
        lanes_to_add = (num_lanes < NUM_OF_LANES_IN_PM) ? num_lanes : NUM_OF_LANES_IN_PM;
        SOC_PBMP_PORTS_RANGE_ADD(*pbmp, ilkn_wrap[ilkn_wrap_pm].phy, lanes_to_add);

        ++ilkn_wrap_pm;
        num_lanes -= lanes_to_add;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys, soc_pbmp_t* src_pbmp)
{
    int i, lanes;
    soc_port_t phy;
    char* propval;
    char* propkey;
    int ilkn_wrap_index, ilkn_wrap_pm;
    soc_pbmp_t bm, phy_pbmp;
    portmod_pm_identifier_t *ilkn_wrap;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(phy_pbmp);

    propkey = spn_ILKN_LANES;
    propval = soc_property_port_get_str(unit, ilkn_id, propkey);
    
    if(src_pbmp != NULL) {
        BCM_PBMP_ASSIGN(bm,*src_pbmp);
    } else if (propval != NULL) {
        if (_shr_pbmp_decode(propval, &bm) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("failed to decode (\"%s\") for %s"), propval, propkey));
        }
    }

    ilkn_wrap_index = ilkn_id>>1; 
    ilkn_wrap = qax_ilkn_pm_table[ilkn_wrap_index];

    if((propval != NULL) || (src_pbmp != NULL)) {

        SOC_PBMP_COUNT(bm, lanes);
        if ((ilkn_id & 1) && lanes > 12) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d can't have more than 12 lanes"), ilkn_id));
        }

        SOC_PBMP_ITER(bm, i) {
            if(i >= JER_NIF_ILKN_MAX_NOF_LANES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("ILKN%d lane %d is out of range"), ilkn_id, i));
            }
            ilkn_wrap_pm = i / NUM_OF_LANES_IN_PM;
            phy = ilkn_wrap[ilkn_wrap_pm].phy + (i % NUM_OF_LANES_IN_PM);

            SOC_PBMP_PORT_ADD(phy_pbmp, phy);
        }

    } else {

        lanes = soc_property_port_get(unit, ilkn_id, spn_ILKN_NUM_LANES, 12);

        SOCDNX_IF_ERR_EXIT(soc_qax_nif_ilkn_pbmp_from_num_lanes_get(unit, lanes, SOC_QAX_FIRST_ILKN_PHY, ilkn_id, &phy_pbmp));

    }
    SOC_PBMP_ASSIGN(*phys, phy_pbmp);

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_qax_port_ilkn_bypass_interface_enable(int unit, int port, int enable) {

    soc_pbmp_t phys, phy_lanes;
    soc_port_t phy;
    int first_packet_sw_bypass;
    int shift;
    uint32 rst_nbih_lanes=0, rst_nbil0_lanes=0, rst_nbil1_lanes=0, reg_val;
    uint32 pml_base_lane    = SOC_DPP_DEFS_GET(unit, pml0_base_lane);
    uint32 sw_db_flags;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));

    SOC_PBMP_ITER(phy_lanes, phy){

       if (phy == 0)  
           continue;

       shift = (phy - 1) % SOC_QAX_NOF_LANES_PER_NBI;

       if (phy <= pml_base_lane)
       {
           rst_nbih_lanes  |= (1 << shift);  
       }
       else if (phy <= (pml_base_lane + SOC_QAX_NOF_LANES_PER_NBI))
       {
           rst_nbil0_lanes |= (1 << shift);  
       }
       else if (phy <= (pml_base_lane + (2 * SOC_QAX_NOF_LANES_PER_NBI)))
       {
           rst_nbil1_lanes |= (1 << shift);  
       }
    }

    
     first_packet_sw_bypass = SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_first_packet_sw_bypass;

     SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &sw_db_flags));

    if (rst_nbih_lanes != 0)
    {
        if ((!first_packet_sw_bypass) || (!enable) || SOC_PORT_IS_ELK_INTERFACE(sw_db_flags)){
            SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, &reg_val));
            reg_val = (enable) ? (reg_val | rst_nbih_lanes) : (reg_val & (~rst_nbih_lanes));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, reg_val));
        }
        SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, &reg_val));
        reg_val = (enable) ? (reg_val | rst_nbih_lanes) : (reg_val & (~rst_nbih_lanes));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, reg_val));
    }

    if (rst_nbil0_lanes != 0)
    {
        if ((!first_packet_sw_bypass) || (!enable) || SOC_PORT_IS_ELK_INTERFACE(sw_db_flags)){
            SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, &reg_val));
            reg_val = (enable) ? (reg_val | rst_nbil0_lanes) : (reg_val & (~rst_nbil0_lanes));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, reg_val));
        }
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 0, &reg_val));
        reg_val = (enable) ? (reg_val | rst_nbil0_lanes) : (reg_val & (~rst_nbil0_lanes));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 0, reg_val));
    }

    if (rst_nbil1_lanes != 0)
    {
        if ((!first_packet_sw_bypass) || (!enable) || SOC_PORT_IS_ELK_INTERFACE(sw_db_flags)){
            SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, &reg_val));
            reg_val = (enable) ? (reg_val | rst_nbil1_lanes) : (reg_val & (~rst_nbil1_lanes));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, reg_val));
        }
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 1, &reg_val));
        reg_val = (enable) ? (reg_val | rst_nbil1_lanes) : (reg_val & (~rst_nbil1_lanes));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 1, reg_val));
    }


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_port_ilkn_bypass_interface_rx_check_and_enable(int unit, int port) {

    soc_pbmp_t phys, phy_lanes;
    soc_port_t phy;
    int shift;
    uint32 rst_nbih_lanes=0, rst_nbil0_lanes=0, rst_nbil1_lanes=0, reg_val;
    uint32 pml_base_lane    = SOC_DPP_DEFS_GET(unit, pml0_base_lane);
    soc_field_t field[] = {TSC_LANE_STATUS_0__Nf,TSC_LANE_STATUS_1__Nf,TSC_LANE_STATUS_2__Nf,TSC_LANE_STATUS_3__Nf};
    int acc_phy_status = JER_NIF_TSC_LINK_IS_UP;
    int phy_was_checked = FALSE;
    int temp_phy_status = 0, phy_status, current_rx_status, user_request, need_to_set, reg_idx, field_idx;
    uint32 tx_reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));



    

    SOC_PBMP_ITER(phy_lanes, phy){

        if (phy == 0)  
            continue;

        shift = (phy - 1) % SOC_QAX_NOF_LANES_PER_NBI;

        if (phy <= pml_base_lane)
        {
            rst_nbih_lanes  |= (1 << shift);  
        }
        else if (phy <= (pml_base_lane + SOC_QAX_NOF_LANES_PER_NBI))
        {
            rst_nbil0_lanes |= (1 << shift);  
        }
        else if (phy <= (pml_base_lane + (2 * SOC_QAX_NOF_LANES_PER_NBI)))
        {
            rst_nbil1_lanes |= (1 << shift);  
        }

        
        field_idx = shift % NUM_OF_LANES_IN_PM;
        reg_idx = shift / NUM_OF_LANES_IN_PM;

        if (phy <= pml_base_lane)
        { 
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_NIF_TSC_LANE_STATUSr, REG_PORT_ANY, reg_idx, &reg_val));
            temp_phy_status =  soc_reg_field_get(unit, NBIH_NIF_TSC_LANE_STATUSr, reg_val, field[field_idx]);
        }
        else if (phy <= (pml_base_lane + SOC_QAX_NOF_LANES_PER_NBI))
        { 
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIL_NIF_TSC_LANE_STATUSr, 0, reg_idx, &reg_val));
            temp_phy_status =  soc_reg_field_get(unit, NBIL_NIF_TSC_LANE_STATUSr, reg_val, field[field_idx]);
        }
        else if (phy <= (pml_base_lane + (2 * SOC_QAX_NOF_LANES_PER_NBI)))
        {  
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIL_NIF_TSC_LANE_STATUSr, 1, reg_idx, &reg_val));
            temp_phy_status =  soc_reg_field_get(unit, NBIL_NIF_TSC_LANE_STATUSr, reg_val, field[field_idx]);
        }
        acc_phy_status &= temp_phy_status; 
        phy_was_checked = TRUE;
    }

    
    phy_status = (acc_phy_status == JER_NIF_TSC_LINK_IS_UP) && phy_was_checked;

    if (rst_nbih_lanes != 0)
    {
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_TX_RSTNr(unit, &tx_reg_val));
        user_request = ((rst_nbih_lanes & tx_reg_val) != 0);

        need_to_set = user_request && phy_status;

        
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, &reg_val));
        current_rx_status = ((rst_nbih_lanes & reg_val) == rst_nbih_lanes);

        if (current_rx_status != need_to_set){
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "ilkn_bypass_interface_rx_check_and_enable - NBIH: port = %d, phy_status = %d, user_request (nbih) = %d\n"), port, phy_status, user_request));
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "    rst_lanes (nbih) = %x, rx_reg_val (nbih) (before action taken) = %x, tx_reg_val (nbih) = %x\n"), rst_nbih_lanes, reg_val, tx_reg_val));
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "    acc_phy_status = %d, current_rx_status (nbih) = %d, need_to_set (nbih) = %d\n"), acc_phy_status, current_rx_status, need_to_set));
            reg_val = (need_to_set) ? (reg_val | rst_nbih_lanes) : (reg_val & (~rst_nbih_lanes));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, reg_val));
        }
    }

    if (rst_nbil0_lanes != 0)
    {
        
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 0, &tx_reg_val));
        user_request = ((rst_nbil0_lanes & tx_reg_val) != 0);

        need_to_set = user_request && phy_status;

        
        
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, &reg_val));
        current_rx_status = ((rst_nbil0_lanes & reg_val) == rst_nbil0_lanes);

        if (current_rx_status != need_to_set){
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "ilkn_bypass_interface_rx_check_and_enable - NBIL0: port = %d, phy_status = %d, user_request (nbil0) = %d\n"), port, phy_status, user_request));
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "    rst_lanes (nbil0) = %x, rx_reg_val (nbil0) (before action taken) = %x, tx_reg_val (nbil0) = %x\n"), rst_nbil0_lanes, reg_val, tx_reg_val));
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "    acc_phy_status = %d, current_rx_status (nbil0) = %d, need_to_set (nbil0) = %d\n"), acc_phy_status, current_rx_status, need_to_set));
            reg_val = (need_to_set) ? (reg_val | rst_nbil0_lanes) : (reg_val & (~rst_nbil0_lanes));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, reg_val));
        }
    }

    if (rst_nbil1_lanes != 0)
    {
        
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_TX_RSTNr(unit, 1, &tx_reg_val));
        user_request = ((rst_nbil1_lanes & tx_reg_val) != 0);

        need_to_set = user_request && phy_status;

        
        
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, &reg_val));
        current_rx_status = ((rst_nbil1_lanes & reg_val) == rst_nbil1_lanes);

        if (current_rx_status != need_to_set){
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "ilkn_bypass_interface_rx_check_and_enable - NBIL1: port = %d, phy_status = %d, user_request (nbil1) = %d\n"), port, phy_status, user_request));
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "    rst_lanes (nbil1) = %x, rx_reg_val (nbil1) (before action taken) = %x, tx_reg_val (nbil1) = %x\n"), rst_nbil1_lanes, reg_val, tx_reg_val));
            LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit, "    acc_phy_status = %d, current_rx_status (nbil1) = %d, need_to_set (nbil1) = %d\n"), acc_phy_status, current_rx_status, need_to_set));
            reg_val = (need_to_set) ? (reg_val | rst_nbil1_lanes) : (reg_val & (~rst_nbil1_lanes));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_port_ilkn_bypass_interface_rx_enable_get(int unit, int port, uint32* rx_enable) {

    soc_pbmp_t phys, phy_lanes;
    soc_port_t phy;
    int shift;
    uint32 rst_nbih_lanes=0, rst_nbil0_lanes=0, rst_nbil1_lanes=0, reg_val;
    uint32 pml_base_lane    = SOC_DPP_DEFS_GET(unit, pml0_base_lane);
    int current_rx_status_nbih = 1, current_rx_status_nbil0 = 1, current_rx_status_nbil1 = 1;
    uint32 nof_lanes;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(rx_enable);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &phys, &phy_lanes));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &nof_lanes));

    SOC_PBMP_ITER(phy_lanes, phy){

        if (phy == 0)  
            continue;

        shift = (phy - 1) % SOC_QAX_NOF_LANES_PER_NBI;

        if (phy <= pml_base_lane)
        {
            rst_nbih_lanes  |= (1 << shift);  
        }
        else if (phy <= (pml_base_lane + SOC_QAX_NOF_LANES_PER_NBI))
        {
            rst_nbil0_lanes |= (1 << shift);  
        }
        else if (phy <= (pml_base_lane + (2 * SOC_QAX_NOF_LANES_PER_NBI)))
        {
            rst_nbil1_lanes |= (1 << shift);  
        }
    }

    if (rst_nbih_lanes != 0)
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PM_ILKN_RX_RSTNr(unit, &reg_val));
        current_rx_status_nbih = ((rst_nbih_lanes & reg_val) == rst_nbih_lanes);
    }

    if (rst_nbil0_lanes != 0)
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 0, &reg_val));
        current_rx_status_nbil0 = ((rst_nbil0_lanes & reg_val) == rst_nbil0_lanes);
    }

    if (rst_nbil1_lanes != 0)
    {
        SOCDNX_IF_ERR_EXIT(READ_NBIL_NIF_PM_ILKN_RX_RSTNr(unit, 1, &reg_val));
        current_rx_status_nbil1 = ((rst_nbil1_lanes & reg_val) == rst_nbil1_lanes);
    }

    
    if (nof_lanes > 0) {
        *rx_enable = current_rx_status_nbih && current_rx_status_nbil0 && current_rx_status_nbil1;
    }
    else {
        *rx_enable = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp)
{
    int first_phy;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*phy_pbmp);
    if(id < 48) {
        first_phy = id  + 17;
    } else {
        first_phy = id - 48 + 69;
    }

    SOC_PBMP_PORT_ADD(*phy_pbmp, first_phy);

    SOCDNX_FUNC_RETURN;
}

int
soc_qax_nif_sif_set(int unit, uint32 first_phy)
{
    int port_num, pm_select = 0, nbil_index;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
        if(first_phy != QUX_NIF_PHY_SIF_PORT_NBI)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid phy port configured as statistics interface"), first_phy));
        }
        SOCDNX_IF_ERR_EXIT(READ_NIF_SIF_CFGr(unit, &reg_val));
        soc_reg_field_set(unit, NIF_SIF_CFGr, &reg_val, SIF_PORT_ENf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NIF_SIF_CFGr(unit, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NIF_ADDITIONAL_RESETSr(unit, &reg_val));
        soc_reg_field_set(unit, NIF_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NIF_ADDITIONAL_RESETSr(unit, reg_val));
    } else {
        if (first_phy == QAX_NIF_PHY_SIF_PORT_NBIL0)
        {
            nbil_index = 0;
            port_num = 0;
            pm_select = 0;
        }
        else if (first_phy == QAX_NIF_PHY_SIF_PORT_NBIL1)
        {
            nbil_index = 1;
            port_num = 0;
            pm_select = 0;
        }
        else
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid phy port configured as statistics interface"), first_phy));
        }

        SOCDNX_IF_ERR_EXIT(READ_NBIL_SIF_CFGr(unit, nbil_index, port_num, &reg_val));
        soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_ENf, 1);
        soc_reg_field_set(unit, NBIL_SIF_CFGr, &reg_val, SIF_PORT_N_PM_SELf, pm_select);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_SIF_CFGr(unit, nbil_index, port_num, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, nbil_index, &reg_val));
        soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, nbil_index, reg_val));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 qax_prd_enable_hrf_mask[] = {
    1 << 16,
    1 << 16,
    1 << 17,
    1 << 16,
};

int
soc_qax_port_prd_enable_set(int unit, soc_port_t port, uint32 flags, int enable)
{
    int rv, nbil_reg_port = 0;
    soc_reg_above_64_val_t reg_prd_config;
    uint32 reg_val, field, prd_mode = 0, is_hg, en_mask = 0, offset = 0;
    SOC_TMC_PORT_HEADER_TYPE hdr_type;
    soc_reg_t reg_name = 0;
    soc_field_t field_name = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_enable_set(unit, port, flags, enable));
    }
    else {
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &hdr_type);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_get(unit, port, &is_hg));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;
        en_mask = qax_prd_enable_hrf_mask[offset];

        if (enable) { 
            if (is_hg) {
                prd_mode = QAX_SOC_PRD_MODE_HIGIG;
            }
            else {
                switch(hdr_type)
                {
                case SOC_TMC_PORT_HEADER_TYPE_ETH:
                    prd_mode = QAX_SOC_PRD_MODE_VLAN;
                    break;

                case SOC_TMC_PORT_HEADER_TYPE_TM:
                    prd_mode = QAX_SOC_PRD_MODE_ITMH;
                    break;

                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported header type\n")));
                }
            }
            if (SOC_QAX_IS_ILKN24(offset))
            {
                reg_name = (offset == 0) ? NBIH_PRD_CONFIG_HRF_0r : NBIH_PRD_CONFIG_HRF_2r;
                field_name = (offset == 0) ? PRD_PKT_TYPE_HRF_0f : PRD_PKT_TYPE_HRF_2f;

                SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_ENr(unit, &reg_val));
                field =  soc_reg_field_get(unit, NBIH_PRD_ENr, reg_val, PRD_ENf);
                soc_reg_field_set(unit, NBIH_PRD_ENr, &reg_val, PRD_ENf, field | en_mask);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_ENr(unit, reg_val));

                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
                soc_reg_above_64_field32_set(unit, reg_name, reg_prd_config, field_name, prd_mode);
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));

            }
            else if (SOC_QAX_IS_ILKN12(offset))
            {
                SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, nbil_reg_port, &reg_val));
                field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
                soc_reg_field_set(unit, NBIL_PRD_ENr, &reg_val, PRD_ENf, field | en_mask);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_ENr(unit, nbil_reg_port, reg_val));

                SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
                soc_reg_above_64_field32_set(unit, NBIL_PRD_CONFIG_HRF_0r, reg_prd_config, PRD_PKT_TYPE_HRF_0f, prd_mode);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
            }
        }
        else {
            if (SOC_QAX_IS_ILKN24(offset))
            {
                SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_ENr(unit, &reg_val));
                field =  soc_reg_field_get(unit, NBIH_PRD_ENr, reg_val, PRD_ENf);
                soc_reg_field_set(unit, NBIH_PRD_ENr, &reg_val, PRD_ENf, field & ~en_mask);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_PRD_ENr(unit, reg_val));
            }
            else if (SOC_QAX_IS_ILKN12(offset))
            {
                SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, nbil_reg_port, &reg_val));
                field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
                soc_reg_field_set(unit, NBIL_PRD_ENr, &reg_val, PRD_ENf, field & ~en_mask);
                SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_ENr(unit, nbil_reg_port, reg_val));
            }
        }
    }

    exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_enable_get(int unit, soc_port_t port, uint32 flags, int *enable)
{
    int nbil_reg_port = 0;
    uint32 reg_val, field = 0, en_mask = 0, offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_enable_get(unit, port, flags, enable));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;
        en_mask = qax_prd_enable_hrf_mask[offset];

        if (SOC_QAX_IS_ILKN24(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_ENr(unit, &reg_val));
            field =  soc_reg_field_get(unit, NBIH_PRD_ENr, reg_val, PRD_ENf);
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_ENr(unit, nbil_reg_port, &reg_val));
            field =  soc_reg_field_get(unit, NBIL_PRD_ENr, reg_val, PRD_ENf);
        }
        *enable = (field & en_mask) ? 1 : 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_config_set(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config)
{
    int nbil_reg_port = 0;
    soc_reg_above_64_val_t reg_prd_config;
    uint32 offset = 0, untagged_pcp = 0;
    soc_reg_t reg_name = 0;
    soc_field_t field_name = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_config_set(unit, port, flags, config));
    }
    else {
        untagged_pcp = config->untagged_pcp;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;

        if (SOC_QAX_IS_ILKN24(offset))
        {
            reg_name = (offset == 0) ? NBIH_PRD_CONFIG_HRF_0r : NBIH_PRD_CONFIG_HRF_2r;
            field_name = (offset == 0) ? PRD_UNTAG_PCP_HRF_0f : PRD_UNTAG_PCP_HRF_2f;

            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
            soc_reg_above_64_field32_set(unit, reg_name, reg_prd_config, field_name, untagged_pcp);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
            soc_reg_above_64_field32_set(unit, NBIL_PRD_CONFIG_HRF_0r, reg_prd_config, PRD_UNTAG_PCP_HRF_0f, untagged_pcp);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_config_get(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config)
{
    int nbil_reg_port = 0;
    soc_reg_above_64_val_t reg_prd_config;
    uint32 untagged_pcp = 0, offset = 0;
    soc_reg_t reg_name = 0;
    soc_field_t field_name = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_config_get(unit, port, flags, config));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;

        if (SOC_QAX_IS_ILKN24(offset))
        {
            reg_name = (offset == 0) ? NBIH_PRD_CONFIG_HRF_0r : NBIH_PRD_CONFIG_HRF_2r;
            field_name = (offset == 0) ? PRD_UNTAG_PCP_HRF_0f : PRD_UNTAG_PCP_HRF_2f;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
            untagged_pcp = soc_reg_above_64_field32_get(unit, reg_name, reg_prd_config, field_name);
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
            untagged_pcp = soc_reg_above_64_field32_get(unit, NBIL_PRD_CONFIG_HRF_0r, reg_prd_config, PRD_UNTAG_PCP_HRF_0f);
        }

        config->untagged_pcp = untagged_pcp;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_threshold_set(int unit, soc_port_t port, uint32 flags, int priority, uint32 value)
{
    int nbil_reg_port = 0;
    uint64 reg_val64;
    uint32 field, offset = 0;
    soc_reg_t reg_name = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_threshold_set(unit, port, flags, priority, value));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;

        if (SOC_QAX_IS_ILKN24(offset))
        {
            reg_name = (offset == 0) ? NBIH_HRF_RX_PRD_THRESHOLDS_CONFIG_0r : NBIH_HRF_RX_PRD_THRESHOLDS_CONFIG_2r;
            if (offset == 0) {
                field = (priority == 0) ? HRF_RX_PRD_THRESHOLD_0_HRF_0f : \
                    ((priority == 1) ? HRF_RX_PRD_THRESHOLD_1_HRF_0f : HRF_RX_PRD_THRESHOLD_2_HRF_0f);
            }
            else {
                field = (priority == 0) ? HRF_RX_PRD_THRESHOLD_0_HRF_2f : \
                    ((priority == 1) ? HRF_RX_PRD_THRESHOLD_1_HRF_2f : HRF_RX_PRD_THRESHOLD_2_HRF_2f);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg_name, REG_PORT_ANY, 0, &reg_val64));
            soc_reg64_field32_set(unit, reg_name, &reg_val64, field, value);
            SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, reg_name, REG_PORT_ANY, 0, reg_val64));
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            field = (priority == 0) ? HRF_RX_PRD_THRESHOLD_0_HRF_Nf : \
                ((priority == 1) ? HRF_RX_PRD_THRESHOLD_1_HRF_Nf : HRF_RX_PRD_THRESHOLD_2_HRF_Nf);
            SOCDNX_IF_ERR_EXIT(READ_NBIL_HRF_RX_PRD_THRESHOLDS_CONFIGr(unit, nbil_reg_port, &reg_val64));
            soc_reg64_field32_set(unit, NBIL_HRF_RX_PRD_THRESHOLDS_CONFIGr, &reg_val64, field, value);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_HRF_RX_PRD_THRESHOLDS_CONFIGr(unit, nbil_reg_port, reg_val64));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_threshold_get(int unit, soc_port_t port, uint32 flags, int priority, uint32 *value)
{
    int nbil_reg_port = 0;
    uint64 reg_val64;
    uint32 field, th_value = 0, offset = 0;
    soc_reg_t reg_name = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_threshold_get(unit, port, flags, priority, value));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;

        if (SOC_QAX_IS_ILKN24(offset))
        {
            reg_name = (offset == 0) ? NBIH_HRF_RX_PRD_THRESHOLDS_CONFIG_0r : NBIH_HRF_RX_PRD_THRESHOLDS_CONFIG_2r;
            if (offset == 0) {
                field = (priority == 0) ? HRF_RX_PRD_THRESHOLD_0_HRF_0f : \
                    ((priority == 1) ? HRF_RX_PRD_THRESHOLD_1_HRF_0f : HRF_RX_PRD_THRESHOLD_2_HRF_0f);
            }
            else {
                field = (priority == 0) ? HRF_RX_PRD_THRESHOLD_0_HRF_2f : \
                    ((priority == 1) ? HRF_RX_PRD_THRESHOLD_1_HRF_2f : HRF_RX_PRD_THRESHOLD_2_HRF_2f);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, reg_name, REG_PORT_ANY, 0, &reg_val64));
            th_value = soc_reg64_field32_get(unit, reg_name, reg_val64, field);
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            field = (priority == 0) ? HRF_RX_PRD_THRESHOLD_0_HRF_Nf : \
                ((priority == 1) ? HRF_RX_PRD_THRESHOLD_1_HRF_Nf : HRF_RX_PRD_THRESHOLD_2_HRF_Nf);
            SOCDNX_IF_ERR_EXIT(READ_NBIL_HRF_RX_PRD_THRESHOLDS_CONFIGr(unit, nbil_reg_port, &reg_val64));
            th_value = soc_reg64_field32_get(unit, NBIL_HRF_RX_PRD_THRESHOLDS_CONFIGr, reg_val64, field);
        }
        *value = th_value;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_map_set(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int priority)
{
    int nbil_reg_port = 0;
    uint32 offset = 0;
    soc_reg_above_64_val_t reg_prd_config;
    int key_tbl_index;
    soc_reg_above_64_val_t prio_map;
    soc_reg_t reg_name = 0;
    soc_field_t field_name = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if ((priority < SOC_DPP_COSQ_PORT_PRIORITY_MIN) || (priority > SOC_DPP_COSQ_PORT_PRIORITY_MAX)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Priority %d is invalid"), priority));
    }
    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_map_set(unit, port, flags, map, key, priority));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;
        key_tbl_index = key * QAX_SOC_PRD_MAP_BITS_PER_PRIORITY;

        switch (map) {
            case socDppPrdEthPcpDeiToPriorityTable:
                if (key > SOC_QAX_PRD_ETH_MAP_MAX_VAL) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Key %d is invalid."), key));
                }
                break;
            case socDppPrdTmTcDpPriorityTable:
                if (key > SOC_QAX_PRD_TM_MAP_MAX_VAL) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Key %d is invalid."), key));
                }
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Priority drop map %d is invalid"), map));
                break;
        }

        if (SOC_QAX_IS_ILKN24(offset))
        {
            reg_name = (offset == 0) ? NBIH_PRD_CONFIG_HRF_0r : NBIH_PRD_CONFIG_HRF_2r;
            field_name = (offset == 0) ? PRD_PRIO_MAP_HRF_0f : PRD_PRIO_MAP_HRF_2f;

            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
            soc_reg_above_64_field_get(unit, reg_name, reg_prd_config, field_name, prio_map);
            SOC_REG_ABOVE_64_RANGE_COPY(prio_map, key_tbl_index, (uint32*)&priority, 0, QAX_SOC_PRD_MAP_BITS_PER_PRIORITY);
            soc_reg_above_64_field_set(unit, reg_name, reg_prd_config, field_name, prio_map);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
            soc_reg_above_64_field_get(unit, NBIL_PRD_CONFIG_HRF_0r, reg_prd_config, PRD_PRIO_MAP_HRF_0f, prio_map);
            SOC_REG_ABOVE_64_RANGE_COPY(prio_map, key_tbl_index, (uint32*)&priority, 0, QAX_SOC_PRD_MAP_BITS_PER_PRIORITY);
            soc_reg_above_64_field_set(unit, NBIL_PRD_CONFIG_HRF_0r, reg_prd_config, PRD_PRIO_MAP_HRF_0f, prio_map);
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_map_get(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int *priority)
{
    int nbil_reg_port = 0;
    uint32 offset = 0;
    soc_reg_above_64_val_t reg_prd_config;
    int key_tbl_index;
    soc_reg_above_64_val_t prio_map;
    soc_reg_t reg_name = 0;
    soc_field_t field_name = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *priority = 0;
    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_map_get(unit, port, flags, map, key, priority));
    }
    else {
        switch (map) {
            case socDppPrdEthPcpDeiToPriorityTable:
                if (key > SOC_QAX_PRD_ETH_MAP_MAX_VAL) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Key %d is invalid."), key));
                }
                break;
            case socDppPrdTmTcDpPriorityTable:
                if (key > SOC_QAX_PRD_TM_MAP_MAX_VAL) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Key %d is invalid."), key));
                }
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Priority drop map %d is invalid"), map));
                break;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbil_reg_port = offset >> 1;

        if (SOC_QAX_IS_ILKN24(offset))
        {
            reg_name = (offset == 0) ? NBIH_PRD_CONFIG_HRF_0r : NBIH_PRD_CONFIG_HRF_2r;
            field_name = (offset == 0) ? PRD_PRIO_MAP_HRF_0f : PRD_PRIO_MAP_HRF_2f;

            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, REG_PORT_ANY, 0, reg_prd_config));
            soc_reg_above_64_field_get(unit, reg_name, reg_prd_config, field_name, prio_map);
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_CONFIG_HRF_0r(unit, nbil_reg_port, reg_prd_config));
            soc_reg_above_64_field_get(unit, NBIL_PRD_CONFIG_HRF_0r, reg_prd_config, PRD_PRIO_MAP_HRF_0f, prio_map);
        }
        key_tbl_index = key * QAX_SOC_PRD_MAP_BITS_PER_PRIORITY;
        SOC_REG_ABOVE_64_RANGE_COPY((uint32*)priority, 0, prio_map, key_tbl_index, QAX_SOC_PRD_MAP_BITS_PER_PRIORITY);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_prd_drop_count_get(int unit, soc_port_t port, uint64 *count)
{
    int nbil_reg_port = 0, nbih_reg_port = 0;
    uint32 reg_val, offset = 0;
    uint32 cnt32bit;
    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_prd_drop_count_get(unit, port, count));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        nbih_reg_port = nbil_reg_port = offset >> 1;

        if (SOC_QAX_IS_ILKN24(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_PKT_DROP_CNT_HRFr_REG32(unit, nbih_reg_port, &reg_val));
            cnt32bit = soc_reg_field_get(unit, NBIH_PRD_PKT_DROP_CNT_HRFr, reg_val, PRD_PKT_DROP_CNT_HRFf);
            COMPILER_64_SET(*count, 0, cnt32bit);
        }
        else if (SOC_QAX_IS_ILKN12(offset))
        {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_PKT_DROP_CNT_HRF_0r(unit, nbil_reg_port, &reg_val));
            cnt32bit = soc_reg_field_get(unit, NBIL_PRD_PKT_DROP_CNT_HRF_0r, reg_val, PRD_PKT_DROP_CNT_HRF_0f);
            COMPILER_64_SET(*count, 0, cnt32bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


static int
soc_qax_nif_sku_restricted_lane_find(int unit, uint32 phy_lane, soc_pbmp_t phy_pbmp, uint8 *restricted_lane_configured){

    int quad;
    int lane_in_quad;
    uint32 next_lane;
    uint32 phy_port;
    uint32 next_lane_port;
    uint32 lanes_to_restrict[2];
    soc_port_t port;
    soc_qax_direction_t direction;
    phymod_lane_map_t lane_map;

    SOCDNX_INIT_FUNC_DEFS;

    *restricted_lane_configured=FALSE;

    
    quad = (phy_lane-1)/NUM_OF_LANES_IN_PM;
    
    lane_in_quad = (phy_lane-1)%NUM_OF_LANES_IN_PM;
    
    SOCDNX_IF_ERR_EXIT(soc_jer_lane_map_get(unit, quad, &lane_map));
    
    lanes_to_restrict[SOC_QAX_DIRECTION_RX] = (quad * NUM_OF_LANES_IN_PM) + lane_map.lane_map_rx[lane_in_quad] + 1;
    
    lanes_to_restrict[SOC_QAX_DIRECTION_TX] = (quad * NUM_OF_LANES_IN_PM) + lane_map.lane_map_tx[lane_in_quad] + 1;

    for (direction=SOC_QAX_DIRECTION_FIRST; direction < SOC_QAX_NOF_DIRECTIONS; direction++)
    {
        
        SOCDNX_IF_ERR_EXIT(soc_qax_qsgmii_offsets_add(unit, lanes_to_restrict[direction], &phy_port));

        
        if (lanes_to_restrict[direction] < (SOC_QAX_NOF_LANES-1)){
            next_lane = lanes_to_restrict[direction] + 1;
            SOCDNX_IF_ERR_EXIT(soc_qax_qsgmii_offsets_add(unit, next_lane, &next_lane_port));
        } else {
            next_lane_port=phy_port+1;
        }

        
        for (port=phy_port; port < next_lane_port; port++)
        {
            if(SOC_PBMP_MEMBER(phy_pbmp, port)) {
                
                *restricted_lane_configured=TRUE;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_qax_nif_sku_restrictions(int unit, soc_pbmp_t phy_pbmp, soc_port_if_t interface, uint32 protocol_offset, uint32 is_kbp){

        uint32 flags,is_valid;
        uint8 restricted_lane_configured;
        soc_port_t port;

        SOCDNX_INIT_FUNC_DEFS;

        

        
        if (dcmn_device_block_for_feature(unit,DCMN_QAX_NIF_10_32_FEATURE)) {

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 11, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 10 and 11 of Quad 2 and Quad 3."),
                                                    soc_dev_name(unit)));
           }

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 12, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 10 and 11 of Quad 2 and Quad 3."),
                                                    soc_dev_name(unit)));
           }

           
           if(SOC_PBMP_MEMBER(phy_pbmp, 13) ||
              SOC_PBMP_MEMBER(phy_pbmp, 14) ||
              SOC_PBMP_MEMBER(phy_pbmp, 15) ||
              SOC_PBMP_MEMBER(phy_pbmp, 16)) {
              SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 10 and 11 of Quad 2 and Quad 3."),
                                                soc_dev_name(unit)));
           }
        }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QAX_NIF_12_24_FEATURE)) {

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 19, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 18 and 19 of QUAD 4, lanes 44 and 47 of QUAD 11, QUAD 3 and QUAD 7."),
                                                    soc_dev_name(unit)));
           }

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 20, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 18 and 19 of QUAD 4, lanes 44 and 47 of QUAD 11, QUAD 3 and QUAD 7."),
                                                    soc_dev_name(unit)));
           }

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 45, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 18 and 19 of QUAD 4, lanes 44 and 47 of QUAD 11, QUAD 3 and QUAD 7."),
                                                    soc_dev_name(unit)));
           }

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 48, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 18 and 19 of QUAD 4, lanes 44 and 47 of QUAD 11, QUAD 3 and QUAD 7."),
                                                    soc_dev_name(unit)));
           }

          
          if(SOC_PBMP_MEMBER(phy_pbmp, 13) ||
             SOC_PBMP_MEMBER(phy_pbmp, 14) ||
             SOC_PBMP_MEMBER(phy_pbmp, 15) ||
             SOC_PBMP_MEMBER(phy_pbmp, 16) ||
             SOC_PBMP_MEMBER(phy_pbmp, 65) ||
             SOC_PBMP_MEMBER(phy_pbmp, 66) ||
             SOC_PBMP_MEMBER(phy_pbmp, 67) ||
             SOC_PBMP_MEMBER(phy_pbmp, 68)) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 18 and 19 of QUAD 4, lanes 44 and 47 of QUAD 11, QUAD 3 and QUAD 7."),
                                               soc_dev_name(unit)));
          }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QAX_NIF_16_24_FEATURE)) {

          
          if(SOC_PBMP_MEMBER(phy_pbmp, 65) ||
             SOC_PBMP_MEMBER(phy_pbmp, 66) ||
             SOC_PBMP_MEMBER(phy_pbmp, 67) ||
             SOC_PBMP_MEMBER(phy_pbmp, 68) ||
             SOC_PBMP_MEMBER(phy_pbmp, 117) ||
             SOC_PBMP_MEMBER(phy_pbmp, 118) ||
             SOC_PBMP_MEMBER(phy_pbmp, 119) ||
             SOC_PBMP_MEMBER(phy_pbmp, 120)) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to Quad 7 and Quad 11."),
                                               soc_dev_name(unit)));
          }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QAX_3_CAUI4_PORTS_FEATURE)) {
           if (interface == SOC_PORT_IF_CAUI4 &&
               protocol_offset >= 3) {
               SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can use CAUI4 0-2 only."),
                                                  soc_dev_name(unit)));
           }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QAX_0_CAUI4_PORTS_FEATURE)) {
           if (interface == SOC_PORT_IF_CAUI4) {
               SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't use CAUI4."),
                                                  soc_dev_name(unit)));
           }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QAX_NO_STAT_FEATURE)) {
           SOC_PBMP_ITER(phy_pbmp, port) {
               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
               if (is_valid) {
                   SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
                   if (SOC_PORT_IS_STAT_INTERFACE(flags)){
                       SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't use STAT interface."),
                                                         soc_dev_name(unit)));
                   }
               }
           }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QUX_NIF_24_6_FEATURE)) {

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 7, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 6 and 7 of QUAD 1 and QUAD 8."),
                                                    soc_dev_name(unit)));
           }

           
           SOCDNX_IF_ERR_EXIT(soc_qax_nif_sku_restricted_lane_find(unit, 8, phy_pbmp, &restricted_lane_configured));
           if(restricted_lane_configured==TRUE){
                  SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 6 and 7 of QUAD 1 and QUAD 8."),
                                                    soc_dev_name(unit)));
           }

          
              if(SOC_PBMP_MEMBER(phy_pbmp, 33) ||
             SOC_PBMP_MEMBER(phy_pbmp, 34) ||
             SOC_PBMP_MEMBER(phy_pbmp, 35) ||
             SOC_PBMP_MEMBER(phy_pbmp, 36)) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to lanes 6 and 7 of QUAD 1 and QUAD 8."),
                                               soc_dev_name(unit)));
          }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QUX_NIF_12_4_FEATURE)) {

           
          if(SOC_PBMP_MEMBER(phy_pbmp, 5) ||
             SOC_PBMP_MEMBER(phy_pbmp, 6) ||
             SOC_PBMP_MEMBER(phy_pbmp, 7) ||
             SOC_PBMP_MEMBER(phy_pbmp, 8) ||
             SOC_PBMP_MEMBER(phy_pbmp, 21) ||
             SOC_PBMP_MEMBER(phy_pbmp, 22) ||
             SOC_PBMP_MEMBER(phy_pbmp, 23) ||
             SOC_PBMP_MEMBER(phy_pbmp, 24) ||
             SOC_PBMP_MEMBER(phy_pbmp, 25) ||
             SOC_PBMP_MEMBER(phy_pbmp, 26) ||
             SOC_PBMP_MEMBER(phy_pbmp, 27) ||
             SOC_PBMP_MEMBER(phy_pbmp, 28) ||
             SOC_PBMP_MEMBER(phy_pbmp, 29) ||
             SOC_PBMP_MEMBER(phy_pbmp, 30) ||
             SOC_PBMP_MEMBER(phy_pbmp, 31) ||
             SOC_PBMP_MEMBER(phy_pbmp, 32) ||
             SOC_PBMP_MEMBER(phy_pbmp, 33) ||
             SOC_PBMP_MEMBER(phy_pbmp, 34) ||
             SOC_PBMP_MEMBER(phy_pbmp, 35) ||
             SOC_PBMP_MEMBER(phy_pbmp, 36)) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to QUAD 1, QUAD 5, QUAD 6, QUAD 7 and QUAD 8."),
                                               soc_dev_name(unit)));
          }
       }

       
       if (dcmn_device_block_for_feature(unit,DCMN_QUX_NIF_12_2_FEATURE)) {

           
          if(SOC_PBMP_MEMBER(phy_pbmp, 3) ||
             SOC_PBMP_MEMBER(phy_pbmp, 4) ||
             SOC_PBMP_MEMBER(phy_pbmp, 5) ||
             SOC_PBMP_MEMBER(phy_pbmp, 6) ||
             SOC_PBMP_MEMBER(phy_pbmp, 7) ||
             SOC_PBMP_MEMBER(phy_pbmp, 8) ||
             SOC_PBMP_MEMBER(phy_pbmp, 21) ||
             SOC_PBMP_MEMBER(phy_pbmp, 22) ||
             SOC_PBMP_MEMBER(phy_pbmp, 23) ||
             SOC_PBMP_MEMBER(phy_pbmp, 24) ||
             SOC_PBMP_MEMBER(phy_pbmp, 25) ||
             SOC_PBMP_MEMBER(phy_pbmp, 26) ||
             SOC_PBMP_MEMBER(phy_pbmp, 27) ||
             SOC_PBMP_MEMBER(phy_pbmp, 28) ||
             SOC_PBMP_MEMBER(phy_pbmp, 29) ||
             SOC_PBMP_MEMBER(phy_pbmp, 30) ||
             SOC_PBMP_MEMBER(phy_pbmp, 31) ||
             SOC_PBMP_MEMBER(phy_pbmp, 32) ||
             SOC_PBMP_MEMBER(phy_pbmp, 33) ||
             SOC_PBMP_MEMBER(phy_pbmp, 34) ||
             SOC_PBMP_MEMBER(phy_pbmp, 35) ||
             SOC_PBMP_MEMBER(phy_pbmp, 36)) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Device %s can't be mapped to QUAD 0 lane3&4, QUAD 1, QUAD 5, QUAD 6, QUAD 7 and QUAD 8."),
                                               soc_dev_name(unit)));
          }
       }
exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_qax_port_speed_sku_restrictions(int unit, soc_port_t port, int speed){

    soc_pbmp_t quad_bmp;
    soc_port_if_t interface_type;
    bcm_port_t index;
    int is_ilkn_over_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if (dcmn_device_block_for_feature(unit, DCMN_QAX_LOW_FALCON_SPEED_FEATURE)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_portmod_port_quad_get(unit, port, &quad_bmp, &is_ilkn_over_fabric));
        
        BCM_PBMP_ITER(quad_bmp, index) {
            if (index>=0 && index<=2) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit,  port, &interface_type));
                if ((interface_type==_SHR_PORT_IF_CAUI)||(interface_type==_SHR_PORT_IF_XLAUI2)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("These interfaces (XLGE2,CGE) are not supported by this device.")));
                } else if ((interface_type==_SHR_PORT_IF_ILKN)||(interface_type==_SHR_PORT_IF_XFI)) {
                    if (speed>12500) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("This speed is not available on this QUAD.")));
                    }
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}
int
soc_qux_port_pll_type_get(int unit, soc_port_t port, SOC_QUX_NIF_PLL_TYPE *pll_type)
{
    uint32 first_phy_port;
    uint32 pll_type_pml_last_phy_lane = SOC_DPP_DEFS_GET(unit, pll_type_pml_last_phy_lane);
    uint32 pll_type_pmx_last_phy_lane = SOC_DPP_DEFS_GET(unit, pll_type_pmx_last_phy_lane);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port ));

    first_phy_port--; 

    if (first_phy_port <= pll_type_pml_last_phy_lane) {
        *pll_type = SOC_QUX_NIF_PLL_TYPE_PML;
    } else if (first_phy_port > pll_type_pml_last_phy_lane && first_phy_port <= pll_type_pmx_last_phy_lane) {
        *pll_type = SOC_QUX_NIF_PLL_TYPE_PMX;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid phy port %d"), first_phy_port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_led_pmlq_bypass_config_adjust(int unit, soc_port_t port, int add) {
    soc_pbmp_t quad_ports,port_pbmp;
    int quad;
    uint32 physical_phy,first_phy_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove,
    (unit, first_phy_port , &physical_phy)));
    --physical_phy;
    quad = physical_phy / NUM_OF_LANES_IN_PM;

    if ( add ) {
        SOCDNX_IF_ERR_EXIT(soc_qax_nbil_phy_led_pmlq_bypass_enable(unit, quad, 1));
    } else {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_port_ports_to_same_quad_get(unit, port, &quad_ports));
        SOC_PBMP_CLEAR(port_pbmp);
        SOC_PBMP_PORT_ADD(port_pbmp, port);
        if (SOC_PBMP_EQ(port_pbmp, quad_ports)) {
            SOCDNX_IF_ERR_EXIT(soc_qax_nbil_phy_led_pmlq_bypass_enable(unit, quad, 0));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME
