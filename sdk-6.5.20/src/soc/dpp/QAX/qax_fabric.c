#include <soc/mcm/memregs.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/register.h>

#include <soc/dcmn/error.h>
#include <soc/sand/sand_mem.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/QAX/qax_fabric.h>
#include <soc/dpp/QAX/qax_init.h>
#include <soc/dpp/QAX/qax_mgmt.h>
























soc_error_t
  soc_qax_fabric_force_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN soc_dpp_fabric_force_t        force
  )
{
    uint32  reg32_val;
    uint8 is_traffic_enabled;
    uint8 is_ctrl_cells_enabled;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_SAND_IF_ERR_EXIT(jer_mgmt_enable_traffic_get(unit, &is_traffic_enabled));
    SOC_SAND_IF_ERR_EXIT(arad_mgmt_all_ctrl_cells_enable_get(unit, &is_ctrl_cells_enabled));

    if (is_traffic_enabled || is_ctrl_cells_enabled) {
        LOG_WARN(BSL_LS_SOC_FABRIC, (BSL_META_U(unit,
                                " Warning: fabric force should not be done when traffic is enabled.\nTo disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.\n")));
    }

    if (force == socDppFabricForceFabric) {

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0x1); 
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0x0); 
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

        
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
            soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x1); 
            SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
        }
    } else if (force == socDppFabricForceLocal) {

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0x1); 
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0x0); 
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

        
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
            soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0); 
            SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
        }
    } else if (force == socDppFabricForceRestore) {

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0x0); 
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0x0); 
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

        
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
            soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0); 
            SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
        }
    } else {

        cli_out("Option not supported\n");
        cli_out("Supported options are fabric, local, default\n");
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);

    }


exit:
  SOCDNX_FUNC_RETURN;
}




soc_error_t
soc_qax_fabric_multicast_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count,
    SOC_SAND_IN  soc_module_t                   *destid_array
  )
{
    int index, offset, i;
    uint32 mc_rep = 0;
    soc_module_t local_dest;
    uint32 my_core0_fap_id;
    SHR_BITDCLNAME (table_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    index = mc_id / SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE;
    offset = (mc_id % SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE) * SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH;

    SOCDNX_IF_ERR_EXIT(qax_mgmt_system_fap_id_get(unit, &my_core0_fap_id));

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));
    for (i = 0; i < destid_count; ++i) {
        if (destid_array[i] == my_core0_fap_id){
            mc_rep |= SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT;
        } else { 
            local_dest = SOC_DPP_FABRIC_GROUP_MODID_UNSET(destid_array[i]);
            if (local_dest == 0) { 
                mc_rep |= SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT ;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("dest %d is invalid"),destid_array[i]));
            }

        }
    }
    SHR_BITCOPY_RANGE(table_data, offset, &mc_rep, 0, SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));

exit:
  SOCDNX_FUNC_RETURN;
}




