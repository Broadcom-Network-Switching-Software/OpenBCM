/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_nif_prd.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT

#include <shared/bsl.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/port.h>
#include <soc/dpp/JER/jer_nif_prd.h>
#include <soc/dpp/JER/jer_nif.h>

#define SOC_JER_PLUS_NOF_QMLFS_PER_NBI                      (6)
#define SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF         (4)
#define SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_ILKN_PORT    (4)

STATIC int
soc_jer_plus_first_phy_index_get(int unit, int port, int* first_phy)
{
    int phy;
    soc_pbmp_t nif_ports, phys;
    SOCDNX_INIT_FUNC_DEFS;

    *first_phy=-1;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        *first_phy = (phy-1);
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_qmlf_index_get(int unit, int port, int first_phy, uint32* qmlf_index)
{
    uint32 nof_lanes_nbi;
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    *qmlf_index = (first_phy % nof_lanes_nbi) / NUM_OF_LANES_IN_PM;

    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_enable_set(int unit, int port, uint32 en_mask)
{
    int first_phy;
    uint32 nof_lanes_nbi;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);

    if (first_phy < nof_lanes_nbi)  
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_PRD_ENr, REG_PORT_ANY, 0, en_mask));
    }
    else 
    {
        int reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIL_PRD_ENr, reg_port, 0, en_mask));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_enable_get(int unit, int port, uint32 *en_mask)
{
    int first_phy;
    uint32 nof_lanes_nbi;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);

    if (first_phy < nof_lanes_nbi)  
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_PRD_ENr, REG_PORT_ANY, 0, en_mask));
    }
    else 
    {
        int reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIL_PRD_ENr, reg_port, 0, en_mask));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_config_set(int unit, int port, soc_jer_nif_prd_config_t* prd_config)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_t reg;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi)  
    {
        reg = NBIH_PRD_CONFIGr;
        reg_port = REG_PORT_ANY;
    }
    else 
    {
        reg = NBIL_PRD_CONFIGr;
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, qmlf_index, &reg32_val));
    soc_reg_field_set(unit, reg, &reg32_val, PRD_NEVER_DROP_CONTROL_FRAMES_QMLF_Nf, prd_config->never_drop_control_frames);
    soc_reg_field_set(unit, reg, &reg32_val, PRD_SEPARATE_TDM_AND_NON_TMD_FOR_ETH_QMLF_Nf, prd_config->seperate_tdm_and_non_tdm);
    soc_reg_field_set(unit, reg, &reg32_val, PRD_SEPARATE_TDM_AND_NON_TMD_FROM_TWO_PORTS_QMLF_Nf, prd_config->seperate_tdm_and_non_tdm_two_ports);
    soc_reg_field_set(unit, reg, &reg32_val, PRD_NEVER_DROP_TDM_PACKETS_QMLF_Nf, prd_config->never_drop_tdm_packets);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, qmlf_index, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_config_get(int unit, int port, soc_jer_nif_prd_config_t* prd_config)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_t reg;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi)  
    {
        reg = NBIH_PRD_CONFIGr;
        reg_port = REG_PORT_ANY;
    }
    else 
    {
        reg = NBIL_PRD_CONFIGr;
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, qmlf_index, &reg32_val));
    prd_config->never_drop_control_frames = soc_reg_field_get(unit, reg, reg32_val, PRD_NEVER_DROP_CONTROL_FRAMES_QMLF_Nf);
    prd_config->seperate_tdm_and_non_tdm = soc_reg_field_get(unit, reg, reg32_val, PRD_SEPARATE_TDM_AND_NON_TMD_FOR_ETH_QMLF_Nf);
    prd_config->seperate_tdm_and_non_tdm_two_ports = soc_reg_field_get(unit, reg, reg32_val, PRD_SEPARATE_TDM_AND_NON_TMD_FROM_TWO_PORTS_QMLF_Nf);
    prd_config->never_drop_tdm_packets = soc_reg_field_get(unit, reg, reg32_val, PRD_NEVER_DROP_TDM_PACKETS_QMLF_Nf);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_stage_control_ilkn_set_get(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl, int to_set)
{
    uint32 offset, num_lanes;
    int reg_port;
    soc_reg_t reg;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_CONTROLr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_CONTROLr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set) {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_ITMH_TYPE_SELECTf, hard_stage_ctrl->itmh_type);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf, hard_stage_ctrl->small_chunk_priority_override);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            hard_stage_ctrl->itmh_type = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_ITMH_TYPE_SELECTf);
            hard_stage_ctrl->small_chunk_priority_override = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_CONTROLr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_CONTROLr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_ITMH_TYPE_SELECTf, hard_stage_ctrl->itmh_type);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf, hard_stage_ctrl->small_chunk_priority_override);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            hard_stage_ctrl->itmh_type = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_ITMH_TYPE_SELECTf);
            hard_stage_ctrl->small_chunk_priority_override = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_stage_control_qmlf_set_get(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl, int to_set)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    uint32 reg32_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t itmh_type_fields[] = {QMLF_0_PARSER_HARD_ITMH_TYPE_SELECTf, QMLF_1_PARSER_HARD_ITMH_TYPE_SELECTf, QMLF_2_PARSER_HARD_ITMH_TYPE_SELECTf,
            QMLF_3_PARSER_HARD_ITMH_TYPE_SELECTf, QMLF_4_PARSER_HARD_ITMH_TYPE_SELECTf, QMLF_5_PARSER_HARD_ITMH_TYPE_SELECTf};

    soc_field_t small_chunk_priority_override_fields[] = {QMLF_0_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf, QMLF_1_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf, QMLF_2_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf,
            QMLF_3_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf, QMLF_4_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf, QMLF_5_PARSER_HARD_SMALL_CHUNK_PRIORITY_OVERRIDEf};

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_CONTROLr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_CONTROLr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_CONTROLr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_CONTROLr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_CONTROLr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_CONTROLr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_CONTROLr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_CONTROLr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_CONTROLr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_CONTROLr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_CONTROLr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_CONTROLr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, 0, &reg32_val));
    if (to_set)
    {
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, itmh_type_fields[qmlf_index], hard_stage_ctrl->itmh_type);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, small_chunk_priority_override_fields[qmlf_index], hard_stage_ctrl->small_chunk_priority_override);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, 0, reg32_val));
    }
    else 
    {
        hard_stage_ctrl->itmh_type = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, itmh_type_fields[qmlf_index]);
        hard_stage_ctrl->small_chunk_priority_override = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, small_chunk_priority_override_fields[qmlf_index]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_control_set(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_control_ilkn_set_get(unit, port, hard_stage_ctrl, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_control_qmlf_set_get(unit, port, hard_stage_ctrl, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_control_get(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_control_ilkn_set_get(unit, port, hard_stage_ctrl, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_control_qmlf_set_get(unit, port, hard_stage_ctrl, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_tpid_ilkn_set_get(int unit, int port, int tpid_index, uint32 *tpid_val, int to_set)
{
    uint32 offset, num_lanes;
    int reg_port;
    uint64 reg64_val;
    soc_reg_t reg;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        soc_field_t fields[] = {ILKN_0_PARSER_HARD_PORT_N_TPID_0f, ILKN_0_PARSER_HARD_PORT_N_TPID_1f, ILKN_0_PARSER_HARD_PORT_N_TPID_2f, ILKN_0_PARSER_HARD_PORT_N_TPID_3f};

        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_PORT_ETH_TPIDr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_PORT_ETH_TPIDr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, 0, &reg64_val));
        if (to_set)
        {
            soc_reg64_field32_set(unit, reg, &reg64_val, fields[tpid_index], *tpid_val);
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, 0, reg64_val));
        }
        else 
        {
            *tpid_val = soc_reg64_field32_get(unit, reg, reg64_val, fields[tpid_index]);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        soc_field_t fields[] = {ILKN_1_PARSER_HARD_PORT_N_TPID_0f, ILKN_1_PARSER_HARD_PORT_N_TPID_1f, ILKN_1_PARSER_HARD_PORT_N_TPID_2f, ILKN_1_PARSER_HARD_PORT_N_TPID_3f};

        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_PORT_ETH_TPIDr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_PORT_ETH_TPIDr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, reg_port, 0, &reg64_val));
        if (to_set)
        {
            soc_reg64_field32_set(unit, reg, &reg64_val, fields[tpid_index], *tpid_val);
            SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, reg_port, 0, reg64_val));
        }
        else 
        {
            *tpid_val = soc_reg64_field32_get(unit, reg, reg64_val, fields[tpid_index]);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_tpid_qmlf_set_get(int unit, int port, int tpid_index, uint32 *tpid_val, int to_set)
{
    int first_phy=-1, reg_port, first_phy_index_in_qmlf, field_index_in_arr;
    const int tpids_per_qmlf = 4;
    uint32 nof_lanes_nbi, qmlf_index;
    uint64 reg64_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t fields[] = { QMLF_0_PARSER_HARD_PORT_N_TPID_0f, QMLF_0_PARSER_HARD_PORT_N_TPID_1f, QMLF_0_PARSER_HARD_PORT_N_TPID_2f, QMLF_0_PARSER_HARD_PORT_N_TPID_3f,
                             QMLF_1_PARSER_HARD_PORT_N_TPID_0f, QMLF_1_PARSER_HARD_PORT_N_TPID_1f, QMLF_1_PARSER_HARD_PORT_N_TPID_2f, QMLF_1_PARSER_HARD_PORT_N_TPID_3f,
                             QMLF_2_PARSER_HARD_PORT_N_TPID_0f, QMLF_2_PARSER_HARD_PORT_N_TPID_1f, QMLF_2_PARSER_HARD_PORT_N_TPID_2f, QMLF_2_PARSER_HARD_PORT_N_TPID_3f,
                             QMLF_3_PARSER_HARD_PORT_N_TPID_0f, QMLF_3_PARSER_HARD_PORT_N_TPID_1f, QMLF_3_PARSER_HARD_PORT_N_TPID_2f, QMLF_3_PARSER_HARD_PORT_N_TPID_3f,
                             QMLF_4_PARSER_HARD_PORT_N_TPID_0f, QMLF_4_PARSER_HARD_PORT_N_TPID_1f, QMLF_4_PARSER_HARD_PORT_N_TPID_2f, QMLF_4_PARSER_HARD_PORT_N_TPID_3f,
                             QMLF_5_PARSER_HARD_PORT_N_TPID_0f, QMLF_5_PARSER_HARD_PORT_N_TPID_1f, QMLF_5_PARSER_HARD_PORT_N_TPID_2f, QMLF_5_PARSER_HARD_PORT_N_TPID_3f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    first_phy_index_in_qmlf = first_phy & 3; 
    field_index_in_arr = qmlf_index*tpids_per_qmlf + tpid_index;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_PORT_ETH_TPIDr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_PORT_ETH_TPIDr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_PORT_ETH_TPIDr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_PORT_ETH_TPIDr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_PORT_ETH_TPIDr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_PORT_ETH_TPIDr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_PORT_ETH_TPIDr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_PORT_ETH_TPIDr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_PORT_ETH_TPIDr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_PORT_ETH_TPIDr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_PORT_ETH_TPIDr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_PORT_ETH_TPIDr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, &reg64_val));
    if (to_set)
    {
        soc_reg64_field32_set(unit, regs[qmlf_index], &reg64_val, fields[field_index_in_arr], *tpid_val);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, reg64_val));
    }
    else
    {
        *tpid_val = soc_reg64_field32_get(unit, regs[qmlf_index], reg64_val, fields[field_index_in_arr]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_tpid_set(int unit, int port, int tpid_index, uint32 tpid_val)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_tpid_ilkn_set_get(unit, port, tpid_index, &tpid_val, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_tpid_qmlf_set_get(unit, port, tpid_index, &tpid_val, 1));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_tpid_get(int unit, int port, int tpid_index, uint32 *tpid_val)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_tpid_ilkn_set_get(unit, port, tpid_index, tpid_val, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_tpid_qmlf_set_get(unit, port, tpid_index, tpid_val, 0));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_stage_port_type_ilkn_set_get(int unit, int port, int *port_type, int to_set)
{
    uint32 offset, num_lanes;
    uint32 reg32_val;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_PORT_TYPEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_PORT_TYPEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_TYPEf, *port_type);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            *port_type = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_TYPEf);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_PORT_TYPEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_PORT_TYPEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_TYPEf, *port_type);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            *port_type = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_TYPEf);
        }
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_stage_port_type_qmlf_set_get(int unit, int port, int *port_type, int to_set)
{
    int first_phy=-1, reg_port, first_phy_index_in_qmlf;
    uint32 nof_lanes_nbi, qmlf_index;
    uint32 reg32_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t fields[] = {QMLF_0_PARSER_HARD_PORT_N_TYPEf, QMLF_1_PARSER_HARD_PORT_N_TYPEf, QMLF_2_PARSER_HARD_PORT_N_TYPEf,
            QMLF_3_PARSER_HARD_PORT_N_TYPEf, QMLF_4_PARSER_HARD_PORT_N_TYPEf, QMLF_5_PARSER_HARD_PORT_N_TYPEf};

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    first_phy_index_in_qmlf = first_phy & 3; 

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_PORT_TYPEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_PORT_TYPEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_PORT_TYPEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_PORT_TYPEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_PORT_TYPEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_PORT_TYPEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_PORT_TYPEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_PORT_TYPEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_PORT_TYPEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_PORT_TYPEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_PORT_TYPEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_PORT_TYPEr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, &reg32_val));
    if (to_set)
    {
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[qmlf_index], *port_type);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, reg32_val));
    }
    else
    {
        *port_type = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[qmlf_index]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_port_type_set(int unit, int port, int port_type)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_port_type_ilkn_set_get(unit, port, &port_type, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_port_type_qmlf_set_get(unit, port, &port_type, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_port_type_get(int unit, int port, int *port_type)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_port_type_ilkn_set_get(unit, port, port_type, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_port_type_qmlf_set_get(unit, port, port_type, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_stage_properties_ilkn_set_get(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties, int to_set)
{
    uint32 offset, num_lanes;
    uint32 reg32_val;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_PORT_PROPERTIESr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_PORT_PROPERTIESr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, properties->trust_ip_sdcp);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, properties->trust_mpls_exp);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf, properties->trust_outer_eth_tag);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, properties->trust_inner_eth_tag);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, properties->outer_tag_size);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf, properties->default_priority);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            properties->trust_ip_sdcp = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf);
            properties->trust_mpls_exp = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf);
            properties->trust_outer_eth_tag = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf);
            properties->trust_inner_eth_tag = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf);
            properties->outer_tag_size = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf);
            properties->default_priority = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_PORT_PROPERTIESr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_PORT_PROPERTIESr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, properties->trust_ip_sdcp);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, properties->trust_mpls_exp);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf, properties->trust_outer_eth_tag);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, properties->trust_inner_eth_tag);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, properties->outer_tag_size);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf, properties->default_priority);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            properties->trust_ip_sdcp = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf);
            properties->trust_mpls_exp = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf);
            properties->trust_outer_eth_tag = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf);
            properties->trust_inner_eth_tag = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf);
            properties->outer_tag_size = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf);
            properties->default_priority = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf);
        }
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_stage_properties_qmlf_set_get(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties, int to_set)
{
    int first_phy=-1, reg_port, first_phy_index_in_qmlf, first_field_index_in_arr;
    const int fields_per_qmlf = 6;
    uint32 nof_lanes_nbi, qmlf_index;
    uint32 reg32_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t fields[] = { QMLF_0_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, QMLF_0_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, QMLF_0_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf,
                             QMLF_0_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, QMLF_0_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, QMLF_0_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf,

                             QMLF_1_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, QMLF_1_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, QMLF_1_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf,
                             QMLF_1_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, QMLF_1_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, QMLF_1_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf,

                             QMLF_2_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, QMLF_2_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, QMLF_2_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf,
                             QMLF_2_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, QMLF_2_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, QMLF_2_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf,

                             QMLF_3_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, QMLF_3_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, QMLF_3_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf,
                             QMLF_3_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, QMLF_3_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, QMLF_3_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf,

                             QMLF_4_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, QMLF_4_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, QMLF_4_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf,
                             QMLF_4_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, QMLF_4_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, QMLF_4_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf,

                             QMLF_5_PARSER_HARD_PORT_N_PROP_TRUST_IP_DSCPf, QMLF_5_PARSER_HARD_PORT_N_PROP_TRUST_MPLS_EXPf, QMLF_5_PARSER_HARD_PORT_N_PROP_TRUST_OUTER_ETH_TAGf,
                             QMLF_5_PARSER_HARD_PORT_N_PROP_TRUST_INNER_ETH_TAGf, QMLF_5_PARSER_HARD_PORT_N_PROP_OUTER_TAG_SIZEf, QMLF_5_PARSER_HARD_PORT_N_PROP_DEFAULT_PRIORITYf };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    first_phy_index_in_qmlf = first_phy & 3; 
    first_field_index_in_arr = qmlf_index*fields_per_qmlf;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_PORT_PROPERTIESr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_PORT_PROPERTIESr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_PORT_PROPERTIESr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_PORT_PROPERTIESr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_PORT_PROPERTIESr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_PORT_PROPERTIESr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_PORT_PROPERTIESr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_PORT_PROPERTIESr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_PORT_PROPERTIESr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_PORT_PROPERTIESr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_PORT_PROPERTIESr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_PORT_PROPERTIESr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, &reg32_val));
    if (to_set)
    {
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr], properties->trust_ip_sdcp);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr+1], properties->trust_mpls_exp);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr+2], properties->trust_outer_eth_tag);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr+3], properties->trust_inner_eth_tag);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr+4], properties->outer_tag_size);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr+5], properties->default_priority);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, reg32_val));
    }
    else
    {
        properties->trust_ip_sdcp = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr]);
        properties->trust_mpls_exp = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr+1]);
        properties->trust_outer_eth_tag = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr+2]);
        properties->trust_inner_eth_tag = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr+3]);
        properties->outer_tag_size = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr+4]);
        properties->default_priority = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr+5]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_properties_set(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_properties_ilkn_set_get(unit, port, properties, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_properties_qmlf_set_get(unit, port, properties, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_stage_properties_get(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_properties_ilkn_set_get(unit, port, properties, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_stage_properties_qmlf_set_get(unit, port, properties, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_ether_type_ilkn_set_get(int unit, int port, uint32 ether_type_code, uint32 *ether_type_val, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_above_64_val_t reg_above64_val;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        soc_field_t fields[] = {ILKN_0_PARSER_HARD_ETHER_TYPE_0f, ILKN_0_PARSER_HARD_ETHER_TYPE_1f, ILKN_0_PARSER_HARD_ETHER_TYPE_2f, ILKN_0_PARSER_HARD_ETHER_TYPE_3f,
                ILKN_0_PARSER_HARD_ETHER_TYPE_4f, ILKN_0_PARSER_HARD_ETHER_TYPE_5f, ILKN_0_PARSER_HARD_ETHER_TYPE_6f};

        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_ETHER_TYPEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_ETHER_TYPEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, reg_above64_val));
        if (to_set)
        {
            soc_reg_above_64_field32_set(unit, reg, reg_above64_val, fields[ether_type_code], *ether_type_val);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, reg_above64_val));
        }
        else 
        {
            *ether_type_val = soc_reg_above_64_field32_get(unit, reg, reg_above64_val, fields[ether_type_code]);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        soc_field_t fields[] = {ILKN_1_PARSER_HARD_ETHER_TYPE_0f, ILKN_1_PARSER_HARD_ETHER_TYPE_1f, ILKN_1_PARSER_HARD_ETHER_TYPE_2f, ILKN_1_PARSER_HARD_ETHER_TYPE_3f,
                        ILKN_1_PARSER_HARD_ETHER_TYPE_4f, ILKN_1_PARSER_HARD_ETHER_TYPE_5f, ILKN_1_PARSER_HARD_ETHER_TYPE_6f};

        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_ETHER_TYPEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_ETHER_TYPEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, reg_above64_val));
        if (to_set)
        {
            soc_reg_above_64_field32_set(unit, reg, reg_above64_val, fields[ether_type_code], *ether_type_val);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, reg_above64_val));
        }
        else 
        {
            *ether_type_val = soc_reg_above_64_field32_get(unit, reg, reg_above64_val, fields[ether_type_code]);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_hard_ether_type_qmlf_set_get(int unit, int port, uint32 ether_type_code, uint32 *ether_type_val, int to_set)
{
    int first_phy=-1, reg_port, field_index_in_arr;
    const int ether_types_per_qmlf = 7;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_above_64_val_t reg_above64_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t fields[] = { QMLF_0_PARSER_HARD_ETHER_TYPE_0f, QMLF_0_PARSER_HARD_ETHER_TYPE_1f, QMLF_0_PARSER_HARD_ETHER_TYPE_2f, QMLF_0_PARSER_HARD_ETHER_TYPE_3f,
                             QMLF_0_PARSER_HARD_ETHER_TYPE_4f, QMLF_0_PARSER_HARD_ETHER_TYPE_5f, QMLF_0_PARSER_HARD_ETHER_TYPE_6f,

                             QMLF_1_PARSER_HARD_ETHER_TYPE_0f, QMLF_1_PARSER_HARD_ETHER_TYPE_1f, QMLF_1_PARSER_HARD_ETHER_TYPE_2f, QMLF_1_PARSER_HARD_ETHER_TYPE_3f,
                             QMLF_1_PARSER_HARD_ETHER_TYPE_4f, QMLF_1_PARSER_HARD_ETHER_TYPE_5f, QMLF_1_PARSER_HARD_ETHER_TYPE_6f,

                             QMLF_2_PARSER_HARD_ETHER_TYPE_0f, QMLF_2_PARSER_HARD_ETHER_TYPE_1f, QMLF_2_PARSER_HARD_ETHER_TYPE_2f, QMLF_2_PARSER_HARD_ETHER_TYPE_3f,
                             QMLF_2_PARSER_HARD_ETHER_TYPE_4f, QMLF_2_PARSER_HARD_ETHER_TYPE_5f, QMLF_2_PARSER_HARD_ETHER_TYPE_6f,

                             QMLF_3_PARSER_HARD_ETHER_TYPE_0f, QMLF_3_PARSER_HARD_ETHER_TYPE_1f, QMLF_3_PARSER_HARD_ETHER_TYPE_2f, QMLF_3_PARSER_HARD_ETHER_TYPE_3f,
                             QMLF_3_PARSER_HARD_ETHER_TYPE_4f, QMLF_3_PARSER_HARD_ETHER_TYPE_5f, QMLF_3_PARSER_HARD_ETHER_TYPE_6f,

                             QMLF_4_PARSER_HARD_ETHER_TYPE_0f, QMLF_4_PARSER_HARD_ETHER_TYPE_1f, QMLF_4_PARSER_HARD_ETHER_TYPE_2f, QMLF_4_PARSER_HARD_ETHER_TYPE_3f,
                             QMLF_4_PARSER_HARD_ETHER_TYPE_4f, QMLF_4_PARSER_HARD_ETHER_TYPE_5f, QMLF_4_PARSER_HARD_ETHER_TYPE_6f,

                             QMLF_5_PARSER_HARD_ETHER_TYPE_0f, QMLF_5_PARSER_HARD_ETHER_TYPE_1f, QMLF_5_PARSER_HARD_ETHER_TYPE_2f, QMLF_5_PARSER_HARD_ETHER_TYPE_3f,
                             QMLF_5_PARSER_HARD_ETHER_TYPE_4f, QMLF_5_PARSER_HARD_ETHER_TYPE_5f, QMLF_5_PARSER_HARD_ETHER_TYPE_6f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    field_index_in_arr = qmlf_index*ether_types_per_qmlf + ether_type_code;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_ETHER_TYPEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_ETHER_TYPEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_ETHER_TYPEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_ETHER_TYPEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_ETHER_TYPEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_ETHER_TYPEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_ETHER_TYPEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_ETHER_TYPEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_ETHER_TYPEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_ETHER_TYPEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_ETHER_TYPEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_ETHER_TYPEr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[qmlf_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field32_set(unit, regs[qmlf_index], reg_above64_val, fields[field_index_in_arr], *ether_type_val);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[qmlf_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        *ether_type_val = soc_reg_above_64_field32_get(unit, regs[qmlf_index], reg_above64_val, fields[field_index_in_arr]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_ether_type_set(int unit, int port, uint32 ether_type_code, uint32 ether_type_val)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_ether_type_ilkn_set_get(unit, port, ether_type_code, &ether_type_val, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_ether_type_qmlf_set_get(unit, port, ether_type_code, &ether_type_val, 1));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_hard_ether_type_get(int unit, int port, uint32 ether_type_code, uint32 *ether_type_val)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_ether_type_ilkn_set_get(unit, port, ether_type_code, ether_type_val, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_hard_ether_type_qmlf_set_get(unit, port, ether_type_code, ether_type_val, 0));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_control_plane_ilkn_set_get(int unit, int port, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *control_frame_conf, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_above_64_val_t reg_above64_val;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_CONTROL_PLANEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_CONTROL_PLANEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, control_frame_index, reg_above64_val));
        if (to_set)
        {
            soc_reg_above_64_field64_set(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, control_frame_conf->mac_da_val);
            soc_reg_above_64_field64_set(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf, control_frame_conf->mac_da_mask);
            soc_reg_above_64_field32_set(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, control_frame_conf->ether_type_code);
            soc_reg_above_64_field32_set(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf, control_frame_conf->ether_type_code_mask);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, control_frame_index, reg_above64_val));
        }
        else 
        {
            control_frame_conf->mac_da_val = soc_reg_above_64_field64_get(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf);
            control_frame_conf->mac_da_mask = soc_reg_above_64_field64_get(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf);
            control_frame_conf->ether_type_code = soc_reg_above_64_field32_get(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf);
            control_frame_conf->ether_type_code_mask = soc_reg_above_64_field32_get(unit, reg, reg_above64_val, ILKN_0_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_CONTROL_PLANEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_CONTROL_PLANEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, control_frame_index, reg_above64_val));
        if (to_set)
        {
            soc_reg_above_64_field64_set(unit, reg,  reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, control_frame_conf->mac_da_val);
            soc_reg_above_64_field64_set(unit, reg,  reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf, control_frame_conf->mac_da_mask);
            soc_reg_above_64_field32_set(unit, reg, reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, control_frame_conf->ether_type_code);
            soc_reg_above_64_field32_set(unit, reg, reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf, control_frame_conf->ether_type_code_mask);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, control_frame_index, reg_above64_val));
        }
        else 
        {
            control_frame_conf->mac_da_val = soc_reg_above_64_field64_get(unit, reg, reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf);
            control_frame_conf->mac_da_mask = soc_reg_above_64_field64_get(unit, reg, reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf);
            control_frame_conf->ether_type_code = soc_reg_above_64_field32_get(unit, reg, reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf);
            control_frame_conf->ether_type_code_mask = soc_reg_above_64_field32_get(unit, reg, reg_above64_val, ILKN_1_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf);
        }
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_control_plane_qmlf_set_get(int unit, int port, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *control_frame_conf, int to_set)
{
    int first_phy=-1, reg_port, first_field_index_in_arr;
    const int fields_per_qmlf = 4;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_above_64_val_t reg_above64_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t fields[] = { QMLF_0_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, QMLF_0_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf,
                             QMLF_0_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, QMLF_0_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf,

                             QMLF_1_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, QMLF_1_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf,
                             QMLF_1_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, QMLF_1_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf,

                             QMLF_2_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, QMLF_2_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf,
                             QMLF_2_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, QMLF_2_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf,

                             QMLF_3_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, QMLF_3_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf,
                             QMLF_3_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, QMLF_3_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf,

                             QMLF_4_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, QMLF_4_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf,
                             QMLF_4_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, QMLF_4_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf,

                             QMLF_5_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_VALf, QMLF_5_PARSER_HARD_CONTROL_PLANE_N_MAC_DA_MASKf,
                             QMLF_5_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODEf, QMLF_5_PARSER_HARD_CONTROL_PLANE_N_ETHER_TYPE_CODE_MASKf };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    first_field_index_in_arr = qmlf_index*fields_per_qmlf;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_CONTROL_PLANEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_CONTROL_PLANEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_CONTROL_PLANEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_CONTROL_PLANEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_CONTROL_PLANEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_CONTROL_PLANEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_CONTROL_PLANEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_CONTROL_PLANEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_CONTROL_PLANEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_CONTROL_PLANEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_CONTROL_PLANEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_CONTROL_PLANEr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[qmlf_index], reg_port, control_frame_index, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[qmlf_index],  reg_above64_val, fields[first_field_index_in_arr], control_frame_conf->mac_da_val);
        soc_reg_above_64_field64_set(unit, regs[qmlf_index],  reg_above64_val, fields[first_field_index_in_arr+1], control_frame_conf->mac_da_mask);
        soc_reg_above_64_field32_set(unit, regs[qmlf_index], reg_above64_val, fields[first_field_index_in_arr+2], control_frame_conf->ether_type_code);
        soc_reg_above_64_field32_set(unit, regs[qmlf_index], reg_above64_val, fields[first_field_index_in_arr+3], control_frame_conf->ether_type_code_mask);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[qmlf_index], reg_port, control_frame_index, reg_above64_val));
    }
    else
    {
        control_frame_conf->mac_da_val = soc_reg_above_64_field64_get(unit, regs[qmlf_index], reg_above64_val, fields[first_field_index_in_arr]);
        control_frame_conf->mac_da_mask = soc_reg_above_64_field64_get(unit, regs[qmlf_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        control_frame_conf->ether_type_code = soc_reg_above_64_field32_get(unit, regs[qmlf_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        control_frame_conf->ether_type_code_mask = soc_reg_above_64_field32_get(unit, regs[qmlf_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_control_plane_set(int unit, int port, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *control_frame_conf)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_control_plane_ilkn_set_get(unit, port, control_frame_index, control_frame_conf, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_control_plane_qmlf_set_get(unit, port, control_frame_index, control_frame_conf, 1));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_control_plane_get(int unit, int port, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *control_frame_conf)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_control_plane_ilkn_set_get(unit, port, control_frame_index, control_frame_conf, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_control_plane_qmlf_set_get(unit, port, control_frame_index, control_frame_conf, 0));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_tm_tc_dp_ilkn_set_get(int unit, int port, soc_reg_above_64_val_t map_value, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, map_value));
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, map_value));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_tm_tc_dp_qmlf_set_get(int unit, int port, soc_reg_above_64_val_t map_value, int to_set)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_TM_TC_DP_TO_PRIORITY_TABLEr;
    }

    if (to_set)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[qmlf_index], reg_port, 0, map_value));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[qmlf_index], reg_port, 0, map_value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_tm_tc_dp_set(int unit, int port, soc_reg_above_64_val_t map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_tm_tc_dp_ilkn_set_get(unit, port, map_value, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_tm_tc_dp_qmlf_set_get(unit, port, map_value, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_tm_tc_dp_get(int unit, int port, soc_reg_above_64_val_t map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_tm_tc_dp_ilkn_set_get(unit, port, map_value, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_tm_tc_dp_qmlf_set_get(unit, port, map_value, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_ip_dscp_ilkn_set_get(int unit, int port, soc_reg_above_64_val_t map_value, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, map_value));
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, map_value));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_ip_dscp_qmlf_set_get(int unit, int port, soc_reg_above_64_val_t map_value, int to_set)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_IP_DSCP_TO_PRIORITY_TABLEr;
    }

    if (to_set)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[qmlf_index], reg_port, 0, map_value));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[qmlf_index], reg_port, 0, map_value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_ip_dscp_set(int unit, int port, soc_reg_above_64_val_t map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_ip_dscp_ilkn_set_get(unit, port, map_value, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_ip_dscp_qmlf_set_get(unit, port, map_value, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_ip_dscp_get(int unit, int port, soc_reg_above_64_val_t map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_ip_dscp_ilkn_set_get(unit, port, map_value, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_ip_dscp_qmlf_set_get(unit, port, map_value, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_eth_pcp_dei_ilkn_set_get(int unit, int port, uint32 *map_value, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, map_value));
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, map_value));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_eth_pcp_dei_qmlf_set_get(int unit, int port, uint32 *map_value, int to_set)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_ETH_PCP_DEI_TO_PRIORITY_TABLEr;
    }

    if (to_set)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, 0, *map_value));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, 0, map_value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_eth_pcp_dei_set(int unit, int port, uint32 map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_eth_pcp_dei_ilkn_set_get(unit, port, &map_value, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_eth_pcp_dei_qmlf_set_get(unit, port, &map_value, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_eth_pcp_dei_get(int unit, int port, uint32 *map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_eth_pcp_dei_ilkn_set_get(unit, port, map_value, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_eth_pcp_dei_qmlf_set_get(unit, port, map_value, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_mpls_exp_ilkn_set_get(int unit, int port, uint32 *map_value, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, map_value));
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        if (to_set)
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, *map_value));
        }
        else 
        {
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, map_value));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_prd_map_mpls_exp_qmlf_set_get(int unit, int port, uint32 *map_value, int to_set)
{
    int first_phy=-1, reg_port;
    uint32 nof_lanes_nbi, qmlf_index;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[1] = NBIH_QMLF_1_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[2] = NBIH_QMLF_2_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[3] = NBIH_QMLF_3_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[4] = NBIH_QMLF_4_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[5] = NBIH_QMLF_5_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[1] = NBIL_QMLF_1_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[2] = NBIL_QMLF_2_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[3] = NBIL_QMLF_3_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[4] = NBIL_QMLF_4_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
        regs[5] = NBIL_QMLF_5_PARSER_HARD_MPLS_EXP_TO_PRIORITY_TABLEr;
    }

    if (to_set)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, 0, *map_value));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, 0, map_value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_mpls_exp_set(int unit, int port, uint32 map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_mpls_exp_ilkn_set_get(unit, port, &map_value, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_mpls_exp_qmlf_set_get(unit, port, &map_value, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_prd_map_mpls_exp_get(int unit, int port, uint32 *map_value)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_mpls_exp_ilkn_set_get(unit, port, map_value, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_prd_map_mpls_exp_qmlf_set_get(unit, port, map_value, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_enable_ilkn_set_get(int unit, int port, int *en_eth, int *en_tm, int to_set)
{
    uint32 offset, num_lanes;
    uint32 reg32_val;
    soc_reg_t reg;
    int reg_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf, *en_eth);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_0_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf, *en_tm);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            *en_eth = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf);
            *en_tm = soc_reg_field_get(unit, reg, reg32_val, ILKN_0_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf, *en_eth);
            soc_reg_field_set(unit, reg, &reg32_val, ILKN_1_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf, *en_tm);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            *en_eth = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf);
            *en_tm = soc_reg_field_get(unit, reg, reg32_val, ILKN_1_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_enable_qmlf_set_get(int unit, int port, int *en_eth, int *en_tm, int to_set)
{
    int first_phy=-1, reg_port, first_phy_index_in_qmlf;
    uint32 nof_lanes_nbi, qmlf_index;
    uint32 reg32_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t en_eth_fields[] = {QMLF_0_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf, QMLF_1_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf, QMLF_2_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf,
            QMLF_3_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf, QMLF_4_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf, QMLF_5_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_ETHf};

    soc_field_t en_tm_fields[] = {QMLF_0_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf, QMLF_1_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf, QMLF_2_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf,
            QMLF_3_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf, QMLF_4_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf, QMLF_5_PARSER_SOFT_PORT_N_SOFT_STAGE_ENABLE_TMf};

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    first_phy_index_in_qmlf = first_phy & 3; 

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[1] = NBIH_QMLF_1_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[2] = NBIH_QMLF_2_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[3] = NBIH_QMLF_3_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[4] = NBIH_QMLF_4_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[5] = NBIH_QMLF_5_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[1] = NBIL_QMLF_1_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[2] = NBIL_QMLF_2_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[3] = NBIL_QMLF_3_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[4] = NBIL_QMLF_4_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
        regs[5] = NBIL_QMLF_5_PARSER_SOFT_PORT_SOFT_STAGE_ENABLEr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, &reg32_val));
    if (to_set)
    {
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, en_eth_fields[qmlf_index], *en_eth);
        soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, en_tm_fields[qmlf_index], *en_tm);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, first_phy_index_in_qmlf, reg32_val));
    }
    else
    {
        *en_eth = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, en_eth_fields[qmlf_index]);
        *en_tm = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, en_tm_fields[qmlf_index]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_soft_stage_enable_set(int unit, int port, int en_eth, int en_tm)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_enable_ilkn_set_get(unit, port, &en_eth, &en_tm, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_enable_qmlf_set_get(unit, port, &en_eth, &en_tm, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_soft_stage_enable_get(int unit, int port, int *en_eth, int *en_tm)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_enable_ilkn_set_get(unit, port, en_eth, en_tm, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_enable_qmlf_set_get(unit, port, en_eth, en_tm, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_construct_ilkn_set_get(int unit, int port, uint32 *key_build_offset_array, int to_set)
{
    uint32 offset, num_lanes;
    uint32 reg32_val;
    soc_reg_t reg;
    int reg_port, i;
    const int num_fields_per_reg = 4;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    
    if ((offset & 1) == 0)
    {
        soc_field_t fields[] = {ILKN_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, ILKN_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, ILKN_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, ILKN_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f};
        if (offset == 0) 
        {
            reg = NBIH_ILKN_0_PARSER_SOFT_KEY_CONSTRUCTIONr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_0_PARSER_SOFT_KEY_CONSTRUCTIONr;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            for (i = 0; i < num_fields_per_reg; ++i)
            {
                soc_reg_field_set(unit, reg, &reg32_val, fields[i], key_build_offset_array[i]);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            for (i = 0; i < num_fields_per_reg; ++i)
            {
                key_build_offset_array[i] = soc_reg_field_get(unit, reg, reg32_val, fields[i]);
            }
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        soc_field_t fields[] = {ILKN_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, ILKN_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, ILKN_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, ILKN_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f};

        if ((offset == 0) || (offset == 1)) 
        {
            reg = NBIH_ILKN_1_PARSER_SOFT_KEY_CONSTRUCTIONr;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            reg = NBIL_ILKN_1_PARSER_SOFT_KEY_CONSTRUCTIONr;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, reg_port, 0, &reg32_val));
        if (to_set)
        {
            for (i = 0; i < num_fields_per_reg; ++i)
            {
                soc_reg_field_set(unit, reg, &reg32_val, fields[i], key_build_offset_array[i]);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, reg_port, 0, reg32_val));
        }
        else 
        {
            for (i = 0; i < num_fields_per_reg; ++i)
            {
                key_build_offset_array[i] = soc_reg_field_get(unit, reg, reg32_val, fields[i]);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_construct_qmlf_set_get(int unit, int port, uint32 *key_build_offset_array, int to_set)
{
    int first_phy=-1, reg_port, first_field_index_in_arr, i;
    const int fields_per_qmlf = 4;
    uint32 nof_lanes_nbi, qmlf_index;
    uint32 reg32_val;
    soc_reg_t regs[SOC_JER_PLUS_NOF_QMLFS_PER_NBI];

    soc_field_t fields[] = { QMLF_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, QMLF_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, QMLF_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, QMLF_0_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f,
                             QMLF_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, QMLF_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, QMLF_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, QMLF_1_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f,
                             QMLF_2_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, QMLF_2_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, QMLF_2_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, QMLF_2_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f,
                             QMLF_3_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, QMLF_3_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, QMLF_3_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, QMLF_3_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f,
                             QMLF_4_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, QMLF_4_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, QMLF_4_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, QMLF_4_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f,
                             QMLF_5_PARSER_SOFT_KEY_OFFSET_IN_BYTES_0f, QMLF_5_PARSER_SOFT_KEY_OFFSET_IN_BYTES_1f, QMLF_5_PARSER_SOFT_KEY_OFFSET_IN_BYTES_2f, QMLF_5_PARSER_SOFT_KEY_OFFSET_IN_BYTES_3f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    first_field_index_in_arr = qmlf_index*fields_per_qmlf;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[1] = NBIH_QMLF_1_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[2] = NBIH_QMLF_2_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[3] = NBIH_QMLF_3_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[4] = NBIH_QMLF_4_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[5] = NBIH_QMLF_5_PARSER_SOFT_KEY_CONSTRUCTIONr;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[1] = NBIL_QMLF_1_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[2] = NBIL_QMLF_2_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[3] = NBIL_QMLF_3_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[4] = NBIL_QMLF_4_PARSER_SOFT_KEY_CONSTRUCTIONr;
        regs[5] = NBIL_QMLF_5_PARSER_SOFT_KEY_CONSTRUCTIONr;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs[qmlf_index], reg_port, 0, &reg32_val));
    if (to_set)
    {
        for (i = 0; i < fields_per_qmlf; ++i)
        {
            soc_reg_field_set(unit, regs[qmlf_index], &reg32_val, fields[first_field_index_in_arr+i], key_build_offset_array[i]);
        }
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs[qmlf_index], reg_port, 0, reg32_val));
    }
    else 
    {
        for (i = 0; i < fields_per_qmlf; ++i)
        {
            key_build_offset_array[i] = soc_reg_field_get(unit, regs[qmlf_index], reg32_val, fields[first_field_index_in_arr+i]);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}




int soc_jer_plus_soft_stage_key_construct_set(int unit, int port, uint32 *key_build_offset_array)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_construct_ilkn_set_get(unit, port, key_build_offset_array, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_construct_qmlf_set_get(unit, port, key_build_offset_array, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_soft_stage_key_construct_get(int unit, int port, uint32 *key_build_offset_array)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_construct_ilkn_set_get(unit, port, key_build_offset_array, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_construct_qmlf_set_get(unit, port, key_build_offset_array, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_ilkn_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 offset, num_lanes;
    soc_reg_above_64_val_t reg_above64_val;
    int reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4; 
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_ILKN_PORT];
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));

    tcam_reg_index = key_index/tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    
    if ((offset & 1) == 0)
    {
        soc_field_t fields[] = { ILKN_0_PARSER_SOFT_KEY_0f,     ILKN_0_PARSER_SOFT_MASK_0f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_0f,      ILKN_0_PARSER_SOFT_VALID_0f,
                                 ILKN_0_PARSER_SOFT_KEY_1f,     ILKN_0_PARSER_SOFT_MASK_1f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_1f,      ILKN_0_PARSER_SOFT_VALID_1f,
                                 ILKN_0_PARSER_SOFT_KEY_2f,     ILKN_0_PARSER_SOFT_MASK_2f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_2f,      ILKN_0_PARSER_SOFT_VALID_2f,
                                 ILKN_0_PARSER_SOFT_KEY_3f,     ILKN_0_PARSER_SOFT_MASK_3f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_3f,      ILKN_0_PARSER_SOFT_VALID_3f,
                                 ILKN_0_PARSER_SOFT_KEY_4f,     ILKN_0_PARSER_SOFT_MASK_4f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_4f,      ILKN_0_PARSER_SOFT_VALID_4f,
                                 ILKN_0_PARSER_SOFT_KEY_5f,     ILKN_0_PARSER_SOFT_MASK_5f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_5f,      ILKN_0_PARSER_SOFT_VALID_5f,
                                 ILKN_0_PARSER_SOFT_KEY_6f,     ILKN_0_PARSER_SOFT_MASK_6f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_6f,      ILKN_0_PARSER_SOFT_VALID_6f,
                                 ILKN_0_PARSER_SOFT_KEY_7f,     ILKN_0_PARSER_SOFT_MASK_7f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_7f,      ILKN_0_PARSER_SOFT_VALID_7f,
                                 ILKN_0_PARSER_SOFT_KEY_8f,     ILKN_0_PARSER_SOFT_MASK_8f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_8f,      ILKN_0_PARSER_SOFT_VALID_8f,
                                 ILKN_0_PARSER_SOFT_KEY_9f,     ILKN_0_PARSER_SOFT_MASK_9f,     ILKN_0_PARSER_SOFT_PRIORITY_RESULT_9f,      ILKN_0_PARSER_SOFT_VALID_9f,
                                 ILKN_0_PARSER_SOFT_KEY_10f,    ILKN_0_PARSER_SOFT_MASK_10f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_10f,     ILKN_0_PARSER_SOFT_VALID_10f,
                                 ILKN_0_PARSER_SOFT_KEY_11f,    ILKN_0_PARSER_SOFT_MASK_11f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_11f,     ILKN_0_PARSER_SOFT_VALID_11f,
                                 ILKN_0_PARSER_SOFT_KEY_12f,    ILKN_0_PARSER_SOFT_MASK_12f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_12f,     ILKN_0_PARSER_SOFT_VALID_12f,
                                 ILKN_0_PARSER_SOFT_KEY_13f,    ILKN_0_PARSER_SOFT_MASK_13f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_13f,     ILKN_0_PARSER_SOFT_VALID_13f,
                                 ILKN_0_PARSER_SOFT_KEY_14f,    ILKN_0_PARSER_SOFT_MASK_14f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_14f,     ILKN_0_PARSER_SOFT_VALID_14f,
                                 ILKN_0_PARSER_SOFT_KEY_15f,    ILKN_0_PARSER_SOFT_MASK_15f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_15f,     ILKN_0_PARSER_SOFT_VALID_15f,
                                 ILKN_0_PARSER_SOFT_KEY_16f,    ILKN_0_PARSER_SOFT_MASK_16f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_16f,     ILKN_0_PARSER_SOFT_VALID_16f,
                                 ILKN_0_PARSER_SOFT_KEY_17f,    ILKN_0_PARSER_SOFT_MASK_17f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_17f,     ILKN_0_PARSER_SOFT_VALID_17f,
                                 ILKN_0_PARSER_SOFT_KEY_18f,    ILKN_0_PARSER_SOFT_MASK_18f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_18f,     ILKN_0_PARSER_SOFT_VALID_18f,
                                 ILKN_0_PARSER_SOFT_KEY_19f,    ILKN_0_PARSER_SOFT_MASK_19f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_19f,     ILKN_0_PARSER_SOFT_VALID_19f,
                                 ILKN_0_PARSER_SOFT_KEY_20f,    ILKN_0_PARSER_SOFT_MASK_20f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_20f,     ILKN_0_PARSER_SOFT_VALID_20f,
                                 ILKN_0_PARSER_SOFT_KEY_21f,    ILKN_0_PARSER_SOFT_MASK_21f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_21f,     ILKN_0_PARSER_SOFT_VALID_21f,
                                 ILKN_0_PARSER_SOFT_KEY_22f,    ILKN_0_PARSER_SOFT_MASK_22f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_22f,     ILKN_0_PARSER_SOFT_VALID_22f,
                                 ILKN_0_PARSER_SOFT_KEY_23f,    ILKN_0_PARSER_SOFT_MASK_23f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_23f,     ILKN_0_PARSER_SOFT_VALID_23f,
                                 ILKN_0_PARSER_SOFT_KEY_24f,    ILKN_0_PARSER_SOFT_MASK_24f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_24f,     ILKN_0_PARSER_SOFT_VALID_24f,
                                 ILKN_0_PARSER_SOFT_KEY_25f,    ILKN_0_PARSER_SOFT_MASK_25f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_25f,     ILKN_0_PARSER_SOFT_VALID_25f,
                                 ILKN_0_PARSER_SOFT_KEY_26f,    ILKN_0_PARSER_SOFT_MASK_26f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_26f,     ILKN_0_PARSER_SOFT_VALID_26f,
                                 ILKN_0_PARSER_SOFT_KEY_27f,    ILKN_0_PARSER_SOFT_MASK_27f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_27f,     ILKN_0_PARSER_SOFT_VALID_27f,
                                 ILKN_0_PARSER_SOFT_KEY_28f,    ILKN_0_PARSER_SOFT_MASK_28f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_28f,     ILKN_0_PARSER_SOFT_VALID_28f,
                                 ILKN_0_PARSER_SOFT_KEY_29f,    ILKN_0_PARSER_SOFT_MASK_29f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_29f,     ILKN_0_PARSER_SOFT_VALID_29f,
                                 ILKN_0_PARSER_SOFT_KEY_30f,    ILKN_0_PARSER_SOFT_MASK_30f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_30f,     ILKN_0_PARSER_SOFT_VALID_30f,
                                 ILKN_0_PARSER_SOFT_KEY_31f,    ILKN_0_PARSER_SOFT_MASK_31f,    ILKN_0_PARSER_SOFT_PRIORITY_RESULT_31f,     ILKN_0_PARSER_SOFT_VALID_31f };

        if (offset == 0) 
        {
            regs[0] = NBIH_ILKN_0_PARSER_SOFT_TCAM_0_TO_7r;
            regs[1] = NBIH_ILKN_0_PARSER_SOFT_TCAM_8_TO_15r;
            regs[2] = NBIH_ILKN_0_PARSER_SOFT_TCAM_16_TO_23r;
            regs[3] = NBIH_ILKN_0_PARSER_SOFT_TCAM_24_TO_31r;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            regs[0] = NBIL_ILKN_0_PARSER_SOFT_TCAM_0_TO_7r;
            regs[1] = NBIL_ILKN_0_PARSER_SOFT_TCAM_8_TO_15r;
            regs[2] = NBIL_ILKN_0_PARSER_SOFT_TCAM_16_TO_23r;
            regs[3] = NBIL_ILKN_0_PARSER_SOFT_TCAM_24_TO_31r;
            reg_port = (offset == 2)? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
        if (to_set)
        {
            soc_reg_above_64_field64_set(unit, regs[tcam_reg_index],  reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
            soc_reg_above_64_field64_set(unit, regs[tcam_reg_index],  reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
            soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
            soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
        }
        else 
        {
            tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
            tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
            tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
            tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
        }
    }

    
    if ((offset & 1) || ((num_lanes > 12) && to_set))
    {
        soc_field_t fields[] = { ILKN_1_PARSER_SOFT_KEY_0f,     ILKN_1_PARSER_SOFT_MASK_0f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_0f,      ILKN_1_PARSER_SOFT_VALID_0f,
                                 ILKN_1_PARSER_SOFT_KEY_1f,     ILKN_1_PARSER_SOFT_MASK_1f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_1f,      ILKN_1_PARSER_SOFT_VALID_1f,
                                 ILKN_1_PARSER_SOFT_KEY_2f,     ILKN_1_PARSER_SOFT_MASK_2f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_2f,      ILKN_1_PARSER_SOFT_VALID_2f,
                                 ILKN_1_PARSER_SOFT_KEY_3f,     ILKN_1_PARSER_SOFT_MASK_3f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_3f,      ILKN_1_PARSER_SOFT_VALID_3f,
                                 ILKN_1_PARSER_SOFT_KEY_4f,     ILKN_1_PARSER_SOFT_MASK_4f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_4f,      ILKN_1_PARSER_SOFT_VALID_4f,
                                 ILKN_1_PARSER_SOFT_KEY_5f,     ILKN_1_PARSER_SOFT_MASK_5f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_5f,      ILKN_1_PARSER_SOFT_VALID_5f,
                                 ILKN_1_PARSER_SOFT_KEY_6f,     ILKN_1_PARSER_SOFT_MASK_6f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_6f,      ILKN_1_PARSER_SOFT_VALID_6f,
                                 ILKN_1_PARSER_SOFT_KEY_7f,     ILKN_1_PARSER_SOFT_MASK_7f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_7f,      ILKN_1_PARSER_SOFT_VALID_7f,
                                 ILKN_1_PARSER_SOFT_KEY_8f,     ILKN_1_PARSER_SOFT_MASK_8f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_8f,      ILKN_1_PARSER_SOFT_VALID_8f,
                                 ILKN_1_PARSER_SOFT_KEY_9f,     ILKN_1_PARSER_SOFT_MASK_9f,     ILKN_1_PARSER_SOFT_PRIORITY_RESULT_9f,      ILKN_1_PARSER_SOFT_VALID_9f,
                                 ILKN_1_PARSER_SOFT_KEY_10f,    ILKN_1_PARSER_SOFT_MASK_10f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_10f,     ILKN_1_PARSER_SOFT_VALID_10f,
                                 ILKN_1_PARSER_SOFT_KEY_11f,    ILKN_1_PARSER_SOFT_MASK_11f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_11f,     ILKN_1_PARSER_SOFT_VALID_11f,
                                 ILKN_1_PARSER_SOFT_KEY_12f,    ILKN_1_PARSER_SOFT_MASK_12f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_12f,     ILKN_1_PARSER_SOFT_VALID_12f,
                                 ILKN_1_PARSER_SOFT_KEY_13f,    ILKN_1_PARSER_SOFT_MASK_13f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_13f,     ILKN_1_PARSER_SOFT_VALID_13f,
                                 ILKN_1_PARSER_SOFT_KEY_14f,    ILKN_1_PARSER_SOFT_MASK_14f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_14f,     ILKN_1_PARSER_SOFT_VALID_14f,
                                 ILKN_1_PARSER_SOFT_KEY_15f,    ILKN_1_PARSER_SOFT_MASK_15f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_15f,     ILKN_1_PARSER_SOFT_VALID_15f,
                                 ILKN_1_PARSER_SOFT_KEY_16f,    ILKN_1_PARSER_SOFT_MASK_16f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_16f,     ILKN_1_PARSER_SOFT_VALID_16f,
                                 ILKN_1_PARSER_SOFT_KEY_17f,    ILKN_1_PARSER_SOFT_MASK_17f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_17f,     ILKN_1_PARSER_SOFT_VALID_17f,
                                 ILKN_1_PARSER_SOFT_KEY_18f,    ILKN_1_PARSER_SOFT_MASK_18f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_18f,     ILKN_1_PARSER_SOFT_VALID_18f,
                                 ILKN_1_PARSER_SOFT_KEY_19f,    ILKN_1_PARSER_SOFT_MASK_19f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_19f,     ILKN_1_PARSER_SOFT_VALID_19f,
                                 ILKN_1_PARSER_SOFT_KEY_20f,    ILKN_1_PARSER_SOFT_MASK_20f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_20f,     ILKN_1_PARSER_SOFT_VALID_20f,
                                 ILKN_1_PARSER_SOFT_KEY_21f,    ILKN_1_PARSER_SOFT_MASK_21f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_21f,     ILKN_1_PARSER_SOFT_VALID_21f,
                                 ILKN_1_PARSER_SOFT_KEY_22f,    ILKN_1_PARSER_SOFT_MASK_22f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_22f,     ILKN_1_PARSER_SOFT_VALID_22f,
                                 ILKN_1_PARSER_SOFT_KEY_23f,    ILKN_1_PARSER_SOFT_MASK_23f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_23f,     ILKN_1_PARSER_SOFT_VALID_23f,
                                 ILKN_1_PARSER_SOFT_KEY_24f,    ILKN_1_PARSER_SOFT_MASK_24f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_24f,     ILKN_1_PARSER_SOFT_VALID_24f,
                                 ILKN_1_PARSER_SOFT_KEY_25f,    ILKN_1_PARSER_SOFT_MASK_25f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_25f,     ILKN_1_PARSER_SOFT_VALID_25f,
                                 ILKN_1_PARSER_SOFT_KEY_26f,    ILKN_1_PARSER_SOFT_MASK_26f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_26f,     ILKN_1_PARSER_SOFT_VALID_26f,
                                 ILKN_1_PARSER_SOFT_KEY_27f,    ILKN_1_PARSER_SOFT_MASK_27f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_27f,     ILKN_1_PARSER_SOFT_VALID_27f,
                                 ILKN_1_PARSER_SOFT_KEY_28f,    ILKN_1_PARSER_SOFT_MASK_28f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_28f,     ILKN_1_PARSER_SOFT_VALID_28f,
                                 ILKN_1_PARSER_SOFT_KEY_29f,    ILKN_1_PARSER_SOFT_MASK_29f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_29f,     ILKN_1_PARSER_SOFT_VALID_29f,
                                 ILKN_1_PARSER_SOFT_KEY_30f,    ILKN_1_PARSER_SOFT_MASK_30f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_30f,     ILKN_1_PARSER_SOFT_VALID_30f,
                                 ILKN_1_PARSER_SOFT_KEY_31f,    ILKN_1_PARSER_SOFT_MASK_31f,    ILKN_1_PARSER_SOFT_PRIORITY_RESULT_31f,     ILKN_1_PARSER_SOFT_VALID_31f };

        if ((offset == 0) || (offset == 1)) 
        {
            regs[0] = NBIH_ILKN_1_PARSER_SOFT_TCAM_0_TO_7r;
            regs[1] = NBIH_ILKN_1_PARSER_SOFT_TCAM_8_TO_15r;
            regs[2] = NBIH_ILKN_1_PARSER_SOFT_TCAM_16_TO_23r;
            regs[3] = NBIH_ILKN_1_PARSER_SOFT_TCAM_24_TO_31r;
            reg_port = REG_PORT_ANY;
        }
        else 
        {
            regs[0] = NBIL_ILKN_1_PARSER_SOFT_TCAM_0_TO_7r;
            regs[1] = NBIL_ILKN_1_PARSER_SOFT_TCAM_8_TO_15r;
            regs[2] = NBIL_ILKN_1_PARSER_SOFT_TCAM_16_TO_23r;
            regs[3] = NBIL_ILKN_1_PARSER_SOFT_TCAM_24_TO_31r;
            reg_port = ((offset == 2) || (offset == 3))? 0 : 1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
        if (to_set)
        {
            soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
            soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
            soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
            soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
        }
        else 
        {
            tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
            tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
            tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
            tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf0_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 nof_lanes_nbi;
    soc_reg_above_64_val_t reg_above64_val;
    int first_phy = -1, reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4;
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF];

    soc_field_t fields[] = { QMLF_0_PARSER_SOFT_KEYf,       QMLF_0_PARSER_SOFT_MASKf,       QMLF_0_PARSER_SOFT_PRIORITY_RESULTf,        QMLF_0_PARSER_SOFT_VALIDf,
                             QMLF_0_PARSER_SOFT_KEY_1f,     QMLF_0_PARSER_SOFT_MASK_1f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_1f,      QMLF_0_PARSER_SOFT_VALID_1f,
                             QMLF_0_PARSER_SOFT_KEY_2f,     QMLF_0_PARSER_SOFT_MASK_2f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_2f,      QMLF_0_PARSER_SOFT_VALID_2f,
                             QMLF_0_PARSER_SOFT_KEY_3f,     QMLF_0_PARSER_SOFT_MASK_3f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_3f,      QMLF_0_PARSER_SOFT_VALID_3f,
                             QMLF_0_PARSER_SOFT_KEY_4f,     QMLF_0_PARSER_SOFT_MASK_4f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_4f,      QMLF_0_PARSER_SOFT_VALID_4f,
                             QMLF_0_PARSER_SOFT_KEY_5f,     QMLF_0_PARSER_SOFT_MASK_5f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_5f,      QMLF_0_PARSER_SOFT_VALID_5f,
                             QMLF_0_PARSER_SOFT_KEY_6f,     QMLF_0_PARSER_SOFT_MASK_6f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_6f,      QMLF_0_PARSER_SOFT_VALID_6f,
                             QMLF_0_PARSER_SOFT_KEY_7f,     QMLF_0_PARSER_SOFT_MASK_7f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_7f,      QMLF_0_PARSER_SOFT_VALID_7f,
                             QMLF_0_PARSER_SOFT_KEY_8f,     QMLF_0_PARSER_SOFT_MASK_8f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_8f,      QMLF_0_PARSER_SOFT_VALID_8f,
                             QMLF_0_PARSER_SOFT_KEY_9f,     QMLF_0_PARSER_SOFT_MASK_9f,     QMLF_0_PARSER_SOFT_PRIORITY_RESULT_9f,      QMLF_0_PARSER_SOFT_VALID_9f,
                             QMLF_0_PARSER_SOFT_KEY_10f,    QMLF_0_PARSER_SOFT_MASK_10f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_10f,     QMLF_0_PARSER_SOFT_VALID_10f,
                             QMLF_0_PARSER_SOFT_KEY_11f,    QMLF_0_PARSER_SOFT_MASK_11f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_11f,     QMLF_0_PARSER_SOFT_VALID_11f,
                             QMLF_0_PARSER_SOFT_KEY_12f,    QMLF_0_PARSER_SOFT_MASK_12f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_12f,     QMLF_0_PARSER_SOFT_VALID_12f,
                             QMLF_0_PARSER_SOFT_KEY_13f,    QMLF_0_PARSER_SOFT_MASK_13f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_13f,     QMLF_0_PARSER_SOFT_VALID_13f,
                             QMLF_0_PARSER_SOFT_KEY_14f,    QMLF_0_PARSER_SOFT_MASK_14f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_14f,     QMLF_0_PARSER_SOFT_VALID_14f,
                             QMLF_0_PARSER_SOFT_KEY_15f,    QMLF_0_PARSER_SOFT_MASK_15f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_15f,     QMLF_0_PARSER_SOFT_VALID_15f,
                             QMLF_0_PARSER_SOFT_KEY_16f,    QMLF_0_PARSER_SOFT_MASK_16f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_16f,     QMLF_0_PARSER_SOFT_VALID_16f,
                             QMLF_0_PARSER_SOFT_KEY_17f,    QMLF_0_PARSER_SOFT_MASK_17f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_17f,     QMLF_0_PARSER_SOFT_VALID_17f,
                             QMLF_0_PARSER_SOFT_KEY_18f,    QMLF_0_PARSER_SOFT_MASK_18f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_18f,     QMLF_0_PARSER_SOFT_VALID_18f,
                             QMLF_0_PARSER_SOFT_KEY_19f,    QMLF_0_PARSER_SOFT_MASK_19f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_19f,     QMLF_0_PARSER_SOFT_VALID_19f,
                             QMLF_0_PARSER_SOFT_KEY_20f,    QMLF_0_PARSER_SOFT_MASK_20f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_20f,     QMLF_0_PARSER_SOFT_VALID_20f,
                             QMLF_0_PARSER_SOFT_KEY_21f,    QMLF_0_PARSER_SOFT_MASK_21f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_21f,     QMLF_0_PARSER_SOFT_VALID_21f,
                             QMLF_0_PARSER_SOFT_KEY_22f,    QMLF_0_PARSER_SOFT_MASK_22f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_22f,     QMLF_0_PARSER_SOFT_VALID_22f,
                             QMLF_0_PARSER_SOFT_KEY_23f,    QMLF_0_PARSER_SOFT_MASK_23f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_23f,     QMLF_0_PARSER_SOFT_VALID_23f,
                             QMLF_0_PARSER_SOFT_KEY_24f,    QMLF_0_PARSER_SOFT_MASK_24f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_24f,     QMLF_0_PARSER_SOFT_VALID_24f,
                             QMLF_0_PARSER_SOFT_KEY_25f,    QMLF_0_PARSER_SOFT_MASK_25f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_25f,     QMLF_0_PARSER_SOFT_VALID_25f,
                             QMLF_0_PARSER_SOFT_KEY_26f,    QMLF_0_PARSER_SOFT_MASK_26f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_26f,     QMLF_0_PARSER_SOFT_VALID_26f,
                             QMLF_0_PARSER_SOFT_KEY_27f,    QMLF_0_PARSER_SOFT_MASK_27f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_27f,     QMLF_0_PARSER_SOFT_VALID_27f,
                             QMLF_0_PARSER_SOFT_KEY_28f,    QMLF_0_PARSER_SOFT_MASK_28f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_28f,     QMLF_0_PARSER_SOFT_VALID_28f,
                             QMLF_0_PARSER_SOFT_KEY_29f,    QMLF_0_PARSER_SOFT_MASK_29f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_29f,     QMLF_0_PARSER_SOFT_VALID_29f,
                             QMLF_0_PARSER_SOFT_KEY_30f,    QMLF_0_PARSER_SOFT_MASK_30f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_30f,     QMLF_0_PARSER_SOFT_VALID_30f,
                             QMLF_0_PARSER_SOFT_KEY_31f,    QMLF_0_PARSER_SOFT_MASK_31f,    QMLF_0_PARSER_SOFT_PRIORITY_RESULT_31f,     QMLF_0_PARSER_SOFT_VALID_31f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));

    tcam_reg_index = key_index/tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_0_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIH_QMLF_0_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIH_QMLF_0_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIH_QMLF_0_PARSER_SOFT_TCAM_24_TO_31r;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_0_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIL_QMLF_0_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIL_QMLF_0_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIL_QMLF_0_PARSER_SOFT_TCAM_24_TO_31r;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
        tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf1_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 nof_lanes_nbi;
    soc_reg_above_64_val_t reg_above64_val;
    int first_phy = -1, reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4;
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF];

    soc_field_t fields[] = { QMLF_1_PARSER_SOFT_KEY_0f,     QMLF_1_PARSER_SOFT_MASK_0f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_0f,      QMLF_1_PARSER_SOFT_VALID_0f,
                             QMLF_1_PARSER_SOFT_KEY_1f,     QMLF_1_PARSER_SOFT_MASK_1f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_1f,      QMLF_1_PARSER_SOFT_VALID_1f,
                             QMLF_1_PARSER_SOFT_KEY_2f,     QMLF_1_PARSER_SOFT_MASK_2f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_2f,      QMLF_1_PARSER_SOFT_VALID_2f,
                             QMLF_1_PARSER_SOFT_KEY_3f,     QMLF_1_PARSER_SOFT_MASK_3f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_3f,      QMLF_1_PARSER_SOFT_VALID_3f,
                             QMLF_1_PARSER_SOFT_KEY_4f,     QMLF_1_PARSER_SOFT_MASK_4f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_4f,      QMLF_1_PARSER_SOFT_VALID_4f,
                             QMLF_1_PARSER_SOFT_KEY_5f,     QMLF_1_PARSER_SOFT_MASK_5f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_5f,      QMLF_1_PARSER_SOFT_VALID_5f,
                             QMLF_1_PARSER_SOFT_KEY_6f,     QMLF_1_PARSER_SOFT_MASK_6f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_6f,      QMLF_1_PARSER_SOFT_VALID_6f,
                             QMLF_1_PARSER_SOFT_KEY_7f,     QMLF_1_PARSER_SOFT_MASK_7f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_7f,      QMLF_1_PARSER_SOFT_VALID_7f,
                             QMLF_1_PARSER_SOFT_KEY_8f,     QMLF_1_PARSER_SOFT_MASK_8f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_8f,      QMLF_1_PARSER_SOFT_VALID_8f,
                             QMLF_1_PARSER_SOFT_KEY_9f,     QMLF_1_PARSER_SOFT_MASK_9f,     QMLF_1_PARSER_SOFT_PRIORITY_RESULT_9f,      QMLF_1_PARSER_SOFT_VALID_9f,
                             QMLF_1_PARSER_SOFT_KEY_10f,    QMLF_1_PARSER_SOFT_MASK_10f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_10f,     QMLF_1_PARSER_SOFT_VALID_10f,
                             QMLF_1_PARSER_SOFT_KEY_11f,    QMLF_1_PARSER_SOFT_MASK_11f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_11f,     QMLF_1_PARSER_SOFT_VALID_11f,
                             QMLF_1_PARSER_SOFT_KEY_12f,    QMLF_1_PARSER_SOFT_MASK_12f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_12f,     QMLF_1_PARSER_SOFT_VALID_12f,
                             QMLF_1_PARSER_SOFT_KEY_13f,    QMLF_1_PARSER_SOFT_MASK_13f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_13f,     QMLF_1_PARSER_SOFT_VALID_13f,
                             QMLF_1_PARSER_SOFT_KEY_14f,    QMLF_1_PARSER_SOFT_MASK_14f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_14f,     QMLF_1_PARSER_SOFT_VALID_14f,
                             QMLF_1_PARSER_SOFT_KEY_15f,    QMLF_1_PARSER_SOFT_MASK_15f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_15f,     QMLF_1_PARSER_SOFT_VALID_15f,
                             QMLF_1_PARSER_SOFT_KEY_16f,    QMLF_1_PARSER_SOFT_MASK_16f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_16f,     QMLF_1_PARSER_SOFT_VALID_16f,
                             QMLF_1_PARSER_SOFT_KEY_17f,    QMLF_1_PARSER_SOFT_MASK_17f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_17f,     QMLF_1_PARSER_SOFT_VALID_17f,
                             QMLF_1_PARSER_SOFT_KEY_18f,    QMLF_1_PARSER_SOFT_MASK_18f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_18f,     QMLF_1_PARSER_SOFT_VALID_18f,
                             QMLF_1_PARSER_SOFT_KEY_19f,    QMLF_1_PARSER_SOFT_MASK_19f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_19f,     QMLF_1_PARSER_SOFT_VALID_19f,
                             QMLF_1_PARSER_SOFT_KEY_20f,    QMLF_1_PARSER_SOFT_MASK_20f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_20f,     QMLF_1_PARSER_SOFT_VALID_20f,
                             QMLF_1_PARSER_SOFT_KEY_21f,    QMLF_1_PARSER_SOFT_MASK_21f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_21f,     QMLF_1_PARSER_SOFT_VALID_21f,
                             QMLF_1_PARSER_SOFT_KEY_22f,    QMLF_1_PARSER_SOFT_MASK_22f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_22f,     QMLF_1_PARSER_SOFT_VALID_22f,
                             QMLF_1_PARSER_SOFT_KEY_23f,    QMLF_1_PARSER_SOFT_MASK_23f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_23f,     QMLF_1_PARSER_SOFT_VALID_23f,
                             QMLF_1_PARSER_SOFT_KEY_24f,    QMLF_1_PARSER_SOFT_MASK_24f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_24f,     QMLF_1_PARSER_SOFT_VALID_24f,
                             QMLF_1_PARSER_SOFT_KEY_25f,    QMLF_1_PARSER_SOFT_MASK_25f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_25f,     QMLF_1_PARSER_SOFT_VALID_25f,
                             QMLF_1_PARSER_SOFT_KEY_26f,    QMLF_1_PARSER_SOFT_MASK_26f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_26f,     QMLF_1_PARSER_SOFT_VALID_26f,
                             QMLF_1_PARSER_SOFT_KEY_27f,    QMLF_1_PARSER_SOFT_MASK_27f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_27f,     QMLF_1_PARSER_SOFT_VALID_27f,
                             QMLF_1_PARSER_SOFT_KEY_28f,    QMLF_1_PARSER_SOFT_MASK_28f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_28f,     QMLF_1_PARSER_SOFT_VALID_28f,
                             QMLF_1_PARSER_SOFT_KEY_29f,    QMLF_1_PARSER_SOFT_MASK_29f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_29f,     QMLF_1_PARSER_SOFT_VALID_29f,
                             QMLF_1_PARSER_SOFT_KEY_30f,    QMLF_1_PARSER_SOFT_MASK_30f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_30f,     QMLF_1_PARSER_SOFT_VALID_30f,
                             QMLF_1_PARSER_SOFT_KEY_31f,    QMLF_1_PARSER_SOFT_MASK_31f,    QMLF_1_PARSER_SOFT_PRIORITY_RESULT_31f,     QMLF_1_PARSER_SOFT_VALID_31f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));

    tcam_reg_index = key_index / tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_1_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIH_QMLF_1_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIH_QMLF_1_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIH_QMLF_1_PARSER_SOFT_TCAM_24_TO_31r;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_1_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIL_QMLF_1_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIL_QMLF_1_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIL_QMLF_1_PARSER_SOFT_TCAM_24_TO_31r;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
        tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
        tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf2_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 nof_lanes_nbi;
    soc_reg_above_64_val_t reg_above64_val;
    int first_phy = -1, reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4;
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF];

    soc_field_t fields[] = { QMLF_2_PARSER_SOFT_KEY_0f,     QMLF_2_PARSER_SOFT_MASK_0f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_0f,      QMLF_2_PARSER_SOFT_VALID_0f,
                             QMLF_2_PARSER_SOFT_KEY_1f,     QMLF_2_PARSER_SOFT_MASK_1f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_1f,      QMLF_2_PARSER_SOFT_VALID_1f,
                             QMLF_2_PARSER_SOFT_KEY_2f,     QMLF_2_PARSER_SOFT_MASK_2f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_2f,      QMLF_2_PARSER_SOFT_VALID_2f,
                             QMLF_2_PARSER_SOFT_KEY_3f,     QMLF_2_PARSER_SOFT_MASK_3f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_3f,      QMLF_2_PARSER_SOFT_VALID_3f,
                             QMLF_2_PARSER_SOFT_KEY_4f,     QMLF_2_PARSER_SOFT_MASK_4f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_4f,      QMLF_2_PARSER_SOFT_VALID_4f,
                             QMLF_2_PARSER_SOFT_KEY_5f,     QMLF_2_PARSER_SOFT_MASK_5f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_5f,      QMLF_2_PARSER_SOFT_VALID_5f,
                             QMLF_2_PARSER_SOFT_KEY_6f,     QMLF_2_PARSER_SOFT_MASK_6f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_6f,      QMLF_2_PARSER_SOFT_VALID_6f,
                             QMLF_2_PARSER_SOFT_KEY_7f,     QMLF_2_PARSER_SOFT_MASK_7f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_7f,      QMLF_2_PARSER_SOFT_VALID_7f,
                             QMLF_2_PARSER_SOFT_KEY_8f,     QMLF_2_PARSER_SOFT_MASK_8f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_8f,      QMLF_2_PARSER_SOFT_VALID_8f,
                             QMLF_2_PARSER_SOFT_KEY_9f,     QMLF_2_PARSER_SOFT_MASK_9f,     QMLF_2_PARSER_SOFT_PRIORITY_RESULT_9f,      QMLF_2_PARSER_SOFT_VALID_9f,
                             QMLF_2_PARSER_SOFT_KEY_10f,    QMLF_2_PARSER_SOFT_MASK_10f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_10f,     QMLF_2_PARSER_SOFT_VALID_10f,
                             QMLF_2_PARSER_SOFT_KEY_11f,    QMLF_2_PARSER_SOFT_MASK_11f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_11f,     QMLF_2_PARSER_SOFT_VALID_11f,
                             QMLF_2_PARSER_SOFT_KEY_12f,    QMLF_2_PARSER_SOFT_MASK_12f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_12f,     QMLF_2_PARSER_SOFT_VALID_12f,
                             QMLF_2_PARSER_SOFT_KEY_13f,    QMLF_2_PARSER_SOFT_MASK_13f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_13f,     QMLF_2_PARSER_SOFT_VALID_13f,
                             QMLF_2_PARSER_SOFT_KEY_14f,    QMLF_2_PARSER_SOFT_MASK_14f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_14f,     QMLF_2_PARSER_SOFT_VALID_14f,
                             QMLF_2_PARSER_SOFT_KEY_15f,    QMLF_2_PARSER_SOFT_MASK_15f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_15f,     QMLF_2_PARSER_SOFT_VALID_15f,
                             QMLF_2_PARSER_SOFT_KEY_16f,    QMLF_2_PARSER_SOFT_MASK_16f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_16f,     QMLF_2_PARSER_SOFT_VALID_16f,
                             QMLF_2_PARSER_SOFT_KEY_17f,    QMLF_2_PARSER_SOFT_MASK_17f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_17f,     QMLF_2_PARSER_SOFT_VALID_17f,
                             QMLF_2_PARSER_SOFT_KEY_18f,    QMLF_2_PARSER_SOFT_MASK_18f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_18f,     QMLF_2_PARSER_SOFT_VALID_18f,
                             QMLF_2_PARSER_SOFT_KEY_19f,    QMLF_2_PARSER_SOFT_MASK_19f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_19f,     QMLF_2_PARSER_SOFT_VALID_19f,
                             QMLF_2_PARSER_SOFT_KEY_20f,    QMLF_2_PARSER_SOFT_MASK_20f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_20f,     QMLF_2_PARSER_SOFT_VALID_20f,
                             QMLF_2_PARSER_SOFT_KEY_21f,    QMLF_2_PARSER_SOFT_MASK_21f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_21f,     QMLF_2_PARSER_SOFT_VALID_21f,
                             QMLF_2_PARSER_SOFT_KEY_22f,    QMLF_2_PARSER_SOFT_MASK_22f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_22f,     QMLF_2_PARSER_SOFT_VALID_22f,
                             QMLF_2_PARSER_SOFT_KEY_23f,    QMLF_2_PARSER_SOFT_MASK_23f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_23f,     QMLF_2_PARSER_SOFT_VALID_23f,
                             QMLF_2_PARSER_SOFT_KEY_24f,    QMLF_2_PARSER_SOFT_MASK_24f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_24f,     QMLF_2_PARSER_SOFT_VALID_24f,
                             QMLF_2_PARSER_SOFT_KEY_25f,    QMLF_2_PARSER_SOFT_MASK_25f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_25f,     QMLF_2_PARSER_SOFT_VALID_25f,
                             QMLF_2_PARSER_SOFT_KEY_26f,    QMLF_2_PARSER_SOFT_MASK_26f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_26f,     QMLF_2_PARSER_SOFT_VALID_26f,
                             QMLF_2_PARSER_SOFT_KEY_27f,    QMLF_2_PARSER_SOFT_MASK_27f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_27f,     QMLF_2_PARSER_SOFT_VALID_27f,
                             QMLF_2_PARSER_SOFT_KEY_28f,    QMLF_2_PARSER_SOFT_MASK_28f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_28f,     QMLF_2_PARSER_SOFT_VALID_28f,
                             QMLF_2_PARSER_SOFT_KEY_29f,    QMLF_2_PARSER_SOFT_MASK_29f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_29f,     QMLF_2_PARSER_SOFT_VALID_29f,
                             QMLF_2_PARSER_SOFT_KEY_30f,    QMLF_2_PARSER_SOFT_MASK_30f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_30f,     QMLF_2_PARSER_SOFT_VALID_30f,
                             QMLF_2_PARSER_SOFT_KEY_31f,    QMLF_2_PARSER_SOFT_MASK_31f,    QMLF_2_PARSER_SOFT_PRIORITY_RESULT_31f,     QMLF_2_PARSER_SOFT_VALID_31f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));

    tcam_reg_index = key_index/tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_2_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIH_QMLF_2_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIH_QMLF_2_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIH_QMLF_2_PARSER_SOFT_TCAM_24_TO_31r;

    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_2_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIL_QMLF_2_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIL_QMLF_2_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIL_QMLF_2_PARSER_SOFT_TCAM_24_TO_31r;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
        tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf3_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 nof_lanes_nbi;
    soc_reg_above_64_val_t reg_above64_val;
    int first_phy = -1, reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4;
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF];

    soc_field_t fields[] = { QMLF_3_PARSER_SOFT_KEY_0f,     QMLF_3_PARSER_SOFT_MASK_0f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_0f,      QMLF_3_PARSER_SOFT_VALID_0f,
                             QMLF_3_PARSER_SOFT_KEY_1f,     QMLF_3_PARSER_SOFT_MASK_1f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_1f,      QMLF_3_PARSER_SOFT_VALID_1f,
                             QMLF_3_PARSER_SOFT_KEY_2f,     QMLF_3_PARSER_SOFT_MASK_2f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_2f,      QMLF_3_PARSER_SOFT_VALID_2f,
                             QMLF_3_PARSER_SOFT_KEY_3f,     QMLF_3_PARSER_SOFT_MASK_3f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_3f,      QMLF_3_PARSER_SOFT_VALID_3f,
                             QMLF_3_PARSER_SOFT_KEY_4f,     QMLF_3_PARSER_SOFT_MASK_4f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_4f,      QMLF_3_PARSER_SOFT_VALID_4f,
                             QMLF_3_PARSER_SOFT_KEY_5f,     QMLF_3_PARSER_SOFT_MASK_5f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_5f,      QMLF_3_PARSER_SOFT_VALID_5f,
                             QMLF_3_PARSER_SOFT_KEY_6f,     QMLF_3_PARSER_SOFT_MASK_6f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_6f,      QMLF_3_PARSER_SOFT_VALID_6f,
                             QMLF_3_PARSER_SOFT_KEY_7f,     QMLF_3_PARSER_SOFT_MASK_7f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_7f,      QMLF_3_PARSER_SOFT_VALID_7f,
                             QMLF_3_PARSER_SOFT_KEY_8f,     QMLF_3_PARSER_SOFT_MASK_8f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_8f,      QMLF_3_PARSER_SOFT_VALID_8f,
                             QMLF_3_PARSER_SOFT_KEY_9f,     QMLF_3_PARSER_SOFT_MASK_9f,     QMLF_3_PARSER_SOFT_PRIORITY_RESULT_9f,      QMLF_3_PARSER_SOFT_VALID_9f,
                             QMLF_3_PARSER_SOFT_KEY_10f,    QMLF_3_PARSER_SOFT_MASK_10f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_10f,     QMLF_3_PARSER_SOFT_VALID_10f,
                             QMLF_3_PARSER_SOFT_KEY_11f,    QMLF_3_PARSER_SOFT_MASK_11f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_11f,     QMLF_3_PARSER_SOFT_VALID_11f,
                             QMLF_3_PARSER_SOFT_KEY_12f,    QMLF_3_PARSER_SOFT_MASK_12f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_12f,     QMLF_3_PARSER_SOFT_VALID_12f,
                             QMLF_3_PARSER_SOFT_KEY_13f,    QMLF_3_PARSER_SOFT_MASK_13f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_13f,     QMLF_3_PARSER_SOFT_VALID_13f,
                             QMLF_3_PARSER_SOFT_KEY_14f,    QMLF_3_PARSER_SOFT_MASK_14f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_14f,     QMLF_3_PARSER_SOFT_VALID_14f,
                             QMLF_3_PARSER_SOFT_KEY_15f,    QMLF_3_PARSER_SOFT_MASK_15f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_15f,     QMLF_3_PARSER_SOFT_VALID_15f,
                             QMLF_3_PARSER_SOFT_KEY_16f,    QMLF_3_PARSER_SOFT_MASK_16f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_16f,     QMLF_3_PARSER_SOFT_VALID_16f,
                             QMLF_3_PARSER_SOFT_KEY_17f,    QMLF_3_PARSER_SOFT_MASK_17f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_17f,     QMLF_3_PARSER_SOFT_VALID_17f,
                             QMLF_3_PARSER_SOFT_KEY_18f,    QMLF_3_PARSER_SOFT_MASK_18f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_18f,     QMLF_3_PARSER_SOFT_VALID_18f,
                             QMLF_3_PARSER_SOFT_KEY_19f,    QMLF_3_PARSER_SOFT_MASK_19f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_19f,     QMLF_3_PARSER_SOFT_VALID_19f,
                             QMLF_3_PARSER_SOFT_KEY_20f,    QMLF_3_PARSER_SOFT_MASK_20f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_20f,     QMLF_3_PARSER_SOFT_VALID_20f,
                             QMLF_3_PARSER_SOFT_KEY_21f,    QMLF_3_PARSER_SOFT_MASK_21f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_21f,     QMLF_3_PARSER_SOFT_VALID_21f,
                             QMLF_3_PARSER_SOFT_KEY_22f,    QMLF_3_PARSER_SOFT_MASK_22f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_22f,     QMLF_3_PARSER_SOFT_VALID_22f,
                             QMLF_3_PARSER_SOFT_KEY_23f,    QMLF_3_PARSER_SOFT_MASK_23f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_23f,     QMLF_3_PARSER_SOFT_VALID_23f,
                             QMLF_3_PARSER_SOFT_KEY_24f,    QMLF_3_PARSER_SOFT_MASK_24f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_24f,     QMLF_3_PARSER_SOFT_VALID_24f,
                             QMLF_3_PARSER_SOFT_KEY_25f,    QMLF_3_PARSER_SOFT_MASK_25f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_25f,     QMLF_3_PARSER_SOFT_VALID_25f,
                             QMLF_3_PARSER_SOFT_KEY_26f,    QMLF_3_PARSER_SOFT_MASK_26f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_26f,     QMLF_3_PARSER_SOFT_VALID_26f,
                             QMLF_3_PARSER_SOFT_KEY_27f,    QMLF_3_PARSER_SOFT_MASK_27f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_27f,     QMLF_3_PARSER_SOFT_VALID_27f,
                             QMLF_3_PARSER_SOFT_KEY_28f,    QMLF_3_PARSER_SOFT_MASK_28f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_28f,     QMLF_3_PARSER_SOFT_VALID_28f,
                             QMLF_3_PARSER_SOFT_KEY_29f,    QMLF_3_PARSER_SOFT_MASK_29f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_29f,     QMLF_3_PARSER_SOFT_VALID_29f,
                             QMLF_3_PARSER_SOFT_KEY_30f,    QMLF_3_PARSER_SOFT_MASK_30f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_30f,     QMLF_3_PARSER_SOFT_VALID_30f,
                             QMLF_3_PARSER_SOFT_KEY_31f,    QMLF_3_PARSER_SOFT_MASK_31f,    QMLF_3_PARSER_SOFT_PRIORITY_RESULT_31f,     QMLF_3_PARSER_SOFT_VALID_31f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));

    tcam_reg_index = key_index/tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_3_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIH_QMLF_3_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIH_QMLF_3_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIH_QMLF_3_PARSER_SOFT_TCAM_24_TO_31r;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_3_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIL_QMLF_3_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIL_QMLF_3_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIL_QMLF_3_PARSER_SOFT_TCAM_24_TO_31r;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index],  reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index],  reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
        tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf4_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 nof_lanes_nbi;
    soc_reg_above_64_val_t reg_above64_val;
    int first_phy = -1, reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4;
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF];

    soc_field_t fields[] = { QMLF_4_PARSER_SOFT_KEY_0f,     QMLF_4_PARSER_SOFT_MASK_0f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_0f,      QMLF_4_PARSER_SOFT_VALID_0f,
                             QMLF_4_PARSER_SOFT_KEY_1f,     QMLF_4_PARSER_SOFT_MASK_1f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_1f,      QMLF_4_PARSER_SOFT_VALID_1f,
                             QMLF_4_PARSER_SOFT_KEY_2f,     QMLF_4_PARSER_SOFT_MASK_2f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_2f,      QMLF_4_PARSER_SOFT_VALID_2f,
                             QMLF_4_PARSER_SOFT_KEY_3f,     QMLF_4_PARSER_SOFT_MASK_3f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_3f,      QMLF_4_PARSER_SOFT_VALID_3f,
                             QMLF_4_PARSER_SOFT_KEY_4f,     QMLF_4_PARSER_SOFT_MASK_4f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_4f,      QMLF_4_PARSER_SOFT_VALID_4f,
                             QMLF_4_PARSER_SOFT_KEY_5f,     QMLF_4_PARSER_SOFT_MASK_5f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_5f,      QMLF_4_PARSER_SOFT_VALID_5f,
                             QMLF_4_PARSER_SOFT_KEY_6f,     QMLF_4_PARSER_SOFT_MASK_6f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_6f,      QMLF_4_PARSER_SOFT_VALID_6f,
                             QMLF_4_PARSER_SOFT_KEY_7f,     QMLF_4_PARSER_SOFT_MASK_7f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_7f,      QMLF_4_PARSER_SOFT_VALID_7f,
                             QMLF_4_PARSER_SOFT_KEY_8f,     QMLF_4_PARSER_SOFT_MASK_8f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_8f,      QMLF_4_PARSER_SOFT_VALID_8f,
                             QMLF_4_PARSER_SOFT_KEY_9f,     QMLF_4_PARSER_SOFT_MASK_9f,     QMLF_4_PARSER_SOFT_PRIORITY_RESULT_9f,      QMLF_4_PARSER_SOFT_VALID_9f,
                             QMLF_4_PARSER_SOFT_KEY_10f,    QMLF_4_PARSER_SOFT_MASK_10f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_10f,     QMLF_4_PARSER_SOFT_VALID_10f,
                             QMLF_4_PARSER_SOFT_KEY_11f,    QMLF_4_PARSER_SOFT_MASK_11f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_11f,     QMLF_4_PARSER_SOFT_VALID_11f,
                             QMLF_4_PARSER_SOFT_KEY_12f,    QMLF_4_PARSER_SOFT_MASK_12f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_12f,     QMLF_4_PARSER_SOFT_VALID_12f,
                             QMLF_4_PARSER_SOFT_KEY_13f,    QMLF_4_PARSER_SOFT_MASK_13f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_13f,     QMLF_4_PARSER_SOFT_VALID_13f,
                             QMLF_4_PARSER_SOFT_KEY_14f,    QMLF_4_PARSER_SOFT_MASK_14f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_14f,     QMLF_4_PARSER_SOFT_VALID_14f,
                             QMLF_4_PARSER_SOFT_KEY_15f,    QMLF_4_PARSER_SOFT_MASK_15f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_15f,     QMLF_4_PARSER_SOFT_VALID_15f,
                             QMLF_4_PARSER_SOFT_KEY_16f,    QMLF_4_PARSER_SOFT_MASK_16f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_16f,     QMLF_4_PARSER_SOFT_VALID_16f,
                             QMLF_4_PARSER_SOFT_KEY_17f,    QMLF_4_PARSER_SOFT_MASK_17f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_17f,     QMLF_4_PARSER_SOFT_VALID_17f,
                             QMLF_4_PARSER_SOFT_KEY_18f,    QMLF_4_PARSER_SOFT_MASK_18f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_18f,     QMLF_4_PARSER_SOFT_VALID_18f,
                             QMLF_4_PARSER_SOFT_KEY_19f,    QMLF_4_PARSER_SOFT_MASK_19f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_19f,     QMLF_4_PARSER_SOFT_VALID_19f,
                             QMLF_4_PARSER_SOFT_KEY_20f,    QMLF_4_PARSER_SOFT_MASK_20f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_20f,     QMLF_4_PARSER_SOFT_VALID_20f,
                             QMLF_4_PARSER_SOFT_KEY_21f,    QMLF_4_PARSER_SOFT_MASK_21f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_21f,     QMLF_4_PARSER_SOFT_VALID_21f,
                             QMLF_4_PARSER_SOFT_KEY_22f,    QMLF_4_PARSER_SOFT_MASK_22f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_22f,     QMLF_4_PARSER_SOFT_VALID_22f,
                             QMLF_4_PARSER_SOFT_KEY_23f,    QMLF_4_PARSER_SOFT_MASK_23f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_23f,     QMLF_4_PARSER_SOFT_VALID_23f,
                             QMLF_4_PARSER_SOFT_KEY_24f,    QMLF_4_PARSER_SOFT_MASK_24f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_24f,     QMLF_4_PARSER_SOFT_VALID_24f,
                             QMLF_4_PARSER_SOFT_KEY_25f,    QMLF_4_PARSER_SOFT_MASK_25f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_25f,     QMLF_4_PARSER_SOFT_VALID_25f,
                             QMLF_4_PARSER_SOFT_KEY_26f,    QMLF_4_PARSER_SOFT_MASK_26f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_26f,     QMLF_4_PARSER_SOFT_VALID_26f,
                             QMLF_4_PARSER_SOFT_KEY_27f,    QMLF_4_PARSER_SOFT_MASK_27f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_27f,     QMLF_4_PARSER_SOFT_VALID_27f,
                             QMLF_4_PARSER_SOFT_KEY_28f,    QMLF_4_PARSER_SOFT_MASK_28f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_28f,     QMLF_4_PARSER_SOFT_VALID_28f,
                             QMLF_4_PARSER_SOFT_KEY_29f,    QMLF_4_PARSER_SOFT_MASK_29f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_29f,     QMLF_4_PARSER_SOFT_VALID_29f,
                             QMLF_4_PARSER_SOFT_KEY_30f,    QMLF_4_PARSER_SOFT_MASK_30f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_30f,     QMLF_4_PARSER_SOFT_VALID_30f,
                             QMLF_4_PARSER_SOFT_KEY_31f,    QMLF_4_PARSER_SOFT_MASK_31f,    QMLF_4_PARSER_SOFT_PRIORITY_RESULT_31f,     QMLF_4_PARSER_SOFT_VALID_31f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));

    tcam_reg_index = key_index/tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_4_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIH_QMLF_4_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIH_QMLF_4_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIH_QMLF_4_PARSER_SOFT_TCAM_24_TO_31r;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;

        regs[0] = NBIL_QMLF_4_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIL_QMLF_4_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIL_QMLF_4_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIL_QMLF_4_PARSER_SOFT_TCAM_24_TO_31r;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
        tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf5_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    uint32 nof_lanes_nbi;
    soc_reg_above_64_val_t reg_above64_val;
    int first_phy = -1, reg_port, tcam_reg_index, first_field_index_in_arr;
    const int tcam_entries_per_reg = 8, fields_per_tcam_entry = 4;
    soc_reg_t regs[SOC_JER_PLUS_PARSER_SOFT_TCAM_REGS_PER_QMLF];

    soc_field_t fields[] = { QMLF_5_PARSER_SOFT_KEY_0f,     QMLF_5_PARSER_SOFT_MASK_0f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_0f,      QMLF_5_PARSER_SOFT_VALID_0f,
                             QMLF_5_PARSER_SOFT_KEY_1f,     QMLF_5_PARSER_SOFT_MASK_1f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_1f,      QMLF_5_PARSER_SOFT_VALID_1f,
                             QMLF_5_PARSER_SOFT_KEY_2f,     QMLF_5_PARSER_SOFT_MASK_2f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_2f,      QMLF_5_PARSER_SOFT_VALID_2f,
                             QMLF_5_PARSER_SOFT_KEY_3f,     QMLF_5_PARSER_SOFT_MASK_3f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_3f,      QMLF_5_PARSER_SOFT_VALID_3f,
                             QMLF_5_PARSER_SOFT_KEY_4f,     QMLF_5_PARSER_SOFT_MASK_4f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_4f,      QMLF_5_PARSER_SOFT_VALID_4f,
                             QMLF_5_PARSER_SOFT_KEY_5f,     QMLF_5_PARSER_SOFT_MASK_5f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_5f,      QMLF_5_PARSER_SOFT_VALID_5f,
                             QMLF_5_PARSER_SOFT_KEY_6f,     QMLF_5_PARSER_SOFT_MASK_6f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_6f,      QMLF_5_PARSER_SOFT_VALID_6f,
                             QMLF_5_PARSER_SOFT_KEY_7f,     QMLF_5_PARSER_SOFT_MASK_7f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_7f,      QMLF_5_PARSER_SOFT_VALID_7f,
                             QMLF_5_PARSER_SOFT_KEY_8f,     QMLF_5_PARSER_SOFT_MASK_8f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_8f,      QMLF_5_PARSER_SOFT_VALID_8f,
                             QMLF_5_PARSER_SOFT_KEY_9f,     QMLF_5_PARSER_SOFT_MASK_9f,     QMLF_5_PARSER_SOFT_PRIORITY_RESULT_9f,      QMLF_5_PARSER_SOFT_VALID_9f,
                             QMLF_5_PARSER_SOFT_KEY_10f,    QMLF_5_PARSER_SOFT_MASK_10f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_10f,     QMLF_5_PARSER_SOFT_VALID_10f,
                             QMLF_5_PARSER_SOFT_KEY_11f,    QMLF_5_PARSER_SOFT_MASK_11f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_11f,     QMLF_5_PARSER_SOFT_VALID_11f,
                             QMLF_5_PARSER_SOFT_KEY_12f,    QMLF_5_PARSER_SOFT_MASK_12f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_12f,     QMLF_5_PARSER_SOFT_VALID_12f,
                             QMLF_5_PARSER_SOFT_KEY_13f,    QMLF_5_PARSER_SOFT_MASK_13f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_13f,     QMLF_5_PARSER_SOFT_VALID_13f,
                             QMLF_5_PARSER_SOFT_KEY_14f,    QMLF_5_PARSER_SOFT_MASK_14f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_14f,     QMLF_5_PARSER_SOFT_VALID_14f,
                             QMLF_5_PARSER_SOFT_KEY_15f,    QMLF_5_PARSER_SOFT_MASK_15f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_15f,     QMLF_5_PARSER_SOFT_VALID_15f,
                             QMLF_5_PARSER_SOFT_KEY_16f,    QMLF_5_PARSER_SOFT_MASK_16f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_16f,     QMLF_5_PARSER_SOFT_VALID_16f,
                             QMLF_5_PARSER_SOFT_KEY_17f,    QMLF_5_PARSER_SOFT_MASK_17f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_17f,     QMLF_5_PARSER_SOFT_VALID_17f,
                             QMLF_5_PARSER_SOFT_KEY_18f,    QMLF_5_PARSER_SOFT_MASK_18f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_18f,     QMLF_5_PARSER_SOFT_VALID_18f,
                             QMLF_5_PARSER_SOFT_KEY_19f,    QMLF_5_PARSER_SOFT_MASK_19f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_19f,     QMLF_5_PARSER_SOFT_VALID_19f,
                             QMLF_5_PARSER_SOFT_KEY_20f,    QMLF_5_PARSER_SOFT_MASK_20f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_20f,     QMLF_5_PARSER_SOFT_VALID_20f,
                             QMLF_5_PARSER_SOFT_KEY_21f,    QMLF_5_PARSER_SOFT_MASK_21f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_21f,     QMLF_5_PARSER_SOFT_VALID_21f,
                             QMLF_5_PARSER_SOFT_KEY_22f,    QMLF_5_PARSER_SOFT_MASK_22f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_22f,     QMLF_5_PARSER_SOFT_VALID_22f,
                             QMLF_5_PARSER_SOFT_KEY_23f,    QMLF_5_PARSER_SOFT_MASK_23f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_23f,     QMLF_5_PARSER_SOFT_VALID_23f,
                             QMLF_5_PARSER_SOFT_KEY_24f,    QMLF_5_PARSER_SOFT_MASK_24f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_24f,     QMLF_5_PARSER_SOFT_VALID_24f,
                             QMLF_5_PARSER_SOFT_KEY_25f,    QMLF_5_PARSER_SOFT_MASK_25f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_25f,     QMLF_5_PARSER_SOFT_VALID_25f,
                             QMLF_5_PARSER_SOFT_KEY_26f,    QMLF_5_PARSER_SOFT_MASK_26f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_26f,     QMLF_5_PARSER_SOFT_VALID_26f,
                             QMLF_5_PARSER_SOFT_KEY_27f,    QMLF_5_PARSER_SOFT_MASK_27f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_27f,     QMLF_5_PARSER_SOFT_VALID_27f,
                             QMLF_5_PARSER_SOFT_KEY_28f,    QMLF_5_PARSER_SOFT_MASK_28f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_28f,     QMLF_5_PARSER_SOFT_VALID_28f,
                             QMLF_5_PARSER_SOFT_KEY_29f,    QMLF_5_PARSER_SOFT_MASK_29f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_29f,     QMLF_5_PARSER_SOFT_VALID_29f,
                             QMLF_5_PARSER_SOFT_KEY_30f,    QMLF_5_PARSER_SOFT_MASK_30f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_30f,     QMLF_5_PARSER_SOFT_VALID_30f,
                             QMLF_5_PARSER_SOFT_KEY_31f,    QMLF_5_PARSER_SOFT_MASK_31f,    QMLF_5_PARSER_SOFT_PRIORITY_RESULT_31f,     QMLF_5_PARSER_SOFT_VALID_31f };

    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));

    tcam_reg_index = key_index / tcam_entries_per_reg; 
    first_field_index_in_arr = key_index*fields_per_tcam_entry;

    if (first_phy < nof_lanes_nbi) 
    {
        reg_port = REG_PORT_ANY;
        regs[0] = NBIH_QMLF_5_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIH_QMLF_5_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIH_QMLF_5_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIH_QMLF_5_PARSER_SOFT_TCAM_24_TO_31r;
    }
    else 
    {
        reg_port = (first_phy < 2*nof_lanes_nbi)? 0 : 1;
        regs[0] = NBIL_QMLF_5_PARSER_SOFT_TCAM_0_TO_7r;
        regs[1] = NBIL_QMLF_5_PARSER_SOFT_TCAM_8_TO_15r;
        regs[2] = NBIL_QMLF_5_PARSER_SOFT_TCAM_16_TO_23r;
        regs[3] = NBIL_QMLF_5_PARSER_SOFT_TCAM_24_TO_31r;
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    if (to_set)
    {
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr], tcam_entry->key);
        soc_reg_above_64_field64_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1], tcam_entry->mask);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2], tcam_entry->priority);
        soc_reg_above_64_field32_set(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3], tcam_entry->valid);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, regs[tcam_reg_index], reg_port, 0, reg_above64_val));
    }
    else 
    {
        tcam_entry->key = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr]);
        tcam_entry->mask = soc_reg_above_64_field64_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+1]);
        tcam_entry->priority = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+2]);
        tcam_entry->valid = soc_reg_above_64_field32_get(unit, regs[tcam_reg_index], reg_above64_val, fields[first_field_index_in_arr+3]);
    }
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_jer_plus_soft_stage_key_entry_qmlf_set_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry, int to_set)
{
    int first_phy=-1;
    uint32 qmlf_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_plus_first_phy_index_get(unit, port, &first_phy));
    SOCDNX_IF_ERR_EXIT(soc_jer_plus_qmlf_index_get(unit, port, first_phy, &qmlf_index));

    switch (qmlf_index)
    {
        case 0:
            SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf0_set_get(unit, port, key_index, tcam_entry, to_set));
            break;
        case 1:
            SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf1_set_get(unit, port, key_index, tcam_entry, to_set));
            break;
        case 2:
            SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf2_set_get(unit, port, key_index, tcam_entry, to_set));
            break;
        case 3:
            SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf3_set_get(unit, port, key_index, tcam_entry, to_set));
            break;
        case 4:
            SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf4_set_get(unit, port, key_index, tcam_entry, to_set));
            break;
        case 5:
            SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf5_set_get(unit, port, key_index, tcam_entry, to_set));
            break;
        default:
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_soft_stage_key_entry_set(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_ilkn_set_get(unit, port, key_index, tcam_entry, 1));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf_set_get(unit, port, key_index, tcam_entry, 1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_plus_soft_stage_key_entry_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit, port))
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_ilkn_set_get(unit, port, key_index, tcam_entry, 0));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_plus_soft_stage_key_entry_qmlf_set_get(unit, port, key_index, tcam_entry, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_plus_prd_drop_count_get(int unit, int port, uint64 *count)
{
    int phy, first_phy=-1;
    uint32 offset, num_lanes;
    uint32 nof_lanes_nbi, lane;
    soc_pbmp_t nif_ports, phys;
    SOCDNX_INIT_FUNC_DEFS;

    nof_lanes_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &nif_ports));
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove_pbmp(unit, &nif_ports, &phys));

    SOC_PBMP_ITER(phys, phy) {
        first_phy = (phy-1);
        break;
    }

    if (IS_IL_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
    }

    if (first_phy < nof_lanes_nbi)  
    {
        if (IS_IL_PORT(unit, port)) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_PKT_DROP_CNT_HRFr(unit, (offset & 1) , count));
        } else {
            lane = first_phy; 
            SOCDNX_IF_ERR_EXIT(READ_NBIH_PRD_PKT_DROP_CNT_PORTr(unit, lane, count));
        }
    }
    else if ( first_phy < 2*nof_lanes_nbi )  
    {
        if (IS_IL_PORT(unit, port)) {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_PKT_DROP_CNT_HRFr(unit, 0, (offset & 1) , count));
        } else {
            lane = first_phy - nof_lanes_nbi;
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_PKT_DROP_CNT_PORTr(unit, 0, lane, count));
        }
    }
    else 
    {
        if (IS_IL_PORT(unit, port)) {
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_PKT_DROP_CNT_HRFr(unit, 1, (offset & 1) , count));
        } else {
            lane = first_phy - 2*nof_lanes_nbi;
            SOCDNX_IF_ERR_EXIT(READ_NBIL_PRD_PKT_DROP_CNT_PORTr(unit, 1, lane, count));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