soc_error_t
  soc_qax_fabric_link_config_ovrd(
    int                unit
  )
{
    char* fabric_connect_mode = NULL;
    int is_mesh;
    int is_mesh_hybrid = 0;
    int nof_fabric_links, nof_fabric_pms;

    SOCDNX_INIT_FUNC_DEFS;

    
    fabric_connect_mode = soc_property_get_str(unit, spn_FABRIC_CONNECT_MODE);
    if (fabric_connect_mode == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Property fabric_connect_mode must be defined")));
    }
    is_mesh = (sal_strcmp(fabric_connect_mode, "MESH") == 0);

    if (SOC_IS_KALIA(unit) || is_mesh || soc_feature(unit, soc_feature_packet_tdm_marking)) {
        if (soc_feature(unit, soc_feature_packet_tdm_marking))
        {
            nof_fabric_links = 0;
        } else if (SOC_IS_KALIA(unit))
        {
            nof_fabric_links = 16;
        } else 
        {
            
            nof_fabric_pms = SOC_QAX_PM_FABRIC;

            is_mesh_hybrid = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_enable", 0);
            if (is_mesh_hybrid) {
                nof_fabric_pms = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_nof_fabric_quads", SOC_QAX_MESH_HYBRID_PM_FABRIC);
            }

            
            nof_fabric_links = nof_fabric_pms * SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        }
        SOC_DPP_DEFS_SET(unit, nof_fabric_links, nof_fabric_links);

        SOC_DPP_DEFS_SET(unit, nof_instances_fmac, (is_mesh_hybrid? nof_fabric_pms : 4));
        SOC_DPP_DEFS_SET(unit, first_fabric_link_id, 0);
        SOC_DPP_DEFS_SET(unit, nof_sch_active_links, (is_mesh_hybrid? nof_fabric_links : 16));
        SOC_DPP_DEFS_SET(unit, nof_logical_ports, (is_mesh_hybrid? (512 +  nof_fabric_links) : 528));

        SOC_DPP_IMP_DEFS_SET(unit, rci_severity_factor_1, 5);
        SOC_DPP_IMP_DEFS_SET(unit, rci_severity_factor_2, 6);
        SOC_DPP_IMP_DEFS_SET(unit, rci_severity_factor_3, 7);
        SOC_DPP_IMP_DEFS_SET(unit, rci_threshold_nof_congested_links, is_mesh_hybrid? (nof_fabric_links / 2 + 1)  : 8);
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_set(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_IN int                                   enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOC_MSG("QUX doesn't support this function!")));
    }
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MESHMC_FC_ENr(unit, &reg_val));
        soc_reg_field_set(unit, FDA_FDA_MESHMC_FC_ENr, &reg_val, MESHMC_FC_ENf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_MESHMC_FC_ENr(unit, reg_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("backwards flow control for cosq egress fabric ucast / mcast / tdm fifos are not supported in QAX\n")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_get(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_OUT int                                  *enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOC_MSG("QUX doesn't support this function!")));
    }
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MESHMC_FC_ENr(unit, &reg_val));
        *enable = soc_reg_field_get(unit, FDA_FDA_MESHMC_FC_ENr, reg_val, MESHMC_FC_ENf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("backwards flow control for cosq egress fabric ucast / mcast / tdm fifos are not supported in QAX\n")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
    soc_qax_update_params_based_on_FDA_fifos_weight (
      SOC_SAND_IN  int                                unit)
{
    uint32 nof_pipes, pipe, min_pipe_weight, min_pipe_weight_tmp; 
    uint32 FIELD_4_22_len, FIELD_4_22_max_val = 0;
    
    soc_reg_above_64_val_t reg_above_64_val;
    const soc_field_t fields[3] = {EGQ_WFQ_WEIGHT_FABFIF_0f, EGQ_WFQ_WEIGHT_FABFIF_1f, EGQ_WFQ_WEIGHT_FABFIF_2f};
    uint32 fields_vals[3];
    
    const soc_reg_t regs_0100[3] = {FDR_REG_0100_1r, FDR_REG_0100_2r, FDR_REG_0100_3r};
    const soc_reg_t regs_011D[3] = {FDR_REG_011D_1r, FDR_REG_011D_2r, FDR_REG_011D_3r};
    uint32 reg_val;
    uint32 field_val;

    SOCDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.nof_pipes;

    
    SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, reg_above_64_val));
    for (pipe = 0; pipe < nof_pipes; ++pipe) {
        fields_vals[pipe] = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, reg_above_64_val, fields[pipe]);
    }

    switch (nof_pipes) {
    case 1:
        min_pipe_weight = fields_vals[0];
        break;
    case 2:
        min_pipe_weight = SOC_SAND_MIN(fields_vals[0], fields_vals[1]);
        break;
    case 3:
        min_pipe_weight_tmp = SOC_SAND_MIN(fields_vals[0], fields_vals[1]);
        min_pipe_weight = SOC_SAND_MIN(min_pipe_weight_tmp, fields_vals[2]);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("number of pipes %d in invalid"), nof_pipes));
    }

    
    FIELD_4_22_len = soc_reg_field_length(unit, FDR_REG_011D_1r, FIELD_4_22f);
    SHR_BITSET_RANGE(&FIELD_4_22_max_val, 0, FIELD_4_22_len);

    for (pipe = 0; pipe < nof_pipes; ++pipe) {
        

        field_val = 0x800 * (fields_vals[pipe] / min_pipe_weight);
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs_0100[pipe], REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, regs_0100[pipe], &reg_val, FIELD_0_18f, field_val);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs_0100[pipe], REG_PORT_ANY, 0, reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, regs_011D[pipe], REG_PORT_ANY, 0, &reg_val));
        field_val = soc_reg_field_get(unit, regs_011D[pipe], reg_val, FIELD_0_0f);
        if (field_val) { 
            field_val = 0x1800 * (fields_vals[pipe] / min_pipe_weight);
            if (field_val > FIELD_4_22_max_val) { 
                field_val = FIELD_4_22_max_val;
            }
            soc_reg_field_set(unit, regs_011D[pipe], &reg_val, FIELD_4_22f, field_val);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, regs_011D[pipe], REG_PORT_ANY, 0, reg_val));
        }
    }

    exit:
      SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type)
{
    soc_reg_above_64_val_t above_64_reg_val;
    const soc_field_t fields[3] = {EGQ_WFQ_WEIGHT_FABFIF_0f, EGQ_WFQ_WEIGHT_FABFIF_1f, EGQ_WFQ_WEIGHT_FABFIF_2f};

    SOCDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOC_MSG("QUX doesn't support this function!")));
    }
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_generic_pipe:
        if ((pipe < 0) || (pipe >= (sizeof (fields)/sizeof(soc_field_t)))){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Generic pipe must be between 0 and %d (including)\n"), (sizeof (fields)/sizeof(soc_field_t)-1)));
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, fields[pipe], weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_WFQ_WEIGHT_MESHMCf, weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type
  )
{
    soc_reg_above_64_val_t above_64_reg_val;
    const soc_field_t fields[3] = {EGQ_WFQ_WEIGHT_FABFIF_0f, EGQ_WFQ_WEIGHT_FABFIF_1f, EGQ_WFQ_WEIGHT_FABFIF_2f};

    SOCDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOC_MSG("QUX doesn't support this function!")));
    }
    SOCDNX_NULL_CHECK(weight);
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_generic_pipe:
        if ((pipe < 0) || (pipe >= (sizeof (fields)/sizeof(soc_field_t)))){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Generic pipe must be between 0 and %d (including)\n"), (sizeof (fields)/sizeof(soc_field_t)-1)));
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, fields[pipe]);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_WFQ_WEIGHT_MESHMCf);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_qax_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  )
{

    soc_reg_above_64_val_t above_64_reg_val;
    soc_field_t field;

    SOCDNX_INIT_FUNC_DEFS;

    if (fabric_pipe_type != soc_dpp_cosq_gport_fabric_pipe_egress) { 
        
        switch (pipe) {
        case 0:
            field = IPT_0_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_0_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_0_WFQ_CTX_2_WEIGHTf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d\n"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_0_WFQ_CONFIGr(unit, above_64_reg_val));
        soc_reg_above_64_field32_set(unit, FDT_IPT_0_WFQ_CONFIGr, above_64_reg_val, field, weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_0_WFQ_CONFIGr(unit, above_64_reg_val));
    }else{
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric_pipe_egress is not legal argument for QAX\n")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int*                               weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  )
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t field;

    SOCDNX_INIT_FUNC_DEFS;

    if (weight == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid address %#x\n"), unit ,weight));
    }

    if (fabric_pipe_type != soc_dpp_cosq_gport_fabric_pipe_egress) { 
        switch (pipe) {
        case 0:
            field = IPT_0_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_0_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_0_WFQ_CTX_2_WEIGHTf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d\n"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_0_WFQ_CONFIGr(unit, reg_above_64_val));
        *weight = soc_reg_above_64_field32_get(unit, FDT_IPT_0_WFQ_CONFIGr, reg_above_64_val, field);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric_pipe_egress is not legal argument for QAX\n")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    int i;
    soc_field_t field;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOC_MSG("QUX doesn't support this function!")));
    }
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        if (threshold->dp == 0) {
            field = EGQ_MESHMCFIF_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_MESHMCFIF_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_MESHMCFIF_PRIO_2_DROP_THRf;
        }
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr, &reg_val, field, threshold->value);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, i, reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("priority drop threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast / ucast are not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    soc_field_t field;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                             (_BSL_SOC_MSG("QUX doesn't support this function!")));
    }
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        if (threshold->dp == 0) {
            field = EGQ_MESHMCFIF_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_MESHMCFIF_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_MESHMCFIF_PRIO_2_DROP_THRf;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, 0, &reg_val));
        threshold->value = soc_reg_field_get(unit, FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr, reg_val, field);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("priority drop threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast / ucast are not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }
exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{

    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_ucast:
        SOCDNX_IF_ERR_EXIT(READ_TXQ_LOCAL_FIFO_CFGr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, reg_above_64_val, SRAM_DTQ_LOC_GEN_RCI_THf, threshold_val);
        SOCDNX_IF_ERR_EXIT(WRITE_TXQ_LOCAL_FIFO_CFGr(unit, reg_above_64_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("rci threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast is not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT  int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{

    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_ucast:
        SOCDNX_IF_ERR_EXIT(READ_TXQ_LOCAL_FIFO_CFGr(unit, reg_above_64_val));
        *threshold_val = soc_reg_above_64_field32_get(unit, TXQ_LOCAL_FIFO_CFGr, reg_above_64_val, SRAM_DTQ_LOC_GEN_RCI_THf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("rci threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast is not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
soc_qax_fabric_priority_bits_mapping_to_fdt_index_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     tc,
    SOC_SAND_IN  uint32                     dp,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_OUT uint32                     *index
  )
{
    uint32 is_mc  = 0, is_hp = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *index = 0;

    is_hp  = (flags & SOC_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0 ;
    is_mc  = (flags & SOC_FABRIC_PRIORITY_MULTICAST)       ? 1 : 0 ;

    *index  |=  ((is_hp     << SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_OFFSET)  & SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_MASK  )|
                ((tc        << SOC_QAX_FABRIC_PRIORITY_NDX_TC_OFFSET)     & SOC_QAX_FABRIC_PRIORITY_NDX_TC_MASK     )|
                ((dp        << SOC_QAX_FABRIC_PRIORITY_NDX_DP_OFFSET)     & SOC_QAX_FABRIC_PRIORITY_NDX_DP_MASK     )|
                ((is_mc     << SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_OFFSET)  & SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_MASK  );

    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_qax_fabric_priority_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_IN  int                fabric_priority
  )
{
    uint32 index;
    uint32 tdm_min_prio;
    SOCDNX_INIT_FUNC_DEFS;

    tdm_min_prio = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min;
    
    if ((fabric_priority < 0) || ((fabric_priority >= tdm_min_prio) && !soc_feature(unit, soc_feature_packet_tdm_marking))){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric PKT priority is %d but must be between 0 and TDM min priority (that was set to to %d) \n"), fabric_priority, tdm_min_prio));
    }
    
    SOCDNX_IF_ERR_EXIT(soc_qax_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index));
    
    SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm,
                                                          0, 0, MEM_BLOCK_ALL, index, index, &fabric_priority));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_fabric_priority_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_OUT  int                *fabric_priority
  )
{
    uint32 index, val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_qax_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index));
    
    SOCDNX_IF_ERR_EXIT(READ_TXQ_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ANY, index, &val));
    *fabric_priority = val;

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  soc_qax_fabric_queues_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_OUT soc_dpp_fabric_queues_info_t    *queues_info
  )
{
    uint32 reg32_val, dtq_mode;
    uint64 reg64_val;
    soc_reg_above_64_val_t  reg_above64_val;
    int i, num_of_DTQs;

    SOCDNX_INIT_FUNC_DEFS;

    

    
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP 
         && !soc_feature(unit, soc_feature_packet_tdm_marking)) { 
        num_of_DTQs = 0;
    } else if (SOC_DPP_IS_MESH(unit)){
        num_of_DTQs = 6; 
    } else {
        SOCDNX_IF_ERR_EXIT(READ_TXQ_TXQ_GENERAL_CONFIGURATIONr(unit, &reg32_val));
        dtq_mode = soc_reg_field_get(unit, TXQ_TXQ_GENERAL_CONFIGURATIONr, reg32_val, DTQ_MODEf);
        num_of_DTQs = (dtq_mode + 1)*2; 
    }

    
    if(num_of_DTQs > SOC_DPP_MAX_NUM_OF_FABRIC_DTQS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("error in num_of_DTQs calculation or SOC_DPP_MAX_NUM_OF_FABRIC_DTQS macro")));
    }

    for (i = 0; i < num_of_DTQs; ++i) {
        if (SOC_DPP_IS_MESH(unit) && ((i == 2) || (i == 3))) { 
            continue;
        }
        SOCDNX_IF_ERR_EXIT(READ_TXQ_DTQ_STATUSr(unit, i, &reg64_val));
        queues_info->soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_occ_val = soc_reg64_field32_get(unit, TXQ_DTQ_STATUSr, reg64_val, DTQ_N_WORD_CNTf);
        queues_info->soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_min_occ_val = soc_reg64_field32_get(unit, TXQ_DTQ_STATUSr, reg64_val, DTQ_N_WORD_CNT_MINf);
        queues_info->soc_dpp_fabric_dtq_info[i].soc_dpp_dtq_max_occ_val = soc_reg64_field32_get(unit, TXQ_DTQ_STATUSr, reg64_val, DTQ_N_WORD_CNT_MAXf);
        queues_info->soc_dpp_fabric_dtq_info[i].is_valid = 1;
    }

    
    if (SOC_REG_NUMELS(unit, TXQ_LTQ_STATUSr) > SOC_DPP_MAX_NUM_OF_LOCAL_DTQS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("error in SOC_DPP_MAX_NUM_OF_LOCAL_DTQS define")));
    }

    
    for (i = 0; i < SOC_REG_NUMELS(unit, TXQ_LTQ_STATUSr); ++i) {

        SOCDNX_IF_ERR_EXIT(READ_TXQ_DTQ_STATUSr(unit, i, &reg64_val));
        queues_info->soc_dpp_local_dtq_info[i].soc_dpp_dtq_occ_val = soc_reg64_field32_get(unit, TXQ_LTQ_STATUSr, reg64_val, LTQ_N_WORD_CNTf);
        queues_info->soc_dpp_local_dtq_info[i].soc_dpp_dtq_min_occ_val = soc_reg64_field32_get(unit, TXQ_LTQ_STATUSr, reg64_val, LTQ_N_WORD_CNT_MINf);
        queues_info->soc_dpp_local_dtq_info[i].soc_dpp_dtq_max_occ_val = soc_reg64_field32_get(unit, TXQ_LTQ_STATUSr, reg64_val, LTQ_N_WORD_CNT_MAXf);
        queues_info->soc_dpp_local_dtq_info[i].is_valid = 1;
    }

    

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, PTS_FIFOS_MAX_OCC_0r, REG_PORT_ANY, 0, reg_above64_val));

    queues_info->soc_dpp_fabric_sram_dqcf_max_occ_val.info = soc_reg_above_64_field32_get(unit, PTS_FIFOS_MAX_OCC_0r, reg_above64_val, SRAM_DQCF_MAX_OCf);
    queues_info->soc_dpp_fabric_sram_dqcf_max_occ_ctx.info = soc_reg_above_64_field32_get(unit, PTS_FIFOS_MAX_OCC_0r, reg_above64_val, SRAM_DQCQ_MAX_OC_CNTXf);
    queues_info->soc_dpp_fabric_dram_dblf_max_occ_val.info = soc_reg_above_64_field32_get(unit, PTS_FIFOS_MAX_OCC_0r, reg_above64_val, DRAM_DBLF_MAX_OCf);
    queues_info->soc_dpp_fabric_dram_dblf_max_occ_ctx.info = soc_reg_above_64_field32_get(unit, PTS_FIFOS_MAX_OCC_0r, reg_above64_val, DRAM_DBLF_MAX_OC_CNTXf);

    queues_info->soc_dpp_fabric_sram_dqcf_max_occ_val.is_valid = 1;
    queues_info->soc_dpp_fabric_sram_dqcf_max_occ_ctx.is_valid = 1;
    queues_info->soc_dpp_fabric_dram_dblf_max_occ_val.is_valid = 1;
    queues_info->soc_dpp_fabric_dram_dblf_max_occ_ctx.is_valid = 1;

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  qax_fabric_pcp_dest_mode_config_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_IN uint32           pcp_mode
  )
{
    SHR_BITDCLNAME (fdt_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, modid, fdt_data));
    SHR_BITCOPY_RANGE(fdt_data, 6, &pcp_mode, 0, 2);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ALL, modid, fdt_data));


exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_qax_brdc_fsrd_blk_id_set(int unit){

    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>
